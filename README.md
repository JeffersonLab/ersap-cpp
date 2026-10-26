# ERSAP C++ Binding

An ***E***nvironment for ***R***ealtime ***S***treaming ***A***cquisition and ***P***rocessing,
designed to process unbounded streams of continuous data at scale over distributed heterogeneous resources.


![Build Status](https://github.com/JeffersonLab/ersap-cpp/workflows/Ersap%20CI/badge.svg)
[![Javadoc](https://img.shields.io/badge/doxygen-master-blue.svg?style=flat)](https://ersapweb.jlab.org/ersap/api/cpp/)


## Documentation

In progress...


## Build notes

ERSAP requires a C++17 compiler and CMake 3.5+

#### Ubuntu 18.04 and 20.04

Install GCC and CMake from the repositories:

    $ sudo apt install build-essential cmake

#### macOS

Install Xcode command line tools:

    $ xcode-select --install

Install CMake using [Homebrew](https://brew.sh/):

    $ brew install cmake

### Dependencies

ERSAP is built on top of the [xMsg](https://github.com/JeffersonLab/xmsg-cpp)
pub-sub messaging system, which requires
[Protocol Buffers](https://developers.google.com/protocol-buffers/docs/downloads)
and [ZeroMQ](http://zeromq.org/intro:get-the-software).

xMsg is bundled with ERSAP, and it will be built automatically.

#### Ubuntu 18.04 and 20.04

Install from the repositories:

    $ sudo apt install libzmq5-dev libprotobuf-dev protobuf-compiler

#### macOS

Use Homebrew:

    $ brew install zeromq protobuf

### Installation

To build with CMake a configure wrapper script is provided:

    $ ./configure --prefix="$ERSAP_HOME"
    $ make
    $ make install

This installs ERSAP C++ into the location of your ERSAP distribution.

#### Install layout

ERSAP honours CMake's `GNUInstallDirs`, so the native binaries follow the
host's convention:

| Host | `CMAKE_INSTALL_LIBDIR` | Where native binaries land |
|---|---|---|
| Debian / Ubuntu, macOS | `lib` | `$ERSAP_HOME/lib/{libersap.so, libxmsg.so, ersap/c_dpe, pkgconfig/, cmake/}` |
| RHEL / EL / Fedora (64-bit) | `lib64` | `$ERSAP_HOME/lib64/{libersap.so, libxmsg.so, ersap/c_dpe, pkgconfig/, cmake/}` |

Either way, the installed `$ERSAP_HOME/bin/c_dpe` wrapper is templated at
build time to point at the right libdir, so there's nothing to configure
on the user side. Pass `-DCMAKE_INSTALL_LIBDIR=lib` by hand only if you
need to override the default (for example when packing a Docker image where
you want jars and natives in a single `lib/`):

    $ cmake -S . -B build -DCMAKE_INSTALL_PREFIX="$ERSAP_HOME" -DCMAKE_INSTALL_LIBDIR=lib
    $ cmake --build build -j && cmake --install build

#### Runtime library resolution

On Linux the installed `libersap.so` / `libxmsg.so` carry a RUNPATH that
includes `$ORIGIN` plus every directory CMake saw at link time (protobuf,
abseil, Boost, ZeroMQ, ...). As long as those libraries were present at
build time, the installed objects resolve under `ldd` without any
`LD_LIBRARY_PATH` or `/etc/ld.so.conf.d/*` edits. On macOS the equivalent
is handled by `@loader_path`.

#### Interoperation with ERSAP Java

The main [ERSAP Java](https://github.com/JeffersonLab/ersap-java)
implementation should also be installed in `$ERSAP_HOME` to run C++
services with the standard orchestrator and the ERSAP shell. Its jars and
`run-java` wrapper always live in `$ERSAP_HOME/lib/` (arch-neutral content),
so Java and native sides coexist cleanly on both `lib`-style and
`lib64`-style hosts.


## Authors
* Vardan Gyurjyan
* Sebastián Mancilla

For assistance send email to [ersap@jlab.org](mailto:ersap@jlab.org).
