#include <windows.h>
#include <winternl.h>
#include <stdio.h>


//ROR13
DWORD calculate_hash(char *str){
    DWORD hash = 0;
    while (str)
    {
        hash = (hash >> 13) | (hash << 19);
        hash += *str++;
    }
    return hash;
}


//get kernal32.dll addr
PVOID get_kernel32_base(){

    //on 64bit only
    PPEB pPeb = (PPEB)__readgsqword(0x60);

    // on 32bit:
    // PPEB pPeb = (PPEB)__readfsdword(0x30);

    PPEB_LDR_DATA pLdr = pPeb->Ldr;
    PLDR_DATA_TABLE_ENTRY pModule = (PLDR_DATA_TABLE_ENTRY)pLdr->InMemoryOrderModuleList.Flink;
    return (PVOID)pModule->DllBase;
}

//find func by hash
LPVOID find_function(PVOID base, DWORD target_hash){
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
    PIMAGE_NT_HEADERS((BYTE*)base + dos->e_lfanew);
    DWORD export_dir_rva = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    PIMAGE_EXPORT_DIRECTORY exp = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)base + export_dir_rva);

    DWORD *names = (DWORD *)((BYTE *)base + exp->AddressOfNames);
    WORD *ords = (WORD *)((BYTE *)base + exp->AddressOfNameOrdinals);
    DWORD *funcs = (DWORD *)((BYTE *)base + exp->AddressOfFunctions);

    for (DWORD i = 0; i < exp->NumberOfNames; i++)
    {
        char *name = (char *)((BYTE *)base + names[i]);
        if (calculate_hash(name) == target_hash)
        {
            return (LPVOID)((BYTE *)base + funcs[ords[i]]);
        }
    }
    return NULL;
}

int main()
{
    //VirtualAlloc hash is 0x91afca01
    DWORD vAllocHash = 0x91afca01;

    typedef LPVOID(WINAPI * VirtualAlloc_t)(LPVOID, SIZE_T, DWORD, DWORD);

    //search func
    VirtualAlloc_t myVirtualAlloc = (VirtualAlloc_t)find_function(get_kernel32_base(), vAllocHash);

    //check if the process done success
    if (myVirtualAlloc)
    {
        printf("Success! Found VirtualAlloc at: %p\n", myVirtualAlloc);
        LPVOID mem = myVirtualAlloc(NULL, 1024, MEM_COMMIT, PAGE_READWRITE);
        printf("Memory allocated at: %p\n", mem);
    }
    else
    {
        printf("Failed to find function!\n");
    }

    return 0;
}
