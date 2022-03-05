#pragma once

// Purpose: Conversion helpers to put packet data into correct byte order

//                                                                                                                      
// Tell GCC to not enforce '-Wpendantic' for DPDK headers                                                               
//                                                                                                                      
#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wpedantic"                                                                             
#include <rte_ether.h>
#include <rte_byteorder.h>
#pragma GCC diagnostic pop                                                                                              

#if RTE_BYTE_ORDER == RTE_BIG_ENDIAN
#error Reinvent library assumes bare metal hardware is Intel x86 little-endian
#endif

namespace Reinvent {
namespace Dpdk {

union MacAddressUnion {
  uint64_t              d_uint;
  struct rte_ether_addr d_addr;
};

} // Dpdk
} // Reinvent
