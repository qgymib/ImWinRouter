#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include <Esent.h>
#include <string>

namespace iwr
{

/**
 * @brief Convert wide string to UTF-8 string.
 * @param[in] src   Wide string.
 * @return UTF-8 string.
 */
std::string wide_to_utf8(const WCHAR* src);

/**
 * @brief Convert wide string to UTF-8 string.
 * @param[in] src   Wide string.
 * @return UTF-8 string.
 */
std::string wide_to_utf8(const std::wstring& src);

/**
 * @brief Convert UTF-8 string into wide string.
 * @param[in] src   UTF-8 string.
 * @return Wide string.
 */
std::wstring utf8_to_wide(const char* src);

/**
 * @brief Convert UTF-8 string into wide string.
 * @param[in] src   UTF-8 string.
 * @return Wide string.
 */
std::wstring utf8_to_wide(const std::string& src);

} // namespace iwr

#endif // UTILS_STRING_HPP
