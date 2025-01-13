#include <ws2tcpip.h>
#include <Winsock2.h>
#include <netioapi.h>
#include <imgui.h>
#include <vector>
#include <cinttypes>
#include "i18n/__init__.h"
#include "utils/ip.hpp"
#include "utils/title_builder.hpp"
#include "__init__.hpp"

typedef struct route_item
{
    std::string Destination;
    std::string Gateway;
    ULONG       Interface;
    ULONG       Metric;
} route_item_t;

typedef std::vector<route_item_t> RouteVec;

typedef struct widget_router
{
    widget_router();
    ~widget_router();

    bool                show_window;
    float               ipv6_text_width;
    ImVec2              window_size;
    iwr::TitleBuilder*  window_title;
    RouteVec            ipv4_routes;
    RouteVec            ipv6_routes;
    iwr::IpInterfaceVec ip_interfaces;
} widget_router_t;

static widget_router_t* s_router = nullptr;

widget_router::widget_router()
{
    show_window = false;
    ipv6_text_width = 300;
    window_size = ImVec2(0, ImGui::GetTextLineHeight() * 5);
    window_title = new iwr::TitleBuilder("__WIDGET_ROUTER");
}

widget_router::~widget_router()
{
    delete window_title;
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

    s_router->ip_interfaces = iwr::GetIpInterfaces();

    PMIB_IPFORWARD_TABLE2 pIpForwardTable = nullptr;
    if (GetIpForwardTable2(AF_UNSPEC, &pIpForwardTable) != NO_ERROR)
    {
        return;
    }

    char dest[INET6_ADDRSTRLEN], gateway[INET6_ADDRSTRLEN];
    for (ULONG i = 0; i < pIpForwardTable->NumEntries; i++)
    {
        MIB_IPFORWARD_ROW2* info = &pIpForwardTable->Table[i];
        if (info->DestinationPrefix.Prefix.si_family == AF_INET)
        {
            inet_ntop(AF_INET, &info->DestinationPrefix.Prefix.Ipv4.sin_addr,
                      dest, sizeof(dest));
            inet_ntop(AF_INET, &info->NextHop.Ipv4.sin_addr, gateway,
                      sizeof(gateway));
            route_item_t item = {
                dest,
                gateway,
                info->InterfaceIndex,
                info->Metric,
            };
            s_router->ipv4_routes.push_back(item);
        }
        else
        {
            inet_ntop(AF_INET6, &info->DestinationPrefix.Prefix.Ipv6.sin6_addr,
                      dest, sizeof(dest));
            inet_ntop(AF_INET6, &info->NextHop.Ipv6.sin6_addr, gateway,
                      sizeof(gateway));
            route_item_t item = {
                dest,
                gateway,
                info->InterfaceIndex,
                info->Metric,
            };
            s_router->ipv6_routes.push_back(item);
        }
    }

    FreeMibTable(pIpForwardTable);
}

static void s_widget_router_init()
{
    s_router = new widget_router_t;
    s_widget_router_refresh();
}

static void s_widget_router_show_ip(const char* id, const RouteVec& vec)
{
    const int table_flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable(id, 4, table_flags))
    {
        ImGui::TableSetupColumn("Destination", 0, s_router->ipv6_text_width);
        ImGui::TableSetupColumn("Gateway", 0, s_router->ipv6_text_width);
        ImGui::TableSetupColumn("Interface", 0, 64);
        ImGui::TableSetupColumn("Metric", 0, 64);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin((int)vec.size());

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                const route_item_t& item = vec[i];
                ImGui::PushID(&item.Destination);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", item.Destination.c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", item.Gateway.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%lu", item.Interface);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%lu", item.Metric);

                ImGui::PopID();
            }
        }

        ImGui::EndTable();
    }
}

static void s_widget_router_show_interface()
{
    const char* table_id = "router_interface";
    const int table_flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable(table_id, 6, table_flags))
    {
        ImGui::TableSetupColumn("family");
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
                ImGui::PushID(item.InterfaceLuid);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", item.family);

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

static void s_widget_router_show()
{
    if (ImGui::Button("Refresh router"))
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

static void s_widget_router_draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(T->tools))
        {
            ImGui::MenuItem(T->router, nullptr, &s_router->show_window);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (!s_router->show_window)
    {
        return;
    }

    s_router->window_title->build(T->router);

    const char* title = s_router->window_title->title();
    const int   flags = 0;
    // ImGui::SetNextWindowSize(s_router->window_size);
    if (ImGui::Begin(title, &s_router->show_window, flags))
    {
        s_widget_router_show();
    }
    ImGui::End();
}

const iwr::widget_t iwr::widget_router = {
    s_widget_router_init,
    s_widget_router_exit,
    s_widget_router_draw,
};
