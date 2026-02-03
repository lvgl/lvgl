.. _faq:


===
FAQ
===

Where can I ask questions?
**************************

You can ask questions in the forum:  https://forum.lvgl.io/.

We use `GitHub issues <https://github.com/lvgl/lvgl/issues>`_ for development related discussion.
You should use them only if your question or issue is tightly related to the development of the library.

Before posting a question, please read this FAQ section since you might find the answer to your issue here as well.



Is my MCU/hardware supported?
*****************************

Every MCU which is capable of driving a display via parallel port, SPI, RGB interface or anything else and fulfills the :ref:`requirements` is supported by LVGL.

This includes:

* "Common" MCUs like STM32F, STM32H, NXP Kinetis, LPC, iMX, dsPIC33, PIC32, SWM341 etc.
* Bluetooth, GSM, Wi-Fi modules like Nordic NRF, Espressif ESP32 and Raspberry Pi Pico W
* Linux with frame buffer device such as /dev/fb0. This includes Single-board computers like the Raspberry Pi
* Anything else with a strong enough MCU and a peripheral to drive a display



Is my display supported?
************************

LVGL needs just one simple driver function to copy an array of pixels into a given area of the display.
If you can do this with your display then you can use it with LVGL.

Some examples of the supported display types:

* TFTs with 16 or 24 bit color depth
* Monitors with an HDMI port
* Small monochrome displays
* Gray-scale displays
* even LED matrices
* or any other display where you can control the color/state of the pixels

See the :ref:`display` section to learn more.



LVGL doesn't start, randomly crashes or nothing is drawn on the display. What might be the problem?
***************************************************************************************************

* Try increasing :c:macro:`LV_MEM_SIZE`.
* Be sure your display works without LVGL. E.g. paint it to red on start up.
* Enable :ref:`logging`.
* Enable assertions in ``lv_conf.h`` (``LV_USE_ASSERT_...``).
* If you use an RTOS:

  * Increase the stack size of the task that calls :cpp:func:`lv_timer_handler`.
  * Be sure you are using one of the methods for thread management as described in :ref:`threading`.



My display driver is not called. What have I missed?
****************************************************

Be sure you are calling :cpp:expr:`lv_tick_inc(x)` as prescribed in
:ref:`tick_interface` and are calling :cpp:func:`lv_timer_handler` as prescribed in
:ref:`timer_handler`.

Learn more in the :ref:`tick_interface` and :ref:`timer_handler` sections.



Why is the display driver called only once? Only the upper part of the display is refreshed.
********************************************************************************************

Be sure you are calling :cpp:expr:`lv_display_flush_ready(drv)` at the end of your
"*display flush callback*" as per the :ref:`flush_callback` section.



Why do I see only garbage on the screen?
****************************************

There is probably a bug in your display driver. Try the following code without using
LVGL.  You should see a square with red-blue gradient.

.. code-block:: c

    #define BUF_WIDTH 255
    uint16_t buf[BUF_WIDTH];
    uint32_t i;
    for(i = 0; i < BUF_WIDTH; i++) {
      lv_color_t c = lv_color_mix(lv_color_hex(0xff0000), lv_color_hex(0x00ff00), i);
      buf[i] = lv_color_to_u16(c);
    
      lv_area_t a;
      a.x1 = 5;
      a.x2 = a.x1 + BUF_WIDTH - 1;
      a.y1 = 10 + i;
      a.y2 = 10 + i;
      my_flush_cb(NULL, &a, (void*) buf);
    }



Why do I see nonsense colors on the screen?
*******************************************

The configured LVGL color format is probably not compatible with your display's color
format.  Check :c:macro:`LV_COLOR_DEPTH` in *lv_conf.h*.



How do I speed up my UI?
************************

- Turn on compiler optimization and enable instruction- and data-caching if your MCU has them.
- Increase the size of the display buffer.
- Use two display buffers and flush the buffer with DMA (or similar peripheral) in the background.
- Increase the clock speed of the SPI or parallel port if you use them to drive the display.
- If your display has an SPI port consider changing to a model with a parallel interface because it has much higher throughput.
- Keep the display buffer in internal RAM (not in external SRAM) because LVGL uses it a lot and it should have fast access time.
- Consider minimizing LVGL CPU overhead by updating Widgets:

  - only once just before each display refresh, and
  - only when it will change what the end user sees.



How do I reduce flash/ROM usage?
********************************

You can disable unused features (such as animations, file system, GPU etc.) and widget types in *lv_conf.h*.

If you are using GCC/CLANG you can add `-fdata-sections -ffunction-sections` compiler flags and `--gc-sections` linker flag to remove unused functions and variables from the final binary. If possible, add the `-flto` compiler flag to enable link-time-optimisation together with `-Os` for GCC or `-Oz` for CLANG and newer GCC versions.



How do I reduce RAM usage?
**************************

* Lower the size of the *Display buffer*.
* Reduce :c:macro:`LV_MEM_SIZE` in *lv_conf.h*. This memory is used when you create Widgets like buttons, labels, etc.
* To work with lower :c:macro:`LV_MEM_SIZE` you can create Widgets only when required and delete them when they are no longer needed.



How do I use LVGL with an operating system?
*******************************************

To work with an operating system where tasks can interrupt each other (preemptively),
you must ensure that no LVGL function call be called while another LVGL call is in
progress.  There are several ways to do this.  See the :ref:`threading` section to
learn more.

