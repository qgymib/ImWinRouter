#include <string>
#include "title_builder.hpp"

struct iwr::TitleBuilder::Internal
{
    explicit Internal(const char* id);
    void build(const char* name);

    std::string id;
    std::string title;
    const char* last_name;
};

iwr::TitleBuilder::Internal::Internal(const char* id)
{
    this->id = id;
    this->last_name = nullptr;
}

void iwr::TitleBuilder::Internal::build(const char* name)
{
    if (name == this->last_name)
    {
        return;
    }

    this->last_name = name;
    this->title = name;
    this->title += "###" + this->id;
}

iwr::TitleBuilder::TitleBuilder(const char* id)
{
    m_internal = new Internal(id);
}

iwr::TitleBuilder::~TitleBuilder()
{
    delete m_internal;
}

void iwr::TitleBuilder::build(const char* name)
{
    m_internal->build(name);
}

const char* iwr::TitleBuilder::title()
{
    return m_internal->title.c_str();
}
