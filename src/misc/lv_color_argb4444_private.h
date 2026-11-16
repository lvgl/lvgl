/**
 * @file lv_color_argb4444_private.h
 */

#ifndef LV_COLOR_ARGB4444_PRIVATE_H
#define LV_COLOR_ARGB4444_PRIVATE_H

#include "../lvgl_public.h"

/* Byte accesses also work for odd strides and unaligned image data. */
static inline lv_color32_t lv_color_argb4444_read(const uint8_t * data)
{
    return lv_color_from_argb4444((uint16_t)(data[0] | ((uint16_t)data[1] << 8)));
}

static inline void lv_color_argb4444_write(uint8_t * data, lv_color32_t color)
{
    uint16_t pixel = lv_color_to_argb4444(color);
    data[0] = (uint8_t)pixel;
    data[1] = (uint8_t)(pixel >> 8);
}

#endif /*LV_COLOR_ARGB4444_PRIVATE_H*/
