#include <winsock2.h>
#include <windows.h>
#include <netioapi.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <imgui.h>
#include <sstream>
#include <nlohmann/json.hpp>
#include "i18n/__init__.h"
#include "utils/exception.hpp"
#include "utils/ip.hpp"
#include "utils/memory.hpp"
#include "utils/title_builder.hpp"
#include "__init__.hpp"
#include "notify.hpp"

typedef struct info_item
{
    const char* name;
    std::string (*get)();
} info_item_t;

typedef struct widget_debug
{
    widget_debug();
    ~widget_debug();

    bool               show_window;
    ImVec2             default_window_size;
    iwr::TitleBuilder* window_title;

    std::string system_interface_data;
    int         system_interface_selected;

    char title[1024];
    char message[4096];
} widget_debug_t;

static widget_debug_t* s_debug = nullptr;

widget_debug::widget_debug()
{
    show_window = false;
    default_window_size = ImVec2(640, 320);
    window_title = new iwr::TitleBuilder("__WIDGET_DEBUG");
    system_interface_selected = 0;
    title[0] = '\0';
    message[0] = '\0';
}

widget_debug::~widget_debug()
{
    delete window_title;
}

static void s_widget_debug_init()
{
    s_debug = new widget_debug_t;
}

static void s_widget_debug_exit()
{
    delete s_debug;
    s_debug = nullptr;
}

static void s_win32_set_clipboard(const std::string& text)
{
    std::wstring wText = iwr::utf8_to_wide(text);

    size_t  wTextBytes = wText.size() * sizeof(wchar_t);
    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, wTextBytes + 2);
    if (hGlob == nullptr)
    {
        throw std::runtime_error("failed to allocate memory for the clipboard");
    }

    void* data = GlobalLock(hGlob);
    memcpy(data, wText.c_str(), wTextBytes + 2);
    GlobalUnlock(hGlob);

    if (!OpenClipboard(nullptr))
    {
        DWORD errorCode = GetLastError();
        GlobalFree(hGlob);
        throw iwr::Win32Error(errorCode);
    }
    if (!SetClipboardData(CF_UNICODETEXT, hGlob))
    {
        DWORD errorCode = GetLastError();
        GlobalFree(hGlob);
        CloseClipboard();
        throw iwr::Win32Error(errorCode);
    }

    if (!CloseClipboard())
    {
        throw iwr::Win32Error(GetLastError());
    }
}

static std::string to_string(const SOCKADDR_INET* addr)
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

static std::string to_string(const SOCKET_ADDRESS* address)
{
    LPSOCKADDR sockaddr = address->lpSockaddr;
    if (sockaddr == nullptr)
    {
        return "";
    }

    char buff[64];
    if (sockaddr->sa_family == AF_INET)
    {
        SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(sockaddr);
        inet_ntop(AF_INET, &ipv4->sin_addr, buff, sizeof(buff));
    }
    else
    {
        SOCKADDR_IN6* ipv6 = reinterpret_cast<SOCKADDR_IN6*>(sockaddr);
        inet_ntop(AF_INET6, &ipv6->sin6_addr, buff, sizeof(buff));
    }
    return buff;
}

static std::string to_string(const GUID* guid)
{
    char guid_cstr[64];
    snprintf(guid_cstr, sizeof(guid_cstr),
             "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", guid->Data1,
             guid->Data2, guid->Data3, guid->Data4[0], guid->Data4[1],
             guid->Data4[2], guid->Data4[3], guid->Data4[4], guid->Data4[5],
             guid->Data4[6], guid->Data4[7]);
    return guid_cstr;
}

template <typename T>
static std::string to_string(const T* data, size_t size, const char* delimiter)
{
    std::string result;

    for (size_t i = 0; i < size; i++)
    {
        const T& d = data[i];

        std::ostringstream oss;
        oss << d;
        result += oss.str();

        if (i + 1 < size)
        {
            result += delimiter;
        }
    }

    return result;
}

static std::string s_widget_debug_ip_interface()
{
    iwr::Pointer<MIB_IPINTERFACE_TABLE> pipTable(FreeMibTable);
    DWORD ret = GetIpInterfaceTable(AF_UNSPEC, &pipTable);
    if (ret != NO_ERROR)
    {
        throw std::runtime_error("GetIpInterfaceTable failed");
    }

    nlohmann::json json = nlohmann::json::array();
    for (size_t i = 0; i < pipTable->NumEntries; i++)
    {
        nlohmann::json             item;
        const MIB_IPINTERFACE_ROW* entry = &pipTable->Table[i];

        item["Family"] = entry->Family;
        item["InterfaceLuid"] = entry->InterfaceLuid.Value;
        item["InterfaceIndex"] = entry->InterfaceIndex;
        item["MaxReassemblySize"] = entry->MaxReassemblySize;
        item["InterfaceIdentifier"] = entry->InterfaceIdentifier;
        item["MinRouterAdvertisementInterval"] =
            entry->MinRouterAdvertisementInterval;
        item["MaxRouterAdvertisementInterval"] =
            entry->MaxRouterAdvertisementInterval;
        item["AdvertisingEnabled"] =
            static_cast<bool>(entry->AdvertisingEnabled);
        item["ForwardingEnabled"] = static_cast<bool>(entry->ForwardingEnabled);
        item["WeakHostSend"] = static_cast<bool>(entry->WeakHostSend);
        item["WeakHostReceive"] = static_cast<bool>(entry->WeakHostReceive);
        item["UseAutomaticMetric"] =
            static_cast<bool>(entry->UseAutomaticMetric);
        item["UseNeighborUnreachabilityDetection"] =
            static_cast<bool>(entry->UseNeighborUnreachabilityDetection);
        item["ManagedAddressConfigurationSupported"] =
            static_cast<bool>(entry->ManagedAddressConfigurationSupported);
        item["OtherStatefulConfigurationSupported"] =
            static_cast<bool>(entry->OtherStatefulConfigurationSupported);
        item["AdvertiseDefaultRoute"] =
            static_cast<bool>(entry->AdvertiseDefaultRoute);
        item["RouterDiscoveryBehavior"] = entry->RouterDiscoveryBehavior;
        item["DadTransmits"] = entry->DadTransmits;
        item["BaseReachableTime"] = entry->BaseReachableTime;
        item["RetransmitTime"] = entry->RetransmitTime;
        item["PathMtuDiscoveryTimeout"] = entry->PathMtuDiscoveryTimeout;
        item["LinkLocalAddressBehavior"] = entry->LinkLocalAddressBehavior;
        item["LinkLocalAddressTimeout"] = entry->LinkLocalAddressTimeout;
        item["ZoneIndices"] = to_string(entry->ZoneIndices,
                                        IM_ARRAYSIZE(entry->ZoneIndices), "-");
        item["SitePrefixLength"] = entry->SitePrefixLength;
        item["Metric"] = entry->Metric;
        item["NlMtu"] = entry->NlMtu;
        item["Connected"] = static_cast<bool>(entry->Connected);
        item["SupportsWakeUpPatterns"] =
            static_cast<bool>(entry->SupportsWakeUpPatterns);
        item["SupportsNeighborDiscovery"] =
            static_cast<bool>(entry->SupportsNeighborDiscovery);
        item["SupportsRouterDiscovery"] =
            static_cast<bool>(entry->SupportsRouterDiscovery);
        item["ReachableTime"] = entry->ReachableTime;

        nlohmann::json transmitOffloadJson;
        transmitOffloadJson["NlChecksumSupported"] =
            static_cast<bool>(entry->TransmitOffload.NlChecksumSupported);
        transmitOffloadJson["NlOptionsSupported"] =
            static_cast<bool>(entry->TransmitOffload.NlOptionsSupported);
        transmitOffloadJson["TlDatagramChecksumSupported"] = static_cast<bool>(
            entry->TransmitOffload.TlDatagramChecksumSupported);
        transmitOffloadJson["TlStreamChecksumSupported"] =
            static_cast<bool>(entry->TransmitOffload.TlStreamChecksumSupported);
        transmitOffloadJson["TlStreamOptionsSupported"] =
            static_cast<bool>(entry->TransmitOffload.TlStreamOptionsSupported);
        transmitOffloadJson["FastPathCompatible"] =
            static_cast<bool>(entry->TransmitOffload.FastPathCompatible);
        transmitOffloadJson["TlLargeSendOffloadSupported"] = static_cast<bool>(
            entry->TransmitOffload.TlLargeSendOffloadSupported);
        transmitOffloadJson["TlGiantSendOffloadSupported"] = static_cast<bool>(
            entry->TransmitOffload.TlGiantSendOffloadSupported);
        item["TransmitOffload"] = transmitOffloadJson;

        nlohmann::json receiveOffloadJson;
        receiveOffloadJson["NlChecksumSupported"] =
            static_cast<bool>(entry->ReceiveOffload.NlChecksumSupported);
        receiveOffloadJson["NlOptionsSupported"] =
            static_cast<bool>(entry->ReceiveOffload.NlOptionsSupported);
        receiveOffloadJson["TlDatagramChecksumSupported"] = static_cast<bool>(
            entry->ReceiveOffload.TlDatagramChecksumSupported);
        receiveOffloadJson["TlStreamChecksumSupported"] =
            static_cast<bool>(entry->ReceiveOffload.TlStreamChecksumSupported);
        receiveOffloadJson["TlStreamOptionsSupported"] =
            static_cast<bool>(entry->ReceiveOffload.TlStreamOptionsSupported);
        receiveOffloadJson["FastPathCompatible"] =
            static_cast<bool>(entry->ReceiveOffload.FastPathCompatible);
        receiveOffloadJson["TlLargeSendOffloadSupported"] = static_cast<bool>(
            entry->ReceiveOffload.TlLargeSendOffloadSupported);
        receiveOffloadJson["TlGiantSendOffloadSupported"] = static_cast<bool>(
            entry->ReceiveOffload.TlGiantSendOffloadSupported);
        item["TransmitOffload"] = receiveOffloadJson;

        item["DisableDefaultRoutes"] =
            static_cast<bool>(entry->DisableDefaultRoutes);

        json.push_back(item);
    }

    return json.dump(4);
}

static std::string s_widget_debug_ip_forward()
{
    iwr::Pointer<MIB_IPFORWARD_TABLE2> pIpForwardTable(FreeMibTable);
    DWORD ret = GetIpForwardTable2(AF_UNSPEC, &pIpForwardTable);
    if (ret != NO_ERROR)
    {
        throw std::runtime_error("GetIpForwardTable2 failed");
    }

    nlohmann::json json = nlohmann::json::array();
    for (size_t i = 0; i < pIpForwardTable->NumEntries; i++)
    {
        nlohmann::json            item;
        const MIB_IPFORWARD_ROW2* info = &pIpForwardTable->Table[i];
        item["InterfaceLuid"] = info->InterfaceLuid.Value;
        item["InterfaceIndex"] = info->InterfaceIndex;

        nlohmann::json destinationPrefixJson;
        destinationPrefixJson["Prefix"] =
            to_string(&info->DestinationPrefix.Prefix);
        destinationPrefixJson["PrefixLength"] =
            info->DestinationPrefix.PrefixLength;
        item["DestinationPrefix"] = destinationPrefixJson;

        item["NextHop"] = to_string(&info->NextHop);
        item["SitePrefixLength"] = info->SitePrefixLength;
        item["ValidLifetime"] = info->ValidLifetime;
        item["PreferredLifetime"] = info->PreferredLifetime;
        item["Metric"] = info->Metric;
        item["Protocol"] = info->Protocol;
        item["Loopback"] = static_cast<bool>(info->Loopback);
        item["AutoconfigureAddress"] =
            static_cast<bool>(info->AutoconfigureAddress);
        item["Publish"] = static_cast<bool>(info->Publish);
        item["Immortal"] = static_cast<bool>(info->Immortal);
        item["Age"] = info->Age;
        item["Origin"] = info->Origin;

        json.push_back(item);
    }

    return json.dump(4);
}

static std::string s_widget_debug_adapter_addresses()
{
    ULONG                             outBufLen = 32 * 1024;
    iwr::Memory<IP_ADAPTER_ADDRESSES> buff(outBufLen);

    const ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    DWORD       dwRetVal = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr,
                                                buff.data(), &outBufLen);
    if (dwRetVal != ERROR_SUCCESS)
    {
        throw std::runtime_error("GetAdaptersAddresses failed");
    }

    nlohmann::json json = nlohmann::json::array();
    for (const IP_ADAPTER_ADDRESSES* pAddr = buff.data(); pAddr;
         pAddr = pAddr->Next)
    {
        nlohmann::json item;
        item["IfIndex"] = pAddr->IfIndex;
        item["AdapterName"] = pAddr->AdapterName;

        nlohmann::json unicastJson = nlohmann::json::array();
        for (const IP_ADAPTER_UNICAST_ADDRESS_LH* pUnicastAddr =
                 pAddr->FirstUnicastAddress;
             pUnicastAddr; pUnicastAddr = pUnicastAddr->Next)
        {
            nlohmann::json unicastItem;
            unicastItem["Address"] = to_string(&pUnicastAddr->Address);
            unicastItem["PrefixOrigin"] = pUnicastAddr->PrefixOrigin;
            unicastItem["SuffixOrigin"] = pUnicastAddr->SuffixOrigin;
            unicastItem["DadState"] = pUnicastAddr->DadState;
            unicastItem["ValidLifetime"] = pUnicastAddr->ValidLifetime;
            unicastItem["PreferredLifetime"] = pUnicastAddr->PreferredLifetime;
            unicastItem["LeaseLifetime"] = pUnicastAddr->LeaseLifetime;
            unicastItem["OnLinkPrefixLength"] =
                pUnicastAddr->OnLinkPrefixLength;
            unicastJson.push_back(unicastItem);
        }
        item["UnicastAddresses"] = unicastJson;

        nlohmann::json anycastJson = nlohmann::json::array();
        for (const IP_ADAPTER_ANYCAST_ADDRESS_XP* pAnycastAddr =
                 pAddr->FirstAnycastAddress;
             pAnycastAddr; pAnycastAddr = pAnycastAddr->Next)
        {
            nlohmann::json anycastItem;
            anycastItem["Address"] = to_string(&pAnycastAddr->Address);
        }
        item["AnycastAddress"] = anycastJson;

        nlohmann::json multicastJson = nlohmann::json::array();
        for (const IP_ADAPTER_MULTICAST_ADDRESS_XP* multicastAddr =
                 pAddr->FirstMulticastAddress;
             multicastAddr; multicastAddr = multicastAddr->Next)
        {
            nlohmann::json multicastItem;
            multicastItem["Address"] = to_string(&multicastAddr->Address);
        }
        item["MulticastAddress"] = multicastJson;

        nlohmann::json dnsServerJson = nlohmann::json::array();
        for (const IP_ADAPTER_DNS_SERVER_ADDRESS_XP* dnsServerAddr =
                 pAddr->FirstDnsServerAddress;
             dnsServerAddr; dnsServerAddr = dnsServerAddr->Next)
        {
            nlohmann::json dnsServerItem;
            dnsServerItem["Address"] = to_string(&dnsServerAddr->Address);
        }
        item["DnsServerAddress"] = dnsServerJson;

        item["DnsSuffix"] = pAddr->DnsSuffix;
        item["Description"] = pAddr->Description;
        item["FriendlyName"] = pAddr->FriendlyName;
        item["PhysicalAddress"] = iwr::hex_dump(
            pAddr->PhysicalAddress, pAddr->PhysicalAddressLength, "-");
        item["DdnsEnabled"] = static_cast<bool>(pAddr->DdnsEnabled);
        item["RegisterAdapterSuffix"] =
            static_cast<bool>(pAddr->RegisterAdapterSuffix);
        item["Dhcpv4Enabled"] = static_cast<bool>(pAddr->Dhcpv4Enabled);
        item["ReceiveOnly"] = static_cast<bool>(pAddr->ReceiveOnly);
        item["NoMulticast"] = static_cast<bool>(pAddr->NoMulticast);
        item["Ipv6OtherStatefulConfig"] =
            static_cast<bool>(pAddr->Ipv6OtherStatefulConfig);
        item["NetbiosOverTcpipEnabled"] =
            static_cast<bool>(pAddr->NetbiosOverTcpipEnabled);
        item["Ipv4Enabled"] = static_cast<bool>(pAddr->Ipv4Enabled);
        item["Ipv6Enabled"] = static_cast<bool>(pAddr->Ipv6Enabled);
        item["Ipv6ManagedAddressConfigurationSupported"] =
            static_cast<bool>(pAddr->Ipv6ManagedAddressConfigurationSupported);
        item["Mtu"] = pAddr->Mtu;
        item["IfType"] = pAddr->IfType;
        item["OperStatus"] = pAddr->OperStatus;
        item["Ipv6IfIndex"] = pAddr->Ipv6IfIndex;
        item["ZoneIndices"] = to_string(pAddr->ZoneIndices,
                                        IM_ARRAYSIZE(pAddr->ZoneIndices), "-");

        nlohmann::json prefixJson = nlohmann::json::array();
        for (const IP_ADAPTER_PREFIX_XP* pPrefix = pAddr->FirstPrefix; pPrefix;
             pPrefix = pPrefix->Next)
        {
            nlohmann::json prefixItem;
            prefixItem["Address"] = to_string(&pPrefix->Address);
            prefixItem["PrefixLength"] = pPrefix->PrefixLength;
        }
        item["Prefix"] = prefixJson;

        item["TransmitLinkSpeed"] = pAddr->TransmitLinkSpeed;
        item["ReceiveLinkSpeed"] = pAddr->ReceiveLinkSpeed;

        nlohmann::json winServerJson = nlohmann::json::array();
        for (const IP_ADAPTER_WINS_SERVER_ADDRESS_LH* pWinServerAddr =
                 pAddr->FirstWinsServerAddress;
             pWinServerAddr; pWinServerAddr = pWinServerAddr->Next)
        {
            nlohmann::json winServerItem;
            winServerItem["Address"] = to_string(&pWinServerAddr->Address);
        }
        item["WinServerAddress"] = winServerJson;

        nlohmann::json gatewayJson = nlohmann::json::array();
        for (const IP_ADAPTER_GATEWAY_ADDRESS_LH* pGatewayAddr =
                 pAddr->FirstGatewayAddress;
             pGatewayAddr; pGatewayAddr = pGatewayAddr->Next)
        {
            nlohmann::json gatewayItem;
            gatewayItem["Address"] = to_string(&pGatewayAddr->Address);
        }
        item["GatewayAddress"] = gatewayJson;

        item["Ipv4Metric"] = pAddr->Ipv4Metric;
        item["Ipv6Metric"] = pAddr->Ipv6Metric;
        item["Luid"] = pAddr->Luid.Value;
        item["Dhcpv4Server"] = to_string(&pAddr->Dhcpv4Server);
        item["CompartmentId"] = pAddr->CompartmentId;
        item["NetworkGuid"] = to_string(&pAddr->NetworkGuid);
        item["ConnectionType"] = pAddr->ConnectionType;
        item["TunnelType"] = pAddr->TunnelType;
        item["Dhcpv6Server"] = to_string(&pAddr->Dhcpv6Server);
        item["Dhcpv6ClientDuid"] = iwr::hex_dump(
            pAddr->Dhcpv6ClientDuid, pAddr->Dhcpv6ClientDuidLength, "-");
        item["Dhcpv6Iaid"] = pAddr->Dhcpv6Iaid;

        json.push_back(item);
    }

    return json.dump(4);
}

static void s_widget_debug_notification()
{
    ImGui::InputText("Title", s_debug->title, sizeof(s_debug->title));
    ImGui::InputTextMultiline("Message", s_debug->message,
                              sizeof(s_debug->message));

    if (ImGui::Button("Post"))
    {
        iwr::NotifyDialog(s_debug->title, s_debug->message);
    }
}

static void s_widget_debug_imgui_demo()
{
    ImGui::Text("Dear ImGui Demo");
    ImGui::ShowDemoWindow(nullptr);
}

static void s_widget_system_interface()
{
    static info_item_t items[] = {
        { "GetIpForwardTable2",   s_widget_debug_ip_forward        },
        { "GetIpInterfaceTable",  s_widget_debug_ip_interface      },
        { "GetAdaptersAddresses", s_widget_debug_adapter_addresses },
    };

    const char* preview_value = items[s_debug->system_interface_selected].name;
    const int   flags = 0;
    if (ImGui::BeginCombo("API name", preview_value, flags))
    {
        for (int i = 0; i < IM_ARRAYSIZE(items); i++)
        {
            const bool selected = (i == s_debug->system_interface_selected);
            if (ImGui::Selectable(items[i].name, selected))
            {
                s_debug->system_interface_selected = i;
                s_debug->system_interface_data = items[i].get();
            }

            if (selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }
    ImGui::SameLine();

    if (ImGui::Button("Refresh"))
    {
        s_debug->system_interface_data =
            items[s_debug->system_interface_selected].get();
    }
    ImGui::SameLine();

    if (ImGui::Button("Copy to Clipboard"))
    {
        s_win32_set_clipboard(s_debug->system_interface_data);
    }

    const int childFlags = 0;
    if (ImGui::BeginChild("win32 api content", ImVec2(0, 240), childFlags))
    {
        ImGui::TextWrapped("%s", s_debug->system_interface_data.c_str());
    }
    ImGui::EndChild();
}

static void s_widget_debug_show()
{
    static iwr::UiTab tabs[] = {
        { "Win32 API",    s_widget_system_interface   },
        { "Notification", s_widget_debug_notification },
        { "ImGui Demo",   s_widget_debug_imgui_demo   },
    };

    if (ImGui::BeginTabBar("debug_tabs"))
    {
        for (size_t i = 0; i < IM_ARRAYSIZE(tabs); i++)
        {
            auto tab = &tabs[i];
            if (ImGui::BeginTabItem(tab->name))
            {
                tab->draw();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
}

static void s_widget_debug_draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(T->help))
        {
            ImGui::MenuItem(T->debug, nullptr, &s_debug->show_window);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (!s_debug->show_window)
    {
        return;
    }

    s_debug->window_title->build(T->debug);
    ImGui::SetNextWindowSize(s_debug->default_window_size,
                             ImGuiCond_FirstUseEver);

    const char* title = s_debug->window_title->title();
    const int   flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (ImGui::Begin(title, &s_debug->show_window, flags))
    {
        s_widget_debug_show();
    }
    ImGui::End();
}

const iwr::widget_t iwr::widget_debug = {
    s_widget_debug_init,
    s_widget_debug_exit,
    s_widget_debug_draw,
};
