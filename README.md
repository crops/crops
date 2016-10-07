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

## USING CROPS WITH YOUR OWN TOOLCHAIN

1. Install [Docker (Linux)](https://docs.docker.com/linux/step_one/) or [Docker Toolbox (Windows/Mac)](https://www.docker.com/products/docker-toolbox)

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
   $ <b>docker build -t crops/toolchain:deps -f Dockerfile.toolchain.deps .</b>
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

1. Open Dockerfile.toolchain-local and provide the name of your toolchain

   Example:

   <pre>
   ENV TOOLCHAIN_NAME oecore-x86_64-aarch64-toolchain-nodistro.0.sh
   </pre>

   Also, copy the toolchain to this project's top-level directory.

   <pre>
   $ <b>cp /some/where/oecore-x86_64-aarch64-toolchain-nodistro.0.sh ..</b>
   </pre>

1. Build your toolchain container

   (replace &lt;ver> with something significant to you)

   <pre>
   $ <b>docker build -t crops/toolchain:&lt;ver> -f Dockerfile.toolchain-local ..</b>
   </pre>
   ```
   Sending build context to Docker daemon 168.9 MB
   Step 1 : FROM crops/toolchain:deps
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
   $ <b>docker run -d --name my_toolchain -v $HOME/crops-workspace/:/crops/ --env TURFFID=my_toolchain --net=host crops/toolchain:&lt;ver></b>
   d648cf6d950c5478c2459a6c9f3e2e1d8f5878361b5e61524b023fd5f642e902
   </pre>

1. Build and install the ceed tool

   From the top-level directory, change into the "ceed" directory and:

   <pre>
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

# Optional

1. Share your toolchain with other developers by pushing it to Docker Hub

   Example:
   <pre>
   $ <b>docker push crops/toolchain:&lt;ver></b>
   </pre>
