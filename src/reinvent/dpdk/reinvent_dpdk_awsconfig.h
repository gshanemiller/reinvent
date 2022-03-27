#pragma once

// Purpose: AWS ENA DPDK Configuration
//
// Classes:
//  Dpdk::AWSEnaConfig: None-copyable AWS ENA DPDK Configuration for one DPDK Port/Device
//  
// Thread Safety: Not MT thread-safe.
//                                                                                                                      
// Exception Policy: No exceptions                                                                                      
//
// AWSEnaConfig is a non-copyable value semantic type holding the all the configuration state for one AWS ENA DPAK
// NIC/Device with a zero-based 'deviceId'. In practice application programmers do not construct this object; it is
// initialized by one of the 'Dpdk::InitAWS' helper routines. To that end and towards eliminating substantial
// duplication between attributes here and UNIX environment variables read there, readers are referred to the extensive
// documentation under 'Dpdk::InitAWS'.
//
// 'AWSEnaConfig' is meant to be a semi-organized collection of configuration attributes. The number of DPDK
// applications and HW is legion. Thus the purpose of this class is impose some structure mainly on lcores, VCPUs, and
// RXQ/TXQs leaving the rest of attributes lose. You will not find many helper classes to hold UDP, crypto, and other
// speciality data. So long as every variable is clear and easily accessible, application programmers can extract
// configs into bespoke structures need once the lcores are running.
// 
// Class objects are not copyable for the same reasons Dpdk::RXQ and Dpdk::TXQ are not; see their class descriptions
// for more information.

#include <vector>
#include <assert.h>

#include <dpdk/reinvent_dpdk_rxq.h>
#include <dpdk/reinvent_dpdk_txq.h>
#include <dpdk/reinvent_dpdk_vcpu.h>
#include <dpdk/reinvent_dpdk_lcore.h>
#include <dpdk/reinvent_dpdk_names.h>

//                                                                                                                      
// Tell GCC to not enforce '-Wpendantic' for DPDK headers                                                               
//                                                                                                                      
#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wpedantic"                                                                             
#include <rte_eal.h>                                                                                                    
#include <rte_ethdev.h>                                                                                                 
#include <rte_memzone.h>
#pragma GCC diagnostic pop 

namespace Reinvent {
namespace Dpdk {

class AWSEnaConfig {
private:
  // Validity of state
  int d_isValid;

  // DPDK initialization switches
  std::vector<std::string>  d_dpdkArgs;

  // HW/CPU Inventory
  int                       d_memoryChannelCount;
  std::vector<VCPU>         d_vcpu;

  // NIC queue
  int                       d_deviceId;
  std::string               d_pciId;
  int                       d_numaNode;
  int                       d_rxqCount;
  int                       d_txqCount;
  int                       d_rxqThreadCount;
  int                       d_txqThreadCount;
  std::string               d_rxqPolicy;
  std::string               d_txqPolicy;
  std::vector<int>          d_rxqCandidateVcpuList;
  std::vector<int>          d_txqCandidateVcpuList;
  int                       d_mtu;

  // NIC memzone/mempool
  int                       d_memzoneMask;
  int                       d_memzoneReserveKb;
  std::string               d_memzoneName;
  std::string               d_mempoolPolicy;

  // Queues 
  std::vector<RXQ>          d_rxq;
  std::vector<TXQ>          d_txq;

  // Lcore map
  std::vector<LCORE>        d_lcore;

  // Other NIC or UDP/TCP parms
  int                       d_linkSpeed;
  int                       d_rxMqMask;
  int                       d_txMqMask;
  int                       d_rxOffloadMask;
  int                       d_txOffloadMask;
  int                       d_defaultTxFlow;

  // TXQ/RXQ Threshholds
  int                       d_txqPrefetchThresh;
  int                       d_txqHostThresh;
  int                       d_txqWriteBackThresh;
  int                       d_txqRsThresh;
  int                       d_txqFreeThresh;
  int                       d_rxqPrefetchThresh;
  int                       d_rxqHostThresh;
  int                       d_rxqWriteBackThresh;
  int                       d_rxqFreeThresh;

  // DPDK structures initialized
  struct rte_eth_conf       d_ethDeviceConf;
  struct rte_eth_dev_info   d_ethDeviceInfo;
  const struct rte_memzone *d_memzone;

public:
  // CREATORS
  AWSEnaConfig();
    // Create a AWSEnaConfig object with default values. Note this configuration cannot be used to initialize a NIC.
    // It's only provided so callers can subsequently run set methods culminating in 'isValid()' after which AWS ENA
    // NIC initialization can start.

  AWSEnaConfig(const AWSEnaConfig& object) = delete;
    // Copy constructor not provided

  virtual ~AWSEnaConfig() = default;
    // Destory this object

  // ACCESSORS
  int isValid() const;
    // Return 0 if this object contains a valid configuration and non-zero otherwise. See 'setIsValid()' which sets
    // this attribute once all initialization complete or an error happens whichever comes first.

  const std::vector<std::string>& dpdkArgs() const;
    // Return value of 'dpdkArgs' atrribute

  int memoryChannelCount() const;
    // Return value of 'memoryChannelCount' atrribute
  
  const std::vector<VCPU>& vcpu() const;
    // Return value of 'vcpu' atrribute

  int deviceId() const;
    // Return value of 'deviceId' atrribute

  const std::string& pciId() const;
    // Return value of 'pciId' atrribute

  int numaNode() const;
    // Return value of 'numaNode' atrribute

  int rxqCount() const;
    // Return value of 'rxqCount' atrribute

  int txqCount() const;
    // Return value of 'txqCount' atrribute

  int rxqThreadCount() const;
    // Return value of 'rxqThreadCount' atrribute

  int txqThreadCount() const;
    // Return value of 'txqThreadCount' atrribute

  const std::string& rxqPolicy() const;
    // Return value of 'rxqPolicy' atrribute

  const std::string& txqPolicy() const;
    // Return value of 'txqPolicy' atrribute

  const std::vector<int>& rxqCandidateVcpuList() const;
    // Return value of 'rxqCandidateVcpuList' atrribute

  const std::vector<int>& txqCandidateVcpuList() const;
    // Return value of 'txqCandidateVcpuList' atrribute

  int mtu() const;
    // Return value of 'mtu' atrribute

  int memzoneMask() const;
    // Return value of 'memzoneMask' atrribute

  int memzoneReserveKb() const;
    // Return value of 'memzoneReserveKb' atrribute

  const std::string& memzoneName() const;
    // Return value of 'memzoneName' atrribute

  const struct rte_memzone *memzone() const;
    // Return value of 'memzone' attribiute

  const std::string& mempoolPolicy() const;
    // Return value of 'mempoolPolicy' atrribute

  const std::vector<RXQ>& rxq() const;
    // Return non-modifiable reference to 'rxq' attribute

  const std::vector<TXQ>& txq() const;
    // Return non-modifiable reference to 'txq' attribute

  const std::vector<LCORE>& lcore() const;
    // Return non-modifiable reference to 'lcore' attribute

  int linkSpeed() const;
    // Return value of 'linkSpeed' atrribute
  
  int rxMqMask() const;
    // Return value of 'rxMqMask' attribute

  int txMqMask() const;
    // Return value of 'txMqMask' attribute

  int rxOffloadMask() const;
    // Return value of 'rxOffloadMask' attribute

  int txOffloadMask() const;
    // Return value of 'txOffloadMask' attribute

  int defaultTxFlow() const;
    // Return value of 'defaultTxFlow' attribute

  int txqPrefetchThresh() const;
    // Return value of 'txqPrefetchThresh' attribute

  int txqHostThresh() const;
    // Return value of 'txqHostThresh' attribute

  int txqWriteBackThresh() const;
    // Return value of 'txqWriteBackThresh' attribute

  int txqRsThresh() const;
    // Return value of 'txqRsThresh' attribute

  int txqFreeThresh() const;
    // Return value of 'txqFreeThresh' attribute

  int rxqPrefetchThresh() const;
    // Return value of 'rxqPrefetchThresh' attribute

  int rxqHostThresh() const;
    // Return value of 'rxqHostThresh' attribute

  int rxqWriteBackThresh() const;
    // Return value of 'rxqWriteBackThresh' attribute

  int rxqFreeThresh() const;
    // Return value of 'rxqFreeThresh' attribute
  
  // MANIPULATORS
  void setIsValid(int value);
    // Assign specified 'value' to the attribute 'isValid'. Note callers should set 'value=0' after successful NIC
    // initializtion so validty is proved true by a successful DPDK return witness code.

  void setDpdkArgs(std::vector<std::string>& value);
    // Swap specified 'value' with the attribute 'dpdkArgs'

  std::vector<std::string>& dpdkArgs(void);
    // Return modifiable reference to 'dpdkArgs' attribute

  void setMemoryChannelCount(int value);
    // Assign specified 'value' to the attribute 'memoryChannelCount'
  
  void setVcpu(std::vector<VCPU>& value);
    // Swap specified 'value' with the attribute 'cpu'

  void setDeviceId(int value);
    // Assign specified 'value' to the attribute 'deviceId'

  void setPciId(const std::string& value);
    // Assign specified 'value' to the attribute 'pciId'

  void setNumaNode(int value);
    // Assign specified 'value' to the attribute 'numaNode'

  void setRxqCount(int value);
    // Assign specified 'value' to the attribute 'rxqCount'

  void setTxqCount(int value);
    // Assign specified 'value' to the attribute 'txqCount'

  void setRxqThreadCount(int value);
    // Assign specified 'value' to the attribute 'rxqThreadCount'

  void setTxqThreadCount(int value);
    // Assign specified 'value' to the attribute 'txqThreadCount'

  void setRxqPolicy(const std::string& value);
    // Assign specified 'value' to the attribute 'rxqPolicy'

  void setTxqPolicy(const std::string& value);
    // Assign specified 'value' to the attribute 'txqPolicy'

  void setRxqCandidateVcpuList(std::vector<int>& value);
    // Swap specified 'value' with the attribute 'rxqCandidateVcpuList'

  void setTxqCandidateVcpuList(std::vector<int>& value);
    // Swap specified 'value' with the attribute 'txqCandidateVcpuList'

  void setMtu(int value);
    // Assign specified 'value' to the attribute 'mtu'

  void setMemzoneMask(int value);
    // Assign specified 'value' to the attribute 'memzoneMask'

  void setMemzoneReserveKb(int value);
    // Assign specified 'value' to the attribute 'memzoneReserveKb'
 
  void setMemzoneName(const std::string& value);
    // Assign specified 'value' to the attribute 'memzoneName'

  void setMempoolPolicy(const std::string& value);
    // Assign specified 'value' to the attribute 'mempoolPolicy'

  void setRxq(std::vector<RXQ>& value);
    // Swap specified 'value' with the attribute 'rxq'

  void setTxq(std::vector<TXQ>& value);
    // Swap specified 'value' with the attribute 'txq'

  std::vector<RXQ>& rxq();
    // Return modifiable reference to attribute 'rxq'

  std::vector<TXQ>& txq();
    // Return modifiable reference to attribute 'txq'

  void setLcore(std::vector<LCORE>& value);
    // Swap specified 'value' with the attribute 'lcore'

  void setLinkSpeed(int value);
    // Assign specified 'value' to the attribute 'linkSpeed'

  void setRxMqMask(int value);
    // Assign specified 'value' to the attribute 'rxMqMask'

  void setTxMqMask(int value);
    // Assign specified 'value' to the attribute 'txMask'

  void setRxOffloadMask(int value);
    // Assign specified 'value' to the attribute 'rxOffloadMask'

  void setTxOffloadMask(int value);
    // Assign specified 'value' to the attribute 'txOffloadMask'

  void setDefaultTxFlow(int value);
    // Assign specified 'value' to the attribute 'defaultTxFlow 

  rte_eth_conf *ethDeviceConf();
    // Return modifable pointer to 'ethDeviceConf' attribute

  rte_eth_dev_info *ethDeviceInfo();
    // Return modifiable pointer to 'ethDeviceInfo' attribute

  void setMemzone(const struct rte_memzone *value);
    // Assign value to the 'memzone' attribute

  void setTxqPrefetchThresh(int value);
    // Assign value to the 'txqPrefetchThresh` attribute

  void setTxqHostThresh(int value);
    // Assign value to the 'txqHostThresh` attribute

  void setTxqWriteBackThresh(int value);
    // Assign value to the 'txqWritebackThresh` attribute

  void setTxqRsThresh(int value);
    // Assign value to the 'txqRsThresh` attribute

  void setTxqFreeThresh(int value);
    // Assign value to the 'txqFreeThresh` attribute

  void setRxqPrefetchThresh(int value);
    // Assign value to the 'rxqPrefetchThresh` attribute

  void setRxqHostThresh(int value);
    // Assign value to the 'rxqHostThresh` attribute

  void setRxqWriteBackThresh(int value);
    // Assign value to the 'rxqWriteBackThresh` attribute

  void setRxqFreeThresh(int value);
    // Assign value to the 'rxqFreeThresh` attribute

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Print into specified 'stream' a human readable dump of 'this' returning 'stream'
};

// FREE OPERATORS
std::ostream& operator<<(std::ostream& stream, const AWSEnaConfig& object);
    // Print into specified 'stream' a human readable dump of 'object' returning 'stream'

// INLINE DEFINITIONS
inline
AWSEnaConfig::AWSEnaConfig()
: d_isValid(1)
, d_deviceId(0)
, d_numaNode(0)
, d_rxqCount(0)
, d_txqCount(0)
, d_rxqThreadCount(0)
, d_txqThreadCount(0)
, d_mtu(0)
, d_memzoneMask(0)
, d_memzoneReserveKb(0)
, d_linkSpeed(0)
, d_rxMqMask(0)
, d_txMqMask(0)
, d_rxOffloadMask(0)
, d_txOffloadMask(0)
, d_defaultTxFlow(0)
, d_txqPrefetchThresh(0)
, d_txqHostThresh(0)
, d_txqWriteBackThresh(0)
, d_txqRsThresh(0)
, d_txqFreeThresh(0)
, d_rxqPrefetchThresh(0)
, d_rxqHostThresh(0)
, d_rxqWriteBackThresh(0)
, d_rxqFreeThresh(0)
, d_memzone(0)
{
  memset(static_cast<void*>(&d_ethDeviceConf), 0, sizeof(struct rte_eth_conf));
  memset(static_cast<void*>(&d_ethDeviceInfo), 0, sizeof(struct rte_eth_dev_info));
}

// ACCESSORS
inline
int AWSEnaConfig::isValid() const {
  return d_isValid;
}

inline
const std::vector<std::string>& AWSEnaConfig::dpdkArgs() const {
  return d_dpdkArgs;
}

inline
int AWSEnaConfig::memoryChannelCount() const {
  return d_memoryChannelCount;
}
  
inline
const std::vector<VCPU>& AWSEnaConfig::vcpu() const {
  return d_vcpu;
}

inline
int AWSEnaConfig::deviceId() const {
  return d_deviceId;
}

inline
const std::string& AWSEnaConfig::pciId() const {
  return d_pciId;
}

inline
int AWSEnaConfig::numaNode() const {
  return d_numaNode;
}

inline
int AWSEnaConfig::rxqCount() const {
  return d_rxqCount;
}

inline
int AWSEnaConfig::txqCount() const {
  return d_txqCount;
}

inline
int AWSEnaConfig::rxqThreadCount() const {
  return d_rxqThreadCount;
}

inline
int AWSEnaConfig::txqThreadCount() const {
  return d_txqThreadCount;
}

inline
const std::string& AWSEnaConfig::rxqPolicy() const {
  return d_rxqPolicy;
}

inline
const std::string& AWSEnaConfig::txqPolicy() const {
  return d_txqPolicy;
}

inline
const std::vector<int>& AWSEnaConfig::rxqCandidateVcpuList() const {
  return d_rxqCandidateVcpuList;
}

inline
const std::vector<int>& AWSEnaConfig::txqCandidateVcpuList() const {
  return d_txqCandidateVcpuList;
}

inline
int AWSEnaConfig::mtu() const {
  return d_mtu;
}

inline
int AWSEnaConfig::memzoneMask() const {
  return d_memzoneMask;
}

inline
int AWSEnaConfig::memzoneReserveKb() const {
  return d_memzoneReserveKb;
}

inline
const std::string& AWSEnaConfig::memzoneName() const {
  return d_memzoneName;
}

inline
const struct rte_memzone *AWSEnaConfig::memzone() const {
  return d_memzone;
}

inline
const std::string& AWSEnaConfig::mempoolPolicy() const {
  return d_mempoolPolicy;
}

inline
const std::vector<RXQ>& AWSEnaConfig::rxq() const {
  return d_rxq;
}

inline
const std::vector<TXQ>& AWSEnaConfig::txq() const {
  return d_txq;
}

inline
const std::vector<LCORE>& AWSEnaConfig::lcore() const {
  return d_lcore;
}

inline
int AWSEnaConfig::linkSpeed() const {
  return d_linkSpeed;
}

inline
int AWSEnaConfig::rxMqMask() const {
  return d_rxMqMask;
}

inline
int AWSEnaConfig::txMqMask() const {
  return d_txMqMask;
}

inline
int AWSEnaConfig::rxOffloadMask() const {
  return d_rxOffloadMask;
}

inline
int AWSEnaConfig::txOffloadMask() const {
  return d_txOffloadMask;
}

inline
int AWSEnaConfig::defaultTxFlow() const {
  return d_defaultTxFlow;
}

inline
int AWSEnaConfig::txqPrefetchThresh() const {
  return d_txqPrefetchThresh;
}

inline
int AWSEnaConfig::txqHostThresh() const {
  return d_txqHostThresh;
}

inline
int AWSEnaConfig::txqWriteBackThresh() const {
  return d_txqWriteBackThresh;
}

inline
int AWSEnaConfig::txqRsThresh() const {
  return d_txqRsThresh;
}

inline
int AWSEnaConfig::txqFreeThresh() const {
  return d_txqFreeThresh;
}

inline
int AWSEnaConfig::rxqPrefetchThresh() const {
  return d_rxqPrefetchThresh;
}

inline
int AWSEnaConfig::rxqHostThresh() const {
  return d_rxqHostThresh;
}

inline
int AWSEnaConfig::rxqWriteBackThresh() const {
  return d_rxqWriteBackThresh;
}

inline
int AWSEnaConfig::rxqFreeThresh() const {
  return d_rxqFreeThresh;
}

// MANIPULATORS
inline
void AWSEnaConfig::setIsValid(int value) {
  d_isValid = value;
}

inline
void AWSEnaConfig::setDpdkArgs(std::vector<std::string>& value) {
  d_dpdkArgs.swap(value);
}

inline
std::vector<std::string>& AWSEnaConfig::dpdkArgs(void) {
  return d_dpdkArgs;
}

inline
void AWSEnaConfig::setMemoryChannelCount(int value) {
  d_memoryChannelCount = value;
}
 
inline
void AWSEnaConfig::setVcpu(std::vector<VCPU>& value) {
  d_vcpu.swap(value);
}

inline
void AWSEnaConfig::setDeviceId(int value) {
  d_deviceId = value;
}

inline
void AWSEnaConfig::setPciId(const std::string& value) {
  d_pciId = value;
}

inline
void AWSEnaConfig::setNumaNode(int value) {
  d_numaNode = value;
}

inline
void AWSEnaConfig::setRxqCount(int value) {
  d_rxqCount = value;
}

inline
void AWSEnaConfig::setTxqCount(int value) {
  d_txqCount = value;
}

inline
void AWSEnaConfig::setRxqThreadCount(int value) {
  d_rxqThreadCount = value;
}

inline
void AWSEnaConfig::setTxqThreadCount(int value) {
  d_txqThreadCount = value;
}

inline
void AWSEnaConfig::setRxqPolicy(const std::string& value) {
  d_rxqPolicy = value;
}

inline
void AWSEnaConfig::setTxqPolicy(const std::string& value) {
  d_txqPolicy = value;
}

inline
void AWSEnaConfig::setRxqCandidateVcpuList(std::vector<int>& value) {
  d_rxqCandidateVcpuList.swap(value);
}

inline
void AWSEnaConfig::setTxqCandidateVcpuList(std::vector<int>& value) {
  d_txqCandidateVcpuList.swap(value);
}

inline
void AWSEnaConfig::setMtu(int value) {
  d_mtu = value;
}

inline
void AWSEnaConfig::setMemzoneMask(int value) {
  d_memzoneMask = value;
}

inline
void AWSEnaConfig::setMemzoneReserveKb(int value) {
  d_memzoneReserveKb = value;
}
 
inline
void AWSEnaConfig::setMemzoneName(const std::string& value) {
  d_memzoneName = value;
}

inline
void AWSEnaConfig::setMempoolPolicy(const std::string& value) {
  d_mempoolPolicy = value;
}

inline
void AWSEnaConfig::setLinkSpeed(int value) {
  d_linkSpeed = value;
}

inline
void AWSEnaConfig::setRxq(std::vector<RXQ>& value) {
  d_rxq.swap(value);
}

inline
void AWSEnaConfig::setTxq(std::vector<TXQ>& value) {
  d_txq.swap(value);
}

inline
std::vector<RXQ>& AWSEnaConfig::rxq() {
  return d_rxq;
}

inline
std::vector<TXQ>& AWSEnaConfig::txq() {
  return d_txq;
}

inline
void AWSEnaConfig::setLcore(std::vector<LCORE>& value) {
  d_lcore.swap(value);
}

inline
void AWSEnaConfig::setRxMqMask(int value) {
  d_rxMqMask = value;
}

inline
void AWSEnaConfig::setTxMqMask(int value) {
  d_txMqMask = value;
}

inline
void AWSEnaConfig::setRxOffloadMask(int value) {
  d_rxOffloadMask = value;
}

inline
void AWSEnaConfig::setTxOffloadMask(int value) {
  d_txOffloadMask = value;
}

inline
void AWSEnaConfig::setDefaultTxFlow(int value) {
  d_defaultTxFlow = value;
}

inline
rte_eth_conf *AWSEnaConfig::ethDeviceConf() {
  return &d_ethDeviceConf;
}

inline
rte_eth_dev_info *AWSEnaConfig::ethDeviceInfo() {
  return &d_ethDeviceInfo;
}

inline
void AWSEnaConfig::setMemzone(const struct rte_memzone *value) {
  d_memzone = value;
}

inline
void AWSEnaConfig::setTxqPrefetchThresh(int value) {
  d_txqPrefetchThresh = value;
}

inline
void AWSEnaConfig::setTxqHostThresh(int value) {
  d_txqHostThresh = value;
}

inline
void AWSEnaConfig::setTxqWriteBackThresh(int value) {
  d_txqWriteBackThresh = value;
}

inline
void AWSEnaConfig::setTxqRsThresh(int value) {
  d_txqRsThresh = value;
}

inline
void AWSEnaConfig::setTxqFreeThresh(int value) {
  d_txqFreeThresh = value;
}

inline
void AWSEnaConfig::setRxqPrefetchThresh(int value) {
  d_rxqPrefetchThresh = value;
}

inline
void AWSEnaConfig::setRxqHostThresh(int value) {
  d_rxqHostThresh = value;
}

inline
void AWSEnaConfig::setRxqWriteBackThresh(int value) {
  d_rxqWriteBackThresh = value;
}

inline
void AWSEnaConfig::setRxqFreeThresh(int value) {
  d_rxqFreeThresh = value;
}

// FREE OPERATORS
inline
std::ostream& operator<<(std::ostream& stream, const AWSEnaConfig& object) {
  object.print(stream);
  return stream;
}

} // Dpdk
} // Reinvent
