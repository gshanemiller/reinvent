# reinvent
Library: DPDK userspace library read/write UDP messages with congestion control

# Status
* Early-alpha: under heavy development. [Major limitations given here.](https://github.com/rodgarrison/reinvent/issues)
* See [AWS name issue](https://github.com/rodgarrison/reinvent/issues/23). As discussed there, no code in this library
depends on or requires AWS. References to AWS in types or class names need to replaced with a generic name

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
its structures, and working out what the code or config intends to accomplish. There are no magic constants in code
* Reinvent automatically configures lcores-to-HW-core assignment. Programmers set the desired lcore count in
environment variables. The library then works out RXQ/TXQ assignments from there.
* Reinvent provides a helper structure `AWSWorker` to make lcore startup straightforward
* Reinvent AWSConfig is streamable and is output in JSON format. Pipe into `python3 -m json.tool` to pretty print
* Reinvent provides a uniform structure to report errors: no ad hoc logging/assertions
* Unlike almost all DPDK application libraries, examples, or explainers on the web this repository is substantially
self-contained. The setup script takes a bare machine with nothing on it to being completely configured with DPDK,
and all Mellanox dependencies built and ready for use by this library. The `doc` subdirectory contains an explainer
on DPDK concepts and usage

# Test Hardware
* Tested on Equinix `c3.small.x86` bare metal with Mellanox Connect4-LX NICs
* Last test Novemeber 2022

# Getting Started
* [Read setup instructions](https://github.com/rodgarrison/reinvent/blob/main/doc/equinix_mellanox_setup.md)
* [Read about DPDK packet design for IPV4 UDP](https://github.com/rodgarrison/reinvent/blob/main/doc/.packet_design.md)
