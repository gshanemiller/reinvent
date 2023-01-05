# Static Routing Request/Response in DPDK
This section discusses [an extension of the UDP example](https://github.com/rodgarrison/reinvent/blob/main/doc/equinix_mellanox_setup.md)
adding:

* requests and responses
* static routing in which the client transmits a request to a specific receiver on the server
* the server sends a response back to the requesting client

[Find the code here.](https://github.com/rodgarrison/reinvent/tree/main/integration_tests/reinvent_dpdk_static_route_udp)

Beyond how this is architected in DPDK, we'll use this program to asssess UDP reliability in the absence of conjestion
control and correction to find:

* number of UDP packets dropped
* number of UDP packets misordered

To motivate this work consider the [static routing scenario discussed in the packet design doc.](https://github.com/rodgarrison/reinvent/blob/main/doc/packet_design.md#flow-control)
Here a client `C` wants to obtain the value `V` for a key `K`. To do this, the client first determines the IP address
and destination port `S` of the server which handles `K`. `C` send sends a request to `S`. `S` responds to `C` with the
value `V` for `K` or nil if not found. This is static routing in the sense `C` chooses `S`. Determining `S` is not
discussed here but, for example, `C` could consult `etcd` or some other service which knows which `S` handles which keys
`K`.  

# Design Overview

To do KV lookup through static routing [we need flow control](https://github.com/rodgarrison/reinvent/blob/main/doc/packet_design.md#flow-control)
on clients and servers. Flow control on the servers insures requests go to the right DPDK RXQ which does request
in-take. We need flow control on the clients too so the response is sent back to the client `C` that made the original
request. Note that the source UDP destination port is unused/ignored in this work:

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

Suppose `K=test` is assigned to server #1. Further suppose client#0 wants to do a lookup of `test`. There will be
two packets sent as follows:

* Client pid 1234 thread 0 for lcore 0 (e.g. client#0) handling TXQ0 sends a packet from srcIP=192.168.0.2 srcUdpPort=xx
to dstIp=192.168.0.10 dstUdpPort=2 where 'xx' means the value does not matter. Flow control on the server insures
the right RXQ gets the packet
* Server pid 5557 thread 2 for lcore 2 handling RXQ1 receives this packet and does the lookup of `K` given in
the incoming packet. Let `V` be its value
* Server pid 5557 thread 2 for lcore 2 (e.g. server#1) enqueues a message containing the incoming packet plus `V` for
TXQ1 running on thread 3 for lcore 3
* Server pid 5557 thread 3 for lcore 3 dequeues the message and transmits a packet from srcIp=192.168.0.10 srcUdpPort=xx
to dstIp=192.168.0.2 dstUdpPort=1. Note the message contains a copy of the incoming message which can be used to swap
the src/dst IP addresses and ports to make static routing work in reverse. Flow control on the client ensures the right
RXQ gets the packet
* Client pid 1234 RXQ#0 on thread 1 for lcore 1 receives the response packet

Static routing between the clients and servers means request/responses are share nothing also insuring this arrangement works for
any number of client and server boxes.

To efficiently connect RXQs and TXQs --- for example server 0's RXQ0 and TXQ0 or client 1's RXQ1 and TXQ1 --- we impose
two constraints:

* the threads (lcores) handling the NIC queues run on the same CPU core. This applies to both clients and servers for
better cache locality
* for servers we additionally impose a SPSC (single-producer single-consumer) ring buffer to enqueue at the RXQ side
and dequeue at the resp. TXQ side.

To implement all these constraints we use Reinvent's flexible configuration system:

* [Program flow control client side](https://github.com/rodgarrison/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L92)
* [Program flow control server side](https://github.com/rodgarrison/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L226)
* [Program client TXQ assignment](https://github.com/rodgarrison/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L58)
and [client RXQ assignment](https://github.com/rodgarrison/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L72)
so it's staggered by CPU core
* [Program server TXQ assignment](https://github.com/rodgarrison/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L189)
and [server RXQ assignment](https://github.com/rodgarrison/reinvent/blob/main/scripts/reinvent_dpdk_static_route_udp_integration_test#L197)
so it's staggered by CPU core

These configurations are based on the Equinix's `c3.small.x86` offering which has 1 CPU with 8 cores and two hyper-threads
per core. We use all 16 virtual CPUs making 8 clients per client box and 8 servers per server box. That amounts to 8 RXQs
and 8 TXQs per client box and ditto server side assuming clients and servers are both `c3.small.x86`

Putting these configs together we get on the client side:

* Core 0 VCPU #0 for RXQ0 for client 0; core 0 VCPU#1 for TXQ0 for client 0 flow controlled to 192.168.0.2 udpDestPort 1
* Core 1 VCPU #2 for RXQ1 for client 1; core 1 VCPU#3 for TXQ1 for client 1 flow controlled to 192.168.0.2 udpDestPort 2
* etc.

and similarly for the server.

Finally, we need to integrate one SPSC ring buffer per RXQ/TXQ pair on the server. This is accomplished by extending the
class `Reinvent::Dpdk::Worker` with [ExtWorker](https://github.com/rodgarrison/reinvent/blob/main/integration_tests/reinvent_dpdk_static_route_udp/reinvent_dpdk_static_route_udp_test_common.h#L18). This class is used on server boxes only. Upon construction `ExtWorker` inspects the RXQ/TXQ assignments
to create one [SPSC](https://github.com/rodgarrison/reinvent/blob/main/integration_tests/reinvent_dpdk_static_route_udp/reinvent_dpdk_static_route_udp_test_spsc.h)
per TXQ/RXQ pair. TXQs own the buffer and RXQs have a pointer to it which is exposed in the public API.

This design will see one or two minor modifications once some implementation details are considered.

# Design Details

## SPSC Performance
Imposing a SPSC between server side RXQ/TXQs lcores adds latency. The [SPSC implementation](https://github.com/rodgarrison/reinvent/blob/main/integration_tests/reinvent_dpdk_static_route_udp/reinvent_dpdk_static_route_udp_test_spsc.h) was benchmarked on `c3.small.x86`. It can move around 105 Mb/sec or about 10ns per 64-byte message running two 
threads on the same core. The SPSC had a fixed capacity of 5000 data elements each 64-bytes in size. This flow enqueues
one message, and dequeues one message at a time. Batching read/writes is not coded yet. I consider this acceptable loss.

## Packet memory
[As per DPDK](https://github.com/rodgarrison/reinvent/blob/static/doc/packet_design.md#packet-memory-mental-picture)
packet memory is wrapped in a structure called `rte_mbuf`. Assuming the NIC supports `RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE`
(Mellanox NICs can) the DPDK library will automatically free transmitted packets.

In the **client**:

* TXQ side allocate a `rte_mbuf` from its mempool, prepares, and transmits it
* DPDK `RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE` will free it after transmission on the wire
* RXQ side receives one or more packets into a vector of `rte_mbuf*`; this memory comes from the RXQ's mempool
* After each message is processed RXQ side, `rte_pktmbuf_free` is called on it

The **server side** is complicated by SPSC, potential race conditions managing packet memory, and by the different
size requirements of response packets. Let's take these issues in reverse order.

First, keys are limited to 16-64 bytes, but values could be several kilobytes. Therefore, there's no ultimate benefit
in transferring over SPSC the received request packet e.g. each `rte_mbuf*`. It's likely not big enough to hold the
value. That is, it cannot be mutated to hold the value no matter where that's attempted.

Second, transferring each `rte_mbuf*` over SPSC moves the responsibility of freeing the `rte_mbuf*` (allocated from the
RXQ's mempool) to the lcore handling its respective TXQ. Now, we could rely on `RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE` to
free it --- assuming we mutated it to hold the response, which isn't practically possible anyway --- however this
creates a race condition. The RXQ side must be careful to not write new packets from new incoming requests into
`rte_mbuf*` that the TXQ side hasn't processed onto the wire. This is a second reason to avoid moving RXQ `rte_mbuf*`
from the RXQ lcore to its resp. TXQ lcore over SPSC.

Third, we want to avoid double copying the value especially if it's several KB. If the RXQ is tasked with looking up
the value of the key in the request, it must transfer that value to the TXQ by including it in the message enqueued
onto the SPSC. The value would have to be copied yet again when the outgoing response packet is made. Therefore, it's
better to copy the key only, and move the key lookup into the TXQ lcore. It can copy the value once into the outgoing
packet which can be sized better.

Thus we modify the design **server side** as follows:

* Server RXQ side queues receive incoming packets in a `rte_mbuf*` array. The memory comes from the RXQ's mempool
* The server RXQ enqueues the key, plus src/dst IP address and destination UDP ports onto the SPSC approximately
64 bytes total if the key is no larger than 16-bytes
* The server RXQ calls `rte_pktmbuf_free` on each packet after SPSC enqueue
* In the server's resp. TXQ lcore, messages are dequeued from the SPSC one at a time
* Using the key in the message a key-find is performed
* TXQ side allocate a `rte_mbuf` from its mempool of sufficient size to hold the value, prepares, and transmits it
* DPDK `RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE` will free it after transmission on the wire

This allows the schema for requests and responses to differ since their packet memory layout is also different.

## Hot CPUs
Recall DPDK is poll based. [As shown in the simple UDP example](https://github.com/rodgarrison/reinvent/blob/main/doc/equinix_mellanox_setup.md#rss-benchmark-output-1)
the CPU can spin looking for packet work faster than it sometimes comes. This is shown by the `stalledTx, stalledRx`
metrics. SPSC is another hot-spin loop opportunity: queues may be empty (on read) or full (on write). To avoid running
at 100% CPU utilization all the time, exponential backoff micro sleeps should be added. Once an event is finally found,
the sleep time should be reset to the smallest delay.  
