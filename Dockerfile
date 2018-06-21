# ===============================================================================
# Dockerfile
#   Builds nycoin and the executable binary, "newyorkcd"
#
#
# It is based on Ubuntu 14.04 LTS
# ===============================================================================

# Set the base image to Ubuntu
FROM ubuntu:14.04

# File Author / Maintainer
MAINTAINER jmsds

# ===============================================================================
# Env. Setup
#

# Update repository
RUN apt-get update && apt-get -y upgrade

# ----------------------------------------------------------
# Dependencies
# ----------------------------------------------------------

# Basic Dependencies
#
RUN apt-get install -y git \
            telnet \
            ntp \
            unzip \
            build-essential \
            libtool \
            pkg-config \
            bsdmainutils \
            libssl-dev \
            libdb5.1++-dev \
            libdb5.1-dev \
            libboost-all-dev \
            libqrencode-dev \
            libminiupnpc-dev \
            libevent-dev \
            miniupnpc \
            autoconf \
            autogen

# ===============================================================================
# Set working directory
#
WORKDIR /work

# ===============================================================================
# Install configuration
#

RUN mkdir -p /root/.newyorkcoin

# ===============================================================================
# System Initialization
#

## Copy folders
RUN git clone https://github.com/MrSlosh/NewYorkCoin-AuxPow.git /work
RUN cd /work && \
    git checkout v1.3 && \
    chmod a+x autogen.sh && \
    ./autogen.sh && \
    ./configure && \
    /usr/bin/make

# Set default CMD
CMD /work/src/newyorkcoind && tail -f -n 10 /root/.newyorkcoin/debug.log

EXPOSE 17020 27020 18823 2882
