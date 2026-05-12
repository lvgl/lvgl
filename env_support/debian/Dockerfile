FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

COPY env_support/debian/control /tmp/control
COPY env_support/debian/extract-build-deps.py /tmp/extract-build-deps.py

RUN apt-get update && \
    apt-get install -y software-properties-common && \
    rm -rf /var/lib/apt/lists/*

RUN add-apt-repository ppa:andrecosta/fastgltf && \
    apt-get update && \
    apt-get install -y dpkg-dev debhelper && \
    apt-get install -y $(./tmp/extract-build-deps.py) && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
