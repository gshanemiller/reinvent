#pragma once

#include <dpdk/reinvent_dpdk_init.h>                                                                                    
#include <dpdk/reinvent_dpdk_util.h>                                                                                    

#include <time.h>

struct Message {                                                                                                      
  int      lcoreId;   // lcoreId which sent this message                                                                
  int      txqId;     // txqId which sent this message                                                                  
  unsigned sequence;  // txqId's sequence number                                                                        
  char     pad[20];   // to fill out message to 32 bytes                                                                
};          

extern volatile int terminate;

int clientEntryPoint(std::string& envPrefix, Reinvent::Util::Environment& env, Reinvent::Dpdk::Config& config);

int serverEntryPoint(std::string& envPrefix, Reinvent::Util::Environment& env, Reinvent::Dpdk::Config& config);

uint64_t timeDifference(timespec start, timespec end);
