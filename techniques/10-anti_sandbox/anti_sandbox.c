#include <windows.h>

bool is_user_active()
{
    // check physical memory to ensure it is not a limited analysis environment
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    GlobalMemoryStatusEx(&mem);
    if (mem.ullTotalPhys / 1024 / 1024 / 1024 < 4)
        return false;

    // check processor count to ensure sufficient resources are available
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    if (si.dwNumberOfProcessors < 2)
        return false;

    // verify mouse movement to confirm presence of human interaction
    POINT p1, p2;
    GetCursorPos(&p1);
    Sleep(1000);
    GetCursorPos(&p2);
    if (p1.x == p2.x && p1.y == p2.y)
        return false;

    return true;
}
