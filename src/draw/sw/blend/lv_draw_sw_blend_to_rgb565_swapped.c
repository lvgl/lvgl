/**
 * @file lv_draw_sw_blend_to_rgb565_swapped.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_to_rgb565_swapped.h"
#if LV_USE_DRAW_SW

    #if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED

        #include "lv_draw_sw_blend_private.h"

        /*********************
        *  TEMPLATE INSTANCE
        *********************/

        #define LV_BLEND_COLOR_FN       lv_draw_sw_blend_color_to_rgb565_swapped
        #define LV_BLEND_IMAGE_FN       lv_draw_sw_blend_image_to_rgb565_swapped

        /*No NEON, Helium, etc. implementation for swapped pixels, so use the C implementation*/
        #define LV_BLEND_NO_HOOK(...)   LV_RESULT_INVALID
        #define LV_BLEND_HOOK(name)     LV_BLEND_NO_HOOK

        #define DEST_IS_SWAPPED         1
        #define DEST_LOAD(px)           lv_color_swap_16(px)
        #define DEST_STORE(c)           lv_color_swap_16(c)
        #define DEST_C16(px)            lv_color16_from_u16(lv_color_swap_16(px))

        #include "lv_draw_sw_blend_to_rgb565.inc"

    #endif /*LV_DRAW_SW_SUPPORT_RGB565_SWAPPED*/

#endif /*LV_USE_DRAW_SW*/
