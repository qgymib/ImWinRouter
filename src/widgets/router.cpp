#include <ws2tcpip.h>
#include <Winsock2.h>
#include <netioapi.h>
#include <imgui.h>
#include <vector>
#include <cinttypes>
#include "i18n/__init__.h"
#include "utils/ip.hpp"
#include "__init__.hpp"

typedef struct widget_router
{
    widget_router();
    ~widget_router();

    float               ipv6_text_width;
    iwr::IpForwardVec   ipv4_routes;
    iwr::IpForwardVec   ipv6_routes;
    iwr::IpInterfaceVec ip_interfaces;
} widget_router_t;

static widget_router_t* s_router = nullptr;

widget_router::widget_router()
{
    ipv6_text_width = 300;
}

widget_router::~widget_router()
{
}

static const char* s_view_router_name()
{
    return T->router;
}

static void s_widget_router_exit()
{
    delete s_router;
    s_router = nullptr;
}

static void s_widget_router_refresh()
{
    s_router->ipv4_routes.clear();
    s_router->ipv6_routes.clear();
    s_router->ip_interfaces = iwr::GetIpInterfaceVec();

    iwr::IpForwardVec routes = iwr::GetIpForwardVec();
    for (iwr::IpForward& route : routes)
    {
        if (route.Family == AF_INET)
        {
            s_router->ipv4_routes.push_back(route);
        }
        else
        {
            s_router->ipv6_routes.push_back(route);
        }
    }
}

static void s_widget_router_init()
{
    s_router = new widget_router_t;
    s_widget_router_refresh();
}

static void s_widget_router_show_ip(const char* id, const iwr::IpForwardVec& vec)
{
    const int table_flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable(id, 4, table_flags))
    {
        ImGui::TableSetupColumn("Destination", 0, s_router->ipv6_text_width);
        ImGui::TableSetupColumn("Gateway", 0, s_router->ipv6_text_width);
        ImGui::TableSetupColumn("InterfaceLuid", 0, 64);
        ImGui::TableSetupColumn("Metric", 0, 64);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin((int)vec.size());

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                const iwr::IpForward& item = vec[i];
                ImGui::PushID(&item.Destination);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", item.Destination.c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", item.NextHop.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%" PRIu64, item.InterfaceLuid);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%" PRIu32, item.Metric);

                ImGui::PopID();
            }
        }

        ImGui::EndTable();
    }
}

static void s_widget_router_show_interface()
{
    const char* table_id = "router_interface";
    const int   table_flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable(table_id, 6, table_flags))
    {
        ImGui::TableSetupColumn("Family");
        ImGui::TableSetupColumn("Luid");
        ImGui::TableSetupColumn("Index");
        ImGui::TableSetupColumn("Metric");
        ImGui::TableSetupColumn("Connected");
        ImGui::TableSetupColumn("DisableDefaultRoutes");
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin((int)s_router->ip_interfaces.size());

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                auto& item = s_router->ip_interfaces[i];
                ImGui::PushID(reinterpret_cast<void*>(item.InterfaceLuid));
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", item.Family);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%" PRIu64, item.InterfaceLuid);

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%" PRIu64, item.InterfaceIndex);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%" PRIu32, item.Metric);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d", item.Connected);

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%d", item.DisableDefaultRoutes);

                ImGui::PopID();
            }
        }

        ImGui::EndTable();
    }
}

static void s_widget_router_draw()
{
    if (ImGui::Button(T->refresh))
    {
        s_widget_router_refresh();
    }

    ImGui::SeparatorText("Interface");
    s_widget_router_show_interface();
    ImGui::SeparatorText("IPv4");
    s_widget_router_show_ip("router_table_ipv4", s_router->ipv4_routes);
    ImGui::SeparatorText("IPv6");
    s_widget_router_show_ip("router_table_ipv6", s_router->ipv6_routes);
}

const iwr::view_t iwr::view_router = {
    s_view_router_name,
    s_widget_router_init,
    s_widget_router_exit,
    s_widget_router_draw,
};
