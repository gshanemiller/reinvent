# Key Value RPCs
Evolving from [a trivial UDP example](https://github.com/gshanemiller/reinvent/blob/main/doc/equinix_mellanox_setup.md) to practical RPC (Remote Procedure Call) for scalable KV (Key Value) processing is difficult. Real world RPC under DPDK must meet and exceed all the excellent features of [eRPC](https://www.usenix.org/system/files/nsdi19-kalia.pdf) adding:

* linearizability
* durable writes 
* exactly-once message processing

Composing each feature into a well engineered whole is complex. This document elicits each technical challenge with periodic pauses to summarize and integrate ultimately resolving the overall goal.

To motivate this work consider the [static routing scenario discussed in the packet design doc.](https://github.com/gshanemiller/reinvent/blob/main/doc/packet_design.md#flow-control)
Here a client `C` wants to obtain the value `V` for a key `K`. To do this, the client first determines the IP address and destination port `S` of the server which handles `K`. `C` send sends a request to `S`. `S` responds to `C` with the value `V` for `K` or nil if not found. This is static routing in the sense `C` chooses `S`. Determining `S` is not discussed here but, for example, `C` could consult `etcd` or some other service which knows which `S` handles which keys `K`.  

# Bare Bones Static Routing
To do KV lookup through static routing [we need flow control](https://github.com/gshanemiller/reinvent/blob/main/doc/packet_design.md#flow-control) on clients and servers. Flow control on the servers insures requests go to the right DPDK RXQ which does request in-take. We need flow control on the clients so the response is sent back to the client `C` that made the original request. Note that the source UDP destination port is unused/ignored in this work. Recall that unlike kernel based I/O, ports are pseudo in DPDK to be used or ignored as needed.

```
 /------------------------------Client Box-----------------------------\
+-----------------------------------------------------------------------+
|  /----------------------client app pid 1234------------------------\  |    Flow control routing
| +---------------------------------+---------------------------------+ |
| | TXQ#0 lcore 0 thread 0 client 0 | RXQ#0 lcore 1 thread 1 client 0 | | <- 192.168.0.2 udp dest port 1
| +---------------------------------+---------------------------------+ |
| | TXQ#1 lcore 2 thread 2 client 1 | RXQ#1 lcore 3 thread 3 client 1 | | <- 192.168.0.2 udp dest port 2
| +---------------------------------+---------------------------------+ |
| |        .                        |               .                 | | <- 192.168.0.2 udp dest port 4
| +---------------------------------+---------------------------------+ |
| |        .                        |               .                 | |     .
| +---------------------------------+---------------------------------+ |
| |        .                        |               .                 | +---------------------------+
| +---------------------------------+---------------------------------+ |  DPDK NIC                 |-------+
+-----------------------------------------------------------------------+---------------------------+       |
  Clients send requests on their own TXQ and look for responses on their                                    |
  own RXQ associated with the TXQ                                                                           |
                                                                                                            |
                                                                                                            |
 /------------------------------Server Box-----------------------------\                                    |
+-----------------------------------------------------------------------+                                   |
|  /----------------------server app pid 5557------------------------\  |    Flow control routing           |
| +---------------------------------+---------------------------------+ |                                   |
| | RXQ#0 lcore 0 thread 0 server 0 | TXQ#0 lcore 1 thread 1 server 0 | | <- 192.168.0.10 udp dest port 1   +--------+
| +---------------------------------+---------------------------------+ |                                   | VLAN   |
| | RXQ#1 lcore 2 thread 2 server 1 | TXQ#1 lcore 3 thread 3 server 1 | | <- 192.168.0.10 udp dest port 2   +--------+
| +---------------------------------+---------------------------------+ |                                   |
| |        .                        |               .                 | | <- 192.168.0.10 udp dest port 4   |
| +---------------------------------+---------------------------------+ |                                   |
| |        .                        |               .                 | |     .                             |
| +---------------------------------+---------------------------------+ |                                   |
| |        .                        |               .                 | +---------------------------+       |
| +---------------------------------+---------------------------------+ |  DPDK NIC                 |-------+
+-----------------------------------------------------------------------+---------------------------+
  Servers receive requests on their own RXQ and send responses on their
  own TXQ associated with the RXQ
```

Suppose `K=test` is assigned to the server#1 in the diagram. Further suppose client#1 wants to do a lookup of `test`. There will be at least two packets sent as follows:

* Client pid 1234 thread 0 for lcore 0 handling TXQ0 sends a packet from srcIP=192.168.0.2 srcUdpPort=xx to dstIp=192.168.0.10 dstUdpPort=2 where 'xx' means the value does not matter. Flow control on the server insures the right RXQ gets the packet
* Server pid 5557 thread 2 for lcore 2 handling RXQ1 receives this packet and does the lookup of `K` given in the incoming packet. Let `V` be its value
* Server pid 5557 thread 2 for lcore 2 (e.g. server#1) enqueues a message containing the incoming packet plus `V` for TXQ1 running on thread 3 for lcore 3
* Server pid 5557 thread 3 for lcore 3 dequeues the message and transmits a packet from srcIp=192.168.0.10 srcUdpPort=xx to dstIp=192.168.0.2 dstUdpPort=1. Note the message contains a copy of the incoming message which can be used to swap the src/dst IP addresses and ports to make static routing work in reverse. Flow control on the client ensures the right RXQ gets the packet
* Client pid 1234 RXQ#0 on thread 1 for lcore 1 receives the response packet

By partitioning the KV space over server boxes and lcores within each box, client requests are share nothing and easily paralizable. It generalizes to any number of boxes, for example, through [*Rendezvous Hashing*](https://medium.com/i0exception/rendezvous-hashing-8c00e2fb58b0).

To implement all these constraints we use Reinvent's flexible configuration system:

* [Program flow control client side](https://github.com/gshanemiller/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L92)
* [Program flow control server side](https://github.com/gshanemiller/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L226)
* [Program client TXQ assignment](https://github.com/gshanemiller/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L58) and [client RXQ assignment](https://github.com/gshanemiller/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L72) so it's staggered by CPU core
* [Program server TXQ assignment](https://github.com/gshanemiller/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L189) and [server RXQ assignment](https://github.com/gshanemiller/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L197) so it's staggered by CPU core

These configurations are based on the Equinix's `c3.small.x86` offering which has 1 CPU with 8 cores and two hyper-threads per core. We use all 16 virtual CPUs making 8 clients per client box and 8 servers per server box. That amounts to 8 RXQs and 8 TXQs per client box and ditto server side assuming clients and servers are both `c3.small.x86`

Putting these configs together we get on the client side:

* Core 0 VCPU #0 for RXQ0 for client 0; core 0 VCPU#1 for TXQ0 for client 0 flow controlled to 192.168.0.2 udpDestPort 1
* Core 1 VCPU #2 for RXQ1 for client 1; core 1 VCPU#3 for TXQ1 for client 1 flow controlled to 192.168.0.2 udpDestPort 2
* etc.

and similarly for the server.

This is just the bare bones. Packet (DPDK rte_mbuf) memory management is elided. And at the engineering level, there are important implications for whether the RXQ performs the request work or if the TXQ does it. This comes with additional challenges of transferring request/response state between threads. It also has impacts linearizability.

# Response Processing: Roles and Responsibilities
There are only three kinds of server threads that could be charged with doing the work in the request:

* the server RXQ lcore (dispatch thread in eRPC jargon) which got the request
* the corresponding TXQ lcore (alterate dispatch thread) which is supposed to send the response
* generic worker threads

[eRPC Section 3.2 Worker Threads](https://www.usenix.org/system/files/nsdi19-kalia.pdf) smartly describes the trade-offs:

```
A key design decision for an RPC system is which thread runs an RPC handler. Some RPC systems such as RAMCloud use
dispatch threads for only network I/O. RAMCloud’s dispatch threads communicate with worker threads that run request
handlers. At datacenter network speeds, however, interthread communication is expensive: it reduces throughput and adds
up to 400 ns to request latency [56]. Other RPC systems such as Accelio and FaRM avoid this overhead by running all
request handlers directly in dispatch threads [25, 38]. This latter approach suffers from two drawbacks when executing
long request handlers: First, such handlers block other dispatch processing, increasing tail latency. Second, they
prevent rapid server-to-client congestion feedback, since the server might not send packets while running user code.

Striking a balance, eRPC allows running request handlers in both dispatch threads and worker threads: When registering
a request handler, the programmer specifies whether the handler should run in a dispatch thread. This is the only
additional user input required in eRPC. In typical use cases, handlers that require up to a few hundred nanoseconds use
dispatch threads, and longer handlers use worker threads.
```

Now, the static routing topic above is designed so that RXQ/TXQ lcore pairs are distinct. But this is a design choice too. One DPDK lcore could handle both i.e. read request from its RXQ transmitting a response through its TXQ. Combining both eliminates interthread communication. Charging worker threads with request work involves at least one interthread handoff. The RXQ must find an eligble worker then transfer request state to it. The worker either transmits the response over a TXQ it owns, or gives the response data to a separate TXQ lcore through a second interthread handoff.

Before moving to solutions we need to look at *linearizability* and *queue processing*. The only thing that can be reasonably excluded now is interthread communication of large responses. If computing the result runs on one thread, and is transmitted to the requester in another thead, the response must either be copied or the threads must carefully avoid data races. KV requests are typically small whereas responses can be comparitively large.
 
# Linearizability 
If the KV system allows writes it must guarantee sensible responses interleaved with reads. *Linearizability* is the strongest guarantee. It comes in three parts:

* each request is processed exactly once
* linearizability isn't broken by node crashes or timeouts
* read/writes obey linearizability

[Stanford's Ousterhout et al.](https://web.stanford.edu/~ouster/cgi-bin/papers/rifl.pdf) discuss the first two parts. This section discusses the third. The seminal work on linearizability is [Herlihy's *Linearizability: A Correctness Condition for Concurrent Objects*](https://cs.brown.edu/~mph/HerlihyW90/p463-herlihy.pdf). Herlihy's p466 diagram is widely reproduced giving the intuition for linearizability:

```
     W(0)  A               R(1) A              W(0) C
|---------------|    |---------------|    |---------------|


                                    W(1) B                               R(0) B
                  |-------------------------------------------|    |---------------|

                            Valid Linearizability
```

whereas this small variation is **invalid**:

```
     W(0)  A               R(1) A              W(0) C
|---------------|    |---------------|    |---------------|


                                    W(1) B                               R(1) B
                  |-------------------------------------------|    |---------------|

                            Invalid Linearizability
```

Here three concurrent client processes `A, B, C` are reading and writing the same integer in the server's memory. Time increases left to right. The span of time it takes the server to handle a request is indicated by a line bounded by `|`. Overlapping lines represents the server concurrently handling requests in at least two threads. The code `W(0) A` means client A requested 0 to be written to memory. `R(0) B` means client B got 0 in its response reading the same memory.

In the valid case, A reads 1 because the server already updated the memory with B's write request of 1. As the server continues to make and transmit OK to client B, client C overwrites the memory with 0. C's response is transmitted before B's. Finally, client B reads the memory getting 0 reflecting C's write. The invalid case has this critical difference: the final read by B gets 1 because it doesn't see C's write. Quoting Herlihy [p467]:

```
The latter is intuitively unacceptable because A did a previous read of a 1, implying that B’s write of 1 must have
occurred before A’s read. C’s subsequent write of 0, though concurrent with B’s write of 1, strictly follows A’s
read of 1. 
```

Linearizability correctness runs into DPDK queue processing in two ways. First, a RXQ contain packets for multiple different RPC requests at different stages of completion. Although many RPCs require one just request packet, packet flow control (below) and multi-packet RPCs require two or more request packets. This means the server must carefully start or complete read/writes so linearizability is obeyed.

The second and larger problem is worker threads. Suppose a server's request RXQ contains several read/write requests for the same key. If the server runs the requests concurrently through worker threads system behavior is undefined. Putting aside the obvious race condition reading/writing the same KV pair memory, this scenario is just Herlihy's counter-example in a different context. Multi-key writes amplifies this problem considerably.

Now, in the context of KV alone, worker threads are probably net-bad. If key lookup is fast one distinguished thread can handle each request sequentially. Linearizability is satisfied. But summarily excluding work threads is not consequence free. eRPC reminds lengthy request work in dispatch threads increases latency putting back pressure on clients. Linearizability partially conflicts with general RPC handling options.

Herlihy defines linearizability (p468) as a partial order. For any two events `e0, e1` the server sequence history H in which it processed `e0, e1` satifies linearizability if:

```
e0 <_H e1 if server's response(e0) precedes request(e1) in H. H is irreflexive.
```

This ordering is *partial* not *total*. That means that not all client requests `e0, e1` can be ordered as `e0<e1` or `e1<e0` by linearizability in the server. For example, events going to different servers are not (generally) linearizability unless 2PC (two phase commit) is in effect. `e0 <_H e0` is not defined. H is irreflexive.

The server must interleave all concurrent read/write requests *as if there was only one client* who sent all requests for all clients in *some sequential order*, and the server processed them in that same order. The issue **isn't** the per-client sequential order. It's true `A, B, C` send their requests in an order each according to their own preference. But no client can demand a global sequential order at the server for other clients. All clients race to the server with work. The server determines what's done first. 

Granted we usually expect a client's `e0` at 9AM to be processed before its `e1` at 10PM. We don't expect the server to reorder client requests. Note network congestion in which `e0` sent at 9AM was actually delivered to the server after `e1` at 10PM is not in scope for linearizability. If `e0, e1` are bonafide events from the same client, they were delivered out of order. Packet flow and/or congestion control will deal with this problem before the server attempts to run them.

In any case the problem space is mainly those events which overlap in time in the server. And that happens when the server wants to increase throughput by running events in parallel. The counter example violates linearizability because the response for `W(0) C` came before the request for `R(1) B` so B should have read 0 not 1.

This nifty definition however elides one important detail in the following diagram. It's  exactly the valid diagram above however with an asterisk `*` indicating the point of linearizability plus labeled timestamps. Asterisks show the point in time at which the write (read) is logically and atomically complete:

```
     W(0)  A               R(1) A              W(0) C
|-------*-------|      |------*--------|    |----------*----|
|       |       |      |      |        |    |          |    |
|       |       |      |      |        |    |          |    |
|       |       |      |      |        |    |          |    |              R(0) B
|       |       | |--*-+------+--------+----+----------+----+---|    |---*-----------|
|       |       | |  | |      | W(1) B |    |          |    |   |    |   |           |
|       |       | |  | |      |        |    |          |    |   |    |   |           |
|       |       | |  | |      |        |    |          |    |   |    |   |           |
t0     t1       | t3 | t5    t6       t7    t8        t9   t10  t11  |  t13         t14
               t2    t4                                              t12            

                  Valid Linearizability Points
```

Consider timestamps t2-t11. After t2 before t3 the server did no work. At t3 the server receives `W(1) B`. By t4 the server effected all updates to the shared memory location race condition free for `W(1) B`. However, it took the server until t11 to get the response for `W(1) B` one the wire. At time t6 `R(1) A` completed its full read race condition free of the same shared memory. Even though `R(1) A` overlaps and runs concurrently with `W(1) B` the server has ensured the operations deal with the memory as if it was atomic for the entire server box. The fact that a particular request like `W(1) B` runs far past linearizability point to flush its response is not material. What's material is that the server chooses *linearizability points* satisfying the linearizability definition.

# Queue Processing

## Client/Server Context
The static routing diagram shows RXQ/TXQ queue pairs in the client and server are similarly designed. RXQ/TXQ lcores are distinct, which is one design choice. However, there are contextual differences processing packets. Per Packet Flow Control (below) clients only initiate RPCs. Servers never unilaterally send data to clients. This means client TXQ processing of server responses may potentially leverage state it previosuly made for the request.

RXQ server side does not have this advantage. The server only becomes aware of RPC work when it takes a packet off the wire. Eventually the server might parlay its accumulated request state RXQ side when it later makes and sends TXQ response packets.

RXQ processors on both clients and servers are saddled with RPC identification. The next section goes into more detail. The key problem is knowing whether packet in hand is for a previously known RPC or a brand new RPC. Finding or creating this state must be fast. Removing state for completed RPCs is also a requirement. 

Consequently, the need to deftly transitition RXQ to TXQ is important. The request contains the client IP address and port which should get the reply. The response needs that. The response data has to be handy. Responses can't be attemped or transmitted until the full RPC request is ready. There's no guarantee a request is well-formed in the first packet alone. If TXQ processors leverage RXQ side state, the state has to be made race condition free.

## Mechanics of RXQ Processing
Much but not all RXQ processing lead to DPDK library calls. But once a request packet has been read, the processor must know if it's a new RPC or a continuation packet of an earlier request. In general client RXQs hold responses from multiple distinct responses from multiple servers. Server RXQs hold requests for multiple distinct clients. Packet order within these RXQs is unknown.

Request packets must uniquely describe the client. [RIFL](https://web.stanford.edu/~ouster/cgi-bin/papers/rifl.pdf) partially identifies clients by a [lease ID](https://pkg.go.dev/go.etcd.io/etcd/client/v3#LeaseID). However, the same client can run multiple distinct RPCs, for example, find the value for key K1 then K2 in a second RPC. If we assign a sequence number to each new RPC by leaseId we can make a packet data prefix:

```
struct PacketPrefix = {    leaseId:  uint64,  // leaseId unique per client
                           rpcSeqId: uint32,  // rpcSeqId per leaseId
                           pktSeqId: uint8,   // packet number in rpcSeqId
                           pktType:  uint8    // e.g. REQUEST, RESPONSE, CREDIT_RETURN etc.
};

struct RequestPacket: public PacketPrefix { ... };   // more data
struct Responseacket: public PacketPrefix { ... };   // more data
```

At 14-16 bytes each depending on alignment, it allows at most `0xFFFFFFFF` distinct RPCs per lease, and at most 255 packets per RPC. I assume leases are destroyed long before `0xFFFFFFFF` or that the client is forced into a new leaseId on wrap.

Regardless of the dispatch/worker thread set design, the first RXQ processor task is determining if `(leaseId, rpcSeqId)` is known or unknown. Next steps include:

* create new RPC state when the first packet of a new RPC is seen
* update RPC state if needed for continuation packets
* know if a request is well-formed initiating response work
* dropped/reordered packets: detecting congestion problems depends on prior state
* handle dups part of exactly-one-processing and linearizability

`(leaseId, rpcSeqId)` lookup is a classic data structure excercise so instead I analyze data sizing. [Queue.cpp](https://github.com/gshanemiller/reinvent/blob/main/experiment/packet_flow/flow.cpp) demonstrates filling a queue with packets and processing it. The code runs from *the client perspective* e.g. processing a RXQ in the client. A full server response is 5 packets per RPC.

From BDP discussion (below) we know there's at most BDP per RPC in the queue. Second, the RXQ itself has finite capacity but certainly less than the rack's switch. Third, packet-level-flow (also below) limits each sender to C credits. Using the numbers in the BDP workup, each RPC session has 13 credits:

```
BDP = 19Kb
MTU = 1500 bytes
C   = BDP/MTU = 13 credits per RPC
```

A RXQ with N entries can hold N distinct RPCs in the worst case. This could happen if, improbably, servers managed to get exactly one response packet from N distinct requests into the client's RXQ at the same time. `queue.cpp` models 5 response packets. If we assume that, on average, half of a response per RPC is in queue one can revise the estimate downward to `N / (2.5)`. For a 512 entry RXQ that's 204 unique RPCs. Note that RXQ sizing has two dimensions: the total data size for all packets inqueue, and the number of packets N holding the data. This could be a few huge packets or many small packets. RPC identification depends on packet count only not data size.

The point: the search space is **very likely less than 1,000 identifiers**.

## Exactly-Once-Processing
Whether due to client error or due to congestion error recovery server side of RXQ processing must not re-run old RPCs. Clearly, errorenously re-running a mutation twice corrupts data:

```
Time t0: request 123: put(k, v)       <-- k = v
Time t1: request 322: get(k)          <-- return v
Time t2: request 432: put(k, v1)      <-- k = v1
Time t3: request 123: put(k, v)       <-- repeat 123 corrupts #432
```


# BDP (Bandwidth Delay Product)
Typical data center networks are two tier:

```
                                  +----------+
                                  | Router#1 | Top-of-Rack (tier-1)
                                  +----+-----+
                                 ^     ^     ^
                                /      |      \
                Tier2          /       V       \      Tier 2
              +----------+<---/   +----------+  \-->+----------+
              | Rack#1   |        | Rack#2   |      | Rack#3   |     <- Rack holding host boxes
              | subnet#1 |        | subnet#2 |      | subnet#3 |     <- Rack switch for its subnet
              +----------+        +----------+      +----------+
              | A hosts  |        | B hosts  |      | C hosts  |     <- boxes in rack with NICs
              +----------+        +----------+      +----------+     <- talking to rack switch
```

Packet movement between hosts in the same subnet goes through its respective switch 1, 2, or three. Packet movement between subnets must go out through a switch into the router down to the destination switch into the destination host.

Suppose switch#1 has a 12Mb buffer, 25Gbps bandwidth, and an average 6us RTT in its subnet. Now consider a distinct sender/receiver host pair `(a1, a2)` in subnet#1. Ideally `a1` would transmit data to `a2` at the switch's bandwidth capacity. Assume host box NICs equal or exceed the switch's bandwidth. Once `a2` gets data it transmits `ACKs` to `a1`. So after the first packet of the first message from `a1` is on the wire, it'll require 6us to get an `ACK` by assumption for that message. While `a1` awaits `ACKs` its shoving data on the wire.

The amount of un-ACK'd data in the pipeline between `(a1, a2)` is called the BDP (Bandwidth Delay Product) no matter if it's on the wire, in the switch, or processing in the receiver. So starting at time `t=0us` when the switch is empty and there's no I/O until `t=6us=RTT` the BDP is ~19Kb:

```
BDP = bandwidth * RTT
>>> float(25/8) * (1024*1024*1024) / 1e6 * 6
~20133 bytes
```

This calculation converts 25Gbps to bytes per 1 million microseconds (e.g. 1 sec) then multiplies by the RTT of 6us giving ~19Kb. 

Now, consider what happens there's multiple `(a1, a2)` pairs pushing data through switch#1. The switch can only buffer 12Mb. So if all senders work at 25Gbps there can be at most `12Mb + BDP` of un-ACK'd data over a 6us time period before the switch drops data. In each RTT period 19Kb can be acknowledged allowing a new 19Kb to enqueue. In this way BDP approximates the maximum amount of data each sender should have outstanding before stopping for ACKs. It's not practically possible for senders to transmit a large amount of data without stopping for ACK at some point. BDP is that point.

[Per eRPC Presentation Slide #10](https://www.usenix.org/sites/default/files/conference/protected-files/nsdi19_slides_kalia.pdf) the theoretical number of incasts is `12Mb/19Kb` or about 640. An incast is a data transmitter which pushes data over the network, say, via a RPC. However, the slide also points out just 50 incasts is preferrable based on industry studies. eRPC was tested at 100 incasts without data loss suggesting eRPC is a better steward of bandwidth. 

But in all cases switch buffer sizes must greatly exceed BDP for practical incast counts. Note BDP is bounded from above by the *slowest link in the network*. It's also worth noting that plentiful switch/router buffer while honoring BDP avoids a lot of congestion and packet loss. Per [eRPC's white paper](https://www.usenix.org/system/files/nsdi19-kalia.pdf) section 5.2.2 *Common-case optimizations*:

```
Recent studies of Facebook’s datacenters support this claim: Roy et al. [60] report  that 99% of all datacenter links
are less than 10% utilized at one-minute timescales. Zhang et al. [71, Fig. 6] report that for Web and Cache traffic, 
90% of top-of-rack switch links, which are the most congested switches, are less than  10% utilized at 25 us timescales.
```

## Packet Flow Control
BDP has implications for other parts of the design. [eRPC's white paper](https://www.usenix.org/system/files/nsdi19-kalia.pdf) discusses packet-level flow control in section 4.3.1 *Session credits*.  First, since any userspace network I/O runs over finite sized queues, it's pointless to transmit new packets if the corresponding RXQ holding response data is full. NICs drop data on queue-full. With BDP this also limits outstanding un-ACK'd data.

Second, each eRPC session (RXQ/TXQ lcore pair here) is provisioned `C` credits on creation where `C = BDP/MTU` to achieve line rate. The paper notes Mittal et al. [53] have proposed similar flow control for RDMA NICs. Credits are spent transmitting packets, and replentished when transmitters get responses. Transmitters must await responses before sending more data if it's out of credits. Maintaining the transmitter invariant `C>=0` is best done with client initiated messaging. Servers never unilaterally send data except unless the client requests it first. *Section 5.1 Protocol messages* eRPC describes this packet protocol:

* (Required) client request packet to start a new RPC
* (Optional) client initiated request-for-response (RFR 16-bytes) when responses require multiple continuation packets
* (Required) server credit return (CR 16-bytes) packet sent to client for each incoming packet
 
The final CR is included in the final server RPC response. In the simplest case, a RPC requires only two packets one each for request and response. CRs may included in the final response.

## Packet Drop/Loss
DPDK moves UDP packet drop/loss detection responsibilities onto the library developer. The main challenge here is retransmissions  

For simplicity, eRPC treats reordered packets as losses by
dropping them. This is not a major deciency because datacenter networks typically use ECMP for load balancing,
which preserves intra-ow ordering [30, 71, 72] except during rare route churn events. Note that current RDMA NICs
also drop reordered packets [53].
On suspecting a lost packet, the client rolls back the request’s wire protocol state using a simple go-back-N mechanism. It then reclaims credits used for the rolled-back transmissions, and retransmits from the updated state. The server
never runs the request handler for a request twice, guaranteeing at-most-once RPC semantics.
In case of a false positive, a client may violate the credit
agreement by having more packets outstanding to the server
than its credit limit. In the extremely rare case that such an
erroneous loss detection occurs and the server’s RQ is out
of descriptors, eRPC will have “induced” a real packet loss.
We allow this possibility and handle the induced loss like a
real packet loss.


## Congestion and Rate Limiting

## Client Initiatiated Protocol


## Linearizability and Exactly Once Requests
In order to implement exactly-once semantics, RIFL must
solve four overall problems: RPC identification, completion
record durability, retry rendezvous, and garbage collection.
This section discusses these issues and introduces the key
techniques RIFL uses to deal with them; Section 4 describes
the mechanisms in more detail.
In order to detect redundant RPCs, each RPC must have
a unique identifier, which is present in all invocations of
that RPC. In RIFL, RPC identifiers are assigned by clients
and they consist of two parts: a 64-bit unique identifier for
the client and a 64-bit sequence number allocated by that
client. This requires a system-wide mechanism for allocating
unique client identifiers. RIFL manages client identifiers
with a lease mechanism described later in this section.
The second overall problem is completion record durability. Whenever an operation completes, a record of its completion must be stored durably. This completion record must
include the RPC identifier as well as any results that are
returned to the client. Furthermore, the completion record
must be created atomically with the mutations of the operation, and it must have similar durability properties. It must
not be possible for an operation to complete without a visible completion record, or vice versa. RIFL assumes that the
underlying system provides durable storage for completion
records.
The third problem is retry rendezvous: if an RPC completes and is then retried at a later time, the retries must
find the completion record to avoid re-executing the operation. However, in a large-scale system the retry may not be
sent to the same server as the original request. For example, many systems migrate data after a server crash, transferring ownership of the crashed server’s data to one or more
other servers; once crash recovery completes, RPCs will be
reissued to whichever server now stores the relevant data.
RIFL must ensure that the completion record finds its way
to the server that handles retries and that the server receives
the completion record before any retries arrive. Retry rendezvous also creates issues for distributed operations that
involve multiple servers, such as multi-object transactions:
which server(s) should store the completion record?
RIFL uses a single principle to handle both migration and
distributed operations. Each operation is associated with a
particular object in the underlying system, and the completion record is stored wherever that object is stored. If the
object migrates, then the completion record must move with
it. All retries must necessarily involve the same object(s),
so they will discover the completion record. If an operation
involves more than one object, one of them is chosen as a
distinguished object for that operation, and the completion
record is stored with that object. The distinguished object
must be chosen in an unambiguous fashion, so that retries
use the same distinguished object as the original request.
The fourth overall problem for RIFL is garbage collection: eventually, RIFL must reclaim the storage used for
completion records. A completion record cannot be reclaimed until it is certain that the corresponding request will
never be retried. T



https://web.stanford.edu/~ouster/cgi-bin/papers/rifl.pdf


## ACK/NACK
The design is errorless single message movement both at client and server.  

## Multi-Packet Request/Response
TBD







Appendix B. Handling node failures







## SPSC Performance
Imposing a SPSC between server side RXQ/TXQs lcores adds latency. The [SPSC implementation](https://github.com/gshanemiller/reinvent/blob/main/integration_tests/reinvent_dpdk_static_route_udp/reinvent_dpdk_static_route_udp_test_spsc.h) was benchmarked on `c3.small.x86`. It can move around 105 Mb/sec or about 10ns per 64-byte message running two threads on the same core. The SPSC had a fixed capacity of 5000 data elements each 64-bytes in size. This flow enqueues one message, and dequeues one message at a time. Batching read/writes is not coded yet. I consider this acceptable loss.

## Packet memory
[As per DPDK](https://github.com/gshanemiller/reinvent/blob/static/doc/packet_design.md#packet-memory-mental-picture) packet memory is wrapped in a structure called `rte_mbuf`. Assuming the NIC supports `RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE` (Mellanox NICs can) the DPDK library will automatically free transmitted packets.

In the **client**:

* TXQ side allocate a `rte_mbuf` from its mempool, prepares, and transmits it
* DPDK `RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE` will free it after transmission on the wire
* RXQ side receives one or more packets into a vector of `rte_mbuf*`; this memory comes from the RXQ's mempool
* After each message is processed RXQ side, `rte_pktmbuf_free` is called on it

The **server side** is complicated by SPSC, potential race conditions managing packet memory, and by the different size requirements of response packets. Let's take these issues in reverse order.

First, keys are limited to 16-64 bytes, but values could be several kilobytes. Therefore, there's no ultimate benefit in transferring over SPSC the received request packet e.g. each `rte_mbuf*`. It's likely not big enough to hold the value. That is, it cannot be mutated to hold the value no matter where that's attempted.

Second, transferring each `rte_mbuf*` over SPSC moves the responsibility of freeing the `rte_mbuf*` (allocated from the RXQ's mempool) to the lcore handling its respective TXQ. Now, we could rely on `RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE` to free it --- assuming we mutated it to hold the response, which isn't practically possible anyway --- however this creates a race condition. The RXQ side must be careful to not write new packets from new incoming requests into `rte_mbuf*` that the TXQ side hasn't processed onto the wire. This is a second reason to avoid moving RXQ `rte_mbuf*` from the RXQ lcore to its resp. TXQ lcore over SPSC.

Third, we want to avoid double copying the value especially if it's several KB. If the RXQ is tasked with looking up the value of the key in the request, it must transfer that value to the TXQ by including it in the message enqueued onto the SPSC. The value would have to be copied yet again when the outgoing response packet is made. Therefore, it's better to copy the key only, and move the key lookup into the TXQ lcore. It can copy the value once into the outgoing packet which can be sized better.

Thus we modify the design **server side** as follows:

* Server RXQ side queues receive incoming packets in a `rte_mbuf*` array. The memory comes from the RXQ's mempool
* The server RXQ enqueues the key, plus src/dst IP address and destination UDP ports onto the SPSC approximately 64 bytes total if the key is no larger than 16-bytes
* The server RXQ calls `rte_pktmbuf_free` on each packet after SPSC enqueue
* In the server's resp. TXQ lcore, messages are dequeued from the SPSC one at a time
* Using the key in the message a key-find is performed
* TXQ side allocate a `rte_mbuf` from its mempool of sufficient size to hold the value, prepares, and transmits it
* DPDK `RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE` will free it after transmission on the wire

This allows the schema for requests and responses to differ since their packet memory layout is also different.

## Hot CPUs
DPDK is poll based. [As shown in the simple UDP example](https://github.com/gshanemiller/reinvent/blob/main/doc/equinix_mellanox_setup.md#rss-benchmark-output-1) the CPU can spin for packet work faster than it sometimes comes evidenced by the `stalledTx, stalledRx` metrics. SPSC is another hot-spin loop opportunity: queues may be empty (on read) or full (on write). To avoid running at 100% CPU utilization all the time, exponential backoff micro sleeps should be added. Once an event is finally found, the sleep time should be reset to the smallest delay.  
