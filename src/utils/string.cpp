#include <windows.h>
#include <cassert>
#include "string.hpp"

#include <cstdint>

std::string iwr::ToString(const wchar_t* src)
{
    std::string ret;

    int target_len =
        WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
    if (target_len == 0)
    {
        return ret;
    }

    char* buf = (char*)malloc(target_len);
    if (buf == NULL)
    {
        return ret;
    }

    int r =
        WideCharToMultiByte(CP_UTF8, 0, src, -1, buf, target_len, NULL, NULL);
    assert(r == target_len);
    (void)r;

    ret = buf;
    free(buf);

    return ret;
}

std::string iwr::ToString(const std::wstring& src)
{
    return iwr::ToString(src.c_str());
}

std::wstring iwr::ToWideString(const char* src)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, src, -1, nullptr, 0);
    if (size_needed == 0)
    {
        return nullptr;
    }

    std::wstring result(size_needed - 1, 0);

    int chars_converted =
        MultiByteToWideChar(CP_UTF8, 0, src, -1, &result[0], size_needed);
    assert(chars_converted == size_needed);
    (void)chars_converted;

    return result;
}

std::wstring iwr::ToWideString(const std::string& src)
{
    return iwr::ToWideString(src.c_str());
}

std::string iwr::ToString(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    va_list ap2;
    va_copy(ap2, ap);

    int len = vsnprintf(nullptr, 0, fmt, ap2);
    va_end(ap2);

    std::string result(len + 1, '\0');
    vsnprintf(&result[0], len + 1, fmt, ap);
    va_end(ap);

    return result;
}

std::string iwr::hex_dump_type(const void* data, size_t nb, size_t block_sz,
                               const char* delimiter)
{
    std::string    result;
    const uint8_t* basis = static_cast<const uint8_t*>(data);

    for (size_t idx = 0; idx < nb; idx++)
    {
        for (size_t i = 0; i < block_sz; i++)
        {
            char buff[8];
            snprintf(buff, sizeof(buff), "%.2X", *basis);

            basis++;
            result += buff;
        }

        if (idx != nb - 1)
        {
            result += delimiter;
        }
    }

    return result;
}
