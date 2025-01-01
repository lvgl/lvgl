.. _yocto_lvgl_recipe:

=============
LVGL in Yocto
=============

This chapter serves as a guide to help you create a recipe for an application
using LVGL. While the process will be demonstrated for the Raspberry Pi 3
(64-bit), it can be applied to any board in a similar way.

Build Host Packages
*******************
You must install essential host packages on your build host. The following
command installs the host packages based on an Ubuntu distribution

.. code-block::

    sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential \
    chrpath socat cpio python3 python3-pip python3-pexpect xz-utils \
    debianutils iputils-ping python3-git python3-jinja2 python3-subunit zstd \
    liblz4-tool file locales libacl1

.. note::

    For host package requirements on all supported Linux distributions, see the
    `Required Packages for the Build Host <https://docs.yoctoproject.org/
    ref-manual/system-requirements.html#required-packages-for-the-build-host>`_
    section in the Yocto Project Reference Manual.



Use Git to clone the required repositories
******************************************
After this section the folder tree will be like this

.. code-block:: none

    lvgl_yocto_guide/
    ├── build/
    │   ├── cache/
    │   ├── conf/
    │   ├── downloads/
    │   └── ...
    └── sources/
        ├── meta-openembedded
        ├── meta-raspberrypi
        └── poky

Start creating the folder architecture

.. code-block::

    mkdir -p lvgl_yocto_guide/sources

Clone Poky, meta-openembedded and meta-raspberrypi in the sources

.. code-block::

    cd lvgl_yocto_guide/sources
    git clone --branch scarthgap https://git.yoctoproject.org/poky.git
    git clone --branch scarthgap https://git.openembedded.org/meta-openembedded
    git clone --branch scarthgap git://git.yoctoproject.org/meta-raspberrypi


Build a base image
******************
To understand better what is going on, let's build the image like it is
prepared for us

.. code-block:: none

    cd ../ # go back to the root folder
    source sources/poky/oe-init-build-env

Now you should have a folder named ``build`` next to ``sources``. The last
command
also sets the current directory to the build directory.

In the build directory, there is a ``conf`` folder with some files in it

.. code-block:: none

    └── build/
        └── conf/
            ├── bblayers.conf
            ├── conf-notes.txt
            ├── conf-summary.txt
            ├── local.conf
            └── templateconf.cfg

The important files for us are ``local.conf`` and ``bblayers.conf``

To add layers to the project there are 2 options:

**Bitbake commands (Option 1)**

You need to be in the same terminal you did the ``source`` command. Add the
layers with the Bitbake command like this

.. code-block::

    bitbake-layers add-layer ../sources/meta-openembedded
    bitbake-layers add-layer ../sources/meta-raspberrypi

**Modify conf file (Option 2)**

Open ``conf/bblayers.conf`` file and add manually the paths:

.. code-block:: bash

    # POKY_BBLAYERS_CONF_VERSION is increased each time build/conf/bblayers.conf
    # changes incompatibly
    POKY_BBLAYERS_CONF_VERSION = "2"

    BBPATH = "${TOPDIR}"
    BBFILES ?= ""

    BBLAYERS ?= " \
    ${TOPDIR}/../sources/poky/meta \
    ${TOPDIR}/../sources/poky/meta-poky \
    ${TOPDIR}/../sources/poky/meta-yocto-bsp \
    ${TOPDIR}/../sources/meta-raspberrypi \
    ${TOPDIR}/../sources/meta-openembedded/meta-oe \
    ${TOPDIR}/../sources/meta-openembedded/meta-multimedia \
    ${TOPDIR}/../sources/meta-openembedded/meta-networking \
    ${TOPDIR}/../sources/meta-openembedded/meta-python \
    "

To ensure the layers were added as expected, run the Bitbake command to show
all the layers::

    bitbake-layers show-layers

The following layers should be listed:
    -  core
    -  yocto
    -  yoctobsp
    -  raspberrypi
    -  openembedded-layer
    -  multimedia-layer
    -  networking-layer
    -  meta-python

Build for RaspberryPi3 64
=========================

The available machine configurations for Raspberrypi can be listed like this

.. code-block::

    ls ../sources/meta-raspberrypi/conf/machine/*.conf

To build an image for Raspberrypi3 64 bits, modify the file ``local.conf`` file
replacing the ``MACHINE ??=`` default value like this

.. code-block:: bash

    MACHINE ??= "raspberrypi3-64"

To build the image we will target, it is also needed to add this to the file:

.. code-block:: bash

    LICENSE_FLAGS_ACCEPTED = "synaptics-killswitch"

.. note::

    You can find more information about this variable `here <https://meta-raspberrypi.readthedocs.io/en/latest/ipcompliance.html#linux-firmware-rpidistro>`_.

Everything is setup, time to build the image::

    bitbake core-image-base


Overview of the build
=====================

Let's go through the build folders to understand what happened.

Downloads
---------

The folder location can be changed with the ``DL_DIR`` variable.

The Downloads folder is a fundamental part of the Yocto build environment,
serving as a local cache for all external sources and dependencies. This
caching mechanism is essential for efficient builds, ensuring that repeated
builds do not waste time downloading the same files again.


tmp/deploy
----------

This folder contains the final output artifacts that are ready for deployment.
These artifacts include kernel images, root filesystems, packages, SDKs,
bootloader files, and other components that are generated during the Yocto
build process.

There are some key folders:
    -  **images**: it contains the images that can be flashed or deployed to
       the target device. Files like the Linux kernel, root filesystem (e.g., .
       ext4, .tar.gz, .squashfs), bootloaders (e.g., U-Boot), and other
       bootable images for the device are found here. t’s organized by the
       machine (or target board) for which the image was built.
    -  **rmp/deb/ipk**: These folders contain the individual software packages
       generated during the build, in the specified package format (RPM, DEB,
       or IPK). These packages are typically created when you’re building your
       Yocto project with package management support enabled. These can later
       be installed on the target device using package management tools
    -  **sdk**: This subdirectory contains Software Development Kits (SDKs)
       that you can use to cross-compile applications for your target system.
       The SDKs are generated when you use the ``bitbake -c populate_sdk``
       command.


tmp/sysroots-components
-----------------------
This folder is a modular approach introduced in Yocto to handle sysroots. It
divides the sysroot into individual components, which can be thought of as the
building blocks or packages that make up the entire sysroot environment.

Each component corresponds to a specific package or dependency that is staged
into the sysroot. This approach allows for more efficient reuse of sysroots
across multiple packages or builds, and it helps with dependency tracking and
management during the build process.

The sysroot-components directory helps optimize the build process because if
one package changes, Yocto only needs to update the corresponding component
rather than rebuilding or copying the entire sysroot.

If you followed the previous steps, here are the folders you will find:
    - ``all``: Architecture-independent files.
    - ``cortexa53``: Files for the Cortex-A53 (ARMv8-A) architecture.
    - ``manifests``: Track files installed in the sysroot by package.
    - ``raspberrypi3_64``: Files specific to the Raspberry Pi 3 (64-bit).
    - ``x86_64``: Files for the x86_64 (PC) architecture, typically for
      cross-compilation tools.
    - ``x86_64-nativesdk``: Files related to the SDK for cross-compilation on
      an x86_64 host.

Each folder corresponds to components relevant to the specific architecture,
and they collectively form the complete environment needed to compile and run
software for the target and host systems.

tmp/sysroots-uninative
----------------------

The sysroots-uninative directory in Yocto is used to support the "uninative"
feature, which allows for more reproducible builds by ensuring that the build
environment remains consistent across different host systems. It essentially
provides a way to use the same native build tools across different Linux
distributions.

tmp/work
--------

The ``work`` folder in Yocto is a key directory in the ``tmp`` folder that
holds all the temporary build artifacts for each package during the build
process. It is where the actual building and compiling of individual packages
or recipes takes place. Each package (or "recipe") that BitBake processes
generates temporary files and directories inside this ``work`` folder.

The ``work`` folder is typically structured by machine architecture and
package. Here's how it generally works:

.. code-block:: none

    work/
    └── <architecture>/
        └── <package>/
            └── <version>/
                ├── temp/
                ├── work/
                ├── sysroot-destdir/
                └── image/

This folder is very important and helps a lot during debug phases. The ``temp``
subfolder contains important logs that can help you diagnose build issues. This
is where you'll look when a build fails to figure out what went wrong.


LVGL recipe
***********

Find more information about recipes in :ref:`recipes_section` section.

There is a recipe in ``meta-openembedded`` since ``honister``.

.. list-table:: lvgl recipe version
   :widths: 200 100
   :header-rows: 1

   * - Branch
     - Recipe
   * - scarthgap (Yocto Project 5.0)
     - lvgl 9.1.0
   * - nanbield (Yocto Project 4.3)
     - lvgl 8.3.10
   * - mickledore (Yocto Project 4.2)
     - lvgl 8.1.0
   * - langdale (Yocto Project 4.1)
     - lvgl 8.1.0
   * - langdale (Yocto Project 4.1)
     - lvgl 8.1.0
   * - kirkstone (Yocto Project 4.0)
     - lvgl 8.0.3

In this guide, we are on the ``scarthgap`` branch, so we are using lvgl 9.1.0.

Let's dive into this recipe to understand what is done. The objective is to add
this library as a shared object in the target rootfs, and also to generate a
SDK with lvgl.

This is the path of lvgl recipes: ``lvgl_yocto_guide/sources/meta-openembedded/
meta-oe/recipes-graphics/lvgl``

Here is the architecture of lvgl recipes folder:

.. code-block:: bash

    lvgl
    ├── files
    │   ├── 0002-fix-sdl-handle-both-LV_IMAGE_SRC_FILE-and-LV_IMAGE_S.patch
    │   ├── 0003-Make-fbdev-device-node-runtime-configurable-via-envi.patch
    │   ├── 0004-Factor-out-fbdev-initialization-code.patch
    │   ├── 0005-Add-DRM-KMS-example-support.patch
    │   ├── 0006-Add-SDL2-example-support.patch
    │   ├── 0007-fix-cmake-generate-versioned-shared-libraries.patch
    │   └── 0008-fix-fbdev-set-resolution-prior-to-buffer.patch
    ├── lv-conf.inc
    ├── lvgl_9.1.0.bb
    └── lvgl-demo-fb_9.1.0.bb

-  ``file`` folder contains all the patches that can be applied when
   building the recipe.
-  ``lv_conf.inc`` is an include file, usually containing common configuration
   settings for LVGL that can be shared between multiple recipes.
-  ``lvgl_9.1.0.bb`` is the recipe to build lvgl library.
-  ``lvgl-demo-fb_9.1.0.bb`` is a recipe to build an application using lvgl.

For now let's understand the recipe of lvgl library.

.. code-block:: bash

    # SPDX-FileCopyrightText: Huawei Inc.
    #
    # SPDX-License-Identifier: MIT

    HOMEPAGE = "https://lvgl.io/"
    DESCRIPTION = "LVGL is an OSS graphics library to create embedded GUI"
    SUMMARY = "Light and Versatile Graphics Library"
    LICENSE = "MIT"
    LIC_FILES_CHKSUM = "file://LICENCE.txt;md5=bf1198c89ae87f043108cea62460b03a"

    SRC_URI = "\
        git://github.com/lvgl/lvgl;protocol=https;branch=master \
        file://0002-fix-sdl-handle-both-LV_IMAGE_SRC_FILE-and-LV_IMAGE_S.patch \
        file://0007-fix-cmake-generate-versioned-shared-libraries.patch \
        file://0008-fix-fbdev-set-resolution-prior-to-buffer.patch \
        "
    SRCREV = "e1c0b21b2723d391b885de4b2ee5cc997eccca91"

    inherit cmake

    EXTRA_OECMAKE = "-DLIB_INSTALL_DIR=${baselib} -DBUILD_SHARED_LIBS=ON"
    S = "${WORKDIR}/git"

    require lv-conf.inc

    do_install:append() {
        install -d "${D}${includedir}/${PN}"
        install -m 0644 "${S}/lv_conf.h" "${D}${includedir}/${PN}/lv_conf.h"
    }

    FILES:${PN}-dev += "\
        ${includedir}/${PN}/ \
        "

**LICENSE**: Indicates the licensing of the software, stating that it is
distributed under the MIT License.

**LIC_FILES_CHKSUM**: This variable contains a checksum (in this case, an MD5 hash)
for the license file (here assumed to be LICENCE.txt). This helps to ensure the
integrity of the license file included in the package. In LVGL repository,
there is a LICENSE.txt. To get the value of the hash of the file, you can do
this command: ``md5sum LICENSE.txt``

SRC_URI: Specifies the locations of the source code and patches for the recipe:
    - The main source repository for LVGL specifying the master branch.
    - The following arguments are the local patch files that will be applied to
      the source code during the build process.

**SRCREV**: Defines the specific commit (in this case, a Git SHA hash) from
which the source code will be fetched. This ensures that the build uses a
consistent version of the code.

**inherit**: This line indicates that the recipe uses the ``cmake`` class,
which provides functionality for building projects that use CMake as
their build system.

**EXTRA_OECMAKE**: Additional options passed to CMake during the
configuration step.

**S**: This variable defines the source directory where the unpacked source
code will be located after fetching. ``${WORKDIR}`` is a standard
variable in BitBake that points to the working directory for the recipe.
require: This line includes another configuration file, ``lv-conf.inc``,
which likely contains additional configuration options or variables
specific to the LVGL library.

**FILES**: This is a BitBake variable used to specify the files that should
be included in a particular package. In this case, the variable is
specifying files for a package related to development (i.e., header
files).

Recipe Tasks
************

When a recipe is compiled, it will run multiple tasks. You can run each task
manually to understand what is generated each step, or you can run ``bitbake
lvgl`` to run all the tasks.

Fetch (do_fetch)
   .. code-block:: bash

      bitbake lvgl -c fetch

   Fetch task fetches the package source from the local or remote repository.

   The fetch Repo address has to be stored in **SRC_URI** variable. In
   **SRCREV** Variable the commit hash of github repo is defined.

   When the fetch task has been completed, you can find the fetched sources in
   ``build/downloads``.

   For this recipe, you will find a new folder here: ``lvgl_yocto_guide
   build/downloads/git2/github.com.lvgl.lvgl``.

   You can also find the folder architecture created in ``lvgl_yocto_guide/
   build/tmp/work/cortexa53-poky-linux/lvgl`` but these folders are empty since
   only the fetch was done.




Unpack (do_upack)
   .. code-block:: bash

      bitbake lvgl -c unpack

   Unpack task unpacks the package that has been downloaded with Fetch task.

   In the ``lvgl_yocto_guide/build/tmp/work/cortexa53-poky-linux/lvgl/9.1.0``
   folder, you can now find the source code in ``git`` (as it was defined in
   the recipe). You will also see the patches that will be applied on the next
   step. So for now, the sources are unmodified and the same than the commit
   that was specified.



Patch (do_patch)
   .. code-block:: bash

      bitbake lvgl -c patch

   Patch task locates the patch files and applies the patches to the sources
   if any patch is available. This is optional task, executes if patch is
   available.

   Patch file is also defined in **SRC_URI** variable. By default it runs in
   current source directory **${S}**.

Configure (do_configure)
   .. code-block:: bash

      bitbake lvgl -c configure

   The Configuration task configures the source by enabling and disabling any
   build-time and configuration options for the software being built before
   compilation if any configuration is available.

   This is a optional steps, executes if configuration is available.

   In this case, it creates a build directory, It invokes CMake to configure
   the project, specifying build options and paths based on variables in your
   recipe. It generates Makefiles or project files needed for the build. Also,
   there are operations added in the task in ``lv-conf.inc``.

   So at the end of the task, in the ``lvgl_yocto_guide/build/tmp/work/
   cortexa53-poky-linux/lvgl/9.1.0``, you will find a ``build`` folder that was
   generated running the CMake command, but nothing is built yet. Also, the
   sysroots have everything required to build lvgl library.


Compile (do_compile)
   .. code-block:: bash

      bitbake lvgl -c compile

   The Compilation task compiles the source code if any compilation steps are
   available and generates a binary file.

   This is a optional steps, executes if compilation is available.

   If there are any compilation steps, then these steps are define in
   do_compile() function of bitbake.

   Like in the previous task, this is handle by ``inherit cmake``.

   In the build folder, you can now see the built library. The ``.so`` files
   are available in ``lvgl_yocto_guide/build/tmp/work/ cortexa53-poky-linux/lvgl/9.1.0/build/lib``.

   After this task has been completed, everything is ready to be installed.

Install (do_install)
   .. code-block:: bash

      bitbake lvgl -c install

   The Install task copies files that are to be packaged into the holding area
   **${D}**. This task runs with the current working directory **${S}** which
   is the compilation directory.

   It creates the necessary directory for the header files in the destination
   installation directory.
   It installs the ``lv_conf.h`` header file from the source directory into the
   appropriate include directory in the destination path, ensuring it has the
   correct permissions.

   The lvgl library (``.so`` files) are also ready to be installed in the final
   image. A new folder ``image`` was created.

.. important::

    In the file ``build/conf/local.conf``, add these 2 lines at the end of the
    file:

    .. code-block:: bash

        IMAGE_INSTALL:append = " lvgl"
        TOOLCHAIN_HOST_TASK:append = " lvgl"

    This will add the lvgl library in the generated image, and it will also add
    the library to the host SDK we will generate later on.

    With these modifications, you can now run the image recipe again::

        bitbake core-image-base

    This will execute all the previous described tasks.

    If everything went well, you should now found this file ``build/tmp/deploy/
    rpm/cortexa53/lvgl-9.1.0-r0.cortexa53.rpm`` and other rpm files related to
    lvgl.

SDK generation
**************

Generating a Software Development Kit (SDK) in Yocto serves several important
purposes, particularly in embedded development:

    - **Development Environment**: The SDK provides developers with a
      ready-to-use development environment tailored for a specific target
      hardware platform.
      This includes the necessary tools, libraries, and headers to build
      applications that run on the target device.
    - **Cross-Compilation**: The SDK allows developers to cross-compile
      applications from a host machine (typically x86 or x86_64 architecture)
      for a different architecture (e.g., ARM). This is essential for embedded
      systems, where development often occurs on more powerful machines.
    - **Simplified Development**: By packaging all required components, the SDK
      simplifies the process of setting up a development environment.
      Developers don't need to manually install and configure tools and
      libraries; everything needed is included in the SDK.
    - **Consistent Build Environment**: The SDK ensures that developers are
      working with the same versions of and tools used in the Yocto
      build, which helps to avoid compatibility issues and ensures that
      applications will behave as expected on the target device.

To generate an SDK of the environment, run the following command:

.. code-block:: bash

    bitbake core-image-base -c populate_sdk

This will create a script ``build/tmp/deploy/sdk/
poky-glibc-x86_64-core-image-base-cortexa53-raspberrypi3-64-toolchain-5.0.4.
sh``. This script allows you to install the SDK where you prefer. Here is the
execution output of the script

.. code-block:: bash

    $ ./sdk/poky-glibc-x86_64-core-image-base-cortexa53-raspberrypi3-64-toolchain-5.0.4.sh
    Poky (Yocto Project Reference Distro) SDK installer version 5.0.4
    =================================================================
    Enter target directory for SDK (default: /opt/poky/5.0.4): /opt/poky/sdk-with-lvgl
    You are about to install the SDK to "/opt/poky/sdk-with-lvgl". Proceed [Y/n]? y

If you want to ensure the SDK was generated with lvgl being installed, go to
the path you extracted the SDK and find all lvgl files:

.. code-block:: bash

    cd /opt/poky/5.0.4/sysroots/cortexa53-poky-linux
    find . -name "*lvgl*"

The ``.so`` files you will find will depend on the LVGL configuration you used.

Now to use the SDK environment and cross-compile an application:

.. code-block:: bash

    source /opt/poke/5.0.4/environment-setup-cortexa53-poky-linux

.. note::

   The tools available in LVGL library will depend on the build configuration
   the recipe was done with.


Custom recipe
*************

Until this section, everything was already done for you. We used existing
recipes. The objective here is to create a recipe from scratch and to add the
generated binary in the image.

Create a layer
==============

First, create a layer and add it to the configuration file

.. code-block::

   bitbake-layers create-layer ../sources/meta-mylvgl
   bitbake-layers add-layer ../sources/meta-mylvgl

In the ``sources`` folder, a new folder was created: ``meta-mylvgl`` and the
directory tree should look like the following

.. code-block:: none

   ├── conf
   │   └── layer.conf
   ├── COPYING.MIT
   ├── README
   └── recipes-example
       └── example
           └── example_0.1.bb


Create a recipe
===============

Following this structure, create a folder containing the recipes to build 1
or multiple applications using lvgl

.. code-block::

   cd ../sources/meta-mylvgl
   mkdir -p recipes-lvglapp/lvgl-fbdev-benchmark/files
   touch recipes-lvglapp/lvgl-fbdev-benchmark/lvglbenchmarkfbdev_2.4.bb

We will focus on 1 application that will clone an lvgl git repository and patch
it for our needs.

The content of ``recipes-lvglapp/lvgl-fbdev-benchmark/lvglbenchmarkfbdev_2.4.
bb``

.. code-block:: none

   DESCRIPTION = "My C++ lvgl app with CMake"

   LICENSE = "MIT"
   LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

   # Update SRC_URI to point to the GitHub repository
   SRC_URI = "gitsm://github.com/lvgl/lv_port_linux.git;branch=master;protocol=https \
              file://0001-change-config-file-to-run-fbdev-with-2-threads.patch \
              file://0002-adapt-CMakeLists-file-to-compile-and-link-fbdev.patch \
             "

   S = "${WORKDIR}/git"

   inherit cmake

   CMAKE_PROJECT_NAME = "lvgl_app"
   CMAKE_PROJECT_VERSION = "2.0"

   do_install() {
       install -d ${D}${bindir}
       install -m 0755 ${S}/bin/lvglbenchmark ${D}${bindir}
   }

The sources come from ``lv_port_linux`` repository. We apply 2 patches to modify the ``CMakeLists.txt`` and ``lv_conf.h``.

Patch 1
-------

Create the first patch file

.. code-block::

   touch 0001-change-config-file-to-run-fbdev-with-2-threads.patch

Content of ``0001-change-config-file-to-run-fbdev-with-2-threads.patch``

.. code-block:: none

    From d49d475d57f69d6172a6b38095ebf3b887f0e731 Mon Sep 17 00:00:00 2001
    From: Gabriel Catel Torres <gabriel.catel@edgemtech.ch>
    Date: Tue, 1 Oct 2024 10:28:55 +0200
    Subject: [PATCH] change config file to run fbdev with 2 threads

    ---
    lv_conf.h | 34 +++++++++++++++++-----------------
    1 file changed, 17 insertions(+), 17 deletions(-)

    diff --git a/lv_conf.h b/lv_conf.h
    index 62a834f..58fbe7a 100644
    --- a/lv_conf.h
    +++ b/lv_conf.h
    @@ -39,9 +39,9 @@
      * - LV_STDLIB_RTTHREAD:    RT-Thread implementation
      * - LV_STDLIB_CUSTOM:      Implement the functions externally
      */
    -#define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN
    -#define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN
    -#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN
    +#define LV_USE_STDLIB_MALLOC    LV_STDLIB_CLIB
    +#define LV_USE_STDLIB_STRING    LV_STDLIB_CLIB
    +#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_CLIB

    #define LV_STDINT_INCLUDE       <stdint.h>
    #define LV_STDDEF_INCLUDE       <stddef.h>
    @@ -89,7 +89,7 @@
      * - LV_OS_WINDOWS
      * - LV_OS_MQX
      * - LV_OS_CUSTOM */
    -#define LV_USE_OS   LV_OS_NONE
    +#define LV_USE_OS   LV_OS_PTHREAD

    #if LV_USE_OS == LV_OS_CUSTOM
        #define LV_OS_CUSTOM_INCLUDE <stdint.h>
    @@ -117,12 +117,12 @@
      * and can't be drawn in chunks. */

    /* The target buffer size for simple layer chunks. */
    -#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    (24 * 1024)   /* [bytes] */
    +#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    (512 * 1024)   /* [bytes] */

    /* The stack size of the drawing thread.
      * NOTE: If FreeType or ThorVG is enabled, it is recommended to set it to 32KB or more.
      */
    -#define LV_DRAW_THREAD_STACK_SIZE    (8 * 1024)   /* [bytes] */
    +#define LV_DRAW_THREAD_STACK_SIZE    (32 * 1024)   /* [bytes] */

    #define LV_USE_DRAW_SW 1
    #if LV_USE_DRAW_SW == 1
    @@ -147,7 +147,7 @@
      /* Set the number of draw unit.
          * > 1 requires an operating system enabled in `LV_USE_OS`
          * > 1 means multiple threads will render the screen in parallel */
    -    #define LV_DRAW_SW_DRAW_UNIT_CNT    1
    +    #define LV_DRAW_SW_DRAW_UNIT_CNT    2

        /* Use Arm-2D to accelerate the sw render */
        #define LV_USE_DRAW_ARM2D_SYNC      0
    @@ -979,8 +979,8 @@
    #define LV_USE_LINUX_FBDEV      1
    #if LV_USE_LINUX_FBDEV
        #define LV_LINUX_FBDEV_BSD           0
    -    #define LV_LINUX_FBDEV_RENDER_MODE   LV_DISPLAY_RENDER_MODE_PARTIAL
    -    #define LV_LINUX_FBDEV_BUFFER_COUNT  0
    +    #define LV_LINUX_FBDEV_RENDER_MODE   LV_DISPLAY_RENDER_MODE_DIRECT
    +    #define LV_LINUX_FBDEV_BUFFER_COUNT  2
        #define LV_LINUX_FBDEV_BUFFER_SIZE   60
    #endif

    @@ -1069,19 +1069,19 @@
    #define LV_USE_DEMO_WIDGETS 1

    /* Demonstrate the usage of encoder and keyboard */
    -#define LV_USE_DEMO_KEYPAD_AND_ENCODER 1
    +#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0

    /* Benchmark your system */
    #define LV_USE_DEMO_BENCHMARK 1

    /* Render test for each primitives. Requires at least 480x272 display */
    -#define LV_USE_DEMO_RENDER 1
    +#define LV_USE_DEMO_RENDER 0

    /* Stress test for LVGL */
    -#define LV_USE_DEMO_STRESS 1
    +#define LV_USE_DEMO_STRESS 0

    /* Music player demo */
    -#define LV_USE_DEMO_MUSIC 1
    +#define LV_USE_DEMO_MUSIC 0
    #if LV_USE_DEMO_MUSIC
        #define LV_DEMO_MUSIC_SQUARE    0
        #define LV_DEMO_MUSIC_LANDSCAPE 0
    @@ -1091,16 +1091,16 @@
    #endif

    /* Flex layout demo */
    -#define LV_USE_DEMO_FLEX_LAYOUT     1
    +#define LV_USE_DEMO_FLEX_LAYOUT     0

    /* Smart-phone like multi-language demo */
    -#define LV_USE_DEMO_MULTILANG       1
    +#define LV_USE_DEMO_MULTILANG       0

    /* Widget transformation demo */
    -#define LV_USE_DEMO_TRANSFORM       1
    +#define LV_USE_DEMO_TRANSFORM       0

    /* Demonstrate scroll settings */
    -#define LV_USE_DEMO_SCROLL          1
    +#define LV_USE_DEMO_SCROLL          0

    /* Vector graphic demo */
    #define LV_USE_DEMO_VECTOR_GRAPHIC  0
    --
    2.34.1


Patch 2
-------

Create the first patch file

.. code-block::

   touch 0002-adapt-CMakeLists-file-to-compile-and-link-fbdev.patch

Content of ``0002-adapt-CMakeLists-file-to-compile-and-link-fbdev.patch``

.. code-block:: none

    From ad464e0683aced74085fb89895b8d040ecad0206 Mon Sep 17 00:00:00 2001
    From: Gabriel Catel Torres <gabriel.catel@edgemtech.ch>
    Date: Tue, 1 Oct 2024 10:31:29 +0200
    Subject: [PATCH] adapt CMakeLists file to compile and link only for fbdev

    ---
    CMakeLists.txt | 17 +++++------------
    1 file changed, 5 insertions(+), 12 deletions(-)

    diff --git a/CMakeLists.txt b/CMakeLists.txt
    index 658193f..ad56cc2 100644
    --- a/CMakeLists.txt
    +++ b/CMakeLists.txt
    @@ -1,8 +1,8 @@
    cmake_minimum_required(VERSION 3.10)
    project(lvgl)

    -set(CMAKE_C_STANDARD 99)#C99 # lvgl officially support C99 and above
    -set(CMAKE_CXX_STANDARD 17)#C17
    +set(CMAKE_C_STANDARD 99)
    +set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
    @@ -10,15 +10,8 @@ set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
    add_subdirectory(lvgl)
    target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR})

    -add_executable(main main.c mouse_cursor_icon.c)
    +add_executable(lvglbenchmark main.c mouse_cursor_icon.c)

    -include(${CMAKE_CURRENT_LIST_DIR}/lvgl/tests/FindLibDRM.cmake)
    -include_directories(${Libdrm_INCLUDE_DIRS})
    -
    -find_package(SDL2)
    -find_package(SDL2_image)
    -include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
    -
    -target_link_libraries(main lvgl lvgl::examples lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${Libdrm_LIBRARIES} m pthread)
    -add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)
    +target_link_libraries(lvglbenchmark lvgl lvgl::examples lvgl::demos lvgl::thorvg m pthread)
    +add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/lvglbenchmark DEPENDS main)

    --
    2.34.1

Build the recipe
================

You should now be able to see the recipe listing the existing recipes

.. code-block::

   bitbake-layers show-recipes | grep lvglbenchmarkfbdev

Now add the recipe to the project. Modify ``build/conf/local.conf`` file adding
this line

.. code-block::

   IMAGE_INSTALL:append = " lvglbenchmarkfbdev"

Now build the image

.. code-block::

   bitbake core-image-base

Run this command to ensure the binary was generated and was installed in the
rootfs

.. code-block::

    # Run this command in build/tmp folder
    find . -wholename "*bin/lvglbench*"
