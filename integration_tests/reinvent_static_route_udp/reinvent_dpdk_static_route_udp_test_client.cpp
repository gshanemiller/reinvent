#include <dpdk/reinvent_dpdk_init.h>
#include <dpdk/reinvent_dpdk_worker.h>
#include <dpdk/reinvent_dpdk_util.h>
#include <dpdk/reinvent_dpdk_stream.h>

#include <reinvent_dpdk_static_route_udp_test_common.h>

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

const int CLIENT_RX_BURST_CAPACITY = 15;
const unsigned CLIENT_REPORT_COUNT = 100000;

int clientTxMainLoop(int id, int txqIndex, Reinvent::Dpdk::Worker *worker, unsigned packetCount,
  const Reinvent::Dpdk::IPV4Route& defaultRoute) {
  assert(config);

  printf("lcoreId %02d txqIndex %02d transmitting packets\n", id, txqIndex);
  return 0;

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
  const int packetSize = sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr)+sizeof(Message);

  //
  // Size relative to IP4V header
  //
  const uint16_t ip4Size = rte_cpu_to_be_16(static_cast<uint16_t>(sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr)+sizeof(Message)));

  //
  // Size relative to UDP header
  //
  const uint16_t udpSize = rte_cpu_to_be_16(static_cast<uint16_t>(sizeof(rte_udp_hdr)+sizeof(Message)));

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
    // | rte_ether_hdr | rte_ipv4_hdr | rte_udp_hdr | Message |   
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
    // Prepare Message Payload
    //
    Message *payload = rte_pktmbuf_mtod_offset(mbuf, Message*,
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
  }

  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  uint64_t elapsedNs = timeDifference(start, now);

  double rateNsPerPacket = static_cast<double>(elapsedNs)/static_cast<double>(count);
  double pps = static_cast<double>(1000000000)/rateNsPerPacket;
  double bytesPerSecond = static_cast<double>(count)*static_cast<double>(packetSize)/
    (static_cast<double>(elapsedNs)/static_cast<double>(1000000000));
  double mbPerSecond = bytesPerSecond/static_cast<double>(1024)/static_cast<double>(1024);
  double payloadBytesPerSecond = static_cast<double>(count)*static_cast<double>(sizeof(Message))/
    (static_cast<double>(elapsedNs)/static_cast<double>(1000000000));
  double payloadMbPerSecond = payloadBytesPerSecond/static_cast<double>(1024)/static_cast<double>(1024);

  printf("lcoreId: %02d, txqIndex: %02d: elsapsedNs: %lu, packetsQueued: %u, packetSizeBytes: %d, payloadSizeBytes: %lu, pps: %lf, nsPerPkt: %lf, bytesPerSec: %lf, mbPerSec: %lf, mbPerSecPayloadOnly: %lf stalledTx %u\n", 
    id, txqIndex, elapsedNs, count, packetSize, sizeof(Message), pps, rateNsPerPacket, bytesPerSecond, mbPerSecond, payloadMbPerSecond, stalledTx);

  return 0;
}

int clientRxMainLoop(int id, int rxqIndex, Reinvent::Dpdk::Worker *worker) {
  const uint16_t deviceId = static_cast<uint16_t>(worker->config().deviceId());

  //
  // Finally the main point: receiving packets!
  //
  printf("lcoreId %02d rxqIndex %02d listening for packets\n", id, rxqIndex);
  
  uint32_t count(0);
  uint32_t stalledRx(0);
  std::vector<rte_mbuf*> mbuf(CLIENT_RX_BURST_CAPACITY);

  struct timespec start;
  clock_gettime(CLOCK_REALTIME, &start);

  while(!terminate) {
    //
    // Receive up to CLIENT_RX_BURST_CAPACITY packets
    //
    uint16_t rxCount = rte_eth_rx_burst(deviceId, rxqIndex, mbuf.data(), CLIENT_RX_BURST_CAPACITY);
    if (likely(rxCount==0)) {
      ++stalledRx;
      continue;
    }

    for (unsigned i=0; i<rxCount; ++i) {
      //                                                                                                                
      // Find payload and print it                                                                                      
      //                                                                                                                
      Message *payload = rte_pktmbuf_mtod_offset(mbuf[i], Message*,
        sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr));
      REINVENT_UTIL_LOG_INFO_VARGS("id %d rxqIndex %d packet: sender: lcoreId: %d, txqId: %d, sequence: %d\n",
        id, rxqIndex, payload->lcoreId, payload->txqId, payload->sequence);
      rte_pktmbuf_free(mbuf[i]);
    }

    if ((count += rxCount)>=CLIENT_REPORT_COUNT) {
      struct timespec now;
      clock_gettime(CLOCK_REALTIME, &now);
      uint64_t elapsedNs = timeDifference(start, now);

      const int packetSize = sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr)+sizeof(Message);
      double rateNsPerPacket = static_cast<double>(elapsedNs)/static_cast<double>(count);
      double pps = static_cast<double>(1000000000)/rateNsPerPacket;
      double bytesPerSecond = static_cast<double>(count)*static_cast<double>(packetSize)/
        (static_cast<double>(elapsedNs)/static_cast<double>(1000000000));
      double mbPerSecond = bytesPerSecond/static_cast<double>(1024)/static_cast<double>(1024);
      double payloadBytesPerSecond = static_cast<double>(count)*static_cast<double>(sizeof(Message))/
        (static_cast<double>(elapsedNs)/static_cast<double>(1000000000));
      double payloadMbPerSecond = payloadBytesPerSecond/static_cast<double>(1024)/static_cast<double>(1024);

      printf("lcoreId: %02d, rxqIndex: %02d: elsapsedNs: %lu, packetsDequeued: %u, packetSizeBytes: %d, payloadSizeBytes: %lu, pps: %lf, nsPerPkt: %lf, bytesPerSec: %lf, mbPerSec: %lf, mbPerSecPayloadOnly: %lf, stalledRx: %u\n", 
        id, rxqIndex, elapsedNs, count, packetSize, sizeof(Message), pps, rateNsPerPacket, bytesPerSecond, mbPerSecond, payloadMbPerSecond, stalledRx);
    
      count = stalledRx = 0;

      clock_gettime(CLOCK_REALTIME, &start);
    }
  }

  return 0;
}

int clientTxEntryPoint(int id, int txqIndex, Reinvent::Dpdk::Worker *worker) {
  assert(config);

  //
  // Get number of packets to send
  //
  int rc, tmp;
  std::string variable;
  Reinvent::Dpdk::Names::make(worker->envPrefix(), &variable, "%s", "TXQ_PACKET_COUNT");
  if ((rc = worker->env().valueAsInt(variable, &tmp, true, 1, 100000000))!=0) {
    return rc;
  }
  unsigned packetCount = static_cast<unsigned>(tmp);

  //
  // Get a route to TX on
  //
  if (worker->config().defaultRoute().size()<=(unsigned)txqIndex) {
    REINVENT_UTIL_LOG_ERROR_VARGS("no default route for client txq %d\n", txqIndex);
    return -1;
  }

  //
  // Run the TXQ
  //
  return clientTxMainLoop(id, txqIndex, worker, packetCount, worker->config().defaultRoute()[txqIndex]);
}

int clientRxEntryPoint(int id, int rxqIndex, Reinvent::Dpdk::Worker *worker) {
  assert(config);

  //
  // Proceed directly to RXQ
  //
  return clientRxMainLoop(id, rxqIndex, worker);
}

int clientEntryPoint(void *arg) {
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
    rc = clientTxEntryPoint(id, txqIndex, worker);
    REINVENT_UTIL_LOG_INFO_VARGS("clientEntryPoint TXQ rc=%d\n", rc);
  } else if (rx) {
    rc = clientRxEntryPoint(id, rxqIndex, worker);
    REINVENT_UTIL_LOG_INFO_VARGS("clientEntryPoint RXQ rc=%d\n", rc);
  } else {
    REINVENT_UTIL_LOG_ERROR_VARGS("cannot classify DPDK lcore %d as RX or TX\n", id);
  }

  return rc;
}

int clientEntryPoint(std::string& envPrefix, Reinvent::Util::Environment& env, Reinvent::Dpdk::Config& config) {
  //
  // Setup and run Workers for client. Recall the client sends out messages on
  // its TXQs while its RXQs receive their response(s). So this code runs one
  // thread for each lcore handling its respective TXQ/RXQ
  //
  int rc = -1;
  Reinvent::Dpdk::Worker worker(envPrefix, env, config);
  REINVENT_UTIL_LOG_INFO_VARGS("launching client DPDK worker threads\n");
  if ((rc = rte_eal_mp_remote_launch(clientEntryPoint, static_cast<void*>(&worker), CALL_MAIN))!=0) {
    REINVENT_UTIL_LOG_FATAL_VARGS("Cannot launch DPDK cores rc=%d\n", rc);
  } else {
    REINVENT_UTIL_LOG_INFO_VARGS("waiting for client DPDK worker threads to stop\n");
    rte_eal_mp_wait_lcore();
  }

  return rc;
}
