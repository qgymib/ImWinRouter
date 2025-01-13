#include <imgui.h>
#include <cinttypes>
#include "i18n/__init__.h"
#include "utils/ip.hpp"
#include "utils/title_builder.hpp"
#include "__init__.hpp"
#include "notify.hpp"

typedef struct widget_debug
{
    widget_debug();
    ~widget_debug();

    bool               show_window;
    ImVec2             default_window_size;
    iwr::TitleBuilder* window_title;

    iwr::IpInterfaceVec ip_interfaces;
    iwr::IpForwardVec   ip_forwards;
} widget_debug_t;

static widget_debug_t* s_debug = nullptr;

widget_debug::widget_debug()
{
    show_window = false;
    default_window_size = ImVec2(640, 320);
    window_title = new iwr::TitleBuilder("__WIDGET_DEBUG");
    ip_interfaces = iwr::GetIpInterfaceVec();
    ip_forwards = iwr::GetIpForwardVec();
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

static void s_widget_debug_ip_interface_refresh()
{
    s_debug->ip_interfaces = iwr::GetIpInterfaceVec();
}

static void s_widget_debug_ip_interface()
{
    if (ImGui::Button(T->refresh))
    {
        s_widget_debug_ip_interface_refresh();
    }

    const char* table_id = "debug_ip_interface";
    const int   table_flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable(table_id, 6, table_flags))
    {
        ImGui::TableSetupColumn("Family");
        ImGui::TableSetupColumn("InterfaceLuid");
        ImGui::TableSetupColumn("InterfaceIndex");
        ImGui::TableSetupColumn("Metric");
        ImGui::TableSetupColumn("Connected");
        ImGui::TableSetupColumn("DisableDefaultRoutes");
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(s_debug->ip_interfaces.size()));

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                auto& item = s_debug->ip_interfaces[i];
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

static void s_widget_debug_ip_forward_refresh()
{
    s_debug->ip_forwards = iwr::GetIpForwardVec();
}

static void s_widget_debug_ip_forward()
{
    if (ImGui::Button(T->refresh))
    {
        s_widget_debug_ip_forward_refresh();
    }

    const char* table_id = "debug_ip_forward";
    const int   table_flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable(table_id, 7, table_flags))
    {
        ImGui::TableSetupColumn("Family");
        ImGui::TableSetupColumn("Destination");
        ImGui::TableSetupColumn("PrefixLength");
        ImGui::TableSetupColumn("NextHop");
        ImGui::TableSetupColumn("InterfaceLuid");
        ImGui::TableSetupColumn("InterfaceIndex");
        ImGui::TableSetupColumn("Metric");
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(s_debug->ip_forwards.size()));

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                auto& item = s_debug->ip_forwards[i];
                ImGui::PushID(reinterpret_cast<void*>(item.InterfaceLuid));
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", item.Family);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", item.Destination.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%u", item.PrefixLength);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", item.NextHop.c_str());

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%" PRIu64, item.InterfaceLuid);

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%" PRIu64, item.InterfaceIndex);

                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%" PRIu32, item.Metric);

                ImGui::PopID();
            }
        }

        ImGui::EndTable();
    }
}

static void s_widget_debug_notification()
{
    static char title[4096];
    ImGui::InputText("Title", title, sizeof(title));
    static char message[4096];
    ImGui::InputTextMultiline("Message", message, sizeof(message));

    if (ImGui::Button("Post"))
    {
        iwr::NotifyDialog(title, message);
    }
}

static void s_widget_debug_show()
{
    static iwr::UiTab tabs[] = {
        { "IpForward",    s_widget_debug_ip_forward   },
        { "IpInterface",  s_widget_debug_ip_interface },
        { "Notification", s_widget_debug_notification },
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
    const int   flags = 0;
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
