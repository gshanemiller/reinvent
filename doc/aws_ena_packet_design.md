# Introduction

DPDK is a wonderful and performant library with serious support for non-trivial hardware. But DPDK is also by
specialists for specialists. You're not gonna like DPDK until you cross third-base. Especially for first-time
programmers, DPDK documentation is somewhat lacking in a top-down description of concepts. While DPDK provides
[a programer guide](https://core.dpdk.org/doc), this author did not find it particularly helpful. The specific
issue of how to create and TX UDP packets --- a typical first time exercise --- is especially lacking. This guide
attempts to resolve those issues. The reader will need passing familiarity with how/why bare metal boxes divide
hardware (NICs, memory, and CPUs) into NUMA groups, and understand how multi-socket CPUs are futher sub-divided
into cores. 

You may also find these links helpful:

* [UDP client/server example](https://github.com/rodgarrison/reinvent/tree/main/integration_tests/reinvent_dpdk_udp) 
* [Example UDP client/server configuration](https://github.com/rodgarrison/reinvent/blob/main/scripts/reinvent_dpdk_udp_integration_test)
* [Lcore description](https://github.com/rodgarrison/reinvent/blob/main/src/reinvent/dpdk/reinvent_dpdk_lcore.h)
* [How your threads get started](https://github.com/rodgarrison/reinvent/blob/main/src/reinvent/dpdk/reinvent_dpdk_awsworker.h)
* [How to build this library and setup an AWS bare metal with a NIC](https://github.com/rodgarrison/reinvent/blob/main/doc/aws_ena_setup.md)

# H/W Mental Picture
```
 Client Machine (TX Side)                                        Server Machine (RX Side)                                                             
 +--------------------------------------------------+            +--------------------------------------------------+                                 
 |NUMA Node 0                                       |            |NUMA Node 0                                       |                                 
 |+------------------------------------------------+|            |+------------------------------------------------+|                                 
 ||NIC Device 0                                    ||            ||NIC Device 0                                    ||                                 
 ||+----------------------------------------------+||            ||+----------------------------------------------+||                                 
 |||MAC Address  :  23:45:67:89:0a:bc             |||            |||MAC Address  : 01:23:45:67:78:90              |||                                 
 |||IPv4 Address :  73.23.0.11                    |||            |||IPv4 Address : 192.68.0.212                   |||                                 
 |||+--------------------------------------------+|||            |||+--------------------------------------------+|||                                 
 ||||TXQ 10 lcore 11 port 54124    pthread 2332  |||+---   +---->||||RXQ 0 lcore 4 port 43222       pthread 1    ||||                                 
 |||+--------------------------------------------+|||   \ /      |||+--------------------------------------------+|||                                 
 |||+--------------------------------------------+|||    \       |||+--------------------------------------------+|||                                 
 ||||TXQ 11 lcore 12 port 54125    pthread 4821  |||+---+-+----->||||RXQ 1 lcore 5 port 43223       pthread 1392 ||||                                 
 |||+--------------------------------------------+|||  /  |      |||+--------------------------------------------+|||                                 
 |||+--------------------------------------------+||| /   |      |||+--------------------------------------------+|||                                 
 ||||TXQ 12 lcore 13 port 54189    pthread 232   |||+-    +----->||||RXQ 2 lcore 6 port 43224       pthread 7372 ||||                                 
 |||+--------------------------------------------+|||            |||+--------------------------------------------+|||                                 
 |||                    |                         |||     ^      |||                    |                         |||                                 
 |||                    |                         |||     |      |||                    |                         |||                                 
 |||                    V                         |||     |      |||                    V                         |||                                 
 ||+----------------------------------------------+||    RSS     ||+----------------------------------------------+||                                 
 ||                     |                          || discussed  ||                     |                          ||                                 
 ||                     |                          ||   below    ||                     |                          ||                                 
 ||                     V                          ||            ||                     V                          ||                                 
 |+------------------------------------------------+|            |+------------------------------------------------+|                                 
 |                      |                           |            |                      |                           |                                 
 |                      |                           |            |                      |                           |                                 
 |                      V                           |            |                      V                           |                                 
 +--------------------------------------------------+            +--------------------------------------------------+                                 

Figure 1: One way communication
-------------------------------
NIC with MAC address 01:23:45:67:78:90 is attached to NUMA node 0 of a server machine. Server receives only.
NIC with MAC address 23:45:67:89:0a:bc is attached to NUMA node 0 of a client machine. Client transmits only.

NICs have both RXQ (recceive queues) and TXQ (transmit queues). To start simple, this diagram illustrates the
client machine only transmitting (so no RXQs) while the server machine only receives (so no TXQs). Production
configurations almost certainly do both in the same NIC.

Each queue has a dedicated lcore e.g. pthread pinned to a CPU HW core to handle the roles and responsibilities
of that queue.

``` 

Consider simple one-way communication where a client machine sends packets to a server machine. The server does not
respond. In consequence the client does not receive and the server does not transmit. For brevity I omit other NUMA
nodes, their NICs, each with its queues which can co-exist. This diagram illustrates one NIC attached to NUMA node
zero. Other NUMA nodes, its CPU cores, and memory is not shown.

Even with those omissions many important aspects shine through. First, **lcore here refers to a pthread** which has
been pinned to a physical core (aka vCPU). DPDK `lcore` tends to draw the reader's focus solely onto a CPU core
ommitting the pthread. Applications can only create threads; H/W is fixed. Threads may be pinned or not to a CPU core.
DPDK jargon **lcore** is short hand for an application created pthread pinned to a CPU core.

Second, the existence of queues arise out of the old story in which CPU frequencies plateaued. CPU vendors, by way
of compromise, ship multi-core CPUs. So in order to perform high speed I/O without stalling the NIC --- and thus 
defeating performance because its H/W buffers are full awaiting CPU handoff --- H/W manufactures created NICs with
multiple queues. Each queue works, for all intents and purposes, share-nothing. This way each queue can be pinned to
one core. Now we're back to scalable performance. NIC queues naturally divide into RX queues (RXQ) and TX queues
(TXQ) flavors.

Third, modern machines are NUMA (Non Uniform Machine Architecture). Programmers incur severe performance penalties
if a H/W resource on NUMA node A --- which includes CPU cores, DRAM, NICs among others --- access H/W resources on a
NUMA node `B<>A`. Therefore one should assume client lcores 11, 12, 13 in the diagram are on NUMA node 0 just like
the NIC they serve. Ditto for server lcores 0,1,2.

Next, NICs have a MAC address. NICs can also be assigned a IPv4 address. These identifiers, plus ports are transmitted
as part of a IPV4 UDP packet. Every client lcore TXQ transmits on its own queue. RSS server side delivers the transmitted
packets to a specific RXQ server side. And one server side lcore processes incoming packets for its queue.

Then, note each NIC has a maximum number of RXQ and TXQs in H/W. The programmer can elect to use as many or as few
of these queues as desired. But since receiving or transmitting packets on a given NIC/queue requires a lcore to do
that work, and lcores are by definition pthreads pinned to a CPU core, the programmer must determine which lcore
handles which RX or TX queue. This is done twice: once on the client for its NIC/queues, and again on the server.

The considerations here are similar to mathematics where one designates a function `f: X->Y` as injective, surjective,
or bijective. The focus is less on what `X, Y` are --- in our scenario X is the set of RXQ/TXQs for a single NIC and Y
is the set of CPU cores on the same machine --- but rather on how `f` maps.

For maximum concurrency, `f` should be injective. That is, each specific RXQ or TXQ should get its own lcore, which is
what the diagram illustrates: `f` maps client TXQ 10 to lcore 11. `f` maps no other queue to lcore 11. And same on the
server side. DPDK certainly allows multiple queues to run on the same lcore; programmers just need to accept the
trade-offs that occur when sharing a CPU core. DPDK methodology uses the `--lcores` argument passed to `rte_eal_init()`
to create and pin lcores as per the mapping `f` provides.

For example in the AWS `c5n bare metal` case where ENA NICs have 32 RXQs and 32 TXQs, it is not possible to use all RX
and TX queues with an injective `f`. A `c5n` instance does not have `32+32=64` CPU cores all on the same NUMA node as
the NIC itself. So some CPU cores must do double-duty. Or, optionally, programmers must not use all queues.

The Reinvent library approach is to specify the number of lcores to be run for RXQ and TXQ. Each lcore is assigned its
own RXQ (TXQ). lcores may share a HW CORE if the allocation policy is `SHARED` or never share a HW core if 'DISTINCT'.

# RSS (Receive Side Scaling)
RSS is NIC behavior that, based on a packet content hash, determines which RXQ will take delivery of each packet.
Without this facility packet RX cannot be load balanced across all RXQs and their managing lcores which will eventually
lead to dropped packets or back pressure on TX queues. 

Routing packets is, again at very high level of abstraction, occurs in two phases:

* Switches/routers connected to the NICs route packets based on the MAC/IPV4 addresss in the packet headers
* Once the receiving NIC gets the packets, RSS is used to pick and deliver packets to specific RXQs 

The AWS ENA NIC supports RSS. It has to be enabled by setting the RXQ mask to 1 (RTE_ETH_MQ_RX_RSS_FLAG). RSS performs
a hash of packet contents then mods the result to the number of running RXQs. As long as the TX side varies the packet
contents enough so the hash changes enough, RSS will load balance. Some NICs support programming in a hash which DPDK
does support. However, AWS ENA NICs do not support this functionality. The default RSS hash can be seen like this:

```
$ ethtool -x ens6
RX flow hash indirection table for ens6 with 32 RX ring(s):
    0:      0     1     2     3     4     5     6     7
    8:      8     9    10    11    12    13    14    15
   16:     16    17    18    19    20    21    22    23
   24:     24    25    26    27    28    29    30    31
   32:      0     1     2     3     4     5     6     7
   40:      8     9    10    11    12    13    14    15
   48:     16    17    18    19    20    21    22    23
   56:     24    25    26    27    28    29    30    31
   64:      0     1     2     3     4     5     6     7
   72:      8     9    10    11    12    13    14    15
   80:     16    17    18    19    20    21    22    23
   88:     24    25    26    27    28    29    30    31
   96:      0     1     2     3     4     5     6     7
  104:      8     9    10    11    12    13    14    15
  112:     16    17    18    19    20    21    22    23
  120:     24    25    26    27    28    29    30    31
RSS hash key:
86:e8:45:9a:35:c5:9f:b3:2e:be:89:73:fc:db:ea:29:a6:a5:e8:d5:fe:dd:43:da:2a:89:76:52:ae:b8:65:f7:99:9d:41:22:24:4e:b2:01
RSS hash function:
    toeplitz: on
    xor: off
    crc32: off
```

If RTE_ETH_MQ_RX_RSS_FLAG is not set, all packets seem to go to one RXQ.

# Packet Memory Mental Picture
Read this diagram with [DPDK 10.1 diagram](https://doc.dpdk.org/guides/prog_guide/mbuf_lib.html#figure-mbuf1):

```
+-------------------------------+             +-----------+                                                                           
| Memzone "exampleMemzone"      |             | Hugepage  |                                                                           
|                               |<------------+ TLB mount |                                                                                         
| from Hugepage memory (mnt)    |             | for NUMA  |                                                                           
| for NUMA node 0               |             | node 0    |               continuation mbuf                                           
+-------------------------------+             +-----------+    +-------------------------------------+                                
       |                                                       |                                     V                                
       |           +------------------------------+          +--------------------------------+     +--------------------------------+
       |           |Mempool "RX mempool #1"       |          |mbuf  |        |       |        |     |mbuf  |        |       |        |
       +---------->|For use with RXQ 1            |--------->|struct|headroom|payload|tailroom|     |struct|headroom|payload|tailroom|
       |           |Cached/pinned for RXQ1's lcore|          |      |        |       |        |     |      |        |       |        |
       |           +------------------------------+          +--------------------------------+     +--------------------------------+
       |                                                                                              \                               
       |           +------------------------------+                                                    \                              
       |           |Mempool "TX mempool #23"      |                                                     \                             
       +---------->|For use with TXQ 23           |---------> ... mbufs ...                              -> null                      
       |           |Cached/pinned for TXQ23's core|                                                                                   
       |           +------------------------------+                                                                                   
       |
       |           +------------------------------+                                                   
       |           |Mempool "shared NUMA 0"       |                                                     
       +---------->|For use with any lcore on NUMA|---------> ... mbufs ...                             
       |           |node 0                        |                                                                                   
       |           +------------------------------+                                                                                   
       | 
       |
       +----------> ... mempools ....
       |
       |
       V

Figure 2: DPDK TLB, memzone, mempool, mbuf relationships
--------------------------------------------------------
```

The [setup script](https://github.com/rodgarrison/reinvent/blob/main/scripts/setup) reserves huge TLB memory. TLB
memory helps to avoid TLB misses which is important for user-space NIC work. All packets run through main memory,
and there will be zillions of these. TLB mounts are praxis in this space, and it's easy to do. Creating the mount
is done once after machine (re)start.

Once a DPDK application runs, a memory zone is created with a textual description allocated off the hugepage mount
on its NUMA node. There will be one memzone per NUMA node used. NUMA means non-uniform machine architecture so keeping
HW access and usage segregated across NUMA nodes is a performance must-have. Mempools are created from a memzone, and
all DPDK `mbufs` are allocated from mempools. Mempool allocation give a NUMA node (not a memzone pointer) from which
the related memzone is determined. Individual DPDK `mbufs` hold packet data.

DPDK mempool creation comes in two flavors: `rte_pktmbuf_pool_create` and `rte_mempool_create`. The first method is
widely used:

```
struct rte_mempool* rte_pktmbuf_pool_create(
    const char *    name,                         // The name of the mbuf pool.
    unsigned        n,                            // The number of elements in the mbuf pool. The optimum size (in
                                                  // terms of memory usage) for a mempool is when n is a power of two
                                                  // minus one: n = (2^q - 1).
    unsigned        cache_size,                   // Size of the per-core object cache. See rte_mempool_create() for
                                                  // details.
    uint16_t        priv_size,                    // Size of application private [sic] are between the rte_mbuf
                                                  // structure and the data buffer. This value must be aligned to
                                                  // RTE_MBUF_PRIV_ALIGN
    uint16_t        data_room_size,               // Size of data buffer in each mbuf including RTE_PKTMBUF_HEADROOM
    int             socket_id                     // The socket identifier where the memory should be allocated. The
                                                  // value can be SOCKET_ID_ANY if there is no NUMA constraint for the
                                                  // reserved zone.
);	
```

Each RXQ is required to take a mempool pointer at the time the RXQ is created. Two or more distinct RXQs can
then take the same mempool pointer or not. TXQs work the same way except that TXQs do not take a mempool pointer at
creation time. TXQs get around to mempool choice after the NIC is initialized but before a TXQ enters its main
processing loop. TXQs need mbufs to transmit packets. Thus the same work and principles apply for TXQ like RXQ; it's
just deferred to post-NIC initialization.

`rte_pktmbuf_pool_create` is typically used to create one mempool per RXQ/TXQ e.g. `RX mempool #1` only for RXQ1 or
`TX mempool #23` only for TXQ23 in the diagram. Other code may create one mempool `shared NUMA 0` and pass that mempool
pointer on to all RXQs and TXQs. Programmers explicitly determine which RXQ/TXQ gets which mempool.

The purpose of per lcore-caching is to optimize the case when several queues share a mempool by optimizing concurrent
access to that mempool when mbufs are allocated and freed. Now when any two CPU cores on the same NUMA node access
memory not in the CPU's cache, the performance is the same. But that isn't the end of the story. If the memory is part
of shared structure like a mempool, it'll become necessary to deal with concurrent access to that shared structure.
`cache_size>0` is an optimization here. To see the ramifications of lcore-caching:

* Know the number of lcores on the NUMA node the memory pool (e.g. via memzone) is for. More lcores means each lcore
gets less memory and vice-versa. See `--lcores` DPDK arguement to `rte_eal_init` which holds the lcore mapping.
* Know the number of queues or lcores running on an a HW-core. `cache_size` **is per HW core not lcore**. Suppose I
have 10 lcores each running 1 queue/HW-core, and I also have one H/W core running 8 lcores. The memory apportionment
**is per core**, say n. One the H/W core running 8 lcores, all 8 lcores will fight for the same amount of memory. 
Everywhere else each lcore, which was running on its own HW core, has its own `n` bytes of uncontended memory.
* When initializing a RXQ or finalizing TXQ setup with a caching pool, you must actually provide the caching pool to
the queue. Otherwise it's dead memory
* While the previous point is obvious, there yet remain an opportunity to not assign the caching pool to all queues.
If you have `n` queues on a NUMA node but only provide it to `m<<n` of those queues `m` of those queues will benefit
from the caching pool by construction. But `n-m` queues can't benefit at all because it's not using it. **You will
have essentially lost the memory DPDK apportioned** for those `n-m` queues. It's again dead.
* Caching pools are best and naturally deployed when all queues use it although this is not a hard requirement.

[DPDK 10.1](https://doc.dpdk.org/guides/prog_guide/mbuf_lib.html#figure-mbuf1) includes this important point:

```
For the storage of the packet data (including protocol headers), two approaches were considered:

1. Embed metadata within a single memory buffer the structure followed by a fixed size area for the packet data.
2. Use separate memory buffers for the metadata structure and for the packet data.

The advantage of the first method is that it only needs one operation to allocate/free the whole memory representation
of a packet. On the other hand, the second method is more flexible and allows the complete separation of the allocation
of metadata structures from the allocation of packet data buffers.

The first method was chosen for the DPDK. The metadata contains control information such as message type, length,
offset to the start of the data and a pointer for additional mbuf structures allowing buffer chaining.
```

Because of the decision to go with approach (1) application packet data is added inline (inside of) the mbufs provided
by the mempool. The portion of a mbuf reserved for application data is called the **data_room_size**. Accessing this
portion of the mbuf is done old-C-style. A DPDK macro `rte_pktmbuf_mtod_offset` is provided to give a pointer inside 
the data room portion of a mbuf cast to a caller provided type. As a result DPDK does not provide ready-made APIs
to set UDP, IPV4, TCP etc. data inside an mbuf. Programmers must know ahead of time the bytes, sequence, and alignment
of the data to go into the data room. In turn that means setting `data_room_size` in the mempool creation call is of
utmost importance. You will not be able to get more packet memory later.

# Mempools and Sizing

With structural issues explained, I now turn to memory sizing. A mempool is a fixed-size memory allocator that, once
created, can efficiently hand out or take back mbuf objects. A [mbuf](https://doc.dpdk.org/api/structrte__mbuf.html)
holds one packet or, in other cases, the continuation of an earlier mbuf when payload sizes exceed the NIC's MTU. In
the `rte_pktmbuf_pool_create()` case, the best way to assess mempool sizing is to work backwards from the mbufs it will 
eventually provide. You do not tell DPDK the absolute memory size for a fixed chunk of memory like C `malloc()`; you
parameterize your request by telling DPDK how you want your mbufs configured. DPDK works backward to compute the final
ask. Besides `n` --- the number of mbufs in the pool --- the other critical parameter is `data_room_size` which 
reserves memory the mbuf to hold application created packets.

Once the you know the number `n` of mbufs you want, and you know the mbuf private, data room sizes you can to a
first approximation compute the mempool ask as `n * (sizeof(rte_mbuf) + priv_data + data_room_size)` bytes. Here's
the API again for easy reference:

```
struct rte_mempool* rte_pktmbuf_pool_create(
    const char *    name,                         // The name of the mbuf pool.
    unsigned        n,                            // The number of elements in the mbuf pool. The optimum size (in
                                                  // terms of memory usage) for a mempool is when n is a power of two
                                                  // minus one: n = (2^q-1).
    unsigned        cache_size,                   // Size of the PER-CORE OBJECT CACHE. See rte_mempool_create() for
                                                  // details.      [Emphasis mine]
    uint16_t        priv_size,                    // Size of application private [sic] are between the rte_mbuf
                                                  // structure and the data buffer. This value must be aligned to
                                                  // RTE_MBUF_PRIV_ALIGN
    uint16_t        data_room_size,               // Size of data buffer in each mbuf including RTE_PKTMBUF_HEADROOM
    int             socket_id                     // The socket identifier where the memory should be allocated. The
                                                  // value can be SOCKET_ID_ANY if there is no NUMA constraint for the
                                                  // reserved zone.
);	
```

`priv_size` can be probably be set 0 for most callers. But if preparing a TX packet or processing a RX packet becomes
involved, it may be handy to imbue each mbuf with opaque data (pointer, struture) which provides helper context. In 
that case set `priv_size` equal to the `sizeof()` of that data. The `data_room_size`, however, is subject to several
important considerations:
 
* should account for the maximum payload size in bytes of any packet the mbuf most hold
* should be at least `rte_config.h:#define RTE_PKTMBUF_HEADROOM 128`. Any valid `data_room_size` must be at least
128 bytes. I believe, but am not sure, DPDK uses the headroom for its internal management.
* should be at least `min_[rx|tx]_bufsize(NIC device)` bytes in addition to headroom. For AWS ENA NICs this is 64 bytes.
NICs may impose their own other requirements.

With these pieces DPDK will create a memory pool on the specified NUMA mode taking its direction with per mbuf sizing
placing locating the mbufs and most mbuf fields on cache friendly memory boudaries. Callers [may want to inspect the structure returned](https://doc.dpdk.org/api/structrte__mempool.html) e.g. fields `size` gives the maximum pool size, and `mz` give the memzone the memzone the zone used to make the pool.

Because Reinvent is DPDK userspace library, you get saddled with creating the full packet in addition to your
application payload. As such **the data room size must include space for various ethernet/IP headers**. The packet
headers must be of the right size, right sequence, right byte order, and right memory boundaries to work correctly. 

Consider a program that wants to transmit application payloads over UDP. UDP is a IPV4 packet and therefore comes with
three headers schematically like this:

```
 +---> rte_pktmbuf_mtod_offset(mbuf, rte_ether_hdr*, 0) 
/
+---------------+--------------+-------------+---------------------+
| rte_ether_hdr | rte_ipv4_hdr | rte_udp_hdr | application payload |
+---------------+--------------+-------------+---------------------+

Figure 3: rte_pktmbuf_mtod_offset provides a pointer to the start of 
          data room inside an individual mbuf. This example gives a
          pointer cast to rte_ether_hdr* at offset 0 in the dataroom.
          Repeat rte_pktmbuf_mtod_offset with different offsets to 
          move down the dataroom buffer.
```

Because UDP/TCP packets are so common, DPDK provides ready made helper structures:

* `rte_ether_hdr` [ethernet header](https://doc.dpdk.org/api/structrte__ether__hdr.html)
* `rte_ipv4_hdr` [IPV4 header](https://doc.dpdk.org/api/rte__ip_8h.html)
* `rte_udp_hdr` [IPV4 UDP header](https://doc.dpdk.org/api/structrte__udp__hdr.html)

These structures are important because it'll absolve you of having to figure out the fields, their sizes, and
alignment in memory.

Here are three payloads applications might want to send over a NIC:

```
// x86_64 build

struct MicroPayload {
  char cmd, arg;        // compact, right?
};
// sizeof(MicroPayload)==2

struct MediumPayload {
  int messageType;      // perhaps an enumerated value
  int cmd;              // ditto
  int args[16];         // a bunch of args
  int epsilon;          // perhaps for a calculation
  char callerId[32];    // to connect server errors to client callers
  unsigned sequenceNo;  // some sort of monotonic counter
};
// sizeof(MediumPayload)==112

struct HugePayload {
  int messageType;      // perhaps an enumerated value
  int cmd;              // ditto
  char opaque[4096];    // maybe a protobuf
  unsigned sequenceNo;  // some sort of monotonic counter
};
// sizeof(HugePayload)==4108
```

Let's start simple. Suppose an application only uses one of these payloads everywhere with no private data. Here's
the sizing:

```
 rte_pktmbuf_pool_create() 
+-------------------------+------------------------------+-----------+----------------------------------------------+
| Type used               | Mimimum data_room_size       | priv_size | Comment                                      |
+-------------------------+------------------------------+-----------+----------------------------------------------+
| MicroPayload            | RTE_PKTMBUF_HEADROOM+64      | 0         | RTE_PKTMBUF_HEADROOM+64 represents the min   |
|                         |                              |           | dataroom size imposed by DPDK, AWS ENA NIC.  |
| sizeof(MicroPayload)    |                              |           | H + payload size fits in these 64 bytes      |
|    2 bytes              |                              |           |                                              |
+-------------------------+------------------------------+-----------+----------------------------------------------+
| MediumPayload           | RTE_PKTMBUF_HEADROOM+154     | 0         | e.g. RTE_PKTMBUF_HEADROOM + 112 + H          |
|                         |                              |           |                                              |
| sizeof(MediumPayload)   |                              |           |                                              |
|   112 bytes             |                              |           |                                              |
+-------------------------+------------------------------+-----------+----------------------------------------------+
| HugePayload             | RTE_PKTMBUF_HEADROOM+4150    | 0         | e.g. RTE_PKTMBUF_HEADROOM + 4108 + H         |
|                         |                              |           | Note however this may not fit into the MTU   |
| sizeof(HugePayload)     |                              |           | which sets the maximum packet sizes. So this |
|   4108 bytes            |                              |           | payload may have to split across multiple    |
|                         |                              |           | mbufs                                        |
+-------------------------+------------------------------+-----------+----------------------------------------------+

Diagram 4:
---------

Ref:    E sizeof(rte_ether_hdr) = 14 bytes (x86 64-bit build)
        I sizeof(rte_ipv4_hdr)  = 20 bytes (x86 64-bit build)
        U sizeof(rte_udp_hdr)   = 8 bytes  (x86 64-bit build)
        H = E+I+U               = 42 bytes in headers
```

You will need sligtly higher sizes if **you add an alignment pad after the end of the UDP header** before your payload
begins in the mbuf. 

If your application mixes-n-matches transmissions of different payloads you have several options:

* You know ahead of time that a specific TXQ only sends payloads of one type. So allocate a mempool for it from the
table. Different TXQs sending different payloads have their own mempools with the right data room size
* Allocate all mempools using the maximum data room size. But at packet preparation, use only as many of the bytes
in the data room as that packet needs. 

As the section on packet initialization will make clear, a packet is not required to send the full data room size
over the wire.

To finish sizing we must deal with `n`, the number of elements in the mempool. This is not exactly straightforward
either. **Each queue** requires access to a number of mbufs that,

* derive in part from the queue's ring size. The per queue ring size refers to the mbufs (memory) the NIC H/W writes
into upon packet reception (RXQs), or send to the NIC H/W (TXQ) to write onto the wire for transmission. Those mbufs
come from the queue's mempool. Without this facility the NIC will stall because H/W has nowhere to offload packets (RX)
or get data onto the wire (TX). DPDK reserves mbufs per queue based on the right size. You must allocate at least that
many mbufs.
* DPDK sets a minimum of 128 mbufs/queue but no more than 16384. NIC particulars may modify these limits up/down but
must resolve inside DPDK's limits
* Optional non-negative number of additional TXQ mbufs/queue the application prepares while the NIC H/W is flushing its
TX ring so the NIC H/W is not starved for new work 
* Optional non-negative number of additional RXQ mbufs/queue the application can use to process older, received packets
while the NIC H/W reads new data into available mbufs on its ring
* One is always bounded from above by the amount of free memory

The optional numbers must be considered carefully: you can't reuse older, processed RX mbufs or prepare new mbufs for
TX unless those mbufs are distinct from those in-use by the ring. Therefore, you may want to use `n` bigger than the
ring size per queue, focus on eliminating overhead in mbuf processing so mbufs can be reused sooner, or use more queues
over more cores if share nothing. Another option is receive delegation: once a packet is received, you delegate it with
optional copy via a helper mempool o the original mbuf gets back into the queues ring.

Most programmers most of the time will choose `n` based on the ring size for the queue. Optional, additional mbufs
hopefully are not required if per-mbuf-processing overhead is low.

# Byte Order
It's crucial to prepare packet headers in the right byte order. DPDK wants all packet header data in big-endian format.
UDP IPV4 packet data includes many things,

* MAC addresses
* IP addresses
* Ports
* Packet sizes
* Other

The Reinvent library accomplishes this in three ways:

* Runs DPDK helper macros `rte_cpu_to_be_16, rte_cpu_to_be_32, rte_cpu_to_be_64`
* Runs `rte_ether_unformat_addr` to convert a textual MAC address into packet ready binary format. See [UDPRoute::convertSrcMac](https://github.com/rodgarrison/reinvent/blob/main/src/reinvent/dpdk/reinvent_dpdk_udproute.h#L208)
* A combination of above, for example, [IPV4 addresses in UDPRoute::convertSrcIp](https://github.com/rodgarrison/reinvent/blob/main/src/reinvent/dpdk/reinvent_dpdk_udproute.h#L228)

Programmers **do not** need to prepare the application payload in a defined endianess or byte order if it's guaranteed
that senders and receivers are binary compatible.

# Creating a UDP Packet and Transmitting it
[For a buildable example demonstrating UDP RX/TX see](https://github.com/rodgarrison/reinvent/blob/main/integration_tests/reinvent_dpdk_udp/reinvent_dpdk_udp_integration_test.cpp#L102) in the function `clientMainLoop`. 

**Note:** the following code mention UDP ports. Those who have done UNIX socket programming know ports are an integral
part to point-to-point communication. However, in the DPDK context, **you do not create ports**. These ports are pseudo
ports. Port numbers are provided in the UDP packet header for its side effects only:

* To fulfill a legal UDP packet header
* RSS may use this information for RXQ selection

Consider a program that wants to transmit the following payload over UDP IPV4:

```
struct TxMessage {
  int lcoreId;            // lcoreId which sent this message
  int txqId;              // txqId which sent this message
  int sequenceNumber;     // txqId's sequence number
};
```

DPDK supports transmitting packets in bunches called bursting. Let's consider a burst of eight. The first job is to
allocate eight mbufs (cira line 310 in example code):

```
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
```

[The buildable example](https://github.com/rodgarrison/reinvent/blob/main/integration_tests/reinvent_dpdk_udp/reinvent_dpdk_udp_integration_test.cpp#L102) comes with a default route for each TXQ. Therefore we can prepare some of the information ahead of time before we enter a loop that prepares each packet. Unless otherwise noted these variables are in DPDK big-endian format correct for packet construction:

```
  // rte_cpu_to_be_16 not run here because this doesn't go into packets
  const int burstCapacity = static_cast<int>(mbuf.size());

  const uint16_t ethFlow = rte_cpu_to_be_16(static_cast<uint16_t>(config->awsEnaConfig().txq()[txqIndex].defaultFlow()));

  // rte_cpu_to_be_16 not run here because this doesn't go into packets
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
  // this does not go into a packet
  const uint64_t txOffloadFlags = static_cast<uint64_t>(config->awsEnaConfig().txOffloadMask());

  //
  // Total all-in packet size sent
  //
  // this does not go into a packet
  const int packetSize = sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr)+sizeof(TxMessage);

  //
  // Size relative to IP4V header
  //
  const uint16_t ip4Size = rte_cpu_to_be_16(static_cast<uint16_t>(sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr)+sizeof(TxMessage)));

  //
  // Size relative to UDP header
  //
  const uint16_t udpSize = rte_cpu_to_be_16(static_cast<uint16_t>(sizeof(rte_udp_hdr)+sizeof(TxMessage)));

```

We now run a loop `for(int i=0; i<burstCapacity; ++i)` which, for each packet, does five steps:

1. Prepare the ethernet header
2. Prepare the IPV4 header
3. Prepare the UDP header
4. Prepare the payload
5. Initialize meta fields in the mbuf structure

And once that's done the final step is transmission:

6. Burst write the packets onto the wire

Recall that the order of the packet headers and payload is:

```
  // Within a given mbuf:
  //
  //  +---> rte_pktmbuf_mtod_offset(mbuf, rte_ether_hdr*, 0) 
  // /
  // +---------------+--------------+-------------+-----------+
  // | rte_ether_hdr | rte_ipv4_hdr | rte_udp_hdr | TxMessage |   
  // +---------------+--------------+-------------+-----------+
```

## Step 1: Ethernet header preparation

Get a pointer to the start of the ith mbuf's dataroom at offset zero. This is where the ethernet header goes then
set its fields:

```
struct rte_ether_hdr *ethHdr = rte_pktmbuf_mtod_offset(mbuf[i], rte_ether_hdr*, 0);
ethHdr->src_addr = srcMac;
ethHdr->dst_addr = dstMac;
ethHdr->ether_type = ethFlow;
```

## Step 2: IPV4 Header preparation

The IPV4 header starts on the first byte immediately following the end of the ethernet header:

```
rte_ipv4_hdr *ip4Hdr = rte_pktmbuf_mtod_offset(mbuf[i], rte_ipv4_hdr*, sizeof(rte_ether_hdr));
assert(ip4Hdr);
ip4Hdr->ihl = 0x5;                    // default header is 5 words long
ip4Hdr->version = 0x04;               // IPV4
ip4Hdr->type_of_service = 0;
ip4Hdr->total_length = ip4Size;       // size of IPV4 header and everything that follows
ip4Hdr->packet_id = 0;
ip4Hdr->fragment_offset = 0;
ip4Hdr->time_to_live = IPDEFTTL;      // default 64
ip4Hdr->next_proto_id = IPPROTO_UDP;  // UDP packet follows
ip4Hdr->hdr_checksum = 0;             // Checksum will be offloaded to NIC; see below
ip4Hdr->src_addr = srcIp;
ip4Hdr->dst_addr = dstIp;
```

## Step 3: UDP Header preparation

The UDP header starts on the first byte immediately following the end of the IPV4 header:

```
rte_udp_hdr *udpHdr = rte_pktmbuf_mtod_offset(mbuf[i], rte_udp_hdr*,
  sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr));
assert(udpHdr);
udpHdr->src_port = srcPort;
udpHdr->dst_port = dstPort;
udpHdr->dgram_len = udpSize;          // size of the UDP header and everything that follows
udpHdr->dgram_cksum = 0;              // Checksum will be offloaded to NIC; see below
```

## Step 4: Payload preparation

The application payload starts on the first byte following the end of the UDP header:

```
TxMessage *payload = rte_pktmbuf_mtod_offset(mbuf[i], TxMessage*,
  sizeof(rte_ether_hdr)+sizeof(rte_ipv4_hdr)+sizeof(rte_udp_hdr));
assert(payload);
payload->lcoreId = id;
payload->txqId = txqIndex;
payload->sequenceNumber = sequence++;
```

## Step 5: 

These assignments update the meta data for DPDK:

```
mbuf[i]->nb_segs = 1;                                                             // Packet inside this single mbuf
mbuf[i]->pkt_len = packetSize;                                                    // Total all in packet size
mbuf[i]->data_len = packetSize;                                                   // Total all in packet size
mbuf[i]->ol_flags = (RTE_MBUF_F_TX_IPV4|RTE_MBUF_F_TX_IP_CKSUM);                  // Tells NIC to do IPV4, UDP chksums
mbuf[i]->l2_len = sizeof(rte_ether_hdr);                                          // Size of eth header
mbuf[i]->l3_len = sizeof(rte_ipv4_hdr);                                           // Size of ipv4 header
mbuf[i]->l4_len = sizeof(rte_udp_hdr);                                            // Size of udp header
mbuf[i]->packet_type = (RTE_PTYPE_L2_ETHER|RTE_PTYPE_L3_IPV4|RTE_PTYPE_L4_UDP);   // Identifies packet sent
```

## Step 6: Transmit the packets e.g. write on the wire

```
uint16_t txCount = rte_eth_tx_burst(deviceId, txqIndex, mbuf.data(), burstCapacity);
REINVENT_UTIL_LOG_DEBUG("sent " << txCount << " packets" << std::endl);
```

# Receiving UDP Packet(s)

RX is very simple. See [serverMainLoop](https://github.com/rodgarrison/reinvent/blob/main/integration_tests/reinvent_dpdk_udp/reinvent_dpdk_udp_integration_test.cpp#L270).

# On Queue Thresholds

Although AWS does not yet provide a data sheet on its NICs, my research suggests most NIC H/W process packets in part
parameterized through several threshhold values. As a result DPDK provides structures to hold these values so they can
be configured into the NIC: The [RXQ configs are here](https://doc.dpdk.org/api/structrte__eth__rxconf.html#ab7d7dbc33adbb1740718c9a841555dce);
the [TXQ configs are here](https://doc.dpdk.org/api/structrte__eth__txconf.html#ab7d7dbc33adbb1740718c9a841555dce).

The DPDK documentation for these threshhold values is underwhelming. And since AWS does not provide a datasheet, I cannot
be specific. I can, however, lift the intent of these fields based on [Intel's datasheet](https://interfacemasters.com/pdf/82599_datasheet.pdf).
See page 290, 494.

TXQ Perspective:
* **PTHRESH**: Pre-Fetch Threshold. Controls when a prefetch of descriptors is considered. This threshold refers to the
number of valid, unprocessed transmit descriptors the 82599 has in its on-chip buffer. If this number drops below PTHRESH,
the algorithm considers pre-fetching descriptors from host memory. However, this fetch does not happen unless there are at least
HTHRESH valid descriptors in host memory to fetch. Note: HTHRESH should be given a non-zero value each time PTHRESH is used.
* **HTHRESH**: Host Threshold
* **WTHRESH**: Write-Back Threshold. Controls the write-back of processed transmit descriptors. This threshold refers to the
number of transmit descriptors in the on-chip buffer that are ready to be written back to host memory. In the absence of
external events (explicit flushes), the write-back occurs only after at least WTHRESH descriptors are available for write-back.
Note: Since the default value for write-back threshold is 0b, descriptors are normally written back as soon as they are processed.
WTHRESH must be written to a non-zero value to take advantage of the write-back bursting capabilities of the 82599.
Note: When WTHRESH is set to a non-zero value, the software driver should not set the RS bit in the Tx descriptors. When
WTHRESH is set to zero the software driver should set the RS bit in the last Tx descriptors of every packet (in the case
of TSO it is the last descriptor of the entire large send). Note: When Head write-back is enabled (TDWBAL[n].Head_WB_En = 1b), the
WTHRESH must be set to zero

RXQ Perspective:
Intel does not seem to document pthresh, hthresh, wthresh fields for RX even though DPDK provides a structure to receive them.

On the other hand [this Intel web page](https://www.intel.com/content/www/us/en/developer/articles/guide/dpdk-performance-optimization-guidelines-white-paper.html)
refers to DPDK's defaults for in its [test_pmd_perf.c source code)(http://dpdk.org/browse/dpdk/tree/app/test/test_pmd_perf.c).
They are optimized for Intel's 82599 10 GbE:

```
/*
 * RX and TX Prefetch, Host, and Write-back threshold values should be
 * carefully set for optimal performance. Consult the network
 * controller's datasheet and supporting DPDK documentation for guidance
 * on how these parameters should be set.
 */
#define RX_PTHRESH 8 /**< Default values of RX prefetch threshold reg. */
#define RX_HTHRESH 8 /**< Default values of RX host threshold reg. */
#define RX_WTHRESH 0 /**< Default values of RX write-back threshold reg. */

/*
 * These default values are optimized for use with the Intel(R) 82599 10 GbE
 * Controller and the DPDK ixgbe PMD. Consider using other values for other
 * network controllers and/or network drivers.
 */
#define TX_PTHRESH 32 /**< Default values of TX prefetch threshold reg. */
#define TX_HTHRESH 0  /**< Default values of TX host threshold reg. */
#define TX_WTHRESH 0  /**< Default values of TX write-back threshold reg. */

static struct rte_eth_rxconf rx_conf = {
	.rx_thresh = {
		.pthresh = RX_PTHRESH,
		.hthresh = RX_HTHRESH,
		.wthresh = RX_WTHRESH,
	},
	.rx_free_thresh = 32,
};

static struct rte_eth_txconf tx_conf = {
	.tx_thresh = {
		.pthresh = TX_PTHRESH,
		.hthresh = TX_HTHRESH,
		.wthresh = TX_WTHRESH,
	},
	.tx_free_thresh = 32, /* Use PMD default values */
	.tx_rs_thresh = 32, /* Use PMD default values */
};
```

