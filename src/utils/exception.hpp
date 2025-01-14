#ifndef UTILS_EXCEPTION_HPP
#define UTILS_EXCEPTION_HPP

#include <windows.h>
#include <exception>
#include <utils/string.hpp>

namespace iwr
{

class Win32Error : public std::exception
{
public:
    Win32Error(DWORD errorCode, const std::string& msg = "");

public:
    const char* what() const noexcept override;

private:
    std::string message;
};

} // namespace iwr

#endif // UTILS_EXCEPTION_HPP
