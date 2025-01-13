#include <imgui.h>
#include <vector>
#include "i18n/__init__.h"
#include "utils/time.hpp"
#include "__init__.hpp"
#include "notify.hpp"

struct NotifyData
{
    std::string title;
    std::string message;
    uint64_t    creation_timestamp_ms;
};
typedef std::vector<NotifyData> NotifyDataVec;

typedef struct widget_notify
{
    widget_notify();

    bool          show_window;
    uint64_t      notification_timeout;
    ImVec2        last_window_size;
    NotifyDataVec data_queue;
} widget_notify_t;

static widget_notify_t* s_notify = nullptr;

widget_notify::widget_notify()
{
    show_window = false;
    notification_timeout = 10 * 1000;
    last_window_size = ImVec2(128, 64);
}

static void s_widget_notify_init()
{
    s_notify = new widget_notify_t;
}

static void s_widget_notify_exit()
{
    delete s_notify;
    s_notify = nullptr;
}

static void s_widget_notify_show()
{
    auto& item = s_notify->data_queue[0];
    ImGui::Text("%s", item.title.c_str());
    ImGui::Text("%s", item.message.c_str());

    if (ImGui::IsWindowFocused() || ImGui::IsWindowHovered())
    {
        return;
    }

    const uint64_t curr_ts = iwr::lrtime();
    if (curr_ts - item.creation_timestamp_ms > s_notify->notification_timeout)
    {
        s_notify->data_queue.erase(s_notify->data_queue.begin());
    }
}

static void s_widget_notify_draw()
{
    if (!s_notify->show_window)
    {
        return;
    }
    if (s_notify->data_queue.empty())
    {
        s_notify->show_window = false;
        return;
    }

    ImVec2 view_size = ImGui::GetMainViewport()->WorkSize;
    ImVec2 pos(view_size.x - s_notify->last_window_size.x - 10.0f,
               view_size.y - s_notify->last_window_size.y);
    ImGui::SetNextWindowPos(pos);

    const int flags = ImGuiWindowFlags_NoMove |
                      ImGuiWindowFlags_AlwaysAutoResize |
                      ImGuiWindowFlags_NoFocusOnAppearing;
    if (ImGui::Begin(T->notification, &s_notify->show_window, flags))
    {
        s_widget_notify_show();
        s_notify->last_window_size = ImGui::GetWindowSize();
    }
    ImGui::End();
}

const iwr::widget_t iwr::widget_notify = {
    s_widget_notify_init,
    s_widget_notify_exit,
    s_widget_notify_draw,
};

void iwr::NotifyDialog(const std::string& title, const std::string& message)
{
    NotifyData data = { title, message, iwr::lrtime() };
    s_notify->data_queue.push_back(data);
    s_notify->show_window = true;
}
