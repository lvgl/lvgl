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

The following code demonstrates using the diver in :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT` mode.

.. code-block:: c
    
    elcdif_rgb_mode_config_t config;
    ELCDIF_RgbModeGetDefaultConfig(&config);

    lv_display_t * g_disp = lv_nxp_display_elcdif_create_direct(LCDIF, config, buffer1, buffer2, buf_size);
    lv_display_set_default(g_disp);

To use the driver in :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL` mode, an extra buffer must be allocated,
preferably in the fastest available memory region.

Buffer swapping can be activated by passing a second buffer of same size instead of the :cpp:expr:`NULL` argument.

.. code-block:: c

    static lv_color_t partial_draw_buf[DISPLAY_HEIGHT * DISPLAY_WIDTH / 10];
    lv_display_t * g_disp = lv_nxp_display_elcdif_create_partial(LCDIF, config, partial_draw_buf, NULL, buf_size);
    lv_display_set_default(g_disp);

In runtime, the event handler function from the eLCDIF driver should be called inside of the eLCDIF interrupt handler
This function is responsible for notify the LVGL display subsystem about a finished flush operation:

.. code-block:: c

    void eLCDIF_IRQ_Handler(void)
    {
        lv_nxp_display_elcdif_event_handler(g_disp);
    }
