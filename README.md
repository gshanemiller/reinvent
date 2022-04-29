# reinvent
Library: DPDK userspace library read/write UDP messages with congestion control

# Status
Early-alpha: code under heavy development. [Major limitations given here.](https://github.com/rodgarrison/reinvent/issues)

# Design Goals
This library intends to be a rewrite of [eRPC](https://github.com/erpc-io/eRPC) which was the motivation for this work.
[Refer to the eRPC techical paper for more information](https://www.usenix.org/system/files/nsdi19-kalia.pdf). eRPC 
(in its DPDK implementation) is a UDP only RPC library running over DPDK in userspace with congestion control to handle
UDP packet loss and flow control. The result is a highly efficient yet general purpose NIC I/O library.

# Reinvent features
* Decently documented
* Ships with a working IPV4 UDP TX/RX example
* Shows ENA checksum offload for IPV4, UDP checksums
* No-copy packet preparation
* DPDK configuration including UDP IPV4 routing information is defined outside code as enviromment variables ala
12-factor. For beginners it's far easier to understand configuration because readers are not dragged into DPDK code,
its structures, and working out what the code or config intends to accomplish
* Reinvent automatically configures lcores-to-HW-core assignment. Programmers set the desired lcore count in
environment variables. The library then works out RXQ/TXQ assignments from there.
* Reinvent provides a helper structure `AWSWorker` to make lcore startup straightforward
* Reinvent AWSConfig is streamable and is output in JSON format. Pipe into `python3 -m json.tool` to pretty print
* Reinvent provides a uniform structure to report errors: no ad hoc logging/assertions
* Tested on AWS `c5n` bare metal instances running AWS ENA NICs
* Tested on Equinix `c3.small.x86` bare metal boxes running Mellanox NICs

# Experimental Support
* Micro benchmarking using Intel PMU programmable counters
* [See](https://github.com/rodgarrison/rdpmc) and [this example](https://github.com/rodgarrison/reinvent/blob/dev/performance_tests/reinvent_perf_test1/test1.cpp#L268)
and [reinvent's reorganization of rdpmc here](https://github.com/rodgarrison/reinvent/blob/dev/src/reinvent/perf/reinvent_perf_rdpmc.h)

# Getting Started
* [Read AWS setup instructions](https://github.com/rodgarrison/reinvent/blob/main/doc/aws_ena_setup.md)
* [Read about DPDK packet design for IPV4 UDP](https://github.com/rodgarrison/reinvent/blob/main/doc/aws_ena_packet_design.md). This document applies pretty much to all DPDPDK development regardless of NIC.
* [Read Equinix setup instructions](https://github.com/rodgarrison/reinvent/blob/ubuntu/doc/equinix_mellanox_setup.md)

# AWS Benchmarks
(RX stats not given - will follow shorty)

An AWS ENA NIC is not a physical HW device in the `c5n.metal` instance tested here. I am unclear therefore what kind of
performance hit this imposes relative to the normal case where the NIC is a bonafide NIC card plugged into the HW's PCI
bus.

After playing with the code for a long time, I learned there are not a lot of knobs to turn. Some of the items that
appear to have zero effect on TX performance include:

* queue thresholds. Even though reinvent faithly programs in threshhold values into TX/RX queues as per Amazon they
are not used. See [this closed question](https://github.com/amzn/amzn-drivers/issues/213)
* whether or not 1Gb or 2Mb huge pages are used
* whether or not the dataroom size is as small as possible to fit the packet size sent or wastefully large. For example,
dataroom size of 1K or 2K is not worse that the minimum size DPDK will accept when, in these tests, it only sends 72
byte packets
* whether or not the total memory pool size is overly big or just big enough e.g. if sending 10,000 packets the pool
size can be set to 10,000 as opposed to some much larger size
* whether or not the memory pool does per-lcore caching. [Refer to this documentation](https://doc.dpdk.org/api/rte__mempool_8h.html#a503f2f889043a48ca9995878846db2fd)
where it says "The access to the per-lcore table is of course faster than the multi-producer/consumer pool."
* TX bursting: preparing several packets in a batch then enqueuing them into the TXQ's ring seems to offer no major
benefit over one at a time. Put another way the constant overhead in `rte_eth_tx_burst` does not seem big.

**The number one contraint on performance is TXQ-ring full**. In these log results we program 1 TXQ using the maximum TXQ
ring size of 1024. Each call to `rte_eth_tx_burst` provides 1 packet. We start sending 1000 packets of 74 bytes total
including 32-bytes of application payload. As the number of packets sent crosses 1024 the TXQ ring will become full as
it attempts to flush outstanding packets. Accordingly subsequent calls to enqueue more TX packets via `rte_eth_tx_burst`
does not return 1; it returns 0. Each time 0 is returned a counter is incremented, and the code re-runs `rte_eth_tx_burst`
until DPDK reports 1. This TXQ-ring stalling will decrease performance from around 415 ns/pkt to 1150 ns/pkt:

```
# this has best performance since TXQ ring never gets full. pps means packets per second.
# Note stalledTx at end of log line is zero. **Important** this stat captures packets
# enqueued not necessarily written onto the wire or received. But it makes clear that TXQ
# ring size (currently max of 1024) is a primary performance limitation:
lcoreId: 00, txqIndex: 00: elsapsedNs: 414640, packetsQueued: 1000, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 2411730.657920, nsPerPkt: 414.640000, bytesPerSec: 178468068.686089, mbPerSec: 170.200413, mbPerSecPayloadOnly: 73.600179 stalledTx 0

# In all of the following cases the TXQ ring gets full. Note stalledTx at end of log line.
# For example, in the last run when 10,000 packets were sent it took around 309000 calls to
# rte_eth_tx_burst to flush those 10000 packets out:
lcoreId: 00, txqIndex: 00: elsapsedNs: 1351623, packetsQueued: 2000, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 1479702.550193, nsPerPkt: 675.811500, bytesPerSec: 109497988.714309, mbPerSec: 104.425420, mbPerSecPayloadOnly: 45.156938 stalledTx 23896
lcoreId: 00, txqIndex: 00: elsapsedNs: 3878514, packetsQueued: 4000, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 1031322.821060, nsPerPkt: 969.628500, bytesPerSec: 76317888.758427, mbPerSec: 72.782410, mbPerSecPayloadOnly: 31.473475 stalledTx 74911
lcoreId: 00, txqIndex: 00: elsapsedNs: 6369604, packetsQueued: 6000, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 941973.786753, nsPerPkt: 1061.600667, bytesPerSec: 69706060.219756, mbPerSec: 66.476879, mbPerSecPayloadOnly: 28.746759 stalledTx 169046
lcoreId: 00, txqIndex: 00: elsapsedNs: 8852744, packetsQueued: 8000, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 903674.612075, nsPerPkt: 1106.593000, bytesPerSec: 66871921.293556, mbPerSec: 63.774034, mbPerSecPayloadOnly: 27.577961 stalledTx 244140
lcoreId: 00, txqIndex: 00: elsapsedNs: 11436274, packetsQueued: 10000, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 874410.669069, nsPerPkt: 1143.627400, bytesPerSec: 64706389.511129, mbPerSec: 61.708822, mbPerSecPayloadOnly: 26.684896 stalledTx 309042
```

For comparison the best **kernel based performance** known to this author comes from [Cloudfare](https://blog.cloudflare.com/how-to-receive-a-million-packets/).
This demonstrates UDP code sending 72 byte packets which includes 32 bytes of payload like this test. The maximum I was able
to achieve on the same HW was around 1.1 million packets/sec:

```
# on sender machine:
$ sudo taskset -c 1,2 ./udpsender 172.31.68.106:432

# on receiver machine:
taskset -c 1,2,3 ./udpreceiver1 0.0.0.0:4321 3 1
  1.101M pps  33.596MiB / 281.820Mb 
  1.103M pps  33.662MiB / 282.376Mb
  1.102M pps  33.637MiB / 282.170Mb
  1.103M pps  33.647MiB / 282.249Mb
  1.104M pps  33.684MiB / 282.563Mb <--- 1.104*1000000*32/1024/1024=33.6MiB/sec, 1.104*1000000*32*8/1000/1000=282.6 million bits/sec 
```

Cloudfare work uses 2 cores to TX packets and 3 cores to receive them. The Cloudfare sender uses one TXQ; the receive side
is also one RXQ. The Cloudfare work also uses a special feature [SO_REUSEPORT](https://lwn.net/Articles/542629/) to help
get these numbers.

In contrast this code uses 1 core to TX and 1 core to RX. The RX side seems to keep with the TX side fine. However, once the
TXQ gets full, DPDK performance on ENA NICs drops under Cloudfare's 1.1 million packets/sec to as little as 875,000 pkts/sec
granted on 40% of Cloudfare's 5 cores.

But because an AWS ENA NIC has 32-TX and 32-RX queues (and the queues are essentially share nothing) DPDK will be able
to scale-out and easily beat what traditional kernel based I/O can do.

**Caveat:** [Because of this Amazon Driver Issues](https://github.com/amzn/amzn-drivers/issues/166) which will be fixed soon,
this benchkark code is not able to determine when all TXQ packets were flushed before recording the stop time. As a result
the reported elapsed-time and packet rates will be faster that the real value since it does not account for the few packets
in the TXQ ring not actually written to the wire.

# Equinix Benchmarks
[Please see](https://github.com/rodgarrison/reinvent/blob/ubuntu/doc/equinix_mellanox_setup.md) last section. Mellanox PCI
cards are superior to AWS NICs without any of the issues discussed above.
