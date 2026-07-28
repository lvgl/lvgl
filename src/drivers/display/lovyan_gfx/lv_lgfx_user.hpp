#pragma once

#include "../../../lvgl_public.h"

#if LV_USE_LOVYAN_GFX

/*
 * This header is included by lv_lovyan_gfx.cpp through LV_LGFX_USER_INCLUDE.
 * It must provide a default-constructible class named LGFX.
 *
 * There are three common ways to do that:
 *
 * 1. Use a board supported by LovyanGFX.
 *    Include LovyanGFX's board autodetect header and enable the board macro.
 *
 * 2. Use display and touch drivers supported by LovyanGFX, but configure the
 *    panel yourself by deriving LGFX from lgfx::LGFX_Device.
 *
 * 3. Use a display driver not supported by LovyanGFX.
 *    Implement an LGFX-compatible wrapper class with the methods used by
 *    lv_lovyan_gfx.cpp.
 *
 * Select one option below.
 */

#define LV_LGFX_USE_LOVYAN_BOARD 1
#define LV_LGFX_USE_LOVYAN_DEVICE 2
#define LV_LGFX_USE_CUSTOM_WRAPPER 3

#ifndef LV_LGFX_TYPE
#define LV_LGFX_TYPE LV_LGFX_USE_LOVYAN_BOARD
#endif

#if LV_LGFX_TYPE == LV_LGFX_USE_LOVYAN_BOARD
/* Define the matching LovyanGFX board macro before including LGFX_AUTODETECT.hpp
 * Examples: LGFX_M5STACK, LGFX_M5STACK_CORE2, LGFX_AUTODETECT.
 * See https://github.com/lovyan03/LovyanGFX#%E4%BD%BF%E3%81%84%E6%96%B9-how-to-use
 */
#define LGFX_USE_V1

// #define LGFX_M5STACK // Define a specific board
#define LGFX_AUTODETECT // or use autodetect to automatically select the board based on the detected hardware

#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

#elif LV_LGFX_TYPE == LV_LGFX_USE_LOVYAN_DEVICE
/**
 * LovyanGFX supports many display and touch drivers. If your hardware is supported, 
 * you can derive LGFX from lgfx::LGFX_Device and configure the panel in the constructor.
 */

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_GC9A01 panel;
    lgfx::Bus_SPI bus;
    lgfx::Light_PWM light;
    lgfx::Touch_CST816S touch;

public:
    LGFX(void)
    {
        {
            auto cfg = bus.config();

            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read = 16000000;
            cfg.spi_3wire = false;
            cfg.use_lock = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk = 18;
            cfg.pin_mosi = 23;
            cfg.pin_miso = -1;
            cfg.pin_dc = 2;

            bus.config(cfg);
            panel.setBus(&bus);
        }

        {
            auto cfg = panel.config();

            cfg.pin_cs = 5;
            cfg.pin_rst = 4;
            cfg.pin_busy = -1;
            cfg.memory_width = 240;
            cfg.memory_height = 240;
            cfg.panel_width = 240;
            cfg.panel_height = 240;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = false;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = false;

            panel.config(cfg);
        }

        {
            auto cfg = light.config();

            cfg.pin_bl = -1;
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

            light.config(cfg);
            panel.setLight(&light);
        }

        {
            auto cfg = touch.config();

            cfg.x_min = 0;
            cfg.x_max = 240;
            cfg.y_min = 0;
            cfg.y_max = 240;
            cfg.pin_int = -1;
            cfg.pin_rst = -1;
            cfg.bus_shared = true;
            cfg.offset_rotation = 0;
            cfg.i2c_port = 0;
            cfg.i2c_addr = 0x15;
            cfg.pin_sda = 21;
            cfg.pin_scl = 22;
            cfg.freq = 400000;

            touch.config(cfg);
            panel.setTouch(&touch);
        }

        setPanel(&panel);
    }
};

#elif LV_LGFX_TYPE == LV_LGFX_USE_CUSTOM_WRAPPER
/**
 * If your display is not supported by LovyanGFX, you can implement a wrapper class
 * with the methods used by lv_lovyan_gfx.cpp. The class must be default-constructible
 * and named LGFX.
 */

class LGFX
{
public:
    LGFX(void) {}

    bool init(void)
    {
        // Initialize your display here. Return true if successful, false otherwise.
        return true;
    }

    void initDMA(void)
    {
        // Initialize DMA if your display supports it.
    }

    void waitDMA(void)
    {
        // Wait for any ongoing DMA transfers to complete.
    }

    void fillScreen(uint16_t color)
    {
        // Fill the entire screen with the specified color.
    }

    void setRotation(uint8_t rotation)
    {
        // Set the display rotation (0-3).
    }

    uint8_t getRotation(void)
    {
        // Return the current rotation (0-3). 
        return 0;
    }

    int32_t width(void)
    {
        // Return the width of the display relative to its current rotation.
        return 0;
    }

    int32_t height(void)
    {
        // Return the height of the display relative to its current rotation.
        return 0;
    }

    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t * data)
    {
        // Push an image to the display.
    }

    void pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t * data)
    {
        // Push an image to the display using DMA.
    }

    void startWrite(void)
    {
        // Start a write operation.
    }

    uint32_t getStartCount(void)
    {
        // Return the number of times startWrite() has been called.
        return 0;
    }

    void endWrite(void)
    {
        // End a write operation.
    }

    void setBrightness(uint8_t brightness)
    {
        // Set the display brightness.
    }

    void writePixel(int32_t x, int32_t y, const uint16_t color)
    {
        // Write a single pixel to the display.
    }

    bool getTouch(uint16_t * x, uint16_t * y)
    {
        // Return true if the screen is touched, false otherwise. If touched, set x and y to the touch coordinates.
        return false;
    }
};

#else
#error "Select a valid LV_LGFX_TYPE in lv_lgfx_user.hpp"
#endif

#endif /* LV_USE_LOVYAN_GFX */
