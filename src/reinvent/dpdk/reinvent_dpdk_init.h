#pragma once

// Purpose: Initialize DPDK device (aka port) and related RX/TX software objects
//
// Classes:
//  Dpdk::Init     Configure one DPDK capable device culminating with a set of NUMA aligned lcores each
//                 configured for one RX queue, or one configured TX queue writing the final configuration set into
//                 caller specified 'Dpdk::Config'. Post initialization callers should run rte_eal_mp_remote_launch
//                 to start and run the RX/TX lcores usually with a helper 'Dpdk::Worker' object.
//
// Limitation:     Multi-NICs & distinct HW cores: DPDK generally will not allow different PIDs to use the same DPDK
//                 device. Typically the host runs one task (pid) per DPDK capable NIC. This code can properly do that.
//                 However, as currently written, it cannot know if one pid's HW core assignment, and another pid's are
//                 distinct. It can only guarantee that one session's HW core assignments are distinct. This may be
//                 important when the lcore config keyword DISTINCT is given. For example, suppose a box has two DPDK
//                 capable NICs devices 0, 1. One PID (with its config) runs 2 distinct TX queues on two distinct HW
//                 cores 0, 1 for device 0. The second PID (with its seperate config) wants to do the same thing. But
//                 because the pid for device 1 doesn't know HW cores 0,1 are already in use for device 0, it can also
//                 assign HW cores 0, 1. The work around is to manually ensure each config uses distinct HW cores in
//                 configuration. Device 1's config, for example, should set VCPU core 0, 1 soft disabled.
// 
//                 More importantly, not every single config possible in DPDK is covered by this class. There's no
//                 cyrpto support. Users will find the supported capability here practical but by no means complete.
//
// See Also:       Dpdk::Config
//                 Dpdk::Worker
//                 integration_tests/reinvent_dpdk_udp minimal-complete client/server example
//                 Memory pool sizing documentation in doc/packet_design.md
//
// Note: you can always dump a configuration to stdout: 'Dpdk::Config' supports 'operator<<(ostream&)'
//       the dump will show you how the enviroment was converted into an ENA config
// Note: you can always dump the Environment to stdout: 'Dpdk::Enviroment' supports 'operator<<(ostream)'.
//       the dump will show you all the ENV variables read with their raw values
//  
// Thread Safety: Not MT thread-safe.
//                                                                                                                      
// Exception Policy: No exceptions                                                                                      
//
// Description: DPDK device setup comes in two parts: initialization of the H/W, and reading H/W capabilities to which
// software resources are created and aligned to leverage that H/W. For example, if a NIC has 32 RXQs the software side
// can config up to 32 lcores with attendant mempools to perform RX. Thread/NIC-queue association is a major part of
// this work.
//
// Environment variables are the primary way to name and set out configurations. The names are constructed by chaining
// together naming keywords, prefixes, and suffixes like any moderately complex bash script. Programmers alway have the
// option to combine all configs for all machines in a single file partitioned by prefix names, or keep them separate:
//
// * The initialize method takes two strings, a prefix name optionally empty, and device name which cannot be empty.
// * When locating environment variables the library always looks for variables starting with the concatenation of
//   '<prefix>+<deviceName>' if it is NIC specific or '<prefix>' if the config is machine specific
// * Required keywords and optional suffixes are appended to this result to make the full name
// * Use '<prefix>' and '<deviceName>' to partition your devices from a naming and configuration standpoint
// * Use the following table to find required keywords and optional suffixes
//
// The author is aware that 12-factor style configuration deviates from DPDK code primarly command line and code based.
// However, DPDK methodology is opaque to beginners. It drags the beginner into DPDK specifics, deep into the DPDK
// library, and into non-trivial example code where configs are assigned into DPDK structures. Moving the config
// out of code into a standalone script eliminates this problem. A script reduces the temptation to litter configs in
// code, on the command line switches with its defaults, json files, or ENV variables. The Reinvent library has no
// defaults; all configs are from UNIX environment values. Finally UNIX environment values are the first step to moving
// everything to 'etcd'. In distributed work, praxis is to keep configs in a highly-available cluster storing simple
// key/value pairs. Zookeeper would be another option. 'etcd' is Reinvent's future destination for configs.
//
// The following table lists the UNIX variables inspected. The 'gen' column refers to an example script holding the
// variable or which can generate it. A subset of configuration variables are common across projects. Here a generator
// script makes sense. Programmers can run it then extract whatever is required. These scripts are marked 1,2,3 in the
// 'gen' column. The asterisked items are specific to an application. Reinvent ships with application bash scripts
// which clearly demo these values. Footnotes are given under the table where these scripts can be found.
//
// The 'Env Variable Name' column gives the schema for the variable name. Some configurations are NIC specific; those
// names have the schema '{prefix}_{deviceId}'; see again '<prefix>+<deviceName>' discussed above. Other configs are
// not NIC specific but are machine specific. Here the schema starts with '{prefix}'.'{id}' refers to an integer index
// For example, the schema 'VCPU_{id}' implies the configs read like 'VCPU_0, VCPU_1, VCPU_2' and so on up to the H/W
// maximum given by 'VCPU_MAX'. Unless otherwise documented in the comment column, the description describes the RX 
// variation. The TX situation follows analgously.
//
// The last column provides a description. If the ENV variable merely holds a value that's passed on uniterpreted to
// an attribute in 'Dpdk::Config', that attribute name is given. If there is no direct relationship, the description
// references the attribitues in 'Dpdk::Config' influenced.
//
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | Env Variable Name    (DPDK-API related)     | Gen | Comment                                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_DPDK_INITIALIZATION_PARAMETERS     | [*] | Comma separated DPDK initialization arguments. Can be empty. |
// |                                             |     | Dpdk::Init calculates '--lcores, -n' arguments for you and   |
// |                                             |     | inserts in into this string. Usually you'll have to provide  |
// |                                             |     | '--proc-type, --in-memory' plus '--huge-dir'. '--log-level'  |
// |                                             |     | is optional but typical. See integration examples for example|
// |                                             |     | configurations. This variable deviates from the naming schema|
// |                                             |     | of all other values because DPDK takes these parameters ONLY |
// |                                             |     | as a string which is parses and consumes internally.         |
// +---------------------------------------------+-----+--------------------------------------------------------------+
//
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | Env Variable Name     (VCPU/DRAM related)   | Gen | Comment                                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_VCPU_MAX                           | [2] | The number of VCPUs machine supports. Individual VCPUs {id}s |
// |                                             |     | are named in range [0, MAX). Effects Config::vcpu            |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_VCPU_{id}                          | [2] | One of 'true|false'. 'true' means the VCPU {id} is considerd |
// |                                             |     | enabled for use at runtime; 'false' is soft-disabled         |
// |                                             |     | Effects Config::vcpu                                         |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_VCPU_{id}_IN_CORE                  | [2] | Gives the 0 based core VCPU {id} runs on                     |
// |                                             |     | Effects Config::vcpu                                         |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_VCPU_{id}_IN_SOCKET                | [2] | Gives the 0 based socket VCPU {id} runs on                   |
// |                                             |     | Effects Config::vcpu                                         |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_VCPU_{id}_ON_NUMA_NODE             | [2] | Gives the 0 based NUMA node VCPU {id} runs on                |
// |                                             |     | Effects Config::vcpu                                         |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_MEMORY_CHANNEL_COUNT               | [3] | Number of DRAM channels for host box                         |
// |                                             |     | Effects Config::memoryChannelCount. Critical for mempool     |
// |                                             |     | creation.                                                    |
// +---------------------------------------------+-----+--------------------------------------------------------------+
//
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | Env Variable Name        (NIC related)      | Gen | Comment                                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId} = {id}                  | [*] | DPDK deviceId n>=0 effects Dpdk::Config::deviceId.           |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_PCI_DEVICE_ID           | [1] | e.g. '0000:7f:00.0' for {deviceId}. Effects 'Config::pciId   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_NUMA_NODE               | [1] | The NUMA node to which DPDK device {deviceId} is attached. If|
//                                               |     | this number disagrees with DPDK's runtime report an error is |
// |                                             |     | is reported and init stops. Effects 'Config::numaNode'       |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SOFT_ENABLED            | [1] | One of 'true|false'. 'true' means '{deviceId}' is considered |
// |                                             |     | enabled for use at runtime; 'false' is soft-disabled. Usually|
// |                                             |     | set 'true', may be set false to soft-disable the H/W.        |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_RX_QUEUE_SIZE           | [1] | The number of queues to run. Effects Config::rxqCount and    |
// |                                             |     | txqCount. If this value is higher than the HW max found by   |
// | {prefix}_{deviceId}_TX_QUEUE_SIZE           |     | DPDK an error reported.                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_RXQ_THREAD_COUNT        | [*] | '1<=N<={prefix}_{deviceId}_RX_QUEUE_SIZE' or 'QUEUE_SIZE'    |
// |                                             |     | Sets 'Config::rxqThreadCount'. If set to 'QUEUE_SIZE'        |
// | {prefix}_{deviceId}_TXQ_THREAD_COUNT        |     | RXQ thread count is assigned to the number provided by env   |
// |                                             |     | '{prefix}_{deviceId}_RX_QUEUE_SIZE' and N otherwise.         |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_RXQ_VCPU_POLICY         | [*] | One of 'DISTINCT|MULTI|OFF'. DISTINCT means a distinct H/W   |
// |                                             |     | core is assigned to each RX lcore whose job it is to perform |
// | {prefix}_{deviceId}_TXQ_VCPU_POLICY         |     | RX on exactly one RXQ. The H/W core assigned cannot be used  |
// |                                             |     | by another RXQ or TXQ. MULTI allows two or more lcores to be |
// |                                             |     | assigned to the same H/W core regardless of purpose RX or TX.|
// |                                             |     | OFF means no resources for RX will be configured. Effects    |
// |                                             |     | Config::rxqPolicy, txqPolicy.                                |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_RXQ_VCPU_MASK           | [*] | A VCPU core mask which delimits which HW cores or VCPUs      |
// |                                             |     | can perform RX for device {deviceId}. Valid values is a int  |
// | {prefix}_{deviceId}_TXQ_VCPU_MASK           |     | list or 'ALL'. ALL means any HW core on the same NUMA node to|
// |                                             |     | which '{prefix}_{deviceId}' is attached is valid. A list with|
// |                                             |     | '1,5,10' means only VCPUs 1 or 5 or 10 are valid for RX work |
// |                                             |     | assignment. Each VCPU given must be on the same NUMA node as |
// |                                             |     | the NIC itself. Effects Config::rxqCandidateVcpuList and     |
// |                                             |     | txqCandidateVcpuList. Excluding VCPUs means DPDK resource    |
// |                                             |     | won't be assigned to it leaving those free for other uses.   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_{MTU}                   | [*] | Maximum packet size in bytes. Effects Config::mtu. Packet    |
// |                                             |     | design TX side by respect this if RX will run over that data |
// +---------------------------------------------+-----+--------------------------------------------------------------+
//
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | Env Variable Name      (Memzone Related)    | Gen | Comment                                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMZONE_KB              | [*] | Memory reserved for all uses RX and TX in KB for the device  |
// |                                             |     | '{prefix}_{deviceId}'. Memory is allocated on the same NUMA  |
// |                                             |     | node the NIC is attached to. Effects Config attrs            |
// |                                             |     | Config::memzoneReserveKb                                     |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMZONE_MASK            | [*] | Hugepage mask for '{prefix}_{deviceId}_MEMZONE_KB'. See      |
// |                                             |     | https://doc.dpdk.org/api/rte__memzone_8h.html#a3ccbea77\     |
// |                                             |     | ccab608c6e683817a3eb170f and RTE_MEMZONE_xxx flags which help|
// |                                             |     | choose which TLB pages are used to satisfy request. In this  |
// |                                             |     | library one task has exactly one memzone for one named device|
// |                                             |     | Effects Config::memzoneMask                                  |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMZONE_NAME            | [*] | Textual/human name '{prefix}_{deviceId}_MEMZONE_KB'          |
// |                                             |     | Effects Config::memzoneName                                  |
// +---------------------------------------------+-----+--------------------------------------------------------------+
//
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | Env Variable Name (RXQ/TXQ Mempool Related) | Gen | Comment                                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMPOOL_POLICY          | [*] | One of 'PER_QUEUE|SHARED'. The latter means all RXQ and TXQs |
// |                                             |     | will share the same pool. See 'doc/packet_design.md'         |
// |                                             |     | This option is often used when each queue will take mbufs of |
// |                                             |     | a maximum size using only as many bytes for purpose at hand. |
// |                                             |     | SHARED mempools requires five additional configs below all   |
// |                                             |     | with name substring '_SHARED_MEMPOOL_'. PER_QUEUE means RXQ, |
// |                                             |     | TXQ give its individual requirements separately. PER_QUEUE   |
// |                                             |     | take 8 additional configs with substring '_MEMPOOL_RXQ_'     |
// |                                             |     | (there are 4 of these) and 4 '_MEMPOOL_TXQ_' (four more).    |
// |                                             |     | Each environment variable is an int list containing a number |
// |                                             |     | entries eqaul to '...RXQ_THREAD_COUNT' and 'TXQ_THREAD_COUNT |
// |                                             |     | above. Only those variables implied by this setting are read.|
// |                                             |     | Shared pools are created with 'rte_mempool_create()' and non-|
// |                                             |     | pools created with 'rte_pktmbuf_pool_create()'. Each mempool |
// |                                             |     | is automatically assigned a human readble name. At this time |
// |                                             |     | the function pointers in 'rte_mempool_create()' are not      |
// |                                             |     | supported in this API, but can be leverage post initiaization|
// |                                             |     | by application code prior to entering main event-loop.       |
// |                                             |     | Effects contents of Config::rxq and txq.                     |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMPOOL_RXQ_SIZE        | [*] | Integer list of number of elements (mbufs) per RXQ mempool   |
// |                                             |     | See 'doc/packet_design.md'                                   |
// | {prefix}_{deviceId}_MEMPOOL_TXQ_SIZE        |     | Entry count must equal  RXQ_THREAD_COUNT/TXQ_THREAD_COUNT    |
// |                                             |     | Effects Config::rxq/txq                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMPOOL_RXQ_CACHE_SIZE  | [*] | Integer list of cache size in bytes per RXQ. For non-shared  |
// |                                             |     | 0 bytes/RXQ is typical. See 'doc/packet_design.md'           |
// | {prefix}_{deviceId}_MEMPOOL_TXQ_CACHE_SIZE  |     | Entry count equal to RXQ_THREAD_COUNT/TXQ_THREAD_COUNT       |
// |                                             |     | Effects Config::rxq/txq                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMPOOL_RXQ_PRIV_SIZE   | [*] | Integer list of private size in bytes per mbuf per RXQ often |
// |                                             |     | zero. See 'doc/packet_design.md'                             |
// |                                             |     |                                                              |
// | {prefix}_{deviceId}_MEMPOOL_TXQ_PRIV_SIZE   |     | Entry count equal to RXQ_THREAD_COUNT/TXQ_THREAD_COUNT       |
// |                                             |     | Effects Config::rxq/txq                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMPOOL_RXQ_DATA_ROOM_\ | [*] | Integer list of data room size in bytes per mbuf per RXQ.    |
// |   SIZE                                      |     | Critical setting. See 'doc/packet_design.md'                 |
// |                                             |     |                                                              |
// | {prefix}_{deviceId}_MEMPOOL_TXQ_DATA_ROOM_\ |     | Entry count equal to RXQ_THREAD_COUNT/TXQ_THREAD_COUNT       |
// |   SIZE                                      |     | Effects Config::rxq/txq                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SHARED_MEMPOOL_SIZE     | [*] | Number of mbufs (buffers) in shared mempool for all queues   |
// |                                             |     | uses. See 'doc/packet_design.md'                             |
// |                                             |     | Effects Config::rxq/txq                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SHARED_MEMPOOL_ELT_SIZE | [*] | Size of each mbuf (buffer) in bytes in shared mempool.       |
// |                                             |     | See 'doc/packet_design.md'                                   |
// |                                             |     | Effects Config::rxq/txq                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SHARED_MEMPOOL_CACHE_\  | [*] | Size of per core cache in bytes for shared mempool.          |
// |   SIZE                                      |     | See 'doc/packet_design.md'                                   |
// |                                             |     | Effects Config::rxq/txq                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SHARED_MEMPOOL_PRIVATE\_| [*] | Private data size in bytes of shared mempool.                |
// |   DATA_SIZE                                 |     | See 'doc/packet_design.md'                                   |
// |                                             |     | Effects Config::rxq/txq                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SHARED_MEMPOOL_FLAGS    | [*] | Flags refining behavior of per-core caching                  |
// |                                             |     | See 'doc/packet_design.md'                                   |
// |                                             |     | Effects Config::rxq/txq                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_RXQ_RING_SIZE           | [*] | Number of mbufs (buffers) per RXQ to reserve for NIC to write|
// |                                             |     | packets into after read off the wire. RXQs need mbufs at     | 
// |                                             |     | setup time. This number can differ from TXQ ring size next.  |
// |                                             |     | Either provide a single number in which case every RXQ will  |
// |                                             |     | use that number of mbufs or an int list giving the per RXQ   |
// |                                             |     | amount. If int-list should have RXQ_THREAD_COUNT entries     |
// |                                             |     | Effets Config::rxq                                           |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_TXQ_RING_SIZE           | [*] | Number of mbufs (buffers) per TXQ to reserve for NIC to read |
// |                                             |     | from which it then writes onto the wire. TX queues do not get|
// |                                             |     | memory at setup time; is provided after setup just prior to  |
// |                                             |     | entering the TX main-loop. Init will prealloc it so that it's|
// |                                             |     | ready then. TXQ ring size need not be the same as RXQ above. |
// |                                             |     | Either provide a single number in which case every TXQ will  |
// |                                             |     | use that number of mbufs or an int list giving the per TXQ.  |
// |                                             |     | If int-list should have TXQ_THREAD_COUNT entries. Effects    |
// |                                             |     | Config::txq.                                                 |
// +---------------------------------------------+-----+--------------------------------------------------------------+
//
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | Env Variable Name (TCP/UDP NIC related)     | Gen | Comment                                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_LINK_SPEED                | [*] | A logical-OR of the RTE_ETH_LINK_SPEED defined flags as      |
// |                                             |     | found in https://doc.dpdk.org/api/rte__ethdev_8h.html        |
// |                                             |     | and set in https://doc.dpdk.org/api/structrte__eth__conf.html|
// |                                             |     | Do not use the RTE_ETH_SPEED_NUM variations; use the defines |
// |                                             |     | based on RTE_BIT bit mask not the numeric valyue             |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_RX_MQ_MASK                | [*] | Exactly one of the RTE_ETH_RX_MQ enum values as defined in   |
// |                                             |     | https://doc.dpdk.org/api/rte__ethdev_8h.html and set in      |
// | {prefix}_{device}_TX_MQ_MASK                |     | mq_mode https://doc.dpdk.org/api/structrte__eth__rxmode.html |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_RX_OFFLOAD_MASK           | [*] | A logical-OR of the RTE_ETH_RX_OFFLOAD flags as defined in   |
// |                                             |     | https://doc.dpdk.org/api/rte__ethdev_8h.html and set in      |
// | {prefix}_{device}_TX_OFFLOAD_MASK           |     | offloads https://doc.dpdk.org/api/structrte__eth__rxmode.html|
// |                                             |     | Example: a value of 'RTE_ETH_RX_OFFLOAD_CHECKSUM' offloads   |
// |                                             |     | IPV4, UDP, TCP checksums to NIC H/W. Offload refers to       |
// |                                             |     | delegating to H/W (if H/W supports it) work that would       |
// |                                             |     | otherwise woulkd be done in software. Not offloading can in  |
// |                                             |     | special cases allow software to delegate to other specialty  |
// |                                             |     | H/W like a GPU/FPGA/ASIC, for example, IPSec work            |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_TX_DEFAULT_FLOW           | [*] | rte_ether.h RTE_ETHER_TYPE_* defines e.g. 0x800 is IPV4      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_RX_RSS_KEY                | [*] | 40 byte sequence to help assign a RXQ for an incoming packet |
// |                                             |     | in format 'xx:xx:...:xx:' where each 'xx' is a hex byte value|
// |                                             |     | for a total length of 120=(40*3) chars. If RSS is disabled   |
// |                                             |     | this value should be omitted. Refer to the DPDK documentation|
// |                                             |     | http://doc.dpdk.org/api/structrte__eth__rss__conf.html and   |
// |                                             |     | https://www.ndsl.kaist.edu/~kyoungsoo/papers/TR-symRSS.pdf on|
// |                                             |     | selecting a value. See RX_RSS_HF for additional details.     |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_RX_RSS_HF                 | [*] | A integer value that describes which packet/flow types are   |
// |                                             |     | subject to RSS .e.g. 41868 means all IP packets. Refer to    |
// |                                             |     | http://doc.dpdk.org/api/structrte__eth__rss__conf.html and   |
// |                                             |     | http://doc.dpdk.org/api/rte__ethdev_8h_source.html near line |
// |                                             |     | 734 where macros define RTE_ETH_RSS_IP, RTE_ETH_RSS_UDP etc..|
// |                                             |     | For RSS to work this value must be non-zero, a valid RSS key |
// |                                             |     | must be defined (above), and RX_MQ_MASK (see above) must at  |
// |                                             |     | least enable the RTE_ETH_MQ_RX_RSS bit.                      |
// |                                             |     | Effects Config::defaultRoute
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_DEFAULT_ROUTE_SRC_MAC     | [*] | A list of source MAC addresses where each address is in the  |
// |                                             |     | format 'xx:xx:xx:xx:xx:xx' where each 'xx' is a valid hex.   |
// |                                             |     | This list together with the following 5 lists comprise to    |
// |                                             |     | a set of default routes for transmitting IPV4 packets. There |
// |                                             |     | be a least one default route, however, it's only used if     |
// |                                             |     | code references it. Otherwise code can make routes through   |
// |                                             |     | normal C++ object 'IPV4Route' RII                            |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_DEFAULT_ROUTE_DST_MAC     | [*] | A list of dest MAC addresses where each address is in the    |
// |                                             |     | format 'xx:xx:xx:xx:xx:xx' where each 'xx' is a valid hex.   |
// |                                             |     | The number of entries must equal DEFAULT_ROUTE_SRC_MAC       |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_DEFAULT_ROUTE_SRC_IP      | [*] | A list of source IPV4 addresses where each address is in the |
// |                                             |     | format 'd.d.d.d' where each 'd' is a valid decimal number.   |
// |                                             |     | The number of entries must equal DEFAULT_ROUTE_SRC_MAC       |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_DEFAULT_ROUTE_DST_IP      | [*] | A list of dest IPV4 addresses where each address is in the   |
// |                                             |     | format 'd.d.d.d' where each 'd' is a valid decimal number.   |
// |                                             |     | The number of entries must equal DEFAULT_ROUTE_SRC_MAC       |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_DEFAULT_SRC_PORT          | [*] | A list of source ports typically not including 0 but note    |
// |                                             |     | these ports are pseudo. See 'packet_design.md' for more info.|
// |                                             |     | The number of entries must equal DEFAULT_ROUTE_SRC_MAC       |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_DEFAULT_DST_PORT          | [*] | A list of dest ports typically not including 0 but note      |
// |                                             |     | these ports are pseudo. See 'packet_design.md' for more info.|
// |                                             |     | The number of entries must equal DEFAULT_ROUTE_SRC_MAC       |
// +---------------------------------------------+-----+--------------------------------------------------------------+
//
// [*] Application supplied config reflecting limits and capabilities in [1,2,3]
// [1] As provided by 'scripts/nic_inventory_config'
// [2] As provided by 'scripts/cpu_inventory_config'
// [3] As provided by 'scripts/ram_inventory_config'

#include <dpdk/reinvent_dpdk_rxq.h>                                                                                     
#include <dpdk/reinvent_dpdk_txq.h>                                                                                     
#include <dpdk/reinvent_dpdk_vcpu.h>                                                                                     
#include <dpdk/reinvent_dpdk_lcore.h>                                                                                     
#include <dpdk/reinvent_dpdk_config.h>                                                                                     
#include <dpdk/reinvent_dpdk_ipv4route.h>
#include <util/reinvent_util_environment.h>                                                                             

namespace Reinvent {
namespace Dpdk {

class Init {
  // PRIVATE MANIULATORS
  static int convertRssKey(const std::string& key, uint8_t *keyBytes);

  static int configRings(const std::string& prefix, Util::Environment *env, Config *config,
    std::vector<int>& rxqRingSize, std::vector<int>& txqRingSize);

  static int configSharedMempool(const std::string& prefix, Util::Environment *env, int *sharedMemPoolSize,
    int *sharedMempoolEltSize, int *sharedMempoolCacheSize, int *sharedMempoolPrivateSize, int *sharedMempoolFlags);

  static int configPerQueueMempool(const std::string& prefix, Util::Environment *env, Config *config,
    std::vector<int> *rxqMemPoolSize, std::vector<int> *txqMemPoolSize, std::vector<int> *rxqMemPoolCacheSize,
    std::vector<int> *txqMemPoolCacheSize, std::vector<int> *rxqMemPoolPrivSize, std::vector<int> *txqMemPoolPrivSize,
    std::vector<int> *rxqMemPoolDataRoomSize, std::vector<int> *txqMemPoolDataRoomSize);

  static int makeAssignment(Config *config, std::vector<RXQ>& rxq, std::vector<TXQ>& txq,
    std::vector<LCORE>& lcore);

  static int makeDpdkInitParams(const std::string& envPrefix, Util::Environment *env, Config *config);

  static int configDefaultRouting(const std::string& prefix, Util::Environment *env,
    std::vector<IPV4Route> *defaultTxRoute);

  static int configStaticUdpPortFlowControl(const std::string& prefix, Util::Environment *env, unsigned rxqs,        
    std::vector<unsigned> *staticUdpPortFlowControlQueue, std::vector<unsigned> *staticUdpPortFlowControlBitMask);

  static int createStaticUdpDestPortFlowControl(int deviceId, const std::vector<unsigned>& queue,
    const std::vector<unsigned>& mask, std::vector<struct rte_flow*> *rule);

public:
  // PUBLIC MANIPULATORS
  static int startEna(const std::string& device, const std::string& envPrefix, Util::Environment *env,
    Config *config);
    // Return 0 if a DPDK ENA 'device' was initialized ready for UDP RXQ/TXQ work and non-zero otherwise based on a
    // configuration set in the UNIX enviroment read through specified 'env', optional non-empty 'envPrefix'. The
    // behavior is defined provided 'deviceId' not empty. If zero is returned, the configuration written into specified
    // 'config' is valid. Upon successful return callers should run 'rte_eal_mp_remote_launch()' to create and run the
    // configured lcores.

  static int stopEna(const Config& config);
    // Return 0 if the DPDK device in the specified 'config' is stopped, and non-zero otherwise. Note this method       
    // should be run before 'stopDpdk' below.
                                                                                                                        
  static int stopDpdk();
    // Return 0 if the DPDK ENA subsystem is shutdown, and non-zero otherwise. Call this method once only before exit 
    // even if 'startEna' returned an error. The behavior of DPDK is undefined once this method returns. Note: run
    // 'stopEna' before running this method.
};

} // Dpdk
} // Reinvent
