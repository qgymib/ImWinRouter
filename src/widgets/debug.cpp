#include <imgui.h>
#include <sstream>
#include <nlohmann/json.hpp>
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

    std::string ip_interfaces;
    std::string ip_forwards;
    std::string ip_adapters;
} widget_debug_t;

static widget_debug_t* s_debug = nullptr;

widget_debug::widget_debug()
{
    show_window = false;
    default_window_size = ImVec2(640, 320);
    window_title = new iwr::TitleBuilder("__WIDGET_DEBUG");
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
    if (ImGui::Button(T->refresh))
    {
        std::ostringstream oss;
        oss << iwr::GetIpInterfaceVec();
        s_debug->ip_interfaces = nlohmann::json::parse(oss.str()).dump(2);
    }

    ImGui::TextWrapped("%s", s_debug->ip_interfaces.c_str());
}

static void s_widget_debug_ip_forward()
{
    if (ImGui::Button(T->refresh))
    {
        std::ostringstream oss;
        oss << iwr::GetIpForwardVec();
        s_debug->ip_forwards = nlohmann::json::parse(oss.str()).dump(2);
    }

    ImGui::TextWrapped("%s", s_debug->ip_forwards.c_str());
}

static void s_widget_debug_adapters_addresses()
{
    if (ImGui::Button(T->refresh))
    {
        std::ostringstream oss;
        oss << iwr::GetAdaptersAddressesVec();
        s_debug->ip_adapters = nlohmann::json::parse(oss.str()).dump(2);
    }

    ImGui::TextWrapped("%s", s_debug->ip_adapters.c_str());
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

static void s_widget_debug_imgui_demo()
{
    ImGui::Text("Dear ImGui Demo");
    ImGui::ShowDemoWindow(nullptr);
}

static void s_widget_debug_show()
{
    static iwr::UiTab tabs[] = {
        { "IpForward",         s_widget_debug_ip_forward         },
        { "IpInterface",       s_widget_debug_ip_interface       },
        { "AdaptersAddresses", s_widget_debug_adapters_addresses },
        { "Notification",      s_widget_debug_notification       },
        { "ImGui Demo",        s_widget_debug_imgui_demo         },
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
