#pragma once

// Purpose: Hold RXQ ids, mempool resources
//
// Classes:
//  Dpdk::RXQ: Holds configuration for a DPDK RXQ primarily mempool with links to its VCPU, lcore.
//
// Thread Safety: Not MT thread-safe.
//                                                                                                                      
// Exception Policy: No exceptions                                                                                      
//
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | Field                 | Type          | Default    | Comment                                                     |
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | id                    | int           | 0          | Name or number of this RXQ                                  |
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | vcpuId                | int           | 0          | Name or number of VCPU running this RXQ i.e. the VCPU to    |
// |                       |               |            | which the thread running this queue is pinned.              |
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | lcoreId               | int           | 0          | Name or number of the lcore running this RXQ                |
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | memzone               | rte_memzone * | 0          | the memzone from which this RXQ's mempool was allocated from|
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | mempool               | rte_mempool * | 0          | the mempool to be used for this RXQ                         |
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | ringSize              | int           | 0          | the number of mbufs from mempool that are reserved or used  |
// |                       |               |            | the NIC hardware to write data into after reading it off the|
// |                       |               |            | wire.                                                       |
// +-----------------------+---------------+------------+-------------------------------------------------------------+
// | mempoolPolicy         | std::string   | empty      | One of PER_QUEUE|SHARED. This is a hint on how the RXQ's    |
// |                       |               |            | mempool was created.                                        |
// +-----------------------+---------------+------------+-------------------------------------------------------------+

#include <iostream>
#include <assert.h>
#include <string>

//                                                                                                                      
// Tell GCC to not enforce '-Wpendantic' for DPDK headers                                                               
//                                                                                                                      
#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wpedantic"                                                                             
#include <rte_memzone.h>                                                                                                
#include <rte_mempool.h>                                                                                                
#pragma GCC diagnostic pop 

namespace Reinvent {
namespace Dpdk {

class RXQ {
  // DATA
  int                 d_id;
  int                 d_vcpuId;
  int                 d_lcoreId;
  const rte_memzone  *d_memzone;
  rte_mempool        *d_mempool;
  int                 d_ringSize;
  std::string         d_mempoolPolicy;

public:
  // CREATORS
  RXQ(int id, int vcpuId, int lcoreId, const rte_memzone *memzone, rte_mempool *mempool, int ringSize,
    const std::string& mempoolPolicy);
    // Create a object with specified parameters. The behavior is defined provided integer arguments are greater than
    // or equal, and ringSize>0
  
  RXQ(int id, int vcpuId, int lcoreId);
    // Create a object with specified parameters. The behavior is defined provided integer arguments are greater than
    // or equal to zero. Attributes not appearing in function list are defaulted.

  RXQ();
    // Create RXQ object with default values

  RXQ(const RXQ& object);
    // Create an object whose attributes are identical to specified 'object' except that 'memzone, mempool' attributes   
    // are the zero pointer.

  ~RXQ() = default;
    // Destroy this object

  // ACCESSORS
  int id() const;
    // Return id attribute value

  int vcpuId() const;
    // Return vcpId attribute value

  int lcoreId() const;
    // Return lcoreId attribute value

  const rte_memzone *memzone() const;
    // Return memzone attribute

  rte_mempool *mempool() const;
    // Return mempool attribute

  int ringSize() const;
    // Return ring size attribute

  const std::string& memzonePolicy() const;
    // Return mempool policy attribute

  // MANIPULATORS
  RXQ& operator=(const RXQ& rhs) = delete;
    // Assignment operator not provided
 
  void setMemzone(const rte_memzone *value);
    // Assign the specified 'value' to the memzone attribute

  void setMempool(rte_mempool *value);
    // Assign the specified 'value' to the mempool attribute

  void setRingSize(int value);
    // Assign the specified 'value>=0' to the ringSize attribute

  void setMempoolPolicy(const std::string& value);
    // Assign the specified 'value' to the mempoolPolicy attribute

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Print into specified 'stream' human readable dump of this object returning 'stream'
};

// FREE OPERATORS
std::ostream& operator<<(std::ostream& stream, const RXQ& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

// INLINE FUNCTION DEFINITIONS
inline
RXQ::RXQ(int id, int vcpuId, int lcoreId, const rte_memzone *memzone, rte_mempool *mempool, int ringSize,
  const std::string& mempoolPolicy)
: d_id(id)
, d_vcpuId(vcpuId)
, d_lcoreId(lcoreId)
, d_memzone(memzone)
, d_mempool(mempool)
, d_ringSize(ringSize)
, d_mempoolPolicy(mempoolPolicy)
{
  assert(d_id>=0);
  assert(d_vcpuId>=0);
  assert(d_lcoreId>=0);
  assert(d_memzone);
  assert(d_mempool);
  assert(d_ringSize>0);
  assert(!d_mempoolPolicy.empty());
}

inline                                                                                                                  
RXQ::RXQ(int id, int vcpuId, int lcoreId)
: d_id(id)                                                                                                              
, d_vcpuId(vcpuId)                                                                                                      
, d_lcoreId(lcoreId)                                                                                                    
, d_memzone(0)
, d_mempool(0)
, d_ringSize(1)
{                                                                                                                       
  assert(d_id>=0);                                                                                                      
  assert(d_vcpuId>=0);                                                                                                  
  assert(d_lcoreId>=0);                                                                                                 
  assert(d_memzone==0);                                                                                                 
  assert(d_mempool==0);                                                                                                 
  assert(d_ringSize>0);                                                                                                 
  assert(d_mempoolPolicy.empty());                                                                                     
}

inline
RXQ::RXQ()
: d_id(0)
, d_vcpuId(0)
, d_lcoreId(0)
, d_memzone(0)
, d_mempool(0)
, d_ringSize(0)
{
}

inline                                                                                                                  
RXQ::RXQ(const RXQ& object)
: d_id(object.d_id)
, d_vcpuId(object.d_vcpuId)
, d_lcoreId(object.d_lcoreId)
, d_memzone(0)
, d_mempool(0)
, d_ringSize(object.d_ringSize)
, d_mempoolPolicy(object.d_mempoolPolicy)
{                                                                                                                       
} 

// ACCESSORS
inline
int RXQ::id() const {
  return d_id;
}

inline
int RXQ::vcpuId() const {
  return d_vcpuId;
}

inline
int RXQ::lcoreId() const {
  return d_lcoreId;
}

inline
const rte_memzone *RXQ::memzone() const {
  return d_memzone;
}

inline
rte_mempool *RXQ::mempool() const {
  return d_mempool;
}

inline
int RXQ::ringSize() const {
  return d_ringSize;
}

inline
const std::string& RXQ::memzonePolicy() const {
  return d_mempoolPolicy;
}

// MANIPULATORS
inline
void RXQ::setMemzone(const rte_memzone *value) {
  assert(value);
  d_memzone = value;
}

inline
void RXQ::setMempool(rte_mempool *value) {
  assert(value);
  d_mempool = value;
}

inline
void RXQ::setRingSize(int value) {
  assert(value>0);
  d_ringSize = value;
}

inline
void RXQ::setMempoolPolicy(const std::string& value) {
  d_mempoolPolicy = value;
}

// ASPECTS
inline
std::ostream& operator<<(std::ostream& stream, const RXQ& object) {
  return object.print(stream);
}

} // Dpdk
} // Reinvent
