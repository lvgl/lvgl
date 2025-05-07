FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture i386
RUN apt update

COPY ./scripts/prerequisites-apt.txt .
COPY ./scripts/prerequisites-pip.txt .

RUN cat prerequisites-apt.txt | xargs apt install -y

RUN pip install --break-system-packages -r prerequisites-pip.txt

RUN git clone https://github.com/kornelski/pngquant && \
	cd pngquant && \
	git checkout 2.17.0 && \
	./configure  && \
	make -j$(nproc) && \
	make install
RUN rm -rf pngquant


WORKDIR /work
