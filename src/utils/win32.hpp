#ifndef UTILS_WIN32_HPP
#define UTILS_WIN32_HPP

#include <winsock2.h>
#include <windows.h>
#include <netioapi.h>
#include "utils/string.hpp"

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

/**
 * @brief Convert SOCKET_ADDRESS to UTF-8 string.
 * @param[in] addr Socket address.
 * @return UTF-8 string.
 */
std::string ToString(const SOCKET_ADDRESS* addr);

/**
 * @brief Convert SOCKADDR_INET to UTF-8 string.
 * @param[in] addr Socket address.
 * @return UTF-8 string.
 */
std::string ToString(const SOCKADDR_INET* addr);

/**
 * @brief Convert GUID to UTF-8 string.
 * @param[in] guid GUID.
 * @return UTF-8 string.
 */
std::string ToString(const GUID* guid);

}

#endif // UTILS_WIN32_HPP
