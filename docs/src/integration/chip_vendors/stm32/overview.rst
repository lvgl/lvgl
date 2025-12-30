========
Overview
========


About
*****

STMicroelectronics is a chip vendor and board manufacturer of 32-bit ARM
MCUs and MPUs.


Application Development
-----------------------

STM32CubeIDE is an Eclipse-based IDE which is typically used to develop for STM32.
It can configure your project, build,
flash, and debug. It integrates LVGL similarly to any other Eclipse-based IDE.
The projects it generates are STM32 HAL based. The STM32 HAL is maintained by ST
and is a good fit for many applications. STM32CubeIDE can optionally generate projects that
use FreeRTOS.

`STM32CubeIDE can be downloaded from here. <https://www.st.com/en/development-tools/stm32cubeide.html>`_

STM32 projects can be created with STM32CubeIDE or with other frameworks that don't use the STM32 HAL.
There are many ways to develop for STM32
including manual bare metal, Zephyr RTOS, NuttX RTOS, ChibiOS, and many more. Frameworks
like Zephyr and NuttX often cause a project to be treated like "a Zephyr project on STM32"
rather than an "STM32 project running Zephyr", so their documentation is where to start.
Also worth mentioning is STM32CubeMX; a graphical tool for generating the boilerplate code for an
STM32 HAL project to build it outside of STM32CubeIDE.
Furthermore, STM32CubeCLT is a toolset for integrating ST proprietary tools into
other IDEs such as Visual Studio Code.


Ready-to-Use Projects
---------------------

LVGL maintains a few projects for STM32 boards. See `the boards page <https://lvgl.io/boards#st>`__
for ST boards that have been certified by LVGL and have up-to-date LVGL integration, and check
`all the repos <https://github.com/orgs/lvgl/repositories?q=lv_port_stm>`__
for other ST board repos that exist. Follow the README.md files in those repos
for specific instructions to get started with them.


LVGL Support for STM32 Graphical Peripherals
--------------------------------------------

LVGL has good support for the graphical hardware acceleration peripherals that some STM32
models feature. Sometimes these features can be utilized together at the same time.
See the individual pages about them.

- :ref:`LTDC: display controller. <stm32 ltdc driver>`
- :ref:`DMA2D: asynchronous pixel data memory operations. <dma2d>`
- :ref:`NeoChrom: GPU for rendering 2D graphics primitives. <neochrom>`

Each page mentions if/how they can be used with each other.
