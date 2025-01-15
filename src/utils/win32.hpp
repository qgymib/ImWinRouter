#ifndef UTILS_WIN32_HPP
#define UTILS_WIN32_HPP

#include <windows.h>

namespace iwr
{

/**
 * @brief Set window handle.
 * @param[in] hwnd Window handle.
 */
void SetWindowHandle(HWND hwnd);

/**
 * @brief Get window handle.
 * @return Window handle.
 */
HWND GetWindowHandle();

}

#endif // UTILS_WIN32_HPP
