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

    ImVec2                            left_pane_size;
    iwr::Memory<IP_ADAPTER_ADDRESSES> adapter_addr_buf;
    iwr::LabelText                    texts;

    size_t selected_adapter_idx;
} view_adapter_t;

static view_adapter_t* s_adapter = nullptr;

view_adapter::view_adapter() : adapter_addr_buf(32 * 1024)
{
    left_pane_size = ImVec2(200, 0);
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

static void s_view_adapter_draw()
{
    const int flags = ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX;
    if (ImGui::BeginChild("left pane", s_adapter->left_pane_size, flags))
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

    ImGui::BeginGroup();
    s_adapter->texts.Draw();
    ImGui::EndGroup();
}

const iwr::view_t iwr::view_adapter = {
    s_view_adapter_name,
    s_view_adapter_init,
    s_view_adapter_exit,
    s_view_adapter_draw,
};
