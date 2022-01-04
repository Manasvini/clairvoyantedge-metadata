!/bin/bash

# Print commands and exit on errors
set -xe

apt-get update

KERNEL=$(uname -r)
DEBIAN_FRONTEND=noninteractive apt-get -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" upgrade
apt-get install -y --no-install-recommends --fix-missing\
  autoconf \
  automake \
  build-essential \
  ca-certificates \
  curl \
  git \
  libssl-dev \
  linux-headers-$KERNEL\
  make \
  pkg-config \
  tcpdump \
  zip \
  unzip \
  vim \
  wget \
  xterm
