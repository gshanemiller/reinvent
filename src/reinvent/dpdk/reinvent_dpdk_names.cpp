#include <dpdk/reinvent_dpdk_names.h>

#include <assert.h>
#include <stdarg.h>

namespace Reinvent {

const char *Dpdk::Names::OFF                    = "OFF";
const char *Dpdk::Names::MAX                    = "MAX";
const char *Dpdk::Names::MULTI                  = "MULTI";
const char *Dpdk::Names::DISTINCT               = "DISTINCT";
const char *Dpdk::Names::NUMA_NODE              = "NUMA_NODE";
const char *Dpdk::Names::QUEUE_SIZE             = "QUEUE_SIZE";
const char *Dpdk::Names::RX_QUEUE_SIZE          = "RX_QUEUE_SIZE";
const char *Dpdk::Names::TX_QUEUE_SIZE          = "TX_QUEUE_SIZE";
const char *Dpdk::Names::RXQ_THREAD_COUNT       = "RXQ_THREAD_COUNT";
const char *Dpdk::Names::TXQ_THREAD_COUNT       = "TXQ_THREAD_COUNT";
const char *Dpdk::Names::RXQ_VCPU_MASK          = "RXQ_VCPU_MASK";
const char *Dpdk::Names::TXQ_VCPU_MASK          = "TXQ_VCPU_MASK";
const char *Dpdk::Names::RXQ_VCPU_POLICY        = "RXQ_VCPU_POLICY";
const char *Dpdk::Names::TXQ_VCPU_POLICY        = "TXQ_VCPU_POLICY";
const char *Dpdk::Names::RXQ_RING_SIZE          = "RXQ_RING_SIZE";
const char *Dpdk::Names::TXQ_RING_SIZE          = "TXQ_RING_SIZE";
const char *Dpdk::Names::VCPU                   = "VCPU";
const char *Dpdk::Names::VCPU_MAX               = "VCPU_MAX";
const char *Dpdk::Names::IN_CORE                = "IN_CORE";
const char *Dpdk::Names::IN_SOCKET              = "IN_SOCKET";
const char *Dpdk::Names::ON_NUMA_NODE           = "ON_NUMA_NODE";
const char *Dpdk::Names::PCI_DEVICE_ID          = "PCI_DEVICE_ID";
const char *Dpdk::Names::MEMZONE                = "MEMZONE";
const char *Dpdk::Names::DRAM_MEMORY_CHANNELS   = "DRAM_MEMORY_CHANNELS";

const char *Dpdk::Names::MEMZONE_RESERVE_KB     = "MEMZONE_RESERVE_KB";
const char *Dpdk::Names::MEMZONE_RESERVE_MASK   = "MEMZONE_RESERVE_MASK";
const char *Dpdk::Names::MEMZONE_RESERVE_NAME   = "MEMZONE_RESERVE_NAME";

const char *Dpdk::Names::LINK_SPEED_MASK        = "LINK_SPEED_MASK";
const char *Dpdk::Names::RX_MTU                 = "RX_MTU";
const char *Dpdk::Names::RX_MQ_MASK             = "RX_MQ_MASK";
const char *Dpdk::Names::RX_OFFLOAD_MASK        = "RX_OFFLOAD_MASK";
const char *Dpdk::Names::TX_MQ_MASK             = "TX_MQ_MASK";
const char *Dpdk::Names::TX_OFFLOAD_MASK        = "TX_OFFLOAD_MASK";
const char *Dpdk::Names::TX_DEFAULT_FLOW        = "TX_DEFAULT_FLOW";

const char *Dpdk::Names::RX_RSS_KEY             = "RX_RSS_KEY";
const char *Dpdk::Names::RX_RSS_HF              = "RX_RSS_HF";

const char *Dpdk::Names::DPDK_INITIALIZATION_PARAMETERS = "DPDK_INITIALIZATION_PARAMETERS";

const char *Dpdk::Names::ALL                    = "ALL";
const char *Dpdk::Names::SHARED                 = "SHARED";
const char *Dpdk::Names::PER_QUEUE              = "PER_QUEUE";
const char *Dpdk::Names::MEMPOOL_POLICY         = "MEMPOOL_POLICY";
const char *Dpdk::Names::SOFT_ENABLED           = "SOFT_ENABLED";

const char *Dpdk::Names::MEMPOOL_RXQ_SIZE           = "MEMPOOL_RXQ_SIZE";
const char *Dpdk::Names::MEMPOOL_TXQ_SIZE           = "MEMPOOL_TXQ_SIZE";
const char *Dpdk::Names::MEMPOOL_RXQ_CACHE_SIZE     = "MEMPOOL_RXQ_CACHE_SIZE";
const char *Dpdk::Names::MEMPOOL_TXQ_CACHE_SIZE     = "MEMPOOL_TXQ_CACHE_SIZE";
const char *Dpdk::Names::MEMPOOL_RXQ_PRIV_SIZE      = "MEMPOOL_RXQ_PRIV_SIZE";
const char *Dpdk::Names::MEMPOOL_TXQ_PRIV_SIZE      = "MEMPOOL_TXQ_PRIV_SIZE";
const char *Dpdk::Names::MEMPOOL_RXQ_DATA_ROOM_SIZE = "MEMPOOL_RXQ_DATA_ROOM_SIZE";
const char *Dpdk::Names::MEMPOOL_TXQ_DATA_ROOM_SIZE = "MEMPOOL_TXQ_DATA_ROOM_SIZE";

const char *Dpdk::Names::SHARED_MEMPOOL_SIZE        = "SHARED_MEMPOOL_SIZE";
const char *Dpdk::Names::SHARED_MEMPOOL_ELT_SIZE    = "SHARED_MEMPOOL_ELT_SIZE";
const char *Dpdk::Names::SHARED_MEMPOOL_CACHE_SIZE  = "SHARED_MEMPOOL_CACHE_SIZE";
const char *Dpdk::Names::SHARED_MEMPOOL_PRIVATE     = "SHARED_MEMPOOL_PRIVATE";
const char *Dpdk::Names::SHARED_MEMPOOL_FLAGS       = "SHARED_MEMPOOL_FLAGS";

const char *Dpdk::Names::TXQ_DEFAULT_ROUTE          = "TXQ_DEFAULT_ROUTE";
const char *Dpdk::Names::TXQ_DEFAULT_ROUTE_COUNT    = "TXQ_DEFAULT_ROUTE_COUNT";
const char *Dpdk::Names::TXQ_DEFAULT_SRC_MAC        = "SRC_MAC";
const char *Dpdk::Names::TXQ_DEFAULT_DST_MAC        = "DST_MAC";
const char *Dpdk::Names::TXQ_DEFAULT_SRC_IP         = "SRC_IP";
const char *Dpdk::Names::TXQ_DEFAULT_DST_IP         = "DST_IP";
const char *Dpdk::Names::TXQ_DEFAULT_SRC_PORT       = "SRC_PORT";
const char *Dpdk::Names::TXQ_DEFAULT_DST_PORT       = "DST_PORT";

void Dpdk::Names::make(std::string *name, const char *format, ...) {                                  
  assert(name);                                                                                                         
  assert(format);                                                                                                       
                                                                                                                        
  va_list ap;                                                                                                           
  va_start(ap, format);                                                                                                 
  char tmp[MAX_VARIABLE_NAME_SIZE_BYTES]={0};                                                                           
                                                                                                                        
  vsnprintf(tmp, MAX_VARIABLE_NAME_SIZE_BYTES, format, ap);                                                             
  *name = tmp;                                                                                                          
}                                                                                                                       
                                                                                                                        
void Dpdk::Names::make(const std::string& prefix, std::string *name, const char *format, ...) {       
  assert(name);                                                                                                         
  assert(format);                                                                                                       
  assert(!prefix.empty());
                                                                                                                        
  va_list ap;                                                                                                           
  va_start(ap, format);                                                                                                 
  char tmp[MAX_VARIABLE_NAME_SIZE_BYTES]={0};                                                                           
                                                                                                                        
  name->clear();                                                                                                        
                                                                                                                        
  if (!prefix.empty()) {                                                                                                
    *name = prefix;                                                                                                     
    name->append(1, '_');                                                                                               
  }                                                                                                                     
                                                                                                                        
  vsnprintf(tmp, MAX_VARIABLE_NAME_SIZE_BYTES, format, ap);                                                             
  name->append(tmp);                                                                                                    
  name->resize(MAX_VARIABLE_NAME_SIZE_BYTES-1);                                                                         
}

}
