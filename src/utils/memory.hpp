#ifndef UTILS_MEMORY_HPP
#define UTILS_MEMORY_HPP

#include <new>
#include <cstdlib>
#include <cstring>

namespace iwr
{

template <typename T>
class Memory
{
public:
    /**
     * @brief Allocate memory.
     * @param size Memory size.
     */
    Memory(size_t size)
    {
        this->m_size = size;
        if ((this->m_data = malloc(size)) == nullptr)
        {
            throw std::bad_alloc();
        }
    }

    virtual ~Memory()
    {
        free(this->m_data);
    }

    Memory(const Memory &other)
    {
        this->m_data = nullptr;
        this->m_size = 0;
        copy(other);
    }

public:
    T* data()
    {
        return static_cast<T*>(this->m_data);
    }

    size_t size()
    {
        return this->m_size;
    }

    void resize(size_t size)
    {
        void* new_data = realloc(this->m_data, size);
        if (new_data == nullptr)
        {
            throw std::bad_alloc();
        }

        this->m_data = new_data;
        this->m_size = size;
    }

    void copy(const Memory &other)
    {
        if (this->m_data != nullptr)
        {
            free(this->m_data);
        }

        this->m_size = other.m_size;
        if ((this->m_data = malloc(this->m_size)) == nullptr)
        {
            throw std::bad_alloc();
        }
        memcpy(this->m_data, other.m_data, this->m_size);
    }

private:
    void*  m_data;
    size_t m_size;
};

} // namespace iwr

#endif // UTILS_MEMORY_HPP
