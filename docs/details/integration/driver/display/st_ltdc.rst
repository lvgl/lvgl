.. _stm32 ltdc driver:

=================
STM32 LTDC Driver
=================

Some STM32s have a specialized peripheral for driving
displays called LTDC (LCD-TFT display controller).

Usage Modes With LVGL
*********************

The driver within LVGL is designed to work with an
already-configured LTDC peripheral. It relies on the
HAL to detect information about the configuration.
The color format of the created LVGL display will
match the LTDC layer's color format. Use STM32CubeIDE
or STM32CubeMX to generate LTDC initialization code.

There are some different use cases for LVGL's driver.
All permutations of the below options are well supported.

- single or double buffered
- direct or partial render mode
- OS and no OS
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
if memory is very scarce. If there is almost enough memory for double-buffered
direct mode, but not quite, then use partial render mode.

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

DMA2D
*****

:c:macro:`LV_ST_LTDC_USE_DMA2D_FLUSH` can be enabled to use DMA2D to flush
partial buffers in parallel with other LVGL tasks, whether or not OS is
enabled. If the display is not partial, then there is no need to enable this
option.

Additionally it is possible to mix layers that have color format on
:c:macro:`LV_COLOR_FORMAT_ARGB1555` on top of :c:macro:`LV_COLOR_FORMAT_RGB565`
layers using the DMA2D.

It must not be enabled at the same time as :c:macro:`LV_USE_DRAW_DMA2D`.
See the :ref:`DMA2D support <dma2d>`.


.. admonition::  Further Reading

    You may be interested in enabling the :ref:`Nema GFX renderer <nema_gfx>`
    if your STM32 has a NeoChrom GPU.

    `lv_port_riverdi_stm32u5 <https://github.com/lvgl/lv_port_riverdi_stm32u5>`__
    is a way to quick way to get started with LTDC on LVGL.
