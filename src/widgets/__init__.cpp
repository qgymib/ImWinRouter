#include <imgui.h>
#include "__init__.hpp"

static const iwr::widget_t* s_widgets[] = {
    &iwr::widget_preferences,
    &iwr::widget_debug,
    &iwr::widget_about,
    &iwr::widget_notify,
};

static const iwr::view_t* s_views[] = {
    &iwr::view_adapter,
    &iwr::view_router,
};

void iwr::widget_init()
{
    for (auto& widget : s_widgets)
    {
        widget->init();
    }

    for (auto& view : s_views)
    {
        view->init();
    }
}

void iwr::widget_exit()
{
    for (auto& widget : s_widgets)
    {
        widget->exit();
    }

    for (auto& view : s_views)
    {
        view->exit();
    }
}

void iwr::widget_draw()
{
    for (auto& widget : s_widgets)
    {
        widget->draw();
    }

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    const char* window_title = "__widget_mainview";
    const int   window_flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings;
    if (ImGui::Begin(window_title, nullptr, window_flags))
    {
        if (ImGui::BeginTabBar("mainview_tabbar"))
        {
            for (auto& view : s_views)
            {
                if (ImGui::BeginTabItem(view->name()))
                {
                    view->draw();
                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}
