.. _neochrom:

========
NeoChrom
========

LVGL integrates NeoChrom as a :ref:`draw unit <draw units>`.


Ready-to-use Projects that have NeoChrom enabled
************************************************

`lv_port_riverdi_stm32u5 <https://github.com/lvgl/lv_port_riverdi_stm32u5>`__
is a ready-to-use port for the Riverdi STM32 5.0" Embedded Display
(STM32U599NJH6Q or STM32U5A9NJH6Q) which has NeoChrom enabled.
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

"libs/nema_gfx" contains pre-compiled binaries for the NeoChrom GPU drivers.

`lv_port_riverdi_stm32u5 <https://github.com/lvgl/lv_port_riverdi_stm32u5>`__
is already configured to link the "cortex_m33_revC" binaries when building
and `lv_port_stm32u5g9j-dk2 <https://github.com/lvgl/lv_port_stm32u5g9j-dk2>`__
is configured to link the "cortex_m33_NemaPVG" binaries when building.

"cortex_m33_revC" works on all STM32 m33 devices with a NeoChrom core while "cortex_m33_NemaPVG"
additionally supports the tessellation and matrix multiplication acceleration that
the STM32 U5F and U5G NeoChrom cores are capable of.

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

NeoChrom requires a simple HAL implementation to allocate memory and optionally
lock resources. You may use a custom HAL implementation for your platform or use one of the
provided implementations by setting :c:macro:`LV_USE_NEMA_HAL` to a value other than
:c:macro:`LV_NEMA_HAL_CUSTOM`.


Vector Graphics
***************

The NeoChrom VG driver in LVGL can render vector graphics Widgets using NeoChrom VG's
hardware support for vector graphics drawing. You can display SVG files in your application.
See the SVG examples for usage.

To use vector graphics with NeoChrom, you should enable the following configs in ``lv_conf.h``.

.. code-block::

    LV_USE_NEMA_GFX 1
    LV_USE_NEMA_VG 1
    LV_USE_VECTOR_GRAPHIC 1
    LV_USE_MATRIX 1
    LV_USE_FLOAT 1

To use the SVG widget, additionally enable ``LV_USE_SVG``.

If there is RAM available, SVG performance can be increased by enabling the image cache,
``LV_CACHE_DEF_SIZE``.
``LV_CACHE_DEF_SIZE`` is a cache size in bytes. If it is large enough for your SVGs,
it will cache decoded SVG data so it does not need to be parsed every refresh, significantly
reducing SVG redraw time.

``LV_USE_DEMO_VECTOR_GRAPHIC`` is a demo you can enable which draws some vector graphics shapes.
Gradient and image fills are not supported yet, as well as dashed strokes. These are
missing from the demo when it is run with the NeoChrom driver.


TSC Images
**********

`The TSC converter can be downloaded from here. <https://drive.google.com/file/d/1wUcurTe1bvwC6e-tMDbXd1-e5kS8YdQK/view?usp=sharing>`_

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


Interop with the LTDC driver and the DMA2D Draw Unit
****************************************************

NeoChrom can be enabled at the same time as LTDC. They will not interfere
with each other at all.

NeoChrom and DMA2D may be enabled at the same time. They are both draw units
and they will both independently accept draw tasks.


API
***

.. API startswith:  lv_draw_nema_

.. API startswith:  lv_nemagfx_
