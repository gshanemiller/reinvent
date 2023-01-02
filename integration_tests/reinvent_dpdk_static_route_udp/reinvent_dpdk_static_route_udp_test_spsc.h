#pragma once

// PURPOSE: Multi-threaded safe ring-buffer supporting 'append' for writers, and 'read' for readers based on
//          work from https://rigtorp.se/ringbuffer/
//
// CLASSES: RingBuffer::SPSC: Single producer (SP) single consumer (SC) buffer with fixed capacity k_SPSC_CAPACITY
//                            operating on objects of type 'RingBuffer::Op'

#include <atomic>
#include <array>
#include <assert.h>

//                                                                                                                      
// Tell GCC to not enforce '-Wpendantic' for DPDK headers                                                               
//                                                                                                                      
#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wpedantic"                                                                             
#include <rte_mbuf.h>                                                                                             
#pragma GCC diagnostic pop                                                                                              

namespace RingBuffer {

const uint64_t k_SPSC_CAPACITY = 32;

struct SPSC {
  // DATA
  alignas(64) std::atomic<size_t>                   d_readIdx;
  alignas(64) size_t                                d_readIdxCached;
  alignas(64) std::atomic<size_t>                   d_writeIdx;
  alignas(64) size_t                                d_writeIdxCached;
  alignas(64) std::array<rte_mbuf*,k_SPSC_CAPACITY> d_data;

public:
  // CREATORS
  SPSC();
    // Construct SPSC (single-producer, single-consumer) queue operating on 'Op' objects with fixed capacity
    // k_SPSC_CAPACITY.

  SPSC(const SPSC& other);
    // Copy constructor not provided

  ~SPSC() = default;
    // Destroy this object

  // MANIPULATORS
  bool append(rte_mbuf *value);
    // Return true if specified 'value' was appended to queue and false. Note that false is returned when queue full.
    // Behavior is defined provided the producer thread only calls 'append'.

  bool read(rte_mbuf **value);
    // Return true if next object for reader was copied into specified 'value' and false othewise. Note that false
    // means queue empty or there's no current object to read e.g. reader already end of queue. Behavior is defined
    // provided the consumer thread only calls 'read'.

  SPSC& operator=(const SPSC& rhs) = delete;
    // Assignment operator not provided
};

// INLINE DEFINITIONS
// CREATORS
inline
SPSC::SPSC()
: d_readIdx(0)
, d_readIdxCached(0)
, d_writeIdx(0)
, d_writeIdxCached(0)
{
}

// MANIPULATORS
inline
bool SPSC::append(rte_mbuf *value) {
  auto const writeIdx = d_writeIdx.load(std::memory_order_relaxed);
  auto nextWriteIdx = d_writeIdx + 1;
  if (nextWriteIdx == k_SPSC_CAPACITY) {
    nextWriteIdx = 0;
  }
  if (nextWriteIdx == d_readIdxCached) {
    d_readIdxCached = d_readIdx.load(std::memory_order_acquire);
    if (nextWriteIdx == d_readIdxCached) {
        return false;
    }
  }
  d_data[writeIdx] = value;
  d_writeIdx.store(nextWriteIdx, std::memory_order_release);
  return true;
}

inline
bool SPSC::read(rte_mbuf **value) {
  auto const readIdx = d_readIdx.load(std::memory_order_relaxed);
  if (readIdx == d_writeIdxCached) {
    d_writeIdxCached = d_writeIdx.load(std::memory_order_acquire);
    if (d_readIdx == d_writeIdxCached) {
      return false;
    }
  }
  *value = d_data[readIdx];
  auto nextReadIdx = d_readIdx + 1;
  if (nextReadIdx == k_SPSC_CAPACITY) {
    nextReadIdx = 0;
  }
  d_readIdx.store(nextReadIdx, std::memory_order_release);
  return true;
}

}
