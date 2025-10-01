=========================
EVE External GPU Renderer
=========================

EVE is a type of external GPU IC which accepts high-level drawing commands over SPI
and outputs the rendered graphics to a display over parallel RGB.

The advantage of using an EVE chip is that the rendering responsibility is removed
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
- Only 4 bpp fonts are supported for now.
- The total number of tasks rendered per refresh has an upper limit.
- Layers are not supported.


Usage
*****

Board Parameters
----------------

Find your display parameters and populate a :cpp:struct:`lv_draw_eve_parameters_t`
struct with them.

Here is an example ``lv_draw_eve_target_parameters.h`` for the Riverdi RVT50HQBNWC00-B
which has a BT817Q --- the EVE chip.
These parameters were taken from the collection of configs provided by the EVE
library supporting this renderer.
`See here <https://github.com/RudolphRiedel/FT800-FT813/blob/ee35fa45c37aed78dc4f1196b874ee1c8f80429f/EVE_config.h#L856-L872>`__.
Check there for your board parameters.

.. code-block:: c

    lv_draw_eve_parameters_t params = {
        .hor_res = 800,
        .ver_res = 480,
        .vsync0 = 0,
        .vsync1 = 4,
        .voffset = 8,
        .vcycle = 496,
        .hsync0 = 0,
        .hsync1 = 4,
        .hoffset = 8,
        .hcycle = 816,
        .pclk = 3,
        .pclkpol = 1,
        .swizzle = 0,
        .cspread = 0,
        .has_crystal = true,
        .has_gt911 = false,
        .backlight_freq = 4000,
        .backlight_pwm = 128,
    };


EVE Chip IO Implementation
--------------------------

The user is required to implement the GPIO and SPI IO functionality. The LVGL EVE
renderer will call this callback to perform SPI communication with the EVE chip.

.. code-block:: c

    static void op_cb(lv_display_t * disp, lv_draw_eve_operation_t operation,
                      void * data, uint32_t length)
    {
        /* optional: get the `user_data` parameter you passed to `lv_draw_eve_display_create` */
        void * your_user_data = lv_draw_eve_display_get_user_data(disp);

        switch(operation) {
            case LV_DRAW_EVE_OPERATION_POWERDOWN_SET:
                /* setting the pin low powers down the EVE chip */
                your_gpio_write(PD_N_PIN, 0);
                break;
            case LV_DRAW_EVE_OPERATION_POWERDOWN_CLEAR:
                /* setting the pin high powers on the EVE chip */
                your_gpio_write(PD_N_PIN, 1);
                break;
            case LV_DRAW_EVE_OPERATION_CS_ASSERT:
                /* setting the pin low asserts the EVE chip SPI device */
                your_gpio_write(CS_N_PIN, 0);
                break;
            case LV_DRAW_EVE_OPERATION_CS_DEASSERT:
                /* setting the pin high de-asserts the EVE chip SPI device */
                your_gpio_write(CS_N_PIN, 1);
                break;
            case LV_DRAW_EVE_OPERATION_SPI_SEND:
                /* `data` is the data to send */
                your_spi_transmit(data, length);
                break;
            case LV_DRAW_EVE_OPERATION_SPI_RECEIVE:
                /* `data` is the destination for the data */
                your_spi_receive(data, length);
                break;
        }
    }

You will also need to initialize your SPI peripheral and GPIO pins.

22 MHz was the highest SPI speed that worked during testing with the Riverdi board
and the ESP32-S3. You may not have success with this speed so it is
recommended to validate with an ``SPI_SPEED`` value of ``10`` (10 MHz)
and increase experimentally in your testing.


LVGL EVE Display Creation
-------------------------

To create the LVGL display for the EVE renderer, you call
:cpp:expr:`lv_draw_eve_display_create(params, op_cb, your_user_data)` which returns the
created display. ``your_user_data`` can be ``NULL``. It should be called after GPIO and
SPI is initialized. You may choose to initialize your IO
the first time ``op_cb`` is called.

No buffers are required for the LVGL EVE renderer because no pixels
are written to any buffers in the device running LVGL. When something
needs to be drawn, a series of commands are sent to EVE.


Touch Indev Creation
--------------------

:cpp:expr:`lv_draw_eve_touch_create(disp)` creates a touch :ref:`indev` for the display.

You may need to configure the i2c address of the touch controller connected to EVE.
See the section :ref:`eve register access` for more info about register access.

Here is an example of setting the ``REG_TOUCH_CONFIG`` register on a BT817q EVE chip
for a capacitive touch screen with a controller that has the i2c address ``0x15``.

.. code-block:: c

    /*
    15:   0: capacitive, 1: resistive         CAPACITIVE
    14:   host mode                           NO
    13:   reserved
    12:   ignore short circuit protection     NO
    11:   low-power mode                      NO
    10-4: 7-bit i2c address                   0x15
     3:   reserved
     2:   suppress 300ms startup              NO
     1-0: 2-bit sampling clocks val           use 1 (the reset default)
    */
    lv_draw_eve_memwrite16(disp, LV_EVE_REG_TOUCH_CONFIG, 0x0151);


Display Rotation
----------------

Efficient display rotation is fully supported through :cpp:func:`lv_display_set_rotation`.
Touch input rotation is handled accordingly.


Asset Pre-Upload
----------------

Images and fonts are uploaded to the EVE chip's RAM_G as-needed during rendering.
An image or glyph will not be uploaded until it needs to be rendered. Once it is
uploaded, it will stay in RAM_G until reset. The first time a screen is shown,
there may be a noticeable delay while an image or set of glyphs is uploaded
for use for the first time. To mitigate this delay, there is a set of functions
provided to upload the assets early.

:cpp:func:`lv_draw_eve_pre_upload_image` is used to upload images.
:cpp:func:`lv_draw_eve_pre_upload_font_range` is used to upload a range of characters.
The range is provided as the first and last unicode code points in the range. Both
the start and the end values are included in the range. It can be called multiple
times with different ranges.
:cpp:func:`lv_draw_eve_pre_upload_font_text` is used to upload all the glyphs that
are needed to render a specific string (ASCII or UTF-8).
It can be called multiple times with different strings.

.. code-block:: c

    LV_IMAGE_DECLARE(asset_digital_lock);
    lv_draw_eve_pre_upload_image(disp, &asset_digital_lock);

    LV_FONT_DECLARE(lv_font_montserrat_48);
    lv_draw_eve_pre_upload_font_text(disp, &lv_font_montserrat_48, "The current time is");
    lv_draw_eve_pre_upload_font_range(disp, &lv_font_montserrat_48, '0', '9');
    lv_draw_eve_pre_upload_font_range(disp, &lv_font_montserrat_48, ':', ':');


Supported Asset Formats
-----------------------

The supported image color formats are RGB565, RGB565A8, ARGB8888, and L8. It's good to
note that RGB565A8 and ARGB8888 are converted to ARGB4444 before being uploaded to EVE. The
implication of this is that a reduced color depth will be realized on the display compared
to the original asset. The initial asset upload time may also be longer.

The only supported font format is 4 bpp. If the font's stride is not 1, it will be converted
to stride 1 before being uploaded to EVE. If the font already has a stride of 1, it will be
uploaded directly without conversion, which can improve asset upload time.

To generate a font with a specific stride (such as 1), you should use the
`offline font converter <https://github.com/lvgl/lv_font_conv>`__ and specify a stride
argument on the command line, e.g. ``--stride 1``. A stride of 0 is the default. This means
that the bits are packed even across rows, but EVE cannot use fonts that are packed across rows.


.. _eve register access:

EVE Register Access
-------------------

The functions :cpp:func:`lv_draw_eve_memread8`, :cpp:func:`lv_draw_eve_memread16`, :cpp:func:`lv_draw_eve_memread32`,
:cpp:func:`lv_draw_eve_memwrite8`, :cpp:func:`lv_draw_eve_memwrite16`, and :cpp:func:`lv_draw_eve_memwrite32`
are available if needed. They are wrappers around ``EVE_memRead8()``, etc.

Register definitions and other EVE enumerations are available when you include
``lvgl.h`` under the prefix namespace ``LV_EVE_``. I.e., ``REG_ID`` is available
as ``LV_EVE_REG_ID`` and ``EVE_ROM_CHIPID`` is available as ``LV_EVE_EVE_ROM_CHIPID``, etc.


Further Reading
---------------

- https://brtchip.com/wp-content/uploads/Support/Documentation/Programming_Guides/ICs/EVE/FT81X_Series_Programmer_Guide.pdf
- https://brtchip.com/wp-content/uploads/2024/06/BRT_AN_033_BT81X-Series-Programming-Guide.pdf
