#pragma once

// Purpose: Application state for code run on DPDK prepared VCPUs (lcores)
// 
// Classes:
//   Dpdk::AWSEnaWorker: Base class providing minimal application runtime support for RXQ/TXQ work
//
// Thread Safety: thread-safe.
//
// Exception Policy: No exceptions
//
// Once an AWS ENA device is DPDK initialized the next logical step is telling DPDK what code to run on each VCPU
// (lcore) it prepared. That setup comes in two major pieces: RX work, and TX work. The canonical way is running the
// API method 'rte_eal_mp_remote_launch', which accepts a function pointer and 'void*' argument pointer. This method
// instructs DPDK to run the specified function pointer inside/on each lcore. On entry to that function, one can use
// the argument pointer and DPDK lcore library routines to discover the lcore's purpose and, from there, call another
// function who's purpose is better suited to the intent of a particular lcore. See DPDK documentation:
//
//   https://doc.dpdk.org/guides/prog_guide/env_abstraction_layer.html#initialization-and-core-launching
//
// where this information is mentioned, however unfortuantely, without sufficient detail. In this way, and baring the
// need to create additional threads depending on the application's roles and responsibilities, the number of runtime
// threads and thread responsibility is clearly setout in the application's configuration:
//
// * There are N configured lcores each handling one RXQ
// * There are M configured lcores each handling one TXQ
// * For a total of N+M lcores which DPDK will make
// * The lcores may or not be pinned to the same HW core
// * 'rte_eal_mp_remote_launch' creates the N+M threads pinned to their configure HW core then runs them
// * 'rte_eal_mp_remote_launch' provides a function pointer, and void * argument pointer so that, when the
//   each thread is made, application code can run on the lcore
//
// A pointer to a 'AWSEnaWorker' object is intended for the 'void*' argument in the call to 'rte_eal_mp_remote_launch'.
// This class holds the AWS ENA configuration, and a pointer to 'Reinvent::Util::Environment', which was used by the
// application to prepare the AWS ENA config. Together these objects hold the majority of data that can used to elicit
// a particular lcore's purpose. Subclasses then can then extend this functionality depending on the application's
// need.
//
// The function pointer argument in 'rte_eal_mp_remote_launch' does not appear here nor does it appear in reinvent
// library code. It is found in the application code itself. The contents of that function pointer almost always works
// out to the following basic steps:
//
// * Discover if the lcore is for RX or TX using AWSEnaWorker::id(...)
// * Application specific setup
// * Entry into the main processing loop for the lcore 
// * Detection of a termination condition so that the function pointer exits. Without termination the application's
//   main thread can't stop.
//
// AWSEnaWorker is equipped with an 'id' accessor. This 'id' reports the caller's DPDK zero-based lcore index, whether
// or not the lcore is for RXQ (TXQ), and the 0-based RXQ (TXQ) number. These identifiers can be then used to access
// the config's 'lcore, rxq, txq' attributes for relavent information. For example: if a config runs 4 DPDK lcores with
// 2 RXQs and 2 TXQs 'id' will identify each of the four threads like this:
//
//   +--------------------+------+------+-------+-------+
//   | DPDK lcore #       | isRX | isTX | RXQ # | TXQ # |
//   +--------------------+------+------+-------+-------+
//   | 0                  | Yes  | No   | 0     | -1    |
//   +--------------------+------+------+-------+-------+
//   | 1                  | Yes  | No   | 1     | -1    |   Information provided by 'id' method in this class
//   +--------------------+------+------+-------+-------+   which is used to access config's lcore attribute
//   | 2                  | No   | Yes  | -1    | 0     |   which can be followed into config's rxq/txq members
//   +--------------------+------+------+-------+-------+
//   | 3                  | No   | Yes  | -1    | 1     |
//   +--------------------+------+------+-------+-------+
//
// The application's state is collected into an 'AWSEnaWorker' object which,
//
// * holds a 'Reinvent::Util::Environment' reference in attribute 'env'
// * holds a 'Reinvent::Dpdk::AWSEnaConfig' reference in attribute 'awsEnaConfig'
//
// Additional state can be done several ways:
//
// * Global variables, but note MT-safe challenge
// * Local variables in the rte_eal_mp_remote_launch called function
// * Subclassing AWSEnaWorker adding state and MT-safe manipulators
// * Adding thread-local storage.
//
// Note the 'rte_eal_mp_remote_launch' approach provides only a single function pointer, and a single argument pointer
// (putatively a 'AWSEnaWorker' object) to all lcores. Since the lcores run concurrently access to the single
// 'AWSEnaWorker' object is by construction also MT safe. 'AWSEnaWorker' facilitates concurrent MT-safe access by:
//
// * Attribute 'envPrefix' is const
// * Attribute 'env' is itself MT safe
// * Attribute 'awsEnaConfig' is const and can't be changed
// * All other manipulators are mutex protected and MT-safe
//
// Subclasses may use AWSEnaWorker's mutex manipulator to stay MT-safe

#include <dpdk/reinvent_dpdk_initaws.h>

//                                                                                                                      
// Tell GCC to not enforce '-Wpendantic' for DPDK headers
//
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <rte_lcore.h>
#pragma GCC diagnostic pop

#include <mutex>
#include <assert.h>

namespace Reinvent {
namespace Dpdk {

class AWSEnaWorker {
  // DATA
  const std::string&                    d_envPrefix;
  Reinvent::Util::Environment&          d_env;
  const Reinvent::Dpdk::AWSEnaConfig&   d_awsEnaConfig;
  std::mutex                            d_mux;

public:
  // CREATORS
  AWSEnaWorker() = delete;
    // Default constructor not provided
 
  AWSEnaWorker(const std::string& prefix, Reinvent::Util::Environment& env,
    const Reinvent::Dpdk::AWSEnaConfig& awsEnaConfig);
    // Create AWSEnaWorker with specified 'envPrefix, env, awsEnaConfig'. 'txqMempool' has size zero on return.

  AWSEnaWorker(const AWSEnaWorker& other) = delete;
    // Copy constructor not provided

  ~AWSEnaWorker();
    // Destroy this object

  // ACCESSORS
  const std::string& envPrefix() const;
    // Provide a non-modifiable reference to the 'envPrefix' attribute provided at construction time

  Reinvent::Util::Environment& env() const;
    // Provide a non-modifiable reference to the 'env' attribute provided at construction time

  const Reinvent::Dpdk::AWSEnaConfig& awsEnaConfig() const;
    // Provide a non-modifiable reference to the 'awsEnaConfigenv' object provided at construction time

  int id(int *value, bool *isRX, bool *isTX, int *rxqIndex, int *txqIndex);
    // Return 0 and set specified 'value' to the DPDK lcore zero based index of the caller, setting 'isRX' true if 'id'
    // is a RX lcore (false otherwise), setting 'isTX' true if 'id' is a TX lcore (false otherwise), setting 'rxqIndex'
    // to the zero-based index of the RXQ (-1 if 'id' is for TX), and 'txqIndex' to the zero-based index of the TXQ
    // (-1 if 'id' is for RX). Note that 'value' can be used as an index on config's 'lcore' attribute, 'rxqIndex' in
    // 'rxq' attribute, and 'txqIndex' on 'txq' attribute. Also note this method uses DPDK API 'rte_lcore_id' so it's
    // important callers invoke this method from a bonafide lcore thread to avoid errors.

  AWSEnaWorker& operator=(const AWSEnaWorker& rhs) = delete;
    // Assignment operator not provided
};

// INLINE DEFINITIONS

// CREATORS
inline
AWSEnaWorker::AWSEnaWorker(const std::string& envPrefix, Reinvent::Util::Environment& env,
  const Reinvent::Dpdk::AWSEnaConfig& awsEnaConfig)
: d_envPrefix(envPrefix)
, d_env(env)
, d_awsEnaConfig(awsEnaConfig)
{
}

inline
AWSEnaWorker::~AWSEnaWorker() {
  std::lock_guard<std::mutex> lock(d_mux);
}

// ACCESSORS
inline
const std::string& AWSEnaWorker::envPrefix() const {
  return d_envPrefix;
}

inline
Reinvent::Util::Environment& AWSEnaWorker::env() const {
  return d_env;
}

inline
const Reinvent::Dpdk::AWSEnaConfig& AWSEnaWorker::awsEnaConfig() const {
  return d_awsEnaConfig;
}

inline
int AWSEnaWorker::id(int *val, bool *isRX, bool *isTX, int *rxqIndex, int *txqIndex) {
  assert(val);
  assert(isRX);
  assert(isTX);
  assert(rxqIndex);
  assert(txqIndex);

  std::lock_guard<std::mutex> lock(d_mux);
  
  //
  // Default values
  //
  *val = -1;
  *isRX = false;
  *isTX = false;
  *rxqIndex = -1;
  *txqIndex = -1;

  //
  // Get the caller's DPDK provide lcoreId
  //
  unsigned n(rte_lcore_id());
  int value = static_cast<int>(n);

  if (n==LCORE_ID_ANY) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (n!=LCORE_ID_ANY),
      "Caller DPDK zero-based lcore index lookup yields", value, static_cast<int>(rte_lcore_count()), "lcores are known to DPDK. Likely called from bad thread");
  }

  if (value<0||n>=d_awsEnaConfig.lcore().size()) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (value>=0&&n<d_awsEnaConfig.lcore().size()),
      "Caller DPDK zero-based lcore index lookup yields", value, static_cast<int>(rte_lcore_count()), "lcores are known to DPDK. Likely called from bad thread");
  }

  //
  // Assign lcoreId
  //
  *val = value;

  // 
  // Assign role and queue index
  //
  bool assigned = false;
  if (d_awsEnaConfig.lcore()[n].role().size()>0) {
    if (d_awsEnaConfig.lcore()[n].role()[0]==Reinvent::Dpdk::LCORE::e_LCORE_TXQ) {
      *isTX = true;
      *txqIndex = d_awsEnaConfig.lcore()[n].queue()[0];
      assigned = true;
    } else if (d_awsEnaConfig.lcore()[n].role()[0]==Reinvent::Dpdk::LCORE::e_LCORE_RXQ) {
      *isRX = true;
      *rxqIndex = d_awsEnaConfig.lcore()[n].queue()[0];
      assigned = true;
    }
  }

  if (!assigned) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_NO_RESOURCE, (assigned),
      "Caller DPDK zero-based lcore index lookup yields", value, static_cast<int>(rte_lcore_count()), "none of which could be identified as RXQ or TXQ");
  }

  return 0;
}

} // Dpdk
} // Reinvent
