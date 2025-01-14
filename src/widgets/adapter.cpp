#include <imgui.h>
#include <cinttypes>
#include "i18n/__init__.h"
#include "utils/ip.hpp"
#include "__init__.hpp"

typedef struct view_adapter
{
    view_adapter();

    ImVec2                    left_pane_size;
    iwr::AdaptersAddressesVec adapter_addresses;
    size_t                    selected_adapter_idx;
} view_adapter_t;

static view_adapter_t* s_adapter = nullptr;

view_adapter::view_adapter()
{
    left_pane_size = ImVec2(200, 0);
    adapter_addresses = iwr::GetAdaptersAddressesVec();
    selected_adapter_idx = 0;
}

static const char* s_view_adapter_name()
{
    return T->network_adapter;
}

static void s_view_adapter_init()
{
    s_adapter = new view_adapter_t;
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
        for (size_t i = 0; i < s_adapter->adapter_addresses.size(); i++)
        {
            const auto& adapter = s_adapter->adapter_addresses[i];
            const char* name = adapter.FriendlyName.c_str();
            bool        selected = s_adapter->selected_adapter_idx == i;
            if (ImGui::Selectable(name, selected))
            {
                s_adapter->selected_adapter_idx = i;
            }
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginGroup();
    if (s_adapter->selected_adapter_idx < s_adapter->adapter_addresses.size())
    {
        const auto& adapter =
            s_adapter->adapter_addresses[s_adapter->selected_adapter_idx];
        ImGui::LabelText("AdapterName", "%s", adapter.AdapterName.c_str());
        ImGui::LabelText("Luid", "%" PRIu64, adapter.Luid);
        ImGui::LabelText("Description", "%s", adapter.Description.c_str());
        ImGui::LabelText("PhysicalAddress", "%s",
                         adapter.PhysicalAddress.c_str());
        ImGui::LabelText("DnsSuffix", "%s", adapter.DnsSuffix.c_str());

        ImGui::SeparatorText("IPv4");
        ImGui::LabelText("Ipv4Enabled", "%d", adapter.Ipv4Enabled);
        ImGui::LabelText("Ipv4Metric", "%" PRIu32, adapter.Ipv4Metric);
        ImGui::LabelText("Dhcpv4Enabled", "%d", adapter.Dhcpv4Enabled);

        ImGui::SeparatorText("IPv6");
        ImGui::LabelText("Ipv6Enabled", "%d", adapter.Ipv6Enabled);
        ImGui::LabelText("Ipv6Metric", "%" PRIu32, adapter.Ipv6Metric);
    }
    ImGui::EndGroup();
}

const iwr::view_t iwr::view_adapter = {
    s_view_adapter_name,
    s_view_adapter_init,
    s_view_adapter_exit,
    s_view_adapter_draw,
};
