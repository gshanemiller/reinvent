# Purpose
This GIT repository provides instructions on how to run eRPC on AWS instances.

eRPC is a high performance user-space NIC I/O library built on DPDK. DPDK provides user-space I/O code building blocks. eRPC adds congestion management, packet loss recovery, and a high level RPC framework that reliably works over lossy COTS ethernet hardware. eRPC compares to [Apache Seastar's](http://seastar.io) I/O library except eRPC is written in C lightly wrapped in C++ classes without being encumbered by C++ futures/promises. Seastar like eRPC builds on DPDK to achieve kernel by-pass I/O. Seastar is used in [ScyllaDb](https://www.scylladb.com), the C++ Cassandra port.

# References:
* [Upstream eRPC Library](https://github.com/erpc-io/eRPC)
* [eRPC Paper](https://www.usenix.org/system/files/nsdi19-kalia.pdf)
* [DPDK](https://www.dpdk.org/)
* [AWS Verified eRPC Fork (this repository)](https://github.com/rodgarrison/eRPC)

# Performance Results
(TBD Link to technical paper describing work)

# General Requirements
* Intel Xeon x86 64-bit running Linux `Amazon Linux 2 AMI (HVM)`
* GNU C/C++ tool chain. 
  * There is no fundamental reason code cannot be built for other CPUs or via Clang. 
  * However, Linux is a firm requirement. 
  * Build instructions are provided for GNU C/C++ on Xeon Intel x86 64-bit only. Experienced developers can generalize from there. 
* Machines require sufficient space for builds, and deployment of a few tasks. 8GiB EBS storage is fine. The eRPC programs do not involve disk I/O other than minor logging. They benchmark DRAM memory, NIC I/O. Future work will explore NVM storage with eRPC.
* The deployment machines should be ENA (Elastic Network Adaptor) enabled. ENA is default enabled in the Linux image AWS installs for you.
* You need at least two deployment machines where code is run. Clearly, network I/O cannot meaningfully occur unless client and server processes run on different physical machines.
* The deployment machines should be dedicated hosts or metal (optionally spot) instances. Shared tenancy brings complications not in scope here.
* Each deployment machine requires at least (2) NICS. One NIC comes with the instance; it's used to login to the machine. Designate this NIC as the control NIC. The second NIC must created and attached so that it can be used by DPDK/eRPC. Call the second NIC the eRPC NIC. eRPC uses the control NIC to talk to software on remote machines whence information about remote eRPC NICs can be found.
* The deployment machines should have at least 8GiB memory.
* Latency will increase as more switch/routers are hit between the client and server. To minimize those effects create a AWS placement group into which all deployment machines are members. Or use one data center.

# Configuration Tested
* Intel Xeon x86 64-bit running `Amazon Linux 2 AMI (HVM) - Kernel 5.10 64-bit x86` AMI-ID `ami-0ed9277fb7eb570c9`
* Build and deployment machines: (2) spot `c5n.metal` instances. Note `c5n.metal` supports LLQ mode
* Data Center Tested: US-East (Northern Virginia, US)
* Stock AWS NICs
* Stock packages installed by `yum`
* [DPDK](https://git.dpdk.org/) head commit
* [AWS Verified eRPC Fork](https://github.com/rodgarrison/eRPC)

# AWS Setup Notes

## Financial Costs
You will not be able to run eRPC on free-tier hardware. You'll need an account and supply method of payment. And since metal or dedicated host instances are required with high CPU count, you'll **first have to ask Amazon** for a limit increase. This work was completed with Amazon permitting `All Standard (A, C, D, H, I, M, R, T, Z) Spot instances` with a limit of 256 vCPUs. Spot instances costing approximately $1.50/hr or less were used. The instances were never recalled. Benchmarking, setup, or builds was never interrupted.

## PEM Files
When you create an AWS machine instance you are required to make and download a PEM (private encryption key) file for it. This PEM file is used to secure the connection when logging into the AWS instance over ssh from your office or home computer. So, for example, if your instance has public IPv4 address 1.2.3.4 and your downloaded PEM file is located at `/home/user/aws.pem` the correct ssh login command is `ssh -i /home/user/aws.pem ec2-user@1.2.3.4`. Your PEM file **must have chmod mode 0400**. The user `ec2-user` is a consequence of using AWS Linux. A second or additional PEM file is not required for eRPC/DPDK NIC; it is required only for the control NIC.

## Security Groups
When you provision a AWS instance or create additional NICs you'll need to specify a security group. The security group is a kind of firewall that allows or disallows network access by mode, and port. A pragmatic approach is to create one group and use this group as you provision instances and when you create additional NICs. The verified configuration here uses one security group with the following settings:
* Inbound:
  * `IPv4 SSH         TCP 22    0.0.0.0/0` - Allow this computer to accept incoming ssh on port 22 over IPv4 from any other IP address
  * `IPv4 Custom-UDP  UDP 31850 0.0.0.0/0` - Allow this computer to receive UDP packets (eRPC/DPDK) over port 31850 from any other IP address
* Outbound:
  * `IPv4 Custom-UDP  UDP 31850 0.0.0.0/0` - Allow this computer to transmit UDP packets (eRPC/DPDK) over port 31850 to any other IP address
  * `IPv4 Any         Any Any   0.0.0.0/0` - Allow this computer to transmit TCP packets in any mode to any port to any other IP address

UDP port 31850 is presently hardcoded into eRPC code. You may narrow reachable machines by changing `0.0.0.0/0`. Finally, while the inbound ssh rule allows connection from any machine on the internet, you must still have the right PEM file for it to work.

## How to Add More NICs
AWS instances come pre-configured with one NIC with a public IP4V address preset for it. This NIC will be used to login to the machine, and to communicate with other deployment machines where eRPC tasks run. You must provision at least once more NIC for DPDK which eRPC will use in turn. You will repeat these steps once per deployment machine. Since the minimum configuration is two host instances, you'll do this at least twice. Testing with two or more DPDK assignable NICs is future work:

* Note the data center assigned to your deployment machines e.g. `US-EAST-1A`
* On the EC2 dashboard locate `Network & Security` under which appears `Network Interfaces`. Click on `Network Interfaces`
* Top right corner of screen click on `Create Network Interface`
* Name the NIC, then choose a subnet in the same location as your instance e.g. `US-EAST-1A`
* Choose you security group, for example, the same one used to create instance(s)
* **Do not enable EFA.**
* Navigate back to the EC2 instance list; select the instance to which the second NIC should be added
* Under the actions drop down select `Networking` then `Attach Network Interface` choosing the NIC created above
* Your instance will now have two NICS: the original NIC with the public IPv4 address for ssh login, while the second NIC is not public accessible.

## AWS/eRPC Queue Differences
eRPC upstream code assumes NICs have 16 queues. AWS NICS may have other values. `c5n.metal` instances have/had 32; this is configured in the forked code. To determine the number of queues run the command `ethtool -S ens6` where, for example, `ens6` is the name of your control NIC. Note ouput will reference queues:

```
$ ethtool -S ens6
NIC statistics:
     tx_timeout: 0
     suspend: 0
     resume: 0
     wd_expired: 0
     interface_up: 2
     .
      .
       .
     queue_0_tx_cnt: 66
     .
      .
       .
     queue_31_tx_cnt: 0
     .
      .
       .
```
If you don't know your control NIC name, run `ip link`. If this command is run before you add the second NIC, or after you've completed the [Post Build Instructions](#post-build-instructions-5mins) when the second NIC is attached to DPDK, `ip link` will only show one non-loopback device. That's your control NIC:

```
$ ip link
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
2: ens6: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 9001 qdisc mq state UP mode DEFAULT group default qlen 1000
    link/ether 02:e3:fc:59:d6:8f brd ff:ff:ff:ff:ff:ff
```

Otherwise you have the second NIC added but it's not DPDK bound. In that case `ip link` will show a third interface in the `DOWN` state. Since both NICs are the same hardware, `ethtool -S <control-NIC-name>` will yield the same information. 

If your value differs you'll need to set it in code:

```
.../eRPC/src/transport_impl/dpdk/dpdk_transport.h 
31 - static constexpr size_t kMaxQueuesPerPort = kIsWindows ? 1 : 8;
31 + static constexpr size_t kMaxQueuesPerPort = kIsWindows ? 1 : 32;
```

Because of the old story about plateauing CPU frequencies compensated by multi-cores, NIC manufactures had to support multi-queues whereby packets are placed into different NIC queues. Otherwise multi-core designs would be blocked in the NIC because its buffers are full. Then, on the other hand, user-space code can pin a thread per core to handle one queue. eRPC/DPDK relies on this design. **Changing this code forces manual rebuild**.

RSS (Receive Side Scaling) Links:
* [RSS Scaling](https://medium.com/@anubhavchoudhary/introduction-to-receive-side-scaling-rss-7cd97307d220)
* [AWS ENA](https://www.kernel.org/doc/html/latest/networking/device_drivers/ethernet/amazon/ena.html)
* [Symmetric Hashing](https://www.ran-lifshitz.com/2014/08/28/symmetric-rss-receive-side-scaling/)

## AWS/eRPC Queue Hash Incompatibility
Once the prospect of NIC multi-queue arises, the potential for configuring a hash to assign packets to queues occurs. Some NICs allow this; some don't. Upstream eRPC code tries to do this, but AWS NICs do not support this capability. You can re-confirm the queue count and hash function with `ethtool`. `toeplitz` hashing seems to preferred, which is default:

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

The forked eRPC code comments out RSS NIC hashing configuration code:

```
src/transport_impl/dpdk/dpdk_init.cc
void DpdkTransport::setup_phy_port(uint16_t phy_port, size_t numa_node,...
  if (!kIsWindows) {
55  - eth_conf.rxmode.mq_mode = ETH_MQ_RX_RSS;
    + eth_conf.rxmode.mq_mode = ETH_MQ_RX_NONE;
    + // eth_conf.rxmode.mq_mode = ETH_MQ_RX_RSS;
      eth_conf.lpbk_mode = 1;
    - eth_conf.rx_adv_conf.rss_conf.rss_key =
    -    const_cast<uint8_t *>(kDefaultRssKey);
    - eth_conf.rx_adv_conf.rss_conf.rss_key_len = 40;
    - eth_conf.rx_adv_conf.rss_conf.rss_hf = ETH_RSS_UDP;
    + // eth_conf.rx_adv_conf.rss_conf.rss_key =
    + //  const_cast<uint8_t *>(kDefaultRssKey);
    + // eth_conf.rx_adv_conf.rss_conf.rss_key_len = 40;
    + // On AWS RSS hash key can be changed but not the hash function
    + // eth_conf.rx_adv_conf.rss_conf.rss_hf = 0;
```

## eRPC Mellanox Build Conflict
The eRPC authors used Mellanox NICs in some of their work. AWS does not provide Mellanox. Unfortuantely the code seems to be unable to run without a full Mellanox library install although the whole point is to eschew specialty hardware. To make it work on AWS the forked eRPC code comments out these lines:

```
.../eRPC/src/transport_impl/dpdk/dpdk_transport.cc
void DpdkTransport::resolve_phy_port()...
174 + /*
        const std::string drv_name = dev_info.driver_name;
        rt_assert(drv_name == "net_mlx4" or drv_name == "net_mlx5" or
                drv_name == "mlx5_pci",
               "eRPC supports only mlx4 or mlx5 devices with DPDK");
179 + */
```

# No build Approach
The public AWS AMI `ami-095d03d09d3d3d401` is a minimal, complete image containing the code, build dependencies, build tool chain, and pre-built tasks. Provision your AWS instances with this AMI-ID then proceed directly to [Post Build Instructions](#post-build-instructions-5mins). The AMI image reflects `H=/home/ec2-user/Dev` and `D=/home/ec2-user/local`, and are preset as environment variables when you login as `ec2-user` or `root`.

# Pre-build Instructions (15mins)
Note: machine reboot takes about 10mins/ea.

These instructions are based on the [DPDK ENA Setup](https://doc.dpdk.org/guides/nics/ena.html) guide. For each deployment machine,

1. Insure machine meets requirements
2. SSH login to machine allocated in (1)
3. Run command: `sudo vi /etc/default/grub`
4. Append to the end of the `GRUB_CMDLINE_LINUX_DEFAULT` two parameters: `iommu=1 intel_iommu=on`. See below for an example
5. Save the file
6. Run command: `sudo grub2-mkconfig > /boot/grub2/grub.cfg`
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

Failure to enable IOMMU is, in part, exhibited by DPDK-bind reporting an error during post-build-instructions: ` Cannot bind to driver vfio-pci: [Errno 19] No such device`.

# Build instructions (30mins)
DPDK build is about ~15mins. EPRC is ~1mins. For each provisioned machine running eRPC:

1. Insure machine meets requirements
2. SSH login to machine allocated in (1)
3. Make an empty parent directory called `H` (home) under which code will be placed:
  * Run command: `export H=<abs-path>`
  * Run command: `mkdir -p $H`
4. Install build dependencies:
  * Run command: `sudo /usr/bin/amazon-linux-extras`
  * Run command: `sudo yum install make gcc-c++.x86_64 numactl-devel.x86_64 numactl-libs.x86_64 rdma-core-devel.x86_64 git`
  * Run command: `pip3 install --user meson pyelftools ninja`
  * **Note** `yum install cmake` is too old. Install prebuilt binary.
  * Run command: `cd $H && wget https://cmake.org/files/v3.22/cmake-3.22.1-linux-x86_64.sh`
  * Run command: `cd $H && chmod 500 cmake-3.22.1-linux-x86_64.sh && ./cmake-3.22.1-linux-x86_64.sh`
  * Run command: `export PATH=$H/cmake-3.22.1-linux-x86_64/bin:$PATH`
  * Run command: `cd $H && rm -f cmake-3.22.1-linux-x86_64.sh`
5. Run command      : `cd $H`
6. Clone DPDK       : `git clone git://dpdk.org/dpdk`
7. Clone eRPC       : `git clone https://github.com/rodgarrison/eRPC.git`
8. Clone AWS drivers: `git clone https://github.com/amzn/amzn-drivers.git`
9. Build and install the Amazon VFIO driver needed by DPDK:
  * Run command: `cd $H/amzn-drivers/userspace/dpdk/enav2-vfio-patch`
  * Run command: `sudo ./get-vfio-with-wc.sh`
10. Prepare to build DPDK
  * Choose a destination directory `D` into which DPDK libraries, headers will be installed
  * `${D}` cannot equal or be inside `${H}`
  * If `${D}` or its parent(s) is owned by root you'll need to prefix some commands with `sudo`
  * Run command: `cd $H/dpdk`
  * Run command: `export D=<abs-path>`
  * Edit `$H/dpdk/meson_options.txt` changing `use_hpet` config to true: `'use_hpet', type: 'boolean', value: true`
  * Edit `$H/dpdk/meson_options.txt` changing `tests` config to false
  * DPDK build warns `variable-length array is too large` in `drivers/common/cnxk/roc_irq.c` during link time. Ignore these errors. [cnxk](https://doc.dpdk.org/guides/platform/cnxk.html) is not used in eRPC.
11. Build and Install DPDK
  * Run command: `cd $H/dpdk`
  * Run command: `meson -Db_lto=true -Dplatform=native -Dprefix=${D} build`
  * Run command: `[sudo] [time] ninja -j 10 -C build install`
  * Note `-j 10`. Omitting allows ninja to build faster by using all CPUs, but LTO optimization will overrun temporary space on an 8GiB volume. Procure a larger disk or keep concurrent jobs bounded.
12. Prepare to build eRPC:
  * The top-level eRPC `CMakeLists.txt` in (6) assumes DPDK was installed in `/home/ec2-user/local`.
  * If that's not your case search-replace `$H/eRPC/CMakeLists.txt` for the substring `/home/ec2-user/local` and update to your `$D`
  * This `CMakeLists.txt` also differs from its upstream version in these respects:
    * `LOG_LEVEL` changed to `info` from `warn`
    * Adds `-Wno-address-of-packed-member` per [GIT Issue](https://github.com/erpc-io/eRPC/issues/65)
    * Adds a few other `-Wno-...` flags
    * Removed Mellanox, verbs library dependencies  
    * Links >10x faster than upstream by choosing required DPDK libraries only
13. Build eRPC:
  * Run command: `cd $H/eRPC`
  * Run command: `cmake . -DPERF=ON -DTRANSPORT=dpdk; time make -j 10`
14. Proceed to [post-build-instructions](#post-build-instructions5-mins)

# Post Build Instructions (5mins)
At this point in time you must have:
* Built all software on all deployment machines or used the public AMI. See above.
* Added a 2nd NIC to each deployment instance

The following instructions **must be done once only following reboot or machine start** before you run eRPC code.

```
#!/bin/bash

#
# run as root
#
umount /dev/hugepages
umount /mnt/huge
echo 2048 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
echo 2048 > /sys/devices/system/node/node1/hugepages/hugepages-2048kB/nr_hugepages
mkdir -p /mnt/huge
mount -t hugetlbfs nodev /mnt/huge
modprobe vfio-pci
python3 /home/ec2-user/dpdk/usertools/dpdk-devbind.py -b vfio-pci 7f:00.0
python3 /home/ec2-user/dpdk/usertools/dpdk-devbind.py -s
```

An explanation follows, however, the above script is a typical for most purposes. Find it in `${H}/eRPC/scripts/setup` via AMI.

## Step 1 of 3: Setup TLB Huge Pages
eRPC/DPDK moves packet handling into user space. Each packet therefore will reside in user memory subject to inspection, mutation, and marshalling. Especially when combined with distributed big-data work, memory lookups are legion. To avoid TLB misses it's standard DPDK practice to enable huge TLB lookup:

```
# We don't want hugepages
sudo umount /dev/hugepages
# Unmout correct location just in case
sudo umount /mnt/huge
# this must be done once per NUMA node. c5n.metal has two NUMA nodes 0 and 1:
sudo echo 2048 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
sudo echo 2048 > /sys/devices/system/node/node1/hugepages/hugepages-2048kB/nr_hugepages
sudo mkdir -p /mnt/huge
sudo mount -t hugetlbfs nodev /mnt/huge
```

AWS `c5n.metal` instances have two NUMA nodes. To confirm:

```
$ dmesg | grep -i numa
[    0.012864] NUMA: Initialized distance table, cnt=2
.
 .
  .
[    2.091253] pci_bus 0000:00: on NUMA node 0
.
 .
  .
[    2.410230] pci_bus 0000:c2: on NUMA node 1
.
 .
  .
```

## Step 2 of 3: Load VFIO Kernel Module
You must explicitly enable the VFIO kernel patch: `sudo modprobe vfio-pci`. Confirm with `lsmod | grep vfio`

## Step 3 of 3: Bind the 2nd NIC to DPDK
Each NIC will have a PCI entry:

```
$ lspci | grep Eth
7e:00.0 Ethernet controller: Amazon.com, Inc. Elastic Network Adapter (ENA)
7f:00.0 Ethernet controller: Amazon.com, Inc. Elastic Network Adapter (ENA)
```

Typically, the first item `7e:00.0` is the control NIC while `7f:00.0` is the eRPC/DPDK NIC. To verify which is which,

```
# See build instructions for setting $H which will tend be the same when building manually or using the AMI:
$ sudo python3 ${H}/dpdk/usertools/dpdk-devbind.py -s
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

The **active NIC using the kernel driver** is the control NIC leaving `7f:00.0` as the eRPC NIC. Now, if your output shows two NICs (7e:00.0 and 7f:00.0) as active kernel NICs -- AWS sometimes sometimes enables second NIC like a control NIC --- you'll take the `7f:00.0` down first:

```
# case when both NICs are kernel active
$ sudo python3 ${H}/dpdk/usertools/dpdk-devbind.py -s
.
 .
  .
Network devices using kernel driver
===================================
0000:7e:00.0 'Elastic Network Adapter (ENA) ec20' if=eth0 drv=ena unused=vfio-pci *Active*
0000:7f:00.0 'Elastic Network Adapter (ENA) ec20' if=eth1 drv=ena unused=vfio-pci *Active*
.
 .
  .
 
$ ip link
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 9001 qdisc mq state UP mode DEFAULT group default qlen 1000
    link/ether 02:fb:88:01:e2:1f brd ff:ff:ff:ff:ff:ff
3: eth1: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 9001 qdisc mq state UP mode DEFAULT group default qlen 1000
    link/ether 02:ed:1f:ee:dd:f3 brd ff:ff:ff:ff:ff:ff

$ sudo ip link set eth1 down
# ip link show



$ sudo python3 ${H}/dpdk/usertools/dpdk-devbind.py -s
.
 .
  .

Network devices using kernel driver
===================================
0000:7e:00.0 'Elastic Network Adapter (ENA) ec20' if=eth0 drv=ena unused=vfio-pci *Active*
0000:7f:00.0 'Elastic Network Adapter (ENA) ec20' if=eth1 drv=ena unused=vfio-pci 
.
 .
  .
```

Bind it to DPDK over VFIO, and confirm status:

```
$ sudo python3 /home/ec2-user/dpdk/usertools/dpdk-devbind.py -b vfio-pci 7f:00.0
$ sudo python3 /home/ec2-user/dpdk/usertools/dpdk-devbind.py -s
.
 .
  .
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
