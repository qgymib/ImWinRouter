#include <winsock2.h>
#include <windows.h>
#include <netioapi.h>
#include <stdexcept>
#include "ip.hpp"

iwr::IpInterfaceVec iwr::GetIpInterfaces()
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
