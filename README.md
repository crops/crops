## What is CROPS (CROssPlatformS)?

CROPS is an open source, cross-platform development framework that leverages Docker containers to provide an easily managed, extensible environment which allows developers to build binaries for a variety of architectures of Windows, Linux and Mac OS X hosts.

### CROPS components
* **CEED** - runs on the development host and exposes an API to Integrated
Development Environments (IDEs) or CLI callers
* **TURFF** - runs in a container and services requests from CODI
* **CODI** - COntainer DIspatcher runs in a container and keeps track of all available TURFF instances
in its internal table. CODI also redirects CEED requests to the corresponding TURFF instance

## USING CROPS FOR [ZEPHYR OS](https://www.zephyrproject.org/) DEVELOPMENT

Please refer to the [GitHub Wiki](https://github.com/todorez/crops/wiki)

## USING CROPS WITH YOUR OWN TOOLCHAIN

 1. **Install [Docker (Linux)](https://docs.docker.com/linux/step_one/) or [Docker Toolbox (Windows/Mac)](https://www.docker.com/products/docker-toolbox)**

 2. **Build CODI dependencies container**

  Execute the following command from within the dockerfiles directory:

  ```
  docker build -t crops/codi:deps -f Dockerfile.codi.deps .
  ```
 3. **Build CODI container**

  Execute the following command from within the dockerfiles directory:

  ```
  docker build -t crops/codi:version -f Dockerfile.codi ../
  ```

 4. **Build toolchain dependencies container**

  Execute the following command from within the dockerfiles directory:

  ```
  docker build -t crops/toolchain:deps -f Dockerfile.toolchain.deps .
  ```

 5. **Open the Dockerfile.toolchain file and provide the URL to your toolchain**

  Example:

  ```
  ENV TOOLCHAIN_NAME poky-glibc-x86_64-core-image-sato-i586-toolchain-2.0.sh
  ENV TOOLCHAIN_PATH http://downloads.yoctoproject.org/releases/yocto/yocto-2.0/toolchain/x86_64/
  ```

 6. **Build your toolchain container**

  ```
  docker build -t crops/toolchain:my_toolchain -f Dockerfile.toolchain ../
  ```

 7. **Start CODI container**

  ```
  docker run -d --name codi-test -v /var/run/docker.sock:/var/run/docker.sock --net=host crops/codi:version
  ```

 8. **Start toolchain container**

  ```
  mkdir -p $HOME/crops-workspace && docker run -d --name crops-toolchain-my_toolchain -v $HOME/crops-workspace/:/crops/   --env TURFFID=crops-toolchain-my_toolchain --net=host crops/toolchain:my_toolchain
  ```

 9. **Run the Zephyr installer and answer "Yes" when prompted to install CEED. Answer "No" to all other questions**

  ```
  curl -kOs https://raw.githubusercontent.com/todorez/crops/master/installers/zephyr-installer.sh && source   ./zephyr-installer.sh
  ```

 10. **Place your project in the shared workspace**

  Example:
  ```
  $HOME/crops-workspace/my_project/
  ```

 11. **Build your project**

  Example:
  ```
  $HOME/.crops/ceed/ceed -d crops-toolchain-my_toolchain -g "make -C /crops/my_project/"
  ```
 12. **Share your toolchain with other developers by pushing it to Docker Hub**

  Example:
  ```
  $docker push crops/toolchain:my_toolchain"
  ```

## CONTRIBUTING TO CROPS

#### COMPILE CEED, TURFF AND CODI ON LINUX

**Required Prerequisites**

* **libsqlite3-dev** - "SQLite is a C library that implements an SQL database engine."
* **libcurl4-openssl-dev (7.40 or later)** - "libcurl is an easy-to-use client-side URL transfer library, supporting DICT, FILE, FTP, FTPS, GOPHER, HTTP, HTTPS, IMAP, IMAPS, LDAP, LDAPS, POP3, POP3S, RTMP, RTSP, SCP, SFTP, SMTP, SMTPS, TELNET and TFTP."
* **libjansson-dev** - "Jansson is a C library for encoding, decoding and manipulating JSON data."


1. **Install Prerequisites On Debian / Ubuntu**

  ```
  apt-get install libsqlite3-dev libcurl4-openssl-dev libjansson-dev
  ```

2. **Compile**

  * GCC

  ```
  CC=gcc make all
  ```

  * CLANG

  ```
  CC=clang make all
  ```

3. **Debug Compile**

  * GCC

  ```
  CC=gcc make debug
  ```

  * CLANG

  ```
  CC=clang make debug
  ```

#### RUNNING A CLANG STATIC ANALYSYS

1. **Run the static analyzer**

  ```
  scan-build -V make
  ```

2. **Point your browser at the following URL to view the static analysis results**

  ```
  http://127.0.0.1:8181
  ```
