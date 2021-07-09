FROM debian:stretch-slim AS build

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install --assume-yes apt-utils

# Useful packages
RUN apt-get update && apt-get install -y \
        ca-certificates \
        curl \
        procps \
        wget \
    && rm -rf /var/lib/apt/lists/*

# Build tools
RUN apt-get update && apt-get install -y \
        cmake \
        g++ \
        make \
    && rm -rf /var/lib/apt/lists/*

# Build dependencies
RUN apt-get update && apt-get install -y \
        libzmq3-dev \
        libprotobuf-dev \
        protobuf-compiler \
    && rm -rf /var/lib/apt/lists/*

ENV ERSAP_HOME="/usr/local/ersap"
ENV PATH="${ERSAP_HOME}/bin:${PATH}"
ENV LD_LIBRARY_PATH="${ERSAP_HOME}/lib"

RUN mkdir -p /usr/local/src
WORKDIR /usr/local/src

# Install xMsg
ARG xmsg_rev="master"
ARG xmsg_api_url="https://api.github.com/repos/JeffersonLab/xmsg-cpp"
ADD ${xmsg_api_url}/git/refs/heads/${xmsg_rev} xmsg.json
RUN mkdir -p xmsg-cpp && cd xmsg-cpp \
    && curl -L ${xmsg_api_url}/tarball/${xmsg_rev} | tar xz --strip=1 -C . \
    && ./configure --prefix=${ERSAP_HOME} --disable-tests \
    && make && make install \
    && rm -rf /usr/local/src/xmsg-cpp

# Build ERSAP
ADD . ersap-cpp
RUN cd ersap-cpp \
    && ./configure --prefix=${ERSAP_HOME} --disable-tests \
    && make && make install \
    && rm -rf build

WORKDIR /root
VOLUME ["${ERSAP_HOME}/data/input", "${ERSAP_HOME}/data/output", "${ERSAP_HOME}/log"]

EXPOSE 7781-7785


FROM debian:stretch-slim

# Useful packages
RUN apt-get update && apt-get install -y \
        procps \
    && rm -rf /var/lib/apt/lists/*

# Run dependencies
RUN apt-get update && apt-get install -y \
        libzmq5 \
        libprotobuf10 \
    && rm -rf /var/lib/apt/lists/*

ENV ERSAP_HOME="/usr/local/ersap"
ENV PATH="${ERSAP_HOME}/bin:${PATH}"
ENV LD_LIBRARY_PATH="${ERSAP_HOME}/lib"

COPY --from=build ${ERSAP_HOME} ${ERSAP_HOME}

WORKDIR /root
VOLUME ["${ERSAP_HOME}/data/input", "${ERSAP_HOME}/data/output", "${ERSAP_HOME}/log"]

EXPOSE 7781-7785
