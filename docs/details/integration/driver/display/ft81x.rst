=====
FT81x
=====

A minimal framebuffer driver for EVE FT81x smart display controllers. Works for BT81x too.

Single-buffered partial render mode supported for now. It may not support DSPI or QSPI currently.

Tested at 32 MHz on ft812 and at 23 MHz on bt817.

Usage
*****

.. code-block:: c

    #define FB_SIZE             800 * 2 * 50
    #define MAX_TRANSFER_SIZE   FB_SIZE

    static void spi_cb(lv_display_t * disp, lv_ft81x_spi_operation operation, void * data, uint32_t length)
    {
        spi_device_handle_t spi = lv_ft81x_get_user_data(disp);
        switch(operation) {
            case LV_FT81X_SPI_OPERATION_CS_ASSERT:
                gpio_set_level(CS_PIN, 0);
                break;
            case LV_FT81X_SPI_OPERATION_CS_DEASSERT:
                gpio_set_level(CS_PIN, 1);
                esp_rom_delay_us(10); /* tiny delay in case a CS_ASSERT immediately follows */
                break;
            case LV_FT81X_SPI_OPERATION_SEND: {
                spi_transaction_t trans = {0};
                while(length) {
                    uint32_t sz = length < MAX_TRANSFER_SIZE ? length : MAX_TRANSFER_SIZE;
                    trans.length = sz * 8;
                    trans.rxlength = 0;
                    trans.tx_buffer = data;
                    spi_device_polling_transmit(spi, &trans);
                    length -= sz;
                    data += sz;
                }
                break;
            }
            case LV_FT81X_SPI_OPERATION_RECEIVE: {
                spi_transaction_t trans = {0};
                trans.length = length * 8;
                trans.rxlength = length * 8;
                trans.rx_buffer = data;
                spi_device_polling_transmit(spi, &trans);
                break;
            }
        }
    }

    int main() {
        /* ... */

        // reset the ft81x
        gpio_set_level(PD_PIN, 0);
        vTaskDelay(6 / portTICK_PERIOD_MS);
        gpio_set_level(PD_PIN, 1);
        vTaskDelay(21 / portTICK_PERIOD_MS);

        /* ... */

        // taken from https://github.com/lvgl/lvgl_esp32_drivers/blob/9fed1cc47b5a45fec6bae08b55d2147d3b50260c/lvgl_tft/EVE_config.h
        // NHD-5.0-800480FT-CxXx-xxx 800x480 5.0" Newhaven, resistive or capacitive, FT81x
        // EVE_NHD_50
        #define EVE_VSYNC0	(0L)
        #define EVE_VSYNC1	(3L)
        #define EVE_VOFFSET	(32L)
        #define EVE_VCYCLE	(525L)
        #define EVE_HSYNC0	(0L)
        #define EVE_HSYNC1	(48L)
        #define EVE_HOFFSET	(88L)
        #define EVE_HCYCLE 	(928L)
        #define EVE_PCLKPOL	(0L)
        #define EVE_SWIZZLE	(0L)
        #define EVE_PCLK	(2L)
        #define EVE_CSPREAD	(1L)

        lv_ft81x_parameters_t params = {
            .hor_res = 800,
            .ver_res = 480,

            .hcycle = EVE_HCYCLE,
            .hoffset = EVE_HOFFSET,
            .hsync0 = EVE_HSYNC0,
            .hsync1 = EVE_HSYNC1,
            .vcycle = EVE_VCYCLE,
            .voffset = EVE_VOFFSET,
            .vsync0 = EVE_VSYNC0,
            .vsync1 = EVE_VSYNC1,
            .swizzle = EVE_SWIZZLE,
            .pclkpol = EVE_PCLKPOL,
            .cspread = EVE_CSPREAD,
            .pclk = EVE_PCLK,

            .has_crystal = true,
            .is_bt81x = false
        };

        static uint8_t fb[FB_SIZE] __attribute__((aligned(4)));
        lv_display_t * disp = lv_ft81x_create(&params, fb, FB_SIZE, spi_cb, spi);

        /* ... */
    }


Troubleshooting
***************

If the backlight does not come on (or is too bright or dim),
try changing the value of ``PWM_DUTY_BACKLIGHT_ON``
in ``lv_ft81x.c``, which can vary by board.
