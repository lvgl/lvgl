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

Creating the Docker image
-------------------------

Toradex provides a `VSCode extension <https://developer.toradex.com/torizon/application-development/ide-extension/>`_ that offers a collection of templates used to configure
and automate the tasks needed to cross-compile applications and build Docker images.


However, this guide explains how to perform those operations manually.

.. note::

   The template for a LVGL application is currently being added to the VSCode extension and will be available soon.

To build a Torizon container Docker ARM emulation needs to be enabled
by typing the following commands:

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

.. code-block::

   ARG CROSS_SDK_BASE_TAG=3.2.1-bookworm
   ARG BASE_VERSION=3.2.1-bookworm
   ##
   # Board architecture
   # arm or arm64
   ##
   ARG IMAGE_ARCH=arm64
   
   ##
   # Directory of the application inside container
   ##
   ARG APP_ROOT=/usr/lvgl_application
   
   # BUILD ------------------------------------------------------------------------
   FROM torizon/debian-cross-toolchain-${IMAGE_ARCH}:${CROSS_SDK_BASE_TAG} AS build
   
   ARG APP_ROOT
   ARG IMAGE_ARCH
   
   RUN apt-get -q -y update && \
       apt-get -q -y install && \
       apt-get clean && apt-get autoremove && \
       apt-get install -q -y curl git cmake file && \
       rm -rf /var/lib/apt/lists/*
   
   COPY . ${APP_ROOT}
   WORKDIR ${APP_ROOT}
   
   # Compile lv_port_linux
   RUN CC=aarch64-linux-gnu-gcc cmake -S ./lv_port_linux -B build
   RUN make -j 4 -C ${APP_ROOT}/build
   
   # DEPLOY -----------------------------------------------------------------------
   FROM --platform=linux/${IMAGE_ARCH} torizon/debian:${BASE_VERSION} AS deploy
   
   ARG IMAGE_ARCH
   ARG APP_ROOT
   
   RUN apt-get -y update && apt-get install -y --no-install-recommends \
   && apt-get clean && apt-get autoremove && rm -rf /var/lib/apt/lists/*
   
   # Copy the lvglsim executable compiled in the build step to the $APP_ROOT directory
   # path inside the container
   COPY --from=build ${APP_ROOT}/lv_port_linux/bin/lvglsim ${APP_ROOT}
   
   # Command executed during runtime when the container starts
   ENTRYPOINT [ "./lvglsim" ]


The ``Dockerfile`` acts like a recipe to build two images and  ``build`` and ``deploy``.

First it downloads the necessary packages to build the simulator using Debian's package manager ``apt-get``.

After compilation, the resulting executable is written to ``lv_port_linux/bin/lvglsim``.

The ``deploy`` image will be deployed on the device.
The executable created in the previous image is copied to the ``/usr/bin`` directory of the current image.

This creates a smaller image that does not include the tool chain and the build dependencies.

The images are built with the following command:

``docker build . -t lvgl_app``

Docker will interpret the ``Dockerfile`` present in the current working directory.
The ``-t`` argument gives a name to the resulting image.

Upon completion, ensure that the image is listed by Docker:

``docker image list | grep lvgl_app``

It should display the image along with its ID that will be used later.

Deploying the container image to the device
-------------------------------------------

The image is now ready to be deployed on the device. There are several ways to perform
this task.

Read `this article <https://developer.toradex.com/torizon/application-development/working-with-containers/deploying-container-images-to-torizoncore/>`_ for more information.

For this guide, we are going to setup a Docker registry container on the development host 
which will be accessible from any device on your LAN. The Toradex board being on the same network 
will be able to pull the image from the registry.

The registry is created like so:

``docker run -d -p 5000:5000 --name registry registry:2.7``

The ``-d`` flag runs the container in detached mode. The ``-p`` argument specifies the port mapping.
The registry container will listen on port ``TCP/5000`` and will map to the same port externally.

Push the image created in the previous step to the newly created registry:

``docker tag <IMAGE_ID> 127.0.0.1:5000/lvgl-app``

``docker push 127.0.0.1:5000/lvgl-app``

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

Start the container like so:

``docker run --device /dev/fb0:/dev/fb0 <IMAGE_ID>``

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
