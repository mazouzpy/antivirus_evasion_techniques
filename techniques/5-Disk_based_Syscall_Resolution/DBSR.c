#include <windows.h>
#include <stdio.h>

DWORD get_syscall_id_from_disk(const char *func_name)
{
    //Open the clean ntdll.dll file from the system directory
    HANDLE hFile = CreateFileA("C:\\Windows\\System32\\ntdll.dll", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return 0;

    //Map the file into memory to read its structure
    DWORD fileSize = GetFileSize(hFile, NULL);
    LPVOID pBuffer = VirtualAlloc(NULL, fileSize, MEM_COMMIT, PAGE_READWRITE);
    ReadFile(hFile, pBuffer, fileSize, NULL, NULL);
    CloseHandle(hFile);

    //Parse PE headers to locate the Export Directory
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pBuffer;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((BYTE *)pBuffer + dos->e_lfanew);
    DWORD export_rva = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    PIMAGE_EXPORT_DIRECTORY exp = (PIMAGE_EXPORT_DIRECTORY)((BYTE *)pBuffer + export_rva);

    DWORD *names = (DWORD *)((BYTE *)pBuffer + exp->AddressOfNames);
    DWORD *funcs = (DWORD *)((BYTE *)pBuffer + exp->AddressOfFunctions);
    WORD *ords = (WORD *)((BYTE *)pBuffer + exp->AddressOfNameOrdinals);

    DWORD syscall_id = 0;

    //Iterate through exported functions to find the target syscall
    for (DWORD i = 0; i < exp->NumberOfNames; i++)
    {
        char *name = (char *)((BYTE *)pBuffer + names[i]);
        if (strcmp(name, func_name) == 0)
        {
            BYTE *func_ptr = (BYTE *)pBuffer + funcs[ords[i]];

            //Locate the syscall instruction pattern (mov eax, ID)
            //Pattern: 4C 8B D1 B8 [ID] ...
            if (func_ptr[0] == 0x4C && func_ptr[1] == 0x8B && func_ptr[2] == 0xD1 && func_ptr[3] == 0xB8)
            {
                syscall_id = *(DWORD *)(func_ptr + 4);
            }
            break;
        }
    }

    //Cleanup memory before returning the ID
    VirtualFree(pBuffer, 0, MEM_RELEASE);
    return syscall_id;
}
// im not gonna lie i use ia on this ,it a hard method