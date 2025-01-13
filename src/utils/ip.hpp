#ifndef UTILS_IP_HPP
#define UTILS_IP_HPP

#include <cstdint>
#include <vector>
#include "utils/string.hpp"

namespace iwr
{

struct RouteItem
{
    std::string Destination;    /**< Destination address. */
    std::string Gateway;        /**< Gateway. */
    unsigned    InterfaceIndex; /**< Interface index. */
    unsigned    Metric;         /**< Metric */
};
typedef std::vector<RouteItem> RouteVec;

struct IpInterface
{
    int family; /**< AF_INET / AF_INET6 */

    /**
     * @brief The locally unique identifier (LUID) for the network interface.
     */
    uint64_t InterfaceLuid;

    /**
     * @brief The local index value for the network interface.
     *
     * This index value may change when a network adapter is disabled and then
     * enabled, or under other circumstances, and should not be considered
     * persistent.
     */
    uint64_t InterfaceIndex;

    /**
     * @brief The interface metric.
     *
     * Note the actual route metric used to compute the route preference is the
     * summation of the route metric offset specified in the Metric member of
     * the MIB_IPFORWARD_ROW2 structure and the interface metric specified in
     * this member.
     */
    uint32_t Metric;

    /**
     * @brief If the interface is connected to a network access point.
     */
    bool Connected;

    /**
     * @brief If using default route on the interface should be disabled.
     */
    bool DisableDefaultRoutes;
};
typedef std::vector<IpInterface> IpInterfaceVec;

/**
 * @brief Retrieves the IP interface entries on the local computer.
 * @return Interface list.
 */
IpInterfaceVec GetIpInterfaces();

} // namespace iwr

#endif // UTILS_IP_HPP
