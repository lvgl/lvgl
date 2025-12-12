LVGL application
################

This section provides information about the steps to follow to get a custom
application using LVGL running on the board.

Update RootFS
*************

Depending on the application, it might be necessary to update the rootfs. Let's
take as example the compilation of LVGL with DRM. The system must have
``libdrm`` installed.

.. code-block:: bash

    cd output
    make menuconfig

To search for a string pattern in the configuration, press ``/`` followed by
the desired pattern. For example, search for ``libdrm``. You should find
``BR2_PACKAGE_LIBDRM`` set to **[=n]**. The Location field indicates where to
find this option. By pressing the corresponding number key (9-0), you can
navigate directly to the option.

In the ``Search Results`` window, the ``Depends on`` section lists the required
packages or options that need to be enabled (or disabled) to make the target
package configurable. For any package you wish to add, these dependencies must
be met; otherwise, the option will remain hidden.

When the package configuration is completed, build the environment to add the
packages (``make`` in **output** folder).

To verify the library was installed, we can find it in the target sysroot:

.. code-block:: bash

    find build/ -name "*libdrm*"

You should see the include folder and the .so files.

.. _generate_sdk:

Generate SDK and set up environment
***********************************

Generate an SDK that you can use to cross-compile the application for the
target (RPi4).

.. code-block:: bash

    make sdk

A ``.tar.gz`` is generated in output/images. This is the SDK!

To use it, extract it anywhere.

.. code-block:: bash

    mkdir -p ~/sdk
    tar -xzf images/aarch64-buildroot-linux-gnu_sdk-buildroot.tar.gz -C ~/sdk
    cd ..

To set up the environment

.. code-block:: bash

    mkdir application && cd application
    touch setup-build-env.sh && chmod +x setup-build-env.sh

The script ``setup-build-env.sh`` looks like this:

.. code-block:: bash

    #!/bin/bash

    SDK_PATH="$HOME/sdk/aarch64-buildroot-linux-gnu_sdk-buildroot"

    export PATH="${SDK_PATH}/bin:${PATH}"

    export SYSROOT="${SDK_PATH}/aarch64-buildroot-linux-gnu/sysroot"

    export CROSS_COMPILE="aarch64-buildroot-linux-gnu-"

    export CC="${CROSS_COMPILE}gcc"
    export CXX="${CROSS_COMPILE}g++"
    export LD="${CROSS_COMPILE}ld"
    export AR="${CROSS_COMPILE}ar"
    export AS="${CROSS_COMPILE}as"

    export CFLAGS="--sysroot=${SYSROOT}"
    export LDFLAGS="--sysroot=${SYSROOT}"


Build the application
*********************

The environment is now set up, and we're ready to build an application using
the ``lv_benchmark`` repository that is inspired from `lv_port_linux
<https://github.com/lvgl/lv_port_linux>`_.

Navigate back to the root of the project and clone the repository:

.. code-block:: bash

    git clone --recurse-submodules https://github.com/EDGEMTech/lv_benchmark.git

The application is configured to run on fbdev. You can either maintain the
default configuration or modify it according to your preferences.

Compile the application

.. code-block:: bash

    cd lv_benchmark
    cmake -B build -S . -DCMAKE_C_COMPILER=${CROSS_COMPILE}gcc -DCMAKE_CXX_COMPILER=${CROSS_COMPILE}g++ -DCMAKE_SYSROOT=${SYSROOT} -DCMAKE_C_FLAGS="--sysroot=${SYSROOT}" -DCMAKE_CXX_FLAGS="--sysroot=${SYSROOT}"
    make -j $(nproc) -C build

Verify that the output executable was compiled with the correct toolchain:

.. code-block:: bash

    file bin/lvgl-app
    cd ../..

The output should contain these information:

    - ARM aarch64
    - interpreter /lib/ld-linux-aarch64.so.1

Set a rootfs overlay
********************

In Buildroot, a rootfs overlay (or root filesystem overlay) is a mechanism that
allows you to add custom files, directories, and configurations directly into
the root filesystem of the target image during the build process. It is a way
to extend or modify the content of the root filesystem without altering the
base packages or recompiling everything.

Create the rootfs overlay structure and include the executable of the built
application.

.. code-block:: bash

    mkdir -p resources/board/rootfs_overlay/usr/bin
    cp application/lv_benchmark/bin/lvgl-app resources/board/rootfs_overlay/usr/bin/

Update the configuration.

.. code-block:: bash

    cd output
    make menuconfig

Find the rootfs overlay option ``System configuration > Root filesystem overlay
directories`` and use the relative path to the rootfs overlay with your
application (**../resources/board/rootfs_overlay**) and save the
configuration.

Now use ``make`` command to apply the rootfs overlay and confirm the
sysroot was updated as expected.

.. code-block:: bash

    find . -name lvgl-app

:ref:`flash_the_image` and run the benchmark application on the board.
