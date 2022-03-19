# reinvent
Library: DPDK userspace library read/write UDP messages with congestion control

# Status
Early-alpha: code under heavy development. [Major limitations given here.](https://github.com/rodgarrison/reinvent/issues)

# Design Goals
This library intends to be a rewrite of [eRPC](https://github.com/erpc-io/eRPC) which was the motivation for this work.
[Refer to the eRPC techical paper for more information](https://www.usenix.org/system/files/nsdi19-kalia.pdf). eRPC 
(in its DPDK implementation) is a UDP only RPC library running over DPDK in userspace with congestion control to handle
UDP packet loss and flow control. The result is a highly efficient yet general purpose NIC I/O library.

# Current Reinvent features
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

# Getting Started
* [Read setup instructions](https://github.com/rodgarrison/reinvent/blob/main/doc/aws_ena_setup.md)
* [Read about DPDK packet design for IPV4 UDP](https://github.com/rodgarrison/reinvent/blob/main/doc/aws_ena_packet_design.md)

# Benchmarks
Frankly, the **first cut of code is terrible**. Consider sending data UDP with `ncat`. This code reads a file and sends
data IPV4 UDP between the same two machines as DPDK tests:

```
Ex. output from: ncat -4 -v -u 172.31.67.198 1234 < <filename>
------
Ncat: Version 7.50 ( https://nmap.org/ncat )
Ncat: Connected to 172.31.67.198:1234.
Ncat: 1 bytes sent, 0 bytes received in 0.01 seconds.
Ncat: Version 7.50 ( https://nmap.org/ncat )
Ncat: Connected to 172.31.67.198:1234.
Ncat: 64 bytes sent, 0 bytes received in 0.01 seconds.
Ncat: Version 7.50 ( https://nmap.org/ncat )
Ncat: Connected to 172.31.67.198:1234.
Ncat: 128 bytes sent, 0 bytes received in 0.00 seconds.
Ncat: Version 7.50 ( https://nmap.org/ncat )
Ncat: Connected to 172.31.67.198:1234.
Ncat: 512 bytes sent, 0 bytes received in 0.00 seconds.
Ncat: Version 7.50 ( https://nmap.org/ncat )
Ncat: Connected to 172.31.67.198:1234.
Ncat: 1024 bytes sent, 0 bytes received in 0.00 seconds.
Ncat: Version 7.50 ( https://nmap.org/ncat )
Ncat: Connected to 172.31.67.198:1234.
Ncat: 1048576 bytes sent, 0 bytes received in 0.01 seconds.
Ncat: Version 7.50 ( https://nmap.org/ncat )
Ncat: Connected to 172.31.67.198:1234.
Ncat: 52428800 bytes sent, 0 bytes received in 0.06 seconds.
Ncat: Version 7.50 ( https://nmap.org/ncat )
Ncat: Connected to 172.31.67.198:1234.
Ncat: 104857600 bytes sent, 0 bytes received in 0.12 seconds.
Ncat: Version 7.50 ( https://nmap.org/ncat )
Ncat: Connected to 172.31.67.198:1234.
Ncat: 524288000 bytes sent, 0 bytes received in 0.60 seconds.      <--- .81 Gb/sec
Ncat: Version 7.50 ( https://nmap.org/ncat )
Ncat: Connected to 172.31.67.198:1234.
Ncat: 1073741824 bytes sent, 0 bytes received in 1.23 seconds.     <--- .81 Gb/sec
```

In contrast the DPDK code is almost two orders of 10 slower. There are two caveats (to follow) which make it a bit worse:
The best rate is only `0.118042` falling to `0.039643` Gb/sec:

```
lcoreId: 00, txqIndex: 00: elsapsedNs: 43347, packetsQueued: 15, packetSizeBytes: 54, nsPerPkt: 2889.800000, bytesPerSec: 18686414.284726, gbPerSec: 0.017403
lcoreId: 00, txqIndex: 00: elsapsedNs: 63907, packetsQueued: 150, packetSizeBytes: 54, nsPerPkt: 426.046667, bytesPerSec: 126746678.767584, gbPerSec: 0.118042
lcoreId: 00, txqIndex: 00: elsapsedNs: 702288, packetsQueued: 1500, packetSizeBytes: 54, nsPerPkt: 468.192000, bytesPerSec: 115337297.518967, gbPerSec: 0.107416
lcoreId: 00, txqIndex: 00: elsapsedNs: 18049829, packetsQueued: 15000, packetSizeBytes: 54, nsPerPkt: 1203.321933, bytesPerSec: 44875771.399275, gbPerSec: 0.041794
lcoreId: 00, txqIndex: 00: elsapsedNs: 37171258, packetsQueued: 30000, packetSizeBytes: 54, nsPerPkt: 1239.041933, bytesPerSec: 43582060.096002, gbPerSec: 0.040589
lcoreId: 00, txqIndex: 00: elsapsedNs: 113252273, packetsQueued: 90000, packetSizeBytes: 54, nsPerPkt: 1258.358589, bytesPerSec: 42913045.992463, gbPerSec: 0.039966
lcoreId: 00, txqIndex: 00: elsapsedNs: 189744955, packetsQueued: 150000, packetSizeBytes: 54, nsPerPkt: 1264.966367, bytesPerSec: 42688882.031145, gbPerSec: 0.039757
lcoreId: 00, txqIndex: 00: elsapsedNs: 379995938, packetsQueued: 300000, packetSizeBytes: 54, nsPerPkt: 1266.653127, bytesPerSec: 42632034.661381, gbPerSec: 0.039704
lcoreId: 00, txqIndex: 00: elsapsedNs: 1142983532, packetsQueued: 900000, packetSizeBytes: 54, nsPerPkt: 1269.981702, bytesPerSec: 42520297.659022, gbPerSec: 0.039600
lcoreId: 00, txqIndex: 00: elsapsedNs: 1268608467, packetsQueued: 1000005, packetSizeBytes: 54, nsPerPkt: 1268.602124, bytesPerSec: 42566537.591933, gbPerSec: 0.039643
```

In other tests I have seen `nsPerPkt` go as low as `250` but typically is around `500` in the better cases. That's a long
time to PCI write one packet of 54 bytes to the NIC.

The caveats are:

* DPDK code has no disk I/O. ncat does
* Because of [this missing AWS driver API](https://github.com/amzn/amzn-drivers/issues/166), which will be fixed soon,
the TXQ code can't detect nor wait for the TXQ output queue to flush before stopping and taking stats. So, in general,
there will be some 10s or 100s of packets in the TXQ's output queue that are not flushed meaning the elapsed time is a
little low

**Amazon advertises 100Gbps for `c5n.metal` instances if ENA is enabled. At this point in time, the second cut of
performance testing may need to reflect improvements in one or all of the following:

* bigger ring size
* a different TXQ burst size
* see if Amazon AWS ENA drivers will support RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE. Right now it does not
* better TXQ threshhold management. Right now config uses default which programs in 0 for all threshhold values
* force-set a different link speed. The code is not able to determine the link speed; both `rte_eth_link_get_nowait`
and `rte_eth_link_get` report `0x00` which does not correspond to `RTE_ETH_SPEED_NUM_` define not `RTE_ETH_SPEED_NUM_NONE`.
* need to determine if ncat is somehow using multiple TXQs. The DPDK test uses one
