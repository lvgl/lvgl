.. _nuttx:

==========
NuttX RTOS
==========

What is NuttX?
**************

`NuttX <https://nuttx.apache.org/>`__ is a mature and secure real-time
operating system (RTOS) with an emphasis on technical standards
compliance and small size. It is scalable from 8-bit to 64-bit
microcontrollers and microprocessors and compliant with the Portable
Operating System Interface (POSIX) and the American National Standards
Institute (ANSI) standards and with many Linux-like subsystems. The best
way to think about NuttX is to think of it as a small Unix/Linux for
microcontrollers.

Highlights of NuttX
-------------------

-  **Small** - Fits and runs in microcontrollers as small as 32 kB Flash
   and 8 kB of RAM.
-  **Compliant** - Strives to be as compatible as possible with POSIX
   and Linux.
-  **Versatile** - Supports many architectures (ARM, ARM Thumb, AVR,
   MIPS, OpenRISC, RISC-V 32-bit and 64-bit, RX65N, x86-64, Xtensa,
   Z80/Z180, etc.).
-  **Modular** - Its modular design allows developers to select only
   what really matters and use modules to include new features.
-  **Popular** - NuttX is used by many companies around the world.
   Probably you already used a product with NuttX without knowing it was
   running NuttX.
-  **Predictable** - NuttX is a preemptible Realtime kernel, so you can
   use it to create predictable applications for realtime control.

Why NuttX + LVGL?
*****************

Although NuttX has its own graphic library called
`NX <https://cwiki.apache.org/confluence/pages/viewpage.action?pageId=139629474>`__,
LVGL is a good alternative because users could find more eye-candy demos
and they can reuse code from previous projects. LVGL is an
`Object-Oriented Component
Based <https://blog.lvgl.io/2018-12-13/extend-lvgl-objects>`__
high-level GUI library, that could fit very well for a RTOS with
advanced features like NuttX. LVGL is implemented in C and its APIs are
in C.

Here are some advantages of using LVGL in NuttX
-----------------------------------------------

-  Develop GUI in Linux first and when it is done just compile it for
   NuttX. Nothing more, no wasting of time.
-  Usually, GUI development for low level RTOS requires multiple
   iterations to get things right, where each iteration consists of
   **Change code > Build > Flash > Run**. Using LVGL,
   Linux and NuttX you can reduce this process and just test everything
   on your computer and when it is done, compile it on NuttX and that is
   it.

NuttX + LVGL could be used for
------------------------------

-  GUI demos to demonstrate your board graphics capacities.
-  Fast prototyping GUI for MVP (Minimum Viable Product) presentation.
-  visualize sensor data directly and easily on the board without using
   a computer.
-  Final products with a GUI without a touchscreen (i.e. 3D Printer
   Interface using Rotary Encoder to Input data).
-  Final products with a touchscreen (and all sorts of bells and
   whistles).

How to get started with NuttX and LVGL?
***************************************

There are many boards in the `NuttX
mainline <https://github.com/apache/incubator-nuttx>`__ with support for
LVGL. Let's use the
`STM32F429IDISCOVERY <https://www.st.com/en/evaluation-tools/32f429idiscovery.html>`__
as an example because it is a very popular board.

First you need to install the pre-requisites on your system
-----------------------------------------------------------

Let's use the `Windows Subsystem for
Linux <https://acassis.wordpress.com/2018/01/10/how-to-build-nuttx-on-windows-10/>`__

.. code-block:: shell

   $ sudo apt-get install automake bison build-essential flex gcc-arm-none-eabi gperf git libncurses5-dev libtool libusb-dev libusb-1.0.0-dev pkg-config kconfig-frontends openocd

Now let's create a workspace to save our files
----------------------------------------------

.. code-block:: shell

   $ mkdir ~/nuttxspace
   $ cd ~/nuttxspace

Clone the NuttX and Apps repositories
-------------------------------------

.. code-block:: shell

   $ git clone https://github.com/apache/incubator-nuttx nuttx
   $ git clone https://github.com/apache/incubator-nuttx-apps apps

NuttX Simulator
---------------

The NuttX simulator allows you to run NuttX and LVGL on your PC.
This can be especially useful for developing LVGL UIs with immediate
visual feedback, and also developing platform independent NuttX apps.

Configure NuttX to use the ``sim`` board and the LVGL Demo
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: shell

   $ ./tools/configure.sh sim:lvgl_fb
   $ make

If everything went fine you should have now the file ``nuttx``

.. code-block:: shell

   $ ls -l nuttx
   -rwxr-xr-x 1 root root 2819920 May 12 15:01 nuttx

Running the NuttX Simulator
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: shell

   $ ./nuttx

Using the ``NSH>`` terminal start the LVGL demo:

.. code-block:: shell

   nsh> lvgldemo

NuttX On-Device
---------------

If you don't have an STM32F429I-DISCO, this process will be similar
for other boards.

Configure NuttX to use the stm32f429i-disco board and the LVGL Demo
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: shell

   $ ./tools/configure.sh stm32f429i-disco:lvgl
   $ make

If everything went fine you should have now the file ``nuttx.bin`` to
flash on your board:

.. code-block:: shell

   $ ls -l nuttx.bin
   -rwxrwxr-x 1 alan alan 287144 Jun 27 09:26 nuttx.bin

Flashing the firmware in the board using OpenOCD
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: shell

   $ sudo openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg -c init -c "reset halt" -c "flash write_image erase nuttx.bin 0x08000000"

Reset the board and using the 'NSH>' terminal start the LVGL demo:

.. code-block:: shell

   nsh> lvgldemo

Custom Usage
************

The example app called ``lvgldemo`` uses LVGL's NuttX integration
to run the demos in NuttX.

https://github.com/apache/nuttx-apps/tree/master/examples/lvgldemo

It is used by all LVGL defconfigs. See ``sim:lvgl_fb`` and ``sim:lvgl_lcd``.

You can use it as a reference for adding LVGL to your own NuttX app.
Start by copying the contents of ``lvgldemo.c``. It handles fb and lcd based
display drivers, the touchscreen input driver, and libuv.

NuttX Driver Support
********************

-  **fbdev** - LVGL can create a display for ``/dev/fb*`` devices.
-  **lcd** - LVGL can create a display for ``/dev/lcd*`` devices.
-  **input/uinput touchscreen** - LVGL can create an indev(s) for
   touch devices that use the NuttX input/uinput system.
-  **libuv** - LVGL has support for using libuv as its event loop manager.
   libuv apps can integrate seamlessly.

There is no support for input/uinput keyboards yet. The NuttX X11 sim
keyboard driver emits keycodes that are not standard so the LVGL integration
with it would not be generic, if it were added.

Other Integrations
******************

-  LVGL+NuttX profiler integration
-  Optional dedicated image cache heap

Configurations
**************

The LVGL library is a NuttX "app". You can configure LVGL in ``menuconfig``.

.. code-block:: shell

    $ ./tools/configure.sh stm32f429i-disco:lvgl
    $ make menuconfig

The LVGL `Kconfig`
file provides the available config options to the NuttX build system. The LVGL configs in
``menuconfig`` can be found under
**Application Configuration > Graphics Support > Light and Versatile Graphic Library (LVGL) > LVGL configuration**.

To find the location of a specific config, Press the "/" key to open a search interface.

Here are some configurations that you can use to customize your NuttX and LVGL setup:

-  **LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP** - You can enable or disable the
   LVGL image heap in NuttX. By default, it is disabled. If you enable
   it, LVGL will use the NuttX heap instead.

Using :c:macro:`LV_STDLIB_CLIB` for :c:macro:`LV_USE_STDLIB_MALLOC`,
:c:macro:`LV_USE_STDLIB_STRING`, or :c:macro:`LV_USE_STDLIB_SPRINTF` is
reasonable as NuttX fully implements these standard library APIs. Whether or
not you choose to use NuttX's ``malloc`` depends on whether you want LVGL
to allocate from the NuttX global heap or use its own.

Where is ``LV_OS_NUTTX``?
-------------------------

NuttX tries to be POSIX compliant where possible, meaning it supports
pthreads (POSIX threads). To enable OS features in LVGL on NuttX,
set :c:macro:`LV_USE_OS` to :c:macro:`LV_OS_PTHREAD`.
The main reason for enabling OS features is for multi-core rendering.
See :c:macro:`LV_DRAW_SW_DRAW_UNIT_CNT`. Otherwise there is no case
for setting it.

Using a Specific Version of LVGL
********************************

Here are the steps to use the latest (or any) version of LVGL in NuttX.

First ensure the build artifacts have been cleared. It will remove the existing unpacked LVGL and ZIP
if there is one.

.. code-block:: shell

    $ make distclean

Now you can ``git clone`` the latest LVGL into the NuttX apps tree. It will persist even after ``make distclean``
because the build system will not remove lvgl if it is a Git repo.

.. code-block:: shell

    $ cd /path/to/nuttxspace/apps/graphics/lvgl
    $ git clone https://github.com/lvgl/lvgl.git

Finally, you must copy the content of ``apps/graphics/lvgl/lvgl/Kconfig`` into
the middle of ``apps/graphics/lvgl/Kconfig``. See the "PASTE THE CONTENTS ..." section below.

``apps/graphics/lvgl/Kconfig``:

.. code-block:: kconfig

    #
    # For a description of the syntax of this configuration file,
    # see the file kconfig-language.txt in the NuttX tools repository.
    #

    menuconfig GRAPHICS_LVGL
    	bool "Light and Versatile Graphic Library (LVGL)"
    	default n
    	---help---
    		Enable support for the LVGL GUI library.

    if GRAPHICS_LVGL

    (PASTE THE CONTENTS OF apps/graphics/lvgl/lvgl/Kconfig HERE)

    config LV_OPTLEVEL
    	string "Customize compilation optimization level"
    	default ""

    endif # GRAPHICS_LVGL

Where can I find more information?
**********************************

-  This blog post: `LVGL on
   LPCXpresso54628 <https://acassis.wordpress.com/2018/07/19/running-nuttx-on-lpcxpresso54628-om13098/>`__
-  NuttX mailing list: `Apache NuttX Mailing
   List <http://nuttx.incubator.apache.org/community/>`__


.. Comment:  The above blank line is necessary for Sphinx to not complain,
    since it looks for the blank line after a bullet list.
