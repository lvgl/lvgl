***
NXP
***

NXP has integrated LVGL into the MCUXpresso SDK packages for general
purpose and crossover microcontrollers, allowing easy evaluation and
migration into your product design.
`Download an SDK for a supported board <https://www.nxp.com/design/software/embedded-software/littlevgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY?&tid=vanLITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY>`__
today and get started with your next GUI application.


Creating new project with LVGL
==============================

Downloading the MCU SDK example project is recommended as a starting
point. It comes fully configured with LVGL (and with PXP/VGLite support
if the modules are present), no additional integration work is required.


HW acceleration for NXP iMX RT platforms
========================================

Depending on the RT platform used, the acceleration can be done by NXP
PXP (PiXel Pipeline) and/or the Verisilicon GPU through an API named
VGLite. Each accelerator has its own context that allows them to be used
individually as well simultaneously (in LVGL multithreading mode).


.. toctree::
    :maxdepth: 2

    /overview/renderers/nxp/pxp
    /overview/renderers/nxp/vglite

