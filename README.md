# What is CROPS (CROssPlatformS)?

CROPS is an open source tool that leverages Docker containers to make it
easier to provide cross-compilers, toolchains, and SDKs to developers,
independent of the development host's OS.

CROPS allows Linux-based cross toolchains to be used on Linux, MacOS, and
Windows hosts through the use of containers.

#### CROPS components

* **CEED** - runs on the development host and exposes an API to Integrated
Development Environments (IDEs) or CLI callers
* **TURFF** - runs in a container and services requests from CODI
* **CODI** - COntainer DIspatcher runs in a container and keeps track of all available TURFF instances
in its internal table. CODI also redirects CEED requests to the corresponding TURFF instance

# Using CROPS With Your Own OE-Generated Toolchain

#### Preparation

1. Install [Docker (Linux)](https://docs.docker.com/linux/step_one/) or [Docker Toolbox (Windows/Mac)](https://www.docker.com/products/docker-toolbox)

1. Yocto/OE Preparation

   Following the information in the [Yocto Quick Start Guide](http://www.yoctoproject.org/docs/latest/yocto-project-qs/yocto-project-qs.html#the-linux-distro)
   install the necessary packages on your host in order to work with OE.

#### OE Image and SDK

1. Clone

   <pre>
   $ <b>mkdir -p ~/crops/oe</b>
   $ <b>cd ~/crops/oe</b>
   </pre>

   openembedded-core:

   <pre>
   $ <b>git clone git://git.openembedded.org/openembedded-core</b>
   Cloning into 'openembedded-core'...
   remote: Counting objects: 265969, done.
   remote: Compressing objects: 100% (70100/70100), done.
   remote: Total 265969 (delta 191164), reused 265703 (delta 190964)
   Receiving objects: 100% (265969/265969), 94.12 MiB | 1.30 MiB/s, done.
   Resolving deltas: 100% (191164/191164), done.
   Checking connectivity... done.
   </pre>

   bitbake:

   <pre>
   $ <b>git clone git://git.openembedded.org/bitbake</b>
   remote: Counting objects: 45963, done.
   remote: Compressing objects: 100% (10491/10491), done.
   remote: Total 45963 (delta 34350), reused 45558 (delta 34037)
   Receiving objects: 100% (45963/45963), 9.87 MiB | 2.66 MiB/s, done.
   Resolving deltas: 100% (34350/34350), done.
   Checking connectivity... done.
   </pre>

1. Source the OE Build Script

   <pre>
   $ <b>. openembedded-core/oe-init-build-env build bitbake/</b>
    You had no conf/local.conf file. This configuration file has therefore been
    created for you with some default values. You may wish to edit it to, for
    example, select a different MACHINE (target hardware). See conf/local.conf
    for more information as common configuration options are commented.
    
    You had no conf/bblayers.conf file. This configuration file has therefore been
    created for you with some default values. To add additional metadata layers
    into your configuration please add entries to conf/bblayers.conf.
    
    The Yocto Project has extensive documentation about OE including a reference
    manual which can be found at:
        http://yoctoproject.org/documentation
    
    For more information about OpenEmbedded see their website:
        http://www.openembedded.org/
    
    ### Shell environment set up for builds. ###
    
    You can now run 'bitbake <target>'
    
    Common targets are:
        core-image-minimal
        core-image-sato
        meta-toolchain
        meta-ide-support
    
    You can also run generated qemu images with a command like 'runqemu qemux86'
   </pre>

1. Build Image

   <pre>
   $ <b>MACHINE=qemuarm64 bitbake core-image-full-cmdline</b>
   Parsing recipes: 100% |####################| Time: 0:00:07
   Parsing of 863 .bb files complete (0 cached, 863 parsed). 1316 targets, 86 skipped, 0 masked, 0 errors.
   NOTE: Resolving any missing task queue dependencies

   Build Configuration:
   BB_VERSION        = "1.31.2"
   BUILD_SYS         = "x86_64-linux"
   NATIVELSBSTRING   = "SUSELINUX-42.1"
   TARGET_SYS        = "aarch64-oe-linux"
   MACHINE           = "qemuarm64"
   DISTRO            = "nodistro"
   DISTRO_VERSION    = "nodistro.0"
   TUNE_FEATURES     = "aarch64"
   TARGET_FPU        = ""
   meta              = "master:4a1c04c0d509b2cda9b2ccd5a80523c05fa279c6"
   
   Initialising tasks: 100% |####################| Time: 0:00:04
   NOTE: Executing SetScene Tasks
   NOTE: Executing RunQueue Tasks
   NOTE: Tasks Summary: Attempted 2664 tasks of which 11 didn't need to be rerun and all succeeded.
   </pre>

1. Build SDK

   <pre>
   $ <b>MACHINE=qemuarm64 bitbake core-image-full-cmdline -c populate_sdk</b>
   Loading cache: 100% |####################| Time: 0:00:00
   Loaded 1316 entries from dependency cache.
   NOTE: Resolving any missing task queue dependencies

   Build Configuration:
   BB_VERSION        = "1.31.2"
   BUILD_SYS         = "x86_64-linux"
   NATIVELSBSTRING   = "SUSELINUX-42.1"
   TARGET_SYS        = "aarch64-oe-linux"
   MACHINE           = "qemuarm64"
   DISTRO            = "nodistro"
   DISTRO_VERSION    = "nodistro.0"
   TUNE_FEATURES     = "aarch64"
   TARGET_FPU        = ""
   meta              = "master:4a1c04c0d509b2cda9b2ccd5a80523c05fa279c6"

   Initialising tasks: 100% |####################| Time: 0:00:04
   NOTE: Executing SetScene Tasks
   NOTE: Executing RunQueue Tasks
   NOTE: Tasks Summary: Attempted 2854 tasks of which 1993 didn't need to be rerun and all succeeded.
   </pre>

#### CROPS Containers

1. Checkout CROPS Project

   <pre>
   $ <b>cd ~/crops</b>
   $ <b>git clone https://github.com/twoerner/crops</b>
   Cloning into 'crops'...
   remote: Counting objects: 688, done.
   remote: Total 688 (delta 0), reused 0 (delta 0), pack-reused 688
   Receiving objects: 100% (688/688), 133.82 KiB | 69.00 KiB/s, done.
   Resolving deltas: 100% (406/406), done.
   $ <b>cd crops/dockerfiles</b>
   </pre>

1. Build CODI dependencies container

   Execute the following command from within the dockerfiles directory:

   <pre>
   $ <b>docker build -t crops/codi:deps -f Dockerfile.codi.deps .</b>
   Sending build context to Docker daemon 17.92 kB
   Step 1 : FROM debian:wheezy
    ---> d465a57cd01b
   Step 2 : MAINTAINER Todor Minchev &lt;todor.minchev@linux.intel.com>
    ---> Running in 4eb93a20a1b6
    ---> f547e0b8730b
   Removing intermediate container 4eb93a20a1b6
   Step 3 : RUN apt-get update -qq && apt-get install -y -qq       libsqlite3-dev  libjansson-dev          git             wget    daemontools     autoconf  automake         libtool         build-essential &&      apt-get upgrade -y -qq
    ---> Running in 1aafedbabf3e
   debconf: delaying package configuration, since apt-utils is not installed
   (Reading database ... 6732 files and directories currently installed.)
   Preparing to replace perl-base 5.14.2-21+deb7u3 (using .../perl-base_5.14.2-21+deb7u4_amd64.deb) ...
   Unpacking replacement perl-base ...
   Setting up perl-base (5.14.2-21+deb7u4) ...
   ...
   <i>...lots of output later...</i>
   ...
   Removing automake ...
   Removing autoconf ...
   Purging configuration files for autoconf ...
   Removing libtool ...
    ---> c4c6095cce4a
   Removing intermediate container 605eb3849607
   Successfully built c4c6095cce4a
   </pre>

1. Build CODI container

   Execute the following command from within the dockerfiles directory
   (replace &lt;ver> with a version string that is significant to you):

   <pre>
   $ <b>docker build -t crops/codi:&lt;ver> -f Dockerfile.codi ../</b>
   Sending build context to Docker daemon 272.4 kB
   Step 1 : FROM crops/codi:deps
    ---> c4c6095cce4a
   Step 2 : MAINTAINER Todor Minchev &lt;todor.minchev@linux.intel.com>
    ---> Running in 9831684db8f7
    ---> ea2a29c31ea8
   Removing intermediate container 9831684db8f7
   Step 3 : RUN mkdir -p /usr/local/crops/codi/
    ---> Running in 24ef7267cf72
    ---> 6c15ec8777eb
   Removing intermediate container 24ef7267cf72
   Step 4 : COPY codi /usr/local/crops/codi/
    ---> 638081218613
   Removing intermediate container 75824284243f
   Step 5 : COPY utils.[ch] /usr/local/crops/
    ---> 63d416f5d640
   Removing intermediate container f5ae0a776ab6
   Step 6 : COPY globals.[ch] /usr/local/crops/
    ---> 09205a264462
   Removing intermediate container b0ca1ff036d0
   Step 7 : ARG build_type
    ---> Running in 3dd336892ba4
    ---> 76166270190a
   Removing intermediate container 3dd336892ba4
   Step 8 : RUN cd /usr/local/crops/codi &&        make $build_type &&     mkdir -p /bin/codi &&   cp /usr/local/crops/codi/codi /bin/codi/run &&  rm -rf /usr/local/crops
    ---> Running in 874f071a90cf
   cc -c -o codi.o codi.c -g -I. -I../
   cc -c -o codi_db.o codi_db.c -g -I. -I../
   cc -c -o codi_list.o codi_list.c -g -I. -I../
   cc -c -o codi_launcher.o codi_launcher.c -g -I. -I../
   cc -c -o codi_api.o codi_api.c -g -I. -I../
   cc -c -o ../utils.o ../utils.c -g -I. -I../
   cc -c -o ../globals.o ../globals.c -g -I. -I../
   cc -o codi codi.o codi_db.o codi_list.o codi_launcher.o codi_api.o ../utils.o ../globals.o -g -I. -I../ -lsqlite3 -lm -ljansson -lcurl -lpthread
    ---> 5189dc7dd0a2
   Removing intermediate container 874f071a90cf
   Step 9 : ENTRYPOINT supervise /bin/codi
    ---> Running in b1196d7abd6c
    ---> e1368c9011ee
   Removing intermediate container b1196d7abd6c
   Step 10 : EXPOSE 10000
    ---> Running in 3d20210130c4
    ---> 5b451097b87a
   Removing intermediate container 3d20210130c4
   Successfully built 5b451097b87a
   </pre>

1. Build toolchain dependencies container

   Execute the following command from within the dockerfiles directory:

   <pre>
   $ <b>docker build -t crops/oe:deps -f Dockerfile.oe.deps .</b>
   Sending build context to Docker daemon 17.92 kB
   Step 1 : FROM debian:wheezy
    ---> d465a57cd01b
   Step 2 : MAINTAINER Todor Minchev <todor.minchev@linux.intel.com>
    ---> Using cache
    ---> f547e0b8730b
   Step 3 : RUN apt-get update -qq && apt-get install -y -qq       python          daemontools     git     build-essential         wget
    ---> Running in dc16b39113df
   debconf: delaying package configuration, since apt-utils is not installed
   (Reading database ... 6732 files and directories currently installed.)
   Preparing to replace perl-base 5.14.2-21+deb7u3 (using .../perl-base_5.14.2-21+deb7u4_amd64.deb) ...
   Unpacking replacement perl-base ...
   Setting up perl-base (5.14.2-21+deb7u4) ...
   Selecting previously unselected package libgpg-error0:amd64.
   (Reading database ... 6732 files and directories currently installed.)
   ...
   <i>...lots of output later...</i>
   ...
   Setting up libalgorithm-merge-perl (0.08-2) ...
   Setting up libfile-fcntllock-perl (0.14-2) ...
   Processing triggers for ca-certificates ...
   Updating certificates in /etc/ssl/certs... 171 added, 0 removed; done.
   Running hooks in /etc/ca-certificates/update.d....done.
    ---> 58185aafd647
   Removing intermediate container dc16b39113df
   Successfully built 58185aafd647
   </pre>

1. Open Dockerfile.oe-local and provide the name of your toolchain

   Example:

   <pre>
   ENV TOOLCHAIN_NAME oecore-x86_64-aarch64-toolchain-nodistro.0.sh
   </pre>

   Also, copy the toolchain to this project's top-level directory.

   <pre>
   $ <b>cp ~/crops/oe/build/tmp-glibc/deploy/sdk/oecore-x86_64-aarch64-toolchain-nodistro.0.sh ~/crops/crops</b>
   </pre>

1. Build your toolchain container

   (replace &lt;ver> with something significant to you)

   <pre>
   $ <b>docker build -t crops/toolchain:&lt;ver> -f Dockerfile.oe-local ..</b>
   </pre>
   ```
   Sending build context to Docker daemon 168.9 MB
   Step 1 : FROM crops/oe:deps
    ---> 58185aafd647
   Step 2 : MAINTAINER Trevor Woerner <twoerner@gmail.com>
    ---> Running in ca1957ffe7c2
    ---> 354714c32f12
   Removing intermediate container ca1957ffe7c2
   Step 3 : ENV TOOLCHAIN_NAME oecore-x86_64-aarch64-toolchain-nodistro.0.sh
    ---> Running in 1c9fb6a402cf
    ---> 007bdf40afd4
   Removing intermediate container 1c9fb6a402cf
   Step 4 : RUN mkdir -p /usr/local/crops/turff/
    ---> Running in 50309b8f7db3
    ---> 2d826a7cce65
   Removing intermediate container 50309b8f7db3
   Step 5 : COPY turff /usr/local/crops/turff/
    ---> 26ca51e46d05
   Removing intermediate container d3db88bd9652
   Step 6 : COPY utils.[ch] /usr/local/crops/
    ---> de0db5dd6249
   Removing intermediate container 8ac4c049d81c
   Step 7 : COPY globals.[ch] /usr/local/crops/
    ---> 3977635f9ebf
   Removing intermediate container 90a5eda4c3bc
   Step 8 : COPY $TOOLCHAIN_NAME /tmp
    ---> 02e72963d558
   Removing intermediate container 9654fad5a093
   Step 9 : RUN cd /usr/local/crops/turff &&       make &&         mkdir -p /bin/turff &&  cp /usr/local/crops/turff/turff /bin/turff/run &&       cp /usr/local/crops/turff/turff_launcher /bin/
    ---> Running in dfcee065d163
   cc -c -o turff.o turff.c -g -I. -I../
   cc -c -o turff_api.o turff_api.c -g -I. -I../
   cc -c -o ../utils.o ../utils.c -g -I. -I../
   cc -c -o ../globals.o ../globals.c -g -I. -I../
   cc -o turff turff.o turff_api.o ../utils.o ../globals.o -g -I. -I../ -lm
    ---> 55d4dd20a45a
   Removing intermediate container dfcee065d163
   Step 10 : RUN cd /tmp &&                chmod 755 ./${TOOLCHAIN_NAME} &&                ./${TOOLCHAIN_NAME} -d /opt/poky/ -y
    ---> Running in 8329689984fe
   OpenEmbedded SDK installer version nodistro.0
   =============================================
   You are about to install the SDK to "/opt/poky". Proceed[Y/n]? Y
   Extracting SDK.......................................done
   Setting it up...done
   SDK has been successfully set up and is ready to be used.
   Each time you wish to use the SDK in a new shell session, you need to source the environment setup script e.g.
    $ . /opt/poky/environment-setup-aarch64-oe-linux
    ---> a69b8c3c4155
   Removing intermediate container 8329689984fe
   Step 11 : RUN chmod 755 /opt/poky/environment-setup*
    ---> Running in 608ffaf44490
    ---> f8c20c5bc25e
   Removing intermediate container 608ffaf44490
   Step 12 : ENTRYPOINT /bin/turff_launcher -f /opt/poky/environment-setup*
    ---> Running in c87c5230e61b
    ---> 18395aa90519
   Removing intermediate container c87c5230e61b
   Successfully built 18395aa90519
   ```

1. Start CODI container

   <pre>
   $ <b>docker run -d --name codi-test -v /var/run/docker.sock:/var/run/docker.sock --net=host crops/codi:&lt;ver></b>
   caae1eb3dcc7103ba8dc89e6e75c84319418efb51552247d773e1cc4f5610768
   </pre>

1. Create a location for your project's code

   <pre>
   $ <b>mkdir -p $HOME/crops-workspace</b>
   </pre>

1. Start toolchain container

   <pre>
   $ <b>docker run -d --name my_toolchain -v $HOME/crops-workspace/:/crops/ --env TURFFID=my_toolchain --net=host crops/oe:&lt;ver></b>
   d648cf6d950c5478c2459a6c9f3e2e1d8f5878361b5e61524b023fd5f642e902
   </pre>

1. Build and install the ceed tool

   From the top-level directory, change into the "ceed" directory and:

   <pre>
   $ <b>cd ~/crops/ceed</b>
   $ <b>make</b>
   cc -c -o ceed.o ceed.c -g -I. -I../
   cc -c -o ceed_api.o ceed_api.c -g -I. -I../
   cc -c -o ../utils.o ../utils.c -g -I. -I../
   cc -c -o ../globals.o ../globals.c -g -I. -I../
   cc -o ceed ceed.o ceed_api.o ../utils.o ../globals.o -g -I. -I../ -lm
   </pre>

   Place the resulting "ceed" executable somehwere in your $PATH

# Develop Your Code

1. Place your project in the shared workspace

   Example:
   <pre>
   $HOME/crops-workspace/my_project/
   </pre>

1. Build your project

   Example:
   <pre>
   $ <b>ceed -d my_toolchain -g "make -C /crops/my_project/"</b>
   [INFO] Connected to CODI on 127.0.0.1 port : 10000
   [INFO] Host Architecture : linux
   [INFO]  ==========  CEED OUTPUT BEGIN  ==========
   make: Entering directory `/crops/hello'
   aarch64-oe-linux-gcc  --sysroot=/opt/poky/sysroots/aarch64-oe-linux  -O2 -pipe -g -feliminate-unused-debug-types   -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed  hello.c   -o hello
   make: Leaving directory `/crops/hello'
   [INFO] Connected to codi on 127.0.0.1:10000
   [INFO] TURFF listening on port: 9999
   [INFO]  ==========  CEED OUTPUT END  ==========
   </pre>

   Verify:
   <pre>
   $ <b>file ~/crops-workspace/my_project/hello</b>
   ~/crops-workspace/my_project/hello: ELF 64-bit LSB executable, ARM aarch64, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 3.14.0, BuildID[sha1]=811acb3821620ca06f7582317752bdc4dcecbe30, not stripped
   </pre>

# On-Target Testing

#### Run The Target (qemu)

   Now we're going to test our build results on the target device. For this
   example, our target is an ARM 64-bit virtual machine running under qemu.
   The same basic principles apply if your device is real hardware (such as
   a raspberrypi, minnow, dragonboard, etc).

   Make sure to run the following from the exact same terminal that performed
   the OE build above. If you have closed the build terminal, simply perform
   all the following steps, otherwise you can skip the first 2 steps:

   <pre>
   $ <b>cd ~/crops/oe</b>
   $ <b>. openembedded-core/oe-init-build-env build bitbake/</b>
   </pre>
   ```
    
    ### Shell environment set up for builds. ###
    
    You can now run 'bitbake <target>'
    
    Common targets are:
        core-image-minimal
        core-image-sato
        meta-toolchain
        meta-ide-support
    
    You can also run generated qemu images with a command like 'runqemu qemux86'
   ```

   <pre>
   $ <b>cd ~/crops/oe/build</b>
   $ <b>runqemu qemuarm64</b>
   runqemu - INFO - Assuming MACHINE = qemuarm64
   runqemu - INFO - Running MACHINE=qemuarm64 bitbake -e...
   runqemu - INFO - MACHINE: qemuarm64
   runqemu - INFO - DEPLOY_DIR_IMAGE: ~/crops/oe/build/tmp-glibc/deploy/images/qemuarm64
   runqemu - INFO - Running ls -t ~/crops/oe/build/tmp-glibc/deploy/images/qemuarm64/*.qemuboot.conf...
   runqemu - INFO - CONFFILE: ~/crops/oe/build/tmp-glibc/deploy/images/qemuarm64/core-image-full-cmdline-qemuarm64-20161005224755.qemuboot.conf
   runqemu - INFO - Continuing with the following parameters:

   KERNEL: [~/crops/oe/build/tmp-glibc/deploy/images/qemuarm64/Image]
   MACHINE: [qemuarm64]
   FSTYPE: [ext4]
   ROOTFS: [~/crops/oe/build/tmp-glibc/deploy/images/qemuarm64/core-image-full-cmdline-qemuarm64-20161005224755.rootfs.ext4]
   CONFFILE: [~/crops/oe/build/tmp-glibc/deploy/images/qemuarm64/core-image-full-cmdline-qemuarm64-20161005224755.qemuboot.conf]

   runqemu - INFO - Running /bin/ip link...
   runqemu - INFO - Setting up tap interface under sudo
   root's password: <b><i>enter your host's root password</i></b>
   runqemu - INFO - Acquiring lockfile /tmp/qemu-tap-locks/tap0.lock...
   runqemu - INFO - Created tap: tap0
   runqemu - INFO - Running ldd ~/crops/oe/build/tmp-glibc/sysroots/x86_64-linux/usr/bin/qemu-system-aarch64...
   runqemu - INFO - Running ~/crops/oe/build/tmp-glibc/sysroots/x86_64-linux/usr/bin/qemu-system-aarch64 -netdev tap,id=net0,ifname=tap0,script=no,downscript=no -device virtio-net-device,netdev=net0,mac=52:54:00:12:34:02  -nographic -machine virt -cpu cortex-a57 -m 512 -drive id=disk0,file=~/crops/oe/build/tmp-glibc/deploy/images/qemuarm64/core-image-full-cmdline-qemuarm64-20161005224755.rootfs.ext4,if=none,format=raw -device virtio-blk-device,drive=disk0 -show-cursor -device virtio-rng-pci -kernel ~/crops/oe/build/tmp-glibc/deploy/images/qemuarm64/Image -append 'root=/dev/vda rw highres=off  console=ttyS0 mem=512M ip=192.168.7.2::192.168.7.1:255.255.255.0 console=ttyAMA0,38400'
   [    0.000000] Booting Linux on physical CPU 0x0
   [    0.000000] Linux version 4.8.0-yocto-standard (trevor@openSUSE-i7) (gcc version 6.2.0 (GCC) ) #1 SMP PREEMPT Wed Oct 5 19:25:49 EDT 2016
   [    0.000000] Boot CPU: AArch64 Processor [411fd070]
   [    0.000000] Memory limited to 512MB
   [    0.000000] efi: Getting EFI parameters from FDT:
   [    0.000000] efi: UEFI not found.
   [    0.000000] psci: probing for conduit method from DT.
   [    0.000000] psci: PSCIv0.2 detected in firmware.
   [    0.000000] psci: Using standard PSCI v0.2 function IDs
   [    0.000000] psci: Trusted OS migration not required
   [    0.000000] percpu: Embedded 20 pages/cpu @ffffffc01ffd5000 s41624 r8192 d32104 u81920
   [    0.000000] Detected PIPT I-cache on CPU0
   ...
   <i>...lots of output later, typical Linux bootup...</i>
   ...
   [    3.553343] EXT4-fs (vda): couldn't mount as ext3 due to feature incompatibilities
   [    3.556334] EXT4-fs (vda): couldn't mount as ext2 due to feature incompatibilities
   [    3.596300] EXT4-fs (vda): mounted filesystem with ordered data mode. Opts: (null)
   [    3.597108] VFS: Mounted root (ext4 filesystem) on device 253:0.
   [    3.633784] devtmpfs: mounted
   [    3.686902] Freeing unused kernel memory: 640K (ffffffc000860000 - ffffffc000900000)
   INIT: version 2.88 booting
   Error opening /dev/fb0: No such file or directory
   Starting udev
   [    5.342397] random: fast init done
   udev: Not using udev cache because of changes detected in the following files:
   udev:     /proc/version /proc/cmdline /proc/devices
   udev:     lib/udev/rules.d/* etc/udev/rules.d/*
   udev: The udev cache will be regenerated. To identify the detected changes,
   udev: compare the cached sysconf at   /etc/udev/cache.data
   udev: against the current sysconf at  /dev/shm/udev.cache
   [    6.183398] udevd[107]: starting version 3.2
   [    6.322232] udevd[108]: starting eudev-3.2
   [    7.975437] EXT4-fs (vda): re-mounted. Opts: (null)
   Populating dev cache
   INIT: Entering runlevel: 5
   Configuring network interfaces... done.
   Starting system message bus: dbus.
   Starting OpenBSD Secure Shell server: sshd
   done.
   Starting rpcbind daemon...done.
   starting statd: done
   Starting atd: OK
   exportfs: can't open /etc/exports for reading
   [   16.952733] Installing knfsd (copyright (C) 1996 okir@monad.swb.de).
   starting 8 nfsd kernel threads: [   17.440121] NFSD: Using /var/lib/nfs/v4recovery as the NFSv4 state recovery directory
   [   17.442080] NFSD: starting 90-second grace period (net ffffff800899e700)
   done
   starting mountd: done
   Starting system log daemon...0
   Starting kernel log daemon...0
   Starting crond: OK
   
   OpenEmbedded nodistro.0 qemuarm64 /dev/ttyAMA0
   
   qemuarm64 login: root
   root@qemuarm64:~#
   </pre>

#### Copy Build Artifacts to Target

1. Target IP

   On your target device, identify its IP address (in this example case
   the target's IP is <i>192.168.7.2</i>):

   <pre>
   root@qemuarm64:~# <b>ifconfig</b>
   eth0      Link encap:Ethernet  HWaddr 52:54:00:12:34:02
             inet addr:192.168.7.2  Bcast:192.168.7.255  Mask:255.255.255.0
             inet6 addr: fe80::5054:ff:fe12:3402/64 Scope:Link
             UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
             RX packets:29 errors:0 dropped:0 overruns:0 frame:0
             TX packets:9 errors:0 dropped:0 overruns:0 carrier:0
             collisions:0 txqueuelen:1000
             RX bytes:7515 (7.3 KiB)  TX bytes:738 (738.0 B)
   
   lo        Link encap:Local Loopback
             inet addr:127.0.0.1  Mask:255.0.0.0
             inet6 addr: ::1/128 Scope:Host
             UP LOOPBACK RUNNING  MTU:65536  Metric:1
             RX packets:2 errors:0 dropped:0 overruns:0 frame:0
             TX packets:2 errors:0 dropped:0 overruns:0 carrier:0
             collisions:0 txqueuelen:1
             RX bytes:140 (140.0 B)  TX bytes:140 (140.0 B)
   </pre>

1. Copy Artifacts

   On your host, check your networking configuration, you should have a
   <i>tap0</i> device as follows:

   <pre>
   $ <b>ifconfig</b>
   ...
   <i>...other network interfaces' output...</i>
   ...
   tap0      Link encap:Ethernet  HWaddr B6:FC:C4:3C:FA:A3
             inet addr:192.168.7.1  Bcast:192.168.7.255  Mask:255.255.255.255
             inet6 addr: fe80::b4fc:c4ff:fe3c:faa3/64 Scope:Link
             UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
             RX packets:10 errors:0 dropped:0 overruns:0 frame:0
             TX packets:79 errors:0 dropped:0 overruns:0 carrier:0
             collisions:0 txqueuelen:500
             RX bytes:828 (828.0 b)  TX bytes:16866 (16.4 Kb)
   ...
   <i>...other network interfaces' output...</i>
   ...
   </pre>

   Now copy your cross-compiled artifact to the target:

   <pre>
   $ <b>scp -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null ~/crops-workspace/my_project/hello root@192.168.7.2:</b>
   Warning: Permanently added '192.168.7.2' (ECDSA) to the list of known hosts.
   hello                                          100%   14KB  13.7KB/s   00:00
   </pre>

1. Test

   Back on the target:
   
   <pre>
   root@qemuarm64:~# <b>./hello</b>
   Hello, world!

   root@qemuarm64:~# <b>/lib/ld-linux-aarch64.so.1 --list ./hello</b>
           linux-vdso.so.1 (0x0000007faaa84000)
           libc.so.6 => /lib/libc.so.6 (0x0000007faa937000)
           /lib/ld-linux-aarch64.so.1 (0x000000557dbf1000)
   </pre>

# Optional

1. Share your toolchain with other developers by pushing it to Docker Hub

   Example:
   <pre>
   $ <b>docker push crops/oe:&lt;ver></b>
   </pre>
