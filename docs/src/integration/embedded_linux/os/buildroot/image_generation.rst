RPi4 custom image
#################

This chapter offers a detailed guide for creating a custom image for the
Raspberry Pi 4 (RPi4). Key Buildroot components and concepts will be
introduced. The process outlined here can be adapted for use with
other boards. For advanced features not covered in this guide, please consult
the `Buildroot manual <https://buildroot.org/downloads/manual/manual.html>`_

This guide was done on ``Ubuntu 22.04``

Prepare the workspace
*********************

The workspace is structured to utilize Buildroot without any modifications.
Execute the following commands:

.. code-block::

    mkdir rpi4_buildroot_lvgl && cd rpi4_buildroot_lvgl
    mkdir resources && cd resources
    mkdir board
    mkdir configs
    touch Config.in && touch external.desc && touch external.mk
    echo "name: rpi4_lvgl" >> external.desc
    echo "desc: rpi4 lvgl custom build" >> external.desc
    cd ..

Each folder utility will be explained throughout the guide.

Get Buildroot
*************

First, according to the `Builroot Manual <https://buildroot.org/downloads/
manual/manual.html>`_, Buildroot requires certain packages to be installed
before starting the build. Lets install them using Ubuntu's package manager.

.. code-block:: bash

    sudo apt install sed make binutils gcc g++ bash patch gzip bzip2 perl tar \
            cpio python3 unzip rsync wget libncurses-dev

Now that we have satisfied all the prerequisites lets download a stable release
of Buildroot.

.. code-block:: bash

    git clone --branch 2024.08 --depth 1 https://github.com/buildroot/buildroot.git

Create RPi4 image
*****************

Let's see if there is already a configuration for RPi4 in Buildroot:

.. code-block:: bash

    cd buildroot
    make list-defconfigs | grep rasp

There is a build available for RPi4 62 bits: ``raspberrypi4_64_defconfig``.

You can also find all the configurations in the Buildroot repository
``buildroot > configs``

Start with this configuration as a base, but this configuration needs to be
changed to meet lvgl requirements. So let's copy this base and keep it out of
Buildroot.

.. code-block:: bash

    # move to the project root
    cp buildroot/configs/raspberrypi4_64_defconfig resources/configs/raspberrypi4_lvgl_defconfig

Create a new file ``build.sh`` at the root of the project with this content:

.. code-block:: bash

    #!/bin/bash
    SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

    cd $SCRIPTPATH
    mkdir -p output
    cd buildroot
    make defconfig O=../output BR2_DEFCONFIG=../resources/configs/raspberrypi4_lvgl_defconfig

Make this file executable.

.. code-block:: bash

    chmod +x build.sh
    ./build.sh

Running this script creates an output folder that contains everything the
process will generate:

    - Fetched resources
    - Toolchain
    - Images

This helps maintain a clean Buildroot directory. When the script is executed,
an (almost) empty folder named ``output`` is created, as the build process has
not yet begun.

Let's modify the configuration with ``menuconfig``.

.. code-block:: bash

    cd output
    make menuconfig

Change these configurations:

    - System host name to ``lvgl-buildroot`` under ``system_configuration > System hostname``.
    - Root password to ``lvgl-buildroot``  under ``system_configuration > Root password``.
    - Enable SSH in ``target-packages > Networking applications > openssh``

.. _build-environment:

Build the image

.. code-block:: bash

    make

What happened?
**************

Let's explore the contents of the output folder and what it contains.

build
=====

This folder contains the intermediate files and build artifacts for the various
packages that are being compiled. Each package has its own subdirectory within
the ``build`` folder, where the build process takes place. It includes files
such as configuration files, source code, and object files that are generated
as part of the build process.

host
====

The ``host`` folder contains files and binaries that are built for the host
system rather than the target system. This includes tools and utilities that
are needed to build packages or to run the build system itself. It may contain
compilers, build tools, and libraries that are required to support the build
process for the target.

.. _rpi4_images:

images
======

This directory holds the final output images generated for the target system,
such as filesystem images, kernel images, or bootloader images. Depending on
the configuration, you may find files like ``rootfs.tar``, ``zImage``,
``uImage``, or others that are ready to be deployed onto the target hardware.

target
======

The ``target`` folder contains the files that are specifically intended for the
target system. This includes the root filesystem and any additional files that
will be included in the target environment. The structure within this folder
often mimics the directory structure of a standard Linux system, containing
directories like ``bin``, ``lib``, ``etc``, ``usr``, and others, which hold the
binaries, libraries, configuration files, and other necessary components for
the target system to function properly.

.. _flash_the_image:

Flash the image
***************

Insert the SD card into the laptop and check its mount point. It is typically
labeled as sda or sdb, but you can use the lsblk command to confirm this
information.

If it is mounted on /dev/sda, run the following command

.. code-block:: bash

    sudo dd if=images/sdcard.img of=/dev/sda

As mentioned in :ref:`rpi4_images`, the output image is in ``images`` and named
``sdcard.img``.

Connect an Ethernet cable to the RPi4 and ensure the laptop and the RPi4 are on
the same network.

You can use ``ifconfig`` or ``ip a`` to find your IP address. Then, use
``nmap`` or any tool to scan the network and find the IP address of the
RPi4.

.. code-block:: bash

    nmap -sn <YOUR_IP_ADDRESS>/24 | grep lvgl

All of this can be done with the UART instead of SSH if you don't want to
connect the RPi4 to the network.

