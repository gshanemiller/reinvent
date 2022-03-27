#include <dpdk/reinvent_dpdk_initaws.h>
#include <dpdk/reinvent_dpdk_stream.h>
#include <util/reinvent_util_errno.h>

#include <map>
#include <string.h>                                                                                                     
#include <assert.h>

//                                                                                                                      
// Tell GCC to not enforce '-Wpendantic' for DPDK headers                                                               
//                                                                                                                      
#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wpedantic"                                                                             
#include <rte_flow.h>                                                                                                
#include <rte_bus_pci.h>
#pragma GCC diagnostic pop 

namespace Reinvent {

int Dpdk::InitAWS::configTxRouting(const std::string& prefix, Util::Environment *env, AWSEnaConfig *config,                
  std::vector<int>& defaultTxSrcPort, std::vector<int>& defaultTxDstPort, std::string& defaultTxSrcMac,  
  std::vector<std::string>& defaultTxDstMac, std::string& defaultTxSrcIp, std::vector<std::string>& defaultTxDstIp,
  std::vector<UDPRoute>& defaultTxRoute) {

  assert(env);
  assert(config);

  int rc, sz;
  bool valid;
  std::string value;
  std::string variable;

  //
  // Get TX src mac addresses
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_DEFAULT_SRC_MAC);
  if ((rc = env->valueAsString(variable, &defaultTxSrcMac))!=0) {
    return rc;
  }

  //
  // Get TX dst mac addresses
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_DEFAULT_DST_MAC);
  if ((rc = env->valueAsStringList(variable, &defaultTxDstMac))!=0) {
    return rc;
  }
  sz = static_cast<int>(defaultTxDstMac.size());                                                                      
  valid = sz==config->txqThreadCount();
  if (!valid) {                                                                                                       
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),                                 
      variable.c_str(), config->txqThreadCount(), sz, "values were provided: must be equal");                         
  } 

  //
  // Get TX src IP addresses
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_DEFAULT_SRC_IP);
  if ((rc = env->valueAsString(variable, &defaultTxSrcIp))!=0) {
    return rc;
  }

  //
  // Get TX dst IP addresses
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_DEFAULT_DST_IP);
  if ((rc = env->valueAsStringList(variable, &defaultTxDstIp))!=0) {
    return rc;
  }
  sz = static_cast<int>(defaultTxDstIp.size());                                                                      
  valid = sz==config->txqThreadCount();
  if (!valid) {                                                                                                       
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),                                 
      variable.c_str(), config->txqThreadCount(), sz, "values were provided: must be equal");                         
  }

  //
  // Get TX src port numbers
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_DEFAULT_SRC_PORT);
  if ((rc = env->valueAsIntList(variable, &defaultTxSrcPort))!=0) {
    return rc;
  }
  sz = static_cast<int>(defaultTxSrcPort.size());                                                                      
  valid = sz==config->txqThreadCount();
  if (!valid) {                                                                                                       
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),                                 
      variable.c_str(), config->txqThreadCount(), sz, "values were provided: must be equal");                         
  } 

  //
  // Get TX dst port numbers
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_DEFAULT_DST_PORT);
  if ((rc = env->valueAsIntList(variable, &defaultTxDstPort))!=0) {
    return rc;
  }
  sz = static_cast<int>(defaultTxDstPort.size());                                                                      
  valid = sz==config->txqThreadCount();
  if (!valid) {                                                                                                       
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),                                 
      variable.c_str(), config->txqThreadCount(), sz, "values were provided: must be equal");                         
  } 

  //
  // Make and verify default TX routes
  // 
  for (unsigned i=0; i<defaultTxDstMac.size(); ++i) {
    UDPRoute route(defaultTxSrcMac, defaultTxDstMac[i], defaultTxSrcIp, defaultTxDstIp[i],
        defaultTxSrcPort[i], defaultTxDstPort[i]);

    uint32_t ipAddr;
    rte_ether_addr binMac;

    if ((rc = route.convertSrcMac(&binMac))!=0) {
      Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_DEFAULT_SRC_MAC);
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (rc==0),
        variable.c_str(), defaultTxSrcMac.c_str(), "is not a valid MAC address");
    }

    if ((rc = route.convertDstMac(&binMac))!=0) {
      Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_DEFAULT_DST_MAC);
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (rc==0),
        variable.c_str(), defaultTxDstMac[i].c_str(), "is not a valid MAC address");
    }

    if ((rc = route.convertSrcIp(&ipAddr))!=0) {
      Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_DEFAULT_SRC_IP);
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (rc==0),
        variable.c_str(), defaultTxSrcIp.c_str(), "is not a valid IPV4 address");
    }

    if ((rc = route.convertDstIp(&ipAddr))!=0) {
      Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_DEFAULT_DST_IP);
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (rc==0),
        variable.c_str(), defaultTxDstIp[i].c_str(), "is not a valid IPV4 address");
    }

    defaultTxRoute.push_back(route);
  }

  return 0;
}

int Dpdk::InitAWS::makeDpdkInitParams(const std::string& envPrefix, Util::Environment *env, AWSEnaConfig *config) {
  assert(env);
  assert(config);

  //
  // DPDK initializtion partly depends on arguments passed to 'rte_eal_init'
  //
  // See: https://doc.dpdk.org/guides/linux_gsg/linux_eal_parameters.html
  //      https://doc.dpdk.org/api/rte__log_8h.html
  //

  int rc;
  char *context, *token;                                                                                                
  std::string variable, value;
  const char commaDelimiter[]=",";                                                                                      

  // Load possibly empty DPDK init params from env
  Dpdk::Names::make(envPrefix, &variable, "%s", Dpdk::Names::DPDK_INITIALIZATION_PARAMETERS);
  if ((rc = env->valueAsString(variable, &value, false))!=0) {
    return rc;
  }

  // Get DPDK vector holding arguments and clear it
  std::vector<std::string>& dpdkArgs = config->dpdkArgs();
  dpdkArgs.clear();

  // Now parse out comma separated argument (if any)
  for (token = strtok_r(const_cast<char*>(value.c_str()), commaDelimiter, &context); token;                              
    token = strtok_r(0, commaDelimiter, &context)) {  
    dpdkArgs.push_back(token);
  }

  // Append memory channel config
  Dpdk::Names::make(envPrefix, &variable, "%s", Dpdk::Names::DRAM_MEMORY_CHANNELS);
  if ((rc = env->valueAsString(variable, &value))!=0) {
    return rc;
  }
  dpdkArgs.push_back("-n");
  dpdkArgs.push_back(value);

  // Append lcore-vcpu map in form '(<lcoreIdSet>)@<vcpuId>,...'. '<lcoreIdSet>' is a comma seperated
  // set of lcoreIds. In the this map key is 'vcpuId'; value are all lcores pinned on key
  std::map<int, std::vector<int> > map;
  for (unsigned i=0; i<config->lcore().size(); ++i) {
    auto iter = map.find(config->lcore()[i].vcpuId());
    if (iter==map.end()) {
      std::vector<int> list;
      list.push_back(config->lcore()[i].id());
      map[config->lcore()[i].vcpuId()] = list;
    } else {
      iter->second.push_back(config->lcore()[i].id());
    }
  }

  std::string lcoreValue;
  for (auto iter=map.begin(); iter!=map.end(); ++iter) {
    std::string token("(");
    for (unsigned i=0; i<iter->second.size(); ++i) {
      token.append(std::to_string(iter->second[i]));
      if ((i+1)!=iter->second.size()) {
        token.append(",");
      }
    }
    token.append(")@");
    token.append(std::to_string(iter->first));
    token.append(",");
    lcoreValue.append(token);
  }

  if (lcoreValue.length()) {
    // Remove trailing ','
    lcoreValue.pop_back();
  }

  dpdkArgs.push_back("--lcores");
  dpdkArgs.push_back(lcoreValue);

  return 0;
}

int Dpdk::InitAWS::makeAssignment(AWSEnaConfig *config, std::vector<RXQ>& rxq, std::vector<TXQ>& txq,
  std::vector<LCORE>& lcore) {
  assert(config);
  assert(config->vcpu().size()!=0);

  const std::vector<Dpdk::VCPU>& vcpu(config->vcpu());

  std::vector<bool> notAssignable(config->vcpu().size());
  std::vector<bool> notRxAssignable(config->vcpu().size());
  std::vector<bool> notTxAssignable(config->vcpu().size());
  std::vector<int>  assignmentCount(config->vcpu().size());

  // Figure out which VCPUs are not assignable                                                        
  for (std::size_t i=0; i<vcpu.size(); ++i) {
    assert(vcpu[i].id()==static_cast<int>(i));

    // If VCPU is soft-disabled, can't use
    if (!vcpu[i].softEnabled()) {
      notAssignable[i] = true;
    }
                                                                                                                        
    // If the VCPU is not on the same NUMA node as NIC, can't use
    if (vcpu[i].numaNode()!=config->numaNode()) {
      notAssignable[i] = true;
    }
                                                                                                                        
    // Mark as ineligble any VCPU not in RXQ list (O(N^2) slow)
    bool valid = false;
    for (std::size_t v1=0; v1<config->rxqCandidateVcpuList().size(); ++v1) {
      if (static_cast<std::size_t>(config->rxqCandidateVcpuList()[v1]) == i) {
        valid = true;
        break;
      }
    }
    if (!valid) {
      notRxAssignable[i] = true;
    }
                                                                                                                        
    // Mark as ineligble any VCPU not in TXQ list (O(N^2) slow)
    valid = false;
    for (std::size_t v1=0; v1<config->txqCandidateVcpuList().size(); ++v1) {
      if (static_cast<std::size_t>(config->txqCandidateVcpuList()[v1]) == i) {
        valid = true;
        break;
      }
    }
    if (!valid) {
      notTxAssignable[i] = true;
    }
  }

  bool assigned;        // True if found VCPU for queue
  int lcoreId(0);       // lcoreId
  int currPass(1);      // Incremented when all VCPUs seen
  int rxAssigned(0);    // Count of assigned RX queues
  int txAssigned(0);    // Count of assigned TX queues
  std::size_t cpu(0);   // Candidate VCPU

  // Do RX assignments unless policy is OFF
  if (config->rxqPolicy() != Dpdk::Names::OFF) {
    while (rxAssigned<config->rxqThreadCount() && rxAssigned<config->rxqCount() && currPass<=2) {
      for (assigned = false; cpu < vcpu.size(); ++cpu) {
        // Is the VCPU RX assignable?
        if (notAssignable[cpu] || notRxAssignable[cpu]) {
          continue;
        }
    
        // How many assignments have been made to VCPU?
        int count = assignmentCount[cpu];
        if (count>0) {
          if (currPass==1) {
            // Still haven't seen all VCPU in pass 1 so continue to
            // look for a free slot
            continue;
          } else if (currPass==2 && config->rxqPolicy()==Dpdk::Names::MULTI) {
            // We're in pass 2 but MULTI allowed
            assigned = true;
          } else {
            REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (assigned),
              "RXQ thread count", config->rxqThreadCount(), 0, "remaining RXQ assignable VCPUs are left");
          }
        } else {
          // Ideal case: got the VCPU to self
          assigned = true;
        }

        // If assigned a VCPU count it and move on
        if (assigned) {
          Dpdk::RXQ q(rxAssigned, cpu, lcoreId);
          rxq.push_back(q);
          Dpdk::LCORE l(lcoreId, cpu);
          l.addQueueRole(Dpdk::LCORE::e_LCORE_RXQ, rxAssigned);
          lcore.push_back(l);
          assignmentCount[cpu] = (count+1);
          ++lcoreId;
          ++cpu;
          ++rxAssigned;
          break;
        }
      }

      // Wrap around if needed
      if (cpu>=vcpu.size()) {
        cpu = 0;
        ++currPass;
      }
    }
  }

  // Make TXQ assignments unless configed OFF
  if (config->txqPolicy() != Dpdk::Names::OFF) {
    while (txAssigned<config->txqThreadCount() && txAssigned<config->txqCount() && currPass<=2) {
      for (assigned = false; cpu < vcpu.size(); ++cpu) {
        // Is the VCPU TX assignable?
        if (notAssignable[cpu] || notTxAssignable[cpu]) {
          continue;
        }
    
        // How many assignments have been made to VCPU?
        int count = assignmentCount[cpu];
        if (count>0) {
          if (currPass==1) {
            // Still haven't seen all VCPU in pass 1 so continue to
            // look for a free slot
            continue;
          } else if (currPass==2 && config->txqPolicy()==Dpdk::Names::MULTI) {
            // We're in pass 2 but MULTI allowed
            assigned = true;
          } else {
            REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (assigned),
              "TXQ thread count", config->txqThreadCount(), 0, "remaining TXQ assignable VCPUs are left");
          }
        } else {
          // Ideal case: got the VCPU to self
          assigned = true;
        }

        // If assigned a VCPU count it and move on
        if (assigned) {
          Dpdk::TXQ q(txAssigned, cpu, lcoreId);
          txq.push_back(q);
          assignmentCount[cpu] = (count+1);
          Dpdk::LCORE l(lcoreId, cpu);
          l.addQueueRole(Dpdk::LCORE::e_LCORE_TXQ, txAssigned);
          lcore.push_back(l);
          assignmentCount[cpu] = (count+1);
          ++lcoreId;
          ++cpu;
          ++txAssigned;
          break;
        }
      }

      // Wrap around if needed
      if (cpu>=vcpu.size()) {
        cpu = 0;
        ++currPass;
      }
    }
  }

  if (config->rxqPolicy() != Dpdk::Names::OFF) {
    if (rxAssigned!=config->rxqThreadCount()) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (rxAssigned==config->rxqThreadCount()),
        "RXQ thread count", config->rxqThreadCount(), 0, "remaining RXQ assignable VCPUs are left");
    }
  }

  if (config->txqPolicy() != Dpdk::Names::OFF) {
    if (txAssigned!=config->txqThreadCount()) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (txAssigned==config->txqThreadCount()),
        "TXQ thread count", config->txqThreadCount(), 0, "remaining TXQ assignable VCPUs are left");
    }
  }

  return 0;
}

int Dpdk::InitAWS::configRings(const std::string& prefix, Util::Environment *env, AWSEnaConfig *config,                       
    std::vector<int>& rxqRingSize, std::vector<int>& txqRingSize) {
  assert(env);
  assert(config);

  int val, rc;
  std::string variable;

  rxqRingSize.clear();
  txqRingSize.clear();

  if (config->rxqPolicy()!=Dpdk::Names::OFF) {
    //
    // Get RXQ ring size - try as int first
    //
    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::RXQ_RING_SIZE);
    rc = env->valueAsInt(variable, &val, true, 1);
    if (rc==0) {
      rxqRingSize.clear();
      for (int i=0; i<config->rxqThreadCount(); i++) {
        rxqRingSize.push_back(val);
      }
    } else {
      // Get RXQ ring size - try as int list
      rc = env->valueAsIntList(variable, &rxqRingSize, true, 1);
      if (rc!=0) {
        return rc;
      }
      int sz = static_cast<int>(rxqRingSize.size());
      bool valid = sz==config->rxqThreadCount();
      if (!valid) {
        REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),
          variable.c_str(), config->rxqThreadCount(), sz, "values were provided: must be equal");
      }
    }
  } else {
    rxqRingSize.clear();
    for (int i=0; i<config->rxqThreadCount(); i++) {
      rxqRingSize.push_back(0);
    }
  }

  if (config->txqPolicy()!=Dpdk::Names::OFF) {
    //
    //  Get TXQ ring size
    //    
    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_RING_SIZE);
    rc = env->valueAsInt(variable, &val, true, 1);
    if (rc==0) {
      txqRingSize.clear();
      for (int i=0; i<config->txqThreadCount(); i++) {
        txqRingSize.push_back(val);
      }
    } else {
      // Get TXQ ring size - try as int list
      rc = env->valueAsIntList(variable, &txqRingSize, true, 1);
      if (rc!=0) {
        return rc;
      }
      int sz = static_cast<int>(txqRingSize.size());
      bool valid = sz==config->txqThreadCount();
      if (!valid) {
        REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),
          variable.c_str(), config->txqThreadCount(), sz, "values were provided: must be equal");
      }
    }
  } else {
    txqRingSize.clear();
    for (int i=0; i<config->txqThreadCount(); i++) {
      txqRingSize.push_back(0);
    }
  }

  return 0;
}

int Dpdk::InitAWS::configPerQueueMempool(const std::string& prefix, Util::Environment *env, AWSEnaConfig *config,
  std::vector<int> *rxqMemPoolSize, std::vector<int> *txqMemPoolSize, std::vector<int> *rxqMemPoolCacheSize,          
  std::vector<int> *txqMemPoolCacheSize, std::vector<int> *rxqMemPoolPrivSize, std::vector<int> *txqMemPoolPrivSize,  
  std::vector<int> *rxqMemPoolDataRoomSize, std::vector<int> *txqMemPoolDataRoomSize) {
  assert(env);
  assert(config);
  assert(rxqMemPoolSize);
  assert(txqMemPoolSize);
  assert(rxqMemPoolCacheSize);
  assert(txqMemPoolCacheSize);
  assert(rxqMemPoolPrivSize);
  assert(txqMemPoolPrivSize);
  assert(rxqMemPoolDataRoomSize);
  assert(txqMemPoolDataRoomSize);

  std::string variable;

  int rc;
  bool valid;

  auto rxSize = static_cast<std::size_t>(config->rxqThreadCount());
  auto txSize = static_cast<std::size_t>(config->txqThreadCount());

  if (config->rxqPolicy() != Dpdk::Names::OFF) {
    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMPOOL_RXQ_SIZE);
    if ((rc = env->valueAsIntList(variable, rxqMemPoolSize, true, 1))!=0) {
      return rc;
    }
    valid = rxSize==rxqMemPoolSize->size();
    if (!valid) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),
        variable.c_str(), config->rxqThreadCount(), rxqMemPoolSize->size(), "values were provided: must be equal");
    }

    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMPOOL_RXQ_CACHE_SIZE);
    if ((rc = env->valueAsIntList(variable, rxqMemPoolCacheSize, true, 0))!=0) {
      return rc;
    }
    valid = rxSize==rxqMemPoolCacheSize->size();
    if (!valid) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),
        variable.c_str(), config->rxqThreadCount(), rxqMemPoolCacheSize->size(), "values were provided: must be equal");
    }

    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMPOOL_RXQ_PRIV_SIZE);
    if ((rc = env->valueAsIntList(variable, rxqMemPoolPrivSize, true, 0))!=0) {
      return rc;
    }
    valid = rxSize==rxqMemPoolPrivSize->size();
    if (!valid) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),
        variable.c_str(), config->rxqThreadCount(), rxqMemPoolPrivSize->size(), "values were provided: must be equal");
    }

    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMPOOL_RXQ_DATA_ROOM_SIZE);
    if ((rc = env->valueAsIntList(variable, rxqMemPoolDataRoomSize, true, 1))!=0) {
      return rc;
    }
    valid = rxSize==rxqMemPoolDataRoomSize->size();
    if (!valid) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),
        variable.c_str(), config->rxqThreadCount(), rxqMemPoolDataRoomSize->size(), "values were provided: must be equal");
    }
  }

  if (config->txqPolicy() != Dpdk::Names::OFF) {
    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMPOOL_TXQ_SIZE);
    if ((rc = env->valueAsIntList(variable, txqMemPoolSize, true, 1))!=0) {
      return rc;
    }
    valid = txSize==txqMemPoolSize->size();
    if (!valid) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),
        variable.c_str(), config->txqThreadCount(), txqMemPoolSize->size(), "values were provided: must be equal");
    }

    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMPOOL_TXQ_CACHE_SIZE);
    if ((rc = env->valueAsIntList(variable, txqMemPoolCacheSize, true, 0))!=0) {
      return rc;
    }
    valid = txSize==txqMemPoolCacheSize->size();
    if (!valid) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),
        variable.c_str(), config->txqThreadCount(), txqMemPoolCacheSize->size(), "values were provided: must be equal");
    }

    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMPOOL_TXQ_PRIV_SIZE);
    if ((rc = env->valueAsIntList(variable, txqMemPoolPrivSize, true, 0))!=0) {
      return rc;
    }
    valid = txSize==txqMemPoolPrivSize->size();
    if (!valid) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),
        variable.c_str(), config->txqThreadCount(), txqMemPoolPrivSize->size(), "values were provided: must be equal");
    }

    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMPOOL_TXQ_DATA_ROOM_SIZE);
    if ((rc = env->valueAsIntList(variable, txqMemPoolDataRoomSize, true, 1))!=0) {
      return rc;
    }
    valid = txSize==txqMemPoolDataRoomSize->size();
    if (!valid) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (valid),
        variable.c_str(), config->txqThreadCount(), txqMemPoolPrivSize->size(), "values were provided: must be equal");
    }
  }

  return 0;
}

int Dpdk::InitAWS::configSharedMempool(const std::string& prefix, Util::Environment *env, int *sharedMemPoolSize,
  int *sharedMempoolEltSize, int *sharedMempoolCacheSize, int *sharedMempoolPrivateSize, int *sharedMempoolFlags) {
  assert(env);
  assert(sharedMemPoolSize);
  assert(sharedMempoolEltSize);
  assert(sharedMempoolCacheSize);
  assert(sharedMempoolPrivateSize);
  assert(sharedMempoolFlags);

  int rc;
  std::string variable;

  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::SHARED_MEMPOOL_SIZE);
  if ((rc = env->valueAsInt(variable, sharedMemPoolSize, true, 1, 1000000000))!=0) {
    return rc;
  }

  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::SHARED_MEMPOOL_ELT_SIZE);
  if ((rc = env->valueAsInt(variable, sharedMempoolEltSize, true, 1))!=0) {
    return rc;
  }

  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::SHARED_MEMPOOL_CACHE_SIZE);
  if ((rc = env->valueAsInt(variable, sharedMempoolCacheSize))!=0) {
    return rc;
  }

  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::SHARED_MEMPOOL_PRIVATE);
  if ((rc = env->valueAsInt(variable, sharedMempoolPrivateSize))!=0) {
    return rc;
  }

  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::SHARED_MEMPOOL_FLAGS);
  if ((rc = env->valueAsInt(variable, sharedMempoolFlags))!=0) {
    return rc;
  }

  return 0;
}

int Dpdk::InitAWS::enaUdp(const std::string& device, const std::string& envPrefix, Util::Environment *env, AWSEnaConfig *config) {
  assert(!device.empty());
  assert(env);
  assert(config);

  bool enabled;
  int intValue, rc;
  int deviceId;
  std::string value;
  std::string variable;
  std::string prefix;

  //
  // Get the deviceId to initialize
  //
  Dpdk::Names::make(envPrefix, &variable, "%s", device.c_str());
  if ((rc = env->valueAsInt(variable, &deviceId))!=0) {
    return rc;
  }
  config->setDeviceId(deviceId);

  //
  // Establish prefix for rest of work
  //
  if (!envPrefix.empty()) {
    prefix = envPrefix;
    prefix.append("_");
    prefix.append(device);
  } else {
    prefix = device;
  }

  //
  // See if this port is soft-disabled
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::SOFT_ENABLED);
  if ((rc = env->valueAsBool(variable, &enabled))!=0) {
    return rc;
  }
  if (!enabled) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (enabled),
      variable.c_str(), deviceId, 0, "of those/this device is soft enabled");
  }

  //
  // Get the number of DRAM memory channels
  //
  Dpdk::Names::make(envPrefix, &variable, "%s", Dpdk::Names::DRAM_MEMORY_CHANNELS);
  if ((rc = env->valueAsInt(variable, &intValue, true, 1))!=0) {
    return rc;
  }
  config->setMemoryChannelCount(intValue);

  //
  // Read in the VCPU, socket, core variables
  //
  int vcpuMax;
  std::vector<VCPU> vcpu;

  Dpdk::Names::make(envPrefix, &variable, "%s", Dpdk::Names::VCPU_MAX);
  if ((rc = env->valueAsInt(variable, &vcpuMax, true, 1))!=0) {
    return rc;
  }

  for (int i=0; i<vcpuMax; ++i) {
    bool softEnabled;
    int core, socket, numaNode;

    // See if VCPU_{i} soft enabled/disabled
    Dpdk::Names::make(envPrefix, &variable, "%s_%d_%s", Dpdk::Names::VCPU, i, Dpdk::Names::SOFT_ENABLED);
    if ((rc = env->valueAsBool(variable, &softEnabled))!=0) {
      return rc;
    }

    // See what core VCPU_{i} on
    Dpdk::Names::make(envPrefix, &variable, "%s_%d_%s", Dpdk::Names::VCPU, i, Dpdk::Names::IN_CORE);
    if ((rc = env->valueAsInt(variable, &core))!=0) {
      return rc;
    }
  
    // See what socket VCPU_{i} on
    Dpdk::Names::make(envPrefix, &variable, "%s_%d_%s", Dpdk::Names::VCPU, i, Dpdk::Names::IN_SOCKET);
    if ((rc = env->valueAsInt(variable, &socket))!=0) {
      return rc;
    }

    // See what NUMA mode VCPU_{i} on
    Dpdk::Names::make(envPrefix, &variable, "%s_%d_%s", Dpdk::Names::VCPU, i, Dpdk::Names::ON_NUMA_NODE);
    if ((rc = env->valueAsInt(variable, &numaNode))!=0) {
      return rc;
    }

    vcpu.push_back(Dpdk::VCPU(i, core, socket, numaNode, softEnabled));
  }

  if (vcpu.empty()) {
    env->valueAsString(Dpdk::Names::VCPU_MAX, &value);
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (vcpuMax>0 && !vcpu.empty()),
      Dpdk::Names::VCPU_MAX, value.c_str(), "is <=0 or too many VCPUs were not marked soft-enabled");
  }
  config->setVcpu(vcpu);

  //
  // Get PCI Device Id
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::PCI_DEVICE_ID);
  if ((rc = env->valueAsString(variable, &value))!=0) {
    return rc;
  }
  config->setPciId(value);

  //
  // Get NIC NUMA NODE
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::NUMA_NODE);
  if ((rc = env->valueAsInt(variable, &intValue))!=0) {
    return rc;
  }
  config->setNumaNode(intValue);

  //
  // Get RXQ_VCPU_POLICY
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::RXQ_VCPU_POLICY);
  if ((rc = env->valueAsString(variable, &value))!=0) {
    return rc;
  }
  bool valid = (value!=Dpdk::Names::OFF || value!=Dpdk::Names::MULTI || value!=Dpdk::Names::DISTINCT);
  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (valid), variable.c_str(), value.c_str(),
      "has an unexpected value");
  }
  config->setRxqPolicy(value);

  if (value!=Dpdk::Names::OFF) {
    //
    // Get the RXQ size
    //
    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::RX_QUEUE_SIZE);
    if ((rc = env->valueAsInt(variable, &intValue, true, 1))!=0) {
      return rc;
    }
    config->setRxqCount(intValue);
    int rxqThreadCount(intValue);

    //
    // Get the RXQ thread count
    //
    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::RXQ_THREAD_COUNT);
    if ((rc = env->valueAsString(variable, &value))!=0) {
      return rc;
    }
    if (value!=Dpdk::Names::QUEUE_SIZE) {
      if ((rc = env->valueAsInt(variable, &intValue, true, 1))!=0) {
        return rc;
      }
      config->setRxqThreadCount(intValue);
    } else {
      config->setRxqThreadCount(rxqThreadCount);
    }
  } else {
    config->setRxqCount(0);
    config->setRxqThreadCount(0);
  }

  //
  // Get TXQ_VCPU_POLICY
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_VCPU_POLICY);
  if ((rc = env->valueAsString(variable, &value))!=0) {
    return rc;
  }
  valid = (value!=Dpdk::Names::OFF || value!=Dpdk::Names::MULTI || value!=Dpdk::Names::DISTINCT);
  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (valid), variable.c_str(), value.c_str(),
      "has an unexpected value");
  }
  config->setTxqPolicy(value);

  if (value!=Dpdk::Names::OFF) {
    //
    // Get the TXQ queue size
    //
    Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TX_QUEUE_SIZE);
    if ((rc = env->valueAsInt(variable, &intValue, true, 1))!=0) {
      return rc;
    }
    config->setTxqCount(intValue);
    int txqThreadCount(intValue);

    //
    // Get the TXQ thread count
    //
    Dpdk::Names::make(prefix, &variable, "%s",Dpdk::Names::TXQ_THREAD_COUNT);
    if ((rc = env->valueAsString(variable, &value))!=0) {
      return rc;
    }
    if (value!=Dpdk::Names::QUEUE_SIZE) {
      if ((rc = env->valueAsInt(variable, &txqThreadCount, true, 1))!=0) {
        return rc;
      }
      config->setTxqThreadCount(intValue);
    } else {
      config->setTxqThreadCount(txqThreadCount);
    }
  } else {
    config->setTxqCount(0);
    config->setTxqThreadCount(0);
  }

  //
  // Get Memzone reserve size
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMZONE_RESERVE_KB);
  if ((rc = env->valueAsInt(variable, &intValue, true, 1, 1048576))!=0) {
    return rc;
  }
  config->setMemzoneReserveKb(intValue);

  //
  // Get RXQ VCPU Candidate List
  //
  std::vector<int> rxqCpuCandidate;
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::RXQ_VCPU_MASK);
  if ((rc = env->valueAsString(variable, &value))!=0) {
    return rc;
  }
  if (Dpdk::Names::ALL==value) {
    // Every enabled VCPU on same NUMA node is candidate
    const std::vector<VCPU>& vcpu(config->vcpu());
    for (unsigned i=0; i<vcpu.size(); ++i) {
      if (vcpu[i].numaNode()==config->numaNode() && vcpu[i].softEnabled()) {
        rxqCpuCandidate.push_back(vcpu[i].id());
      }
    }
  } else if ((rc = env->valueAsIntList(variable, &rxqCpuCandidate))!=0) {
      return rc;
  }
  config->setRxqCandidateVcpuList(rxqCpuCandidate);

  //
  // Get TXQ VCPU Candidate List
  //
  std::vector<int> txqCpuCandidate;
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TXQ_VCPU_MASK);
  if ((rc = env->valueAsString(variable, &value))!=0) {
    return rc;
  }
  if (Dpdk::Names::ALL==value) {
    // Every enabled VCPU on same NUMA node is candidate
    const std::vector<VCPU>& vcpu(config->vcpu());
    for (unsigned i=0; i<vcpu.size(); ++i) {
      if (vcpu[i].numaNode()==config->numaNode() && vcpu[i].softEnabled()) {
        txqCpuCandidate.push_back(vcpu[i].id());
      }
    }
  } else if ((rc = env->valueAsIntList(variable, &txqCpuCandidate))!=0) {
      return rc;
  }
  config->setTxqCandidateVcpuList(txqCpuCandidate);

  //
  // Get the MTU
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::RX_MTU);
  if ((rc = env->valueAsInt(variable, &intValue, true, 1))!=0) {
    return rc;
  }
  config->setMtu(intValue);

  //
  // Get memzone hugepage mask
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMZONE_RESERVE_MASK);
  if ((rc = env->valueAsInt(variable, &intValue, true, 1))!=0) {
    return rc;
  }
  config->setMemzoneMask(intValue);

  //
  // Get memzone name
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMZONE_RESERVE_NAME);
  if ((rc = env->valueAsString(variable, &value))!=0) {
    return rc;
  }
  config->setMemzoneName(value);

  //
  // Get mempool policy
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::MEMPOOL_POLICY);
  if ((rc = env->valueAsString(variable, &value))!=0) {
    return rc;
  }
  valid = (value!=Dpdk::Names::PER_QUEUE || value!=Dpdk::Names::SHARED);
  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_ENVIRONMENT, (valid), variable.c_str(), value.c_str(),
      "has an unexpected value");
  }
  config->setMempoolPolicy(value);

  int sharedMemPoolSize;
  int sharedMempoolEltSize;
  int sharedMempoolCacheSize;
  int sharedMempoolPrivateSize;
  int sharedMempoolFlags;

  std::vector<int> rxqMemPoolSize;
  std::vector<int> txqMemPoolSize;
  std::vector<int> rxqMemPoolCacheSize;
  std::vector<int> txqMemPoolCacheSize;
  std::vector<int> rxqMemPoolPrivSize;
  std::vector<int> txqMemPoolPrivSize;
  std::vector<int> rxqMemPoolDataRoomSize;
  std::vector<int> txqMemPoolDataRoomSize;

  std::vector<int> rxqRingSize;
  std::vector<int> txqRingSize;

  //
  // Get mempool settings
  //
  if (value==Dpdk::Names::PER_QUEUE) {
    rc = Dpdk::InitAWS::configPerQueueMempool(prefix, env, config, &rxqMemPoolSize, &txqMemPoolSize,
      &rxqMemPoolCacheSize, &txqMemPoolCacheSize, &rxqMemPoolPrivSize, &txqMemPoolPrivSize, &rxqMemPoolDataRoomSize,
      &txqMemPoolDataRoomSize);
  } else {
    rc = Dpdk::InitAWS::configSharedMempool(prefix, env, &sharedMemPoolSize, &sharedMempoolEltSize,
      &sharedMempoolCacheSize, &sharedMempoolPrivateSize, &sharedMempoolFlags);
  }
  if (rc!=0) {
    return rc;
  }

  //
  // Get RXQ/TXQ ring sizes
  //
  if ((rc = Dpdk::InitAWS::configRings(prefix, env, config, rxqRingSize, txqRingSize))!=0) {
    return rc;
  }

  //
  // Get linkSpeed
  // 
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::LINK_SPEED_MASK);
  if ((rc = env->valueAsInt(variable, &intValue))!=0) {
    return rc;
  }
  config->setLinkSpeed(intValue);

  //
  // Get RX MQ mask
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::RX_MQ_MASK);
  if ((rc = env->valueAsInt(variable, &intValue))!=0) {
    return rc;
  }
  config->setRxMqMask(intValue);

  //
  // Get TX MQ mask
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TX_MQ_MASK);
  if ((rc = env->valueAsInt(variable, &intValue))!=0) {
    return rc;
  }
  config->setTxMqMask(intValue);

  //
  // Get RX offload mask
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::RX_OFFLOAD_MASK);
  if ((rc = env->valueAsInt(variable, &intValue, true, 0, 1000000))!=0) {
    return rc;
  }
  config->setRxOffloadMask(intValue);

  //
  // Get TX offload mask
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TX_OFFLOAD_MASK);
  if ((rc = env->valueAsInt(variable, &intValue, true, 0, 1000000))!=0) {
    return rc;
  }
  config->setTxOffloadMask(intValue);

  //
  // Get default TX data flow
  //
  Dpdk::Names::make(prefix, &variable, "%s", Dpdk::Names::TX_DEFAULT_FLOW);
  if ((rc = env->valueAsInt(variable, &intValue))!=0) {
    return rc;
  }
  config->setDefaultTxFlow(intValue);

  std::vector<int> defaultTxSrcPort;
  std::vector<int> defaultTxDstPort;
  std::string defaultTxSrcMac;
  std::vector<std::string> defaultTxDstMac;
  std::string defaultTxSrcIp;
  std::vector<std::string> defaultTxDstIp;
  std::vector<UDPRoute> defaultTxRoute;

  //
  // Get default routes for TXQ
  //
  if (config->txqPolicy() != Dpdk::Names::OFF) {
    if ((rc = configTxRouting(prefix, env, config, defaultTxSrcPort, defaultTxDstPort, defaultTxSrcMac,
      defaultTxDstMac, defaultTxSrcIp, defaultTxDstIp, defaultTxRoute))!=0) {
        return rc;
    }
  }

  //
  // Get TXQ threshold values
  //
  if (config->txqPolicy() != Dpdk::Names::OFF) {
    Dpdk::Names::make(prefix, &variable, "%s", "TXQ_PTHRESH");
    if ((rc = env->valueAsInt(variable, &intValue))!=0) {
      return rc;
    }
    config->setTxqPrefetchThresh(intValue);

    Dpdk::Names::make(prefix, &variable, "%s", "TXQ_HTHRESH");
    if ((rc = env->valueAsInt(variable, &intValue))!=0) {
      return rc;
    }
    config->setTxqHostThresh(intValue);
  
    Dpdk::Names::make(prefix, &variable, "%s", "TXQ_WTHRESH");
    if ((rc = env->valueAsInt(variable, &intValue))!=0) {
      return rc;
    }
    config->setTxqWriteBackThresh(intValue);

    Dpdk::Names::make(prefix, &variable, "%s", "TXQ_RS_THRESH");
    if ((rc = env->valueAsInt(variable, &intValue))!=0) {
      return rc;
    }
    config->setTxqRsThresh(intValue);

    Dpdk::Names::make(prefix, &variable, "%s", "TXQ_FREE_THRESH");
    if ((rc = env->valueAsInt(variable, &intValue))!=0) {
      return rc;
    }
    config->setTxqFreeThresh(intValue);
  }

  //
  // Get RXQ threshold values
  //
  if (config->rxqPolicy() != Dpdk::Names::OFF) {
    Dpdk::Names::make(prefix, &variable, "%s", "RXQ_PTHRESH");
    if ((rc = env->valueAsInt(variable, &intValue))!=0) {
      return rc;
    }
    config->setRxqPrefetchThresh(intValue);

    Dpdk::Names::make(prefix, &variable, "%s", "RXQ_HTHRESH");
    if ((rc = env->valueAsInt(variable, &intValue))!=0) {
      return rc;
    }
    config->setRxqHostThresh(intValue);

    Dpdk::Names::make(prefix, &variable, "%s", "RXQ_WTHRESH");
    if ((rc = env->valueAsInt(variable, &intValue))!=0) {
      return rc;
    }
    config->setRxqWriteBackThresh(intValue);

    Dpdk::Names::make(prefix, &variable, "%s", "RXQ_FREE_THRESH");
    if ((rc = env->valueAsInt(variable, &intValue))!=0) {
      return rc;
    }
    config->setRxqFreeThresh(intValue);
  }

  // =================================================================================
  // Done reading environment. Turn towards initialization
  // =================================================================================

  //
  // Assign queues to HW cores. Required for rte_eal_init. Assignment is
  // required to make lcore to HW core assignment which leads to --lcores
  // DPDK command line argument
  //
  std::vector<Dpdk::RXQ> rxq;
  std::vector<Dpdk::TXQ> txq;
  std::vector<Dpdk::LCORE> lcore;
  if ((rc = Dpdk::InitAWS::makeAssignment(config, rxq, txq, lcore))!=0) {
    return rc;
  }
  config->setRxq(rxq);
  config->setTxq(txq);
  config->setLcore(lcore);

  //
  // Call rte_eal_init(): no DPDK calls will succeed prior to this call
  // We have to play around with command line args along to the way
  //
  if ((rc = Dpdk::InitAWS::makeDpdkInitParams(envPrefix, env, config))!=0) {
    return rc;
  } 
  std::vector<const char *> argv;
  for (std::size_t i=0; i<config->dpdkArgs().size(); ++i) {
    argv.push_back(config->dpdkArgs()[i].c_str());
  }
  char **argvc = const_cast<char **>(argv.data());
  if ((rc = rte_eal_init(static_cast<int>(argv.size()), argvc))<0) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (rc>=0),
      "DPDK EAL initialization", rte_strerror(rc), rc);
  }

  //
  // See how many devices are available
  //
  uint16_t dpdkAvailablePorts = rte_eth_dev_count_avail();
  if (config->deviceId()>=dpdkAvailablePorts) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (config->deviceId()<dpdkAvailablePorts),
      "DPDK device", config->deviceId(), dpdkAvailablePorts, "devices available");
  }

  //
  // Get NIC capabilities according to DPDK
  // 
  rte_eth_dev_info* ethDeviceInfo = config->ethDeviceInfo();
  rte_eth_dev_info_get(config->deviceId(), ethDeviceInfo);
  REINVENT_UTIL_LOG_DEBUG("DPDK reported deviceInfo: " << *ethDeviceInfo << std::endl);

  //
  // See if the DPDK device has the same PCI ID requested
  //
  valid = false;
  char pciName[128];
  snprintf(pciName, sizeof(pciName), "%s", "**(uninitialized)**");
  if (ethDeviceInfo->device) {
    const struct rte_bus *bus = rte_bus_find_by_device(ethDeviceInfo->device);
    if (bus && !strcmp(bus->name, "pci")) {
      const struct rte_pci_device *pci_dev = RTE_DEV_TO_PCI(ethDeviceInfo->device);
      snprintf(pciName, sizeof(pciName), "%04x:%02x:%02x.%x", pci_dev->addr.domain, pci_dev->addr.bus,
        pci_dev->addr.devid, pci_dev->addr.function);
      if (pciName==config->pciId()) {
        valid=true;
      }
    }
  }
  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, valid,
      config->pciId().c_str(), 1, 0, pciName);
  }
  REINVENT_UTIL_LOG_DEBUG("DPDK found device: '" << pciName << "'" << std::endl);

  // See if RXQ size exceeds size known to DPDK
  valid = (config->rxqCount()>=0&&config->rxqCount()<=ethDeviceInfo->max_rx_queues);
  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, valid,
      "RXQ configuration sets", config->rxqCount(), ethDeviceInfo->max_rx_queues,
      "RX queues are known to DPDK or RXQ count non-positive");
  }

  // See if TXQ size exceeds size as known to DPDK
  valid = (config->txqCount()>=0&&config->txqCount()<=ethDeviceInfo->max_tx_queues);
  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, valid,
      "TXQ configuration sets", config->txqCount(), ethDeviceInfo->max_tx_queues,
      "TX queues are known to DPDK or TXQ count non-positive");
  }

  // The rxqThreadCount cannot exceed the RXQ count
  valid = (config->rxqThreadCount()>=0&&config->rxqThreadCount()<=config->rxqCount());
  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, valid,
      "RXQ thread count", config->rxqThreadCount(), config->rxqCount(),
      "RXQs exist in NIC and/or configured for use or RXQ thread count non-positive");
  }

  // The txqThreadCount cannot exceed the TXQ count
  valid = (config->txqThreadCount()>=0&&config->txqThreadCount()<=config->txqCount());
  if (!valid) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, valid,
      "TXQ thread count", config->txqThreadCount(), config->txqCount(),
      "TXQs exist in NIC and/or configured for use or TXQ thread count non-positive");
  }

  //
  // Get device status, duplex mode, speed
  //
  rte_eth_link status;
  memset(&status, 0, sizeof(status));
  if ((rc = rte_eth_link_get(config->deviceId(), &status))!=0) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (rc==0), "Get DPDK device status", rte_strerror(rc), rc);
  }
  REINVENT_UTIL_LOG_DEBUG("DPDK reported link status: " << status << std::endl);

  // =================================================================================
  // Start initialization of NIC, queues
  // =================================================================================

  //
  // Initalize device with a configuration.
  // Advance RX/TX configs not yet supported
  //
  rte_eth_conf* deviceConfig      = config->ethDeviceConf();
  memset(deviceConfig, 0, sizeof(rte_eth_conf));
  deviceConfig->link_speeds       = config->linkSpeed();
  deviceConfig->rxmode.mq_mode    = static_cast<rte_eth_rx_mq_mode>(config->rxMqMask());
  deviceConfig->rxmode.mtu        = config->mtu();
  deviceConfig->rxmode.offloads   = config->rxOffloadMask();
  deviceConfig->txmode.mq_mode    = static_cast<rte_eth_tx_mq_mode>(config->txMqMask());
  deviceConfig->txmode.offloads   = config->txOffloadMask();

  if ((rc = rte_eth_dev_configure(config->deviceId(), config->rxqThreadCount(), config->txqThreadCount(), deviceConfig))!=0) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (rc==0), "Initialize DPDK AWS ENA device config", rte_strerror(rc), rc);
  }

  uint16_t adjustedRxq, adjustedTxq;
  if ((rc = rte_eth_dev_adjust_nb_rx_tx_desc(config->deviceId(), &adjustedRxq, &adjustedTxq))!=0) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (rc==0), "Initialize DPDK AWS ENA device config", rte_strerror(rc), rc);
  }
  
  //
  // Allocate memzone
  //
  const rte_memzone *zone = rte_memzone_reserve(config->memzoneName().c_str(), static_cast<std::size_t>(config->memzoneReserveKb()*1024),
    config->numaNode(), config->memzoneMask());
  if (0==zone) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (zone), "Reserving DPDK AWS ENA zone memory", rte_strerror(rte_errno), rte_errno);
  }
  config->setMemzone(zone);

  //
  // Make mempools
  //
  if (config->mempoolPolicy()==Dpdk::Names::PER_QUEUE) {
    //
    // Per-RXQ mempool
    //
    for (int i=0; i<config->rxqThreadCount(); i++) {
      std::string poolName;
      Dpdk::Names::make(envPrefix, &poolName, "DEV_%d_RXQ_%d", config->deviceId(), i);
      rte_mempool *pool = rte_pktmbuf_pool_create(poolName.c_str(),
        rxqMemPoolSize[i], rxqMemPoolCacheSize[i], rxqMemPoolPrivSize[i],
        rxqMemPoolDataRoomSize[i], config->numaNode());
      if (0==pool) {
        REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (zone), "Reserving DPDK AWS ENA RXQ mempool memory", rte_strerror(rte_errno), rte_errno);
      }
      config->rxq()[i].setRingSize(rxqRingSize[i]);
      config->rxq()[i].setMemzone(zone);
      config->rxq()[i].setMempool(pool);
      config->rxq()[i].setMempoolPolicy(config->mempoolPolicy());
      if (0==pool) {
        REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (pool),
          "Reserving DPDK AWS ENA RX pool memory", rte_strerror(rte_errno), rte_errno);
      }
    }

    //
    // Per-TXQ mempool
    //
    for (int i=0; i<config->txqThreadCount(); i++) {
      std::string poolName;
      Dpdk::Names::make(envPrefix, &poolName, "DEV_%d_TXQ_%d", config->deviceId(), i);
      rte_mempool *pool = rte_pktmbuf_pool_create(poolName.c_str(),
        txqMemPoolSize[i], txqMemPoolCacheSize[i], txqMemPoolPrivSize[i],
        txqMemPoolDataRoomSize[i], config->numaNode());
      if (0==pool) {
        REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (zone), "Reserving DPDK AWS ENA TXQ mempool memory", rte_strerror(rte_errno), rte_errno);
      }
      config->txq()[i].setRingSize(txqRingSize[i]);
      config->txq()[i].setMemzone(zone);
      config->txq()[i].setMempool(pool);
      config->txq()[i].setMempoolPolicy(config->mempoolPolicy());
      if (0==pool) {
        REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (pool),
          "Reserving DPDK AWS ENA TX pool memory", rte_strerror(rte_errno), rte_errno);
      }
    }
  } else {
    //
    // Shared mempool for all queues 
    //
    std::string poolName;
    Dpdk::Names::make(envPrefix, &poolName, "DEV_%d_SHARED", config->deviceId());

    rte_mempool *pool = rte_mempool_create(poolName.c_str(), static_cast<unsigned>(sharedMemPoolSize),
      static_cast<unsigned>(sharedMempoolEltSize), static_cast<unsigned>(sharedMempoolCacheSize),
      static_cast<unsigned>(sharedMempoolPrivateSize), 0, 0, rte_pktmbuf_init, 0, config->numaNode(),
      static_cast<unsigned>(sharedMempoolFlags));
    if (0==pool) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (pool),
        "Reserving DPDK AWS ENA shared pool memory", rte_strerror(rte_errno), rte_errno);
    }

    //
    // Install shared pool into RXQ
    //
    for (int i=0; i<config->rxqThreadCount(); i++) {
      config->rxq()[i].setRingSize(rxqRingSize[i]);
      config->rxq()[i].setMemzone(zone);
      config->rxq()[i].setMempool(pool);
      config->rxq()[i].setMempoolPolicy(config->mempoolPolicy());
    }

    //
    // Install shared pool into TXQ
    //
    for (int i=0; i<config->txqThreadCount(); i++) {
      config->txq()[i].setRingSize(txqRingSize[i]);
      config->txq()[i].setMemzone(zone);
      config->txq()[i].setMempool(pool);
      config->txq()[i].setMempoolPolicy(config->mempoolPolicy());
    }
  }

  //
  // Install default flow, routes into TXQs
  //
  for (int i=0; i<config->txqThreadCount(); i++) {
    config->txq()[i].setDefaultFlow(config->defaultTxFlow());
    config->txq()[i].setDefaultRoute(defaultTxRoute[i]);
  }

  //
  // Create RXQs
  //
  rte_eth_rxconf rxCfg = ethDeviceInfo->default_rxconf;
  rxCfg.offloads = deviceConfig->rxmode.offloads;
  rxCfg.rx_thresh.pthresh = config->rxqPrefetchThresh();
  rxCfg.rx_thresh.hthresh = config->rxqHostThresh();
  rxCfg.rx_thresh.wthresh = config->rxqWriteBackThresh();
  rxCfg.rx_free_thresh = config->rxqFreeThresh();
  REINVENT_UTIL_LOG_DEBUG("RXQ conf: " << rxCfg << std::endl);
  for (int i=0; i<config->rxqThreadCount(); ++i) {
    if ((rc = rte_eth_rx_queue_setup(config->deviceId(), i, config->rxq()[i].ringSize(), config->numaNode(), &rxCfg,
      config->rxq()[i].mempool()))!=0) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (rc==0), "Initialize DPDK AWS ENA RXQ", rte_strerror(rc), rc);
    }
  }

  //
  // Create TXQs
  //
  rte_eth_txconf txCfg = ethDeviceInfo->default_txconf;
  txCfg.offloads = deviceConfig->txmode.offloads;
  txCfg.tx_thresh.pthresh = config->txqPrefetchThresh();
  txCfg.tx_thresh.hthresh = config->txqHostThresh();
  txCfg.tx_thresh.wthresh = config->txqWriteBackThresh();
  txCfg.tx_rs_thresh = config->txqRsThresh();
  txCfg.tx_free_thresh = config->txqFreeThresh();
  REINVENT_UTIL_LOG_DEBUG("TXQ conf: " << txCfg << std::endl);
  for (int i=0; i<config->txqThreadCount(); ++i) {
    if ((rc = rte_eth_tx_queue_setup(config->deviceId(), i, config->txq()[i].ringSize(), config->numaNode(), &txCfg))!=0) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (rc==0), "Initialize DPDK AWS ENA TXQ", rte_strerror(rc), rc);
    }
  }

  // =================================================================================
  // Final step: start-up the NIC device and its quues in DPDK
  // =================================================================================

  //
  // Start device
  //
  if ((rc = rte_eth_dev_start(config->deviceId()))!=0) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (rc==0), "Start DPDK AWS ENA device", rte_strerror(rc), rc);
  }

  return 0;
}

} // namespace Reinvent
