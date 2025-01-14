#ifndef UTILS_IOSTREAM_HPP
#define UTILS_IOSTREAM_HPP

#include <iostream>
#include <vector>
#include <set>
#include <map>

namespace iwr
{

/**
 * @brief Print std::vector.
 * @param[in] os    Out stream.
 * @param[in] vec   The vector to print
 * @return          Reference to std::ostream.
 */
template <typename T>
std::ostream& ostream(std::ostream& os, const std::vector<T>& vec)
{
    const size_t                            vec_sz = vec.size();
    typename std::vector<T>::const_iterator it = vec.begin();

    os << "[";
    for (size_t i = 0; it != vec.end(); ++it)
    {
        os << *it;
        if (++i < vec_sz)
        {
            os << ",";
        }
    }
    os << "]";
    return os;
}

/**
 * @brief Print std::set.
 * @param[in] os    Out stream.
 * @param[in] set   The set to print
 * @return          Reference to std::ostream.
 */
template <typename T>
std::ostream& ostream(std::ostream& os, const std::set<T>& set)
{
    const size_t                         set_sz = set.size();
    typename std::set<T>::const_iterator it = set.begin();

    os << "{";
    for (size_t i = 0; i < set_sz; ++it)
    {
        os << *it;
        if (++i < set_sz)
        {
            os << ",";
        }
    }
    os << "}";
    return os;
}

/**
 * @brief Print std::map.
 * @param[in] os    Out stream.
 * @param[in] map   The map to print
 * @return          Reference to std::ostream.
 */
template <typename K, typename V>
std::ostream& ostream(std::ostream& os, const std::map<K, V>& map)
{
    const size_t                            map_sz = map.size();
    typename std::map<K, V>::const_iterator it = map.begin();

    os << "{";
    for (size_t i = 0; i < map_sz; ++it)
    {
        os << it->first << ":" << it->second;
        if (++i < map_sz)
        {
            os << ",";
        }
    }
    os << "}";
    return os;
}

} // namespace iwr

#endif // UTILS_IOSTREAM_HPP
