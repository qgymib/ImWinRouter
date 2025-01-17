#include "utils/win32.hpp"
#include "i18n/__init__.h"
#include "utils/exception.hpp"
#include "utils/ip.hpp"
#include "utils/memory.hpp"
#include "utils/title_builder.hpp"
#include "__init__.hpp"
#include "notify.hpp"

#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <imgui.h>
#include <sstream>
#include <nlohmann/json.hpp>

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
    std::wstring wText = iwr::ToWideString(text);

    size_t  wTextBytes = wText.size() * sizeof(wchar_t);
    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, wTextBytes + sizeof(wchar_t));
    if (hGlob == nullptr)
    {
        throw std::runtime_error("failed to allocate memory for the clipboard");
    }

    void* data = GlobalLock(hGlob);
    memcpy(data, wText.c_str(), wTextBytes + sizeof(wchar_t));
    GlobalUnlock(hGlob);

    if (!OpenClipboard(iwr::GetWindowHandle()))
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
            iwr::ToString(&info->DestinationPrefix.Prefix);
        destinationPrefixJson["PrefixLength"] =
            info->DestinationPrefix.PrefixLength;
        item["DestinationPrefix"] = destinationPrefixJson;

        item["NextHop"] = iwr::ToString(&info->NextHop);
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

    const ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_WINS_INFO |
                        GAA_FLAG_INCLUDE_GATEWAYS |
                        GAA_FLAG_INCLUDE_ALL_INTERFACES |
                        GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER;
    DWORD dwRetVal = ERROR_BUFFER_OVERFLOW;
    while (true)
    {
        dwRetVal = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, buff.data(),
                                        &outBufLen);
        if (dwRetVal != ERROR_BUFFER_OVERFLOW)
        {
            break;
        }
        buff.resize(outBufLen);
    }

    if (dwRetVal != ERROR_SUCCESS)
    {
        throw iwr::Win32Error(dwRetVal, "GetAdaptersAddresses()");
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
            unicastItem["Address"] = iwr::ToString(&pUnicastAddr->Address);
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
            anycastItem["Address"] = iwr::ToString(&pAnycastAddr->Address);
            anycastJson.push_back(anycastItem);
        }
        item["AnycastAddress"] = anycastJson;

        nlohmann::json multicastJson = nlohmann::json::array();
        for (const IP_ADAPTER_MULTICAST_ADDRESS_XP* multicastAddr =
                 pAddr->FirstMulticastAddress;
             multicastAddr; multicastAddr = multicastAddr->Next)
        {
            nlohmann::json multicastItem;
            multicastItem["Address"] = iwr::ToString(&multicastAddr->Address);
            multicastJson.push_back(multicastItem);
        }
        item["MulticastAddress"] = multicastJson;

        nlohmann::json dnsServerJson = nlohmann::json::array();
        for (const IP_ADAPTER_DNS_SERVER_ADDRESS_XP* dnsServerAddr =
                 pAddr->FirstDnsServerAddress;
             dnsServerAddr; dnsServerAddr = dnsServerAddr->Next)
        {
            nlohmann::json dnsServerItem;
            dnsServerItem["Address"] = iwr::ToString(&dnsServerAddr->Address);
            dnsServerJson.push_back(dnsServerItem);
        }
        item["DnsServerAddress"] = dnsServerJson;

        item["DnsSuffix"] = iwr::ToString(pAddr->DnsSuffix);
        item["Description"] = iwr::ToString(pAddr->Description);
        item["FriendlyName"] = iwr::ToString(pAddr->FriendlyName);
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
            prefixItem["Address"] = iwr::ToString(&pPrefix->Address);
            prefixItem["PrefixLength"] = pPrefix->PrefixLength;
            prefixJson.push_back(prefixItem);
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
            winServerItem["Address"] = iwr::ToString(&pWinServerAddr->Address);
            winServerJson.push_back(winServerItem);
        }
        item["WinServerAddress"] = winServerJson;

        nlohmann::json gatewayJson = nlohmann::json::array();
        for (const IP_ADAPTER_GATEWAY_ADDRESS_LH* pGatewayAddr =
                 pAddr->FirstGatewayAddress;
             pGatewayAddr; pGatewayAddr = pGatewayAddr->Next)
        {
            nlohmann::json gatewayItem;
            gatewayItem["Address"] = iwr::ToString(&pGatewayAddr->Address);
            gatewayJson.push_back(gatewayItem);
        }
        item["GatewayAddress"] = gatewayJson;

        item["Ipv4Metric"] = pAddr->Ipv4Metric;
        item["Ipv6Metric"] = pAddr->Ipv6Metric;
        item["Luid"] = pAddr->Luid.Value;
        item["Dhcpv4Server"] = iwr::ToString(&pAddr->Dhcpv4Server);
        item["CompartmentId"] = pAddr->CompartmentId;
        item["NetworkGuid"] = iwr::ToString(&pAddr->NetworkGuid);
        item["ConnectionType"] = pAddr->ConnectionType;
        item["TunnelType"] = pAddr->TunnelType;
        item["Dhcpv6Server"] = iwr::ToString(&pAddr->Dhcpv6Server);
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
        { "GetAdaptersAddresses", s_widget_debug_adapter_addresses },
        { "GetIpForwardTable2",   s_widget_debug_ip_forward        },
        { "GetIpInterfaceTable",  s_widget_debug_ip_interface      },
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

    if (ImGui::Button(T->refresh))
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
