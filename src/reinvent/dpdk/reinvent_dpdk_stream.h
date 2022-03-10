#pragma once

//
// Purpose: Pretty print selected DPDK structures to log
//

//                                                                                                                      
// Tell GCC to not enforce '-Wpendantic' for DPDK headers                                                               
//                                                                                                                      
#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wpedantic"                                                                             
#include <rte_ethdev.h>
#pragma GCC diagnostic pop 

#include <iostream>                                                                                                     

namespace Reinvent {
namespace Dpdk {

// FREE OPERATORS
std::ostream& operator<<(std::ostream& stream, const rte_eth_link& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

std::ostream& operator<<(std::ostream& stream, const rte_eth_dev_info& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

std::ostream& operator<<(std::ostream& stream, const rte_ether_addr& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

std::ostream& operator<<(std::ostream& stream, const rte_ether_addr& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

std::ostream& operator<<(std::ostream& stream, const rte_eth_txconf& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

std::ostream& operator<<(std::ostream& stream, const rte_eth_rxconf& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

std::ostream& operator<<(std::ostream& stream, const rte_eth_rss_conf& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

} // Dpdk
} // Reinvent
