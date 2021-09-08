# ERSAP C++ Binding

An ***E***nvironment for ***R***ealtime ***S***treaming ***A***cquisition and ***P***rocessing,
designed to process unbounded streams of continuous data at scale over distributed heterogeneous resources.


![Build Status](https://github.com/JeffersonLab/ersap-cpp/workflows/Ersap%20CI/badge.svg)
[![Javadoc](https://img.shields.io/badge/doxygen-master-blue.svg?style=flat)](https://ersapweb.jlab.org/ersap/api/cpp/)


## Documentation

In progress...


## Build notes

ERSAP requires a C++14 compiler and CMake 3.5+

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

This will install ERSAP C++ into the location of your ERSAP distribution.

The main [ERSAP Java](https://github.com/JeffersonLab/ersap-java) implementation
should also be already installed in `$ERSAP_HOME`
to run C++ services with the standard orchestrator, and the ERSAP shell.


## Authors

* Vardan Gyurjyan
* Sebastián Mancilla

For assistance send email to [ersap@jlab.org](mailto:ersap@jlab.org).
