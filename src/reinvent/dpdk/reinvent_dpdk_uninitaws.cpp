#include <dpdk/reinvent_dpdk_uninitaws.h>

#include <dpdk/reinvent_dpdk_names.h>
#include <util/reinvent_util_errno.h>

#include <rte_eal.h>

#include <string.h>                                                                                                     

namespace Reinvent {

int Dpdk::UnInitAWS::device(const AWSEnaConfig& config) {
  int rc;

  //
  // Stop device
  //
  if ((rc = rte_eth_dev_stop(config.deviceId()))!=0) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (rc==0), "DPDK AWS ENA stop device",
      rte_strerror(rc), rc);
  }

  if (config.mempoolPolicy()!=Dpdk::Names::SHARED) {
    //
    // Free per queue RX mempools
    //
    for (unsigned i=0; i<config.rxq().size(); ++i) {
      rte_mempool *pool = config.rxq()[i].mempool();
      if (pool) {
        rte_mempool_free(pool);
      }
    }

    //
    // Free per queue TX mempools
    //
    for (unsigned i=0; i<config.txq().size(); ++i) {
      rte_mempool *pool = config.txq()[i].mempool();
      if (pool) {
        rte_mempool_free(pool);
      }
    }
  } else {
    //
    // Free mempool shared by all queues
    //
    rte_mempool *pool = 0;

    for (unsigned i=0; i<config.rxq().size(); ++i) {
      rte_mempool *pool = config.rxq()[i].mempool();
      if (pool) {
        break;
      }
    }

    //
    // RX may have been turned off so check TX
    //
    if (pool==0) {
      for (unsigned i=0; i<config.rxq().size(); ++i) {
        rte_mempool *pool = config.txq()[i].mempool();
        if (pool) {
          break;
        }
      }
    }

    if (pool) {
      rte_mempool_free(pool);
    }
  }

  //
  // Free memzone
  //
  if (config.memzone()) {
    if ((rc = rte_memzone_free(config.memzone()))!=0) {
      REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (rc==0), "DPDK AWS ENA device memzone cleanup",
        rte_strerror(rc), rc);
    }
  }

  return rc;
}                                                                       

int Dpdk::UnInitAWS::ena() {
  int rc;

  if ((rc = rte_eal_cleanup())!=0) {
    REINVENT_UTIL_ERRNO_RETURN(Util::Errno::REINVENT_UTIL_ERRNO_API, (rc==0), "DPDK AWS ENA deinitialization",
      rte_strerror(rc), rc);
  }

  return rc;
}

} // namespace Reinvent
