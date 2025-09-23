.. _esp_ppa:

==========================================
PPA (Pixel Processing Accelerator) Support
==========================================

Overview
********

Some ESP32 chip series, like the ESP32-P4 support the Pixel Processing Accelerator hardware (PPA), which is capable of
speeding-up the filling and image blending operations, this peripheral works with the
DMA-2D hardware which is responsible to move the input/output buffers into/from the PPA processing engine.

Supported devices
-----------------

The Espressif targets that support the PPA are:

- ESP32-P4 series.


Using the LVGL PPA draw unit on your ESP-IDF project
****************************************************

LVGL supports, in experimental level, the filling and the image blending
acceleration through the PPA, the user can enable it in their ``sdkconfig.defaults`` by
adding the following option to enable the PPA draw unit in conjunction with the software renderer:

   .. code:: c

      CONFIG_LV_USE_PPA=y

Save the file and then rebuild the project, this will be sufficient to add the PPA code and it will start to run automatically, so
no further steps are required from the user code perspective.

Benchmarking
------------

When running the `lv_demo_benchmark` from LVGL the user can compare the performance when 
the PPA is enabled versus the pure software rendering, by using the latest version of the 
LVGL and LVGL port component it can be observed an average saving of 30% of the rendering
time for draw tasks that are image and rectangle fill. On some cases for pure filling on 
integer multiples of the display size it is possible to observe up 9x of speed increase when
the PPA is enabled.

Limitations
-----------

Please notice that the PPA is at experimental level where some performance gains are expected on drawing tasks related
to rectangle copy or filling, while for image blending, even though it is operational, there are no significant gains,
the initial cause for that according to the PPA section from reference manual is due to the DMA-2D memory bandwidth.


Using the Espressif LVGL component PPA features
***********************************************

The Espressif IDF LVGL port component also offers hardware acceleration
for the display operations, that is it, once the LVGL render completes 
an operation and the draw buffer is already handled to the display driver,
the Espressif component can optionally perform mirror and rotation of 
the rendered data using hardware assistance.

The LVGL display driver for Espressif operates by reading the
descriptor of the drawn data to check if rotation and mirror are flags
set on the data properties, in positive case, before sending the data
to the display, the mirror and rotation are performed first using the 
PPA rotation and mirror client.

When compared to pure software rotation, using the PPA to rotate the objects
can save up 40% of rendering time in average, while keeping the CPU in idle 
saving power, or using it to perform other tasks, PPA rotation is asynchronous,
and once started it will signal the application using its dedicated interrupt.

To enable such behavior on the display driver, the PPA acceleration should
be set on the project `sdkconfig.defaults` via the symbol `CONFIG_LVGL_PORT_ENABLE_PPA`,
setting it to true will make the hardware assistance for image rotation and mirroring 
automatic. No extra code is required.

Please notice that such option can be set via menuconfig using the 
standard IDF command for configuration: `idf.py menuconfig`.

API
***

.. API startswith:  lv_draw_ppa_
