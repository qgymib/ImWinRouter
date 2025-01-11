#include "__init__.hpp"

static const iwr::widget_t* s_widgets[] = {
    &iwr::widget_preferences,
    &iwr::widget_router,
    &iwr::widget_debug,
    &iwr::widget_about,
};

void iwr::widget_init()
{
    for (auto& widget : s_widgets)
    {
        widget->init();
    }
}

void iwr::widget_exit()
{
    for (auto& widget : s_widgets)
    {
        widget->exit();
    }
}

void iwr::widget_draw()
{
    for (auto& widget : s_widgets)
    {
        widget->draw();
    }
}
