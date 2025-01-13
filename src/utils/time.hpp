#ifndef UTILS_TIME_HPP
#define UTILS_TIME_HPP

#include <cstdint>

namespace iwr
{

/**
 * @brief Get high-resolution time in nanoseconds.
 * @return High-resolution time in nanoseconds.
 */
uint64_t hrtime();

/**
 * @brief Get low-resolution time in nanoseconds.
 * @return Low-resolution time in nanoseconds.
 */
uint64_t lrtime();

}

#endif // UTILS_TIME_HPP
