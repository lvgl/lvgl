.. _rt_thread:

==============
RT-Thread RTOS
==============

What is RT-Thread?
------------------

`RT-Thread <https://www.rt-thread.io/>`__ is an `open
source <https://github.com/RT-Thread/rt-thread>`__, neutral, and
community-based real-time operating system (RTOS). RT-Thread has
**Standard version** and **Nano version**. For resource-constrained
microcontroller (MCU) systems, the Nano version that requires only 3 KB
Flash and 1.2 KB RAM memory resources can be tailored with easy-to-use
tools. For resource-rich IoT devices, RT-Thread can use the **online
software package** management tool, together with system configuration
tools, to achieve intuitive and rapid modular cutting, seamlessly import
rich software packages; thus, achieving complex functions like Android's
graphical interface and touch sliding effects, smart voice interaction
effects, and so on.

Key features
~~~~~~~~~~~~

-  Designed for resource-constrained devices, the minimum kernel
   requires only 1.2KB of RAM and 3 KB of Flash.
-  A variety of standard interfaces, such as POSIX, CMSIS, C++
   application environment.
-  Has rich components and a prosperous and fast growing `package ecosystem <https://packages.rt-thread.org/en/>`__
-  Elegant code style, easy to use, read and master.
-  High Scalability. RT-Thread has high-quality scalable software
   architecture, loose coupling, modularity, is easy to tailor and
   expand.
-  Supports high-performance applications.
-  Supports all mainstream compiling tools such as GCC, Keil and IAR.
-  Supports a wide range of `architectures and chips <https://www.rt-thread.io/board.html>`__

How to run LVGL on RT-Thread?
-----------------------------

`中文文档 <https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/packages-manual/lvgl-docs/introduction>`__

LVGL has registered as a
`softwarepackage <https://packages.rt-thread.org/en/detail.html?package=LVGL>`__
of RT-Thread. By using
`Env tool <https://www.rt-thread.io/download.html?download=Env>`__ or
`RT-Thread Studio IDE <https://www.rt-thread.io/download.html?download=Studio>`__,
RT-Thread users can easily download LVGL source code and combine with
RT-Thread project.

RT-Thread community has port LVGL to several Board-Support Packages (BSPs):

+---------------------------------------+------------------------+
| BSP                                   | BSP                    |
+=======================================+========================+
| `QEMU simulator`_                     | `Renesas ra6m3-ek`_    |
+---------------------------------------+------------------------+
| `Visual Studio simulator`_            | `Renesas ra6m4-cpk`_   |
+---------------------------------------+------------------------+
| `Nuvoton numaker-iot-m487`_           | `Renesas ra6m3-hmi`_   |
+---------------------------------------+------------------------+
| `Nuvoton numaker-pfm-m487`_           | `Renesas ra8d1-ek`_    |
+---------------------------------------+------------------------+
| `Nuvoton nk-980iot`_                  | `Renesas ra8m1-ek`_    |
+---------------------------------------+------------------------+
| `Nuvoton numaker-m2354`_              | `STM32F407 explorer`_  |
+---------------------------------------+------------------------+
| `Nuvoton nk-n9h30`_                   | `STM32F407 RT-Spark`_  |
+---------------------------------------+------------------------+
| `Nuvoton numaker-m032ki`_             | `STM32F469 Discovery`_ |
+---------------------------------------+------------------------+
| `Nuvoton numaker-hmi-ma35d1`_         | `STM32H750 ART-Pi`_    |
+---------------------------------------+------------------------+
| `Nuvoton numaker-iot-m467`_           | `STM32H787 Discovery`_ |
+---------------------------------------+------------------------+
| `Nuvoton numaker-m467hj`_             | `STM32L475 pandora`_   |
+---------------------------------------+------------------------+
| `Infineon psoc6-evaluationkit-062S2`_ | `NXP imxrt1060-evk`_   |
+---------------------------------------+------------------------+
| `Raspberry PICO`_                     | `NXP LPC55S69`_        |
+---------------------------------------+------------------------+

Tutorials
~~~~~~~~~

-  `Introduce about RT-Thread and how to run LVGL on RT-Thread in simulators`_
-  `How to import a BSP project with latest code into RT-Thread Studio`_
-  `How to Use LVGL with RT-Thread Studio in STM32F469 Discovery Board`_
-  `RT-Thread Youtube Channel`_
-  `RT-Thread documentation center`_

.. ------------------------------------------------------------------------
.. RT-Thread External Links
.. ------------------------------------------------------------------------
.. _Infineon psoc6-evaluationkit-062S2:  https://github.com/RT-Thread/rt-thread/tree/master/bsp/Infineon/psoc6-evaluationkit-062S2/applications/lvgl
.. _Nuvoton nk-980iot:                   https://github.com/RT-Thread/rt-thread/tree/master/bsp/nuvoton/nk-980iot/applications/lvgl
.. _Nuvoton nk-n9h30:                    https://github.com/RT-Thread/rt-thread/tree/master/bsp/nuvoton/nk-n9h30/applications/lvgl
.. _Nuvoton numaker-hmi-ma35d1:          https://github.com/RT-Thread/rt-thread/tree/master/bsp/nuvoton/numaker-hmi-ma35d1/applications/lvgl
.. _Nuvoton numaker-iot-m467:            https://github.com/RT-Thread/rt-thread/tree/master/bsp/nuvoton/numaker-iot-m467/applications/lvgl
.. _Nuvoton numaker-iot-m487:            https://github.com/RT-Thread/rt-thread/tree/master/bsp/nuvoton/numaker-iot-m487/applications/lvgl
.. _Nuvoton numaker-m032ki:              https://github.com/RT-Thread/rt-thread/tree/master/bsp/nuvoton/numaker-m032ki/applications/lvgl
.. _Nuvoton numaker-m2354:               https://github.com/RT-Thread/rt-thread/tree/master/bsp/nuvoton/numaker-m2354/applications/lvgl
.. _Nuvoton numaker-m467hj:              https://github.com/RT-Thread/rt-thread/tree/master/bsp/nuvoton/numaker-m467hj/applications/lvgl
.. _Nuvoton numaker-pfm-m487:            https://github.com/RT-Thread/rt-thread/tree/master/bsp/nuvoton/numaker-pfm-m487/applications/lvgl
.. _NXP imxrt1060-evk:                   https://github.com/RT-Thread/rt-thread/tree/master/bsp/nxp/imx/imxrt/imxrt1060-nxp-evk/applications/lvgl
.. _NXP LPC55S69:                        https://github.com/RT-Thread/rt-thread/tree/master/bsp/nxp/lpc/lpc55sxx/lpc55s69_nxp_evk/applications/lvgl
.. _QEMU simulator:                      https://github.com/RT-Thread/rt-thread/tree/master/bsp/qemu-vexpress-a9/applications/lvgl
.. _Raspberry PICO:                      https://github.com/RT-Thread/rt-thread/tree/master/bsp/raspberry-pico/RP2040/applications/lvgl
.. _Renesas ra6m3-ek:                    https://github.com/RT-Thread/rt-thread/tree/master/bsp/renesas/ra6m3-ek/board/lvgl
.. _Renesas ra6m3-hmi:                   https://github.com/RT-Thread/rt-thread/tree/master/bsp/renesas/ra6m3-hmi-board/board/lvgl
.. _Renesas ra6m4-cpk:                   https://github.com/RT-Thread/rt-thread/tree/master/bsp/renesas/ra6m4-cpk/board/lvgl
.. _Renesas ra8d1-ek:                    https://github.com/RT-Thread/rt-thread/tree/master/bsp/renesas/ra8d1-ek/board/lvgl
.. _Renesas ra8m1-ek:                    https://github.com/RT-Thread/rt-thread/tree/master/bsp/renesas/ra8m1-ek/board/lvgl
.. _Visual Studio simulator:             https://github.com/RT-Thread/rt-thread/tree/master/bsp/simulator/applications/lvgl
.. _STM32F407 explorer:                  https://github.com/RT-Thread/rt-thread/tree/master/bsp/stm32/stm32f407-atk-explorer/applications/lvgl
.. _STM32F407 RT-Spark:                  https://github.com/RT-Thread/rt-thread/tree/master/bsp/stm32/stm32f407-rt-spark/board/ports/lvgl
.. _STM32F469 Discovery:                 https://github.com/RT-Thread/rt-thread/tree/master/bsp/stm32/stm32f469-st-disco/applications/lvgl
.. _STM32H750 ART-Pi:                    https://github.com/RT-Thread/rt-thread/tree/master/bsp/stm32/stm32h750-artpi/applications/lvgl
.. _STM32H787 Discovery:                 https://github.com/RT-Thread/rt-thread/tree/master/bsp/stm32/stm32h7s7-st-disco/applications/lvgl
.. _STM32L475 pandora:                   https://github.com/RT-Thread/rt-thread/tree/master/bsp/stm32/stm32l475-atk-pandora/applications/lvgl
.. _Introduce about RT-Thread and how to run LVGL on RT-Thread in simulators:
                                         https://www.youtube.com/watch?v=k7QYk6hSwnc
.. _How to import a BSP project with latest code into RT-Thread Studio:
                                         https://www.youtube.com/watch?v=fREPLuh-h8k
.. _How to Use LVGL with RT-Thread Studio in STM32F469 Discovery Board:
                                         https://www.youtube.com/watch?v=O_QA99BxnOE
.. _RT-Thread Youtube Channel:           https://www.youtube.com/channel/UCdDHtIfSYPq4002r27ffqPw
.. _RT-Thread documentation center:      https://www.rt-thread.io/document/site/
