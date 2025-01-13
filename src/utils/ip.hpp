#ifndef UTILS_IP_HPP
#define UTILS_IP_HPP

#include <cstdint>
#include <vector>
#include "utils/string.hpp"

namespace iwr
{

struct IpForward
{
    /**
     * @brief The address family.
     */
    int Family;

    /**
     * @brief The IP address prefix for the destination IP address for this
     * route.
     */
    std::string Destination;

    /**
     * @brief The length, in bits, of the prefix or network part of the IP
     * address.
     *
     * For a unicast IPv4 address, any value greater than 32 is an
     * illegal value. For a unicast IPv6 address, any value greater than 128
     * is an illegal value. A value of 255 is commonly used to represent an
     * illegal value.
     */
    uint8_t PrefixLength;

    /**
     * @brief The IP address of the next system or gateway en route. Known as
     * gateway.
     *
     * If the route is to a local loopback address or an IP address on the local
     * link, the next hop is unspecified (all zeros). For a local loopback
     * route, this member should be an IPv4 address of 0.0.0.0 for an IPv4 route
     * entry or an IPv6 address of 0::0 for an IPv6 route entry.
     */
    std::string NextHop;

    /**
     * @brief The locally unique identifier (LUID) for the network interface
     *   associated with this IP route entry.
     */
    uint64_t InterfaceLuid;

    /**
     * @brief The local index value for the network interface associated with
     * this IP route entry.
     *
     * This index value may change when a network adapter is disabled and then
     * enabled, or under other circumstances, and should not be considered
     * persistent.
     */
    uint64_t InterfaceIndex;

    /**
     * @brief The route metric offset value for this IP route entry.
     */
    uint32_t Metric;
};
typedef std::vector<IpForward> IpForwardVec;

struct IpInterface
{
    /**
     * @brief The address family of IP interfaces.
     */
    int Family;

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
IpInterfaceVec GetIpInterfaceVec();

/**
 * @brief Retrieves the IP route entries on the local computer.
 * @return Route list.
 */
IpForwardVec GetIpForwardVec();

} // namespace iwr

#endif // UTILS_IP_HPP
