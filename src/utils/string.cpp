#include <windows.h>
#include <cassert>
#include "string.hpp"

std::string iwr::wide_to_utf8(const WCHAR* src)
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

std::string iwr::wide_to_utf8(const std::wstring& src)
{
    return iwr::wide_to_utf8(src.c_str());
}

std::wstring iwr::utf8_to_wide(const char* src)
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

std::wstring iwr::utf8_to_wide(const std::string& src)
{
    return iwr::utf8_to_wide(src.c_str());
}
