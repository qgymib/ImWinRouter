#include <sstream>
#include "utils/string.hpp"
#include "exception.hpp"

iwr::Win32Error::Win32Error(DWORD errorCode, const std::string& msg)
{
    const DWORD dwFlags =
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
    WCHAR* msgBuf = NULL;
    DWORD  dwLanguageId = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
    DWORD  dwRet = FormatMessageW(dwFlags, nullptr, errorCode, dwLanguageId,
                                  (LPWSTR)&msgBuf, 0, nullptr);
    if (dwRet == 0)
    {
        DWORD lastError = GetLastError();
        (void)lastError;
        abort();
    }

    std::string errMsg = iwr::wide_to_utf8(msgBuf);
    LocalFree(msgBuf);

    if (!msg.empty())
    {
        this->message = msg + ":";
    }

    std::ostringstream oss;
    oss << errorCode;
    this->message += "ERR(" + oss.str() + "):" + errMsg;
}

const char* iwr::Win32Error::what() const noexcept
{
    return this->message.c_str();
}
