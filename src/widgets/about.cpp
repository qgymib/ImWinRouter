#include <imgui.h>
#include "i18n/__init__.h"
#include "utils/title_builder.hpp"
#include "__init__.hpp"

typedef struct dep_info
{
    const char* name;
    const char* version;
    const char* url;
} dep_info_t;

typedef struct widget_about
{
    widget_about();
    ~widget_about();

    /**
     * @brief Whether show window.
     */
    bool show_window;

    /**
     * @brief Whether show extra information.
     */
    bool show_extra_info;

    /**
     * @brief Window title.
     */
    iwr::TitleBuilder* window_title;
} widget_about_t;

static widget_about_t* s_about = nullptr;

widget_about::widget_about()
{
    show_window = false;
    show_extra_info = false;
    window_title = new iwr::TitleBuilder("__WIDGET_ABOUT");
}

widget_about::~widget_about()
{
    delete window_title;
}

static void s_widget_about_init()
{
    s_about = new widget_about;
}

static void s_widget_about_exit()
{
    delete s_about;
    s_about = nullptr;
}

static void s_widget_about_show_extra()
{
    static dep_info_t deps[] = {
        { "imgui", IMGUI_VERSION, "https://github.com/ocornut/imgui" },
    };

    const char* table_id = "about_extra_table";
    const int   table_flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable(table_id, 3, table_flags))
    {
        ImGui::TableSetupColumn(T->name);
        ImGui::TableSetupColumn(T->version);
        ImGui::TableSetupColumn(T->homepage);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(IM_ARRAYSIZE(deps));

        while (clipper.Step())
        {
            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd;
                 row_n++)
            {
                dep_info_t* info = &deps[row_n];
                ImGui::PushID((void*)info);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", info->name);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", info->version);

                ImGui::TableSetColumnIndex(2);
                ImGui::TextLinkOpenURL(T->homepage, info->url);

                ImGui::PopID();
            }
        }

        ImGui::EndTable();
    }
}

static void s_widget_about_show_window()
{
    ImGui::Text("ImWinRouter");
    ImGui::TextLinkOpenURL(T->homepage,
                           "https://github.com/qgymib/ImWinRouter");
    ImGui::Separator();

    ImGui::Checkbox("Show extra information", &s_about->show_extra_info);
    if (!s_about->show_extra_info)
    {
        return;
    }

    const char* child_id = "about_show_extra_info";
    ImVec2      child_sz = ImVec2(320, ImGui::GetTextLineHeight() * 4);
    if (ImGui::BeginChild(child_id, child_sz, ImGuiChildFlags_FrameStyle))
    {
        s_widget_about_show_extra();
    }
    ImGui::EndChild();
}

static void s_widget_about_draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(T->help))
        {
            ImGui::MenuItem(T->about, nullptr, &s_about->show_window);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (!s_about->show_window)
    {
        return;
    }

    s_about->window_title->build(T->about);

    const char* title = s_about->window_title->title();
    const int   flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (ImGui::Begin(title, &s_about->show_window, flags))
    {
        s_widget_about_show_window();
    }
    ImGui::End();
}

const iwr::widget_t iwr::widget_about = {
    s_widget_about_init,
    s_widget_about_exit,
    s_widget_about_draw,
};
