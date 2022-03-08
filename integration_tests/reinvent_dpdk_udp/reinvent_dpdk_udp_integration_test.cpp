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
#pragma GCC diagnostic pop 

#include <unistd.h>
#include <arpa/inet.h>

//
// Default burst capacities
//
int RX_BURST_CAPACITY = 8;
int TX_BURST_CAPACITY = 8;

//
// Clients send this. Servers get it
//
struct TxMessage {
  int lcoreId;            // lcoreId which sent this message
  int txqId;              // txqId which sent this message
  int sequenceNumber;     // txqId's sequence number
};

int onExit(const Reinvent::Dpdk::AWSEnaConfig& config) {
  int rc;

  if ((rc = Reinvent::Dpdk::UnInitAWS::device(config))!=0) {
    REINVENT_UTIL_LOG_WARN_VARGS("Cannot uninitialize AWS ENA device rc=%d\n", rc);
  }

  if ((rc = Reinvent::Dpdk::UnInitAWS::ena())!=0) {
    REINVENT_UTIL_LOG_WARN_VARGS("Cannot uninitialize AWS ENA system rc=%d\n", rc);
  }

  return rc;
}

void usageAndExit() {
  printf("reinvent_dpdk_udp_integration_test.tsk -m <client|server> -p <env-var-prefix>\n");
  printf("   -m <client|server>       optional (default server). mode to run task in.\n");
  printf("   -p <string>              required: non-empty ENV variable prefx name with config\n");
  printf("   -t <integer>             optional: positive per TXQ burst capacity default %d\n", TX_BURST_CAPACITY);
  printf("   -r <integer>             optional: positive per RXQ burst capacity default %d\n", RX_BURST_CAPACITY);
  exit(2);
}

void parseCommandLine(int argc, char **argv, bool *isServer, std::string *prefix) {
  int c, n;

  *isServer = true;
  prefix->clear();

  while ((c = getopt (argc, argv, "m:p:t:r:")) != -1) {
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
        TX_BURST_CAPACITY = n;
        break;
      case 'r':
        n = atoi(optarg);
        if (n<=0) {
          usageAndExit();
        }
        RX_BURST_CAPACITY = n;
        break;
      default:
        usageAndExit();
    }
  }

  if (prefix->empty()) {
    usageAndExit();
 }
}

int clientMainLoop(int id, int txqIndex, Reinvent::Dpdk::AWSEnaWorker *config, int packetCount,
  std::vector<rte_mbuf*>& mbuf) {

  assert(config);
  assert(mbuf.size());

  const int burstCapacity = static_cast<int>(mbuf.size());

  const uint16_t ethFlow = rte_cpu_to_be_16(static_cast<uint16_t>(config->awsEnaConfig().txq()[txqIndex].defaultFlow()));

  const uint16_t deviceId = static_cast<uint16_t>(config->awsEnaConfig().deviceId());

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
  uint32_t srcIp;
  config->awsEnaConfig().txq()[txqIndex].defaultRoute().convertSrcIp(&srcIp);

  //
  // Convert dst IP address from string to binary
  //
  uint32_t dstIp;
  config->awsEnaConfig().txq()[txqIndex].defaultRoute().convertDstIp(&dstIp);

  //
  // UDP pseudo ports
  //
  const uint16_t srcPort = rte_cpu_to_be_16(static_cast<uint16_t>(config->awsEnaConfig().txq()[txqIndex].defaultRoute().srcPort()));
  const uint16_t dstPort = rte_cpu_to_be_16(static_cast<uint16_t>(config->awsEnaConfig().txq()[txqIndex].defaultRoute().dstPort()));

  //
  // TX offload flags to carry down to packet
  //
  // const uint64_t txOffloadFlags = static_cast<uint64_t>(config->awsEnaConfig().txOffloadMask());

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

  // Within a given mbuf:
  //
  //  +---> rte_pktmbuf_mtod_offset(mbuf, rte_ether_hdr*, 0) 
  // /
  // +---------------+--------------+-------------+-----------+
  // | rte_ether_hdr | rte_ipv4_hdr | rte_udp_hdr | TxMessage |   
  // +---------------+--------------+-------------+-----------+

  //
  // Finally the main point: transmitting packets!
  //
  int sequence=0;

  while (sequence<packetCount) {
    for(int i=0; i<burstCapacity; ++i) {
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
      payload->sequenceNumber = sequence++;

      //
      // Finalize mbuf in DPDK
      //
      mbuf[i]->nb_segs = 1;
		  mbuf[i]->pkt_len = packetSize;
		  mbuf[i]->data_len = packetSize;
		  mbuf[i]->ol_flags = 0;
    }

    //
    // TX packets e.g. write them onto the wire
    //
	  uint16_t txCount = rte_eth_tx_burst(deviceId, txqIndex, mbuf.data(), burstCapacity);
    REINVENT_UTIL_LOG_INFO("burst sent " << txCount << " packets" << std::endl);

    //
    // Free mbufs
    //
    rte_pktmbuf_free_bulk(mbuf.data(), mbuf.size());
  }

  return 0;
}

int serverMainLoop(int id, int rxqIndex, Reinvent::Dpdk::AWSEnaWorker *config, int timeoutSecs,
  std::vector<rte_mbuf*>& mbuf) {

  const int burstCapacity = static_cast<int>(mbuf.size());
  const uint16_t deviceId = static_cast<uint16_t>(config->awsEnaConfig().deviceId());

  //
  // Finally the main point: receiving packets!
  //
  REINVENT_UTIL_LOG_INFO_VARGS("id %d rxqIndex %d listening for packets\n", id, rxqIndex);
  
  while(timeoutSecs) {
	  uint16_t rxCount = rte_eth_rx_burst(deviceId, rxqIndex, mbuf.data(), burstCapacity);
    if (likely(rxCount==0)) {
      continue;
    }
    for (unsigned i=0; i<rxCount; ++i) {
      //                                                                                                                
      // Find payload and print it                                                                                      
      //                                                                                                                
      TxMessage *payload = rte_pktmbuf_mtod_offset(mbuf[i], TxMessage*,
        sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr));
      REINVENT_UTIL_LOG_INFO_VARGS("id %d rxqIndex %d packet: sender: lcoreId: %d, txqId: %d, sequenceNumber: %d\n",
        id, rxqIndex, payload->lcoreId, payload->txqId, payload->sequenceNumber);

      //
      // Free mbuf
      //
      rte_mbuf_raw_free(mbuf[i]); 
    }
  }

  return 0;
}

int clientEntryPoint(int id, int txqIndex, Reinvent::Dpdk::AWSEnaWorker *config) {
  assert(config);

  //
  // Get number of packets to send from client to server
  //
  int rc, packetCount;
  std::string variable;
  Reinvent::Dpdk::Names::make(config->envPrefix(), &variable, "%s", "TXQ_PACKET_COUNT");
  if ((rc = config->env().valueAsInt(variable, &packetCount, true, 1))!=0) {
    return rc;
  }
  
  //
  // Find the TXQ's mempool
  //
  rte_mempool *pool = config->awsEnaConfig().txq()[txqIndex].mempool();
  assert(pool);

  //
  // Allocte mbufs
  //
  std::vector<rte_mbuf*> mbuf(TX_BURST_CAPACITY);
  for (int i=0; i<TX_BURST_CAPACITY; ++i) {
    mbuf[i] = rte_pktmbuf_alloc(pool);
    if (0==mbuf[i]) {
      REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_API, (mbuf[i]),
        "Client burst mbuf allocation failed", rte_strerror(rc), rc);
    }
  }
 
  //
  // Finally enter the main processing loop passing state collected here
  //
  return clientMainLoop(id, txqIndex, config, packetCount, mbuf);
}

int serverEntryPoint(int id, int rxqIndex, Reinvent::Dpdk::AWSEnaWorker *config) {
  assert(config);

  //
  // Find the RXQ's mempool
  //
  rte_mempool *pool = config->awsEnaConfig().rxq()[rxqIndex].mempool();
  assert(pool);

  //
  // Get number of seconds to wait to get all client packets
  //
  int rc, timeoutSecs;
  std::string variable;
  Reinvent::Dpdk::Names::make(config->envPrefix(), &variable, "%s", "RX_SESSION_TIME_SECS");
  if ((rc = config->env().valueAsInt(variable, &timeoutSecs, true, 1))!=0) {
    return rc;
  }
  
  //
  // Allocte mbufs
  //
  std::vector<rte_mbuf*> mbuf(RX_BURST_CAPACITY);
  for (int i=0; i<RX_BURST_CAPACITY; ++i) {
    mbuf[i] = rte_pktmbuf_alloc(pool);
    if (0==mbuf[i]) {
      REINVENT_UTIL_ERRNO_RETURN(Reinvent::Util::Errno::REINVENT_UTIL_ERRNO_API, (mbuf[i]),
        "Server burst mbuf allocation failed", rte_strerror(rc), rc);
    }
  }
 
  //
  // Finally enter the main processing loop passing state collected here
  //
  return serverMainLoop(id, rxqIndex, config, timeoutSecs, mbuf);
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

  if ((rc = config->id(&id, &rx, &tx, &rxqIndex, &txqIndex))!=0) {
    return rc;
  } 

  if (tx) {
    return clientEntryPoint(id, txqIndex, config);
  } else if (rx) {
    return serverEntryPoint(id, rxqIndex, config);
  } else {
    REINVENT_UTIL_LOG_ERROR_VARGS("cannot classify DPDK lcore %d as RX or TX\n", id);
  }

  // unexpected  
  return -1;
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
  // Initialize the AWS ENA Nic
  //
  int rc = Reinvent::Dpdk::InitAWS::enaUdp(device, prefix, &env, &config);
  REINVENT_UTIL_LOG_INFO(config << std::endl);
  if (rc!=0) {
    REINVENT_UTIL_LOG_FATAL_VARGS("Cannot initialize AWS ENA device rc=%d\n", rc);                                      
    return onExit(config);
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
  return onExit(config);
}
