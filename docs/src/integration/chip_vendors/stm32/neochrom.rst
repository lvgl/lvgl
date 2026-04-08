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

`lv_port_stm32h7s78-dk <https://github.com/lvgl/lv_port_stm32h7s78-dk>`__
is a ready-to-use port for the STM32H7S78-DK devkit with a 5.0" display
and a NeoChrom GPU. It's a high performance MCU with an ARM Cortex-M7 core.
Follow the instructions in the readme to get started.


Usage and Configuration
***********************

Enable the renderer by setting :c:macro:`LV_USE_NEMA_GFX` to ``1`` in
lv_conf.h.

Set :c:macro:`LV_USE_NEMA_LIB` to the correct version for the core in
your MCU. If left as ``LV_NEMA_LIB_NONE``, M33 RevC will be assumed.

If using :c:macro:`LV_USE_NEMA_VG`,
set :c:macro:`LV_NEMA_GFX_MAX_RESX` and :c:macro:`LV_NEMA_GFX_MAX_RESY`
to the size of the display you will be using so that enough static
memory will be reserved for VG. Without VG, more task types will be
performed by the software renderer.

"libs/nema_gfx" contains pre-compiled binaries for the NeoChrom GPU drivers.

`lv_port_riverdi_stm32u5 <https://github.com/lvgl/lv_port_riverdi_stm32u5>`__
is already configured to link the "cortex_m33_revC" binaries when building.
`lv_port_stm32u5g9j-dk2 <https://github.com/lvgl/lv_port_stm32u5g9j-dk2>`__
is configured to link the "cortex_m33_NemaPVG" binaries when building.
`lv_port_stm32h7s78-dk <https://github.com/lvgl/lv_port_stm32h7s78-dk>`__
is configured to link the "cortex_m7" binaries when building.

"cortex_m33_revC" works on all STM32 Cortex-M33 devices with a NeoChrom core while "cortex_m33_NemaPVG"
additionally supports the tessellation and matrix multiplication acceleration that
the STM32 U5F and U5G NeoChrom cores are capable of.
"cortex_m7" should be used on MCUs with a Cortex-M7 core
and "cortex_m55" should be used on MCUs with a Cortex-M55 core.

With a different STM32CubeIDE project, you can configure the libraries to be linked
by right-clicking the project in the "Project Explorer" sidebar, clicking
"Properties", navigating to "C/C++ Build", "Settings", "MCU G++ Linker", and then
"Libraries". Add an entry under "Libraries (-l)" that is "nemagfx-float-abi-hard".
Add an entry under "Library search path (-L)" which is a path to
"libs/nema_gfx/lib/core/cortex_m33_revC/gcc" e.g.
"${workspace_loc:/${ProjName}/Middlewares/LVGL/lvgl/libs/nema_gfx/lib/core/cortex_m33_revC/gcc}".
Click "Apply and Close".

.. note::

    ``cannot find -lnemagfx-float-abi-hard: No such file or directory``

    The library version for ST was updated to ``revC`` so the path component
    ``cortex_m33`` must be updated to ``cortex_m33_revC`` in STM32CubeIDE.


32 and 16 bit :c:macro:`LV_COLOR_DEPTH` is supported.

NeoChrom requires a simple HAL implementation to allocate memory and optionally
lock resources. You may use a custom HAL implementation for your platform or use one of the
provided implementations by setting :c:macro:`LV_USE_NEMA_HAL` to a value other than
:c:macro:`LV_NEMA_HAL_CUSTOM`.

If your core has a data cache and it's enabled, which may be the case when using a Cortex-M7,
you may set :c:macro:`LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM` to a compiler-specific attribute
that will place the memory shared between the CPU and the GPU in a region of memory that is
not cached. This requires coordination from the linker script and memory protection unit
configuration. See `lv_port_stm32h7s78-dk <https://github.com/lvgl/lv_port_stm32h7s78-dk>`__
as a reference.


.. _neochrom vector graphics:

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

To use the SVG widget, additionally enable :c:macro:`LV_USE_SVG`.

If there is RAM available, SVG performance can be increased by enabling the image cache, :c:macro:`LV_CACHE_DEF_SIZE`.
:c:macro:`LV_CACHE_DEF_SIZE` is a cache size in bytes. If it is large enough for your SVGs,
it will cache decoded SVG data so it does not need to be parsed every refresh, significantly
reducing SVG redraw time.

:c:macro:`LV_USE_DEMO_VECTOR_GRAPHIC` is a demo you can enable which draws some vector graphics shapes.
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
