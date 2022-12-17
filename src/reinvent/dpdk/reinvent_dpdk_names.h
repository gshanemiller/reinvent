# pragma once

#include <string>

namespace Reinvent {
namespace Dpdk {

struct Names {
  // ENUM
  enum {
    MAX_VARIABLE_NAME_SIZE_BYTES = 64
  };

  // STATIC DATA
  static const char *OFF;
  static const char *MAX;
  static const char *MULTI;
  static const char *DISTINCT;
  static const char *NUMA_NODE;
  static const char *QUEUE_SIZE;
  static const char *RX_QUEUE_SIZE;
  static const char *TX_QUEUE_SIZE;
  static const char *RXQ_THREAD_COUNT;
  static const char *TXQ_THREAD_COUNT;
  static const char *RXQ_VCPU_MASK;
  static const char *TXQ_VCPU_MASK;
  static const char *RXQ_VCPU_POLICY;
  static const char *TXQ_VCPU_POLICY;
  static const char *RXQ_RING_SIZE;
  static const char *TXQ_RING_SIZE;
  static const char *VCPU;
  static const char *VCPU_MAX;
  static const char *IN_CORE;
  static const char *IN_SOCKET;
  static const char *ON_NUMA_NODE;
  static const char *PCI_DEVICE_ID;
  static const char *MEMZONE;
  static const char *DRAM_MEMORY_CHANNELS;
  static const char *DPDK_INITIALIZATION_PARAMETERS;

  static const char *MEMZONE_RESERVE_KB;
  static const char *MEMZONE_RESERVE_MASK;
  static const char *MEMZONE_RESERVE_NAME;

  static const char *LINK_SPEED_MASK;
  static const char *RX_MTU;
  static const char *RX_MQ_MASK;
  static const char *RX_OFFLOAD_MASK;
  static const char *TX_MQ_MASK;
  static const char *TX_OFFLOAD_MASK;

  static const char *RX_RSS_KEY;
  static const char *RX_RSS_HF;

  static const char *ALL;
  static const char *SHARED;
  static const char *PER_QUEUE;
  static const char *SOFT_ENABLED;
  static const char *MEMPOOL_POLICY;
  
  static const char *MEMPOOL_RXQ_SIZE;
  static const char *MEMPOOL_TXQ_SIZE;
  static const char *MEMPOOL_RXQ_CACHE_SIZE;
  static const char *MEMPOOL_TXQ_CACHE_SIZE;
  static const char *MEMPOOL_RXQ_PRIV_SIZE;
  static const char *MEMPOOL_TXQ_PRIV_SIZE;
  static const char *MEMPOOL_RXQ_DATA_ROOM_SIZE;
  static const char *MEMPOOL_TXQ_DATA_ROOM_SIZE;

  static const char *SHARED_MEMPOOL_SIZE;
  static const char *SHARED_MEMPOOL_ELT_SIZE;
  static const char *SHARED_MEMPOOL_CACHE_SIZE;
  static const char *SHARED_MEMPOOL_PRIVATE;
  static const char *SHARED_MEMPOOL_FLAGS;

  static const char *TX_DEFAULT_FLOW;
  static const char *TXQ_DEFAULT_SRC_MAC;
  static const char *TXQ_DEFAULT_DST_MAC;
  static const char *TXQ_DEFAULT_SRC_IP;
  static const char *TXQ_DEFAULT_DST_IP;
  static const char *TXQ_DEFAULT_SRC_PORT;
  static const char *TXQ_DEFAULT_DST_PORT;

  // STATIC METHODS
  static void make(std::string *name, const char *format, ...);
    // Construct an environment variable name writing the result into specified 'name' by sprintfing any supplied
    // arguments against specified printf-style 'format'. Note the resulting variable is truncated to at most
    // MAX_VARIABLE_NAME_SIZE_BYTES-1 characters.

  static void make(const std::string& prefix, std::string *name, const char *format, ...);
    // Construct an environment variable name writing the result into specified 'name'. First, 'name' is initialized
    // to specified 'prefix'. If prefix is non empty an underscore is appended. Then all supplied arguments are sprintf
    // appended to the end of 'name' via specified printf-style 'format'. Note the resulting variable is truncated to
    // at most MAX_VARIABLE_NAME_SIZE_BYTES-1 characters.
};

} // Dpdk
} // Reinvent
