=========================
EVE External GPU Renderer
=========================

EVE is a type of external GPU IC which accepts high-level drawing commands over SPI
and outputs the rendered graphics to a display over parallel RGB.

The advantage of using an EVE chip that the rendering responsibility is removed
from the driving MCU so it can be a lower-spec part or dedicate more of its
processing time to other tasks. The SPI interface is simpler to connect than
parallel RGB especially if the EVE chip is integrated into a display assembly.

LVGL features a renderer for EVE. LVGL UIs can be rendered by EVE
and are effectively indistinguishable from the software renderer is most cases.

See also the :ref:`ft81x` framebuffer driver. It drives the same EVE chips
but is a simpler, more standalone implementation which uses software rendering
and sends all the pixels over SPI so it is much slower.

Limitations
***********

- Image format, size, and count limit.
- Font format, size, and count limit.
- The total number of tasks rendered per refresh has an upper limit.
- Layers are not supported.

Usage
*****

Board Parameters
----------------

Create a ``lv_draw_eve_target_parameters.h`` and add it to the include search
path of your build.

Here is an example ``lv_draw_eve_target_parameters.h`` for the Riverdi RVT50HQBNWC00-B
which has a BT817Q --- the EVE chip.
These parameters were taken from the collection of configs provided by the EVE
library supporting this renderer.
`See here <https://github.com/RudolphRiedel/FT800-FT813/blob/ee35fa45c37aed78dc4f1196b874ee1c8f80429f/EVE_config.h#L856-L872>`__.
Check there for your board parameters.

.. code-block:: c

    #pragma once

    #define EVE_HSIZE (800L)
    #define EVE_VSIZE (480L)
    #define EVE_VSYNC0 (0L)
    #define EVE_VSYNC1 (4L)
    #define EVE_VOFFSET (8L)
    #define EVE_VCYCLE (496L)
    #define EVE_HSYNC0 (0L)
    #define EVE_HSYNC1 (4L)
    #define EVE_HOFFSET (8L)
    #define EVE_HCYCLE (816L)
    #define EVE_PCLK (3L)
    #define EVE_PCLKPOL (1L)
    #define EVE_SWIZZLE (0L)
    #define EVE_CSPREAD (0L)
    #define EVE_HAS_CRYSTAL
    /* notice EVE_HAS_GT911 is not defined */
    #define EVE_GEN 4

Add it to the include search path of your build by adding the compiler
flag ``-I /path/to/dir/that/has/the/file/``. In CMake, see
``include_directories`` and ``target_include_directories``. In many cases,
placing the file in the same directory as your ``lv_conf.h`` will work.

User-Provided SPI HAL Implementation
------------------------------------

The user is required to implement four functions which the LVGL EVE
renderer will call to perform SPI communication with the EVE chip.
The functions can simply be defined in your own files. They must not be
``static``.

.. code-block:: c

    #include "lvgl/lvgl.h"

    void lv_draw_eve_target_powerdown(bool powerdown)
    {
        ...
    }

    void lv_draw_eve_target_spi_cs(bool selected)
    {
        ...
    }

    void lv_draw_eve_target_spi_transmit(uint8_t * src, uint32_t len)
    {
        ...
    }

    void lv_draw_eve_target_spi_receive(uint8_t * dst, uint32_t len)
    {
        ...
    }

The ``powerdown`` parameter of ``lv_draw_eve_target_powerdown`` when ``true``
means that the ``PD_N`` pin should be set low (power off)
and ``false`` means that it should be set high (power on).

The ``selected`` parameter of ``lv_draw_eve_target_spi_cs`` when ``true``
means that the ``CS_N`` pin should be set low (chip is selected)
and ``false`` means that it should be set high (chip is deselected).

The ``src`` parameter of ``lv_draw_eve_target_spi_transmit`` is not a
pointer to ``const`` since your implementation may naturally want to
overwrite the outgoing data with (meaningless) incoming data
(which will be ignored by LVGL).

In practice, the ``len`` parameters will be only 1 or 4 for transmit, and
always 1 for receive. A forward-compatible implementation should account
for other values, though.

Here is an example implementation for ESP32-S3.

.. code-block:: c

    #include "lvgl/lvgl.h"
    #include "driver/spi_master.h"
    #include "driver/gpio.h"

    #define EVE_CS   GPIO_NUM_5
    #define EVE_PDN  GPIO_NUM_4
    #define EVE_SCK  GPIO_NUM_12
    #define EVE_MISO GPIO_NUM_13
    #define EVE_MOSI GPIO_NUM_11

    static spi_device_handle_t spi_device = {0};

    void lv_draw_eve_target_powerdown(bool powerdown)
    {
        gpio_set_level(EVE_PDN, !powerdown);
    }

    void lv_draw_eve_target_spi_cs(bool selected)
    {
        if(selected) {
            spi_device_acquire_bus(spi_device, portMAX_DELAY);
            gpio_set_level(EVE_CS, 0U);
        }
        else {
            gpio_set_level(EVE_CS, 1U);
            spi_device_release_bus(spi_device);
        }
    }

    void lv_draw_eve_target_spi_transmit(uint8_t * src, uint32_t len)
    {
        spi_transaction_t trans = {0};
        trans.length = 8U * len;
        trans.tx_buffer = src;
        spi_device_polling_transmit(spi_device, &trans);
    }

    void lv_draw_eve_target_spi_receive(uint8_t * dst, uint32_t len)
    {
        spi_transaction_t trans = {0};
        trans.length = trans.rxlength = 8U * len;
        trans.rx_buffer = dst;
        spi_device_polling_transmit(spi_device, &trans);
    }

You will also need to initialize your SPI peripheral and GPIO pins.
Here is the example code for that, which corresponds to the above ESP32-S3
implementation. This ``init_spi`` can happen after :cpp:func:`lv_init`
but **must** happen before :cpp:func:`lv_draw_eve_display_create`,
which is discussed in the next section.

22 MHz was the highest SPI speed that worked during testing with the Riverdi board
and the ESP32-S3. You may not have success with this speed so it is
recommended to validate with an ``SPI_SPEED`` value of ``10`` (10 MHz)
and increase experimentally in your testing.

.. code-block:: c

    /* Start with 10 (10 MHz) to validate, and then experiment with higher values. */
    #define SPI_SPEED (/* set me to 10 */)

    static void init_spi(void)
    {
        spi_bus_config_t buscfg = {0};
        spi_device_interface_config_t devcfg = {0};
        gpio_config_t io_cfg = {0};

    #if ESP_IDF_VERSION_MAJOR <= 4
        io_cfg.intr_type = GPIO_PIN_INTR_DISABLE;
    #elif ESP_IDF_VERSION_MAJOR == 5
        io_cfg.intr_type = GPIO_INTR_DISABLE;
    #endif
        io_cfg.mode = GPIO_MODE_OUTPUT;
        io_cfg.pin_bit_mask = BIT64(EVE_PDN) | BIT64(EVE_CS);
        gpio_config(&io_cfg);

        gpio_set_level(EVE_CS, 1);
        gpio_set_level(EVE_PDN, 0);

        buscfg.mosi_io_num = EVE_MOSI;
        buscfg.miso_io_num = EVE_MISO;
        buscfg.sclk_io_num = EVE_SCK;
        buscfg.quadwp_io_num = -1;
        buscfg.quadhd_io_num = -1;
        buscfg.max_transfer_sz= 4088;

        devcfg.clock_speed_hz = SPI_SPEED * 1000U * 1000U; /* SPI_SPEED 10 means 10 MHz */
        devcfg.mode = 0;                          /* SPI mode 0 */
        devcfg.spics_io_num = -1;                 /* CS pin operated by app */
        devcfg.queue_size = 3;                    /* we need only one transaction in the que */
        devcfg.command_bits = 0;                  /* command operated by app */
        devcfg.address_bits = 0;
        devcfg.post_cb = 0;

        spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
        spi_bus_add_device(SPI2_HOST, &devcfg, &spi_device);
    }

LVGL EVE Display Creation
-------------------------

To create the LVGL display for the EVE renderer, you call
:cpp:expr:`lv_draw_eve_display_create()` which returns the
created display. It should be called after
SPI is initialized and your ``lv_draw_eve_target_...`` implementation
functions are ready to be called.

No buffers are required for the LVGL EVE renderer because no pixels
are written to any buffers in the device running LVGL. When something
needs to be drawn, a series of commands are sent to EVE.
