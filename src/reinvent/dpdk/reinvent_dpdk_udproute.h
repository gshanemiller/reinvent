#pragma once

// Purpose: UDP route
//
// Classes:
//  Dpdk::UDPRoute: Value semantic object holding combined L2 ether src/dst, L3 src/dst IP addresses, and L4 (UDP)
//                  src/dst ports
//
// See Also:
//  Dpdk::InitAWS
//  Dpdk::AWSEnaConfig
//  integration_tests/reinvent_dpdk_udp minimal-complete client/server example
//  Packet routing documentation in doc/aws_ena_packet_design.md
//  
// Thread Safety: MT thread-safe.
//                                                                                                                      
// Exception Policy: No exceptions                                                                                      
//
// Description: For TCP/UDP packet transmission each such packet must hold a route. The simplest possible route is a
// pair of 3-tuples src=(macAddress, IP address, port) and dst=(macAddress, IP address, port). Principally a TX
// concern, TX side work will construct a route backwards from how RSS on RX side will deliver packets to RX queues.
//
// This class is called 'UDPRoute' but in fact it holds data placed into an ether header (L2), IP header (L3), and
// L4 UDP header.
//
// Helper methods are provided to converted MAC, IP addresses from string format to DPDK binary format.
//
// +------------+-------------+----------------+------------------------------+
// | Field      |  Type       | Default        | Comment                      |
// +------------+-------------+----------------+------------------------------+
// | srcMac     |  string     | empty string   | MAC address of the TX queue  |
// |            |             |                | transmitting packet          |
// +------------+-------------+----------------+------------------------------+
// | dstMac     |  string     | empty string   | MAC address of the RX queue  |
// |            |             |                | to receive packet            |
// +------------+-------------+----------------+------------------------------+
// | dstIp      |  string     | empty string   | IPV4 IP address of TX queue  |
// |            |             |                | transmitting pack            |
// +------------+-------------+----------------+------------------------------+
// | srcIp      |  string     | empty string   | IPV4 IP address of RX queue  |
// |            |             |                | to receive packet            |
// +------------+-------------+----------------+------------------------------+
// | srcPort    |  int        | 0              | port from which the TX queue |
// |            |             |                | transmits the packet out of  |
// +------------+-------------+----------------+------------------------------+
// | dstPort    |  int        | 0              | port from which the RX queue |
// |            |             |                | will receive packets         |
// +------------+-------------+----------------+------------------------------+

//                                                                                                                      
// Tell GCC to not enforce '-Wpendantic' for DPDK headers                                                               
//                                                                                                                      
#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wpedantic"                                                                             
#include <rte_errno.h>
#include <rte_ether.h>                                                                                                  
#pragma GCC diagnostic pop 

#include <string>
#include <iostream>
#include <string.h>
#include <errno.h>

#include <dpdk/reinvent_dpdk_util.h>
#include <util/reinvent_util_errno.h>

#include <assert.h>

namespace Reinvent {
namespace Dpdk {

class UDPRoute {
  // DATA
  std::string   d_srcMac;
  std::string   d_dstMac;
  std::string   d_srcIp;
  std::string   d_dstIp;
  int           d_srcPort;
  int           d_dstPort;

public:
  // CREATORS
  UDPRoute(const std::string& srcMac, const std::string& dstMac, const std::string& srcIp,
    const std::string& dstIp, int srcPort, int dstPort);
    // Create a object with specified arguments. The behavior is defined provided the MAC, IP arguments are validly
    // formatted strings, and ports are greater than zero. See https://doc.dpdk.org/api/rte__ether_8h.html method
    // 'rte_ether_unformat_addr' for details on MAC addresses as string. Valid IP addresses are "<d>.<d>.<d>.<d>' 
    // where each '0<=d<=255'.

  UDPRoute();
    // Create UDPRoute object with default values.

  UDPRoute(const UDPRoute& object) = default;
    // Create UDPRoute object such *this==object

  ~UDPRoute() = default;
    // Destroy this object

  // ACCESSORS
  const std::string& srcMac() const;
    // Return 'srcMac' attribute

  const std::string& dstMac() const;
    // Return 'srcMac' attribute

  int srcPort() const;
    // Return 'srcPort' attribute

  const std::string& srcIp() const;
    // Return 'srcIp' attribute

  const std::string& dstIp() const;
    // Return 'srcIp' attribute

  int dstPort() const;
    // Return 'dstPort' attribute
   
  int convertSrcMac(rte_ether_addr *value) const;
    // Return 0 if the 'srcMac' attribute was converted into DPDK's binary format and assigned to specified 'value'
    // and non-zero otherwise. Note this routine puts the bytes into the correct order for packet construction.

  int convertDstMac(rte_ether_addr *value) const;
    // Return 0 if the 'dstMac' attribute was converted into DPDK's binary format and assigned to specified 'value'
    // and non-zero otherwise. Note this routine puts the bytes into the correct order for packet construction.

  int convertSrcIp(uint32_t *value) const;
    // Return 0 if the 'srcIp' attribute was converted into DPDK's binary format and assigned to specified 'value'
    // and non-zero otherwise. Note this routine puts the bytes into the correct order for packet construction.

  int convertDstIp(uint32_t *value) const;
    // Return 0 if the 'dstIp' attribute was converted into DPDK's binary format and assigned to specified 'value'
    // and non-zero otherwise. Note this routine puts the bytes into the correct order for packet construction.

  // MANIPULATORS
  UDPRoute& operator=(const UDPRoute& rhs) = default;
    // Create and return an object semantically equal to specified 'rhs'

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Print into specified 'stream' human readable dump of this object returning 'stream'
};

// FREE OPERATORS
std::ostream& operator<<(std::ostream& stream, const UDPRoute& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

bool operator==(const UDPRoute& lhs, const UDPRoute& rhs);
  // Return true if 'lhs, rhs' are semantically equal and false otherwise  

// INLINE FUNCTION DEFINITIONS
inline
UDPRoute::UDPRoute(const std::string& srcMac, const std::string& dstMac, const std::string& srcIp,                          
    const std::string& dstIp, int srcPort, int dstPort)
: d_srcMac(srcMac)
, d_dstMac(dstMac)
, d_srcIp(srcIp)
, d_dstIp(dstIp)
, d_srcPort(srcPort)
, d_dstPort(dstPort)
{
  assert(!d_srcMac.empty());
  assert(!d_dstMac.empty());
  assert(!d_srcIp.empty());
  assert(!d_dstIp.empty());
  assert(srcPort>0);
  assert(dstPort>0);
}

inline
UDPRoute::UDPRoute()
: d_srcPort(0)
, d_dstPort(0)
{
}

// ACCESSORS
inline
const std::string& UDPRoute::srcMac() const {
  return d_srcMac;
}

inline
const std::string& UDPRoute::dstMac() const {
  return d_srcMac;
}

inline
const std::string& UDPRoute::srcIp() const {
  return d_srcIp;
}

inline
const std::string& UDPRoute::dstIp() const {
  return d_dstIp;
}

inline
int UDPRoute::srcPort() const {
  return d_srcPort;
}

inline
int UDPRoute::dstPort() const {
  return d_dstPort;
}

inline
int UDPRoute::convertSrcMac(rte_ether_addr *value) const {
  assert(value);
  int rc;
  if ((rc = rte_ether_unformat_addr(d_srcMac.c_str(), value))!=0) {
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_API, (rc!=0), "Cannot convert source MAC address to binary", rte_strerror(rc), rc);
  }
  return 0;
}

inline
int UDPRoute::convertDstMac(rte_ether_addr *value) const {
  assert(value);
  int rc;
  if ((rc = rte_ether_unformat_addr(d_dstMac.c_str(), value))!=0) {
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_API, (rc!=0), "Cannot convert destination MAC address to binary", rte_strerror(rc), rc);
  }
  return 0;
}

inline
int UDPRoute::convertSrcIp(uint32_t *value) const {
  assert(value);
  unsigned char addr[4];
	int rc = sscanf(d_srcIp.c_str(), "%hhu.%hhu.%hhu.%hhu",addr+0,addr+1,addr+2,addr+3);
  if (rc!=4) {
    // Promote to errno
    rc = EINVAL;
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_API, (rc!=EINVAL), "Cannot convert source IP address to binary", strerror(rc), rc);
	}

  // Assemble bytes
  *value = (uint32_t)(addr[0]) << 24 |
		       (uint32_t)(addr[1]) << 16 |
		       (uint32_t)(addr[2]) << 8 |
		       (uint32_t)(addr[3]);

  // Put into correct byte order
  *value = rte_cpu_to_be_32(*value);
  return 0;
}

inline
int UDPRoute::convertDstIp(uint32_t *value) const {
  assert(value);
  unsigned char addr[4];
	int rc = sscanf(d_dstIp.c_str(), "%hhu.%hhu.%hhu.%hhu",addr+0,addr+1,addr+2,addr+3);
  if (rc!=4) {
    // Promote to errno
    rc = EINVAL;
    REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_API, (rc!=EINVAL), "Cannot convert source IP address to binary", strerror(rc), rc);
	}

  // Assemble bytes
  *value = (uint32_t)(addr[0]) << 24 |
		       (uint32_t)(addr[1]) << 16 |
		       (uint32_t)(addr[2]) << 8 |
		       (uint32_t)(addr[3]);

  // Put into correct byte order
  *value = rte_cpu_to_be_32(*value);
  return 0;
}

// ASPECTS
inline
std::ostream& operator<<(std::ostream& stream, const UDPRoute& object) {
  return object.print(stream);
}

inline
bool operator==(const UDPRoute& lhs, const UDPRoute& rhs) {
  return  ( lhs.srcMac()==rhs.srcMac()   &&
            lhs.dstMac()==rhs.dstMac()   && 
            lhs.srcIp()==rhs.srcIp()     &&
            lhs.dstIp()==rhs.dstIp()     &&
            lhs.srcPort()==rhs.srcPort() &&
            lhs.dstPort()==rhs.dstPort());
}

} // Dpdk
} // Reinvent
