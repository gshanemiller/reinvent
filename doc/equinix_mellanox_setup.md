# Equinix Bare Metal https://console.equinix.com/

For general DPDK or NIC I/O development, Equinix provides arguably better hardware
and half the price compared to AWS. A c3.small.x86 server,

* 1x Xeon E-2278G 3.40Ghz 8-cores
* 2x 480Gb SSD
* 32Gb RAM
* 2x Mellanox Technologies MT27710 Family [ConnectX-4 Lx] NICs
* 2x Intel Corporation I210 Gigabit NICs

runs only $0.75/hr. Not only are Mellanox NICs more powerful than AWS virtual
NICs, they are plugged directly into the PCI bus. Two instances cost the same as
one AWS c5n. Now, to be fair, AWS was never going to be fully baremetal and c5n
CPUs are larger and more powerful. It has more memory. But for development purposes
I find Equinix a far better bang for the buck.

The purpose of this article to explain how to set up a pair of c3.small.x86 servers
for reinvent development and testing. In particular, this procedure makes one of the
available Mellanox NICs available for DPDK use.

# Expected Outcome

When the procedure below is followed each machine will be configured as follows:

1. The reinvent code will be built under `/root/Dev/reinvent/build`
2. DPDK will have been built and installed to `/root/local`
3. Root's `.bashrc` file will be updated with helper alias, env-vars, path,
and LD_LIBRARY_PATH settings
4. Machines will communicate over a private, virtual LAN. The procedure trivially
extends to any number of machines.
5. One of the Mellanox NICs per machine will be reserved for DPDK.
6. You'll exercise reinvent code to make a basic assessment of throughput.
7. Note DPDK tends to require root access only because some of the Linux system files
accessed at runtime have ACLs for root only. This is not a firm requirement, but fixing
this is not in scope here.

At any point in time go to https://console.equinix.com/ and delete/remove your machines
to terminate billing. You'll lose your work; AWS AMI like capability I believe is
possible in Equinix, but is not discussed in this document.

# Procedure Part 1 of 3
This will take approximately 30 mins per machine most of it building DPDK and
uploading one file.

1. You'll need an Equinix account and a form of payment. You will incur no charges
except for the machine time you reserve. Charges are per hour, and you can return
the machines to Equinix at will. You'll also need to setup your SSH keys. All this
is a one-time effort.
2. [Go to Equinix console page](https://console.equinix.com/). Click on the new
server button. Then click on the on-demand box.
3. In the next page choose your preferred data center. Locations are in the US,
Europe, Hong Kong, and others.
4. Click on the c3.small.x86 box selecting server size.
5. Choose Ubuntu 22.04 LTS O/S
6. Select or provision two machines. Name them, for example, 'client, server'.
7. Click on 'Deploy Now'. All machines will be equipped with all ssh keys you
provided in step 1.
8. Your screen will refresh after a momemt and display a two lines with your
servers and their public IP addresses. On the far right under the actions column
note a clock icon reading 3-o'clock. Wait for it to disappear meaning your boxes are
ready.
9. Login to one of the machines with `ssh -i <ssh-key-pem-file> root@<public-ip-address>`
using the public IP address from step 8. Make sure your machine has the four NIC
cards mentioned above. Very rarely you'll get a machine which does not:

```
$ lspci | grep Ether
01:00.0 Ethernet controller: Mellanox Technologies MT27710 Family [ConnectX-4 Lx]
01:00.1 Ethernet controller: Mellanox Technologies MT27710 Family [ConnectX-4 Lx]
03:00.0 Ethernet controller: Intel Corporation I210 Gigabit Network Connection (rev 03)
04:00.0 Ethernet controller: Intel Corporation I210 Gigabit Network Connection (rev 03)
```

Instructions 10-20 must repeated once for each machine:

10. [In your browser surf to the install script](https://github.com/rodgarrison/reinvent/blob/main/scripts/install)
11. Copy the script by clicking on github's copy button.
12. In your ssh login run the command: `cat > install` then paste in the script. Press CTRL-D.
13. Run command: `chmod 755 install`
14. Run command: `./install`
15. After `apt` installs required packages you may be prompted to restart services. Select all
services or none or even cancel. You'll need to reboot anyway later on.

After about 10 mins the script runs `vi` on the primary DPDK configuration. At the top of the file there's a
variable declaration for `apps` listing all the applications to be built in addition to the library. For most purposes
only `test-pmd` is needed. Building everything will quadruple time. DPDK uses LTO. It's recommended to keep only
`test-pmd` so the variable declaration reads:

```
apps = [
        'test-pmd',
]
```

16. Inside the VI editor on `app/meson.build` either immediately quit and exit with vi command `:wq` or change the `apps`
array to include only `test-pmd` then write the changes and quit with `:wq`. Once done the script will build and install
DPDK.
17. After about 10 minutes the script will prompt `you need to enable IOMMU`. Press ENTER to continue. VI is run on the
grub boot file.
18. In the editor change the `GRUB_CMDLINE_LINUX` line appending a space plus `iommu=1 intel_iommu=on'` so that it reads
`export GRUB_CMDLINE_LINUX='... iommu=1 intel_iommu=on'`
19. Save and exit by escaping and entering `:wq`
20. Installation part 1 is done. Press CTRL-C and proceed to part 2 next. You can defer reboot until end of part 2.

# Procedure Part 2 of 3
By default Equinix configures both Mellanox cards into a single bonded device. The purpose of this procedure is to break
the bond leaving one Mellanox NIC for DPDK. This will require about 10 mins per machine. Your public IP address will
not change and, other than reduced redundancy, there is no other effect including price.

1. In your browser go to: `https://console.equinix.com`.
2. From your machine list, locate the action column right most.
3. Click on the action icon reading as `...` and choose `Details & Manage`
4. In the screen which appears click on `Network` from column left-most on screen
5. Click on `Convert to Other Network Type` blue button.
6. Click on `Hybrid` then the `unbonded` radio button`.
7. Click on `Convert to Hybrid Network` blue button
8. Repeat steps 1-7 for your other machines

The next step is to connect all of the machines to the same VLAN. In Equinix a VLAN is private non-public virtual network.

9. In your browser click on the `IPs & Networks` drop down middle top of page then choose `Layer 2`
10. Click on `Add VLAN` blue button top right
11. Top right select the same data center you gave in step 3. Give a description, optional but recommended.
12. Assign a VNID number, also optional
13. Now also in your browser click on the `Servers` drop down to bring back the machine list.
14. On your first machine click on actions `...` and choose `Details & Manage`
15. From the left hand side click on `Network` (not the IPs & Networks drop down).
16. Scroll to the bottom and click `Add New Vlan` blue button
17. Choose interface `eth1` and set the network to the VLAN you just created.
18. Click on `Add` blue button
19. Repeat steps 13-18 for your other machines. Always use the same VLAN and `eth1`
20. Note: when you come to step (22) you will NOT see eth1 in the inteface list. This is a point of confusion, but
the procedure will work. The isolated DPDK Mellanox card will be given on interface `enp1s0f1` instead

Now in the last sub-step you'll fix your NIC interfaces file in each machine.

**IMPORTANT NOTE** 99.9% of the time Equinix servers use `enp1s0f0/enp1s0f1` for the bonded Mellanox NICs you
just unbonded above. However, once in a while you may find `enp2s0f0/enp2s0f1`. If that's your situation the
procedure is essentially unchanged. Wherever you read `enp1s0f0` in the instructions replace with `enp2s0f0`
and ditto `enp2s0f1` for `enp1s0f1`. You'll need to keep this in mind for Part 3 below when you run the DPDK
code too.

21. ssh login to one of your machines
22. Run command: `vi /etc/network/interfaces`
23. Change the entry for `enp1s0f1` from:

```
auto enp1s0f1                                                                                                           
iface enp1s0f1 inet manual                                                                                              
    pre-up sleep 4                                                                                                      
    bond-master bond0
```

To:

```
# assign IP address 192.168.0.2 to enp1s0f1
auto enp1s0f1
iface enp1s0f1 inet static
    address 192.168.0.2/28
```

24. Near line 36 in the entry for `auto bond0` find the line reading `bond-slaves enp1s0f0 enp1s0f1`
and simply remove `enp1s0f1` so it reads `bond-slaves enp1s0f0`. Save and exit.
25. Now reboot your machine: run `reboot`
26. Repeat steps 21-25 for each of your other machines **EXCEPT** do not give the static address `192.168.0.2/28`.
Use the next IPV4 address e.g. `192.168.0.3/28` so each machine has a distinct IP address.

# Procedure Part 3 of 3
You will now exercise the reinvent test UDP program to make a basic assessment of throughput between two of the machines.
This will require about 10 minutes.

There are two small bits of configuration left. You'll need to plug-in the ethenet addresses and IP addresses of the
DPDK Mellanox cards. And you'll need to run a premade script to setup huge-pages. 

Pick two of your machines. Call one `client`. Call the other machine `server`.

1. Login to your client machine and run `ifconfig enp1s0f1`. This should record the 192.168.x.x address you configured
above. For example,

```
# client's enp1s0f1
$ ifconfig enp1s0f1
enp1s0f1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.0.2  netmask 255.255.255.240  broadcast 192.168.0.15.    <---- IP address 192.x.x.x
        inet6 fe80::e42:a1ff:fe97:faa5  prefixlen 64  scopeid 0x20<link>
        ether 0c:42:a1:97:fa:a5  txqueuelen 1000  (Ethernet)                  <---- MAC (ether) address here
        RX packets 7809  bytes 960533 (960.5 KB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 1165  bytes 143598 (143.5 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

2. Login to your server machine and run `ifconfig enp1s0f1`.
3. In your client machine run command: `cd /root/Dev/reinvent/scripts`
4. In your client machine run command: `vi reinvent_dpdk_udp_integration_test`
5. Near line 83 change `TEST_CLIENT_DPDK_NIC_DEVICE_TXQ_DEFAULT_SRC_MAC` to hold the ethernet address from your client's
enp1s0f1 device ex. `0c:42:a1:97:fa:a5`.
6. Near line 84 change `TEST_CLIENT_DPDK_NIC_DEVICE_TXQ_DEFAULT_DST_MAC` to hold the ethernet address from your server's
enp1s0f1 device.
7. Near line 85 change `TEST_CLIENT_DPDK_NIC_DEVICE_TXQ_DEFAULT_SRC_IP` to hold the IPV4 address from your client's
enp1s0f1 device ex. `192.168.0.2`
8. Near line 86 change `TEST_CLIENT_DPDK_NIC_DEVICE_TXQ_DEFAULT_DST_IP` to hold the IPV4 address from your server's
enp1s0f1 device.
9. Save and exit.
10. In the current working directory (still on client machine) run: `./setup`. This will configure huge pages.
11. On your server machine run command: `cd /root/Dev/reinvent/scripts` and then run `./setup`
12. On your server machine run command: `./reinvent_dpdk_udp_integration_test server perf`. You should see this output:

```
root@server:~/Dev/reinvent/scripts# ./reinvent_dpdk_udp_integration_test server perf
../build/reinvent_dpdk_udp_opt_integration_test.tsk -m server -p TEST_SERVER -P
EAL: Detected CPU lcores: 8
EAL: Detected NUMA nodes: 1
EAL: Detected shared linkage of DPDK
EAL: Selected IOVA mode 'VA'
EAL: No free 2048 kB hugepages reported on node 0
EAL: VFIO support initialized
EAL: Probe PCI driver: mlx5_pci (15b3:1015) device: 0000:01:00.1 (socket 0)
mlx5_net: No available register for sampler.
TELEMETRY: No legacy callbacks, legacy socket not created
lcoreId 01 rxqIndex 01 listening for packets
lcoreId 02 rxqIndex 02 listening for packets
lcoreId 03 rxqIndex 03 listening for packets
lcoreId 04 rxqIndex 04 listening for packets
lcoreId 05 rxqIndex 05 listening for packets
lcoreId 06 rxqIndex 06 listening for packets
lcoreId 07 rxqIndex 07 listening for packets
lcoreId 00 rxqIndex 00 listening for packets
```

13. On your client machine run command: `./reinvent_dpdk_udp_integration_test client perf`
14. The client will exit to the shell when done; to terminate the server press CTRL-C at any time.

See next section for discussion of ouput.

# RSS Benchmark Output #1

The default configuration runs 1 TXQ at client and 8 RXQs with RSS on the server. The client output will resemble the following:

```
# ./reinvent_dpdk_udp_integration_test client perf
../build/reinvent_dpdk_udp_opt_integration_test.tsk -m client -p TEST_CLIENT -P
EAL: Detected CPU lcores: 8
EAL: Detected NUMA nodes: 1
EAL: Detected shared linkage of DPDK
EAL: Selected IOVA mode 'VA'
EAL: No free 2048 kB hugepages reported on node 0
EAL: VFIO support initialized
EAL: Probe PCI driver: mlx5_pci (15b3:1015) device: 0000:01:00.1 (socket 0)
mlx5_net: No available register for sampler.
TELEMETRY: No legacy callbacks, legacy socket not created
lcoreId: 00, txqIndex: 00: elsapsedNs: 557813876, packetsQueued: 5000000, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 8963563.323046, nsPerPkt: 111.562775, bytesPerSec: 663303685.905440, mbPerSec: 632.575689, mbPerSecPayloadOnly: 273.546244 stalledTx 0
linkRate value: 10000
in  packets : 0
out packets : 5000000
in  bytes   : 0
out bytes   : 370000000
missed pkts : 0
in err pkts : 0
out err pkts: 0
rx allc errs: 0
rx_good_packets                 : 0
tx_good_packets                 : 5000000
rx_good_bytes                   : 0
tx_good_bytes                   : 370000000
rx_missed_errors                : 0
rx_errors                       : 0
tx_errors                       : 0
rx_mbuf_allocation_errors       : 0
tx_q0_packets                   : 5000000
tx_q0_bytes                     : 370000000
rx_wqe_errors                   : 0
rx_unicast_packets              : 0
rx_unicast_bytes                : 0
tx_unicast_packets              : 5000000
tx_unicast_bytes                : 370000000
rx_multicast_packets            : 0
rx_multicast_bytes              : 0
tx_multicast_packets            : 0
tx_multicast_bytes              : 0
rx_broadcast_packets            : 0
rx_broadcast_bytes              : 0
tx_broadcast_packets            : 0
tx_broadcast_bytes              : 0
tx_phy_packets                  : 5000000
rx_phy_packets                  : 0
rx_phy_crc_errors               : 0
tx_phy_bytes                    : 390000000
rx_phy_bytes                    : 0
rx_phy_in_range_len_errors      : 0
rx_phy_symbol_errors            : 0
rx_phy_discard_packets          : 0
tx_phy_discard_packets          : 0
tx_phy_errors                   : 0
rx_out_of_buffer                : 0
tx_pp_missed_interrupt_errors   : 0
tx_pp_rearm_queue_errors        : 0
tx_pp_clock_queue_errors        : 0
tx_pp_timestamp_past_errors     : 0
tx_pp_timestamp_future_errors   : 0
tx_pp_jitter                    : 0
tx_pp_wander                    : 0
tx_pp_sync_lost                 : 0
```
The bottom half pretty-prints statistics DPDK keeps on the NIC printed at exit. Our focus is this key metric:

```
lcoreId: 00, txqIndex: 00: elsapsedNs: 557813876, packetsQueued: 5000000, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 8963563.323046, nsPerPkt: 111.562775, bytesPerSec: 663303685.905440, mbPerSec: 632.575689, mbPerSecPayloadOnly: 273.546244 stalledTx 0
```

5,000,000 packets 74 bytes/each (32 bytes/ea payload) where queued and sent needing 557813876 nanoseconds. That works out to some 8,963,563 packets per second or 111.6 ns/packet. The last metric `stalledTx` is the number of times an attempt to enqueue and send a packet failed because HW resources were full or busy. 0 means this never happened.

On the server (receiver side) we see the following output:

```
# ./reinvent_dpdk_udp_integration_test server perf
../build/reinvent_dpdk_udp_opt_integration_test.tsk -m server -p TEST_SERVER -P
EAL: Detected CPU lcores: 8
EAL: Detected NUMA nodes: 1
EAL: Detected shared linkage of DPDK
EAL: Selected IOVA mode 'VA'
EAL: No free 2048 kB hugepages reported on node 0
EAL: VFIO support initialized
EAL: Probe PCI driver: mlx5_pci (15b3:1015) device: 0000:01:00.1 (socket 0)
mlx5_net: No available register for sampler.
TELEMETRY: No legacy callbacks, legacy socket not created
lcoreId 01 rxqIndex 01 listening for packets
lcoreId 02 rxqIndex 02 listening for packets
lcoreId 03 rxqIndex 03 listening for packets
lcoreId 04 rxqIndex 04 listening for packets
lcoreId 05 rxqIndex 05 listening for packets
lcoreId 06 rxqIndex 06 listening for packets
lcoreId 07 rxqIndex 07 listening for packets
lcoreId 00 rxqIndex 00 listening for packets
lcoreId: 02, rxqIndex: 02: elsapsedNs: 7905058420, packetsDequeued: 100002, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 12650.380894, nsPerPkt: 79049.003220, bytesPerSec: 936128.186134, mbPerSec: 0.892761, mbPerSecPayloadOnly: 0.386059, stalledRx: 251154981
.
 .
  .
lcoreId: 00, rxqIndex: 00: elsapsedNs: 83092236, packetsDequeued: 100001, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 1203493.910069, nsPerPkt: 830.914051, bytesPerSec: 89058549.345092, mbPerSec: 84.932851, mbPerSecPayloadOnly: 36.727719, stalledRx: 2183622

lcoreId: 01, rxqIndex: 01: elsapsedNs: 83201758, packetsDequeued: 100002, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 1201921.719010, nsPerPkt: 832.000940, bytesPerSec: 88942207.206728, mbPerSec: 84.821899, mbPerSecPayloadOnly: 36.679740, stalledRx: 2149252

lcoreId: 02, rxqIndex: 02: elsapsedNs: 83117863, packetsDequeued: 100000, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 1203110.816263, nsPerPkt: 831.178630, bytesPerSec: 89030200.403492, mbPerSec: 84.905816, mbPerSecPayloadOnly: 36.716028, stalledRx: 2150613

lcoreId: 03, rxqIndex: 03: elsapsedNs: 83174204, packetsDequeued: 100001, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 1202307.869397, nsPerPkt: 831.733723, bytesPerSec: 88970782.335350, mbPerSec: 84.849150, mbPerSecPayloadOnly: 36.691524, stalledRx: 2149664

lcoreId: 04, rxqIndex: 04: elsapsedNs: 83216152, packetsDequeued: 100001, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 1201701.804236, nsPerPkt: 832.153198, bytesPerSec: 88925933.513484, mbPerSec: 84.806379, mbPerSecPayloadOnly: 36.673029, stalledRx: 2151479

lcoreId: 05, rxqIndex: 05: elsapsedNs: 83021039, packetsDequeued: 100001, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 1204525.999729, nsPerPkt: 830.202088, bytesPerSec: 89134923.979932, mbPerSec: 85.005688, mbPerSecPayloadOnly: 36.759216, stalledRx: 2154651

lcoreId: 06, rxqIndex: 06: elsapsedNs: 82933965, packetsDequeued: 100001, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 1205790.655252, nsPerPkt: 829.331357, bytesPerSec: 89228508.488651, mbPerSec: 85.094937, mbPerSecPayloadOnly: 36.797811, stalledRx: 2152868

lcoreId: 07, rxqIndex: 07: elsapsedNs: 83232336, packetsDequeued: 100000, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 1201456.126379, nsPerPkt: 832.323360, bytesPerSec: 88907753.352015, mbPerSec: 84.789041, mbPerSecPayloadOnly: 36.665531, stalledRx: 2144852
```

Every 100000 (approximately) packets each RXQ receives, it dumps a performance line. RSS spreads the 5,000,0000 packets 
sent by the client over the 8 RXQs here. Therefore each RXQ lcore will provide several output lines. Especially if there's
a sizeable delay between when you ran the server then started the client, you should ignore the first set of reports. The
elapsed time per queue will include that delay. Instead look at the bottom or last set, which I've provide above only
sorted by lcore and printed for readability.

Consider the last entry `lcoreId 07`: it took 83232336 to receive the last round of 100,000 packets e.g. the elapsed
time from the 1st packet to the 100,000th packet. This works out to approximately 1,201,456 packets/sec or 832 ns/packet.
However, each RXQ does about the same work over the same interval. Aggregating we get: `8*1201456.126379 = ~9,611,649`
packets/sec aggregate receive rate. The `stalledRx` metric counts the number of times the RXQ lcore tried to read a
packet but none was available. These numbers average around 2.1 million per 100,000 packets received. That issue is
further discussed below. See CPU usage.

Note these rates are far higher than what the kernel can do. A [CloudFare Technical Article (2015)](https://blog.cloudflare.com/how-to-receive-a-million-packets/) shows Linux can only do some 1,200,000 pps using more CPU resources. DPDK is faster and efficient.

Once you press CTRL-C at the server, the program will again dump DPDK collected NIC statistics. Here you'll clearly see
the per RXQ work done. Note your output will differ; I ran the client several times with different configs:

```
^ClinkRate value: 10000
in  packets : 8999864
out packets : 0
in  bytes   : 665989936
out bytes   : 0
missed pkts : 0
in err pkts : 0
out err pkts: 0
rx allc errs: 0
rx_good_packets                 : 8999864
tx_good_packets                 : 0
rx_good_bytes                   : 665989936
tx_good_bytes                   : 0
rx_missed_errors                : 0
rx_errors                       : 0
tx_errors                       : 0
rx_mbuf_allocation_errors       : 0
rx_q0_packets                   : 1124976
rx_q0_bytes                     : 83248224
rx_q0_errors                    : 0
rx_q1_packets                   : 1124989
rx_q1_bytes                     : 83249186
rx_q1_errors                    : 0
rx_q2_packets                   : 1124989
rx_q2_bytes                     : 83249186
rx_q2_errors                    : 0
rx_q3_packets                   : 1124981
rx_q3_bytes                     : 83248594
rx_q3_errors                    : 0
rx_q4_packets                   : 1124988
rx_q4_bytes                     : 83249112
rx_q4_errors                    : 0
rx_q5_packets                   : 1124976
rx_q5_bytes                     : 83248224
rx_q5_errors                    : 0
rx_q6_packets                   : 1124976
rx_q6_bytes                     : 83248224
rx_q6_errors                    : 0
rx_q7_packets                   : 1124989
rx_q7_bytes                     : 83249186
rx_q7_errors                    : 0
rx_wqe_errors                   : 0
rx_unicast_packets              : 8999864
rx_unicast_bytes                : 665989936
tx_unicast_packets              : 0
tx_unicast_bytes                : 0
rx_multicast_packets            : 33
rx_multicast_bytes              : 3927
tx_multicast_packets            : 0
tx_multicast_bytes              : 0
rx_broadcast_packets            : 0
rx_broadcast_bytes              : 0
tx_broadcast_packets            : 0
tx_broadcast_bytes              : 0
tx_phy_packets                  : 0
rx_phy_packets                  : 9000033
rx_phy_crc_errors               : 0
tx_phy_bytes                    : 0
rx_phy_bytes                    : 702004059
rx_phy_in_range_len_errors      : 0
rx_phy_symbol_errors            : 0
rx_phy_discard_packets          : 0
tx_phy_discard_packets          : 0
tx_phy_errors                   : 0
rx_out_of_buffer                : 0
tx_pp_missed_interrupt_errors   : 0
tx_pp_rearm_queue_errors        : 0
tx_pp_clock_queue_errors        : 0
tx_pp_timestamp_past_errors     : 0
tx_pp_timestamp_future_errors   : 0
tx_pp_jitter                    : 0
tx_pp_wander                    : 0
tx_pp_sync_lost                 : 0
```

# Benchmark Output #2

In this example run, we'll modify the server to run only 1 RXQ leaving the client the same. Using `vi` edit 
`./reinvent_dpdk_udp_integration_test` on the server box completing the following changes. Do not change the
client. The lines below show what should appear in the file **after edit**. All other lines should remain
absolutely unchanged. Note you may comment out (shown) or remove entirely the RSS config lines: 

```
export TEST_SERVER_DPDK_NIC_DEVICE_MEMPOOL_RXQ_SIZE="2048"
export TEST_SERVER_DPDK_NIC_DEVICE_MEMPOOL_RXQ_CACHE_SIZE="0"
export TEST_SERVER_DPDK_NIC_DEVICE_MEMPOOL_RXQ_PRIV_SIZE="0"
export TEST_SERVER_DPDK_NIC_DEVICE_MEMPOOL_RXQ_DATA_ROOM_SIZE="2048"
export TEST_SERVER_DPDK_NIC_DEVICE_RX_QUEUE_SIZE="1"
export TEST_SERVER_DPDK_NIC_DEVICE_RX_MQ_MASK="0"
export TEST_SERVER_DPDK_NIC_DEVICE_RXQ_THREAD_COUNT="1"
# export TEST_SERVER_DPDK_NIC_DEVICE_RX_RSS_KEY="6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:6d:5a:"
# export TEST_SERVER_DPDK_NIC_DEVICE_RX_RSS_HF="41868"
```

Rerunning the same procedure the server only runs one RXQ. In my session here's the last line output:

```
lcoreId: 00, rxqIndex: 00: elsapsedNs: 10386902, packetsDequeued: 100002, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 9627702.273498, nsPerPkt: 103.866943, bytesPerSec: 712449968.238845, mbPerSec: 679.445236, mbPerSecPayloadOnly: 293.814156, stalledRx: 283155
```

This output is pretty much 1:1 with the client. Note also the pps is much higher compared to the RSS case. This isn't because RSS is slow; 
it's done in HW. This is an artifact of a single queue receive side. More packets per unit time arrive at RXQ0 so it takes less time to get 
100,000 packets needed for the reporting interval. The rapid receipt of packets is also why the `statlledRx` figure is about 10x less. 
Nonetheless, the aggregate RSS receive rates matches single queue receive rate.
