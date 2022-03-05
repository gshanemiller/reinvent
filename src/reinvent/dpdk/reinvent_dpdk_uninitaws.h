#pragma once

// Purpose: Uninitialize DPDK ENA AWS subsystem
//
// Classes:
//  Dpdk::UnInitAWS: Shutdown DPDK for all ENA devices
//  
// Thread Safety: Not MT thread-safe.
//                                                                                                                      
// Exception Policy: No exceptions                                                                                      

#include <dpdk/reinvent_dpdk_awsconfig.h>

namespace Reinvent {
namespace Dpdk {

class UnInitAWS {
public:
  // PUBLIC MANIPULATORS
  static int device(const AWSEnaConfig& config);
    // Return 0 if the DPDK device in the specified 'config' is stopped, and non-zero otherwise. Note this method
    // should be run before 'ena' below.

  static int ena();
    // Return 0 if the DPDK ENA subsystem is shutdown, and non-zero otherwise. Call this method once only before exit
    // even if 'Dpdk::InitAWS::ena' returned an error. The behavior of DPDK is undefined once this method returns.
};

} // Dpdk
} // Reinvent
