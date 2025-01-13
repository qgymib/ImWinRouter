#include <imgui.h>
#include <cinttypes>
#include "i18n/__init__.h"
#include "utils/ip.hpp"
#include "utils/title_builder.hpp"
#include "__init__.hpp"

typedef struct widget_debug
{
    widget_debug();
    ~widget_debug();

    bool                show_window;
    iwr::TitleBuilder*  window_title;
    bool                show_imgui_demo;
    iwr::IpInterfaceVec ip_interfaces;
} widget_debug_t;

static widget_debug_t* s_debug = nullptr;

widget_debug::widget_debug()
{
    show_window = false;
    show_imgui_demo = false;
    window_title = new iwr::TitleBuilder("__WIDGET_DEBUG");
    ip_interfaces = iwr::GetIpInterfaces();
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

static void s_widget_debug_ip_interface()
{
    const char* table_id = "debug_ip_interface";
    const int table_flags =  ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;

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
        clipper.Begin((int)s_debug->ip_interfaces.size());

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                auto& item = s_debug->ip_interfaces[i];
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

static void s_widget_debug_show()
{
    ImGui::Checkbox("Show Imgui Demo window", &s_debug->show_imgui_demo);
    if (s_debug->show_imgui_demo)
    {
        ImGui::ShowDemoWindow(&s_debug->show_imgui_demo);
    }

    static iwr::UiTab tabs[] = {
        { "IpInterface", s_widget_debug_ip_interface },
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
