#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include <string>

namespace iwr
{

/**
 * @brief Convert wide string to UTF-8 string.
 * @param[in] src   Wide string.
 * @return UTF-8 string.
 */
std::string wide_to_utf8(const wchar_t* src);

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

/**
 * @brief Convert any data into hex string.
 * @see hex_dump().
 * @param[in] data Data to convert.
 * @param[in] nb Number of block.
 * @param[in] block_sz Sizeof block in bytes.
 * @param[in] delimiter Delimiter
 * @return
 */
std::string hex_dump_type(const void* data, size_t nb, size_t block_sz,
                          const char* delimiter);

/**
 * @brief Convert any data into hex string.
 * @param[in] data Data to convert.
 * @param[in] nb Number of block.
 * @param[in] delimiter Delimiter
 * @return
 */
template <typename T>
static std::string hex_dump(const T* data, size_t nb, const char* delimiter)
{
    return hex_dump_type(data, nb, sizeof(T), delimiter);
}

} // namespace iwr

#endif // UTILS_STRING_HPP
