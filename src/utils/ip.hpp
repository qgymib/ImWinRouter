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

struct AdaptersAddresses
{
    /**
     * @brief The name of the adapter with which these addresses are associated.
     *
     * Unlike an adapter's friendly name, the adapter name specified in
     * AdapterName is permanent and cannot be modified by the user.
     */
    std::string AdapterName;

    /**
     * @brief A user-friendly name for the adapter.
     *
     * This name appears in contexts such as the ipconfig command line program
     * and the Connection folder.
     */
    std::string FriendlyName;

    /**
     * @brief A description for the adapter.
     */
    std::string Description;

    /**
     * @brief The Media Access Control (MAC) address for the adapter.
     *
     * For interfaces that do not have a data-link layer, this value is empty.
     */
    std::string PhysicalAddress;

    /**
     * @brief The interface LUID for the adapter address.
     */
    uint64_t Luid;

    bool Ipv4Enabled;
    bool Ipv6Enabled;

    /**
     * @brief The IPv4 interface metric for the adapter address. This member is
     * only applicable to an IPv4 adapter address.
     *
     * The actual route metric used to compute the route preferences for IPv4 is
     * the summation of the route metric offset specified in the Metric member
     * of the MIB_IPFORWARD_ROW2 structure and the interface metric specified in
     * this member for IPv4.
     */
    uint32_t Ipv4Metric;

    /**
     * @brief The IPv6 interface metric for the adapter address. This member is
     * only applicable to an IPv6 adapter address.
     *
     * The actual route metric used to compute the route preferences for IPv6 is
     * the summation of the route metric offset specified in the Metric member
     * of the MIB_IPFORWARD_ROW2 structure and the interface metric specified in
     * this member for IPv4.
     */
    uint32_t Ipv6Metric;
};
typedef std::vector<AdaptersAddresses> AdaptersAddressesVec;

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

/**
 * @brief Retrieves the addresses associated with the adapters on the local
 * computer.
 * @return Adapters address list.
 */
AdaptersAddressesVec GetAdaptersAddressesVec();

} // namespace iwr

#endif // UTILS_IP_HPP
