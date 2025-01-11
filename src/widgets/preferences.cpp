#include <imgui.h>
#include "i18n/__init__.h"
#include "utils/title_builder.hpp"
#include "__init__.hpp"

typedef struct widget_preferences
{
    widget_preferences();
    ~widget_preferences();

    bool               show_window;
    int                selected_locale;
    iwr::TitleBuilder* window_title;
} widget_preferences_t;

typedef struct preference_tab
{
    /**
     * @brief The name of tab.
     */
    const char* name;

    /**
     * @brief Draw function.
     */
    void (*draw)();
} preference_tab_t;

static widget_preferences_t* s_preferences = nullptr;

widget_preferences::widget_preferences()
{
    show_window = false;
    selected_locale = 0;
    window_title = new iwr::TitleBuilder("__WIDGET_PREFERENCES");
}

widget_preferences::~widget_preferences()
{
    delete window_title;
}

static void s_widget_preferences_init()
{
    s_preferences = new widget_preferences_t;
}

static void s_widget_preferences_exit()
{
    delete s_preferences;
    s_preferences = nullptr;
}

static void s_widget_preferences_show_generic()
{
    static const char* locales[] = {
#define EXPAND_LOCALES(e) #e,
        IWR_I18N_LOCALE_TABLE(EXPAND_LOCALES)
#undef EXPAND_LOCALES
    };

    const char* label = T->localization;
    if (ImGui::Combo(label, &s_preferences->selected_locale, locales,
                     IM_ARRAYSIZE(locales)))
    {
        iwr_i18n_setlocale(locales[s_preferences->selected_locale]);
    }
}

static void s_widget_preferences_show()
{
    const preference_tab_t tabs[] = {
        { T->generic, s_widget_preferences_show_generic },
    };

    if (ImGui::BeginTabBar("preferences_tab"))
    {
        for (size_t i = 0; i < IM_ARRAYSIZE(tabs); i++)
        {
            const preference_tab_t* tab = &tabs[i];
            if (ImGui::BeginTabItem(tab->name))
            {
                tab->draw();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
}

static void s_widget_preferences_draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(T->settings))
        {
            ImGui::MenuItem(T->preferences, nullptr,
                            &s_preferences->show_window);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    if (!s_preferences->show_window)
    {
        return;
    }

    s_preferences->window_title->build(T->preferences);

    const char* title = s_preferences->window_title->title();
    const int   flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (ImGui::Begin(title, &s_preferences->show_window, flags))
    {
        s_widget_preferences_show();
    }
    ImGui::End();
}

const iwr::widget_t iwr::widget_preferences = {
    s_widget_preferences_init,
    s_widget_preferences_exit,
    s_widget_preferences_draw,
};
