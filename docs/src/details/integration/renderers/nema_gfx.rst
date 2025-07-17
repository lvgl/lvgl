.. _nema_gfx:

====================
NemaGFX Acceleration
====================

NemaGFX is a high-level graphics API supported
by many embedded 2.5D GPUs. LVGL can use it to
natively render LVGL graphics.

Get Started with the Riverdi STM32U5 5-inch Display
***************************************************

`lv_port_riverdi_stm32u5 <https://github.com/lvgl/lv_port_riverdi_stm32u5>`__
is a ready-to-use port for the Riverdi STM32 5.0" Embedded Display
(STM32U599NJH6Q or STM32U5A9NJH6Q) which has Nema enabled.
Follow the instructions in the readme to get started.

`lv_port_stm32u5g9j-dk2 <https://github.com/lvgl/lv_port_stm32u5g9j-dk2>`__
is a ready-to-use port for the STM32U5G9J-DK2 devkit with a 5.0" display
and a NeoChrom VG GPU.
Follow the instructions in the readme to get started.

Usage and Configuration
***********************

Enable the renderer by setting :c:macro:`LV_USE_NEMA_GFX` to ``1`` in
lv_conf.h. If using :c:macro:`LV_USE_NEMA_VG`,
set :c:macro:`LV_NEMA_GFX_MAX_RESX` and :c:macro:`LV_NEMA_GFX_MAX_RESY`
to the size of the display you will be using so that enough static
memory will be reserved for VG. Without VG, more task types will be
performed by the software renderer.

"libs/nema_gfx" contains pre-compiled binaries for the Nema GPU drivers.

`lv_port_riverdi_stm32u5 <https://github.com/lvgl/lv_port_riverdi_stm32u5>`__
is already configured to link the "cortex_m33_revC" binaries when building
and `lv_port_stm32u5g9j-dk2 <https://github.com/lvgl/lv_port_stm32u5g9j-dk2>`__
is configured to link the "cortex_m33_NemaPVG" binaries when building.

"cortex_m33_revC" works on all STM32 m33 devices with a Nema core while "cortex_m33_NemaPVG"
additionally supports the tesselation and matrix multiplication acceleration that
the STM32 U5F and U5G Nema cores are capable of.

With a different STM32CubeIDE project, you can configure the libraries to be linked
by right-clicking the project in the "Project Explorer" sidebar, clicking
"Properties", navigating to "C/C++ Build", "Settings", "MCU G++ Linker", and then
"Libraries". Add an entry under "Libraries (-l)" that is "nemagfx-float-abi-hard".
Add an entry under "Library search path (-L)" which is a path to
"libs/nema_gfx/lib/core/cortex_m33_revC/gcc" e.g.
"${workspace_loc:/${ProjName}/Middlewares/LVGL/lvgl/libs/nema_gfx/lib/core/cortex_m33_revC/gcc}".
You will also want to add the "libs/nema_gfx/include" directory to your include
search paths. Under "MCU GCC Compiler", "Include paths", add an entry to "Include paths (-I)"
which is a path to "libs/nema_gfx/include" e.g.
"${workspace_loc:/${ProjName}/Middlewares/LVGL/lvgl/libs/nema_gfx/include}".
Click "Apply and Close".

.. note::

    ``cannot find -lnemagfx-float-abi-hard: No such file or directory``

    The library version for ST was updated to ``revC`` so the path component
    ``cortex_m33`` must be updated to ``cortex_m33_revC`` in STM32CubeIDE.


32 and 16 bit :c:macro:`LV_COLOR_DEPTH` is supported.

At the time of writing, :c:macro:`LV_USE_OS` support is experimental
and not yet working in
`lv_port_riverdi_stm32u5 <https://github.com/lvgl/lv_port_riverdi_stm32u5>`__

NemaGFX requires a simple HAL implementation to allocate memory and optionally
lock resources. You may use a custom HAL implementation for your platform or use one of the
provided implementations by setting :c:macro:`LV_USE_NEMA_HAL` to a value other than
:c:macro:`LV_NEMA_HAL_CUSTOM`.

TSC Images
**********

TSC (ThinkSilicon Compression) images can be drawn by this renderer. The
TSC 4/6/6A/12/12A color formats are part of :cpp:type:`lv_color_format_t`.
All other renderers will ignore images with these color formats.
Define an image descriptor variable with the corresponding
TSC color format and the GPU will be able to draw it directly.
Stride does not need to be specified because it will be computed by the
renderer.

.. code-block:: c

    const lv_image_dsc_t img_demo_widgets_avatar_tsc6a = {
        .header.cf = LV_COLOR_FORMAT_NEMA_TSC6A,
        .header.w = 144,
        .header.h = 144,
        .data = img_demo_widgets_avatar_tsc6a_map,
        .data_size = sizeof(img_demo_widgets_avatar_tsc6a_map),
    };

DMA2D
*****

The Nema renderer uses DMA2D to flush in parallel with rendering in
`lv_port_riverdi_stm32u5 <https://github.com/lvgl/lv_port_riverdi_stm32u5>`__.

If your STM does not have the Nema GPU, it may still support
DMA2D. DMA2D is a simple peripheral which can draw fills
and images independently of the CPU.
See the LVGL :ref:`DMA2D support <dma2d>`.

API
***

:ref:`lv_draw_nema_gfx_h`

:ref:`lv_draw_nema_gfx_utils_h`
