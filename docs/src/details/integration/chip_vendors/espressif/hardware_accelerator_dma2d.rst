.. _esp_dma2d:

=======================================
2D Direct Memory Access (DMA2D) Support
=======================================

Overview
********

Some chips from Espressif such as the ESP32-P4 family features a peripheral
that enhances the copy of 2-D data from a place to another, including output
this 2-D data to other peripherals.

This peripheral is the 2-D direct memory access, or DMA2D, the Espressif 
SDK, the IDF, offers a full featured driver for the DMA2D that is automatically
enabled on the supported chips.

One of its primary role is to serve as support for the Pixel Processor Accelerator
the PPA, being used to copy the source image data to the desired PPA client splitting
these data into fixed size blocks, called bursts. Also the DMA2D is used to pick the
output chunks from the PPA client and copy over to the destination buffer, or the display
frame-buffer.

More information about PPA can be found in: :ref:`PPA (Pixel Processing Accelerator) Support <esp_ppa>`.

Interacting to the DMA2D
------------------------

The LVGL does not interact directly to the DMA2D, instead, the Espressif LVGL port component
display driver uses the DMA2D to copy the target drawn buffer to the display buffer without CPU
intervention. Even though this option is available the user is responsible to explicitly enable
it on the display driver of the LVGL port component.

To enabling it the user should set on its `sdkconfig.defaults` the `CONFIG_BSP_DISPLAY_LVGL_AVOID_TEAR`, 
which will tell the driver to use the DMA2D to optimize transfer. Please notice that enabling
this option will be only available when using the PSRAM memory and double buffer mode, otherwise
a compiler error will be raised.

If the user is configuring the display manually, it is possible to enable the DMA2D in runtime by
setting the DMA2D flag to true:

.. code-block:: c

        /* Gets the default configuration for the MIPI display controller */
        esp_lcd_dpi_panel_config_t dpi_config = EK79007_1024_600_PANEL_60HZ_CONFIG(LCD_COLOR_PIXEL_FORMAT_RGB888);

        /* Explictly set the DMA2D to assist the buffer transfer */
        dpi_config.flags.use_dma2d = true;

        ek79007_vendor_config_t vendor_config = {
            .mipi_config = {
                .dsi_bus = mipi_dsi_bus,
                .dpi_config = &dpi_config,
            },
        };

        esp_lcd_panel_dev_config_t lcd_dev_config = {
            .reset_gpio_num = UI_FIRMWARE_PIN_NUM_LCD_RST,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
            .bits_per_pixel = 24,
            .vendor_config = &vendor_config,
        };

        /* Configures and enable the display */
        ESP_ERROR_CHECK(esp_lcd_new_panel_ek79007(mipi_dbi_io, &lcd_dev_config, &mipi_dpi_panel));

