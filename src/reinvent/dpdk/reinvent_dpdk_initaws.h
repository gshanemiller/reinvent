#pragma once

// Purpose: Initialize DPDK device (aka port) and related RX/TX software objects
//
// Classes:
//  Dpdk::InitAWS: Configure one DPDK AWS ENA capable device culminating with a set of NUMA aligned lcores each
//                 configured for one RX queue, or one configured TX queue writing the final configuration set into
//                 caller specified 'Dpdk::AWSConfig'. Post initialization callers should run rte_eal_mp_remote_launch
//                 to start and run the RX/TX lcores usually with a helper 'Dpdk::AWSEnaWorker' object.
//
// Limitation:     Multi-NICs not supported. DPDK generally will not allow different PIDs to use the same DPDK device.
//                 So the typical situation is when a host box has multiple NICs and each task runs one NIC. This code
//                 will properly configure all those NICs except, because it can't know which lcores were created
//                 by those other tasks, cannot guarantee the assigned lcores are actually DISTINCT if a DISTINCT
//                 policy was used. To fix this lcore-to-HW-core assignments have shared between each participating
//                 task.
//
// See Also:       Dpdk::AWSConfig
//                 Dpdk::AWSEnaWorker
//                 integration_tests/reinvent_dpdk_udp minimal-complete client/server example
//                 Memory pool sizing documentation in doc/aws_ena_packet_design.md
//  
// Thread Safety: Not MT thread-safe.
//                                                                                                                      
// Exception Policy: No exceptions                                                                                      
//
// Description: DPDK device setup comes in two parts: initialization of the H/W, and reading H/W capabilities to which
// software resources are created and aligned to leverage that H/W. For example, if a NIC has 32 RXQs the software side
// can create up to 32 lcores with attendant mempools to perform RX. Thread/NIC-queue association is a major part of
// this work. Given the many facets of setup, each AWS DPDK supported device has its own initialize method below.
//
// Environment variables are the primary way to name and set out configurations. The names are constructed by chaining
// together constant suffixes with variable length prefixes. Providing a full application suite configuration is done
// using five mini strategies much like any moderately complex bash script. Programmers alway have the option to
// combine all configs for all machines in a single bash script of UNIX environment variables or keep seperate as
// desired:
//
// * Configurations can run over one or several NICs. So the first issue to make clear is which NIC a configuration is
//   for. Dpdk::InitAWS initializes one NIC writing the config into one 'Dpdk::AWSConfig' object. In this work the
//   programmer must focus on a single machine at a time. That machine may have one, two, or more NIC. Each NIC has
//   zero-based '{devId}' assigned to it by DPDK. Now, the first argument to InitAWS is the text name of the device
//   denoted by '{deviceName}' whose value is the device number written '{devId}'. From '{deviceName}' initialization
//   finds '{devId}'. With the NIC identified by name and devId rhe rest of the NIC's configs will have the form 
//   '{prefix}_{deviceId}_' if the prefix is non-empty, or just '{deviceId}_' if the prefix is empty. This string 
//   forms the stalk prefix by which all other configs are named. Prefix is discussed in point two, three.
// * For simple apps where there's only one machine, no prefix is used. Programmers will read off the fixed, constant
//   part of the enviromment variable name from the table below and provide it with a value. For example, setup must
//   know the DPDK's 0-based port number for a NIC: '{deviceId}=0' works fine.
// * Your application works over a fixed number of machines whose role yield simple names. The UDP client/server
//   provided in this library is a good example. Here we need two sets of configs one for the client running on one
//   machine and one for the server running on another machine. One can provide everything in the same bash file by
//   prefixing each config with 'CLIENT' or 'SERVER'. The client machine's DPDK device number is given by
//   'CLIENT_{deviceId}=0' (i.e. client machine configs DPDK device 0) and the server's 'SERVER_{deviceId}=2' (i.e.
//   the server machine config is for DPDK device 2).
// * You have a large but finite number of machines in your configuration where naming each machine is harder. So add a
//   number to the end of a stalk prefix. This longer prefix can be provided to 'InitAWS::ena'. On a set of 64 boxes,
//   for example, configure the NIC device numbers with prefixes 'PEER_0', 'PEER_1', 'PEER_2', and so on. This
//   naming scheme abstracts away from roles desingating each machine simply as a peer. Now a single bash script
//   config (or separate per machine if preferred) will provide 'PEER_0_{deviceId}=0', 'PEER_1_{deviceId}=1',
//   'PEER_2_{deviceId}=0'. The application task(s) running on the nth machine will take 'PEER_10' (here n=10) as
//   a prefix. Now 'Dpdk::InitAWS::ena' can find 'PEER_10_{deviceId}=...' plus the rest of 'PEER_10's config.
// * Some configs refer to multiple pieces of H/W on the same the same machine e.g. VCPU/lcores. Here again you can
//   use an optional prefix and print style combination to name and find these items. The UDP client/server example 
//   provided here gives 'TEST_CLIENT_VCPU_MAX=72' with prefix 'TEST_CLIENT' meaning there's 72 lcores/VCPUs on the
//   client machine. Each VCPU is appears like 'TEST_CLIENT_VCPU_1_IN_SOCKET=1' (VCPU 1 runs on H/W socket 1),
//   and so on up to the configured 72 machines. Since this kind of hardware is not on a NIC, no '{deviceId}' is not
//   needed or expected.
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
// which clearly demo those values. Footnotes are given under the table where these scripts can be found.
//
// The 'Env Variable Name' column gives the schema for the variable name. '{prefix}' refers to a programmer supplied
// prefix. '{prefix}' is given once, typically in 'main()' where it's passed into 'Dpdk::AWSInit::ena'. '{deviceId}'
// is the NIC's text name; see above. '{id}' refers to an integer typically an index. For example, the schema
// 'VCPU_{id}' implies the configs will read like 'VCPU_0, VCPU_1, VCPU_2' and so on up to the H/W maximum given by
// 'VCPU_MAX'.
//
// The last column provides a description. If the ENV variable merely holds a value that's passed on uniterpreted to
// an attribute in 'Dpdk::AwsConfig', that attribute name is given. If there is no direct relationship, the description
// references the attribitues in 'Dpdk::AwsConfig' influenced. Ultimately a AWS ENA config from the UNIX environment
// is held solely by an one 'Dpdk::AwsConfig' object.
//
// Even with this context, lcore count and lcore-to-queue assignment may remain difficult to extract from the configs
// alone. The following explanation for RX is analogous for TX. The maximum HW number supported RXQs is provided with 
// '{prefix}_{deviceId}_RX_QUEUE_SIZE'. If this number exceeds DPDK's assessment of the H/W capability at runtime, an
// error is reported and initialization stops. '{prefix}_{deviceId}_RXQ_THREAD_COUNT' provides the number of RX lcores
// to be created at runtime. '{prefix}_{deviceId}_RXQ_VCPU_POLICY' provides a policy on how lcore to HW core assignment
// is made. Once the lcore to HW core mapping is set, 'Dpdk::InitAWS' will assign to each lcore the roles and
// responsibility of RX for exactly one RXQ. So upon entering the main event-loop on a lcore it'll always be clear the
// assigned role: RX (or TX) and on what specific queue number. 
//
// The key number here is 'RXQ_THREAD_COUNT'. 'Dpdk::InitAWS' will try make 'RXQ_THREAD_COUNT' lcores each on its own
// HW core handling exactly one RXQ provided:
//
// * '{prefix}_{deviceId}_RXQ_THREAD_COUNT' <= '{prefix}_{deviceId}_RX_QUEUE_SIZE' i.e. you cannot run 128 RXQ lcores
//   if the NIC H/W only has 32 RXQs. 
// * If the number of HW cores on the NUMA node to which '{prefix}_{deviceId}' is attached is less than 
//   '{prefix}_{deviceId}_RXQ_THREAD_COUNT', one or lcores will have to double up on a HW core. You can't 10 lcores
//   for RX and TX queues if you only have 9 HW cores. One of the HW cores will have to run two lcores or you'll have
//   to configure fewer lcores to again achieve one lcore per HW core. Whether or not there can be HW sharing is
//   determined by '{prefix}_{deviceId}_RXQ_VCPU_POLICY'
//
// In sum the programmer will set out the number of lcores for RXQ desired. 'Dpdk::InitAWS' will try to achieve that
// goal assigning one lcore for one RXQ to its own HW core if possible. If that's not possible two more lcores will
// have to be assigned to the same HW core. Programmers should then set '{prefix}_{deviceId}_RXQ_VCPU_POLICY'
// to allow or disallow as desired.
//
// Example: A 'c5n' bare metal instance has 72 H/W cores of which 36 are on CPU socket 0. Socket 0 is attached to NUMA
// node 0. Suppose an ENA NIC is attached to NUMA node 0. Now an ENA NIC has 32 RXQs and 32 TXQs. It is not possible 
// to run 32 RXQ lcores and 32 TXQs lcores such that each lcore is on its own H/W core. That'd require 32+32=64 H/W
// cores on NUMA node 0. But there are only 36. Nevertheless the programmer provides 'RXQ_THREAD_COUNT=32' and also
// 'TXQ_THREAD_COUNT=32'. If either 'RXQ_VCPU_POLICY' or 'TXQ_VCPU_POLICY' is 'DISTINCT', 'Dpdk::InitAWS' will abort
// with an error as there are insufficient resources. If at least one policy is set 'MULTI' 'Dpdk::InitAWS' will assign
// each of the 32 RXQs each to its own lcore such that the lcore is on its own H/W core. Four of the TXQs will each
// get an lcore assigned to the remaining four unused H/W cores left on NUMA node 0. That leaves 28 TXQs unassigned.
// Those 28 TXQs each will get their lcore assigned to some of the H/W cores previously assigned to RXQs. In the end
// AWSConfig will have configured 64-lcores numbered or identified 0,1,2,...63. AWSConfig provides a map that takes an
// lcore to the VCPU it runs on. In addition, AWSConfig will have a RXQ/TXQ vector in which each queue is identified
// 0,1,2,...31 and ditto for the TXQs. AWSConfig also provides maps so you know to which lcore a given RX or TX queue
// corresponds, and from there, which VCPU it runs on. This way the individual components and their relationship to
// each other is set out in a straightforward way.
//
// Example: Consider again an 'c5n' bare metal instance with a ENA NIC on NUMA node 0. This time the programmer 
// determines 'RXQ_THREAD_COUNT=10' and 'TXQ_THREAD_COUNT=10' is sufficient to handle the work load. The RX/TXQ queue
// sizes remain set to 32. But one needs only 10 each of these 32. And since 10+10 is certainly less that that the 36
// H/W cores available on NUMA node 0 the RXQ and TXQ policy can be distinct. 'Dpdk::InitAWS' will than assign RXQ0
// through RXQ9 to their lcores each on its own H/W core and ditto TX. As a result RXQs 10 through 31 are unused;
// ditto TXQ.
//
// In this way the programmer picks lcore count which then picks which queues are used subject to the policy
// constraint. Initialization always works for maximum concurrency where possible. Put another way 'Dpdk::InitAWS'
// does not allow the configuration to fix RXQ or TXQs which then works backwards into thread or core count. The
// reverse applies. The design point is that so long as the programmer achieves the right number lcores assigned to
// the right H/W core complete with a clear assignment to a particular RXQ or TXQ ... that's good enough.
//
// Readers must be aware of DPDK's two indexing schemes. Recall the 'RXQ_THREAD_COUNT=10' and 'TXQ_THREAD_COUNT=10'
// example above. From the lcore perspective DPDK will refer to lcore 0, 1, 2, ..., 19 since there are 20 lcores
// running. The emphasis is not on what an lcore does, but rather on distinguishing one pinned pthread from another.
// The other numbering scheme is RXQ0, RXQ1, ..., RXQ9 and TXQ0, TXQ1, ..., TXQ9. This is an alternate way to refer to
// a lcore this time imbued with roles and responsibilities. Neither indexing scheme is unambiguos. Simply saying
// lcore9 does not say what it does, nor what HW core it's on. Saying TXQ5 is better but it doesn't tell you which
// lcore it is nor what HW core runs it. This is the reason 'Dpdk::AWSConfig' holds maps to relate one persective to
// another.
//
// Note: you can always dump a configuration to stdout: 'Dpdk::AWSConfig' supports 'operator<<(ostream&)'
//       the dump will show you how the enviroment was converted into an ENA config
// Note: you can always dump the Environment to stdout: 'Dpdk::Enviroment' supports 'operator<<(ostream)'.
//       the dump will show you all the ENV variables read with their values
//
// To avoid a lot of pointless duplication in documentation between the environment variables here, and the attributes
// into which they ultimatelay are written within 'Dpdk::AWSConfig', all documentation is provided here. Second, if a
// TX oriented value works likes its RX cousin, the TX variable is listed and grouped with its RX equivalent. However,
// the TX side of configuration is different or subtley different, it is listed and described separately. 
//
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | Env Variable Name    (DPDK-API related)     | Gen | Comment                                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_DPDK_INITIALIZATION_PARAMETERS       | [*] | Comma separated DPDK initialization arguments. Can be empty. |
// |                                             |     | Dpdk::InitAws calculates '--lcores, -n' arguments for you.   |
// |                                             |     | Usually you'll have to provide '--proc-type, --in-memory'    |
// |                                             |     | plus '--huge-dir'. '--log-level' is optional but typical. See|
// |                                             |     | integration examples for example configurations. There is no |
// |                                             |     | {prefix} or {deviceId} in this variable because there can    |
// |                                             |     | be one set of DPDK initialization parameters for a task no   |
// |                                             |     | the number of kinds of NICS configured.                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
//
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | Env Variable Name     (VCPU/DRAM related)   | Gen | Comment                                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_VCPU_MAX                           | [2] | The number of VCPUs machine supports. Individual VCPUs {id}s |
// |                                             |     | are named in range [0, MAX).                                 |
// |                                             |     | Effects AWSConfig::vcpu                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_VCPU_{id}                          | [2] | One of 'true|false'. 'true' means the VCPU {id} is considerd |
// |                                             |     | enabled for use at runtime; 'false' is soft-disabled         |
// |                                             |     | Effects AWSConfig::vcpu                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_VCPU_{id}_IN_CORE                  | [2] | Gives the 0 based core VCPU {id} runs on                     |
// |                                             |     | Effects AWSConfig::vcpu                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_VCPU_{id}_IN_SOCKET                | [2] | Gives the 0 based socket VCPU {id} runs on                   |
// |                                             |     | Effects AWSConfig::vcpu                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_VCPU_{id}_ON_NUMA_NODE             | [2] | Gives the 0 based NUMA node VCPU {id} runs on                |
// |                                             |     | Effects AWSConfig::vcpu                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_MEMORY_CHANNEL_COUNT               | [3] | Number of DRAM channels for host box                         |
// |                                             |     | Effects AWSConfig::memoryChannelCount. Critical for mempool  |
// |                                             |     | creation.
// +---------------------------------------------+-----+--------------------------------------------------------------+
//
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | Env Variable Name        (NIC related)      | Gen | Comment                                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId} = {devId}               | [*] | DPDK deviceId n>=0 effects Dpdk::AWSEnaConfig::deviceId.     |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_PCI_DEVICE_ID           | [1] | e.g. '0000:7f:00.0' for {deviceId}. Effects                  |
// |                                             |     | 'AWSEnaConfig::pciId                                         |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_NUMA_NODE               | [1] | The NUMA node to which DPDK device {deviceId} is attached. If|
//                                               |     | this number disagrees with DPDK's runtime report an error is |
// |                                             |     | is reported and init stops. Effects 'AWSConfig::numaNode'    |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SOFT_ENABLED            | [1] | One of 'true|false'. 'true' means '{deviceId}' is considered |
// |                                             |     | enabled for use at runtime; 'false' is soft-disabled. Usually|
// |                                             |     | set 'true', may be set false to soft-disable the H/W.        |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_RX_QUEUE_SIZE           | [1] | The maximum number of RX queues {deviceId} H/W supports.     |
// |                                             |     | Effects AWSConfig::rxqCount/txqCount. If this value is higher|
// | {prefix}_{deviceId}_TX_QUEUE_SIZE           |     | than DPDK's H/W capability report, init will stop with an    |
// |                                             |     | error. It may be used to set a soft upper limit for          |
// |                                             |     | 'QUEUE_SIZE'                                                 |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_RXQ_THREAD_COUNT        | [*] | '1<=N<={prefix}_{deviceId}_RX_QUEUE_SIZE' or 'QUEUE_SIZE'    |
// |                                             |     | Sets 'AWSEnaConfig::rxqThreadCount'. If set to 'QUEUE_SIZE'  |
// | {prefix}_{deviceId}_TXQ_THREAD_COUNT        |     | RXQ thread count is assigned to the number provided by env   |
// |                                             |     | '{prefix}_{deviceId}_RX_QUEUE_SIZE' and N otherwise.         |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_RXQ_VCPU_POLICY         | [*] | One of 'DISTINCT|MULTI|OFF'. DISTINCT means a distinct H/W   |
// |                                             |     | core is assigned to each RX lcore whose job it is to perform |
// | {prefix}_{deviceId}_TXQ_VCPU_POLICY         |     | RX on exactly one RXQ. The H/W core assigned cannot be used  |
// |                                             |     | by another RXQ or TXQ. MULTI allows two or more lcores to be |
// |                                             |     | assigned to the same H/W core regardless of purpose RX or TX.|
// |                                             |     | OFF means no resources for RX will be configured. Effects    |
// |                                             |     | AWSConfig::rxqPolicy, txqPolicy.                             |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_RXQ_VCPU_MASK           | [*] | A VCPU core mask which delimits which HW cores or VCPUs      |
// |                                             |     | can perform RX for device {deviceId}. Valid values is a int  |
// | {prefix}_{deviceId}_TXQ_VCPU_MASK           |     | list or 'ALL'. ALL means any HW core on the same NUMA node to|
// |                                             |     | which '{prefix}_{deviceId}' is attached is valid. A list with|
// |                                             |     | '1,5,10' means only VCPUs 1 or 5 or 10 are valid for RX work |
// |                                             |     | assignment. Each VCPU given must be on the same NUMA node as |
// |                                             |     | the NIC itself. Effects AWSConfig::rxqCandidateVcpuList and  |
// |                                             |     | txqCandidateVcpuList. Excluding VCPUs means DPDK resource    |
// |                                             |     | won't be assigned to it leaving those free for other uses.   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_{MTU}                   | [*] | Maximum packet size in bytes. Effects AWSConfig::mtu. Packet |
// |                                             |     | design TX side by respect this if RX will run over that data |
// +---------------------------------------------+-----+--------------------------------------------------------------+
//
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | Env Variable Name      (Memzone Related)    | Gen | Comment                                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMZONE_KB              | [*] | Memory reserved for all uses RX and TX in KB for the device  |
// |                                             |     | '{prefix}_{deviceId}'. Memory is allocated on the same NUMA  |
// |                                             |     | node the NIC is attached to. Effects AWSConfig attrs         |
// |                                             |     | AWSConfig::memzoneReserveKb                                  |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMZONE_MASK            | [*] | Hugepage mask for '{prefix}_{deviceId}_MEMZONE_KB'. See      |
// |                                             |     | https://doc.dpdk.org/api/rte__memzone_8h.html#a3ccbea77\     |
// |                                             |     | ccab608c6e683817a3eb170f and RTE_MEMZONE_xxx flags which help|
// |                                             |     | choose which TLB pages are used to satisfy request. In this  |
// |                                             |     | library one task has exactly one memzone for one named device|
// |                                             |     | Effects AWSConfig::memzoneMask                               |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMZONE_NAME            | [*] | Textual/human name '{prefix}_{deviceId}_MEMZONE_KB'          |
// |                                             |     | Effects AWSConfig::memzoneName                               |
// +---------------------------------------------+-----+--------------------------------------------------------------+
//
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | Env Variable Name (RXQ/TXQ Mempool Related) | Gen | Comment                                                      |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMPOOL_POLICY          | [*] | One of 'PER_QUEUE|SHARED'. The latter means all RXQ and TXQs |
// |                                             |     | will share the same pool. See 'doc/aws_ena_packet_design.md' |
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
// |                                             |     | Effects contents of AWSConfig::rxq and txq.                  |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMPOOL_RXQ_SIZE        | [*] | Integer list of number of elements (mbufs) per RXQ mempool   |
// |                                             |     | See 'doc/aws_ena_packet_design.md'                           |
// | {prefix}_{deviceId}_MEMPOOL_TXQ_SIZE        |     | Entry count must equal  RXQ_THREAD_COUNT/TXQ_THREAD_COUNT    |
// |                                             |     | Effects AWSConfig::rxq/txq                                   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMPOOL_RXQ_CACHE_SIZE  | [*] | Integer list of cache size in bytes per RXQ. For non-shared  |
// |                                             |     | 0 bytes/RXQ is typical. See 'doc/aws_ena_packet_design.md'   |
// | {prefix}_{deviceId}_MEMPOOL_TXQ_CACHE_SIZE  |     | Entry count equal to RXQ_THREAD_COUNT/TXQ_THREAD_COUNT       |
// |                                             |     | Effects AWSConfig::rxq/txq                                   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMPOOL_RXQ_PRIV_SIZE   | [*] | Integer list of private size in bytes per mbuf per RXQ often |
// |                                             |     | zero. See 'doc/aws_ena_packet_design.md'                     |
// |                                             |     |                                                              |
// | {prefix}_{deviceId}_MEMPOOL_TXQ_PRIV_SIZE   |     | Entry count equal to RXQ_THREAD_COUNT/TXQ_THREAD_COUNT       |
// |                                             |     | Effects AWSConfig::rxq/txq                                   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_MEMPOOL_RXQ_DATA_ROOM_\ | [*] | Integer list of data room size in bytes per mbuf per RXQ.    |
// |   SIZE                                      |     | Critical setting. See 'doc/aws_ena_packet_design.md'         |
// |                                             |     |                                                              |
// | {prefix}_{deviceId}_MEMPOOL_TXQ_DATA_ROOM_\ |     | Entry count equal to RXQ_THREAD_COUNT/TXQ_THREAD_COUNT       |
// |   SIZE                                      |     | Effects AWSConfig::rxq/txq                                   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SHARED_MEMPOOL_SIZE     | [*] | Number of mbufs (buffers) in shared mempool for all queues   |
// |                                             |     | uses. See 'doc/aws_ena_packet_design.md'                     |
// |                                             |     | Effects AWSconfig::rxq/txq                                   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SHARED_MEMPOOL_ELT_SIZE | [*] | Size of each mbuf (buffer) in bytes in shared mempool.       |
// |                                             |     | See 'doc/aws_ena_packet_design.md'                           |
// |                                             |     | Effects AWSconfig::rxq/txq                                   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SHARED_MEMPOOL_CACHE_\  | [*] | Size of per core cache in bytes for shared mempool.          |
// |   SIZE                                      |     | See 'doc/aws_ena_packet_design.md'                           |
// |                                             |     | Effects AWSconfig::rxq/txq                                   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SHARED_MEMPOOL_PRIVATE\_| [*] | Private data size in bytes of shared mempool.                |
// |   DATA_SIZE                                 |     | See 'doc/aws_ena_packet_design.md'                           |
// |                                             |     | Effects AWSconfig::rxq/txq                                   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_SHARED_MEMPOOL_FLAGS    | [*] | Flags refining behavior of per-core caching                  |
// |                                             |     | See 'doc/aws_ena_packet_design.md'                           |
// |                                             |     | Effects AWSconfig::rxq/txq                                   |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_RXQ_RING_SIZE           | [*] | Number of mbufs (buffers) per RXQ to reserve for NIC to write|
// |                                             |     | packets into after read off the wire. RXQs need mbufs at     | 
// |                                             |     | setup time. This number can differ from TXQ ring size next.  |
// |                                             |     | Either provide a single number in which case every RXQ will  |
// |                                             |     | use that number of mbufs or an int list giving the per RXQ   |
// |                                             |     | amount. If int-list should have RXQ_THREAD_COUNT entries     |
// |                                             |     | Effets AWSConfig::rxq                                        |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{deviceId}_TXQ_RING_SIZE           | [*] | Number of mbufs (buffers) per TXQ to reserve for NIC to read |
// |                                             |     | from which it then writes onto the wire. TX queues do not get|
// |                                             |     | memory at setup time; is provided after setup just prior to  |
// |                                             |     | entering the TX main-loop. Init will prealloc it so that it's|
// |                                             |     | ready then. TXQ ring size need not be the same as RXQ above. |
// |                                             |     | Either provide a single number in which case every TXQ will  |
// |                                             |     | use that number of mbufs or an int list giving the per TXQ.  |
// |                                             |     | If int-list should have TXQ_THREAD_COUNT entries. Effects    |
// |                                             |     | AWSConfig::txq.                                              |
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
// | {prefix}_{device}_TX_DEFAULT_ROUTE_SRC_MAC  | [*] | String list 1 mac address per TXQ                            |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_TX_DEFAULT_ROUTE_DST_MAC  | [*] | String list 1 mac address per TXQ                            |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_TX_DEFAULT_ROUTE_SRC_IP   | [*] | String list 1 mac address per TXQ                            |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_TX_DEFAULT_ROUTE_DST_IP   | [*] | String list 1 mac address per TXQ                            |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_TXQ_DEFAULT_SRC_PORT      | [*] | integer list 1 port per TXQ                                  |
// +---------------------------------------------+-----+--------------------------------------------------------------+
// | {prefix}_{device}_TXQ_DEFAULT_DST_PORT      | [*] | integer list 1 port per TXQ                                  |
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
#include <dpdk/reinvent_dpdk_awsconfig.h>                                                                                     
#include <util/reinvent_util_environment.h>                                                                             

namespace Reinvent {
namespace Dpdk {

class InitAWS {
  // PRIVATE MANIULATORS
  static int configRings(const std::string& prefix, Util::Environment *env, AWSEnaConfig *config,
    std::vector<int>& rxqRingSize, std::vector<int>& txqRingSize);

  static int configSharedMempool(const std::string& prefix, Util::Environment *env, int *sharedMemPoolSize,
    int *sharedMempoolEltSize, int *sharedMempoolCacheSize, int *sharedMempoolPrivateSize, int *sharedMempoolFlags);

  static int configPerQueueMempool(const std::string& prefix, Util::Environment *env, AWSEnaConfig *config,
    std::vector<int> *rxqMemPoolSize, std::vector<int> *txqMemPoolSize, std::vector<int> *rxqMemPoolCacheSize,
    std::vector<int> *txqMemPoolCacheSize, std::vector<int> *rxqMemPoolPrivSize, std::vector<int> *txqMemPoolPrivSize,
    std::vector<int> *rxqMemPoolDataRoomSize, std::vector<int> *txqMemPoolDataRoomSize);

  static int makeAssignment(AWSEnaConfig *config, std::vector<RXQ>& rxq, std::vector<TXQ>& txq,
    std::vector<LCORE>& lcore);

  static int makeDpdkInitParams(const std::string& envPrefix, Util::Environment *env, AWSEnaConfig *config);

  static int configTxRouting(const std::string& prefix, Util::Environment *env, AWSEnaConfig *config,
    std::vector<int>& defaultTxSrcPort, std::vector<int>& defaultTxDstPort, std::string& defaultTxSrcMac,
    std::vector<std::string>& defaultTxDstMac, std::string& defaultTxSrcIp, std::vector<std::string>& defaultTxDstIp,
    std::vector<UDPRoute>& defaultTxRoute);

public:
  // PUBLIC MANIPULATORS
  static int enaUdp(const std::string& device, const std::string& envPrefix, Util::Environment *env,
    AWSEnaConfig *config);
    // Return 0 if a AWS ENA 'device' was initialized ready for UDP RXQ/TXQ work and non-zero otherwise based on a
    // configuration set in the UNIX enviroment read through specified 'env', optional non-empty 'envPrefix'. The
    // behavior is defined provided 'deviceId' not empty. If zero is returned, the configuration written into specified
    // 'config' is valid. Upon successful return callers should run 'rte_eal_mp_remote_launch()' to create and run the
    // configured lcores.
};

} // Dpdk
} // Reinvent
