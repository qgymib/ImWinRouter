#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <netioapi.h>
#include <iphlpapi.h>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "utils/memory.hpp"
#include "ip.hpp"

iwr::IpInterfaceVec iwr::GetIpInterfaceVec()
{
    IpInterfaceVec result;

    iwr::Pointer<MIB_IPINTERFACE_TABLE> pipTable(FreeMibTable);
    DWORD ret = GetIpInterfaceTable(AF_UNSPEC, &pipTable);
    if (ret != NO_ERROR)
    {
        throw std::runtime_error("GetIpInterfaceTable failed");
    }

    for (size_t i = 0; i < pipTable->NumEntries; i++)
    {
        const MIB_IPINTERFACE_ROW* entry = &pipTable->Table[i];
        IpInterface                item = {
            entry->Family,
            entry->InterfaceLuid.Value,
            entry->InterfaceIndex,
            entry->LinkLocalAddressBehavior,
            static_cast<uint32_t>(entry->Metric),
            static_cast<bool>(entry->Connected),
            static_cast<bool>(entry->DisableDefaultRoutes),
        };
        result.push_back(item);
    }

    return result;
}

iwr::IpForwardVec iwr::GetIpForwardVec()
{
    IpForwardVec result;

    iwr::Pointer<MIB_IPFORWARD_TABLE2> pIpForwardTable(FreeMibTable);
    DWORD ret = GetIpForwardTable2(AF_UNSPEC, &pIpForwardTable);
    if (ret != NO_ERROR)
    {
        throw std::runtime_error("GetIpForwardTable2 failed");
    }

    char dest[INET6_ADDRSTRLEN], gateway[INET6_ADDRSTRLEN];
    for (size_t i = 0; i < pIpForwardTable->NumEntries; i++)
    {
        const MIB_IPFORWARD_ROW2* info = &pIpForwardTable->Table[i];
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
            static_cast<bool>(info->Loopback),
            static_cast<bool>(info->AutoconfigureAddress),
            info->Origin,
        };
        result.push_back(item);
    }

    return result;
}
