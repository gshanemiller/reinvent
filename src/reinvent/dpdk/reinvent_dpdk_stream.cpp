#include <dpdk/reinvent_dpdk_stream.h>
#include <dpdk/reinvent_dpdk_util.h>

#include <stdio.h>

namespace Reinvent {
namespace Dpdk {

std::ostream& operator<<(std::ostream& stream, const rte_eth_link& object) {
  char tmpbuf[256];
  stream << "{";
  snprintf(tmpbuf, sizeof(tmpbuf), "\"link_speed\": \"0x%04x\", ", object.link_speed); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"link_duplex\": \"0x%02x\", ", object.link_duplex); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"link_autoneg\": \"0x%02x\", ", object.link_autoneg); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"link_status\": \"0x%02x\"", object.link_status); 
  stream << tmpbuf;
  stream << "}";

  return stream;
}

std::ostream& operator<<(std::ostream& stream, const rte_eth_dev_info& object) {
  char tmpbuf[256];
  stream << "{";
  snprintf(tmpbuf, sizeof(tmpbuf), "\"device\": \"(elided)\", "); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"driver_name\": \"%s\", ", object.driver_name); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"if_index\": \"0x%04x\", ", object.if_index); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"min_mtu\": \"0x%02x\", ", object.min_mtu); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"max_mtu\": \"0x%02x\", ", object.max_mtu); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"dev_flags\": \"0x%04x\", ", *(object.dev_flags)); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"min_rx_bufsize\": \"0x%04x\", ", object.min_rx_bufsize); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"max_rx_pktlen\": \"0x%04x\", ", object.max_rx_pktlen); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"max_lro_pkt_size\": \"0x%04x\", ", object.max_lro_pkt_size); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"max_rx_queues\": \"0x%02x\", ", object.max_rx_queues); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"max_tx_queues\": \"0x%02x\", ", object.max_tx_queues); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"max_mac_addrs\": \"0x%04x\", ", object.max_mac_addrs); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"max_vfs\": \"0x%02x\", ", object.max_vfs); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"max_vmdq_pools\": \"0x%02x\", ", object.max_vmdq_pools); 
  stream << tmpbuf;

  snprintf(tmpbuf, sizeof(tmpbuf), "\"rx_seg_capa\": \"(elided)\", "); 
  stream << tmpbuf;

  snprintf(tmpbuf, sizeof(tmpbuf), "\"rx_offload_capa\": \"0x%08lx\", ", object.rx_offload_capa); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"tx_offload_capa\": \"0x%08lx\", ", object.tx_offload_capa); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"rx_queue_offload_capa\": \"0x%08lx\", ", object.rx_queue_offload_capa); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"tx_queue_offload_capa\": \"0x%08lx\", ", object.tx_queue_offload_capa); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"reta_size\": \"0x%02x\", ", object.reta_size); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"hash_key_size\": \"0x%02x\", ", object.hash_key_size); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"flow_type_rss_offloads\": \"0x%08lx\", ", object.flow_type_rss_offloads); 
  stream << tmpbuf;

  stream << "\"default_rxconf\": " << object.default_rxconf << ", ";
  stream << "\"default_txconf\": " << object.default_txconf << ", ";

  snprintf(tmpbuf, sizeof(tmpbuf), "\"vmdq_queue_base\": \"0x%02x\", ", object.vmdq_queue_base); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"vmdq_queue_num\": \"0x%02x\", ", object.vmdq_queue_num); 
  stream << tmpbuf;

  snprintf(tmpbuf, sizeof(tmpbuf), "\"rx_desc_lim\": \"(elided)\", "); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"tx_desc_lim\": \"(elided)\", "); 
  stream << tmpbuf;

  snprintf(tmpbuf, sizeof(tmpbuf), "\"speed_capa\": \"0x%04x\", ", object.speed_capa); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"nb_rx_queues\": \"0x%04x\", ", object.nb_rx_queues); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"nb_tx_queues\": \"0x%04x\", ", object.nb_tx_queues); 
  stream << tmpbuf;

  snprintf(tmpbuf, sizeof(tmpbuf), "\"default_rxportconf\": \"(elided)\", "); 
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"default_txportconf\": \"(elided)\", "); 
  stream << tmpbuf;

  snprintf(tmpbuf, sizeof(tmpbuf), "\"dev_capa\": \"0x%08lx\", ", object.dev_capa); 
  stream << tmpbuf;

  snprintf(tmpbuf, sizeof(tmpbuf), "\"switch_info\": \"(elided)\""); 
  stream << tmpbuf;

  stream << "}";

  return stream;
}

std::ostream& operator<<(std::ostream& stream, const rte_ether_addr& object) {
  char tmpbuf[256];
  Dpdk::MacAddressUnion tmp;
  tmp.d_addr = object;

  stream << "{";
  snprintf(tmpbuf, sizeof(tmpbuf), "\"mac\": \"0x%08lx\"", tmp.d_uint);
  stream << "}";

  return stream;
}

std::ostream& operator<<(std::ostream& stream, const rte_eth_txconf& object) {
  char tmpbuf[256];
  stream << "{";

  snprintf(tmpbuf, sizeof(tmpbuf), "\"tx_thresh\": { \"pthresh\": %u, \"hthresh\": %u, \"wthresh\": %u }, ",
    object.tx_thresh.pthresh, object.tx_thresh.hthresh, object.tx_thresh.wthresh);
  stream << tmpbuf;

  snprintf(tmpbuf, sizeof(tmpbuf), "\"tx_rs_thresh\": %u, ", object.tx_rs_thresh);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"tx_free_thresh\": %u, ", object.tx_free_thresh);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"tx_deferred_start\": %u, ", object.tx_deferred_start);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"tx_offloads\": %08lx, ", object.offloads);
  stream << tmpbuf;

  stream << "}";
  return stream;
}

std::ostream& operator<<(std::ostream& stream, const rte_eth_rxconf& object) {
  char tmpbuf[256];
  stream << "{";

  snprintf(tmpbuf, sizeof(tmpbuf), "\"rx_thresh\": { \"pthresh\": %u, \"hthresh\": %u, \"wthresh\": %u }, ",
    object.rx_thresh.pthresh, object.rx_thresh.hthresh, object.rx_thresh.wthresh);
  stream << tmpbuf;

  snprintf(tmpbuf, sizeof(tmpbuf), "\"rx_free_thresh\": %u, ", object.rx_free_thresh);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"rx_drop_en\": %u, ", object.rx_drop_en);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"rx_deferred_start\": %u, ", object.rx_deferred_start);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"rx_nseg\": %u, ", object.rx_nseg);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"share_group\": %u, ", object.share_group);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"share_qid\": %u, ", object.share_qid);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"offloads\": %08lx, ", object.offloads);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"rx_seg\": \"(elided)\"");
  stream << tmpbuf;

  stream << "}";
  return stream;
}

std::ostream& operator<<(std::ostream& stream, const rte_eth_rss_conf& object) {
  char tmpbuf[1024];
  stream << "{";
  snprintf(tmpbuf, sizeof(tmpbuf), "\"rss_key_len\": 0x%02x, ", object.rss_key_len);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"rss_hf\": 0x%08lx, ", object.rss_hf);
  stream << tmpbuf;
  snprintf(tmpbuf, sizeof(tmpbuf), "\"rss_key\": 0x");
  stream << tmpbuf;

  tmpbuf[0]=0;
  for (unsigned i=0; i<object.rss_key_len; ++i) {
    snprintf(tmpbuf+strlen(tmpbuf), sizeof(tmpbuf), "%02x", object.rss_key[i]);
  }
  stream << tmpbuf;

  stream << "}";
  return stream;
}

} // namespace Dpdk
} // namespace Reinvent
