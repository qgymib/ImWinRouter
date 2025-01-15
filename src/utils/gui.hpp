#ifndef UTILS_GUI_HPP
#define UTILS_GUI_HPP

#include "utils/string.hpp"

namespace iwr
{

/**
 * @brief Show tool tip.
 * @param[in] tip The message to show.
 */
void GuiTooltip(const std::string& tip);

/**
 * @brief Show a help tip.
 * @param[in] tip The message to show.
 */
void GuiHelpMark(const std::string& tip);

class LabelText
{
public:
    LabelText();
    virtual ~LabelText();

public:
    LabelText& Add(const char* label, const char* message);
    LabelText& Add(const char* label, const wchar_t* message);
    LabelText& Add(const char* label, const std::string& message);
    LabelText& Add(const char* label, const std::wstring& message);
    void Clear();
    void Draw();

private:
    struct Internal;
    Internal* m_internal;
};

} // namespace iwr

#endif // UTILS_GUI_HPP
