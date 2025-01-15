#include "win32.hpp"

static HWND s_hwnd = nullptr;

void iwr::SetWindowHandle(HWND hwnd)
{
    s_hwnd = hwnd;
}

HWND iwr::GetWindowHandle()
{
    return s_hwnd;
}
