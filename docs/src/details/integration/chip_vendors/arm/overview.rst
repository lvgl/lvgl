.. _arm_overview:

========
Overview
========

Arm is a leading semiconductor and software design company, renowned for creating the Cortex-M microcontroller (MCU) cores and Cortex-A/R (MPU) processor cores, which are integral to a wide range of devices. These cores are at the heart of many embedded systems, powering chips from industry giants such as STMicroelectronics, NXP, and Renesas. Arm's energy-efficient designs are used in billions of devices worldwide, from microcontrollers to smartphones and servers. By licensing their processor designs, Arm enables a broad ecosystem of partners to develop customized solutions optimized for performance, power, and size. Arm's architecture is highly compatible with various operating systems and software libraries, including LVGL, making it a versatile choice for developers creating efficient, high-performance graphical user interfaces.

Compile LVGL for Arm
--------------------

No specific action is required. Any compiler that supports the target Arm architecture can be used to compile LVGL's source code, including GCC, LLVM, and AC6.

It is also possible to cross-compile LVGL for an MPU (instead of compiling it on the target hardware) or create a shared library. For more information, check out :ref:`build_cmake`.

Getting Started with AC6
~~~~~~~~~~~~~~~~~~~~~~~~

Since AC6 is a proprietary toolchain, it contains many specific optimizations, so you can expect the best performance when using it.

AC6 is not free, but it offers a community license that can be activated as follows:

1. Download and install the AC6 compiler from `Arm's website <https://developer.arm.com/Tools%20and%20Software/Arm%20Compiler%20for%20Embedded>`__.
2. To register a community license, go to the ``bin`` folder of the compiler and, in a terminal, run ``armlm.exe activate -server https://mdk-preview.keil.arm.com -product KEMDK-COM0`` (On Linux, use ``./armlm``).

IDE Support
-----------

There are no limitations on the supported IDEs. LVGL works in various vendors' IDEs, including Arm's Keil MDK, IAR, Renesas's e2 studio, NXP's MCUXpresso, ST's CubeIDE, as well as custom make or CMake projects.

Arm2D and the Helium instruction set
------------------------------------

Arm Cortex-M55 and Cortex-M85 have the `SIMD Helium <https://www.arm.com/technologies/helium>`__ instruction set.
Among many others, this can effectively speed up UI rendering. :ref:`Arm2D <arm2d>` is a library maintained by Arm that leverages the Helium instruction set.

Note that GCC has some known issues with Helium intrinsics. It is recommended to use AC6 or LLVM when dealing with Helium code.


To add Arm2D to your project, follow these steps:

1. To utilize its power, ensure that ``mcpu`` is set to ``cortex-m85``, ``cortex-m55``, or ``cortex-m52`` and add the ``-fvectorize`` flag. To test without SIMD, use e.g. ``cortex-m85+nomve``.
2. Arm2D can be downloaded from `https://github.com/ARM-software/Arm-2D <https://github.com/ARM-software/Arm-2D>`__. Consider using the ``developing`` branch, which contains the latest updates.
3. Add ``Arm-2D/Library/Include`` to the include paths.
4. Copy ``Arm-2D/Library/Include/template/arm_2d_cfg.h`` to any location you prefer to provide the default configuration for Arm2D. Ensure that the folder containing ``arm_2d_cfg.h`` is added to the include path.
5. The Arm2D repository contains several examples and templates; however, ensure that only ``Arm-2D/Library/Source`` is compiled.
6. The CMSIS DSP library also needs to be added to the project. You can use CMSIS-PACKS or add it manually.
7. For better performance, enable ``LTO`` (Link Time Optimization) and use ``-Omax`` or ``-Ofast``.
8. Arm2D tries to read/write multiple data with a single instruction. Therefore, it's important to use the fastest memory (e.g., ``BSS`` or ``TCM``) for LVGL's buffer to avoid memory bandwidth bottlenecks.
9. Enable ``LV_USE_DRAW_ARM2D_SYNC 1`` and ``LV_USE_DRAW_SW_ASM LV_DRAW_SW_ASM_HELIUM`` in ``lv_conf.h``.

Neon Acceleration
-----------------

Some ARM Cortex-A and Cortex-R processors with the ARMv7 architecture and every ARM Cortex-A and Cortex-R processor from the ARMv8 architecture support the `Neon SIMD <https://www.arm.com/technologies/neon>` instruction set.
LVGL has built-in support to improve the performance of software rendering by utilizing Neon instructions.

Architecture Support
--------------------

Both 32-bit and 64-bit ARM architectures are supported. Simply set ``LV_USE_DRAW_SW_ASM`` to ``LV_DRAW_SW_ASM_NEON`` in ``lv_conf``.

.. note::
   All ARM Cortex-A and Cortex-R 64-bit processors include Neon support as a mandatory feature starting with the ARMv8 architecture specification. This makes Neon acceleration universally available on all 64-bit ARM platforms, including current and future ARM architectures.
