#include <imgui.h>
#include "i18n/__init__.h"
#include "utils/title_builder.hpp"
#include "__init__.hpp"

typedef struct widget_debug
{
    widget_debug();
    ~widget_debug();

    bool show_window;

    /**
     * @brief Show imgui demo window.
     */
    bool show_imgui_demo;

    iwr::TitleBuilder* window_title;
} widget_debug_t;

static widget_debug_t* s_debug = nullptr;

widget_debug::widget_debug()
{
    show_window = false;
    show_imgui_demo = false;
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

static void s_widget_debug_show()
{
    ImGui::Checkbox("Show Imgui Demo window", &s_debug->show_imgui_demo);
    if (s_debug->show_imgui_demo)
    {
        ImGui::ShowDemoWindow(&s_debug->show_imgui_demo);
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
