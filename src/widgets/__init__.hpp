#ifndef WIDGETS_INIT_HPP
#define WIDGETS_INIT_HPP

namespace iwr
{

struct UiTab
{
    /**
     * @brief The name of tab.
     */
    const char* name;

    /**
     * @brief Draw function.
     */
    void (*draw)();
};

/**
 * @brief Widget type.
 */
typedef struct widget
{
    /**
     * @brief Initialize widget.
     */
    void (*init)();

    /**
     * @brief Cleanup widget.
     */
    void (*exit)();

    /**
     * @brief Draw widget.
     */
    void (*draw)();
} widget_t;

typedef struct view
{
    /**
     * @brief Get name of this view.
     */
    const char* (*name)();

    /**
     * @brief Initialize view.
     */
    void (*init)();

    /**
     * @brief Cleanup view.
     */
    void (*exit)();

    /**
     * @brief Draw view.
     */
    void (*draw)();
} view_t;

/**
 * @brief Initialize all widgets.
 */
void widget_init();

/**
 * @brief Cleanup all widgets.
 */
void widget_exit();

/**
 * @brief Draw all widgets.
 */
void widget_draw();

extern const widget_t widget_about;
extern const widget_t widget_debug;
extern const widget_t widget_notify;
extern const widget_t widget_preferences;

extern const view_t view_adapter;
extern const view_t view_router;

} // namespace iwr

#endif // WIDGETS_INIT_HPP
