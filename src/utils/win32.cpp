#include "win32.hpp"
#include <netioapi.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>

static HWND s_hwnd = nullptr;

void iwr::SetWindowHandle(HWND hwnd)
{
    s_hwnd = hwnd;
}

HWND iwr::GetWindowHandle()
{
    return s_hwnd;
}

std::string iwr::ToString(const SOCKET_ADDRESS* addr)
{
    LPSOCKADDR sockaddr = addr->lpSockaddr;
    if (sockaddr == nullptr)
    {
        return "";
    }

    char buff[64];
    if (sockaddr->sa_family == AF_INET)
    {
        const SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(sockaddr);
        inet_ntop(AF_INET, &ipv4->sin_addr, buff, sizeof(buff));
    }
    else
    {
        const SOCKADDR_IN6* ipv6 = reinterpret_cast<SOCKADDR_IN6*>(sockaddr);
        inet_ntop(AF_INET6, &ipv6->sin6_addr, buff, sizeof(buff));
    }
    return buff;
}

std::string iwr::ToString(const SOCKADDR_INET* addr)
{
    char buff[64];
    if (addr->si_family == AF_INET)
    {
        inet_ntop(AF_INET, &addr->Ipv4.sin_addr, buff, sizeof(buff));
    }
    else
    {
        inet_ntop(AF_INET6, &addr->Ipv6.sin6_addr, buff, sizeof(buff));
    }
    return buff;
}

std::string iwr::ToString(const GUID* guid)
{
    char guid_cstr[64];
    snprintf(guid_cstr, sizeof(guid_cstr),
             "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid->Data1,
             guid->Data2, guid->Data3, guid->Data4[0], guid->Data4[1],
             guid->Data4[2], guid->Data4[3], guid->Data4[4], guid->Data4[5],
             guid->Data4[6], guid->Data4[7]);
    return guid_cstr;
}
