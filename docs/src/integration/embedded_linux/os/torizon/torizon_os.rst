.. _torizon-os-section:

Torizon OS
==========

Torizon OS is an open-source software platform that aims to simplify the
development and maintenance of embedded Linux software.

By using docker containers it allows developers to focus on the application
instead of building and maintaining a custom distribution.

This guide explains how to create a docker image containing LVGL and a simple demo
application that can be deployed on any Toradex device running TorizonOS.

Prerequisites
-------------

To follow this guide you obviously need to have a Toradex SoM along with a carrier board.
More information is available on the Toradex `website <https://www.toradex.com/computer-on-modules>`_.


.. note::

   You need to be familiar with some Docker fundamentals to be able to understand and follow
   this guide. Also Docker needs to be present on the development host.

The `Toradex documentation <https://developer.toradex.com>`_ is a helpful resource. This article contains many references
to it.

Board setup
-----------

Begin by installing TorizonOS by using the Toradex Easy Installer.
Follow the official bring-up `guides <https://developer.toradex.com/quickstart/bringup/>`_.

In the Easy Installer, select TorizonCore as the operating system to install.

Copy the IP address that was assigned to the device by the DHCP server on your network.
The address is displayed in the bottom right corner. It will be used later.

Once the setup is complete, the device will boot into TorizonOS.

VS Code extension
-----------------

Toradex provides a `VS Code extension <https://developer.toradex.com/torizon/application-development/ide-extension/>`_ that offers a collection of templates used 
to configure and automate the tasks needed to cross-compile applications and build Docker images.

These templates now include support for LVGL applications, available as one of the `partner templates <https://github.com/torizon/vscode-torizon-templates?tab=readme-ov-file#partner-templates>`_.

This guide explains how to perform those operations manually.

Creating the Docker image
-------------------------

To build a Torizon container for ARM on your development machine, you need to enable Docker emulation.
Run the following commands to enable it:

.. code-block:: sh

   docker run --rm -it --privileged torizon/binfmt
   docker run --rm -it --pull always --platform linux/arm64/v8 debian arch

The second command should print ``aarch64``.

More information on the build environment for Torizon containers is available `here
<https://developer.toradex.com/torizon/application-development/working-with-containers/configure-build-environment-for-torizon-containers/>`_.

Let's begin by creating a directory that will contain the ``Dockerfile`` and source code
that will be used to build the image.

Type the following commands in a terminal window:

.. code-block:: sh

   mkdir -p ~/lvgl_torizon_os/
   cd ~/lvgl_torizon_os/
   touch Dockerfile

   git clone --depth 1 https://github.com/lvgl/lv_port_linux
   git -C lv_port_linux submodule update --init

These commands create the project directory and the ``Dockerfile``.
``git`` is used to download the ``lv_port_linux`` and ``lvgl`` repositories from Github.

.. note::

   By default, ``lv_port_linux`` is configured to use the legacy framebuffer device
   ``/dev/fb0``. It is also possible to use another rendering backend by enabling the
   correct options in ``lv_port_linux/lv_conf.h``.


Now edit the Dockerfile. Copy-paste the block below into the file:

.. code-block:: docker


    ARG CROSS_SDK_BASE_TAG=3.2.1-bookworm
    ARG BASE_VERSION=3.2.1-bookworm
    ##
    # Board architecture
    # arm or arm64
    ##
    ARG IMAGE_ARCH=arm64
    
    
    # BUILD ------------------------------------------------------------------------
    FROM torizon/debian-cross-toolchain-${IMAGE_ARCH}:${CROSS_SDK_BASE_TAG} AS build
    
    ARG IMAGE_ARCH
    
    RUN apt-get -q -y update && \
        apt-get clean && apt-get autoremove && \
        apt-get install -q -y curl git cmake file python3 python3-venv pkg-config libevdev-dev:arm64 && \
        rm -rf /var/lib/apt/lists/*
    
    WORKDIR /app
    COPY . .
    
    # Compile lv_port_linux
    RUN CC=aarch64-linux-gnu-gcc cmake -S ./lv_port_linux -B build
    RUN cmake --build build -j$(nproc)
    
    # DEPLOY -----------------------------------------------------------------------
    FROM --platform=linux/${IMAGE_ARCH} torizon/debian:${BASE_VERSION} AS deploy
    
    ARG IMAGE_ARCH
    
    RUN apt-get -y update && apt-get clean && apt-get autoremove && rm -rf /var/lib/apt/lists/*
    
    # Copy the lvglsim executable compiled in the build step to the /usr/lvgl_widgets directory
    # path inside the container
    COPY --from=build /app/build/bin/lvglsim /usr/lvgl_widgets
    
    # Command executed during runtime when the container starts
    ENTRYPOINT [ "/usr/lvgl_widgets" ]

The ``Dockerfile`` acts like a recipe to build two images:  ``build`` and ``deploy``.

First it downloads the necessary packages to build the simulator using Debian's package manager ``apt-get``.

After compilation, the resulting executable is written to ``lv_port_linux/bin/lvglsim``.

The ``deploy`` image will be deployed on the device.
The executable created in the previous image is copied to the ``/usr/bin`` directory of the current image.

This creates a smaller image that does not include the tool chain and the build dependencies.

The images are built with the following command:

.. code-block:: sh

    docker build . -t lvgl_app

Docker will interpret the ``Dockerfile`` present in the current working directory.
The ``-t`` argument gives a name to the resulting image.

Upon completion, ensure that the image is listed by Docker:

.. code-block:: sh

    docker image list | grep lvgl_app
    lvgl_app                  latest    2967a34a9e74   2 minutes ago    118MB


Alongside the image name, you'll also find its ID (``2967a34a9e74`` in this example). This will be useful for later.

Deploying the container image to the device
-------------------------------------------

The image is now ready to be deployed on the device. There are several ways to perform
this task.

Read `this article <https://developer.toradex.com/torizon/application-development/working-with-containers/deploying-container-images-to-torizoncore/>`_ for more information.

For this guide, we are going to setup a Docker registry container on the development host
which will be accessible from any device on your LAN. The Toradex board being on the same network
will be able to pull the image from the registry.

The registry can be installed and started with a single command:

.. code-block:: sh

    docker run -d -p 5000:5000 --name registry registry:2.7


The ``-d`` flag runs the container in detached mode. The ``-p`` argument specifies the port mapping.
The registry container will listen on port ``TCP/5000`` and will map to the same port externally.

Push the image created in the previous step to the newly created registry:

.. code-block:: sh

    docker tag lvgl_app 127.0.0.1:5000/lvgl-app
    docker push 127.0.0.1:5000/lvgl-app


By default a local container registry uses clear text HTTP so the Docker instance
running on the device has to be configured to allow fetching images from an 'insecure' repository.

Get the IP address of the development host and open a remote shell on the device.

.. code-block:: sh

   sudo su # When prompted type in the password of the torizon user

   # Be sure to replace set the IP address of your host instead
   cat << heredoc > /etc/docker/daemon.json
   {
        "insecure-registries" : ["<IP_ADDR_OF_DEVELOPMENT_HOST>:5000"]
   }
   heredoc

   systemctl restart docker.service

   # Return to regular user
   exit

   # Pull the image from your development host
   docker pull <IP_ADDR_OF_DEVELOPMENT_HOST>:5000/lvgl-app


By default, Docker containers are isolated from the hardware
of the host system. While this is great for security, an embedded application will obviously need
`access to the hardware. <https://developer.toradex.com/torizon/application-development/use-cases/peripheral-access/best-practices-with-hardware-access/>`_

The container running the LVGL application needs access to the framebuffer device ``/dev/fb0``.

By using the ``--device`` argument it is possible to map a device to a container.

Start the container like so, using the image ID:

.. code-block:: sh

    docker run --device /dev/fb0:/dev/fb0 <IMAGE_ID>

Conclusion
----------

You now have a running LVGL application. Where to go from here?

* You are now ready to build your LVGL application for Torizon OS.
  It is recommended to get familiar with VSCode IDE extension
  as it will simplify your workflow.

  If you are a VSCode user, it is the best way to develop for Torizon OS. If you use
  another editor or IDE you can always
  write scripts to automate the building/pushing/pulling operations.

* Read this `article <https://developer.toradex.com/torizon/application-development/application-development-overview>`_
  to understand how to design applications for Torizon OS.

* Getting familiar with Torizon Cloud and Torizon OS builder is
  also recommended when you get closer to production.
