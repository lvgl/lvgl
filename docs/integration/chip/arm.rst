.. _arm:

===
Arm
===

Arm is a leading semiconductor and software design company, renowned for creating the Cortex-M microcontroller (MCU) cores and Cortex-A (MPU) processor cores, which are integral to a wide range of devices. These cores are at the heart of many embedded systems, powering chips from industry giants such as STMicroelectronics, NXP, and Renesas. Arm's energy-efficient designs are used in billions of devices worldwide, from microcontrollers to smartphones and servers. By licensing their processor designs, Arm enables a broad ecosystem of partners to develop customized solutions optimized for performance, power, and size. Arm's architecture is highly compatible with various operating systems and software libraries, including LVGL, making it a versatile choice for developers creating efficient, high-performance graphical user interfaces.

Compile LVGL for Arm
--------------------

No specific action is required. Any compiler supporting the target Arm architecture can be used to compile LVGL's source code too. It includes GCC, LLVM, Ac6.

It's also possible to cross-compile LVGL to an MPU (instead of compiling it on the target hardware), or a shared library can be built as well. For more information check out :ref:`build_cmake`

Getting started with Ac6
~~~~~~~~~~~~~~~~~~~~~~~~

Ac6 is the proprietary compiler of Arm. As Ac6 maintained by Arm it contains a lot of specific optimization, so you can expect the best performance by using it.

Ac6 is not free, but it has a community license which can be activated like this:

1. Download and install the Ac6 compiler from from `Arm's website <https://developer.arm.com/Tools%20and%20Software/Arm%20Compiler%20for%20Embedded>`__
2. To register a community license go to ``bin`` folder of the compiler and in a Terminal run ``armlm.exe activate -server https://mdk-preview.keil.arm.com -product KEMDK-COM0``

Arm2D
-----

Arm Cortex-M55 and Cortex-M85 has the `SIMD Helium <https://www.arm.com/technologies/helium>`__ instructions set. It can effectively speed up UI rendering.

Arm2D is a library maintained by Arm, and it can be used to leverage the power Helium. By enabling ``LV_USE_DRAW_ARM2D_SYNC 1`` in ``lv_conf.h`` LVGL will use Arm2D's API to speed up software rendering.

Note that Arm2D cannot be compiled with GCC, and it's recommended to use it with Ac6.

To add Arm2D to you project follow these steps:

- In order to utilize its power be sure to set ``mcpu`` to ``cortex-m85`` and add the ``-fvectorize`` flag. To test without SIMD use ``cortex-m85+nomve``
- Arm2D can be downloaded from `https://github.com/ARM-software/Arm-2D <https://github.com/ARM-software/Arm-2D>`__ .
- The CMSIS DSP library also needs to be added to the project
- For better performance be sure the LTO is enabled and -Omax is used. -Omax cannot be selected from the list, but needs to be added manually as compiler flag (see above)
Arm2D tries to read/write multiple data with a single instruction. Therefore it's important to use the fastest memory for LVGL's buffer. Usually it's the BSS and on this board it's 64kB. An array can be placed to this section like this: static uint8_t partial_draw_buf[64 * 1024] BSP_PLACE_IN_SECTION(".bss.dtcm_bss") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);


