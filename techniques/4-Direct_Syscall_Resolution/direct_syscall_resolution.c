#include <windows.h>

extern NTSTATUS NtAllocateVirtualMemmory(
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect);

int main(){
    PVOID base = NULL;
    SIZE_T size = 1024;


    //direct call
    NtAllocateVirtualMemmory((HANDLE)-1,PAGE_EXECUTE_READWRITE);
    return 0;
}