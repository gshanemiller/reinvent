# Equinix Bare Metal https://console.equinix.com/

For general DPDK or NIC I/O development, Equinix provides arguably better hardware
and half the price compared to AWS. A c3.small.x86 server,

* 1x Xeon E-2278G 3.40Ghz 8-cores
* 2x 480Gb SSD
* 32Gb RAM
* 2x Mellanox Technologies MT27710 Family [ConnectX-4 Lx] NICs
* 2x Intel Corporation I210 Gigabit NICs

runs only $0.50/hr. Not only are Mellanox NICs more powerful than AWS virtual
NICs, they are plugged directly into the PCI bus. Two instances cost less than
one AWS c5n. Now, to be fair, AWS was never going to be fully baremetal and c5n
CPUs are larger and more powerful. It has more memory. But for development purposes
I find Equinix a better bang for the buck.

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
5. Choose Ubuntu 20.04 LTS O/S
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

Instructions 10-26 must repeated once for each machine:

10. [In your browser surf to the install script](https://github.com/rodgarrison/reinvent/blob/ubuntu/scripts/install)
11. Copy the script by clicking on github's copy button.
12. In your ssh login run the command: `cat > install` then paste in the script. Press CTRL-D.
13. Run command: `chmod 755 install`
14. Run command: `./install`

At this point the script installs development requirements for DPDK, compiler. After
about one minute the script will stop reading:

```
Mellanox/Nvida MLNX_OFED_LINUX driver set cannot be obtained by curl/wget. You'll need to obtain it by hand
at https://network.nvidia.com/products/infiniband-drivers/linux/mlnx_ofed/ after accepting the EUA. Then scp it
into this machine into the /tmp directory:

example command: scp -i <ssh-key/pem-file> MLNX_OFED_LINUX-5.5-1.0.3.2-ubuntu20.04-x86_64.tgz root@<ip-address>:/tmp

scp .tgz file in by hand to /tmp then press ENTER:
```

Because of the EUA (end user agreement) Nvidia makes you accept you can't get to the `.tgz` file directly. Therefore
[you will have to surf to the MLNX download page](https://network.nvidia.com/products/infiniband-drivers/linux/mlnx_ofed/)
and download the required file to your local box. You'll `scp` it onto the Equinix machine yourself.

15. In your browser [go to Mellanox's OFED download page](https://network.nvidia.com/products/infiniband-drivers/linux/mlnx_ofed/)
16. At page bottom click on `Download`
17. Choose `Ubuntu` then then `Ubuntu 20.04` then `x86_64`
18. Click on and download the `.tgz` file. Do not choose the `iso` file. As of this writing the file is called
`MLNX_OFED_LINUX-5.5-1.0.3.2-ubuntu21.10-x86_64.tgz`.
19. Accept the EUA and download it.
20. After it's downloaded copy it from your local machine running the browser to the Equinix machine in step 9. The command
you'll use is: `scp -i <ssh-key/pem-file> MLNX_OFED_LINUX-5.5-1.0.3.2-ubuntu20.04-x86_64.tgz root@<public-ip-address>:/tmp`.
This will require 1-10 mins depending on your link speed. Note it must be copied to '/tmp'.
21. Once this is done return to the Equinix CLI and press ENTER. You'll be prompted to OK the install of Mellanox drivers.
Press 'y'.

Once step (21) is done, the script will download DPDK from githib. It'll then run `vi` on the main build-setup file. At
the top of the file there's a variable declaration for `apps` listing all the applications to be built in addition to
the library. For almost all purposes all but one of these applications is not needed. Building all applications will
double DPDK built time because DPDK uses LTO. It's recommanded to keep only `test-pmd` so the variable declaration reads:

```
apps = [
        'test-pmd',
]
```

22. Inside the VI editor on `app/meson.build` either immediately quit and exit with vi command `:wq` or change the `apps`
array to include only `test-pmd` then write the changes and quit with `:wq`. Once done the script will build and install
DPDK.
23. After about 10 minutes the script will prompt `you need to enable IOMMU`. Press ENTER to continue and run VI on the grub
boot file.
24. In the editor change the `GRUB_CMDLINE_LINUX` line adding a space plus `iommu=1 intel_iommu=on'` so that it reads
`export GRUB_CMDLINE_LINUX='console=tty0 console=ttyS1,115200n8 iommu=1 intel_iommu=on'`
25. Save and exit by escaping and entering `:wq`
26. Installation part 1 is done. Press ENTER to reboot. The Mellanox kernel driver set needs to initialize and run.

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
auto enp1s0f1
iface enp1s0f1 inet static
    address 192.168.0.2/28
```

24. Near line 36 in the entry for `auto bond0` find the line reading `bond-slaves enp1s0f0 enp1s0f1`
and simply remove `enp1s0f1` so it reads `bond-slaves enp1s0f0`. Save and exit.
25. If you have not rebooted yet do so now by running `reboot` in your shell. If you have rebooted you can simply
restart the networking subsystem with `systemctl restart networking`
26. Repeat steps 21-25 for each of your other machines **EXCEPT** do not give the static address `192.168.0.2/28`.
Use the next IPV4 address e.g. `192.168.0.3/28`.

# Procedure Part 3 of 3
You will now exercise the reinvent test UDP program to make a basic assessment of throughput between two of the machines.
This will require about 10 minutes.

There are two small bits of configuration left. You'll need to plug-in the ethenet addresses and IP addresses of the
DPDK Mellanox cards. And you'll need to run a premade script to setup huge-pages. 

Pick two of your machines. Call one `client` and call the other one `server`.

1. Login to your client machine and run `ifconfig enp1s0f1`. This should record the 192.168.x.x address you configured
above. For example,

```
# client's enp1s0f1
$ ifconfig enp1s0f1
enp1s0f1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.0.2  netmask 255.255.255.240  broadcast 192.168.0.15
        inet6 fe80::e42:a1ff:fe97:faa5  prefixlen 64  scopeid 0x20<link>
        ether 0c:42:a1:97:fa:a5  txqueuelen 1000  (Ethernet)
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
../build/reinvent_dpdk_udp_opt_integration_test.tsk -m server -p TEST_SERVER -P
EAL: Detected CPU lcores: 16
EAL: Detected NUMA nodes: 1
EAL: Detected shared linkage of DPDK
EAL: Selected IOVA mode 'VA'
EAL: No free 2048 kB hugepages reported on node 0
EAL: VFIO support initialized
EAL: Probe PCI driver: mlx5_pci (15b3:1015) device: 0000:01:00.1 (socket 0)
mlx5_net: No available register for sampler.
TELEMETRY: No legacy callbacks, legacy socket not created
lcoreId 00 rxqIndex 00 listening for packets
```

Cores reads 16 not 8 because CPU hyper threads are enabled. In fact, this is often disabled for production work loads.

13. On your client machine run command: `./reinvent_dpdk_udp_integration_test client perf`
14. The client will exit to the shell when done; to terminate the server press CTRL-C at any time.

# Brief Discussion of benchmark output

For every 100,000 packets received the server will print the current receive rate two ways: pps (packets per second)
and the inverse (nanoseconds per packet). The client will send serveral bursts of packets UDP. Each packet is 74 bytes
with 32 bytes of payload. The difference is IP headers. It'll also display transmission rates for example:

```
lcoreId: 00, txqIndex: 00: elsapsedNs: 368201, packetsQueued: 2000, packetSizeBytes: 74, payloadSizeBytes: 32, pps: 5431815.774536, nsPerPkt: 184.100500, bytesPerSec: 401954367.315678, mbPerSec: 383.333556, mbPerSecPayloadOnly: 165.765862 stalledTx 0
```

Not only is this performance far superior to kernel based I/O (see the aws setup document in this directory), it's better
than AWS virtual NICs running the exact same code which usually requires at least 550ns/packet. AWS ENA NICs have other
smaller problems too.

The client sends all packets on one TXQ and all packets are received on one RX queue. RSS is disabled. Except when the PCI
bus or DRAM bandwidth is saturated, these numbers will basically scale linearly as more and more Mellanox queues are used.
The client uses one thread pinned to one core, as does the server in this demonstration. This Mellanox NIC supports up to
1024 RXQ and 1024 TXQs, which are located in the NIC itself. You can confirm this as follows:

```
$ cd /root/Dev/reinvent/scripts
$ ./runpmd # run DPDK's helper program
testpmd> show port info 0
********************* Infos for port 0  *********************
MAC address: 0C:42:A1:97:FA:A5
Device name: 01:00.1
Driver name: mlx5_pci
Firmware-version: 14.27.1016
Devargs: class=eth
Connect to socket: 0
memory allocation on the socket: 0
Link status: up
Link speed: 10 Gbps
Link duplex: full-duplex
Autoneg status: On
MTU: 1500
Promiscuous mode: enabled
Allmulticast mode: disabled
Maximum number of MAC addresses: 128
Maximum number of MAC addresses of hash filtering: 0
VLAN offload: 
  strip off, filter off, extend off, qinq strip off
Hash key size in bytes: 40
Redirection table size: 1
Supported RSS offload flow types:
  ipv4
  ipv4-frag
  ipv4-tcp
  ipv4-udp
  ipv4-other
  ipv6
  ipv6-frag
  ipv6-tcp
  ipv6-udp
  ipv6-other
  ipv6-ex
  ipv6-tcp-ex
  ipv6-udp-ex
  user defined 60
  user defined 61
  user defined 62
  user defined 63
Minimum size of RX buffer: 32
Maximum configurable length of RX packet: 65536
Maximum configurable size of LRO aggregated packet: 65280
Current number of RX queues: 1
Max possible RX queues: 1024
Max possible number of RXDs per queue: 65535
Min possible number of RXDs per queue: 0
RXDs number alignment: 1
Current number of TX queues: 1
Max possible TX queues: 1024
Max possible number of TXDs per queue: 65535
Min possible number of TXDs per queue: 0
TXDs number alignment: 1
Max segment number per packet: 40
Max segment number per MTU/TSO: 40
Device capabilities: 0x10( FLOW_SHARED_OBJECT_KEEP )
Switch name: 01:00.1
Switch domain Id: 0
Switch Port Id: 65535
```

Enter CTRL-D to exit.
