#include <stdio.h>
#include <sys/types.h>
#include <assert.h>

#include <random>
#include <vector>
#include <unordered_map>

struct Packet {
  enum Type {
      REQUEST              = 1,
      CREDIT               = 2,
      REQUEST_FOR_RESPONSE = 4,
      RESPONSE             = 8
  };

  u_int64_t d_clientId;           // aka RIFL's leaseId
  u_int32_t d_rpcSeqId;           // sequence number of RPC per clientId
  u_int8_t  d_pktSeqId;           // sequence number of packet per RPC per clientId
  u_int8_t  d_typeId;             // packet type

  Packet()
  : d_clientId(0)
  , d_rpcSeqId(0)
  , d_pktSeqId(0)
  , d_typeId(0)
  {
  }

  Packet(u_int64_t clientId, u_int32_t rpcSeqId, u_int8_t pktSeqId, u_int8_t typeId)
  : d_clientId(clientId)
  , d_rpcSeqId(rpcSeqId)
  , d_pktSeqId(pktSeqId)
  , d_typeId(typeId)
  {
  }

  void print(u_int32_t index) const {
    printf("clientId: %04llu, rpcSeqId: %04u, pktSeqId: %u, typeId: %u, rqxIndex: %u\n",
      d_clientId, d_rpcSeqId, d_pktSeqId, d_typeId, index);
  }

  __int128 id() const {
    return __int128(d_clientId)<<64 | d_rpcSeqId;
  }
};

// Data will model a RXQ holding up to 'CAPACITY' packets for different RPCs
std::vector<Packet> data;
const u_int32_t CAPACITY=4096;

// Test set of 5 response packets types a client might expect per RPC
const std::vector<u_int8_t> packetTemplate4= {  Packet::Type::CREDIT,
                                                Packet::Type::CREDIT,
                                                Packet::Type::CREDIT|Packet::Type::RESPONSE,
                                                Packet::Type::CREDIT|Packet::Type::RESPONSE };

/*
    Randomly populate specified 'data' with RPC packets in order:

    * There's RPC_COUNT = CAPACITY/packetTemplate4.size() RPCs
    * Each RPC needs all packetTemplate4 packets somewhere in 'data'
    * For each RPC 'i' let k0 be the index in 'data' holding i's packetTemplate4[0]
                           k1 be the index in 'data' holding i's packetTemplate4[1]
                           . . .
                           k3 be the index in 'data' holding i's packetTemplate4[4]
    * We require 0 <= k0 < k1 < k2 < k3 < data.size()
    * We also require for any two distinct RPCs i,j all packets for i come before j in 'data'
*/
int fillRpc4(std::vector<Packet>& data) {

  // The number of distinct RPCs populated in 'data'
  const u_int32_t RPC_COUNT = CAPACITY/packetTemplate4.size();  
  assert((RPC_COUNT*packetTemplate4.size()) == CAPACITY);
  printf("RPC_COUNT = %u\n", RPC_COUNT);

  // For each distinct rpc 'i' packetsSent[i] holds the highest packet
  // enqueued for it e.g. packetsSent[i]==0 means packetTemplate4[0] was queued
  // A value of 1 means packetTemplate4[0], packetTemplate4[1] was queued
  // in that order in 'data'. i = (clientId, rpcSeqId) stored as 128-bit int
  std::unordered_map<__int128, u_int8_t> packetsSent;

  // These are the clientIds we'll use to populate with
  const std::vector<u_int64_t> clientIds = {0, 1, 2, 3};
  // These are the per clientId rpcSeqIds starting at 0
  std::vector<u_int32_t> rpcSeqIds = {0, 0, 0, 0};

  // The number of packets to be enqueued: RPC_COUNT*packetTemplate4.size()
  const u_int32_t max = RPC_COUNT*packetTemplate4.size();
  printf("packetCount = %u\n", max);

  // Size 'data' to the number of output packets
  data.clear();
  data.resize(max);
  assert(data.size()==max);

  // In order to fill 'data' so that all RPCs have all their packets
  // we only allow at most CAPACITY/packetTemplate4.size()/clientIds.size()
  // per client. otherwise the tail end of 'data' will contain incomplete
  // packet sets for some rpcId
  const u_int32_t maxRpcSeqId = CAPACITY/packetTemplate4.size()/clientIds.size();

  // Setup random number generator to choose a random client
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, clientIds.size()-1);

  // The index in 'data' we're populating/initialzing
  u_int32_t k=0;

  while (k<max) {
    // choose a random client
    auto i = distrib(gen);
    assert(i>=0 && i<clientIds.size());

/*
    if (rpcSeqIds[i]>=maxRpcSeqId) {
      // Choose another random client -- hit max number for it
      continue;
    }
*/

    // Make the unique identifier for the RPC
    const __int128 rpcId = __int128(clientIds[i])<<64 | rpcSeqIds[i];

    // Have we seen this RPC before?
    u_int8_t packetSeqId = 0;
    const std::unordered_map<__int128, u_int8_t>::iterator iter = packetsSent.find(rpcId);

    if (iter!=packetsSent.end()) {
      // Then get the last packet sent
      packetSeqId = packetsSent[rpcId];
      // And if we've sent everything skip it. randomly choose something else
      if (packetSeqId == packetTemplate4.size()-1) {
        continue;
      } else {
        // Not done: enqueue next packet
        ++packetSeqId;
      }
    }

    // Record fact that we're enqueuing 'packetSeqId' for rpcId
    assert(packetSeqId>=0 && packetSeqId<packetTemplate4.size());
    packetsSent[rpcId] = packetSeqId;

    // Enqueue packet
    data[k].d_clientId = clientIds[i];
    data[k].d_rpcSeqId = rpcSeqIds[i];
    data[k].d_pktSeqId = packetSeqId;
    data[k].d_typeId   = packetTemplate4[packetSeqId];

    // Are we done for this RPC?
    if (packetSeqId == packetTemplate4.size()-1) {
      rpcSeqIds[i] = rpcSeqIds[i]+1;
    }

    k++;
  }

  return 0;
}

/*
    Simulate RXQ processing loop where 'data' holds packets to be processed.
    The challenge here is that the packets in 'data' are for N distinct RPCs.
    While, in this test, there are no missing or dropped packets, packets
    data[i] and data[i+1] are in general different RPCs. To work towards
    dealing with packet drop/loss, the RXQ loop must quickly make and update
    state per distinct RPC. It needs to know if the response is complete;
    it needs to know if packets came out of order.
*/
int readRpc4(const std::vector<Packet>& data) {
  for (u_int32_t i=0; i<data.size(); ++i) {
      const Packet& pkt = data[i];
      pkt.print(i);
  }
  
  return 0;
}

int main() {
  int rc = fillRpc4(data);
  assert(rc==0);

  rc = readRpc4(data);
  assert(rc==0);

  return 0;
}
