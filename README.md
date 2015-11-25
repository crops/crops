## CROssPlatformS (CROPS)

CROPS is an interface between self-contained Linux cross-compilation environments and Linux or non-Linux (Windows/Mac OS X) development hosts. CROPS features three main components:

* **CEED** - runs on the development host and exposes an API to Integrated
Development Environments (IDEs)
* **TURFF** - runs on the build device and services requests from CEED
* **CODI** - COntainer DIspatcher which keeps track of all available TURFF instances
in its internal table and redirects CEED requests to the corresponding TURFF instance

## PREREQUISITES

* **libsqlite3-dev** - "SQLite is a C library that implements an SQL database engine."

* **libcurl4-openssl-dev (7.40 or later)** - "libcurl is an easy-to-use client-side URL transfer library, supporting DICT, FILE, FTP, FTPS, GOPHER, HTTP, HTTPS, IMAP, IMAPS, LDAP, LDAPS, POP3, POP3S, RTMP, RTSP, SCP, SFTP, SMTP, SMTPS, TELNET and TFTP."

* **libjansson-dev** - "Jansson is a C library for encoding, decoding and manipulating JSON data."


**Install Prerequisites On Debian / Ubuntu**

```
apt-get install libsqlite3-dev libcurl4-openssl-dev libjansson-dev
```

## COMPILE

* GCC

```
CC=gcc make all
```

* CLANG

```
CC=clang make all
```

## DEBUG COMPILE

* GCC

```
CC=gcc make debug
```

* CLANG

```
CC=clang make debug
```

## INSTALL

* Transfer the turff binary found in the turff directory to you build device
* Transfer the ceed binary found in the ceed directory to your development host
* Transfer the codi binary found in the codi directory to a local or remote host

## RUNNING A CLANG STATIC ANALYSYS

* Run the static analyzer

```
scan-build -V make
```

* Point your browser at the following URL to view the static analysis results

```
http://127.0.0.1:8181
```
