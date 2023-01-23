# Status
This document is currently under heavy, daily revision. The tail end of the hold abandoned or in-progress sections

# Key Value RPCs
Evolving from [a trivial DPDK UDP example](https://github.com/gshanemiller/reinvent/blob/main/doc/equinix_mellanox_setup.md) to practical RPC (Remote Procedure Call) for scalable KV (Key Value) processing is difficult. Real world RPC under DPDK must meet and exceed all the excellent features of [eRPC](https://www.usenix.org/system/files/nsdi19-kalia.pdf) adding:

* linearizability
* exactly-once message processing

Then add what RAMCloud does:

* KV data structure
* crash recovery
* durable writes 
* add/remove nodes at will

Composing each feature into a well engineered whole is hard. This document describes each technical challenge with periodic pauses to summarize and integrate. In the second half of the document, a practical design is given.

To motivate this work consider the [static routing scenario discussed in the packet design doc.](https://github.com/gshanemiller/reinvent/blob/main/doc/packet_design.md#flow-control) Here a client `C` wants to obtain the value `V` for a key `K`. To do this, the client first determines the IP address and destination port `S` of the server which handles `K`. `C` send sends a request to `S`. `S` responds to `C` with the value `V` for `K` or nil if not found. This is static routing in the sense `C` chooses `S`. Determining `S` is not discussed here but, for example, `C` could consult `etcd` or some other service which knows which `S` handles which keys `K`.  

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
* the corresponding TXQ lcore (alternate dispatch thread) which is supposed to send the response
* generic worker threads

[eRPC Section 3.2 Worker Threads](https://www.usenix.org/system/files/nsdi19-kalia.pdf) smartly describes the trade-offs:

```
A key design decision for an RPC system is which thread runs an RPC handler. Some RPC systems such as RAMCloud use
dispatch threads for only network I/O. RAMCloud’s dispatch threads communicate with worker threads that run request
handlers. At data center network speeds, however, inter-thread communication is expensive: it reduces throughput and adds
up to 400 ns to request latency [56]. Other RPC systems such as Accelio and FaRM avoid this overhead by running all
request handlers directly in dispatch threads [25, 38]. This latter approach suffers from two drawbacks when executing
long request handlers: First, such handlers block other dispatch processing, increasing tail latency. Second, they
prevent rapid server-to-client congestion feedback, since the server might not send packets while running user code.

Striking a balance, eRPC allows running request handlers in both dispatch threads and worker threads: When registering
a request handler, the programmer specifies whether the handler should run in a dispatch thread. This is the only
additional user input required in eRPC. In typical use cases, handlers that require up to a few hundred nanoseconds use
dispatch threads, and longer handlers use worker threads.
```

Now, the static routing topic above is designed so that RXQ/TXQ lcore pairs are distinct. This one design choice. One DPDK lcore could handle both i.e. read request from its RXQ transmitting a response through its TXQ. Combining both eliminates inter-thread communication. Delegating \textt{get/put} requests to worker threads work involves at least one inter-thread hand-off. The RXQ must find an eligible worker then transfer request state to it. The worker either transmits the response over a TXQ it owns, or gives the response data to a separate TXQ lcore through a second inter-thread hand-off. Workers compete for access to a shared data structure.

Before moving to solutions we need to look at *linearizability* and *queue processing*. The only thing that can be reasonably excluded now is inter-thread communication of large responses. If computing the result runs on one thread, and is transmitted back to the requester in another thread, the response must either be copied or the threads must carefully avoid data races. KV requests are typically small whereas responses can be comparatively large.
 
# Linearizability 
If the KV system allows writes it must guarantee sensible responses interleaved with reads. *Linearizability* is the strongest guarantee. It comes in three parts:

* each request is processed exactly once
* linearizability isn't broken by node crashes or timeouts
* read/writes obey linearizability

[Stanford's Ousterhout et al.](https://web.stanford.edu/~ouster/cgi-bin/papers/rifl.pdf) discuss the first two parts. This section discusses the third. The seminal work on linearizability is [Herlihy's *Linearizability: A Correctness Condition for Concurrent Objects*](https://cs.brown.edu/~mph/HerlihyW90/p463-herlihy.pdf). Herlihy's p466 diagram is widely reproduced giving the intuition for linearizability:

```
     W(0)  A               R(1) A              W(0) C
|---------------|    |---------------|    |---------------|                                server thread 1


                                    W(1) B                               R(0) B
                  |-------------------------------------------|    |---------------|       server thread 2

                            Valid Linearizability as seen in the server
```

whereas this small variation is **invalid**:

```
     W(0)  A               R(1) A              W(0) C
|---------------|    |---------------|    |---------------|                                server thread 1


                                    W(1) B                               R(1) B
                  |-------------------------------------------|    |---------------|       server thread 2

                            Invalid Linearizability in the server
```

Here three concurrent client processes `A, B, C` are reading and writing the same integer in the server's memory. The server runs two response threads. Time increases left to right. The span of time it takes the server to handle a request is indicated by a line bounded by `|`. Overlapping lines represents the server concurrently handling requests in at least two threads. The code `W(0) A` means client A requested 0 to be written to memory. `R(0) B` means client B got 0 in its response reading the same memory.

In the valid case, A reads 1 because the server already updated the memory with B's write request of 1. As the server continues to make and transmit OK to client B, client C overwrites the memory with 0. C's response is transmitted before B's. Finally, client B reads the memory getting 0 reflecting C's write. The invalid case has this critical difference: the final read by B gets 1 because it doesn't see C's write. Quoting Herlihy [p467]:

```
The latter is intuitively unacceptable because A did a previous read of a 1, implying that B’s write of 1 must have
occurred before A’s read. C’s subsequent write of 0, though concurrent with B’s write of 1, strictly follows A’s
read of 1. 
```

Linearizability may conflict with DPDK queue processing in two ways. First, a server RXQ contain packets for multiple different RPC requests at different stages of completion. Although most RPCs require one just request packet, *packet flow control* (below) and multi-packet RPC requests require two or more request packets. This means the server must carefully start and complete read/writes so linearizability is obeyed.

The larger problem is worker threads. Suppose a server's request RXQ contains several read/write requests for the same key. If the server runs the requests concurrently through worker threads system behavior is undefined. Putting aside the obvious race condition reading/writing the same KV pair memory, this scenario is just Herlihy's counter-example in a different context. 

Herlihy defines linearizability (p468) as a partial order. For any two events `e0, e1` the server sequence history H in which it processed `e0, e1` satifies linearizability if:

```
e0 <_H e1 if server's response(e0) precedes request(e1) in H. H is irreflexive.
```

This ordering is *partial* not *total*. That means that not all client requests `e0, e1` can be ordered as `e0<e1` or `e1<e0` by linearizability in the server. For example, events going to different servers are not (generally) linearizability unless 2PC (two phase commit) is in effect. `e0 <_H e0` is not defined. H is irreflexive.

The server must interleave all concurrent read/write requests *as if there was only one client* who sent all requests for all clients in *some sequential order*, and the server processed them in that same order. The issue **isn't** the per-client sequential order. It's true `A, B, C` send their requests in an order each according to their own preference. But no client can demand a global sequential order at the server for itself *and* other clients. All clients race to the server with work. The server determines what's done first. Granted, we usually expect a client's `e0` at 9AM to be processed before its `e1` at 10PM. We don't expect the server to coarsely reorder client requests. 

In any case the problem space is mainly those events which overlap in time in the server. And that happens when the server wants to increase throughput by running events in parallel. The counter example violates linearizability because the response for `W(0) C` (t4 in the diagram below) came before the request for `R(1) B` (t6) so B should have read 0 not 1.

This definition however elides one important detail. The following diagram reproduces the valid case above with an asterisk `*` indicating *linearizability points*. Asterisks show the point in time at which the write (read) is logically and atomically complete:

```
     W(0)  A               R(1) A              W(0) C
|-------*-------|      |------*--------|    |----------*----|                              server thread 1
|       |       |      |      |        |    |          |    |
|       |       |      |      |        |    |          |    |
|       |       |      |      |        |    |          |    |              R(0) B
|       |       | |--*-+------+--------+----+----------+----+---|    |---*-----------|     server thread 2
|       |       | |  | |      | W(1) B |    |          |    |   |    |   |           |
|       |       | |  | |      |        |    |          |    |   |    |   |           |
|       |       | |  | |      |        |    |          |    |   |    |   |           |
t0     t1       | t3 | t5    t6       t7    t8        t9   t10  t11  |  t13         t14
               t2    t4                                              t12            

                  Valid Linearizability Points (*) in the server
```

Consider timestamps t2-t11. After t2 before t3 the server did no work. At t3 the server receives `W(1) B`. By t4 the server completed all updates to the shared integer memory. Whether you see the writes as atomic, or mutex protected insuring serialized exclusive access, t4 marks the place `W(1) B` is done in the linearized history. The server runs until t11 to get the response for `W(1) B` on the wire.

At time t6 `R(1) A` completed its full read race condition free of the same shared memory. Even though `R(1) A` overlaps and runs concurrently with `W(1) B` the server insures the operations deal with the shared memory sequentially. The fact that some requests run far past its linearizability point flushing response data is not material. What's material is that the server chooses *linearizability points* satisfying the linearizability definition.

# Queue Processing

## Client/Server Context
The static routing diagram shows RXQ/TXQ lcore queue pairs for the client; the server is similarly designed. RXQ/TXQ lcores are distinct, which is one design choice. However, there are contextual differences processing packets in clients versus servers. As described under *Packet Flow Control* (below) clients only initiate RPCs. Servers never unilaterally send data to clients. This means client RXQ processing of server responses may potentially leverage state it previously made for the request. Clients, unlike servers, initiate RPCs when it sent packets out. So it can anticipate responses for those same RPCs in its RXQ.

RXQ server side does not neccessarily have this advantage. The server may only becomes aware of RPC work when it takes a packet off the wire. The server might parlay its accumulated request state RXQ side when it later makes and sends TXQ response packets.

RXQ processors on both clients and servers are saddled with RPC identification. The next section goes into more detail. The key problem is knowing whether packet in hand is for a previously known RPC or a brand new RPC. Finding or creating this state must be fast. Removing state for completed RPCs is also a requirement. 

Consequently the need to deftly transitition RXQ to TXQ is important. The request contains the client IP address and port which should get the reply. The response needs that. The response data has to be handy. Responses can't be attemped or transmitted until the full RPC request is ready. There's no guarantee a request is well-formed in the first packet alone. If TXQ processors leverage RXQ side state, the state has to be made race condition free.

## Mechanics of RXQ Processing
Much but not all RXQ processing lead to DPDK library calls. But once a request packet has been read, the processor must know if it's a new RPC or a continuation packet of an earlier request. In general client RXQs hold responses from multiple distinct responses from multiple servers. Server RXQs hold requests for multiple distinct clients. Packet order within these RXQs is unknown.

Request packets must uniquely describe the client. [RIFL](https://web.stanford.edu/~ouster/cgi-bin/papers/rifl.pdf) partially identifies clients by a [lease ID](https://pkg.go.dev/go.etcd.io/etcd/client/v3#LeaseID). However, the same client can run multiple distinct RPCs, for example, find the value for key K1 then K2 in a second RPC. If we assign a sequence number to each new RPC by clientId we can make a packet data prefix:

```
struct PacketPrefix = {    clientId: uint64,  // unique clientId aka leaseId in RIFL
                           rpcSeqId: uint32,  // rpc sequenceId per clientId
                           pktSeqId: uint8,   // packet sequenceId per rpcSeqId
                           pktType:  uint8    // e.g. REQUEST, RESPONSE, CREDIT_RETURN etc.
};

struct RequestPacket: public PacketPrefix { ... };   // more data
struct Responseacket: public PacketPrefix { ... };   // more data
```

At 14-16 bytes each depending on alignment, it allows at most `0xFFFFFFFF` distinct RPCs per lease, and at most 255 packets per RPC. I assume leases are destroyed long before `0xFFFFFFFF` or that the client is forced into a new clientId on wrap.

Regardless of the dispatch/worker thread set design, the first RXQ processor task is determining if `(clientId, rpcSeqId)` is known or unknown:

* create new RPC state when the first packet of a new RPC is seen
* update RPC state if needed for request continuation packets
* know if a request is well-formed initiating response work
* dropped/reordered packets: detecting congestion problems depends on prior state
* handle duplicate requests part of exactly-one-processing and linearizability

`(clientId, rpcSeqId)` lookup is a classic data structure excercise so instead I analyze data sizing. [Queue.cpp](https://github.com/gshanemiller/reinvent/blob/main/experiment/packet_flow/flow.cpp) demonstrates filling a queue with packets and processing it. The code runs from *the client perspective* e.g. processing a RXQ in the client. A full server response is 5 packets per RPC.

From BDP discussion (below) we know there's at most BDP per RPC in the queue. Second, the RXQ itself has finite capacity but certainly less than the rack's switch. Third, packet-level-flow (also below) limits each sender to C credits. Using the numbers in the BDP workup, each RPC session has 13 credits:

```
BDP = 19Kb
MTU = 1500 bytes
C   = BDP/MTU = 13 credits per RPC
```

A RXQ with N entries can hold N distinct RPCs in the worst case. This could happen if, improbably, servers managed to get exactly one response packet from N distinct requests into the client's RXQ at the same time. `queue.cpp` models 5 response packets. If we assume that, on average, half of a response per RPC is in queue one can revise the estimate downward to `N / (2.5)`. For a 512 entry RXQ that's 204 unique RPCs. Note that RXQ sizing has two dimensions: the total data size for all packets in-queue, and the number of packets N holding the data. This could be a few huge packets or many small packets. RPC identification depends on packet count only not data size.

The point: the search space is **very likely less than 1,000 identifiers**.

## RPCs vs. RPC Packets v. Sessions
Reverse engineering the `rpcId = (clientId, rpcSeqId)` out of each packet is **too much work**. An online mutable data structure is required. eRPC eliminates this code by adding data to each packet from which a *session slot* is extracted. Now, no packets are in scope anywhere until a client creates a RPC. Clients cannot create or run an RPC unless they first have a session connecting its host box to the target server box. eRPC defines a *session* in section 3.1:

```
A session is a one-to-one connection between two Rpc endpoints, i.e., two user threads. The client endpoint of a
session is used to send requests to the user thread at the other end. A user thread may participate in multiple
sessions, possibly playing different roles (i.e., client or server) in different sessions.
```

If an RPC can be directly or indirectly indexed by `i` through its session, this index can be sent with its packets to locate state efficiently in both clients and servers. In conventional kernel based work code a session is the connection object:

```
// Runs in main thread or app created thread

// Create a session from this box's IP, say 4.128.10.168, to destination:
UDP::Connection con("10.10.0.128:332");
con.connect();
con.send(...);
con.receive(...);
con.disconnect();

// Do more work with another endpoint in a new session:
UDP::Connection con1("23.43.22.33:4140");
con1.connect();
. . .
con1.disconnect();
```

In section 4.3 eRPC explains session management in a bit more detail:

```
Each session maintains multiple outstanding requests to keep the network pipe full. Concurrently requests on a session
can complete out-of-order with respect to each other. This avoids blocking dispatch-mode RPCs behind a longrunning
worker-mode RPC. We support a constant number of concurrent requests (default = 8) per session; additional requests
are transparently queued by eRPC. This is inspired by how RDMA connections allow a constant number of operations [10].
A session uses an array of slots to track RPC metadata for outstanding requests
```

eRPC does session management through conventional kernel based APIs. From its `hello_world` example (elided):

```
// Create a RPC
auto rpc = new erpc::Rpc<erpc::CTransport>(...);

// Make a session indexed by 'session_num'
std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
int session_num = rpc->create_session(server_uri, 0);

// Send RPC request through 'session_num'
rpc->enqueue_request(session_num, kReqType, &req, &resp, cont_func, nullptr);
```

The method `create_session()` creates a new session object ultimately calling `sm_pkt_udp_tx_st(...)` to transmit the client's session information to the server who, after reading this packet, creates *mirror state*. Identifying information from this session handshake is included in RPC packets. Each session holds 8 sessions *slots* for active RPCs. If these slots are all used, new RPCs are buffered then moved into a free slot when possible FIFO. Through the session handshake both the client and server are initialized ahead of time so that when DPDK packets for the RPC itself are generated, both ends of the RPC interaction can quickly locate and manage state.

Note that eRPC has other session management messages for disconnect, reconnect, plus handling duplicate session requests.

Putting these pieces together we have:

* eRPC requires a session to be made before RPCs are run
* eRPC requires clients only initiate sessions
* Sessions are created over conventional kernel based sockets requiring client's and server's run a *Session Mananger* thread
* RPCs are assigned to a free session slot in a session. When slots are full, additional requests are queued moved into free slots later
* Identifying information (e.g. session number and slot number) from the session handshake is included in RPC packets sent through DPDK
* DPDK RXQ processors use the identifying session information in packets to find and manage state

## Problem: Linearizability vs. Session slots
Re-quoting eRPC "each session maintains multiple outstanding requests to keep the network pipe full. Concurrently requests on a session can complete out-of-order with respect to each other." In order for the whole system to be efficient, the server has to process requests quickly. Efficiency conflicts with linearizability:

* The server can't be efficient if its RXQs are mostly empty. That's why eRPC has session slots holding *in-process requests* to get more work into the server's RXQ
* The server can't be efficient if it can't work on a RPC in session slot #1 because it's waiting for work in slot#0 to finish. That'll put back pressure on clients and risk filling its RXQ to capacity leading to dropped packets
* The server would like the option of finishing requests out of order so more work per unit time is done
* The server can get more work done if it handles requests in parallel through worker threads
* But if finding an eligible worker and handing off work to it is slow, parallelism is hampered. Work can be done in dispatch threads to avoid this overhead, but parallelism remains low
* The server must obey linearizability, and avoid race conditions on shared memory

And that's why, quoting eRPC again, "a key design decision for an RPC system is which thread runs an RPC handler." To work towards a solution note that server request processing has three distinct parts:

* Waiting until the full request is received. Some requests need multi-packets. If a RPC request in slot #0 is incomplete, the server could work on other slots
* Performing the completed request e.g. KV get/put
* Sending response 

Linearizability isn't broken by incomplete requests; the server can't do anything with those RPCs yet. On the other hand if running requests satisfies linearizability, it can send responses out of order because, by that point, linearizability points are established. The question of whether the server delegates requests to a worker thread or handles it within a dispatch thread (RXQ or TXQ side) in turn depends on four things:

* Does the server support delegation through workers? Who determines if hand-off is needed? Does the client ask or the server decide?
* Is the data structure accessed by the requests serialized? Does it need to be?
* Is system performance better for a given use-case with workers?
* If the primary use case (e.g. KV get/put) is fast so workers are counter-indicated, should the RPC API still allow for delegating if new use-cases are slow? Specific RPC or general?

eRPC provides concrete answers: its RPC API allows client-requested delegation. **eRPC, however, never addresses linearizability** so its design is fundamentally incomplete.

## Problem: Linearizability vs. Session slots vs. New Sessions
Session life cycle maintenance runs concurrent with session slot processing. At any time the server may get new RPCs from existing clients or new sessions from new clients. Clients and servers participating in a session will in general have different session counts e.g. a client runs 10 sessions, but for one of those sessions terminating in server `N`, `N` might participate in 50 sessions because other clients are hitting it too. Something has to deal with connection managment while it processings existing RPCs without stepping on each other. eRPC comes with these types:

```
+--------------+   1:N    +---------------+   2:1 session index   +-----------------------+
| class SSlot  |----------| class Session |-----------------------| class SessionEndpoint |
| +----------+ |          | +-----------+ |                       +-----------------------+
| | client   | |          | | client    | |
| +----------+ |          | +-----------+ |                       +-------------+
| | server   | |          +---------------+                       | class Nexus |
| +----------+ |         /                 \ 1:N session idx.     +-------------+
+--------------+        +                   +                     | +---------+ |
                        |                   |                     | |  Hook*  | |
                        |                   |                     | +---------+ |
                        +                   +--------+            +-------------+   
                        | 1:1                        |            |
                       /                             |            |
+----------------+    /  dst session index           |   +--------+--+
| class pkthdr_t |---+                               +---| class RPC | (confusing name)
+----------------+                                       +-----------+
                                                         | +-------+ |
                                                         | | Hook  | |
                                                         | +-------+ |
                                                         +-----------+

```

All types *are used in clients and servers.* Usage needs context: Am I dealing with it client side or server side? SSlot has a bit identifying a SSlot object for client or server. Once this bit is known, SSlot has private objects holding client/server state. Session is similar holding a nested object for client side sessions only. It holds one set of SSlots for the server, and a second set for the client within its private client state.

A session by definition is a connection between two server endpoints. A session has session slots running the active sub-RPCs. Client RPCs go into active SSlots if one is free, or buffered in a queue if full. Servers don't buffer RPCs. If clients can run a RPC in a SSlot it's implied the server has capacity to handle it.

Session creation is a complex orchestration centered on Nexus, RPC. Each box *client or server runs one Nexus* object. On the server, it can optionally pre-register a class of background threads the client can request later in its RPC invocation. The Nexus is passed by pointer into `erpc::Rpc` together with `0` uniquely describing what the RPC does ex. `0==get`, `1==put`:
```
// eRPC server hello_world
1 void req_handler(erpc::ReqHandle *req_handle, void *) { ... }             <--- BG function pointer impl
2 int main() {
3  erpc::Nexus nexus(server_uri);
4  nexus.register_req_func(kReqType, req_handler);                         <--- register BG threads of kReqType==2
5  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, nullptr);     <--- Nexus-Hook-RPC orchestration for RPC type 0
6  rpc->run_event_loop(100000);                                            <--- server now running
}

// eRPC client hello_world
7 void cont_func(void *, void *) { printf("%s\n", resp.buf_); }
8 void sm_handler(int, erpc::SmEventType, erpc::SmErrType, void *) {}

int main() {
9  erpc::Nexus nexus(client_uri);
10 rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, sm_handler);  <--- Nexus-Hook-RPC orchestration for RPC type 0
11 int session_num = rpc->create_session(server_uri, 0);                   <--- creation session
12 rpc->enqueue_request(session_num, kReqType, &req, &resp, ...);          <--- queue RPC type 0 delegating to BG thread kReqType
13 rpc->run_event_loop(100);                                               <--- run event loop 
}
```

Additional RPC types 1, 2, 3, ... require new `Rpc` objects each sharing the same Nexus. The Nexus object creates and runs one background session management (SM) thread (lines 3, 9). Clients send SM packets (line 11) over conventional kernel based UDP to the server's Nexus who responds. Clients can only successfully run RPCs (ex. type 0) known to the server. The creation and registration an RPC type 0 for the server (line 5) is what ultimately ties RPC, Session, Hook together.

By line 3 server Nexus is running a server SM lister thread. Line 5 makes RPC type 0 taking a pointer to the Nexus. In the constructor for RPC we have (edited):

```
Rpc<TTr>::Rpc(...) {
  . . .
   // Register this->nexus_hook_ w/ Nexus. Nexus keeps a pointer
   // to this hook and initializes it running SM and BG queues.
   // These queues are used to hand off from Nexus' SM handler to
   // a specific RPC via its Hook object
1  nexus_hook_.rpc_id_ = rpc_id;                                           <--- e.g. 0
2  nexus->register_hook(&nexus_hook_);                                     <--- Nexus keeps a copy of &nexus_hook_
   . . .
}
```

Then later when the client runs line 11 to create a session, a client SM packet at the *server's SM handler* (edited):
```
void Nexus::sm_thread_func(SmThreadCtx ctx) {
   . . .
   // ctx is a Nexus::SmThreadCtx which has an array of Hooks by rpc type
   // these hooks pre-registered see above. So find Hook for type 0
1  Hook *target_hook = const_cast<Hook *>(ctx.reg_hooks_arr_[target_rpc_id]);
2  if (target_hook != nullptr) {
     // push connect SM packet onto Hook* ptr for target_rpc_id e.g. 0
3    target_hook->sm_rx_queue_.unlocked_push(SmWorkItem(target_rpc_id, sm_pkt));
  }
  . . .
```

Line 3 passes the connect SM packet to a queue inside the Hook inside the RPC object correct for it's type here 0. That packet is dequeued (edited):

```
void Rpc<TTr>::handle_sm_rx_st() {

1  while (queue.size_ > 0) {
2    const SmWorkItem wi = queue.unlocked_pop();

3    switch (wi.sm_pkt_.pkt_type_) {
4      case SmPktType::kConnectReq: handle_connect_req_st(sm_pkt); break;
}
```

and passed a RPC connect handler (edited):

```
void Rpc<TTr>::handle_connect_req_st(const SmPkt &sm_pkt) {
   // Handle duplicate session connect requests. sm_pkt.uniq_token_
   // is basically RIFL's leaseId
1  if (conn_req_token_map_.count(sm_pkt.uniq_token_) > 0) { ... }

   // If we are here, create a new session and fill preallocated MsgBuffers
2  auto *session = new Session(Session::Role::kServer, sm_pkt.uniq_token_,...);

   // init SSlots for this session
3  for (size_t i = 0; i < kSessionReqWindow; i++) {
     . . .
   }

   // Fill-in the server endpoint
4  session->server_ = sm_pkt.server_;
5  session->server_.session_num_ = session_vec_.size();
6  conn_req_token_map_[session->uniq_token_] = session->server_.session_num_;

   // Fill-in the client endpoint
7  session->client_ = sm_pkt.client_;

8  session->local_session_num_ = session->server_.session_num_;
9  session->remote_session_num_ = session->client_.session_num_;
10 session_vec_.push_back(session);  // Add to list of all sessions
```

The client SM's packet provides its own session number (lines 7, 9) for the server. The server makes its own session number (lines 2, 5, 9). The response SM packet sent to client has the server's session number. Sessions are held in an array per RPC until a SM packet asks for disconnect. And, finally, when packets are sent client-to-server or server-to-client over DPDK each pkthdr_t contains `dest_session_num_`. This is the target client session number (by RPC) when the server sends it, and the server's session number (by RPC) when the client sends it. All this is pre-orchestrated before DPDK packets are in-flight. 

We need two more things. The DPDK packets must be processed by a RXQ owned by the right RPC since the packet's `dest_session_num_` is a per RPC. DPDK flow-control does this. Finally, the RPC object's session array access must be MT safe with other processing. Either RPC uses locks or RPC objects do one thing at a time e.g. handle Session create/destroy or make progress on an existing RPC or read RXQ or transmit on some TXQ.

## Problem Encore: Linearizability vs. Session slots

eRPC has *one set of SSlots per session*. Recall SSlots hold active RPCs in some state of completion. The number of slots depends on BDP and a desire to keep the server's RXQ processor from being idle because its RXQ is starved for work. Session count is akin to incast count discussed in BDP below. To emphasize here's the code again:

```
void Rpc<TTr>::handle_connect_req_st(const SmPkt &sm_pkt) {
   // Handle duplicate session connect requests. sm_pkt.uniq_token_
   // is basically RIFL's leaseId
1  if (conn_req_token_map_.count(sm_pkt.uniq_token_) > 0) { ... }

   // If we are here, create a new session and fill preallocated MsgBuffers
2  auto *session = new Session(Session::Role::kServer, sm_pkt.uniq_token_,...);

   // Now make the SSlots for this session. kSessionReqWindow default 8
3  for (size_t i = 0; i < kSessionReqWindow; i++) {
    MsgBuffer &msgbuf_i = session->sslot_arr_[i].pre_resp_msgbuf_;
    msgbuf_i = alloc_msg_buffer(pre_resp_msgbuf_size_);
     . . .
   }
   . . .
```

Understanding RPC request processing in the server requires a few more pieces:

* What happens if the server's Session SSlots are full? 
* How does the server keep track of different RPC requests from different sessions?
* How does the server make progress completing requests?
* How does SSlot processing deal with linearizability?

From the eRPC server hello world code extract above, eRPC's API requires we run `rpc->run_event_loop(100000)` where `100000` is the maximum time (ms) it blocks. `run_event_loop` delegates to `run_event_loop_do_one_st`. Here's that code focusing on RX first (edited). Note source code marked "key":

```
// Server
void Rpc<TTr>::run_event_loop_do_one_st() {
  // Handle any new session management packets
  if (unlikely(nexus_hook_.sm_rx_queue_.size_ > 0)) handle_sm_rx_st();

  // The packet RX code uses ev_loop_tsc as the RX timestamp, so it must be
  // next to ev_loop_tsc stamping.
  ev_loop_tsc_ = dpath_rdtsc();
  process_comps_st();  // RX
  . . .
}

// Server
void Rpc<TTr>::process_comps_st() {
  // Read some packets
  const size_t num_pkts = transport_->rx_burst();
  if (num_pkts == 0) return;

  for (size_t i = 0; i < num_pkts; i++) {
    . . .
    // Find session for packet  w.r.t to this RPC object
    Session *session = session_vec_[pkthdr->dest_session_num_];

    // If we are here, we have a valid packet for a connected session
    const size_t sslot_i = pkthdr->req_num_ % kSessionReqWindow;     <--- key line!
    SSlot *sslot = &session->sslot_arr_[sslot_i];

    switch (pkthdr->pkt_type_) {
      case PktType::kReq:
        . . .
        break;
      case PktType::kResp: {
        . . .
        break;
      }
      case PktType::kRFR: {
        . . .
        break;
      }
      case PktType::kExplCR: {
        . . .
        break;
      }
    }
  }
  . . .
}
```

To understand how the server chooses its SSlot in the packet's session, look at how the client sends data. Recall `enqeue_request` in client hello world example above (edited). Note line marked key:

```
// Client
void Rpc<TTr>::enqueue_request(int session_num, uint8_t req_type,...) {
  // If a free sslot is unavailable, save to session backlog
  if (unlikely(session->client_info_.sslot_free_vec_.size() == 0)) {
    session->client_info_.enq_req_backlog_.emplace(...) return;
  }

  // Fill in the sslot info
  size_t sslot_i = session->client_info_.sslot_free_vec_.pop_back();
  SSlot &sslot = session->sslot_arr_[sslot_i];
  sslot.cur_req_num_ += kSessionReqWindow;                           <--- key line
  . . .

  // Fill in packet 0's header
  pkthdr_t *pkthdr_0 = req_msgbuf->get_pkthdr_0();
  pkthdr_0->req_type_ = req_type;
  pkthdr_0->msg_size_ = req_msgbuf->data_size_;
  pkthdr_0->dest_session_num_ = session->remote_session_num_;
  pkthdr_0->pkt_type_ = PktType::kReq;
  pkthdr_0->pkt_num_ = 0;
  pkthdr_0->req_num_ = sslot.cur_req_num_;
  . . .
}
```

And to understand the client's line we need this Session code which initializes the SSlots when new Sessions are made:

```
Session(Role role, ...) {
    . . .
    // Arrange the free slot vector so that slots are popped in order
    for (size_t i = 0; i < kSessionReqWindow; i++) {
      // Initialize session slot with index = sslot_i
      const size_t sslot_i = (kSessionReqWindow - 1 - i);
      SSlot &sslot = sslot_arr_[sslot_i];

      sslot.session_ = this;
      sslot.is_client_ = is_client();
      sslot.index_ = sslot_i;
      sslot.cur_req_num_ = sslot_i;  // 1st req num = (+kSessionReqWindow)
    }
    . . .
}
```

Based on what I can see this mod kSessionReqWindow does nothing:

```
#!/usr/local/bin/python3.9
max=8
i=0
index=[]

while i<max:
  sslotIndex = (max - 1 - i)
  index.append(sslotIndex)
  i=i+1

for item in index:
  print(item)
print

i=0
while i<len(index):
  j=1
  k=index[i]
  print("starting at slot {0} value {1}".format(i, k))
  while j<=10:
    k += max
    print("request {0} request%{1}={2} request&{3}={4}".format(k, max, k%max, max-1, k&(max-1)))
    j=j+1
  print
  i=i+1

```

It's a glorified way to pick a slot index. kSessionReqWindow doesn't seem to used for congestion, transport, or nexus. Specifically:

* Q: What happens if the server's Session SSlots are full? A: This can't happen. Sessions have the same number of SSlots client and server. Clients choose a free slot in its session because it initiates work, and the server uses the same slot. If client slots are full, its queued. Provided slots used and disused together the client could send its SSlot index without reference to kSessionReqWindow anywhere
* Q: How does the server keep track of different RPC requests from different sessions? A: DPDK packets are sent per session per slot. Each packet per session per slot has a monotonically increasing sequence number (edited):

```
struct pkthdr_t {
  uint32_t req_type_ : 8;                  // RPC request type
  uint16_t dest_session_num_;              // Session number at receiver
  uint64_t pkt_type_ : 2;                  // The eRPC packet type (request, CR, etc.)
  uint64_t pkt_num_ : kPktNumBits;         // Monotonically increasing packet number
  uint64_t req_num_ : kReqNumBits;
  uint64_t magic_ : k_pkt_hdr_magic_bits;  // So eRPC knows its packets from broadcast/ARP packets
};
```

Last two questions ... TBD

## Summary
TBD

# Congestion, Timely, Packet Drop/Loss
eRPC uses Timely packet RTT (Round Trip Time) measurements to control congestion. Applicable papers:

* [TIMELY: RTT-based Congestion Control for the Datacenter](https://conferences.sigcomm.org/sigcomm/2015/pdf/papers/p537.pdf)
* [TIMELY Power Point Slides](http://radhikam.web.illinois.edu/TIMELY-sigcomm-talk.pdf)
* [TIMELY Source Code from Author](http://people.eecs.berkeley.edu/~radhika/timely-code-snippet.cc) referenced and used in eRPC
* [ECN or Delay: Lessons Learnt from Analysis of DCQCN and TIMELY](http://yibozhu.com/doc/ecndelay-conext16.pdf). Contains Timely bug-fix 
* [Receiver-Driven RDMA Congestion Control by Differentiating Congestion Types in Datacenter Networks](https://icnp21.cs.ucr.edu/papers/icnp21camera-paper45.pdf)
* [eRPC Slides ](https://www.usenix.org/sites/default/files/conference/protected-files/nsdi19_slides_kalia.pdf) discuss Timely p15-16

*Receiver-Driven RDMA Congestion* significantly out performs Timely but requires boxes to br time-synced: "The deployment of RCC relies on high precision clock synchronization throughout the datacenter network. Some recent research efforts can reduce the upper bound of clock synchronization within a datacenter to a few hundred nanoseconds, which is sufficient for our work." eRPC/Timely does not require something like Google's TrueTime.

From the Timely PPS RTT looks like. Now, eRPC does not have large serializtion delays because (see packet flow control) client TXs are always matched against server TXs to replentish credits:
![Diagram1](timely_rtt.png?raw=true "DD1")




![Diagram2](timely_algo.png?raw=true "DD2")








Since slots are per Session and Sessions are 1:1 between client and server, 









Zooming out to client/server boxes we have:

```
+----Client-Box-A------+         +----Server-Box-B------+        
| +------------------+ |         | +------------------+ |
| | Logical client 1 | |         | | Logical server 1 | |
| | in thread 1      | |         | | in thread 1 with | |
| +------------------+ |         | | logical addr1    | |
| | Logical client 2 | |         | +------------------+ |
| | in thread 2      | |         | | Logical server 2 | |
| +------------------+ |         | | in thread 2 with | |
|           .          |         | | logical addr2    | |
|           .          |         | +------------------+ |
+----------------------+---+     |          .           |
   Box A IP address        |     |          .           |
                           |     | +------------------+ |
                           +-----+-| Session Manager  | |   eRPC runs per server mgmnt threads
                    kernel UDP   | | thread N for     | |   managed by a single Nexus object
                                 | | server 1         | |   in the background
                                 | +------------------+ |
                                 | | Server Manager   | |
                                 | | thread N+1 for   | |
                                 | | sever 2          | |
                                 | +------------------+ |
                                 |          .           |
                                 |          .           |
                                 +----------------------+
                                      Box B IP address

 Sessions are between A's IP address to a server's logical address
 through DPDK flow control 
```

When `client A.1` (client box1 logical client 1) wants run `get('test')` with `B.2` per eRPC we have:

```
etcd                         A.1                          B Session Mgr
--+--                       --+--                        -------+-------
  |                           |                                 |
  <---serverEndPoint('test')--+                                 |
  |                           |                                 |
  +---B.2--------------------->                                 |
  |                           |                                 |
  <---makeLeaseId('A.2')------+                                 |
  |                           |                                 |
  +----0x3938483-------------->                                 |
                              |                                 |
                              +----makeSession(A.2,0x3938483)--->


class SSlot {
  // Members that are valid for both server and client
  Session *session_;  ///< Pointer to this sslot's session

  /// True iff this sslot is a client sslot. sslot class does not have complete
  /// access to \p session, so we need this info separately.
  bool is_client_;

  size_t index_;  ///< Index of this sslot in the session's sslot_arr

  /// The request (client) or response (server) buffer. For client sslots, a
  /// non-null value indicates that the request is active/incomplete.
  MsgBuffer *tx_msgbuf_;

  /// Info about the current request
  size_t cur_req_num_;

  union {
    struct {
      MsgBuffer *resp_msgbuf_;      ///< User-supplied response buffer
      erpc_cont_func_t cont_func_;  ///< Continuation function for the request
      void *tag_;                   ///< Tag of the request

      /// Number of packets sent. Packets up to (num_tx - 1) have been sent.
      size_t num_tx_;

      /// Number of pkts received. Pkts up to (num_tx - 1) have been received.
      size_t num_rx_;

      /// TSC at which we last sent or retransmitted a packet, or received an
      /// in-order packet for this request
      size_t progress_tsc_;

      size_t cont_etid_;  ///< eRPC thread ID to run the continuation on

      /// Pointers for the intrusive doubly-linked list of active RPCs
      SSlot *prev_, *next_;

      // Fields for congestion control, cold if CC is disabled.

      /// Packet number n is in the wheel (including its ready queue) iff
      /// in_wheel[n % kSessionCredits] is true
      std::array<bool, kSessionCredits> in_wheel_;
      size_t wheel_count_;  ///< Number of packets in the wheel (or ready queue)

      /// Per-packet TX timestamp. Indexed by pkt_num % kSessionCredits.
      std::array<size_t, kSessionCredits> tx_ts_;
    } client_info_;

    struct {
      /// The fake or dynamic request buffer
      MsgBuffer req_msgbuf_;

      // Request metadata saved by the server before calling the request
      // handler. These fields are needed in enqueue_response(), and the request
      // MsgBuffer, which contains these fields, may not be valid at that point.

      /// The request type. This is set to a valid value only while we are
      /// waiting for an enqueue_response(), from a foreground or a background
      /// thread. This property is needed to safely reset sessions, and it is
      /// difficult to establish with other members (e.g., the MsgBuffers).
      uint8_t req_type_;
      ReqFuncType req_func_type_;  ///< The req handler type (e.g., background)

      /// Number of pkts received. Pkts up to (num_rx - 1) have been received.
      size_t num_rx_;

      /// The server remembers the number of packets in the request after
      /// burying the request in enqueue_response().
      size_t sav_num_req_pkts_;
    } server_info_;
  };
};

class SessionEndpoint {
  TransportType transport_type_;
  char hostname_[kMaxHostnameLen];  ///< DNS-resolvable hostname
  uint16_t sm_udp_port_;            ///< Management UDP port
  uint8_t rpc_id_;                  ///< ID of the owner
  uint16_t session_num_;            ///< The session number of this endpoint in its Rpc
  Transport::routing_info_t routing_info_;  ///< Endpoint's routing info
};

class Session {
  const Role role_;                          ///< The role (server/client) of this session endpoint
  const conn_req_uniq_token_t uniq_token_;   ///< A cluster-wide unique token
  SessionState state_;                       ///< The management state of this session endpoint
  SessionEndpoint client_, server_;          ///< Read-only endpoint metadata

  std::array<SSlot, kSessionReqWindow> sslot_arr_;  ///< The session slots

  ///@{ Info saved for faster unconditional access
  Transport::routing_info_t *remote_routing_info_;
  uint16_t local_session_num_;
  uint16_t remote_session_num_;
  ///@}

  /// Information that is required only at the client endpoint
  struct {
    size_t credits_ = kSessionCredits;  ///< Currently available credits

    /// Free session slots. We could use sslot pointers, but indices are useful
    /// in request number calculation.
    FixedVector<size_t, kSessionReqWindow> sslot_free_vec_;

    /// Requests that spill over kSessionReqWindow are queued here
    std::queue<enq_req_args_t> enq_req_backlog_;

    size_t num_re_tx_ = 0;  ///< Number of retransmissions for this session

    // Congestion control
    struct {
      Timely timely_;
      size_t prev_desired_tx_tsc_;  ///< Desired TX timestamp of the last packet
    } cc_;

    size_t sm_req_ts_;  ///< Timestamp of the last session management request
  } client_info_;
};
```






















Pinning this down requires that we stop deferring the worker thread design problem. Let's start here. DPDK RXQ/TXQ setup, unlike the pseudo kernel UDP code above, is complex. Most DPDK programs create and run RXQ/TXQ queue handlers pinned to CPU cores at startup. This is done once. DPDK praxis is application queue event processing is handled by those same lcores through a function *entry point*:

```
int main(int argc, char **argv) {
  std::string prefix;
  std::string device("DPDK_NIC_DEVICE");

  parseCommandLine(argc, argv, &isServer, &prefix);

  Reinvent::Util::Environment env;
  Reinvent::Dpdk::Config config;

  //
  // Initialize the DPDK ENA Nic
  //
  int rc = Reinvent::Dpdk::Init::startEna(device, prefix, &env, &config);
  REINVENT_UTIL_LOG_INFO(config << std::endl);
  if (rc!=0) {
    REINVENT_UTIL_LOG_FATAL_VARGS("Cannot initialize DPDK ENA device rc=%d\n", rc);                                      
    return 1;
  }

  // 
  // Create and run all configured sessions each starting in function pointer
  // 'entryPoint'. Entrypoint is chosen so it is correct for the client or server
  //
  {
    Reinvent::Dpdk::Worker worker(prefix, env, config);
    REINVENT_UTIL_LOG_INFO_VARGS("launching DPDK sessions...\n");
    if ((rc = rte_eal_mp_remote_launch(entryPoint, static_cast<void*>(&worker), CALL_MAIN))!=0) {
      REINVENT_UTIL_LOG_FATAL_VARGS("Cannot launch DPDK cores rc=%d\n", rc);
    } else {
      REINVENT_UTIL_LOG_INFO_VARGS("waiting for DPDK worker threads to stop\n");
      rte_eal_mp_wait_lcore();
    }
  }
```

In this way all configured sessions are pre-launched, and client application code now looks more like:

```
// Client thread explicitly created by Reinvent library for exactly 1 session
int entryPoint(Reinvent::Dpdk::SessionManager *mgr, ....) {
  const std::string key1("test");
  const Dpdk::Endpoint endpoint1 = etcd.serverForKey(key1);
  const std::string key2("abc");
  const Dpdk::Endpoint endpoint2 = etcd.serverForKey(key2);

  // Create a session for endpoint using this box's IP/MAC address as source
  int sessionid = mgr->createSession(endpoint);

  // This RPC goes into a free session slot or is queued
  auto rpc = mgr->get(sessionId, key1, &value);
  assert(0==rc);
  mgr->disconnectSession(sessionid);

  // This RPC goes into a free session slot or is queued
  sessionid = mgr->createSession(endpoint1);
  int rc = mgr->put(sessionId, key2, &value);
  assert(0==rc);
  mgr->disconnectSession(sessionid);
```

whereas on the server side we have one of several variations:

```
// Server thread explicitly created by Reinvent library for exactly 1 session doing work in RXQ dispatch:
int entryPoint(Reinvent::Dpdk::Session *session, ....) {
  while(1) {
   read RXQ 
   do work
   send response on its own TXQ
} 

// Alternative server thread explicitly created by Reinvent library for exactly 1 session doing work in TXQ dispatch:
int entryPoint(Reinvent::Dpdk::Session *session, ....) {
  while(1) {
   read RXQ 
   delegate work to TXQ which does request and sends response on its own TXQ
} 

// Alternative server thread explicitly created by Reinvent library for exactly 1 session delegating requests to worker threads:
int entryPoint(Reinvent::Dpdk::Session *session, ....) {
  while(1) {
   read RXQ 
   find eligible worker, W
   delegate request to W
} 
```

This approach has merits and demerits:

* Good: Initialization of NIC resources: each RXQ/TXQ is created, assigned, pinned to a HW core either doing RXQ, TXQ alone or both
* Good: Through configuration the entry point can be tailored for clients or servers
* Mixed: The client thread doesn't know what RPCs to run; example hard-coded KV code isn't realistic. The client will likely have to get real RPCs from a queue or other point of IPC injection. The Reinvent library can't know what that is. On the hand, that application code is free to read from conventional kernel sockets if desired so specialty DPDK RPC protocol is not forced onto clients at the point RPCs ultimately start 
* Mixed: As written the client code is *blocking*. The client TXQ thread can't talk to `endpoint1` until `endpoint` is done 
* Maybe bad: The number of clients typically far exceeds the number of servers. One time setup of client lcores is possibly too restrictive
* Bad: The issue of mutating session state while its sessions slots are concurrently mutated is unclear
* Bad: The server loops as shown chose whether or not delegation happens and how it happens. We might prefer to the client request to specify this behavior
* Bad: Delegating to a worker thread leaves open how the response is sent back. Which TXQ? Which lcore? Where is the hand off?
* Bad: There's no reference to sessions slots; there's no indication how server RXQ handlers know requests are complete so request processing can start
* Bad: If RXQ/TXQ handling in the client or server is in separate threads, it will complicate packet drop/loss. Resent packets will have to flow from the RXQ only to be handed off to the TXQ 

To fix these problems let's start server side. While the number of clients is unknown, server clusters are pre-configured. There is a fixed amount of hardware that can participate in a session when the server is started. By static routing we've already established how clients choose sessions to participate in, and eRPC session management clarifies how session/RPC state can be pre-coordinated before RPC packets are in-flight. To eliminate pointless copying of possibly large responses, the server can delegate request work in a distinguished TXQ lcore through inter-thread hand-off, or through a worker. A worker, again if we're not copying responses or risking data races, must have its own TXQ. Copying the response from the worker to a TXQ lcore through a second inter-thread hand-off undoes the work we set out to eliminate.

Our options are:

1. Server RXQ lcore, after request complete, and if client **does not** request worker help, delegates to a distinguished TXQ through a SPSC (single producer single consumer) queue. This TXQ lcore does request work and sends response
2. Server RXQ lcore, after request complete, and if client **does not** request worker help, does request work writing response to a TXQ it owns .e.g. one lcore managing a single RXQ/TXQ set
3. Server RXQ lcore, after request complete, and client **does request** worker help, delegates to eligible worker which owns a TXQ through a MPSC (multi producer single consumer) queue. The worker does the request and sends the response on its own TXQ
4. If workers are ever used, the KV data structure **must be MT safe**
5. If option (1) is used the KV structure **must be MT safe**
6. If option (2) is exclusively used the KV structure **need not be MT safe**

Note that this design implicitly constrains the server, in the static routing sense of the destination part of the endpoint, to one RXQ holding any number of client requests from the source endpoint while allowing for multiple TXQs for server responses. Picking our poison is done at build time with `#ifdef/#endif`:

* If clients can defer to workers a delegate request bit must exist in request packet or session creation message
* If clients can defer to workers a delegate request may need an *enumerated field* so the server can pick the right kind of worker. In this scenario, this enumeration will subsume the request bit in the packet or session creation message 
* If workers are possible, the KV structure must be protected by lock
* If option (1) is used, the KV structure must be protected by lock
* Server side configuration chooses between options 1, 2, or 3 consistent with client configuration. That is, the Reinvent library must supply entry points **for all options** at runtime depending on the client and server config

The next issue is server hand-off for workers and/or option (1). The server RXQ lcore has two options: hand-off DPDK's `rte_mbuf` holding the request or extract the request state from packets and hand off a copy. Packet hand-off has its own set of problems with `rte_mbuf` life-cycle. In DPDK RXQ work looks like this:

```
std::vector<rte_mbuf*> mbuf(RX_BURST_CAPACITY);
  
while(!terminate) {
  //
  // Receive up to RX_BURST_CAPACITY packets
  //
  uint16_t rxCount = rte_eth_rx_burst(deviceId, rxqIndex, mbuf.data(), RX_BURST_CAPACITY);

  // do something with mbuf[0..rxCount)
  if (rxCount>0) {
    doSomething(mbuf)
  } else {
    continue;
  }

  // mark mbufs free
  for (unsigned i=0; i<rxCount; ++i) {
    rte_pktmbuf_free(mbuf[i]);
  }
}
```

The `rte_mbuf` pointers placed into `mbuf` come from the mempool associated with `rxq` in the `rte_eth_rx_burst` call. Something has to mark them free or the RXQ will eventually be full. 

Now, for single packet requests, an optimization might include hand-off of the single `rte_mbuf*` to be freed in the TXQ thread once the response is ready presumably on the same HW core. Note, the TXQ thread almost certainly cannot mutate this `rte_mbuf*` to hold the response. Each `rte_mbuf` only holds a fixed amount of data. Since the response payload is typically larger, it can't physically work. 

The possibility of multi-packet requests is more complicated. Because of congestion plus the interleaving of packets from multiple clients, the request would not generally be held in a contiguous set of `rte_mbufs` inside the `mbuf` vector. There's no guarantee that any one call to `rte_eth_rx_burst` holds one complete RPC request either. So handing off the request over mbufs would mean handing over a variable set of pointers. And that's more data to track through more code. It may be better to hand off the request by extracting and copying it out of the packets.

This suggests Reinvent has a more robust set of entry points including:

* Single packet requests handlers for delegation
* Multi-packet request handlers through copy-and-delegate

Then there's the issue of session management. While it's straightforward to equip each server (again in the sense of static routing) with a thread to deal with session connect/disconnect, changing session state could involve data races with response processors:


```
// Session management thread #1 for logical server N
int sessionManagement(Reinvent::Dpdk::SessionManager *mgr, ...) {
  while (!terminate) {
     try w/ timeout to read from UDP port
     if data {
       switch (data.request) {
         case: CONNECT: mgr->makeNewSession(); break;
         case: DISCONNECT: mgr->closeSession(...); break;
         default: ...
       }
     }
   }
}

// DPDK lcore thread #2 for logical server N doing RXQ
int entryPoint(Reinvent::Dpdk::SessionManager *mgr,...) {
  std::vector<rte_mbuf*> mbuf(RX_BURST_CAPACITY);
  while(!terminate) {
    // Receive up to RX_BURST_CAPACITY packets
    uint16_t rxCount = rte_eth_rx_burst(deviceId, rxqIndex, mbuf.data(), RX_BURST_CAPACITY);

    if (unlikely(rxCount==0) {
      continue;
    }

    // do something with mbuf[0..rxCount). each mbuf holds state pre-setup through session manager
    for (unsigned i=0; i<rxCount; ++i) {
      updateRpcState(mbuf[i]);
    }

    // do something with complete requests
    
     
  // mark mbufs free
  for (unsigned i=0; i<rxCount; ++i) {
    rte_pktmbuf_free(mbuf[i]);
  }
}
```

## Exactly-Once-Processing
Whether due to client error or due to congestion error recovery, the server side of RXQ processing must not re-run old RPCs. Errorenously re-running a mutation twice corrupts data:

```
Time t0: request 123: put(k, v)       <-- k = v
Time t1: request 322: get(k)          <-- return v
Time t2: request 432: put(k, v1)      <-- k = v1
Time t3: request 123: put(k, v)       <-- repeat #123 corrupts request #432
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







rpc Appendix B. Handling node failures







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
