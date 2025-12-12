.. include:: /include/substitutions.txt
.. _stm32 ltdc driver:

=========================
STM32 LTDC Display Driver
=========================

Some STM32s have a specialized peripheral called
LTDC (LCD-TFT Display Controller) for driving displays.


Usage Modes With LVGL
*********************

The driver within LVGL is designed to work with an
already-configured LTDC peripheral. It relies on the
STM32 HAL to detect information about the configuration.
The color format of the created LVGL display will
match the LTDC layer's color format. Use STM32CubeIDE
or STM32CubeMX to generate LTDC initialization code.
It is an STM32 HAL-based driver and relies on being
able to include ``"main.h"`` from within LVGL to get
access to STM32 HAL definitions and function prototypes.

There are some different use cases for LVGL's driver.
All permutations of the below options are well supported.

- single or double buffered
- direct or partial render mode
- OS or no OS
- parallelized flushing with DMA2D (only for partial render mode)

If OS is enabled, a synchronization primitive will be used to
give the thread a chance to yield to other threads while blocked,
improving CPU utilization. See :c:macro:`LV_USE_OS` in your lv_conf.h


LTDC Layers
***********

This driver creates an LVGL display
which is only concerned with a specific layer of the LTDC peripheral, meaning
two LVGL LTDC displays can be created and operate independently on the separate
layers.


Direct Render Mode
******************

For direct render mode, invoke :cpp:func:`lv_st_ltdc_create_direct` like this:

.. code-block:: c

    void * my_ltdc_framebuffer_address = (void *)0x20000000u;
    uint32_t my_ltdc_layer_index = 0; /* typically 0 or 1 */
    lv_display_t * disp = lv_st_ltdc_create_direct(my_ltdc_framebuffer_address,
                                                   optional_other_full_size_buffer,
                                                   my_ltdc_layer_index);

``my_ltdc_framebuffer_address`` is the framebuffer configured for use by
LTDC. ``optional_other_full_size_buffer`` can be another buffer which is the same
size as the default framebuffer for double-buffered
mode, or ``NULL`` otherwise. ``my_ltdc_layer_index`` is the layer index of the
LTDC layer to create the display for.

For the best visial results, ``optional_other_full_size_buffer`` should be used
if enough memory is available. Single-buffered mode is what you should use
if memory is very scarce. Chips with a CPU data cache have unavoidable visual
artifacts when using single-buffered direct mode. If there is almost enough
memory for double-buffered direct mode, but not quite, then use partial render mode.

To clarify what ``my_ltdc_framebuffer_address`` exactly is, it's the value of
``pLayerCfg.FBStartAdress`` when the LTDC layer is configured using the STM32 HAL,
which is written to the ``CFBAR`` register of the LTDC layer peripheral.

.. image:: /_static/images/stm32cubemx-ltdc-fbaddr.png


Partial Render Mode
*******************

For partial render mode, invoke :cpp:func:`lv_st_ltdc_create_partial` like this:

.. code-block:: c

    static uint8_t partial_buf1[65536];
    static uint8_t optional_partial_buf2[65536];
    uint32_t my_ltdc_layer_index = 0; /* typically 0 or 1 */
    lv_display_t * disp = lv_st_ltdc_create_partial(partial_buf1,
                                                    optional_partial_buf2,
                                                    65536,
                                                    my_ltdc_layer_index);

The driver will use the information in the LTDC layer configuration to find the
layer's framebuffer and flush to it.

Providing a second partial buffer can improve CPU utilization and increase
performance compared to
a single buffer if :c:macro:`LV_ST_LTDC_USE_DMA2D_FLUSH` is enabled.


Linker Script
*************

You should ensure the LTDC framebuffer memory is actually reserved in the linker script.
This is a file that normally has the extension ``.ld``.
In the below example, ``1125K`` is specified because the color depth is 24 (3 bytes per pixel),
the display width is 800, the display height is 480, and ``1K`` means 1024 bytes.
3 |times| 800 |times| 480 |divide| 1024 = 1125. You should ensure the sum of the RAM entries
(``FB_RAM`` + ``RAM``) equals the total RAM of the device.

.. code-block::

    /* Memories definition */
    MEMORY
    {
        FB_RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 1125K /* single 24bit 800x480 buffer */
        RAM    (xrw)   : ORIGIN = 0x20119400, LENGTH = 1883K
        FLASH  (rx)    : ORIGIN = 0x08000000, LENGTH = 4096K
    }

If the framebuffer is not reserved in the linker script or the framebuffer is not set during
board initialization, you may absolutely set it at runtime before creating the LVGL display.

.. code-block:: c

    /* like this */
    HAL_LTDC_SetAddress(&hltdc, (uint32_t)my_ltdc_framebuffer_address, my_ltdc_layer_index);

    /* and then... */

    lv_display_t * disp;
    disp = lv_st_ltdc_create_direct(my_ltdc_framebuffer_address,
                                    optional_other_full_size_buffer,
                                    my_ltdc_layer_index);
    /* or */
    disp = lv_st_ltdc_create_partial(partial_buf1,
                                     optional_partial_buf2,
                                     partial_buf_size,
                                     my_ltdc_layer_index);


Display Rotation
****************

The driver supports display rotation with
:cpp:expr:`lv_display_set_rotation(disp, rotation)` where rotation is one of
:cpp:enumerator:`LV_DISP_ROTATION_90`, :cpp:enumerator:`LV_DISP_ROTATION_180`,
or :cpp:enumerator:`LV_DISP_ROTATION_270`. The rotation is initially
:cpp:enumerator:`LV_DISP_ROTATION_0`.

The rotation is done in software and only works if the display was
created using :cpp:func:`lv_st_ltdc_create_partial`.
:c:macro:`LV_ST_LTDC_USE_DMA2D_FLUSH` will be have no effect if rotation
is used.


Interop with the DMA2D and NeoChrom Draw Units
**********************************************

:c:macro:`LV_ST_LTDC_USE_DMA2D_FLUSH` can be enabled to use DMA2D to flush
partial buffers in parallel with other LVGL tasks, whether or not OS is
enabled. If the display is not partial, then there is no need to enable this
option.

It must not be enabled at the same time as :c:macro:`LV_USE_DRAW_DMA2D`.
See the :ref:`DMA2D support <dma2d>`.

NeoChrom can be enabled at the same time as LTDC. They will not interfere
with each other at all.
