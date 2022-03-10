#include <dpdk/reinvent_dpdk_initaws.h>
#include <dpdk/reinvent_dpdk_uninitaws.h>
#include <dpdk/reinvent_dpdk_awsworker.h>
#include <dpdk/reinvent_dpdk_util.h>
#include <dpdk/reinvent_dpdk_stream.h>

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
#pragma GCC diagnostic pop 

#include <time.h>
#include <unistd.h>
#include <signal.h>

//
// Default burst capacities
//
unsigned RX_BURST_CAPACITY = 15;
unsigned TX_BURST_CAPACITY = 15;

unsigned SLEEP_TIME = 10;

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
  printf("   -t <integer>             optional: per TXQ burst capacity default %d\n", TX_BURST_CAPACITY);
  printf("   -r <integer>             optional: per RXQ burst capacity default %d\n", RX_BURST_CAPACITY);
  printf("   -P                       increment src/dst ports for each TX packet sent\n");
  printf("                            this option helps RSS use more queues by changing cksum\n");
  printf("   -S <integer>             number of seconds to sleep after RX/TX work done before lcore exits\n");
  printf("                            default %d seconds. See https://github.com/amzn/amzn-drivers/issues/166\n", SLEEP_TIME);
  exit(2);
}

void parseCommandLine(int argc, char **argv, bool *isServer, std::string *prefix) {
  int c, n;

  *isServer = true;
  prefix->clear();

  while ((c = getopt (argc, argv, "m:p:t:r:PS:")) != -1) {
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
      case 't':
        n = atoi(optarg);
        if (n<=0) {
          usageAndExit();
        }
        TX_BURST_CAPACITY = static_cast<unsigned>(n);
        break;
      case 'r':
        n = atoi(optarg);
        if (n<=0) {
          usageAndExit();
        }
        RX_BURST_CAPACITY = static_cast<unsigned>(n);
        break;
      case 'P':
        constantPorts=false;
        break;
      case 'S':
        n = atoi(optarg);
        if (n<=0) {
          usageAndExit();
        }
        SLEEP_TIME = static_cast<unsigned>(n);
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

  assert(config);
  assert(mbuf.size());

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
  rte_ether_addr srcMac;
  config->awsEnaConfig().txq()[txqIndex].defaultRoute().convertSrcMac(&srcMac);

  //
  // Convert dst MAC address from string to binary
  //
  rte_ether_addr dstMac;
  config->awsEnaConfig().txq()[txqIndex].defaultRoute().convertDstMac(&dstMac);

  //
  // Convert src IP address from string to binary
  //
  uint32_t srcIp(0);
  config->awsEnaConfig().txq()[txqIndex].defaultRoute().convertSrcIp(&srcIp);

  //
  // Convert dst IP address from string to binary
  //
  uint32_t dstIp(0);
  config->awsEnaConfig().txq()[txqIndex].defaultRoute().convertDstIp(&dstIp);

  //
  // UDP pseudo ports
  //
  uint16_t srcPort = rte_cpu_to_be_16(static_cast<uint16_t>(config->awsEnaConfig().txq()[txqIndex].defaultRoute().srcPort()));
  uint16_t dstPort = rte_cpu_to_be_16(static_cast<uint16_t>(config->awsEnaConfig().txq()[txqIndex].defaultRoute().dstPort()));

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

  //
  // Finally the main point: transmitting packets!
  //
  unsigned count(0);
  unsigned sequence(0);
  std::vector<rte_mbuf*> mbuf(TX_BURST_CAPACITY);

  struct timespec start;
  clock_gettime(CLOCK_REALTIME, &start);
 
  while (sequence<packetCount) {
    //
    // Allocate 'TX_BURST_CAPACITY' mbufs
    //
    for(unsigned i=0; i<TX_BURST_CAPACITY; ++i) {
      if ((mbuf[i] = rte_pktmbuf_alloc(pool))==0) {
        printf("failed to allocate mbuf\n");
        return 0;
      }
    }

    //
    // Prepare and queue for write TX_BURST_CAPACITY packets
    //
    for(unsigned i=0; i<TX_BURST_CAPACITY; ++i) {
      // Within a given mbuf:
      //
      //  +---> rte_pktmbuf_mtod_offset(mbuf, rte_ether_hdr*, 0) 
      // /
      // +---------------+--------------+-------------+-----------+
      // | rte_ether_hdr | rte_ipv4_hdr | rte_udp_hdr | TxMessage |   
      // +---------------+--------------+-------------+-----------+
      //

      //
      // Unknown purpose
      //
      rte_pktmbuf_reset_headroom(mbuf[i]);

      //
      // Prepare ether header
      //
      struct rte_ether_hdr *ethHdr = rte_pktmbuf_mtod_offset(mbuf[i], rte_ether_hdr*, 0);
      assert(ethHdr);
      ethHdr->src_addr = srcMac;
      ethHdr->dst_addr = dstMac;
      ethHdr->ether_type = ethFlow;

      //
      // Prepare IPV4 header
      //
      rte_ipv4_hdr *ip4Hdr = rte_pktmbuf_mtod_offset(mbuf[i], rte_ipv4_hdr*, sizeof(rte_ether_hdr));
      assert(ip4Hdr);
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
      rte_udp_hdr *udpHdr = rte_pktmbuf_mtod_offset(mbuf[i], rte_udp_hdr*,
        sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr));
      assert(udpHdr);
      udpHdr->src_port = srcPort;
      udpHdr->dst_port = dstPort;
      udpHdr->dgram_len = udpSize;
      udpHdr->dgram_cksum = 0;
  
      //
      // Prepare TxMessage Payload
      //
      TxMessage *payload = rte_pktmbuf_mtod_offset(mbuf[i], TxMessage*,
        sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr));
      assert(payload);
      payload->lcoreId = id;
      payload->txqId = txqIndex;
      payload->sequence = sequence++;

      //
      // Finalize mbuf in DPDK
      //
      mbuf[i]->nb_segs = 1;
      mbuf[i]->pkt_len = packetSize;
      mbuf[i]->data_len = packetSize;
      mbuf[i]->ol_flags = (RTE_MBUF_F_TX_IPV4|RTE_MBUF_F_TX_IP_CKSUM);
      mbuf[i]->l2_len = sizeof(rte_ether_hdr);
      mbuf[i]->l3_len = sizeof(rte_ipv4_hdr);
      mbuf[i]->l4_len = sizeof(rte_udp_hdr);
      mbuf[i]->packet_type = (RTE_PTYPE_L2_ETHER|RTE_PTYPE_L3_IPV4|RTE_PTYPE_L4_UDP);

      if (!constantPorts) {
        ++srcPort;
        ++dstPort;
      }
    }

    //
    // TX packets e.g. append to NIC's output queue to write to wire
    //
    unsigned sent(0);
    unsigned left(TX_BURST_CAPACITY);
    while (left) {
      unsigned txCount = rte_eth_tx_burst(deviceId, txqIndex, mbuf.data()+sent, left);
      left -= txCount;
      sent += txCount;
      count += txCount;
    }
  }

  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  uint64_t elapsedNs = timeDifference(start, now);
  double rateNsPerPacket = static_cast<double>(elapsedNs)/static_cast<double>(count);
  double bytesPerSecond = static_cast<double>(count)*static_cast<double>(packetSize)/
    (static_cast<double>(elapsedNs)/static_cast<double>(1000000000));
  double gbPerSecond = bytesPerSecond/static_cast<double>(1024)/static_cast<double>(1024)/static_cast<double>(1024);

  printf("lcoreId: %02d, txqIndex: %02d: elsapsedNs: %lu, packetsQueued: %u, packetSizeBytes: %d, nsPerPkt: %lf, bytesPerSec: %lf, gbPerSec: %lf\n", 
    id, txqIndex, elapsedNs, count, packetSize, rateNsPerPacket, bytesPerSecond, gbPerSecond);

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
  unsigned packetCount = static_cast<unsigned>(tmp);
  if (packetCount<=0||(packetCount%RX_BURST_CAPACITY)!=0) {
      REINVENT_UTIL_LOG_ERROR_VARGS("%s must be a multiple of RXQ burst count %u\n", variable.c_str(), RX_BURST_CAPACITY);
      return -1;
  }

  //
  // Finally the main point: receiving packets!
  //
  printf("lcoreId %02d rxqIndex %02d listening for %u packets\n", id, rxqIndex, packetCount);
  
  unsigned count(0);
  std::vector<rte_mbuf*> mbuf(RX_BURST_CAPACITY);

  struct timespec start;
  clock_gettime(CLOCK_REALTIME, &start);

  packetCount -=5;
  while(!terminate || count==packetCount) {
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
      //                                                                                                                
      // Find payload and print it                                                                                      
      //                                                                                                                
      TxMessage *payload = rte_pktmbuf_mtod_offset(mbuf[i], TxMessage*,
        sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr));
      REINVENT_UTIL_LOG_INFO_VARGS("id %d rxqIndex %d packet: sender: lcoreId: %d, txqId: %d, sequence: %d\n",
        id, rxqIndex, payload->lcoreId, payload->txqId, payload->sequence);
      rte_pktmbuf_free(mbuf[i]);
    }

    count += rxCount;
  }

  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  uint64_t elapsedNs = timeDifference(start, now);

  printf("lcoreId: %02d, rxqIndex: %02d: elsapsedNs: %lu, packetsReceived: %u\n", id, rxqIndex, elapsedNs, count);

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
  if (packetCount<=0||(packetCount%TX_BURST_CAPACITY)!=0) {
      REINVENT_UTIL_LOG_ERROR_VARGS("%s must be a multiple of TXQ burst count %u\n", variable.c_str(), TX_BURST_CAPACITY);
      return -1;
  }
  
  //
  // Finally enter the main processing loop passing state collected here
  //
  return clientMainLoop(id, txqIndex, config, static_cast<unsigned>(packetCount));
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
