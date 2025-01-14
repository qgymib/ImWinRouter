#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <netioapi.h>
#include <iphlpapi.h>
#include <stdexcept>
#include "utils/memory.hpp"
#include "utils/string.hpp"
#include "ip.hpp"

iwr::IpInterfaceVec iwr::GetIpInterfaceVec()
{
    IpInterfaceVec result;

    MIB_IPINTERFACE_TABLE* pipTable = nullptr;
    DWORD                  ret = GetIpInterfaceTable(AF_UNSPEC, &pipTable);
    if (ret != NO_ERROR)
    {
        throw std::runtime_error("GetIpInterfaceTable failed");
    }

    for (size_t i = 0; i < pipTable->NumEntries; i++)
    {
        MIB_IPINTERFACE_ROW* entry = &pipTable->Table[i];
        IpInterface          item = {
            entry->Family,
            entry->InterfaceLuid.Value,
            entry->InterfaceIndex,
            entry->Metric,
            static_cast<bool>(entry->Connected),
            static_cast<bool>(entry->DisableDefaultRoutes),
        };
        result.push_back(item);
    }

    FreeMibTable(pipTable);

    return result;
}

iwr::IpForwardVec iwr::GetIpForwardVec()
{
    IpForwardVec result;

    MIB_IPFORWARD_TABLE2* pIpForwardTable = nullptr;
    DWORD                 ret = GetIpForwardTable2(AF_UNSPEC, &pIpForwardTable);
    if (ret != NO_ERROR)
    {
        throw std::runtime_error("GetIpForwardTable2 failed");
    }

    char dest[INET6_ADDRSTRLEN], gateway[INET6_ADDRSTRLEN];
    for (size_t i = 0; i < pIpForwardTable->NumEntries; i++)
    {
        MIB_IPFORWARD_ROW2* info = &pIpForwardTable->Table[i];
        if (info->DestinationPrefix.Prefix.si_family == AF_INET)
        {
            inet_ntop(AF_INET, &info->DestinationPrefix.Prefix.Ipv4.sin_addr,
                      dest, sizeof(dest));
            inet_ntop(AF_INET, &info->NextHop.Ipv4.sin_addr, gateway,
                      sizeof(gateway));
        }
        else if (info->DestinationPrefix.Prefix.si_family == AF_INET6)
        {
            inet_ntop(AF_INET6, &info->DestinationPrefix.Prefix.Ipv6.sin6_addr,
                      dest, sizeof(dest));
            inet_ntop(AF_INET6, &info->NextHop.Ipv6.sin6_addr, gateway,
                      sizeof(gateway));
        }
        else
        {
            throw std::runtime_error("Unknown destination prefix");
        }

        IpForward item = {
            info->DestinationPrefix.Prefix.si_family,
            dest,
            info->DestinationPrefix.PrefixLength,
            gateway,
            info->InterfaceLuid.Value,
            info->InterfaceIndex,
            static_cast<uint32_t>(info->Metric),
        };
        result.push_back(item);
    }

    FreeMibTable(pIpForwardTable);
    return result;
}

static std::string s_physical_address_to_string(const BYTE* PhysicalAddress,
                                                DWORD PhysicalAddressLength)
{
    std::string result;

    char buff[8];
    for (DWORD i = 0; i < PhysicalAddressLength; i++)
    {
        const int v = (int)PhysicalAddress[i];
        if (i == (PhysicalAddressLength - 1))
        {
            snprintf(buff, sizeof(buff), "%.2X\n", v);
        }
        else
        {
            snprintf(buff, sizeof(buff), "%.2X-", v);
        }
        result += buff;
    }

    return result;
}

iwr::AdaptersAddressesVec iwr::GetAdaptersAddressesVec()
{
    AdaptersAddressesVec result;

    ULONG                             outBufLen = 16 * 1024;
    iwr::Memory<IP_ADAPTER_ADDRESSES> buff(outBufLen);

    const ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    DWORD       dwRetVal = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr,
                                                buff.data(), &outBufLen);
    if (dwRetVal == ERROR_BUFFER_OVERFLOW)
    {
        buff.resize(outBufLen);
        dwRetVal = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, buff.data(),
                                        &outBufLen);
    }
    if (dwRetVal != ERROR_SUCCESS)
    {
        throw std::runtime_error("GetAdaptersAddresses failed");
    }

    for (const IP_ADAPTER_ADDRESSES* pCurrAddresses = buff.data();
         pCurrAddresses; pCurrAddresses = pCurrAddresses->Next)
    {
        AdaptersAddresses item = {
            pCurrAddresses->AdapterName,
            iwr::wide_to_utf8(pCurrAddresses->FriendlyName),
            iwr::wide_to_utf8(pCurrAddresses->Description),
            s_physical_address_to_string(pCurrAddresses->PhysicalAddress,
                                         pCurrAddresses->PhysicalAddressLength),
            pCurrAddresses->Luid.Value,
            static_cast<bool>(pCurrAddresses->Ipv4Enabled),
            static_cast<bool>(pCurrAddresses->Ipv6Enabled),
            static_cast<uint32_t>(pCurrAddresses->Ipv4Metric),
            static_cast<uint32_t>(pCurrAddresses->Ipv6Metric),
        };
        result.push_back(item);
    }

    return result;
}
