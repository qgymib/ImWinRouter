#include "utils/string.hpp"
#include "exception.hpp"

iwr::Win32Error::Win32Error(DWORD errorCode, const std::string& msg)
{
    const DWORD dwFlags =
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
    WCHAR* msgBuf = NULL;
    DWORD  dwRet = FormatMessageW(dwFlags, nullptr, errorCode, 0,
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
    this->message += errMsg;
}

const char* iwr::Win32Error::what() const noexcept
{
    return this->message.c_str();
}
