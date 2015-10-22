## CROssPlatformS (CROPS)

CROPS is an interface between self-contained Linux cross-compilation environments and Linux or non-Linux (Windows/Mac OS X) development hosts. CROPS features three main components:

* **CEED** - runs on the development host and exposes an API to Integrated
Development Environments (IDEs)
* **TURFF** - runs on the build device and services requests from CEED
* **CODI** - COntainer DIspatcher which keeps track of all available TURFF instances
in its internal table and redirects CEED requests to the corresponding TURFF instance


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
