/**
 * @file lv_draw_sw_blend_to_rgb565.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_to_rgb565.h"
#if LV_USE_DRAW_SW

    #if LV_DRAW_SW_SUPPORT_RGB565

        #include "lv_draw_sw_blend_private.h"

        /*********************
        *      DEFINES
        *********************/

        /*Unset accelerator hooks fall back to the C implementation*/
        #ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565
            #define LV_DRAW_SW_COLOR_BLEND_TO_RGB565(...)                           LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA
            #define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA(...)                  LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK
            #define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK(...)                 LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA
            #define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA(...)              LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565
            #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565(...)                       LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_OPA
            #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)              LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_MASK
            #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)             LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
            #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)          LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565
            #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565(...)                       LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_OPA
            #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)              LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_MASK
            #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)             LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
            #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)          LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565
            #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565(...)                   LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA
            #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)          LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK
            #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)         LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
            #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)      LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_RGB565
            #define LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_RGB565(...)                   LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_RGB565_WITH_OPA
            #define LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)          LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_RGB565_WITH_MASK
            #define LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)         LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
            #define LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)      LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565
            #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565(...)                   LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA
            #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)          LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK
            #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)         LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
            #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)      LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565
            #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565(...)                 LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA
            #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)        LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK
            #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)       LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
            #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)    LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565
            #define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565(...) LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_WITH_OPA
            #define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_WITH_OPA(...) LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_WITH_MASK
            #define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_WITH_MASK(...) LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
            #define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...) LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565
            #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565(...)  LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_WITH_OPA
            #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)  LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_WITH_MASK
            #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)  LV_RESULT_INVALID
        #endif

        #ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
            #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)  LV_RESULT_INVALID
        #endif

        /*********************
        *  TEMPLATE INSTANCE
        *********************/

        #define LV_BLEND_COLOR_FN       lv_draw_sw_blend_color_to_rgb565
        #define LV_BLEND_IMAGE_FN       lv_draw_sw_blend_image_to_rgb565
        #define LV_BLEND_HOOK(name)     LV_DRAW_SW_##name

        #define DEST_IS_SWAPPED         0
        #define DEST_LOAD(px)           (px)
        #define DEST_STORE(c)           (c)
        #define DEST_C16(px)            (*(lv_color16_t *)&(px))

        #include "lv_draw_sw_blend_to_rgb565.inc"

    #endif /*LV_DRAW_SW_SUPPORT_RGB565*/

#endif /*LV_USE_DRAW_SW*/
