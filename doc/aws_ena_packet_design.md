# Introduction

DPDK is a wonderful and performant library with serious support for non-trivial hardware. But DPDK is also by
specialists for specialists. You're not gonna like DPDK until you cross third-base. Be patient. This exposition
is top-down with emphasis on concepts and their composition. You'll compliment it by reading DPDK's doxygen APIs
and examples that ship with this library. You'll need passing familiarity with NUMA nodes in a machine, and know
that NICs, CPU HW cores, and memory have a preferred NUMA node association.

You may find these references helpful:

* [UDP client/server example](https://github.com/rodgarrison/reinvent/tree/main/integration_tests/reinvent_dpdk_udp) 
* [Example UDP client/server configuration](https://github.com/rodgarrison/reinvent/blob/main/scripts/reinvent_dpdk_udp_integration_test)
* [How to make a configuration for this library](https://github.com/rodgarrison/reinvent/blob/main/src/reinvent/dpdk/reinvent_dpdk_initaws.h)
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
one core. Now we're back to scalable performance. NIC queues naturally in RX queues (RXQ) and TX queues (TXQ) flavors.

Third, modern machines are NUMA (Non Uniform Machine Architecture). Programmers incur severe performance penalties
if a H/W resource on NUMA node A --- which includes CPU cores, DRAM, NICs among others --- access H/W resources on a
NUMA node `B<>A`. Therefore one should assume client lcores 11, 12, 13 in the diagram are on NUMA node 0 just like
the NIC they serve. Ditto for server lcores 0,1,2.

Next, NICs have a MAC address and an IPv4 address. These identifiers are transmitted as part of the packet. The server
listens on multi-ports one per queue not a single port. Every client TXQ transmits on its own port. The client TXQ
packets must include source and destination MAC, IPv4, and ports as part of the packet

Then, note each NIC has a maximum number of RXQ and TXQs in H/W. The programmer can elect to use as many or as few
of these queues as desired. But since receiving or transmitting packets on a given NIC/queue requires a lcore to do
that work, and lcores are by definition pthreads pinned to a CPU core, the programmer must determine which lcore
handles which RX or TX queue. This is done twice: once on the client for its NIC/queues, and again on the server.

The considerations here are similar to mathematics where one designates a function `f: X->Y` as injective, surjective,
or bijective. The focus is less on what `X, Y` are --- in our scenario X is the set of RXQ/TXQs for a single NIC and Y
is the set of CPU cores on the same machine both indentified by non-negative integers --- but on how `f` maps.

For maximum concurrency, `f` should be injective. That is, each specific RXQ or TXQ should get its own lcore, which is
what the diagram illustrates: `f` maps client TXQ 10 to lcore 11. `f` maps no other queue to lcore 11. And same on the
server side. DPDK certainly allows multiple queues to run on the same lcore; programmers just need to accept the
trade-offs. DPDK methodology uses the `--lcores` argument passed to `rte_eal_init()` to create and pin lcores as per
the mapping `f` provides.

It's worth noting, for example in the AWS `c5n bare metal` case where ENA NICs have 32 RXQs and 32 TXQs, it is not
possible to use all RX and TX queues with an injective `f`. A `c5n` instance does not have `32+32=64` CPU cores all
on the same NUMA node as the NIC itself. So some CPU cores must do double-duty. Or, optionally, programmers must not
use all queues.

In summary, and for each machine in isolation, the considerations are:

* Make sure the NIC, its memory, and the lcores handling NIC queue processing are on the same NUMA node
* Memory is NUMA aligned by creating huge-page memory mounts one per NUMA node. This is a one time effort at machine
start. DPDK memzones are created one per NUMA node at runtime. Mempools are created from zones. DPDK packets or
mbufs are allocated from memory pools
* Pick a function `f` that determines which lcores run which NIC queues. Typically programmers try to make `f`
injective, but this isn't always possible or needed. For example, if load is low it may be pointless to tie up
CPU cores that could be better used elsewhere. Sharing a core can be net-better
* Each TXQ should transmit packets on its own distinct port; this is appropriate for UDP/TCP 
* Each RXQ should listen for packets on its own distinct port; this is appropriate for UDP/TCP 
* Packet transmission requires the sender's and receiver's MAC, IPv4 address and port. This must be known at mbuf
preparation time. One approach is to provide all those details in the config upfront; this library does this. Another
option is to use a helper TCP kernel-based socket and discover peer information at runtime. Peers ask each other for
H/W inventory before DPDK NIC initialization starts.

This library's 12-factor configuration approach --- essentially key/value pairs maintained in the UNIX environment ---
is the first step to moving configs into `etcd`. Distributed system praxis persistently stores key/value configs in a
high-available seperate cluster.

# Packet Memory Mental Picture

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

While lcore (aka H/W thread, vCPU) to thread affinity is a well developed subject --- generally one prefers threads 
pinned to a CPU core so that its memory accesses come out CPU L1/2/3 cache --- there is no such affinity between a vCPU
and a mempool. If the memory isn't in a CPU's cache the work to retrieve it from DRAM on distinct cores on the same
NUMA node is the same. With this in mind, any reference below to sharing resources across lcores always implies the
resources reside on the same NUMA node. Crossing NUMA nodes is strongly discouraged.

DPDK mempool creation comes in two flavors: shared (or not) and per-lcore-cached (or not). Two mempool creation
routines are typical here. The second method is detailed later so an elided declaration is provided only:

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

struct rte_mempool* rte_mempool_create(
  const char *          name,
  unsigned 	            n,
  unsigned 	            elt_size,
  unsigned              cache_size,
  unsigned              private_data_size,
  rte_mempool_ctor_t * 	mp_init,
  void*                 mp_init_arg,
  rte_mempool_obj_cb_t* obj_init,
  void*                 obj_init_arg,
  int                   socket_id,
  unsigned              dflags 
);	
```

Each RXQ is required to take a mempool pointer at the time the RXQ is created. Two or more distinct RXQs can
then take the same mempool pointer or not. TXQs work the same way except that TXQs do not take a mempool pointer at
creation time. TXQs get around to mempool choice after the NIC is initialized but before a TXQ enters its main
processing loop. TXQs need mbufs to transmit packets hence need a mempool. Thus the same work and principles apply for
TXQ; it's just deferred to post-NIC initialization.

Some code like [eRPC](https://github.com/erpc-io/eRPC) create one mempool per distinct RXQ/TXQ e.g. `RX mempool #1`
only for RXQ1 or `TX mempool #23` only for TXQ23 in the diagram. Other code may create one mempool `shared NUMA 0` and
pass that mempool pointer on to all RXQs and TXQs. Programmers explicitly determine which RXQ/TXQ gets which mempool.
This assignment can be injective or not. Programmers can mix-n-match: some queues may have their own mempools (the
`RX mempool #1` case) or share (`shared NUMA 0` case). Programmers can go to extremes giving each queue its own mempool
(injective, eRPC case) or go to the other extreme making one mempool all queues use.

The second dimension is whether DPDK carves up the memory in the mempool with per-lcore-caching. Per lcore caching
occurs when `cache_size>0`. The purpose of per lcore-caching is to optimize the case when several queues share a
mempool by optimizing concurrent access to that mempool when mbufs are allocated and freed. This is consistent with
point made earlier: retrieving memory from DRAM --- here DRAM in a specific mempool for mbuf allocation --- from two
distinct lcores on the same NUMA node is the same. But that isn't the end of the story. If the memory is part of shared
structure like a mempool, it'll become necessary to deal with concurrent access to that shared structure.
`cache_size>0` is an optimization here. This will become clearer when `rte_mempool_create()` is discussed because it
makes those details explicit. With `rte_pktmbuf_pool_create()` it's slightly hidden.

Whether a programmer chooses an injective scheme or rejects it may turn on packet sizing. If all queues work on uniform
data/packets a single mempool with or without per-lcore-caching is fine. The `elt_size` or `data_room_size`, depending
on which API you use, is satisfactory for all queues. But if some queues predominately work on tiny packets while
others work on large packets making all queues use mbufs of the same size may be counter productive. It'll force the
small packet cases to get mbufs larger than strictly needed using only a portion thereof.  Some programmers, if memory
is plentiful, may prefer this trade-off. It's simpler: one app, one mempool. Each queue uses as much or as little of
the mbuf needed. The injective method directly solves this problem by allowing each queue to choose the number and size
of mbufs for its own purpose.

On the other hand, the injective method means programmers must understand maximal memory demand by queue. Once all the
mbufs are used, you cannot get more especially in the RXQ case. DPDK doesn't provide a realloc or resize helper method
for mempools. Mempools are not elastic `std::vector`s. In the RXQ case you must shutdown the queue, get a new mempool,
and reinitialize it. This may have system-wide effects because the RXQ and its port listener disappear. The programmer
will also have to restart a lcore thread to get the RXQ up and running on the new, larger mempool. That comes with its
own complications. TXQs, upon mempool mbuf exhaustion, can get a new mempool by just creating a new one. But then
there's a time where it'll have to straddle mbufs in different pools. Shared mempools allow different queues to use
different numbers of mbufs without being overly concerned by large per-queue deviations in mbuf usage.

# Mempools and Sizing

With structural issues explained, I now turn to memory sizing. A mempool is a fixed-size memory allocator that, once
created, can efficiently hand out or take back mbuf objects. A [mbuf](https://doc.dpdk.org/api/structrte__mbuf.html)
holds one packet or, in other cases, the continuation of an earlier mbuf when payload sizes exceed the NIC's MTU. In
the `rte_pktmbuf_pool_create()` case, the best way to assess mempool sizing is to work backwards from the mbufs it will 
eventually provide. You do not tell DPDK the absolute memory for a fixed chunk of memory like C `malloc()`; you
parameterize your request by telling DPDK how you want your mbufs configured. DPDK works backward to compute the final
ask.

Best I can tell --- indeed the formal arguments in mempool creation routines are not 1:1 with DPDK's diagram --- the
memory layout for a mbuf is:

```
rte_mbuf object
+-------------------------------+
| rte_mbuf                      |  sizeof(rte_mbuf) bytes starting on an efficient memory boundary
+-------------------------------+
| priv_data                     |  >=0 bytes of private data starting on an efficient memory boundary
+-------------------------------+
| +---------------------------+ |
| | RTE_PKTMBUF_HEADROOM      | |
| +---------------------------+ |  data_room_size bytes starting on an efficient memory boundary
| | data_room_size -          | |  which is at least RTE_PKTMBUF_HEADROOM=128 bytes
| |   RTE_PKTMBUF_HEADROOM    | |
| +---------------------------+ |
+-------------------------------+
```

So, once the you know the number `n` of mbufs you want, and you know the mbuf private, data room size you can to a
first approximation compute the mempool ask as `n * (sizeof(rte_mbuf) + priv_data + data_room_size)` bytes. The
API `rte_pktmbuf_pool_create()` is parameterized by programmer supplied `priv_size, data_room_size` sizes after which
DPDK boils in the rest of the overhead to arrive at `sizeof(mbuf)` **for that mempool**. Different mempools can have
different parameters, clearly, so the resulting mbufs you allocate out of the mempool will also differ in size. How
that happens will become clear when mbuf initialization is done below.

To work backwards into `rte_pktmbuf_pool_create()` one must understand the purpose of `priv_data` and `data_room_size`
in the API given here for handy reference:

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
 
* should account for the payload size in bytes i.e. what applications really care about. Examples follow
* should be at least `rte_config.h:#define RTE_PKTMBUF_HEADROOM 128`. Any valid `data_room_size` must be at least
128 bytes because this is where ethernet headers go for routing purposes. No payload can be transmitted without them
* should be at least `min_[rx|tx]_bufsize(NIC device)`. For AWS ENA NICs this is 64 bytes beyond RTE_PKTMBUF_HEADROOM.
NICs may impose their own requirements on what it will TX or RX. **TODO: need to confirm if it's 64 or HEADROOM +
64 whichever is larger**

**No number you provide should reflect cache line or other memory alignment**. DPDK will organize the memory layout for
optimality.

There's no obvious equivalent to the `tail room` in [DPDK 10.1 diagram](https://doc.dpdk.org/guides/prog_guide/mbuf_lib.html#figure-mbuf1).
At any rate, with these pieces DPDK will create a memory pool on the specified NUMA mode taking its direction with per
mbuf sizing placing the respective fields on appropriate memory boudaries. Callers [may want to inspect the structure returned](https://doc.dpdk.org/api/structrte__mempool.html)
e.g. `size` gives the maximum pool size, and `mz` give the memzone the memzone the zone used to make the pool.

Readers should note neither the private memory or data room size memory in a mbuf is strongly typed. DPDK provides
methods to get the offset to the start of these fields in a specified mbuf only. The programmer must cast to type and
initialize from there without over running memory. Packet initialization examples below will tighten this up.

Callers may set `cache_size>0` in which case DPDK will attempt to evenly partition the mempool with local per-lcore
caching. Recall the point of this optimization is to cut down on concurrent access to a shared mempool object. Let's
call these caching pools. To see the ramifications of a caching pool programmers must:

* Know the number of lcores on the NUMA node the memory pool (e.g. via memzone) is for. More lcores means each lcore
gets less memory and vice-versa. See `--lcores` above which holds the lcore mapping and lcore count
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

Consider three payloads applications might want to send over a NIC:

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

Let's start simple. Suppose an application only uses one of these payloads everywhere without private data. Here's
the natural sizing and mempool policy:

```
 rte_pktmbuf_pool_create() 
+-------------------------+------------------------------+-----------+----------------------------------------------+
| Type used               | data_room_size               | priv_size | Mempool Policy                               |
+-------------------------+------------------------------+-----------+----------------------------------------------+
| MicroPayload            | RTE_PKTMBUF_HEADROOM+2       | 0         | Due to uniform usage one mempool for all     |
|                         |                              |           | lcores is indicated. lcore-caching optional  |
+-------------------------+------------------------------+-----------+----------------------------------------------+
| MediumPayload           | RTE_PKTMBUF_HEADROOM+112     | 0         | Ditto                                        |
+-------------------------+------------------------------+-----------+----------------------------------------------+
| HugePayload             | RTE_PKTMBUF_HEADROOM+4108    | 0         | Ditto, however, this type is so big it may   |
|                         |                              |           | exceed the MTU size. If it does code will    |
|                         |                              |           | either have to increase MTU or split one or  |
|                         |                              |           | fields across multiple mbufs so that multi-  |
|                         |                              |           | mbufs correspond to one HugePayload          |
+-------------------------+------------------------------+-----------+----------------------------------------------+
```

Note data room size is bounded from above by MTU. Keeping priv_size 0, how might one deal with application sets using
all types at the same time? Some of the more obvious solutions include:

```
 rte_pktmbuf_pool_create() 
+---------------------------+-----------------------------+---------------------------------------------------------+
| data_room_size            |  Number of mempools         | Policy                                                  |
+---------------------------+-----------------------------+---------------------------------------------------------+
| RTE_PKTMBUF_HEADROOM+2    |  A pools for MicroPayload   | A total of A+B+C mempools are allocated. A of these are |
| RTE_PKTMBUF_HEADROOM+112  |  B pools for MediumPayload  | assigned to A queues for tiny messages; B to B queues   |
| RTE_PKTMBUF_HEADROOM+4108 |  C pools for HugePayload    | medium messages and the last C for huge messages. Here  |
|                           |                             | the code knows which queues are designated for a payload|
| Injective-case            |  Total A+B+C mempools       | and allocate memory for its specific needs only.        |
|                           |  non caching pools          |                                                         |
+---------------------------+-----------------------------+---------------------------------------------------------+
| RTE_PKTMBUF_HEADROOM+2    |  1 pool for MicroPayload    | 1 mempool is allocated for tiny work. A queues use it.  |
| RTE_PKTMBUF_HEADROOM+112  |  1 pool for MediumPayload   | 1 mempool is allocated for medium work. B queues use it.|
| RTE_PKTMBUF_HEADROOM+4108 |  1 pool for HugePayload     | 1 mempool is allocated for huge work; C queues using. In|
|                           |                             | this approach all A queues working tiny messages share  |
| Hybrid-shared-case        |  Total 3 mempools           | the same queue. Ditto medium, huge. The MTU exception   |
|                           |  non caching pools          | still applies. data room size cannot exceed MTU         |
+---------------------------+-----------------------------+---------------------------------------------------------+
| RTE_PKTMBUF_HEADROOM+4108 |  1 pool for all queues      | Each queue will get an mbuf of the maximum size and only|
|                           |                             | use the number of bytes for the purpose at hand. If     |
| Pure-shared-case          |  caching pool               | memory is plentiful this option may be appealing.       |
+---------------------------+-----------------------------+---------------------------------------------------------+
```

As the section on packet initialization will make clear, a packet is not required to send the full data room size
over the wire. That's why pure-shared mode is a viable solution. Each packet sets its own size bounded from above by
the lesser of MTU size, `data_room_size` and bounded below by RTE_PKTMBUF_HEADROOM. Private memory `k>0`, if used, will
increase numbers by a constant `k` bytes per mbuf (well k + an epsilon e due to padding), and otherwise leave the
larger points unchanged.

**To finish sizing we must deal with `n`, the number of elements** in the mempool. Should be simple, yes? Nope.
It's not quite the no-brainer we'd like to go out on. **Each queue** requires access to a number of mbufs that,

* derive in part from the queue's ring size. The per queue ring size refers to the mbufs (memory) the NIC H/W writes
into upon packet reception (RXQs), or send to the NIC H/W (TXQ) to write onto the wire for transmission. Those mbufs
come from the queue's mempool. Without this facility the NIC will stall because H/W has nowhere to offload packets (RX)
or get data onto the wire (TX).
* DPDK sets a minimum of 128 mbufs/queue but no more than 16384. NIC particulars may modify these limits up/down but
must resolve inside DPDK's limits
* Optional non-negative number of additional TXQ mbufs/queue the application prepares while the NIC H/W is flushing its
TX ring so the NIC H/W is not starved for new work 
* Optional non-negative number of additional RXQ mbufs/queue the application can use to process older, received packets
while the NIC H/W reads new data into available mbufs on its ring
* One is always bounded from above by the amount of free memory

The optional numbers must be considered carefully: you can't work on received, older mbufs or prepare new mbufs for TX
unless those mbufs are distinct from those in-used in the ring. That'll involve a non-avoidable mbuf copy somewhere.
Therefore, you may want to use a bigger ring size per queue, and instead focus on eliminating overhead in mbuf
processing and/or use more queues over more cores if share nothing. Another option is receive delegation: once a packet
is received, you delegate it with optional copy via a helper mempool not attached to a queue so the original mbuf gets
back into the RXQ's ring sooner for reuse. 

A mbuf is returned to the queue's ring for re-use by deallocating or freeing it. In fact DPDK does not actually free
the memory; it's marked usuable.

**Example:** An AWS ENA NIC has 32 RXQ and 32 TXQ H/W queues. DPDK imposes a minimum of 128 mbufs per queue. If we go
with a pure-shared mempool policy and zero-copy processing for all 64 queues we'll arrive at a mimimum configuration
with `n=8192` since:

* Each queue must have at least 128 mbufs
* There are 64 queues total
* All 64 queues are configured with a ring size of 128
* So that's `n=64*128=8192` mbufs
* There's one mempool for all 64 queues so it'll take `n=8192`
* A caching mempool may be useful here
* The queues are zero copy --- barring an unforced programmer copy --- because RX mbufs are processed in-place and
freed for reuse in the ring. TX side mbufs are taken from the mempool, initialized, written, and freed.

If we determine the RX processing is blocked because RXQ rings stay full we might cheaply procure extra performance
by increasing the RXQ ring size to 1024 and separating it out from TX assuming available memory. That gets us two 
memory pools like this:

* 32 RXQs each with 1024 mbufs or `n=32*1024=32768` in the RX pure-shared mempool
* 32 TXQs each with 128 mbufs or `m=32*128=4096` in the TX pure-shared mempool
* A caching pool may be counter-indicated. Suppose the RXQs run on a core subset distinct from TXQ. Each caching
mempool spreads its **memory over all cores** with at least one lcore on it. That immediately means RXQs cannot benefit
from the all the memory in its pool since some if it was diverted to cores containing only TXQ lcores and vice-versa

Most programmers most of the time will choose `n` based on the ring size for the queue. Optional, additional mbufs
hopefully are not required if per-mbuf-processing overhead is low.

Finally, and to close this section, I turn attention to `rte_mempool_create()`. No fundamentals change. The conceptual
difference of `rte_mempool_create()` over `rte_pktmbuf_pool_create()` is emphasis. The `rte_mempool` family of
functions are conceptually focused on allocating a fixed chunk of memory with efficient access. If the programmer's
intention is to use the object created by `rte_mempool_create()` to get mbufs, confluence will get you to the same
place. All mbuf allocations goes through `rte_mbuf* rte_pktmbuf_alloc(struct rte_mempool *mp)`, and  all
`rte_pktmbuf` creation calls all end up in mempool methods anyway. Here's the annotated API:
	
```
struce rte_mempool* rte_mempool_create(
    const char *            name,                 // The name of the mempool
    unsigned                n,                    // The number of elements in the mempool. The optimum size (in terms
                                                  // of memory usage) for a mempool is when n is a power of two minus
                                                  // one: n = (2^q-1)
    unsigned                elt_size,             // The size of each element
    unsigned                cache_size,           // If cache_size is non-zero, the rte_mempool library will try to
                                                  // limit the accesses to the common lockless pool, by maintaining a
                                                  // per-lcore object cache. This argument must be lower or equal to
                                                  // RTE_MEMPOOL_CACHE_MAX_SIZE and n / 1.5. It is advised to choose
                                                  // cache_size to have "n modulo cache_size == 0": if this is not the
                                                  // case, some elements will always stay in the pool and will never be
                                                  // used. The access to the per-lcore table is of course faster than
                                                  // the multi-producer/consumer pool. The cache can be disabled if the
                                                  // cache_size argument is set to 0; it can be useful to avoid losing
                                                  // objects in cache.
    unsigned                private_data_size,    // The size of the private data appended after the mempool structure.
                                                  // This is useful for storing some private data after the mempool
                                                  // structure, as is done for rte_mbuf_pool for example.
    rte_mempool_ctor_t *    mp_init,              // A function pointer that is called for initialization of the pool,
                                                  // before object initialization. The user can initialize the private
                                                  // data in this function if needed. This parameter can be NULL if not
                                                  // needed.
    void*                   mp_init_arg,          // An opaque pointer to data that can be used in the mempool
                                                  // constructor function.  
    rte_mempool_obj_cb_t *  obj_init,             // A function pointer that is called for each object at
                                                  // initialization of the pool. The user can set some meta data in
                                                  // objects if needed. This parameter can be NULL if not needed. The
                                                  // obj_init() function takes the mempool pointer, the init_arg, the
                                                  // object pointer and the object number as parameters.
    void*                   obj_init_arg,         // An opaque pointer to data that can be used as an argument for each
                                                  // call to the object constructor function.
    int                     socket_id,            // The socket_id argument is the socket identifier in the case of
                                                  // NUMA. The value can be SOCKET_ID_ANY if there is no NUMA
                                                  // constraint for the reserved zone.
    unsigned                flags                 // The flags arguments is an OR of following flags:
                                                  // * RTE_MEMPOOL_F_NO_SPREAD: By default, objects addresses are
                                                  //   spread between channels in RAM: the pool allocator will add
                                                  //   padding between objects depending on the hardware configuration.
                                                  //   See Memory alignment constraints for details. If this flag is
                                                  //   set, the allocator will just align them to a cache line.
                                                  // * RTE_MEMPOOL_F_NO_CACHE_ALIGN: By default, the returned objects
                                                  //   are cache-aligned. This flag removes this constraint, and no
                                                  //   padding will be present between objects. This flag implies
                                                  //   RTE_MEMPOOL_F_NO_SPREAD.
                                                  // * RTE_MEMPOOL_F_SP_PUT: If this flag is set, the default behavior
                                                  //   when using rte_mempool_put() or rte_mempool_put_bulk() is
                                                  //   "single-producer". Otherwise, it is "multi-producers".
                                                  // * RTE_MEMPOOL_F_SC_GET: If this flag is set, the default behavior
                                                  //   when using rte_mempool_get() or rte_mempool_get_bulk() is
                                                  //   "single-consumer". Otherwise, it is "multi-consumers".
                                                  // * RTE_MEMPOOL_F_NO_IOVA_CONTIG: If set, allocated objects won't
                                                  //   necessarily be contiguous in IO memory.
```

This API provides means to initialize the memory with function pointers, and makes more explicit via the formal
argument `flags` how `cache_size>0` mempool caching can be optimized.

The formal arguemens `elt_size, private_data_size` indicate this DPDK library call parameterizes the memory ask
differently compared to `rte_pktmbuf_pool_create()`. In this case `elt_size`:

* should include the payload size in bytes of the packet i.e. what applications really care about
* should be at least `rte_config.h:#define RTE_PKTMBUF_HEADROOM 128`. Any valid `data_room_size` must be at least
128 bytes because this is where ethernet headers go for routing purposes. No payload can be transmitted without them.
Your payload will follow those 128 bytes.
* should be at least `min_[rx|tx]_bufsize(NIC device)`. For AWS ENA NICs this is 64 bytes beyond RTE_PKTMBUF_HEADROOM.
NICs may impose their own requirements on what it will TX or RX.
* should add in or include `sizeof(rte_mbuf)`

That is, `elt_size` is an all-in-ask. But like before, **no number you provide should reflect cache line or other memory
alignment**. This API also provides for the facility to append a programmer mananged `private_data_size` at the end of
the pool.

# TX Mbuf Initialization and Preparation

Once you've entered the main loop for a TXQ, you've got a lot of structure working in your favor:

* You know which TXQ you're running on
* You know the lcore you're running on [or can use the RTE lcore library to tech that out](https://doc.dpdk.org/api/rte__lcore_8h.html)
* You have a mempool in hand [from which you can allocate an mbuf](https://doc.dpdk.org/api/rte__mbuf_8h.html#ad4d1c289d8cffc831dfb77c64f52447b)
* You'll almost certainly know the payload type you want to send e.g. `MediumPayload` or can sort that out in a few if-then-elses
* You know the contents of the payload to send
* You know the MAC address, port, and IPv4 address of the intended receiver (packet destination)
* You know the MAC address, port, and IPv4 address of the sender (source)

All you've got to do is stuff those bytes into a mbuf, [and transmit it with a TX burst call](https://doc.dpdk.org/api/rte__ethdev_8h.html#a83e56cabbd31637efd648e3fc010392b).
Then declare success. But how?

Let's suppose we want to send `MediumPayload` objects:

```
struct MediumPayload {
  int messageType;      // perhaps an enumerated value
  int cmd;              // ditto
  int args[16];         // a bunch of args
  int epsilon;          // perhaps for a calculation
  char callerId[32];    // to connect server errors to client callers
  unsigned sequenceNo;  // some sort of monotonic counter
};
// sizeof(MediumPayload)==112
```

First, allocate an mbuf from a pool with a conforming `data_room_size`. The right `data_room_size`, which must have
been passed to `rte_pktmbuf_pool_create()`, will become clear when when all the details on how to populate a mbuf
is given next. You can then lift that informantion back to the place where the pool is made. This way your pool will
conform by construction:

```
  // pool is a non-nil rte_mempool pointer created elsewhere. The mbuf's data_room_size must
  // conform to MediumPayload and otherwise meet or exceed requirements discussed earlier
  rte_mbuf *mbuf = rte_pktmbuf_alloc(pool);
  assert(mbud);
```

Second, get a pointer to the start of the data room where the headers and payload go. Now, this memory area is not
strongly typed by DPDK. This moves the responsibility from DPDK onto the programmer who must determine the types,
order of types, and type instance content as one advances down and through the mbuf dataroom span during
initialization. This is a consequence of the DPDK decision to `embed metadata within a single memory buffer...` in a
mbuf. This is also a consequence of the fact the non-payload headers depend on the NIC H/W. It can also depend on what
message(s) one is sending --- UDP v ARP or IPv4 vs IPv6 --- DPDK cannot practically provide types for all scenarios.

In the AWS ENA NIC case the mental picture starts like this:

```
rte_mbuf object
+-------------------------------+
| rte_mbuf                      |  sizeof(rte_mbuf) bytes starting on an efficient memory boundary
+-------------------------------+
| priv_data                     |  >=0 bytes of private data starting on an efficient memory boundary
|                               |
+-------------------------------+  start of the data room on a cache friendly memory boundary where
| Data room                     |  ethernet headers/payload will be written. To get a pointer to this
|                               |  field run rte_pktmbuf_mtod(mbuf, <T>) where T is a type describing
|                               |  the memory layout starting at byte offset 0 on the data room memory
|                               |  blob. `rte_pktmbuf_mtod` is a macro that casts addresses inside mbuf
|                               |  to caller provided type `T`
|                               |
+-------------------------------+  start of tail of mbuf of uncertian utility as of this writing 
|             .                 |
|             .                 |  
|             .                 |
+-------------------------------+
```

There are two technical details here none complex. First the difference `(tail offset - data room offset)` will not
in general equal `sizeof(T)` putatively equal to `data_room_size` provided to `rte_pktmbuf_pool_create()` when the
pool was made. There may be padding at the end of data room before tail of the mbuf. Second, it is not always helpful
to construe an object of type `T` as giving the memory layout for all bytes in the data room. That's because data room
contents are naturally decomposed into parts arising from the domain of this work: packet transmission. Packet format
is totally dependent on the particular details of how the packet is sent legion in combinations:

```
+-------------------------------+  start of the data room on a cache friendly memory boundary where
| +---------------------------+ |-+ 
| | Route information         | |  \
| +---------------------------+ |   \
| | IPv4 or IPv6 Packet header| |    +---- packed memory
| +---------------------------+ |   /
| | UDP header                | |  /
| +---------------------------+ |-+ 
| | Payload                   | |
| +---------------------------+ | 
+-------------------------------+
```

Programmers may or may not, especially when multiple payload types are in play, locate the fields for the headers in
the payload type. There is a tendancy --- certainly not required --- to have a type for the route information T1,
another type for the IP header T2, a UPD header T3, plus a fourth type for the payload itself. This tendancy is further
influenced by application requirements. If you are transmitting and receiving elsewhere a wide variety of messages
types each with different header requirements, it's net easier to define separate types for everything then, at TX 
packet creation time, lay-in the data for needs at hand.

Our goal here, however, is to stay simple. All messages are IPv4 UDP with the same exact payload type namely
`MediumPayload`. Because of the uniform nature of this use-case I'll fallback to a single type `T` placing the fields
from `MediumPayload` at the end of a combined type. This will be done with a helper structure `IPv4UDP`. There's three
small wins:

* `sizeof(T)` will indeed accurately give the `data_room_size` so creating a conforming mempool is simple
* The offset after from the end of the UDP packet header to the end of payload is already layed out in memory inclusive
of padding. This is important because the UDP header requires a data length size. The compiler will tech out the rest
* We won't have to play alignment, cast, pointer arithmetic games to place the fields in `MediumPayload` on a cache
friendly boundary after the end of the headers without overrunning the end of the mbuf making the final data room size
that much more difficult to calculate

```
// include <sys/types.h>
struct IPv4UDP {
  //
  // Routing information
  //
  u_int8_t  d_dstMacAddress[6]; // MAC address of NIC to receive this data
  u_int8_t  d_srcMacAddress[6]; // MAC address of NIC sending this data
  u_int16_t d_ethernetType;     // one or more DPDK RTE_ETH_FLOW flags e.g. RTE_ETH_FLOW_NONFRAG_IPV4_UDP

  //
  // IP header fields
  // 
  // Documentation sources include,
  //
  // * https://fab.cba.mit.edu/classes/961.04/people/neil/ip.pdf pg 3 
  // * https://en.wikipedia.org/wiki/IPv4#Packet_structure
  //
  u_int8_t  d_version : 4;      // IP version by definition 4 here
  u_int8_t  d_ihl     : 4;      // Length of IP header in 32-bit words typically set 5. That's 5 32-bit words
                                // noting total size of this IP header is 160 bits / 32 bits = 5 32-bit words.
                                // 5 is also a minimum. This header can be in larger in edge cases due to an
                                // optional fields we do not use here
  u_int8_t  d_dscp;             // Differentiated Services Code Point or type of service. See documenation
                                // https://en.wikipedia.org/wiki/Differentiated_services. We'll use 10 which means
                                // 'High-throughput data'
  u_int16_t d_totalLength;      // Total size of IP data e.g. this header, the UDP header that follows, and the
                                // payload after that. This is an all-in-value
  u_int16_t d_id;               // See Wikipedia. Defaulting to 0 always
  u_int16_t d_fragFlags : 3;    // Default to 1 always meaning request H/W does not fragment this IP packet
  u_int16_t d_fragOffset: 13;   // Offset of this packet relative to original, initial fragment. Since not
                                // fragmenting, default to 0 always
  u_int8_t  d_ttl;              // time to live in seconds. when this hits 0 the packet is dropped. Will default
                                // to 10, but this value will see a more complex treatment under congestion control
                                // in a seperate document.
  u_int8_t  d_protocol;         // Default 17 for UDP or User Datagram Protocol
  u_int16_t d_checksum;         // IPv4 checksum of this IP packet. Offloaded to H/W by ________ config.
                                // See https://en.wikipedia.org/wiki/IPv4_header_checksum
  u_int32_t d_srcAddress;       // IPv4 address of NIC sending IP packet
  u_int32_t d_dstAddress;       // IPv4 address of NIC to recieve this IP packet
  
  //
  // UDP packet header
  //
  // Docmentation sources include,
  //
  // * https://en.wikipedia.org/wiki/User_Datagram_Protocol
  //
  u_int16_t d_srcPort;          // UDP port of machine sending this UDP data
  u_int16_t d_dstPort;          // UDP port on machine receiving this UDP data
  u_int16_t d_length;           // Length in bytes of the payload to follow
  u_int16_t d_checksum;         // Checksum of data payload, optional

}; __attribute__((packed));     // Packed!
```

Now compose with `MediumPayload` to make:

```
struct IPv4UDPMedium {
  //
  // The header(s) prefix
  //
  Header          d_header;

  //
  // The payload suffix
  //
  MediumPayload   d_payload;
};
```

Next, and the with planning portion of data rooms sorted out, we can now get an appropriate data room pointer with a
one liner and start assignments:

```
IPv4UDPMedium *packet = rte_pktmbuf_mtod(mbuf, (IPv4UDPMedium*));
assert(packet);
packet->d_header.d_dstMacAddress = ...
 .
  .
   .
packet->d_payload.messageType = ...
```

Some of these fields come with conversion noise (htonl, atoi, internal to IP official formats) so checkout the
`reinvent_dpdk_udp` integration code for specifics. But at least the work is all down hill. The last step writes
the packet onto the wire. Burst transmission tries to amortize overhead by taking multiple prepared packets onto
the wire at once, but the API works just fine with one packet too:

```
// Sent 1 mbuf on DPDK device 0 on TXQ 10 assigning returning number actually sent
uint16_t sent = rte_eth_tx_burst(0, 10, mbuf, 1);
```

The `mbuf` cannot be reused by the TXQ ring until we return it. If we don't return it TX will stall and the mempool
will become exhausted:

```
// Make the mbuf re-useable by the TXQ ring and H/W
rte_pktmbuf_free(mbuf)
```

Given the simple, repetitive nature of mbuf life-cycle, DPDK has helper methods to auto-free.
