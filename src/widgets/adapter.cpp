#include "utils/win32.hpp"
#include <imgui.h>
#include <cinttypes>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "i18n/__init__.h"
#include "utils/exception.hpp"
#include "utils/gui.hpp"
#include "utils/memory.hpp"
#include "__init__.hpp"

typedef struct view_adapter
{
    view_adapter();

    iwr::Memory<IP_ADAPTER_ADDRESSES> adapter_addr_buf;
    iwr::LabelText                    texts;

    float  default_left_pane_width;
    size_t selected_adapter_idx;
} view_adapter_t;

static view_adapter_t* s_adapter = nullptr;

view_adapter::view_adapter() : adapter_addr_buf(32 * 1024)
{
    default_left_pane_width = 200;
    selected_adapter_idx = 0;
}

static const char* s_view_adapter_name()
{
    return T->network_adapter;
}

static void s_view_adapter_rebuild_cache(const IP_ADAPTER_ADDRESSES* entry)
{
    s_adapter->texts.Clear();

    if (entry == nullptr)
    {
        return;
    }

    s_adapter->texts
        .Add("AdapterName:", iwr::ToString("%s", entry->AdapterName))
        .Add("Luid:", iwr::ToString("%" PRIu64, entry->Luid.Value))
        .Add("Description:", entry->Description)
        .Add("PhysicalAddress:",
             iwr::hex_dump(entry->PhysicalAddress, entry->PhysicalAddressLength,
                           "-"))
        .Add("Ipv4Enabled:",
             iwr::ToString("%s", entry->Ipv4Enabled ? "true" : "false"))
        .Add("Ipv6Enabled:",
             iwr::ToString("%s", entry->Ipv6Enabled ? "true" : "false"))
        .Add("Dhcpv4Enabled:",
             iwr::ToString("%s", entry->Dhcpv4Enabled ? "true" : "false"))
        .Add("Dhcpv4Server:", iwr::ToString(&entry->Dhcpv4Server))
        .Add("Ipv4Metric:", iwr::ToString("%lu", entry->Ipv4Metric))
        .Add("Ipv6Metric:", iwr::ToString("%lu", entry->Ipv6Metric));
}

static void s_view_adapter_refresh()
{
    s_adapter->selected_adapter_idx = 0;

    ULONG bufLen = 32 * 1024;
    s_adapter->adapter_addr_buf.resize(bufLen);

    DWORD       dwRetVal = ERROR_BUFFER_OVERFLOW;
    const ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_WINS_INFO |
                        GAA_FLAG_INCLUDE_GATEWAYS |
                        GAA_FLAG_INCLUDE_ALL_INTERFACES |
                        GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER;
    while (true)
    {
        dwRetVal =
            GetAdaptersAddresses(AF_UNSPEC, flags, nullptr,
                                 s_adapter->adapter_addr_buf.data(), &bufLen);
        if (dwRetVal != ERROR_BUFFER_OVERFLOW)
        {
            break;
        }
        s_adapter->adapter_addr_buf.resize(bufLen);
    }
    if (dwRetVal == ERROR_NO_DATA)
    {
        s_view_adapter_rebuild_cache(nullptr);
        return;
    }
    if (dwRetVal != ERROR_SUCCESS)
    {
        throw iwr::Win32Error(dwRetVal, "GetAdaptersAddresses()");
    }

    s_view_adapter_rebuild_cache(s_adapter->adapter_addr_buf.data());
}

static void s_view_adapter_init()
{
    s_adapter = new view_adapter_t;
    s_view_adapter_refresh();
}

static void s_view_adapter_exit()
{
    delete s_adapter;
    s_adapter = nullptr;
}

static void s_view_adapter_draw_details()
{
    s_adapter->texts.Draw();
}

static void s_view_adapter_draw_identity()
{
}

static void s_view_adapter_draw_ipv4()
{
}

static void s_view_adapter_draw_ipv6()
{
}

static void s_view_adapter_draw_security()
{
}

static void s_view_adapter_draw()
{
    const ImVec2 region = ImGui::GetContentRegionAvail();
    const int    flags = ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX;
    if (ImGui::BeginChild("left pane",
                          ImVec2(s_adapter->default_left_pane_width, region.y),
                          flags))
    {
        size_t i = 0;
        for (const IP_ADAPTER_ADDRESSES* pAddr =
                 s_adapter->adapter_addr_buf.data();
             pAddr; i++, pAddr = pAddr->Next)
        {
            std::string name = iwr::ToString(pAddr->FriendlyName);
            const bool  is_selected = (s_adapter->selected_adapter_idx == i);
            if (ImGui::Selectable(name.c_str(), is_selected))
            {
                s_adapter->selected_adapter_idx = i;
                s_view_adapter_rebuild_cache(pAddr);
            }
            iwr::GuiTooltip(name);
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    const iwr::UiTab tabs[] = {
        { "Details",  s_view_adapter_draw_details  },
        { "Identity", s_view_adapter_draw_identity },
        { "IPv4",     s_view_adapter_draw_ipv4     },
        { "IPv6",     s_view_adapter_draw_ipv6     },
        { "Security", s_view_adapter_draw_security },
    };

    if (ImGui::BeginChild("right pane", ImVec2(0, region.y)))
    {
        if (ImGui::BeginTabBar("adapter_tabbar"))
        {
            for (const auto& tab : tabs)
            {
                if (ImGui::BeginTabItem(tab.name))
                {
                    tab.draw();
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();
}

const iwr::view_t iwr::view_adapter = {
    s_view_adapter_name,
    s_view_adapter_init,
    s_view_adapter_exit,
    s_view_adapter_draw,
};
