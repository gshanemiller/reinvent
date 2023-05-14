#pragma once

#include <dpdk/reinvent_dpdk_init.h>                                                                                    
#include <dpdk/reinvent_dpdk_util.h>                                                                                    
#include <dpdk/reinvent_dpdk_worker.h>

#include <reinvent_dpdk_static_route_udp_test_spsc.h>

#include <time.h>

struct Message {                                                                                                      
  int      lcoreId;   // lcoreId which sent this message                                                                
  int      txqId;     // txqId which sent this message                                                                  
  unsigned sequence;  // txqId's sequence number                                                                        
  char     pad[20];   // to fill out message to 32 bytes                                                                
};          

class ExtWorker: public Reinvent::Dpdk::Worker {
  ExtWorker() = delete;
    // Default constructor not provided

  ExtWorker(const std::string& prefix, Reinvent::Util::Environment& env,
    const Reinvent::Dpdk::Config& config);
    // Create Worker with specified 'envPrefix, env, config'

  ExtWorker(const Worker& other) = delete;
    // Copy constructor not provided

  virtual ~ExtWorker();
    // Destroy this object
};

extern volatile int terminate;

int clientEntryPoint(std::string& envPrefix, Reinvent::Util::Environment& env, Reinvent::Dpdk::Config& config);

int serverEntryPoint(std::string& envPrefix, Reinvent::Util::Environment& env, Reinvent::Dpdk::Config& config);

uint64_t timeDifference(timespec start, timespec end);
