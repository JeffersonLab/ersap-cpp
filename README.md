# ERSAP C++ Binding

An ***E***nvironment for ***R***ealtime ***S***treaming ***A***cquisition and ***P***rocessing,
designed to process unbounded streams of continuous data at scale over distributed heterogeneous resources.
 
### ERSAP is modular
ERSAP presents a micro-services architecture for data-stream analytics. One can think of ERSAP as a software LEGO system to design and deploy scientific data processing applications without writing a single line of code. It is a data-in-motion platform to build streaming scientific-data analytics applications.
### ERSAP is resilient
ERSAP application stays responsive in the face of a failure. Resilience is achieved by service replication, failure containment, isolation and delegation. Failures are contained within each service, isolating services from each other and thereby ensuring that parts of the system can fail and recover without compromising the system as a whole. 
### ERSAP is elastic
ERSAP system stays amenable under varying workloads. It reacts to changes in the input data-stream rate by increasing or decreasing the resource allocation to process an input stream. ERSAP orchestrator implements predictive, as well as reactive scaling algorithms by providing relevant live performance measures. We achieve elasticity in a cost-effective way on commodity hardware and software platforms.
### ERSAP is reactive
ERSAP uses asynchronous message-passing to establish boundaries between services that ensure loose coupling, isolation, location transparency, and provides means to delegate errors as messages. Employing explicit message-passing enables load balancing and overall data-flow, i.e. application algorithm control and orchestration.


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

xMsg is bundled with ERSAP and it will be built automatically.

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
to run C++ services with the standard orchestrator and the ERSAP shell.


For assistance send email to [ersap@jlab.org](mailto:ersap@jlab.org).
