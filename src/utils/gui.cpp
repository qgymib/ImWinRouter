#include <imgui.h>
#include <vector>
#include "gui.hpp"

void iwr::GuiTooltip(const std::string& tip)
{
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(tip.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void iwr::GuiHelpMark(const std::string& tip)
{
    ImGui::TextDisabled("(?)");
    GuiTooltip(tip);
}

struct iwr::LabelText::Internal
{
    struct TextItem
    {
        std::string label;
        std::string text;
    };
    typedef std::vector<TextItem> TextVec;

    std::string table_id;
    TextVec     texts;
};

iwr::LabelText::LabelText()
{
    m_internal = new Internal;

    char buff[64];
    snprintf(buff, sizeof(buff), "%p", this);
    m_internal->table_id = std::string("__label_text##") + buff;
}

iwr::LabelText::~LabelText()
{
    delete m_internal;
}

iwr::LabelText& iwr::LabelText::Add(const char* label, const char* message)
{
    m_internal->texts.push_back(
        iwr::LabelText::Internal::TextItem{ label, message });
    return *this;
}

iwr::LabelText& iwr::LabelText::Add(const char* label, const wchar_t* message)
{
    m_internal->texts.push_back(iwr::LabelText::Internal::TextItem{
        label, iwr::ToString(message) });
    return *this;
}

iwr::LabelText& iwr::LabelText::Add(const char*        label,
                                    const std::string& message)
{
    m_internal->texts.push_back(
        iwr::LabelText::Internal::TextItem{ label, message });
    return *this;
}

iwr::LabelText& iwr::LabelText::Add(const char*         label,
                                    const std::wstring& message)
{
    m_internal->texts.push_back(iwr::LabelText::Internal::TextItem{
        label, iwr::ToString(message.c_str()) });
    return *this;
}

void iwr::LabelText::Clear()
{
    m_internal->texts.clear();
}

void iwr::LabelText::Draw()
{
    const int flags =
        ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingFixedSame;
    if (ImGui::BeginTable(m_internal->table_id.c_str(), 2, flags))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        for (const auto& item : m_internal->texts)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", item.label.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", item.text.c_str());
        }

        ImGui::EndTable();
    }
}
