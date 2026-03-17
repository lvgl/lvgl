.. _lcd_stm32_guide:

=====================================
SPI Display Driver Creation for STM32
=====================================

Display Driver
--------------

Here is how you can drive a RGB565 240x320 SPI display using STM32 HAL SPI.
You can use direct or partial mode. Single-buffered direct is shown in this example.

In your initialization code, create the display and set the buffers and flush callback.

.. code-block:: c

    static uint8_t fb[240 * 320 * 2];

    lv_display_t * disp = lv_display_create(240, 320);
    lv_display_set_buffers(disp, fb, NULL, sizeof(fb), LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565); /* not needed if LV_COLOR_DEPTH is 16 */

The generated STM32 HAL code should initialize the SPI peripheral for you based
on the configuration values in STM32CubeIDE or STM32CubeMX.

If there are any commands that should be sent to initialize the display, send them before
entering the timer loop.

Here is an example flush callback implementation.

.. code-block:: c

    extern SPI_HandleTypeDef hspi1;

    static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
    {
        if(lv_display_flush_is_last(disp)) {
            /* Chip select (CS) pins are typically active-low. */
            HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
            HAL_SPI_Transmit(&hspi1, px_map, lv_area_get_size(area) * 2, HAL_MAX_DELAY);
            HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
        }
        lv_display_flush_ready(disp);
    }

Performance can be improved by using DMA with double buffering.


Touch indev
-----------

In your initialization code, create the touch screen indev.

.. code-block:: c

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touch_read);

The generated STM32 HAL code should initialize the I2C peripheral for you based
on the configuration values in STM32CubeIDE or STM32CubeMX.

Here is an example implementation of a touch driver that uses i2c
communication and an interrupt pin to signal available data.

.. code-block:: c

    extern I2C_HandleTypeDef hi2c2;

    static volatile bool do_sample_touch = false;
    static lv_indev_state_t last_state = LV_INDEV_STATE_RELEASED;

    /* the STM32 HAL calls this function for you */
    void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
    {
        if (GPIO_Pin == TP_IRQ_Pin)
        {
            do_sample_touch = true;
        }
    }

    static void touch_read(lv_indev_t * indev, lv_indev_data_t * data)
    {
        NVIC_DisableIRQ(EXTI5_IRQn);
        if (do_sample_touch)
        {
            uint8_t touches = 0;
            uint8_t buf[6];
            const uint16_t STATUS_REG = 0x814E;
            const uint16_t TOUCH_POS_REG = 0x8150;
            uint8_t ZERO = 0;

            HAL_I2C_Mem_Read(&hi2c2, 0xBA, STATUS_REG, 2, buf, 1, HAL_MAX_DELAY);
            touches = (0x0F & buf[0]);

            HAL_I2C_Mem_Write(&hi2c2, 0xBA, STATUS_REG, 2, &ZERO, 1, HAL_MAX_DELAY);

            do_sample_touch = false;

            if (touches > 0)
            {
                last_state = LV_INDEV_STATE_PRESSED;

                HAL_I2C_Mem_Read(&hi2c2, 0xBA, TOUCH_POS_REG, 2, buf, 4, HAL_MAX_DELAY);
                data->point.x = buf[0] + (buf[1] << 8);
                data->point.y = buf[2] + (buf[3] << 8);
            }
            else {
                last_state = LV_INDEV_STATE_RELEASED;
            }
        }
        NVIC_EnableIRQ(EXTI5_IRQn);

        data->state = last_state;
    }
