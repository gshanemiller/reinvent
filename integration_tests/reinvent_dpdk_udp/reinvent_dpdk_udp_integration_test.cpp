#include <dpdk/reinvent_dpdk_init.h>
#include <dpdk/reinvent_dpdk_worker.h>
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
#include <rte_flow.h>
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

const unsigned REPORT_COUNT = 100000;

//
// See -P argument
//
bool constantPorts(true);

//
// See -f argument
//
bool flowControl(false);

//
// Set true to terminate RXQ
//
static volatile int terminate = 0;

//
// For flow control
//
const unsigned RXQ_RULES = 8;
struct rte_flow *flow[RXQ_RULES] = {0};

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

int disableFlowControl() {
  struct rte_flow_error error;
  const uint16_t deviceId = 0;
  for (unsigned rxq = 0; rxq<RXQ_RULES; ++rxq) {
    if (flow[rxq]) {
      rte_flow_destroy(deviceId, flow[rxq], &error);
      REINVENT_UTIL_LOG_DEBUG_VARGS("flowControl for rxqIndex %u destroyed\n", rxq);
      flow[rxq] = 0;
    }
  }
  return 0;
}

int enableFlowControl() {
  for (unsigned rxq = 0; rxq < RXQ_RULES; ++rxq) {
    // Matching packets must be incoming. Note memset also initializes group,
    // and priority to 0
    struct rte_flow_attr attr;
    memset(&attr, 0, sizeof(struct rte_flow_attr));
    attr.ingress = 1;

    // Matching packets must be IP UDP
    struct rte_flow_item pattern[4];
    memset(pattern, 0, sizeof(pattern));
    pattern[0].type = RTE_FLOW_ITEM_TYPE_ETH;
    pattern[1].type = RTE_FLOW_ITEM_TYPE_IPV4;
    pattern[2].type = RTE_FLOW_ITEM_TYPE_UDP;
    pattern[3].type = RTE_FLOW_ITEM_TYPE_END;

    // Set UDP port matching spec, last, mask for UDP pattern[2]:
    // Behavior is defined provided the UDP destination port is
    // non-zero with exactly 1 of the following bits set:
    // +--------+------------+--------------+-----------------------+
    // | bit #  | Mask value | RXQ assigned | Example UDP Dest Port |
    // +--------+------------+--------------+-----------------------+
    // | 0      | 1  (1<<0)  | 0            | 1                     |
    // | 1      | 2  (1<<1)  | 1            | 2                     |
    // | 2      | 4  (1<<2)  | 2            | 4                     |
    // | ...    | ...        | ...          | ...                   |
    // | 7      | 128 (1<<7) | 7            | 128                   |
    // +--------+------------+--------------+-----------------------+
    struct rte_flow_item_udp portSpec;
    struct rte_flow_item_udp portMask;
    memset(&portMask, 0, sizeof(struct rte_flow_item_udp));
    memset(&portSpec, 0, sizeof(struct rte_flow_item_udp));
    portMask.hdr.src_port = 0;        // don't care; mask it to 0 always
    portMask.hdr.dst_port = (1<<rxq); // bit#rxq ON all other bits OFF
    portSpec.hdr.src_port = 0;        // match anything
    portSpec.hdr.dst_port = (1<<rxq); // match anything with bit#rxq ON
    pattern[2].mask = &portMask;
    pattern[2].spec = &portSpec;

    // paterns 0 (ETH), 1 (IPV4) do not get a mask/spec
    // Everything is matched by default there and no action taken

    // Setup the RXQ queue we want to assign
    struct rte_flow_action_queue queue;
    memset(&queue, 0, sizeof(rte_flow_action_queue));
    queue.index = rxq; // this is the queue we're assigning

    // Setup the action and associate the queue assignment to it
    struct rte_flow_action action[2];
    memset(action, 0, sizeof(action));
    action[0].type = RTE_FLOW_ACTION_TYPE_QUEUE;
    action[0].conf = &queue;
    action[1].type = RTE_FLOW_ACTION_TYPE_END;

    // Validate the rule on port 0 and create it
    struct rte_flow_error error;
    const uint16_t deviceId = 0;
    int rc = rte_flow_validate(deviceId, &attr, pattern, action, &error);
    if (0==rc) {
      flow[rxq] = rte_flow_create(0, &attr, pattern, action, &error);
      REINVENT_UTIL_LOG_DEBUG_VARGS("flowControl for rxqIndex %u created\n", rxq);
    } else {
      REINVENT_UTIL_LOG_ERROR_VARGS("flowControl for rxqIndex %u failed: %s\n", rxq, error.message);
      return -1;
    }
  }

  return 0;
}

uint64_t timeDifference(timespec start, timespec end) {
  uint64_t diff = static_cast<uint64_t>(end.tv_sec)*static_cast<uint64_t>(1000000000)+static_cast<uint64_t>(end.tv_nsec);
  diff -= (static_cast<uint64_t>(start.tv_sec)*static_cast<uint64_t>(1000000000)+static_cast<uint64_t>(start.tv_nsec));
  return diff;
}

void internalExit(const Reinvent::Dpdk::Config& config) {
  if (flowControl) {
    disableFlowControl();
  }

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
  printf("   -r <integer>             optional: per RXQ burst capacity default %d\n", RX_BURST_CAPACITY);
  printf("   -P                       increment src/dst ports for each TX packet sent\n");
  printf("                            this option helps RSS use more queues by changing hash\n");
  printf("   -f                       enable flow control for 8 RXQs. don't use with RSS. flow\n");
  printf("                            control config not currently read from environment\n");
  exit(2);
}

void parseCommandLine(int argc, char **argv, bool *isServer, std::string *prefix) {
  int c, n;

  *isServer = true;
  prefix->clear();

  while ((c = getopt (argc, argv, "m:p:r:Pf")) != -1) {
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
      case 'f':
        flowControl=true;
        break;
      default:
        usageAndExit();
    }
  }

  if (prefix->empty()) {
    usageAndExit();
  }
}

int clientMainLoop(int id, int txqIndex, Reinvent::Dpdk::Worker *worker, unsigned packetCount) {
  assert(config);

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
  uint32_t        stalledTx(0);
  uint32_t        ip_cksum;
  uint16_t       *ptr16;
  srcMac = {0};

  // Arbitrarily pick the first default route. ENA Init ensures we have at
  // least one valid route with good mac, ip, and port identifiers
  const Reinvent::Dpdk::IPV4Route& defaultRoute(worker->config().defaultRoute()[0]);

  //
  // Device Id
  //
  const uint16_t deviceId = static_cast<uint16_t>(worker->config().deviceId());

  //
  // Find the TXQ's mempool
  //
  rte_mempool *pool = worker->config().txq()[txqIndex].mempool();
  assert(pool);

  //
  // IPV4 UDP for this demo
  //
  const uint16_t ethFlow = rte_cpu_to_be_16(static_cast<uint16_t>(worker->config().txq()[txqIndex].defaultFlow()));

  //
  // Convert src MAC address from string to binary
  //
  defaultRoute.convertSrcMac(&srcMac);

  //
  // Convert dst MAC address from string to binary
  //
  defaultRoute.convertDstMac(&dstMac);

  //
  // Convert src IP address from string to binary
  //
  defaultRoute.convertSrcIp(&srcIp);

  //
  // Convert dst IP address from string to binary
  //
  defaultRoute.convertDstIp(&dstIp);

  //
  // UDP pseudo ports
  //
  srcPort = rte_cpu_to_be_16(static_cast<uint16_t>(defaultRoute.srcPort()));
  dstPort = rte_cpu_to_be_16(static_cast<uint16_t>(defaultRoute.dstPort()));

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

  struct timespec start;
  clock_gettime(CLOCK_REALTIME, &start);

  lcoreId = id;
  txqId = txqIndex;

  rte_mbuf * mbuf(0);
 
  while (likely(count<packetCount)) {
    if ((mbuf = rte_pktmbuf_alloc(pool))==0) {
      printf("failed to allocate mbuf\n");
      return 0;
    }

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
    // Compute IP checksum
    //
    #pragma GCC diagnostic push                                                                                             
    #pragma GCC diagnostic ignored "-Waddress-of-packed-member"                                                                             
    ptr16 = reinterpret_cast<uint16_t*>(ip4Hdr);
    #pragma GCC diagnostic pop 
    ip_cksum = 0;
    ip_cksum += ptr16[0]; ip_cksum += ptr16[1];
    ip_cksum += ptr16[2]; ip_cksum += ptr16[3];
    ip_cksum += ptr16[4];
    ip_cksum += ptr16[6]; ip_cksum += ptr16[7];
    ip_cksum += ptr16[8]; ip_cksum += ptr16[9];
	  // Reduce 32 bit checksum to 16 bits and complement it.
    ip_cksum = ((ip_cksum & 0xFFFF0000) >> 16) + (ip_cksum & 0x0000FFFF);
    if (ip_cksum > 65535) ip_cksum -= 65535;
    ip_cksum = (~ip_cksum) & 0x0000FFFF;
    if (ip_cksum == 0) ip_cksum = 0xFFFF;
    ip4Hdr->hdr_checksum = static_cast<uint16_t>(ip_cksum);

    //
    // Finalize mbuf in DPDK
    //
    mbuf->nb_segs = 1;
    mbuf->pkt_len = packetSize;
    mbuf->data_len = packetSize;
    mbuf->ol_flags = 0;

    if (unlikely(0==rte_eth_tx_burst(deviceId, txqId, &mbuf, 1))) {
      do {
        ++stalledTx;
      } while(1!=rte_eth_tx_burst(deviceId, txqId, &mbuf, 1));
    }
    ++count;

    if (likely(!constantPorts)) {
      ++dstPort;
    }
  }

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

  printf("lcoreId: %02d, txqIndex: %02d: elsapsedNs: %lu, packetsQueued: %u, packetSizeBytes: %d, payloadSizeBytes: %lu, pps: %lf, nsPerPkt: %lf, bytesPerSec: %lf, mbPerSec: %lf, mbPerSecPayloadOnly: %lf stalledTx %u\n", 
    id, txqIndex, elapsedNs, count, packetSize, sizeof(TxMessage), pps, rateNsPerPacket, bytesPerSecond, mbPerSecond, payloadMbPerSecond, stalledTx);

  return 0;
}

int serverMainLoop(int id, int rxqIndex, Reinvent::Dpdk::Worker *worker) {
  const uint16_t deviceId = static_cast<uint16_t>(worker->config().deviceId());

  //
  // Finally the main point: receiving packets!
  //
  printf("lcoreId %02d rxqIndex %02d listening for packets\n", id, rxqIndex);
  
  uint32_t count(0);
  uint32_t stalledRx(0);
  std::vector<rte_mbuf*> mbuf(RX_BURST_CAPACITY);

  struct timespec start;
  clock_gettime(CLOCK_REALTIME, &start);

  while(!terminate) {
    //
    // Receive up to RX_BURST_CAPACITY packets
    //
    uint16_t rxCount = rte_eth_rx_burst(deviceId, rxqIndex, mbuf.data(), RX_BURST_CAPACITY);
    if (likely(rxCount==0)) {
      ++stalledRx;
      continue;
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

    if ((count += rxCount)>=REPORT_COUNT) {
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

      printf("lcoreId: %02d, rxqIndex: %02d: elsapsedNs: %lu, packetsDequeued: %u, packetSizeBytes: %d, payloadSizeBytes: %lu, pps: %lf, nsPerPkt: %lf, bytesPerSec: %lf, mbPerSec: %lf, mbPerSecPayloadOnly: %lf, stalledRx: %u\n", 
        id, rxqIndex, elapsedNs, count, packetSize, sizeof(TxMessage), pps, rateNsPerPacket, bytesPerSecond, mbPerSecond, payloadMbPerSecond, stalledRx);
    
      count = stalledRx = 0;

      clock_gettime(CLOCK_REALTIME, &start);
    }
  }

  return 0;
}

int clientEntryPoint(int id, int txqIndex, Reinvent::Dpdk::Worker *worker) {
  assert(config);

  //
  // Get number of packets to send from client to server
  //
  int rc, tmp;
  std::string variable;
  Reinvent::Dpdk::Names::make(worker->envPrefix(), &variable, "%s", "TXQ_PACKET_COUNT");
  if ((rc = worker->env().valueAsInt(variable, &tmp, true, 1, 100000000))!=0) {
    return rc;
  }
  unsigned packetCount = static_cast<unsigned>(tmp);
  
  //
  // Finally enter the main processing loop passing state collected here
  //
  return clientMainLoop(id, txqIndex, worker, packetCount);
}

int serverEntryPoint(int id, int rxqIndex, Reinvent::Dpdk::Worker *worker) {
  assert(config);

  //
  // Run main receiving loop
  //
  return serverMainLoop(id, rxqIndex, worker);
}

int entryPoint(void *arg) {
  Reinvent::Dpdk::Worker *worker= reinterpret_cast<Reinvent::Dpdk::Worker*>(arg);

  if (0==worker) {
    REINVENT_UTIL_LOG_ERROR_VARGS("configuration pointer invalid: %p\n", arg);
    return -1;
  }

  int rc, id;
  bool tx, rx;
  int rxqIndex, txqIndex;

  //
  // What does this lcore do?
  //
  if ((rc = worker->id(&id, &rx, &tx, &rxqIndex, &txqIndex))!=0) {
    return rc;
  } 
  
  rc = -1;
  if (tx) {
    rc = clientEntryPoint(id, txqIndex, worker);
    REINVENT_UTIL_LOG_INFO_VARGS("clientEntryPoint rc=%d\n", rc);
  } else if (rx) {
    rc = serverEntryPoint(id, rxqIndex, worker);
    REINVENT_UTIL_LOG_INFO_VARGS("serverEntryPoint rc=%d\n", rc);
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

  if (flowControl) {
    if ((rc = enableFlowControl())!=0) {
      onExit(config);
    }
  }

  //
  // Setup and run Workers. Now if the enviromment is setup right, the client
  // will only configure RXQ work (TX work turned off) and the server will only see
  // the configured RX work (TX work turned off). In turn that means the client only
  // will ever fall into 'txEntryPoint' and 'rxEntryPoint' for server. As such this
  // code file can be used for both modes. It's the enviroment 'prefix' that keeps
  // the code paths separate.
  {
    Reinvent::Dpdk::Worker worker(prefix, env, config);
    REINVENT_UTIL_LOG_INFO_VARGS("launching DPDK worker threads\n");
    if ((rc = rte_eal_mp_remote_launch(entryPoint, static_cast<void*>(&worker), CALL_MAIN))!=0) {
      REINVENT_UTIL_LOG_FATAL_VARGS("Cannot launch DPDK cores rc=%d\n", rc);
    } else {
      REINVENT_UTIL_LOG_INFO_VARGS("waiting for DPDK worker threads to stop\n");
      rte_eal_mp_wait_lcore();
    }
  }

  REINVENT_UTIL_LOG_INFO_VARGS("DPDK worker threads done\n");

  //
  // Cleanup and exit from main thread
  //
  onExit(config);

  return 0;
}
