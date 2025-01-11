#ifndef WIDGETS_INIT_HPP
#define WIDGETS_INIT_HPP

namespace iwr
{

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
extern const widget_t widget_preferences;
extern const widget_t widget_router;

}

#endif // WIDGETS_INIT_HPP
