#pragma once

#if LV_USE_LOVYAN_GFX

/**
 * If using LovyanGFX create LGFX class that inherits from lgfx::LGFX_Device
 * https://github.com/lovyan03/LovyanGFX/blob/master/examples/HowToUse/2_user_setting/2_user_setting.ino */

/**
 * If using other display drivers that is not LovyanGFX
 * Create an LGFX wrapper class that implements the functions used in lv_lovyan_gfx.cpp */
class LGFX
{
public:
    LGFX(void) {}

    bool init(void)
    {
        return true;
    }

    void initDMA(void) {}

    void waitDMA(void) {}

    void fillScreen(uint16_t color) {}

    void setRotation(uint8_t rotation) {}

    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data) {}

    void pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data) {}

    void startWrite(void) {}

    uint32_t getStartCount(void)
    {
        return 0;
    }

    void endWrite(void) {}

    void setBrightness(uint8_t brightness){}

    void writePixel(int32_t x, int32_t y, const uint16_t color) {}

    bool getTouch(uint16_t *x, uint16_t *y)
    {
        return false;
    }
};

#endif
