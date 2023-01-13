#include <stdio.h>
#include <sys/types.h>
#include <assert.h>

#include <random>
#include <vector>

struct Packet {
  enum Type {
      REQUEST              = 1,
      CREDIT               = 2,
      REQUEST_FOR_RESPONSE = 4,
      RESPONSE             = 8
  };
      
  u_int32_t d_clientId;
  u_int8_t  d_clientSeqId;
  u_int8_t  d_typeId;

  Packet()
  : d_clientId(0)
  , d_clientSeqId(0)
  , d_typeId(0)
  {
  }

  Packet(u_int64_t clientId, u_int8_t clientSeqId, u_int8_t typeId)
  : d_clientId(clientId)
  , d_clientSeqId(clientSeqId)
  , d_typeId(typeId)
  {
  }

  void print(u_int32_t index) {
    printf("index: %04u: clientId: %04u, clientSeqId: %u, typeId: %u\n",
      index, d_clientId, d_clientSeqId, d_typeId);
  }
};

std::vector<Packet> data;
const u_int32_t CAPACITY=4096;

// Test set of 5 response packets a client might expect per RPC
std::vector<Packet> packetTemplate5= { {0, 0, Packet::Type::CREDIT},
                                       {0, 1, Packet::Type::CREDIT},
                                       {0, 2, Packet::Type::CREDIT|Packet::Type::RESPONSE},
                                       {0, 3, Packet::Type::CREDIT|Packet::Type::RESPONSE},
                                       {0, 4, Packet::Type::CREDIT|Packet::Type::RESPONSE} };

/*
    Randomly populate specified 'data' with RPC packets in correct order:

    * There's RPC_COUNT = CAPACITY/packetTemplate5.size() RPCs
    * Each RPC needs all packetTemplate5 packets somewhere in 'data'
    * For each RPC 'i' let k0 be the index in 'data' holding i's packetTemplate5[0]
                           k1 be the index in 'data' holding i's packetTemplate5[1]
                           . . .
                           k4 be the index in 'data' holding i's packetTemplate5[4]
    * We require k0>=0 and k0 < k1 < k2 < k3 < k4 and k4<data.size()
*/
int fillRpc5(std::vector<Packet>& data) {

  // The number of distinct RPCs populated in 'data'
  const u_int32_t RPC_COUNT = CAPACITY/packetTemplate5.size();  
  printf("RPC_COUNT = %u\n", RPC_COUNT);

  // For each distinct rpc 'i' rpcSent[i] counts the number of packets
  // enqueued for it e.g. rpcSent[i]==0 means nothing queued, and
  // rpcSent[i]==packetTemplate5.size() means all packets queued. the
  // value 3 means packetTemplate5[k] packets with k=0,1,2 queued in
  // that order
  std::vector<u_int32_t> rpcSent(RPC_COUNT, 0);
  assert(rpcSent.size()==RPC_COUNT);

  // The number of packets to be enqueued: RPC_COUNT*packetTemplate5.size()
  const u_int32_t max = RPC_COUNT*packetTemplate5.size();
  printf("packetCount = %u\n", max);

  // Size the output parameter to the number of output packets
  data.clear();
  data.resize(max);
  assert(data.size()==max);

  // Setup random number generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, RPC_COUNT-1);

  // The index into 'data' we're initialzing
  u_int32_t k=0;

  while (k<max) {
    // choose a random RPC
    auto i = distrib(gen);
    assert(i>=0 && i<RPC_COUNT);

    // Anything left to send?
    const u_int32_t packetIndex = rpcSent[i];
    if (packetIndex!=packetTemplate5.size()) {
        assert(packetIndex>=0 && packetIndex<packetTemplate5.size());
        data[k].d_clientId = i;
        data[k].d_clientSeqId = packetTemplate5[packetIndex].d_clientSeqId;
        data[k].d_typeId      = packetTemplate5[packetIndex].d_typeId;
        rpcSent[i] = packetIndex+1;
        k++;
    }
  }

  // Optional: verify data contents O(n^3)
  u_int32_t foundIndex = 0;
  u_int32_t lastFoundIndex = 0;
  for (u_int32_t i=0; i<RPC_COUNT; ++i) {
    for (u_int32_t j=0; j<packetTemplate5.size(); j++) {
      // look for clientId 'i' entry 'j' from packetTemplate5
      bool found = false;
      for (unsigned k=0; k<data.size() && !found; k++) {
        found =
            ((data[k].d_clientId    == i)                               &&
             (data[k].d_clientSeqId == packetTemplate5[j].d_clientSeqId) &&
             (data[k].d_typeId      == packetTemplate5[j].d_typeId));
        foundIndex = k;
      }

      // If not found error out
      if (!found) {
        printf("ERROR: rpc %u packet %u not found\n", i, j); 
        assert(found);
        return -1;
      }

      // Make sure the found index is valid in 'data'
      assert(foundIndex>=0 && foundIndex<data.size());

      // Make sure its in increasing order
      if (j==0) {
        // This is the first packet of a new RPC
        lastFoundIndex = foundIndex;
        // data[foundIndex].print(foundIndex);
      } else if (foundIndex<=lastFoundIndex) {
        printf("ERROR: rpc %u packet %u found but in wrong order\n", i, j); 
        assert(foundIndex>lastFoundIndex);
        return -1;
      } else {
        lastFoundIndex = foundIndex;
        // data[foundIndex].print(foundIndex);
      }
    }
  }

  return 0;
}

/*
    Simulate RXQ processing loop where 'data' holds packets to be processed.
    The challenge here is that the packets in 'data' are for N distinct RPCs.
    While, in this test, there are no missing or dropped packets, packets
    data[i] and data[i+1] are in general different RPCs. To work towards
    dealing with packet drop/loss, the RXQ loop must quickly make and update
    state per distinct RPC such as,
*/
int readRpc5(const std::vector<Packet>& data) {
  return 0;
}

int main() {
  int rc = fillRpc5(data);
  assert(rc==0);

  rc = readRpc5(data);
  assert(rc==0);

  return 0;
}
