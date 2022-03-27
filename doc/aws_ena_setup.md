# General Requirements
* Intel Xeon x86 64-bit running Linux `Amazon Linux 2 AMI (HVM)`
* GNU C/C++ tool chain. 
  * There is no fundamental reason code cannot be built for other CPUs or via Clang.
  * However, Linux is a firm requirement. 
  * Build instructions are provided for GNU C/C++ on Xeon Intel x86 64-bit only. Experienced developers can generalize from there. 
* Machines require sufficient space for builds, and deployment of a few tasks. 8GiB EBS storage is fine. The resulting programs do not involve disk I/O other than minor logging. They benchmark DRAM memory, NIC I/O. Future work will explore NVM storage.
* The deployment machines should be ENA (Elastic Network Adaptor) enabled. ENA is default enabled in the Linux image AWS installs for you.
* You need at least two deployment machines where code is run. Clearly, network I/O cannot meaningfully occur unless client and server processes run on different physical machines.
* The deployment machines should be dedicated hosts or metal (optionally spot) instances. Shared tenancy brings complications not in scope here.
* Each deployment machine requires at least (2) NICS. One NIC comes with the instance; it's used to ssh login to the machine. Designate this NIC as the control NIC. The second NIC must be attached so that it can be used by DPDK. Call the second NIC the data-NIC.
* The deployment machines should have at least 32GiB memory.
* Latency will increase as more switch/routers are hit between the client and server. To minimize those effects create a AWS placement group into which all deployment machines are members. Or use one data center.

# Configuration Tested
* Intel Xeon x86 64-bit running `Amazon Linux 2 AMI (HVM) - Kernel 5.10 64-bit x86` AMI-ID `ami-0ed9277fb7eb570c9`
* Build and deployment machines: (2) spot `c5n.metal` instances. Note `c5n.metal` supports LLQ mode
* Data Center Tested: US-East (Northern Virginia, US)
* Stock AWS NICs
* Stock packages installed by `yum`
* [DPDK](https://git.dpdk.org/) head commit
* [AWS DPDK Drivers](https://github.com/amzn/amzn-drivers.git) head commit
* [Reinvent source code](https://github.com/rodgarrison/reinvent) head commit
* Last test completed 2022-03-06

# References:
* [eRPC Library](https://github.com/erpc-io/eRPC)
* [eRPC Paper](https://www.usenix.org/system/files/nsdi19-kalia.pdf)
* [DPDK](https://www.dpdk.org/)
* [AWS DPDK Drivers](https://github.com/amzn/amzn-drivers.git)
* [AWS ENA](https://www.kernel.org/doc/html/latest/networking/device_drivers/ethernet/amazon/ena.html)
* [RSS Scaling](https://medium.com/@anubhavchoudhary/introduction-to-receive-side-scaling-rss-7cd97307d220)
* [Symmetric Hashing](https://www.ran-lifshitz.com/2014/08/28/symmetric-rss-receive-side-scaling/)

# Setup Goals
This guide accomplishes two things:
* Install and build all software for DPDK development
* Then, in the post-build work, you will configure two AWS `c5n.metal` machines and send UDP packets between them. The 
test task provided by Reinvent configures two TX queues on a client machine and 32 RX queues on a server machine.
The client sends UDP packets to the server, and the server prints the payload it got.

Instructions are provided to obtain the software from an AMI image so you can skip install and build. Or, optionally,
you can build the code yourself. An `install` script is provided for this purpose so building is almost turn-key.

# AWS Setup Notes

## Financial Costs
You will not be able to run Reinvent on free-tier hardware. You'll need an account and supply method of payment. And since metal or dedicated host instances are required with high CPU count, you'll **first have to ask Amazon** for a limit increase. This work was completed with Amazon permitting `All Standard (A, C, D, H, I, M, R, T, Z) Spot instances` with a limit of 256 vCPUs. Spot instances costing approximately $1.50/hr or less were used. The instances were never recalled. Benchmarking, setup, or builds was never interrupted.

## PEM Files
When you create an AWS machine instance you are required to make and download a PEM (private encryption key) file for it. This PEM file is used to secure the connection when logging into the AWS instance over ssh from your office or home computer. So, for example, if your instance has public IPv4 address 1.2.3.4 and your downloaded PEM file is located at `/home/user/aws.pem` the correct ssh login command is `ssh -i /home/user/aws.pem ec2-user@1.2.3.4`. Your PEM file **must have chmod mode 0400**. The user `ec2-user` is a consequence of using AWS Linux. A second or additional PEM file is not required for data NICs; it is required only for the control NIC.

## Security Groups
When you provision a AWS instance or create additional NICs you'll need to specify a security group. The security group is a kind of firewall that allows or disallows network access by mode, and port. A pragmatic approach is to create one group and use this group as you provision instances and when you create additional NICs. The verified configuration here uses one security group with the following settings. **Constrict these settings for your situation; it's your responsibility to protect your machines**:

* Inbound:
  * `IPv4 SSH         TCP 22    0.0.0.0/0` - Allow this computer to accept incoming ssh on port 22 over IPv4 from any other IP address
  * `IPv4 All-UDP     UDP       0.0.0.0/0` - Allow this computer to receive UDP packets over ports
* Outbound:
  * `IPv4 All-UDP     UDP       0.0.0.0/0` - Allow this computer to transmit UDP packets over any UDP port to other other IP address
  * `IPv4 All Traffic IPV4      0.0.0.0/0` - Allow this computer to transmit IPV4 packets in any mode to any port to any other IP address

## How to Add More NICs
AWS instances come pre-configured with one NIC with a public IP4V address preset for it. This NIC will be used to login to the machine, and to communicate with other deployment machines where Reinvent tasks run. You must provision at least once more NIC for DPDK which DPDK will use in turn. You will repeat these steps once per deployment machine. Since the minimum configuration is two host instances, you'll do this at least twice. Testing with two or more DPDK assignable NICs is future work:

* Note the data center assigned to your deployment machines e.g. `US-EAST-1A`
* On the EC2 dashboard locate `Network & Security` under which appears `Network Interfaces`. Click on `Network Interfaces`
* Top right corner of screen click on `Create Network Interface`
* Name the NIC, then choose a subnet in the same location as your instance e.g. `US-EAST-1A`
* Choose you security group, for example, the same one used to create instance(s)
* **Do not enable EFA.**
* Navigate back to the EC2 instance list; select the instance to which the second NIC should be added
* Under the actions drop down select `Networking` then `Attach Network Interface` choosing the NIC created above
* Your instance will now have two NICS: the original NIC with the public IPv4 address for ssh login, while the second NIC is not public accessible.

# No build Approach
The public AWS AMI `ami-0c342d04107772468` (2022-03-09 on commitish e1cb7078c9fd899e1f07c781fc1d988ca48015a6) is a minimal, complete image containing the code, build dependencies, build tool chain, and pre-built tasks. IOMMU has been enabled. Provision your AWS instances with this AMI-ID then proceed directly to [Post Build Instructions](#post-build-instructions-10mins). The AMI image reflects `H=/home/ec2-user/Dev` and `D=/home/ec2-user/local`, and are preset as environment variables when you login.

# Pre-build Instructions (15mins)
Note: machine reboot takes about 10mins/ea.

These instructions are based on the [DPDK ENA Setup](https://doc.dpdk.org/guides/nics/ena.html) guide. While `c5n.metal` machines are IOMMU capable it is not turned on by default. DPDK requires IOMMU. You only need to do this once. For each deployment machine:

1. Insure machine meets requirements
2. SSH login to machine allocated in (1)
3. Run command: `sudo vi /etc/default/grub`
4. Append to the end of the `GRUB_CMDLINE_LINUX_DEFAULT` two parameters: `iommu=1 intel_iommu=on`. See below for an example
5. Save the file
6. Run command: `sudo /bin/bash` then `grub2-mkconfig > /boot/grub2/grub.cfg`
7. Run command: `sudo reboot -h 0`

Once your machine has rebooted, login and confirm IOMMU groups now exist:

```
$ ls /sys/kernel/iommu_groups
0    104  110  117  123  13   136  142  149  155  161  168  174  180  187  193  2    205  211  218  224  230  237  3   36  42  49  55  61  68  74  80  87  93
1    105  111  118  124  130  137  143  15   156  162  169  175  181  188  194  20   206  212  219  225  231  24   30  37  43  5   56  62  69  75  81  88  94
10   106  112  119  125  131  138  144  150  157  163  17   176  182  189  195  200  207  213  22   226  232  25   31  38  44  50  57  63  7   76  82  89  95
100  107  113  12   126  132  139  145  151  158  164  170  177  183  19   196  201  208  214  220  227  233  26   32  39  45  51  58  64  70  77  83  9   96
101  108  114  120  127  133  14   146  152  159  165  171  178  184  190  197  202  209  215  221  228  234  27   33  4   46  52  59  65  71  78  84  90  97
102  109  115  121  128  134  140  147  153  16   166  172  179  185  191  198  203  21   216  222  229  235  28   34  40  47  53  6   66  72  79  85  91  98
103  11   116  122  129  135  141  148  154  160  167  173  18   186  192  199  204  210  217  223  23   236  29   35  41  48  54  60  67  73  8   86  92  99
```

A good grub file appears as:

```
$ cat /etc/default/grub
GRUB_CMDLINE_LINUX_DEFAULT="console=tty0 console=ttyS0,115200n8 net.ifnames=0 biosdevname=0 nvme_core.io_timeout=4294967295 rd.emergency=poweroff rd.shell=0 iommu=1 intel_iommu=on"
GRUB_TIMEOUT=0
GRUB_DISABLE_RECOVERY="true"
GRUB_TERMINAL="ec2-console"
GRUB_X86_USE_32BIT="true"
```

You can also run this command to double-check change on reboot:

```
$ cat /proc/cmdline
BOOT_IMAGE=/boot/vmlinuz-5.10.96-90.460.amzn2.x86_64 root=UUID=f047d80e-fad3-443d-9478-af686284acba ro console=tty0 console=ttyS0,115200n8 net.ifnames=0 biosdevname=0 nvme_core.io_timeout=4294967295 rd.emergency=poweroff rd.shell=0 iommu=1 intel_iommu=on
```

Failure to enable IOMMU is evidenced by DPDK-bind error during post-build-instructions: `Cannot bind to driver vfio-pci: [Errno 19] No such device`.

# Build instructions (25mins)
DPDK build is about ~20mins.

1. Insure machine meets requirements and that IOMMU is enabled on each machine
2. SSH login to machine allocated in (1)
3. [From the reinvent GIT repo](https://github.com/rodgarrison/reinvent/blob/main/scripts/install) copy-and-paste this script into your home directory in (2)
4. Run command: `chmod 555 ./install`
5. Run command: `./install`
6. When install is done run command: `. ~/.bashrc`

Repeat these steps for each machine in your deployment set.

# Post Build Instructions (10mins)
At this point in time you must have built all software on all deployment machines or used the public AMI. If you have
not already, add a second ENA NIC to each machine in your deployment set. See above for instructions. Now, in what 
follows we'll do four things. Each item is one-time work:

* record the IP and MAC addresses of the 2nd NIC for each machine
* setup TLB huge-pages and make the 2nd NIC available for DPDK use
* update Reinvent's environment variables by changing its MAC, IP, ports UDP routing for your machine set
* run reinvent task (built above) to send UDP packets between two machines

## Record MAC and IP addresses
AWS adds your second NIC in the up state with an assigned IP address. It's called `eth1` while `eth0` is the NIC you
login with over ssh:

```
[ec2-user@ip-172-31-79-180 ~]$ ifconfig -a
eth0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 9001                           <--- leave this unchanged
        inet 172.31.79.180  netmask 255.255.240.0  broadcast 172.31.79.255
        inet6 fe80::140b:d1ff:fe36:d4ad  prefixlen 64  scopeid 0x20<link>
        ether 16:0b:d1:36:d4:ad  txqueuelen 1000  (Ethernet)
        RX packets 1386  bytes 477817 (466.6 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 1269  bytes 172080 (168.0 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

eth1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 9001                           <--- for DPDK
        inet 172.31.67.198  netmask 255.255.240.0  broadcast 172.31.79.255           <--- IPV4 address here
        inet6 fe80::1499:5aff:fe30:1735  prefixlen 64  scopeid 0x20<link>
        ether 16:99:5a:30:17:35  txqueuelen 1000  (Ethernet)                         <--- MAC address here
        RX packets 3  bytes 1180 (1.1 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 19  bytes 2283 (2.2 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

For each machine record `eth1`'s IP address and MAC address. You'll need this later.


## Setup TLB and Enable eth1 for DPDK
* Run command: `sudo ip link set eth1 down`
* Run command: `cd $H/reinvent/scripts`
* Run command: `sudo ./setup`

If the last command is successful you'll see output like this. In this output the device `0000:7f:00.0` corresponds
to what was `eth1`. It is now ready for DPDK use. The device `0000:7e:00.0` is what's `eth0`:

```
Network devices using DPDK-compatible driver
============================================
0000:7f:00.0 'Elastic Network Adapter (ENA) ec20' drv=vfio-pci unused=ena
.
 .
  .
Network devices using kernel driver
===================================
0000:7e:00.0 'Elastic Network Adapter (ENA) ec20' if=ens6 drv=ena unused=vfio-pci *Active*
.
 .
  .
```

## Configure Reinvent Script for UDP Packet Transmission
Now at this point you should have two AWS `c5n.metal` machines each with two NICs. The second NIC is DPDK enabled. In
previous steps you collected the MAC and IPV4 address for each machine's `eth1` NIC. Desiginate one of these machines
`CLIENT` and the other machine `SERVER`. In this test `CLIENT` will send UDP packets to `SERVER`. 

The default config is for the client to create two TXQs. Each TXQ sends packets to the server machine. The server
machine by default is configured to make 32 RXQs. Each RXQ listens for incoming packets and prints the payload
received. Because there are two TXQs the config must have two routes one per TXQ.

So on the client machine do the following:

* Run command: `cd $H/reinvent/scripts`
* Run command: `vi reinvent_dpdk_udp_integration_test`
* In the vi editor go to line 72 reading `TEST_CLIENT_DPDK_NIC_DEVICE_TXQ_DEFAULT_SRC_MAC`
* Update the source MAC address with your client's MAC address in the variable `TEST_CLIENT_DPDK_NIC_DEVICE_TXQ_DEFAULT_SRC_MAC`
* Update the destination MAC address with your server's MAC address in the variable `TEST_CLIENT_DPDK_NIC_DEVICE_TXQ_DEFAULT_DST_MAC` 
You must repeat the same server MAC address without spaces, comma separated twice
* Update the source IPV4 address with your client's IPV4 address in the variable `TEST_CLIENT_DPDK_NIC_DEVICE_TXQ_DEFAULT_SRC_IP`
* Update the destination IPV4 address with your server's IPV4 address in the variable `TEST_CLIENT_DPDK_NIC_DEVICE_TXQ_DEFAULT_DST_IP`.
You must repeat the same server IPV4 address without spaces, comma separated twice
* Leave ports in `TEST_CLIENT_DPDK_NIC_DEVICE_TXQ_DEFAULT_SRC_PORT, TEST_CLIENT_DPDK_NIC_DEVICE_TXQ_DEFAULT_DST_PORT` unchanged.
These are in fact pseudo ports. See `aws_ena_packet_design.md` this directory for more information on pseudo-ports.

## Run the UDP Test Program

First, on the server machine:

* Run command: `cd $H/reinvent/scripts`
* Run command: `sudo ./reinvent_dpdk_udp_integration_test server`

On the client machine:

* Run command: `cd $H/reinvent/scripts`
* Run command: `sudo ./reinvent_dpdk_udp_integration_test client`

## Examining UDP Test Program Output

The server side will report packets received. Each cient TXQ sends 16-packets. Press CTRL-C to exit server

```
000014.755516156 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 6
000014.755543350 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 3
000014.755571390 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 7
000014.755578870 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 5
000014.755585669 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 6
000014.755594826 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 14
000014.755604853 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 0
000014.755613145 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 4
000014.755622379 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 2
000014.755631109 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 15
000014.755638713 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 5
000014.755646909 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 10
000014.755655113 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 11
000014.755663297 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 10
000014.755672373 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 3
000014.755679405 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 4
000014.755687644 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 12
000014.755694503 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 2
000014.755702365 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 11
000014.755710674 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 13
000014.755717437 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 1
000014.755725414 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 1
000014.755735625 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 7
000014.755743328 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 0
000014.755751568 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 12
000014.755759816 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 8
000014.755768154 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 9
000014.755776339 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 13
000014.755784218 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 8
000014.755792562 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 14
000014.755800738 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 9
000014.755808598 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 1, txqId: 1, sequenceNumber: 15
000025.310258390 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 0
000081.630594235 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 0
000137.950927477 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 0
000194.271259188 INFO  reinvent_dpdk_udp_integration_test.cpp:293 id 0 rxqIndex 0 packet: sender: lcoreId: 0, txqId: 0, sequenceNumber: 0
```

The client side shows packets sent:

```
000002.630153421 INFO  reinvent_dpdk_udp_integration_test.cpp:434 launching DPDK worker threads
000002.630237475 DEBUG reinvent_dpdk_udp_integration_test.cpp:264 sent 8 packets
000002.630260970 DEBUG reinvent_dpdk_udp_integration_test.cpp:264 sent 8 packets
000002.630305837 DEBUG reinvent_dpdk_udp_integration_test.cpp:264 sent 8 packets
000002.630316870 DEBUG reinvent_dpdk_udp_integration_test.cpp:264 sent 8 packets
000002.630372664 INFO  reinvent_dpdk_udp_integration_test.cpp:438 waiting for DPDK worker threads to stop
```

Each task dumps the AWSConfig object which configures the DPDK work in Reinvent. It's in JSON format so it can pretty-printed.
The client config is given here (elided) with some comments in the right margin. This config is 100% based on the 
environment variables in the driver script `reinvent_dpdk_udp_integration_test`. The environment variables are described
in [detail here](https://github.com/rodgarrison/reinvent/blob/main/src/reinvent/dpdk/reinvent_dpdk_initaws.h):

```
$ cat config | python3 -m json.tool
{
    "dpdkArgs": [
        "--proc-type",                                              <--- arguments passed to DPDK
        "primary",
        "--in-memory",
        "--log-level",
        "7",
        "--huge-dir",
        "/mnt/huge",
        "-n",
        "12",
        "--lcores",                                                 <--- 2 TXQs means 2 lcores each on own CPU
        "(0)@0,(1)@1"                                                    this argument is computed by reinvent
    ],
    "deviceId": 0,
    "pciId": "0000:7f:00.0",
    "numaNode": 0,                                                  <--- DPDK NIC used is on NUMA node 0
    "rxqCount": 0,
    "txqCount": 2,                                                  <--- Making 2 TXQs available
    "rxqThreadCount": 0,
    "txqThreadCount": 2,                                            <--- Requesting 2 TXQ lcores one per queue
    "rxqPolicy": "OFF",                                             <--- RX disabled on client
    "txqPolicy": "DISTINCT",                                        <--- TX enabled on client: each lcore must be on
    "rxqCandidateVcpuList": [                                            distinct HW core
        0,
        1,
        2,                                                          <--- HW cores on NUMA node 0 available for RX lcores
        3,                                                               but since RX off on client config not used
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        36,
        37,
        38,
        39,
        40,
        41,
        42,
        43,
        44,
        45,
        46,
        47,
        48,
        49,
        50,
        51,
        52,
        53
    ],
    "txqCandidateVcpuList": [                                       <--- NUMA HW cores available for TXQ use
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        36,
        37,
        38,
        39,
        40,
        41,
        42,
        43,
        44,
        45,
        46,
        47,
        48,
        49,
        50,
        51,
        52,
        53
    ],
    "mtu": 1500,
    "memzoneMask": 5,
    "memzoneReserveKb": 153600,
    "memzoneName": "REINVENT_AWS",
    "mempoolPolicy": "PER_QUEUE",                                   <--- requests each TXQ get its own mempool
    "rxq": [],
    "txq": [                                                        <--- info about both TXQs
        {
            "id": 0,
            "vcpuId": 0,
            "lcoreId": 0,
            "memzone": "0x100000318",
            "mempool": "0x2200200bc0",
            "mempoolPolicy": "PER_QUEUE",
            "ringSize": 1024,
            "defaultFlow": 2048,
            "defaultRoute": {
                "sourceMac": "16:b4:a8:69:7a:87",
                "destinationMac": "16:99:5a:30:17:35",
                "sourceIPV4": "172.31.76.132",
                "destinationIPV4": "172.31.67.198",
                "sourcePort": 10000,
                "destinationPort": 20000
            }
        },
        {
            "id": 1,
            "vcpuId": 1,
            "lcoreId": 1,
            "memzone": "0x100000318",
            "mempool": "0x2200200800",
            "mempoolPolicy": "PER_QUEUE",
            "ringSize": 1024,
            "defaultFlow": 2048,
            "defaultRoute": {
                "sourceMac": "16:b4:a8:69:7a:87",
                "destinationMac": "16:99:5a:30:17:35",
                "sourceIPV4": "172.31.76.132",
                "destinationIPV4": "172.31.67.198",
                "sourcePort": 10001,
                "destinationPort": 20001
            }
        }
    ],
    "lcore": [                                                      <--- Identifies each lcore, the HW core it's assigned
        {                                                                and what role it has
            "id": 0,
            "vcpu": 0,
            "role": [
                "TXQ"
            ]
        },
        {
            "id": 1,
            "vcpu": 1,
            "role": [
                "TXQ"
            ]
        }
    ],
    "vcpu": [                                                       <--- Info about the CPU inventory on c5n box
        {
            "id": 0,
            "core": 0,
            "socket": 0,
            "numaNode": 0,
            "softEnabled": 1
        },
        {
            "id": 1,
            "core": 1,
            "socket": 0,
            "numaNode": 0,
            "softEnabled": 1
        },
    "vcpu": [
        {
            "id": 0,
            "core": 0,
            "socket": 0,
            "numaNode": 0,
            "softEnabled": 1
        },
        {
            "id": 1,
            "core": 1,
            "socket": 0,
            "numaNode": 0,
            "softEnabled": 1
        },
.
  .
    .
        {
            "id": 70,
            "core": 26,
            "socket": 1,
            "numaNode": 1,
            "softEnabled": 1
        },
        {
            "id": 71,
            "core": 27,
            "socket": 1,
            "numaNode": 1,
            "softEnabled": 1
        }
    ],
    "linkSpeed": 0,
    "rxMqMask": 0,
    "txMqMask": 0,
    "rxOffloadMask": 0,
    "txOffloadMask": 0,
    "defaultTxFlow": 2048
}
```
