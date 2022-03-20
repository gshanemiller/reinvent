#include <dpdk/reinvent_dpdk_initaws.h>
#include <dpdk/reinvent_dpdk_uninitaws.h>
#include <dpdk/reinvent_dpdk_awsworker.h>
#include <dpdk/reinvent_dpdk_util.h>
#include <dpdk/reinvent_dpdk_stream.h>
#include <perf/reinvent_perf_rdpmc.h>

//                                                                                                                      
// Tell GCC to not enforce '-Wpendantic' for DPDK headers                                                               
//                                                                                                                      
#pragma GCC diagnostic push                                                                                             
#pragma GCC diagnostic ignored "-Wpedantic"                                                                             
#include <rte_ether.h>
#include <rte_ip.h>
#include <rte_udp.h>
#include <rte_mbuf_ptype.h>
#include <rte_rawdev.h>
#include <rte_per_lcore.h>
#pragma GCC diagnostic pop 

#include <time.h>
#include <unistd.h>
#include <signal.h>

//
// Default burst capacities
//
unsigned RX_BURST_CAPACITY = 15;

//
// See -P argument
//
bool constantPorts(true);

//
// Set true to terminate RXQ
//
static volatile int terminate = 0;

//
// Clients send this. Servers get it
//
struct TxMessage {
  int      lcoreId;   // lcoreId which sent this message
  int      txqId;     // txqId which sent this message
  unsigned sequence;  // txqId's sequence number
  char     pad[20];   // to fill out message to 32 bytes
};

static void handle_sig(int sig) {
  switch (sig) {
    case SIGINT:
    case SIGTERM:
      terminate = 1;
      break;
  }
}

uint64_t timeDifference(timespec start, timespec end) {
  uint64_t diff = static_cast<uint64_t>(end.tv_sec)*static_cast<uint64_t>(1000000000)+static_cast<uint64_t>(end.tv_nsec);
  diff -= (static_cast<uint64_t>(start.tv_sec)*static_cast<uint64_t>(1000000000)+static_cast<uint64_t>(start.tv_nsec));
  return diff;
}

void internalExit(const Reinvent::Dpdk::AWSEnaConfig& config) {
  if (Reinvent::Dpdk::UnInitAWS::device(config)!=0) {
    REINVENT_UTIL_LOG_WARN_VARGS("Cannot uninitialize AWS ENA device\n");
  }

  if (Reinvent::Dpdk::UnInitAWS::ena()!=0) {
    REINVENT_UTIL_LOG_WARN_VARGS("Cannot uninitialize DPDK system\n");
  }
}

void onExit(const Reinvent::Dpdk::AWSEnaConfig& config) {
  //
  // Print the report link speed
  //
  rte_eth_link linkStatus;
  rte_eth_link_get_nowait(config.deviceId(), &linkStatus);
  printf("linkRate value: %u\n", linkStatus.link_speed);

  //
  // Get high level stats. Unfortuantely this can only report up
  // to RTE_ETHDEV_QUEUE_STAT_CNTRS queues which is 16. But
  // AWS ENA NICs can have up to 32 queues. So for queue stats we
  // use the xstats API 
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
  printf("   -P                       increment src/dst ports for each TX packet sent\n");
  exit(2);
}

void parseCommandLine(int argc, char **argv, bool *isServer, std::string *prefix) {
  *isServer = true;
  prefix->clear();

  int c;
  while ((c = getopt (argc, argv, "m:p:P")) != -1) {
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
      case 'P':
        constantPorts=false;
        break;
      default:
        usageAndExit();
    }
  }

  if (prefix->empty()) {
    usageAndExit();
 }
}

int clientMainLoop(int id, int txqIndex, Reinvent::Dpdk::AWSEnaWorker *config, unsigned packetCount) {
  rte_ether_addr  srcMac __rte_cache_aligned;
  rte_ether_addr  dstMac = {0};
  uint16_t        srcPort(0);
  uint16_t        dstPort(0);
  uint32_t        srcIp(0);
  uint32_t        dstIp(0);
  uint32_t        sequence(0);
  int32_t         lcoreId(0);
  int32_t         txqId(0);
  uint32_t        count(0);

  rte_mbuf       *mbuf __rte_cache_aligned;
  srcMac = {0};
  mbuf = {0};

  //
  // Device Id
  //
  const uint16_t deviceId = static_cast<uint16_t>(config->awsEnaConfig().deviceId());

  //
  // Find the TXQ's mempool
  //
  rte_mempool *pool = config->awsEnaConfig().txq()[txqIndex].mempool();
  assert(pool);

  //
  // IPV4 UDP for this demo
  //
  const uint16_t ethFlow = rte_cpu_to_be_16(static_cast<uint16_t>(config->awsEnaConfig().txq()[txqIndex].defaultFlow()));

  //
  // Convert src MAC address from string to binary
  //
  config->awsEnaConfig().txq()[txqIndex].defaultRoute().convertSrcMac(&srcMac);

  //
  // Convert dst MAC address from string to binary
  //
  config->awsEnaConfig().txq()[txqIndex].defaultRoute().convertDstMac(&dstMac);

  //
  // Convert src IP address from string to binary
  //
  config->awsEnaConfig().txq()[txqIndex].defaultRoute().convertSrcIp(&srcIp);

  //
  // Convert dst IP address from string to binary
  //
  config->awsEnaConfig().txq()[txqIndex].defaultRoute().convertDstIp(&dstIp);

  //
  // UDP pseudo ports
  //
  srcPort = rte_cpu_to_be_16(static_cast<uint16_t>(config->awsEnaConfig().txq()[txqIndex].defaultRoute().srcPort()));
  dstPort = rte_cpu_to_be_16(static_cast<uint16_t>(config->awsEnaConfig().txq()[txqIndex].defaultRoute().dstPort()));

  //
  // Total all-in packet size sent
  //
  const int packetSize = sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr)+sizeof(TxMessage);

  //
  // Size relative to IP4V header
  //
  const uint16_t ip4Size = rte_cpu_to_be_16(static_cast<uint16_t>(sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr)+sizeof(TxMessage)));

  //
  // Size relative to UDP header
  //
  const uint16_t udpSize = rte_cpu_to_be_16(static_cast<uint16_t>(sizeof(rte_udp_hdr)+sizeof(TxMessage)));

  const unsigned max = packetCount;

  lcoreId = id;
  txqId = txqIndex;

  struct timespec start;                                                                                                
  clock_gettime(CLOCK_REALTIME, &start);

  Reinvent::Perf::RdpmcSumDelta perf(false);

#ifdef PERF_ALL
  perf.start();
#endif

  while (count<max) {
#ifdef PERF_SECTION1
    perf.start();
#endif
    if (unlikely((mbuf = rte_pktmbuf_alloc(pool))==0)) {
      printf("failed to allocate mbuf\n");
      return 0;
    }
#ifdef PERF_SECTION1
    perf.stop();
#endif

    // Within a given mbuf:
    //
    //  +---> rte_pktmbuf_mtod_offset(mbuf, rte_ether_hdr*, 0) 
    // /
    // +---------------+--------------+-------------+-----------+
    // | rte_ether_hdr | rte_ipv4_hdr | rte_udp_hdr | TxMessage |   
    // +---------------+--------------+-------------+-----------+
    //

    //
    // Prepare ether header
    //
#ifdef PERF_SECTION2
    perf.start();
#endif
    struct rte_ether_hdr *ethHdr = rte_pktmbuf_mtod_offset(mbuf, rte_ether_hdr*, 0);
    ethHdr->src_addr = srcMac;
    ethHdr->dst_addr = dstMac;
    ethHdr->ether_type = ethFlow;

    //
    // Prepare IPV4 header
    //
    rte_ipv4_hdr *ip4Hdr = rte_pktmbuf_mtod_offset(mbuf, rte_ipv4_hdr*, sizeof(rte_ether_hdr));
    ip4Hdr->ihl = 0x5;
    ip4Hdr->version = 0x04;
    ip4Hdr->type_of_service = 0;
    ip4Hdr->total_length = ip4Size;
    ip4Hdr->packet_id = 0;
    ip4Hdr->fragment_offset = 0;
    ip4Hdr->time_to_live = IPDEFTTL;
    ip4Hdr->next_proto_id = IPPROTO_UDP;
    ip4Hdr->hdr_checksum = 0;
    ip4Hdr->src_addr = srcIp;
    ip4Hdr->dst_addr = dstIp;

    //
    // Prepare UDP header
    //
    rte_udp_hdr *udpHdr = rte_pktmbuf_mtod_offset(mbuf, rte_udp_hdr*,
      sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr));
    udpHdr->src_port = srcPort;
    udpHdr->dst_port = dstPort;
    udpHdr->dgram_len = udpSize;
    udpHdr->dgram_cksum = 0;
  
    //
    // Prepare TxMessage Payload
    //
    TxMessage *payload = rte_pktmbuf_mtod_offset(mbuf, TxMessage*,
      sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr));
    payload->lcoreId = lcoreId;
    payload->txqId = txqId;
    payload->sequence = ++sequence;

    //
    // Finalize mbuf in DPDK
    //
    mbuf->nb_segs = 1;
    mbuf->pkt_len = packetSize;
    mbuf->data_len = packetSize;
    mbuf->ol_flags = (RTE_MBUF_F_TX_IPV4|RTE_MBUF_F_TX_IP_CKSUM);
    mbuf->l2_len = sizeof(rte_ether_hdr);
    mbuf->l3_len = sizeof(rte_ipv4_hdr);
    mbuf->l4_len = sizeof(rte_udp_hdr);
    mbuf->packet_type = (RTE_PTYPE_L2_ETHER|RTE_PTYPE_L3_IPV4|RTE_PTYPE_L4_UDP);
#ifdef PERF_SECTION2
    perf.stop();
#endif

#ifdef PERF_SECTION3
    perf.start();
#endif
    if (unlikely(0==rte_eth_tx_burst(deviceId, txqId, &mbuf, 1))) {
      while(1!=rte_eth_tx_burst(deviceId, txqId, &mbuf, 1));
    }
    ++count;
#ifdef PERF_SECTION3
    perf.stop();
#endif
  }

#ifdef PERF_ALL
  perf.stop();
  printf("counter0: all: %lu\n", perf.sumDeltaCounter0());
  printf("counter1: all: %lu\n", perf.sumDeltaCounter1());
  printf("counter2: all: %lu\n", perf.sumDeltaCounter2());
  printf("counter3: all: %lu\n", perf.sumDeltaCounter3());
#endif

  printf("counter0 average: %lf\n", static_cast<double>(perf.sumDeltaCounter0())/static_cast<double>(max));
  printf("counter1 average: %lf\n", static_cast<double>(perf.sumDeltaCounter1())/static_cast<double>(max));
  printf("counter2 average: %lf\n", static_cast<double>(perf.sumDeltaCounter2())/static_cast<double>(max));
  printf("counter3 average: %lf\n", static_cast<double>(perf.sumDeltaCounter3())/static_cast<double>(max));

 struct timespec now;                                                                                                  
  clock_gettime(CLOCK_REALTIME, &now);                                                                                  
  uint64_t elapsedNs = timeDifference(start, now);                                                                      
                                                                                                                        
  double rateNsPerPacket = static_cast<double>(elapsedNs)/static_cast<double>(count);                             
  double pps = static_cast<double>(1000000000)/rateNsPerPacket;                                                         
  double bytesPerSecond = static_cast<double>(count)*static_cast<double>(packetSize)/                             
    (static_cast<double>(elapsedNs)/static_cast<double>(1000000000));                                                   
  double mbPerSecond = bytesPerSecond/static_cast<double>(1024)/static_cast<double>(1024);                              
  double payloadBytesPerSecond = static_cast<double>(count)*static_cast<double>(sizeof(TxMessage))/               
    (static_cast<double>(elapsedNs)/static_cast<double>(1000000000));                                                   
  double payloadMbPerSecond = payloadBytesPerSecond/static_cast<double>(1024)/static_cast<double>(1024);                
                                                                                                                        
  printf("lcoreId: %02d, txqIndex: %02d: elsapsedNs: %lu, packetsQueued: %u, packetSizeBytes: %d, payloadSizeBytes: %lu, pps: %lf, nsPerPkt: %lf, bytesPerSec: %lf, mbPerSec: %lf, mbPerSecPayloadOnly: %lf\n", id, txqIndex, elapsedNs, count, packetSize, sizeof(TxMessage), pps, rateNsPerPacket, bytesPerSecond, mbPerSecond, payloadMbPerSecond);

  return 0;
}

int serverMainLoop(int id, int rxqIndex, Reinvent::Dpdk::AWSEnaWorker *config) {
  const uint16_t deviceId = static_cast<uint16_t>(config->awsEnaConfig().deviceId());

  //
  // Get number of packets expect to receive
  //
  int rc, tmp;
  std::string variable;
  Reinvent::Dpdk::Names::make(config->envPrefix(), &variable, "%s", "RXQ_PACKET_COUNT");
  if ((rc = config->env().valueAsInt(variable, &tmp, true, 1, 100000000))!=0) {
    return rc;
  }
  if (tmp<=0||(tmp%RX_BURST_CAPACITY)!=0) {
      REINVENT_UTIL_LOG_ERROR_VARGS("%s must be a multiple of RXQ burst count %u\n", variable.c_str(), RX_BURST_CAPACITY);
      return -1;
  }
  unsigned packetCount = static_cast<unsigned>(tmp)*8;

  //
  // Finally the main point: receiving packets!
  //
  printf("lcoreId %02d rxqIndex %02d listening for %u packets\n", id, rxqIndex, packetCount);
  
  unsigned count(0);
  std::vector<rte_mbuf*> mbuf(RX_BURST_CAPACITY);

  struct timespec start;
  clock_gettime(CLOCK_REALTIME, &start);

  while(!terminate || count>=packetCount) {
    //
    // Receive up to RX_BURST_CAPACITY packets
    //
    uint16_t rxCount = rte_eth_rx_burst(deviceId, rxqIndex, mbuf.data(), RX_BURST_CAPACITY);
    if (likely(rxCount==0)) {
      continue;
    }

    // Re-start clock on first packet reception
    if (unlikely(count==0)) {
      clock_gettime(CLOCK_REALTIME, &start);
    }

    for (unsigned i=0; i<rxCount; ++i) {
      rte_pktmbuf_free(mbuf[i]);
    }

    count += rxCount;
  }

  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  uint64_t elapsedNs = timeDifference(start, now);

  const int packetSize = sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr)+sizeof(TxMessage);
  double rateNsPerPacket = static_cast<double>(elapsedNs)/static_cast<double>(count);
  double pps = static_cast<double>(1000000000)/rateNsPerPacket;
  double bytesPerSecond = static_cast<double>(count)*static_cast<double>(packetSize)/
    (static_cast<double>(elapsedNs)/static_cast<double>(1000000000));
  double mbPerSecond = bytesPerSecond/static_cast<double>(1024)/static_cast<double>(1024);
  double payloadBytesPerSecond = static_cast<double>(count)*static_cast<double>(sizeof(TxMessage))/
    (static_cast<double>(elapsedNs)/static_cast<double>(1000000000));
  double payloadMbPerSecond = payloadBytesPerSecond/static_cast<double>(1024)/static_cast<double>(1024);

  printf("lcoreId: %02d, rxqIndex: %02d: elsapsedNs: %lu, packetsQueued: %u, packetSizeBytes: %d, payloadSizeBytes: %lu, pps: %lf, nsPerPkt: %lf, bytesPerSec: %lf, mbPerSec: %lf, mbPerSecPayloadOnly: %lf\n", 
    id, rxqIndex, elapsedNs, count, packetSize, sizeof(TxMessage), pps, rateNsPerPacket, bytesPerSecond, mbPerSecond, payloadMbPerSecond);

  return 0;
}

int clientEntryPoint(int id, int txqIndex, Reinvent::Dpdk::AWSEnaWorker *config) {
  assert(config);

  //
  // Get number of packets to send from client to server
  //
  int rc, tmp;
  std::string variable;
  Reinvent::Dpdk::Names::make(config->envPrefix(), &variable, "%s", "TXQ_PACKET_COUNT");
  if ((rc = config->env().valueAsInt(variable, &tmp, true, 1, 100000000))!=0) {
    return rc;
  }
  unsigned packetCount = static_cast<unsigned>(tmp);
  
  //
  // Finally enter the main processing loop passing state collected here
  clientMainLoop(id, txqIndex, config, static_cast<unsigned>(packetCount));

  return 0;
}

int serverEntryPoint(int id, int rxqIndex, Reinvent::Dpdk::AWSEnaWorker *config) {
  assert(config);

  //
  // Go direct to main receiving loop
  //
  return serverMainLoop(id, rxqIndex, config);
}

int entryPoint(void *arg) {
  Reinvent::Dpdk::AWSEnaWorker *config = reinterpret_cast<Reinvent::Dpdk::AWSEnaWorker*>(arg);

  if (0==config) {
    REINVENT_UTIL_LOG_ERROR_VARGS("configuration pointer invalid: %p\n", arg);
    return -1;
  }

  int rc, id;
  bool tx, rx;
  int rxqIndex, txqIndex;

  //
  // What does this lcore do?
  //
  if ((rc = config->id(&id, &rx, &tx, &rxqIndex, &txqIndex))!=0) {
    return rc;
  } 
  
  rc = -1;
  if (tx) {
    rc = clientEntryPoint(id, txqIndex, config);
  } else if (rx) {
    rc = serverEntryPoint(id, rxqIndex, config);
  } else {
    REINVENT_UTIL_LOG_ERROR_VARGS("cannot classify DPDK lcore %d as RX or TX\n", id);
  }

  return rc;
}

int main(int argc, char **argv) {
  bool isServer(true);
  std::string prefix;
  std::string device("DPDK_NIC_DEVICE");

  parseCommandLine(argc, argv, &isServer, &prefix);

  Reinvent::Util::LogRuntime::resetEpoch();
  Reinvent::Util::LogRuntime::setSeverityLimit(REINVENT_UTIL_LOGGING_SEVERITY_TRACE);

  Reinvent::Util::Environment env;
  Reinvent::Dpdk::AWSEnaConfig config;

  //
  // Install signal handlers
  //
  signal(SIGINT, &handle_sig);
  signal(SIGTERM, &handle_sig);

  //
  // Initialize the AWS ENA Nic
  //
  int rc = Reinvent::Dpdk::InitAWS::enaUdp(device, prefix, &env, &config);
  REINVENT_UTIL_LOG_INFO(config << std::endl);
  if (rc!=0) {
    REINVENT_UTIL_LOG_FATAL_VARGS("Cannot initialize AWS ENA device rc=%d\n", rc);                                      
    onExit(config);
    return 1;
  }

  //
  // Setup and run AWSEnaWorkers. Now if the enviromment is setup right, the client
  // will only configure RXQ work (TX work turned off) and the server will only see
  // the configured RX work (TX work turned off). In turn that means the client only
  // will ever fall into 'txEntryPoint' and 'rxEntryPoint' for server. As such this
  // code file can be used for both modes. It's the enviroment 'prefix' that keeps
  // the code paths separate.
  {
    Reinvent::Dpdk::AWSEnaWorker worker(prefix, env, config);
    REINVENT_UTIL_LOG_INFO_VARGS("launching DPDK worker threads\n");
    if ((rc = rte_eal_mp_remote_launch(entryPoint, static_cast<void*>(&worker), CALL_MAIN))!=0) {
      REINVENT_UTIL_LOG_FATAL_VARGS("Cannot launch DPDK cores rc=%d\n", rc);
    } else {
      REINVENT_UTIL_LOG_INFO_VARGS("waiting for DPDK worker threads to stop\n");
      rte_eal_mp_wait_lcore();
    }
  }
  
  //
  // Cleanup and exit from main thread
  //
  onExit(config);

  return 0;
}
