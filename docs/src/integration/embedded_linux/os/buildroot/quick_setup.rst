Quick Setup
###########

A Git repository is available that includes everything needed to test the
Buildroot setup without following the guide. It is intended for testing
purposes and can be used if you are already familiar with Buildroot concepts
and architecture.

Get the repository

.. code-block:: bash

    git clone --recurse-submodules https://github.com/lvgl/lv_buildroot.git

Build the image

.. code-block:: bash

    ./build.sh

At this point, you have an image that can be flashed on an SD and used.

Now build the SDK and install it

.. code-block:: bash

    cd output
    make sdk
    mkdir -p ~/sdk
    tar -xzf images/aarch64-buildroot-linux-gnu_sdk-buildroot.tar.gz -C ~/sdk

The SDK is installed. Compile the application.

.. code-block:: bash

    cd ../application/lv_benchmark
    source ../setup-build-env.sh
    cmake -B build -S .
    make -j $(nproc) -C build
    cd ../..

Install the binary in the rootfs_overlay

.. code-block:: bash

    cp application/lv_benchmark/bin/lvgl-app resources/board/rootfs_overlay/usr/bin/

Regenerate the image with the rootfs overlay updated.

.. code-block:: bash

    cd output
    make

Go to :ref:`flash_the_image` to test the system.
