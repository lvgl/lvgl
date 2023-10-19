# Stage 1

FROM alpine:latest as stage1

# AArch32
#RUN apk update; \
#    apk add make cmake gcc-arm-none-eabi libc-dev \
#    bison flex bash patch mount dtc \
#    dosfstools u-boot-tools net-tools \
#    bridge-utils iptables dnsmasq libressl-dev \
#    util-linux qemu-system-arm e2fsprogs

# AArch64
RUN apk update; \
    apk add make cmake gcc-aarch64-none-elf libc-dev \
    bison flex bash patch mount dtc \
    dosfstools u-boot-tools net-tools \
    bridge-utils iptables dnsmasq libressl-dev \
    util-linux qemu-system-aarch64 e2fsprogs


RUN cd /; \
    wget https://github.com/smartobjectoriented/so3/archive/refs/heads/master.zip; \
    unzip master.zip; mv so3-* so3

WORKDIR so3

RUN     find / -name thumb | xargs rm -r; \
        patch -p1 < ci/so3_ci.patch; \
        cd u-boot; make virt64_defconfig; make -j8; cd ..; \
        cd so3; make virt64_defconfig; make -j8; cd ..; \
        cd usr; ./build.sh

# Stage 2

FROM scratch
COPY --from=stage1 / /
WORKDIR so3
EXPOSE 1234

CMD ./st

