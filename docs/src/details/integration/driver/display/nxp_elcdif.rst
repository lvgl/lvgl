.. _nxp_elcdif:

=============
NXP eLCDIF
=============

Overview
--------
eLCDIF is a peripheral that is provided on some of the NXP devices capable to drive display panels through
the RGB interface, it supports different color depths and, on MIPI-DSI capable devices, its output ca be
directed to the MIPI display physical interface. The LVGL's NXP eLCDIF driver is responsible to bind the
NXP MCUx SDK low-level driver to the LVGL display subsystem. 


Prerequisites
-------------

- This driver relies on the presence of the MCUx SDK from NXP in the same project
- Activate the diver by setting :c:macro:`LV_USE_NXP_ELCDIF` to ``1`` in your *"lv_conf.h"*.

Usage
-----

The LVGL driver for eLCDIF assumes the platform already configured the display low-level driver, 
set the pin-mux, clocks, etc. It also requires the base address of the peripheral and configuration
structure already set.

The following code demonstrates using the diver in :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT` mode, 
please notice in this mode of operation the application is responsible to allocate the framebuffers space
and pass them to the display, in the example below `buffer1` and `buffer2` are the current and the next
buffers that will be copied to the display screen, being swapped at each flush operation (managed 
internally by the display driver). Also observe, in direct mode, each buffer should have the space at
least to hold at least the size of the screen, that is it, the heigh times the width times the bytes
for a pixel (which is application dependent or display supported), on the code below this size is represented
by `buf_size`.

.. code-block:: c
    
    elcdif_rgb_mode_config_t config;
    ELCDIF_RgbModeGetDefaultConfig(&config);

    lv_display_t * g_disp = lv_nxp_display_elcdif_create_direct(LCDIF, config, buffer1, buffer2, buf_size);
    lv_display_set_default(g_disp);

To use the driver in :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL` mode, an extra buffer must be allocated,
preferably in the fastest available memory region.

Buffer swapping can be activated by passing a second buffer of same size instead of the :cpp:expr:`NULL` argument.
please notice in this case the `BUF_SIZE` needs to have, at least, space to hold data of 1/10 of the actual
display dimensions.

.. code-block:: c

    #define BUF_SIZE (DISPLAY_HEIGHT * DISPLAY_WIDTH / 10 * 2) /*1/10 screen size for RGB565 format*/
    static uint8_t partial_draw_buf[BUF_SIZE];
    lv_display_t * g_disp = lv_nxp_display_elcdif_create_partial(LCDIF, config, partial_draw_buf, NULL, BUF_SIZE);

In runtime, the event handler function from the eLCDIF driver should be called inside of the eLCDIF interrupt handler
This function is responsible for notify the LVGL display subsystem about a finished flush operation:

.. code-block:: c

    void eLCDIF_IRQ_Handler(void)
    {
        lv_nxp_display_elcdif_event_handler(g_disp);
    }
