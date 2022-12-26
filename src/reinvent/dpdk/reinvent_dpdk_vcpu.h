#pragma once

// Purpose: CPU Core Value Semantic Object 
//
// Classes:
//  Dpdk::VCPU: Identifies CPU H/W core relating it to parent CPU, NUMA node
//
// Thread Safety: MT thread-safe.
//                                                                                                                      
// Exception Policy: No exceptions                                                                                      
//
// Description: A bare metal box is equipped with one or more CPUs. Each CPU is associated with a NUMA node, and each
// CPU has one or more cores which or independent execution units with a sharing arrangment among its parent's CPU
// level 1, 2, 3 cache. This class records this inventory one object per core. To facilitate easy reference each object
// is numbered by a kind of foreign key 'id' which is usually just an monotonic increasing counter starting at zero.
// For example, 'id=5' might be short hand for 'NUMA node 1, socket 1, CPU core 13'.
//
// This class makes no attempt to verify a VCPU object corresponds to to real hardware or that its ID is with respect
// to other IDs unique; it only records a HW report. 'softEnabled' has no reference or meaning for actual HW either.
// This attribute is best understood in context. For example, by DPDK/NIC initialization uses this value soft-disabled
// VCPUs fromDPDK RX/TX queue assignment thereby leaving cores for other purposes. Callers usually insure the object's
// values are correct, for example, Dpdk::Config.
//
// +------------+-------------+----------------+------------------------------+
// | Field      |  Type       | Default        | Comment                      |
// +------------+-------------+----------------+------------------------------+
// | id         |  int        | 0              | Caller provided integer name |
// |            |             |                | for CPU core                 |
// +------------+-------------+----------------+------------------------------+
// | core       |  int        | 0              | Caller provided CPU core     |
// |            |             |                | number belonging to socket   |
// +------------+-------------+----------------+------------------------------+
// | socket     |  int        | 0              | Caller provided CPU socket   |
// |            |             |                | number belonging NUMA node   |
// +------------+-------------+----------------+------------------------------+
// | numaNode   |  int        | 0              | Caller provided NUMA node    |
// |            |             |                | to which CPU socket attached |
// +------------+-------------+----------------+------------------------------+
// | softEnab\  |  bool       | true           | Caller provided flag hinting |
// |  able      |             |                | whether or not VCPU can be   |
// |            |             |                | used. Meaning determined in  |
// |            |             |                | context at caller            |
// +------------+-------------+----------------+------------------------------+

#include <iostream>
#include <assert.h>

namespace Reinvent {
namespace Dpdk {

class VCPU {
  // DATA
  int   d_id;
  int   d_core;
  int   d_socket;
  int   d_numaNode;
  bool  d_softEnabled;

public:
  // CREATORS
  VCPU(int id, int core, int socket, int numaNode, bool softEnabled);
    // Create a object with specified 'id, core, socket, numaNode, softEnable' attributes. The behavior is defined
    // provided all integer arguments are greater than or equal to zero. Note that 'softEnabled' has no meaning or
    // reference to H/W. It's meaning is best seen in context; see description. Also note this class does not attempt
    // to determine if the reference HW actually exists.

  VCPU();
    // Create VCPU object with default values zero and 'softEnabled=true'.

  VCPU(const VCPU& object) = default;
    // Create VCPU object such *this==object

  ~VCPU() = default;
    // Destroy this object

  // ACCESSORS
  int id() const;
    // Return id attribute value

  int core() const;
    // Return core attribute value

  int socket() const;
    // Return socket attribute value

  int numaNode() const;
    // Return NUMA node attribute value

  bool softEnabled() const;
    // Return soft disabled attribute value. True means VCPU was configured disabled or reserved for use elsewhere

  // MANIPULATORS
  VCPU& operator=(const VCPU& rhs) = default;
    // Create and return an object semantically equal to specified 'rhs'

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Print into specified 'stream' human readable dump of this object returning 'stream'
};

// FREE OPERATORS
std::ostream& operator<<(std::ostream& stream, const VCPU& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

bool operator==(const VCPU& lhs, const VCPU& rhs);
  // Return true if 'lhs, rhs' are semantically equal and false otherwise  

// INLINE FUNCTION DEFINITIONS
inline
VCPU::VCPU(int id, int core, int socket, int numaNode, bool softEnabled)
: d_id(id)
, d_core(core)
, d_socket(socket)
, d_numaNode(numaNode)
, d_softEnabled(softEnabled)
{
  assert(id>=0);
  assert(core>=0);
  assert(socket>=0);
  assert(numaNode>=0);
}

inline
VCPU::VCPU()
: d_id(0)
, d_core(0)
, d_socket(0)
, d_numaNode(0)
, d_softEnabled(false)
{
}

// ACCESSORS
inline
int VCPU::id() const {
  return d_id;
}

inline
int VCPU::core() const {
  return d_core;
}

inline
int VCPU::socket() const {
  return d_socket;
}

inline
int VCPU::numaNode() const {
  return d_numaNode;
}

inline
bool VCPU::softEnabled() const {
  return d_softEnabled;
}

// ASPECTS
inline
std::ostream& operator<<(std::ostream& stream, const VCPU& object) {
  return object.print(stream);
}

inline
bool operator==(const VCPU& lhs, const VCPU& rhs) {
  return  ( lhs.id()==rhs.id()                      &&
            lhs.core()==rhs.core()                  && 
            lhs.socket()==rhs.socket()              &&
            lhs.numaNode()==rhs.numaNode()          &&
            lhs.softEnabled()==rhs.softEnabled());
}

} // Dpdk
} // Reinvent
