#include <imgui.h>
#include "i18n/__init__.h"
#include "__init__.hpp"

typedef struct widget_demo
{
    widget_demo();

    bool show_window;
    bool show_imgui_demo;
} widget_demo_t;

static widget_demo_t* s_demo = nullptr;

widget_demo::widget_demo()
{
    show_window = false;
    show_imgui_demo = false;
}

static void s_widget_demo_init()
{
    s_demo = new widget_demo_t;
}

static void s_widget_demo_exit()
{
    delete s_demo;
    s_demo = nullptr;
}

static void s_widget_demo_show()
{
    ImGui::Checkbox("Show Imgui Demo window", &s_demo->show_imgui_demo);
    if (s_demo->show_imgui_demo)
    {
        ImGui::ShowDemoWindow(&s_demo->show_imgui_demo);
    }
}

static void s_widget_demo_draw()
{
    const char* window_title = "Demo";
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(T->help))
        {
            ImGui::MenuItem(window_title, nullptr, &s_demo->show_window);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (!s_demo->show_window)
    {
        return;
    }

    const int window_flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (ImGui::Begin(window_title, &s_demo->show_window, window_flags))
    {
        s_widget_demo_show();
    }
    ImGui::End();
}

const iwr::widget_t iwr::widget_demo = {
    s_widget_demo_init,
    s_widget_demo_exit,
    s_widget_demo_draw,
};
