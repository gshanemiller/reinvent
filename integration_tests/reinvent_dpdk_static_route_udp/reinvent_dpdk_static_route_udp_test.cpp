#include <dpdk/reinvent_dpdk_init.h>
#include <dpdk/reinvent_dpdk_worker.h>
#include <dpdk/reinvent_dpdk_util.h>
#include <dpdk/reinvent_dpdk_stream.h>

#include <reinvent_dpdk_static_route_udp_test_common.h>

#include <unistd.h>
#include <signal.h>

//                                                                                                                      
// Set true to client/server
//                                                                                                                      
volatile int terminate = 0; 

static void handle_sig(int sig) {                                                                                       
  switch (sig) {                                                                                                        
    case SIGINT:                                                                                                        
    case SIGTERM:                                                                                                       
      terminate = 1;                                                                                                    
      break;                                                                                                            
  }                                                                                                                     
}

void internalExit(const Reinvent::Dpdk::Config& config) {
  if (Reinvent::Dpdk::Init::stopEna(config)!=0) {
    REINVENT_UTIL_LOG_WARN_VARGS("Cannot uninitialize DPDK ENA device\n");
  }

  if (Reinvent::Dpdk::Init::stopDpdk()!=0) {
    REINVENT_UTIL_LOG_WARN_VARGS("Cannot uninitialize DPDK system\n");
  }
}

void onExit(const Reinvent::Dpdk::Config& config) {
  //
  // Print the report link speed
  //
  rte_eth_link linkStatus;
  rte_eth_link_get_nowait(config.deviceId(), &linkStatus);
  printf("linkRate value: %u\n", linkStatus.link_speed);

  //
  // Get high level stats. Unfortuantely this can only report up
  // to RTE_ETHDEV_QUEUE_STAT_CNTRS queues which is 16. But
  // most DPDK NICs can have up to 32 queues or more. So for queue
  // stats we use the xstats API 
  //
  rte_eth_stats stats;
  memset(&stats, 0, sizeof(stats));
  rte_eth_stats_get(config.deviceId(), &stats);

  //
  // Dump high level stats
  //
  printf("in  packets : %lu\n", stats.ipackets);
  printf("out packets : %lu\n", stats.opackets);
  printf("in  bytes   : %lu\n", stats.ibytes);
  printf("out bytes   : %lu\n", stats.obytes);
  printf("missed pkts : %lu\n", stats.imissed);
  printf("in err pkts : %lu\n", stats.ierrors);
  printf("out err pkts: %lu\n", stats.oerrors);
  printf("rx allc errs: %lu\n", stats.rx_nombuf);

  rte_eth_xstat *xstats;
  rte_eth_xstat_name *xstats_names;
  int len = rte_eth_xstats_get(config.deviceId(), 0, 0);
  if (len < 0) {
    printf("warn: cannot get xstats\n");
    internalExit(config);
  }

  xstats = static_cast<rte_eth_xstat*>(calloc(len, sizeof(*xstats)));
  if (xstats == 0) {
    printf("warn: out of memory\n");
    internalExit(config);
  }
  int ret = rte_eth_xstats_get(config.deviceId(), xstats, len);
  if (ret<0||ret>len) {
    free(xstats);
    printf("warn: rte_eth_xstats_get unexpected result\n");
    internalExit(config);
  }
  xstats_names = static_cast<rte_eth_xstat_name*>(calloc(len, sizeof(*xstats_names)));
  if (xstats_names == 0) {
    free(xstats);
    printf("warn: out of memory\n");
    internalExit(config);
  }
  ret = rte_eth_xstats_get_names(config.deviceId(), xstats_names, len);
  if (ret<0||ret > len) {
    free(xstats);
    free(xstats_names);
    printf("warn: rte_eth_xstats_get_names unexpected result\n");
    internalExit(config);
  }
  for (int i=0; i<len; i++) {
    printf("%-32s: %lu\n", xstats_names[i].name, xstats[i].value);
  }

  free(xstats);
  free(xstats_names);

  internalExit(config);
}

void usageAndExit() {
  printf("reinvent_dpdk_udp_integration_test.tsk -m <client|server> -p <env-var-prefix>\n");
  printf("   -m <client|server>       optional (default server). mode to run task in.\n");
  printf("   -p <string>              required: non-empty ENV variable prefx name with config\n");
  exit(2);
}

void parseCommandLine(int argc, char **argv, bool *isServer, std::string *prefix) {
  int c;

  *isServer = true;
  prefix->clear();

  while ((c = getopt (argc, argv, "m:p:")) != -1) {
    switch(c) {
      case 'm':
        if (strcmp(optarg, "server")==0) {
          *isServer = true;
        } else if (strcmp(optarg, "client")==0) {
          *isServer = false;
        } else {
          usageAndExit();
        }
        break;
      case 'p':
        *prefix = optarg;
        break;
      default:
        usageAndExit();
    }
  }

  if (prefix->empty()) {
    usageAndExit();
  }
}

int main(int argc, char **argv) {
  bool isServer(true);
  std::string prefix;
  std::string device("DPDK_NIC_DEVICE");

  parseCommandLine(argc, argv, &isServer, &prefix);

  Reinvent::Util::LogRuntime::resetEpoch();
  Reinvent::Util::LogRuntime::setSeverityLimit(REINVENT_UTIL_LOGGING_SEVERITY_TRACE);

  Reinvent::Util::Environment env;
  Reinvent::Dpdk::Config config;

  //
  // Install signal handlers
  //
  signal(SIGINT, &handle_sig);
  signal(SIGTERM, &handle_sig);

  //
  // Initialize the DPDK ENA Nic
  //
  int rc = Reinvent::Dpdk::Init::startEna(device, prefix, &env, &config);
  REINVENT_UTIL_LOG_INFO(config << std::endl);
  if (rc!=0) {
    REINVENT_UTIL_LOG_FATAL_VARGS("Cannot initialize DPDK ENA device rc=%d\n", rc);                                      
    if (Reinvent::Dpdk::Init::stopEna(config)!=0) {
      REINVENT_UTIL_LOG_WARN_VARGS("Cannot uninitialize DPDK ENA device\n");
    }
    return 1;
  }

  // Run in specified mode
  if (isServer) {
    rc = serverEntryPoint(prefix, env, config);
  } else {
    rc = clientEntryPoint(prefix, env, config);
  }

  REINVENT_UTIL_LOG_INFO_VARGS("waiting for DPDK worker threads to stop\n");
  rte_eal_mp_wait_lcore();
  REINVENT_UTIL_LOG_INFO_VARGS("DPDK worker threads done\n");

  //
  // Cleanup and exit from main thread
  //
  onExit(config);

  return rc;
}
