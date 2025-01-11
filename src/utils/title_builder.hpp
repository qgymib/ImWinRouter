#ifndef UTILS_TITLE_BUILDER_HPP
#define UTILS_TITLE_BUILDER_HPP

namespace iwr
{

class TitleBuilder
{
public:
    explicit TitleBuilder(const char* id);
    virtual ~TitleBuilder();

public:
    /**
     * @brief Build new title name.
     * @param[in] name The name of new title. It must be a static variable.
     */
    void build(const char* name);

    /**
     * @brief Get window title.
     * @return Window title.
     */
    const char* title();

private:
    struct Internal;
    struct Internal* m_internal;
};

}

#endif // UTILS_TITLE_BUILDER_HPP
