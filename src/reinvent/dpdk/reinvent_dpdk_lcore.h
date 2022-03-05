#pragma once

// Purpose: Lcore Roles and responsiblities
//
// Classes:
//  Dpdk::LCORE: Value semantic type holding DPDK lcore role assignments with references to VCPU running it
// See Also:                                                                                                            
//  Dpdk::InitAWS                                                                                                       
//  Dpdk::AWSEnaConfig 
//  
// Thread Safety: Not MT thread-safe.
//                                                                                                                      
// Exception Policy: No exceptions                                                                                      
//
// Description: LCORE is a value semantic object holding a caller provider ID, its association to the VCPU running it
// and, depending on its function, references to information on the lcore's roles and responsibilities. Like VCPU this
// class makes no attempt to verify accuracy of provided data. That's typically done by caller ex. Dpdk::InitAWS.
//
// DPDK jargon 'lcore' has a dotted-line connection to VPCU. They are not the same thing, however, if one creates a
// pthread pinned to a particular core, say VCPU-5 (aka NUMA node 1, socket 1, CPU core 13) we can loosely construe
// lcore 5, and VCPU-5 as the same thing. As always if you're clear on the loseness, then all's good. But if your not
// the following points may help:
//                                                                                                                      
// * a bare metal box always has H/W cores, but an lcore cannot exist until a task runs, creates a thread, and the
//   thread is pinned to a specific core for DPDK use. Lcores are per task runtime DPDK entities.            
// * the desigination 'lcore-5' or '...suppose an lcore transmits...' out of context is amigbuous. 'lcore-5' alone does
//   not explicitly provide a NUMA, CPU socket, or CPU core. One doesn't know where it was pinned. We don't know the
//   threadId. If, however 5 means VCPU-5 then this is less unambiguous. We can find a VCPU object with 'id=5' and find   
//   what H/W it the thread was pinned. That, however, is not the most common way lcores are referenced in DPDK. See
//   next.
// * Distinct lcores can be pinned to the same HW CPU core. Readers, to avoid confusion, will have no alternative but
//   to fallback to definition --- a thread pinned to a HW core --- then go back to the LCORE creator to find the
//   HW core, CPU, socket etc. to which it was pinned.   
//
// The lcore's pthreadId is not provided as an attribute for two reasons. First, one cannot know the threadId until
// after initialization is done, and 'rte_eal_mp_remote_launch()' is run. Second, there's not much you can do with the
// threadId. Given than an lcore is basically a highly managed DPDK artifact, one can't stop it, change its affinity,
// or much else without damaging DPDK. DPDK manages lcore thread-local data. You're off better off using DPDK APIs once
// the lcore is running when needed.
 
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | Field                 | Type          | Default    | Comment                                                     |
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | id                    | int           | 0          | Name or number of lcore                                     |
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | vcpuId                | int           | 0          | Name or number of VCPU running this lcore i.e. to which this|
// |                       |               |            | lcore is pinned                                             |
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | role                  | vector<int>   | empty      | An lcore may have up to 'role.size()' roles. The ith        |
// |                       |               |            | assigment is role[i] and gives a flag whose  value is       |
// |                       |               |            | exactly one of the LCORE::enum constants.                   |
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | queue                 | vector<int>   | empty      | If there is a linking queue ID for role[i], it is given by  |
// |                       |               |            | queue[i] or k_LCORE_UNDEF if unknown. For example, if       | 
// |                       |               |            | role[0]&e_LCORE_RXQ then the value queue[0] will give the   |
// |                       |               |            | RXQ it's assigned. See AWSEnaConfig where most of           |
// |                       |               |            | this reference data is kept.                                |
// +-----------------------+---------------+------------+-------------------------------------------------------------+

#include <vector>
#include <iostream>
#include <assert.h>

namespace Reinvent {
namespace Dpdk {

class LCORE {
public:
  // ENUMS
  enum {
    e_LCORE_RXQ     = (1<<0),   // lcore functions in RX queue capacity
    e_LCORE_TXQ     = (1<<1),   //    . . .           TX   . . .
    k_LCORE_UNDEF   = (1<<31),  // no assigned role or unknown role
  };

private:
  // DATA
  int               d_id;
  int               d_vcpuId;
  std::vector<int>  d_role;
  std::vector<int>  d_queue;

public:
  // CREATORS
  LCORE(int id, int vcpuId);
    // Create a object with specified 'id, vcpuId' attributes and empty 'roleAssigment, queueAssignment'. The behavior
    // is provided all arguments are greater than or equal to zero. 

  LCORE();
    // Create LCORE object with default values

  LCORE(const LCORE& object);
    // Create LCORE object such *this==object

  ~LCORE() = default;
    // Destroy this object

  // ACCESSORS
  int id() const;
    // Return id attribute value

  int vcpuId() const;
    // Return vcpId attribute value

  const std::vector<int>& role() const;
    // Return non-modifiable reference to role attribute

  const std::vector<int>& queue() const;
    // Return non-modifiable reference to role attribute

  // MANIPULATORS
  int addQueueRole(int role, int queueId);
    // Return 0 if specified 'role' was appended to role attribute, and specified 'queueId' was appended to 'queue'
    // and non-zero otherwise. The behavior is provided 'role' corresponds to exactly one of the defined enum values.

  LCORE& operator=(const LCORE& rhs);
    // Create and return an object semantically equal to specified 'rhs'

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Print into specified 'stream' human readable dump of this object returning 'stream'
};

// FREE OPERATORS
std::ostream& operator<<(std::ostream& stream, const LCORE& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

bool operator==(const LCORE& lhs, const LCORE& rhs);
  // Return true if 'lhs, rhs' are semantically equal and false otherwise  

// INLINE FUNCTION DEFINITIONS
inline
LCORE::LCORE(int id, int vcpuId) 
: d_id(id)
, d_vcpuId(vcpuId)
{
  assert(d_id>=0);
  assert(d_vcpuId>=0);
  assert(d_role.empty());
  assert(d_queue.empty());
}

inline
LCORE::LCORE(const LCORE& object) {
  if (this==&object) {
    return;
  }

  d_role.clear();
  d_queue.clear();

  d_id      = object.d_id;
  d_vcpuId  = object.d_vcpuId;
  d_role    = object.d_role;
  d_queue   = object.d_queue;
}

inline
LCORE::LCORE()
: d_id(0)
, d_vcpuId(0)
{
  assert(d_id>=0);
  assert(d_vcpuId>=0);
  assert(d_role.empty());
  assert(d_queue.empty());
}

// ACCESSORS
inline
int LCORE::id() const {
  return d_id;
}

inline
int LCORE::vcpuId() const {
  return d_vcpuId;
}

inline
const std::vector<int>& LCORE::role() const {
  return d_role;
}

inline
const std::vector<int>& LCORE::queue() const {
  return d_queue;
}

// MANIPULATORS
inline
int LCORE::addQueueRole(int role, int queueId)  {                                                                             
  // callers must choose one role per call here
  assert(1==__builtin_popcount(static_cast<unsigned>(role)));
  d_role.push_back(role);
  d_queue.push_back(queueId);
  assert(d_role.size()==d_queue.size());
  return 0;
}

inline
LCORE& LCORE::operator=(const LCORE& rhs) {
  if (this==&rhs) {
    return *this;
  }

  d_role.clear();
  d_queue.clear();

  d_id      = rhs.d_id;
  d_vcpuId  = rhs.d_vcpuId;
  d_role    = rhs.d_role;
  d_queue   = rhs.d_queue;

  return *this;
}

// ASPECTS
inline
std::ostream& operator<<(std::ostream& stream, const LCORE& object) {
  return object.print(stream);
}

inline
bool operator==(const LCORE& lhs, const LCORE& rhs) {
  return  ( lhs.id()==rhs.id()                      &&
            lhs.vcpuId()==rhs.vcpuId()              && 
            lhs.role()==rhs.role()                  &&
            lhs.queue()==rhs.queue());
}

} // Dpdk
} // Reinvent
