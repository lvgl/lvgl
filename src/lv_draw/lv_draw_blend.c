/**
 * @file lv_draw_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_blend.h"
#include "lv_img_decoder.h"
#include "../lv_misc/lv_math.h"
#include "../lv_hal/lv_hal_disp.h"
#include "../lv_core/lv_refr.h"

#if LV_USE_GPU_NXP_PXP
    #include "../lv_gpu/lv_gpu_nxp_pxp.h"
#elif LV_USE_GPU_NXP_VG_LITE
    #include "../lv_gpu/lv_gpu_nxp_vglite.h"
#elif LV_USE_GPU_STM32_DMA2D
    #include "../lv_gpu/lv_gpu_stm32_dma2d.h"
#endif

/*********************
 *      DEFINES
 *********************/
#define GPU_SIZE_LIMIT      240

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void fill_set_px(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
                        lv_color_t color, lv_opa_t opa,
                        const lv_opa_t * mask, lv_draw_mask_res_t mask_res);

LV_ATTRIBUTE_FAST_MEM static void fill_normal(const lv_area_t * disp_area, lv_color_t * disp_buf,
                                              const lv_area_t * draw_area,
                                              lv_color_t color, lv_opa_t opa,
                                              const lv_opa_t * mask, lv_draw_mask_res_t mask_res);

#if LV_USE_BLEND_MODES
static void fill_blended(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
                         lv_color_t color, lv_opa_t opa,
                         const lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_blend_mode_t mode);
#endif

static void map_set_px(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
                       const lv_area_t * map_area, const lv_color_t * map_buf, lv_opa_t opa,
                       const lv_opa_t * mask, lv_draw_mask_res_t mask_res);

LV_ATTRIBUTE_FAST_MEM static void map_normal(const lv_area_t * disp_area, lv_color_t * disp_buf,
                                             const lv_area_t * draw_area,
                                             const lv_area_t * map_area, const lv_color_t * map_buf, lv_opa_t opa,
                                             const lv_opa_t * mask, lv_draw_mask_res_t mask_res);

#if LV_USE_BLEND_MODES
static void map_blended(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
                        const lv_area_t * map_area, const lv_color_t * map_buf, lv_opa_t opa,
                        const lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_blend_mode_t mode);

static inline lv_color_t color_blend_true_color_additive(lv_color_t fg, lv_color_t bg, lv_opa_t opa);
static inline lv_color_t color_blend_true_color_subtractive(lv_color_t fg, lv_color_t bg, lv_opa_t opa);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_GPU || LV_USE_GPU_STM32_DMA2D
    LV_ATTRIBUTE_DMA static lv_color_t blend_buf[LV_HOR_RES_MAX];
#endif

/**********************
 *      MACROS
 **********************/


#define FILL_NORMAL_MASK_PX(out_x,  color)                                                          \
    if(*mask_tmp_x) {          \
        if(*mask_tmp_x == LV_OPA_COVER) disp_buf_first[out_x] = color;                                 \
        else disp_buf_first[out_x] = lv_color_mix(color, disp_buf_first[out_x], *mask_tmp_x);            \
    }                                                                                               \
    mask_tmp_x++;


#define FILL_NORMAL_MASK_PX_SCR_TRANSP(out_x,  color)                                               \
    if(*mask_tmp_x) {          \
        if(*mask_tmp_x == LV_OPA_COVER) disp_buf_first[out_x] = color;                                 \
        else if(disp->driver.screen_transp) lv_color_mix_with_alpha(disp_buf_first[out_x], disp_buf_first[out_x].ch.alpha,              \
                                                                        color, *mask_tmp_x, &disp_buf_first[out_x], &disp_buf_first[out_x].ch.alpha);           \
        else disp_buf_first[out_x] = lv_color_mix(color, disp_buf_first[out_x], *mask_tmp_x);            \
    }                                                                                                      \
    mask_tmp_x++;


#define MAP_NORMAL_MASK_PX(x)                                                          \
    if(*mask_tmp_x) {          \
        if(*mask_tmp_x == LV_OPA_COVER) disp_buf_first[x] = map_buf_first[x];                                 \
        else disp_buf_first[x] = lv_color_mix(map_buf_first[x], disp_buf_first[x], *mask_tmp_x);            \
    }                                                                                               \
    mask_tmp_x++;

#define MAP_NORMAL_MASK_PX_SCR_TRANSP(x)                        \
    if(*mask_tmp_x) {          \
        if(*mask_tmp_x == LV_OPA_COVER) disp_buf_first[x] = map_buf_first[x];                                 \
        else if(disp->driver.screen_transp) lv_color_mix_with_alpha(disp_buf_first[x], disp_buf_first[x].ch.alpha,              \
                                                                        map_buf_first[x], *mask_tmp_x, &disp_buf_first[x], &disp_buf_first[x].ch.alpha);                  \
        else disp_buf_first[x] = lv_color_mix(map_buf_first[x], disp_buf_first[x], *mask_tmp_x);            \
    }                                                                                               \
    mask_tmp_x++;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


/**
 * Fill and area in the display buffer.
 * @param clip_area clip the fill to this area  (absolute coordinates)
 * @param fill_area fill this area  (absolute coordinates) (should be clipped)
 * @param color fill color
 * @param mask a mask to apply on the fill (uint8_t array with 0x00..0xff values).
 *             Relative to fill area but its width is truncated to clip area.
 * @param mask_res LV_MASK_RES_COVER: the mask has only 0xff values (no mask),
 *                 LV_MASK_RES_TRANSP: the mask has only 0x00 values (full transparent),
 *                 LV_MASK_RES_CHANGED: the mask has mixed values
 * @param opa overall opacity in 0x00..0xff range
 * @param mode blend mode from `lv_blend_mode_t`
 */
LV_ATTRIBUTE_FAST_MEM void _lv_blend_fill(const lv_area_t * clip_area, const lv_area_t * fill_area,
                                          lv_color_t color, lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_opa_t opa,
                                          lv_blend_mode_t mode)
{
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;
    if(mask_res == LV_DRAW_MASK_RES_TRANSP) return;

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);
    const lv_area_t * disp_area = &vdb->area;
    lv_color_t * disp_buf = vdb->buf_act;

    if(disp->driver.gpu_wait_cb) disp->driver.gpu_wait_cb(&disp->driver);

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = _lv_area_intersect(&draw_area, clip_area, fill_area);
    if(!is_common) return;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    /*Round the values in the mask if anti-aliasing is disabled*/
#if LV_ANTIALIAS
    if(mask && disp->driver.antialiasing == 0)
#else
    if(mask)
#endif
    {
        int32_t mask_w = lv_area_get_width(&draw_area);
        int32_t i;
        for(i = 0; i < mask_w; i++)  mask[i] = mask[i] > 128 ? LV_OPA_COVER : LV_OPA_TRANSP;
    }

    if(disp->driver.set_px_cb) {
        fill_set_px(disp_area, disp_buf, &draw_area, color, opa, mask, mask_res);
    }
    else if(mode == LV_BLEND_MODE_NORMAL) {
        fill_normal(disp_area, disp_buf, &draw_area, color, opa, mask, mask_res);
    }
#if LV_USE_BLEND_MODES
    else {
        fill_blended(disp_area, disp_buf, &draw_area, color, opa, mask, mask_res, mode);
    }
#endif
}

/**
 * Copy a map (image) to a display buffer.
 * @param clip_area clip the map to this area (absolute coordinates)
 * @param map_area area of the image  (absolute coordinates)
 * @param map_buf a pixels of the map (image)
 * @param mask a mask to apply on every pixel (uint8_t array with 0x00..0xff values).
 *                Relative to map area but its width is truncated to clip area.
 * @param mask_res LV_MASK_RES_COVER: the mask has only 0xff values (no mask),
 *                 LV_MASK_RES_TRANSP: the mask has only 0x00 values (full transparent),
 *                 LV_MASK_RES_CHANGED: the mask has mixed values
 * @param opa  overall opacity in 0x00..0xff range
 * @param mode blend mode from `lv_blend_mode_t`
 */
LV_ATTRIBUTE_FAST_MEM void _lv_blend_map(const lv_area_t * clip_area, const lv_area_t * map_area,
                                         const lv_color_t * map_buf,
                                         lv_opa_t * mask, lv_draw_mask_res_t mask_res,
                                         lv_opa_t opa, lv_blend_mode_t mode)
{
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;
    if(mask_res == LV_DRAW_MASK_RES_TRANSP) return;

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = _lv_area_intersect(&draw_area, clip_area, map_area);
    if(!is_common) return;

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);
    const lv_area_t * disp_area = &vdb->area;
    lv_color_t * disp_buf = vdb->buf_act;

    if(disp->driver.gpu_wait_cb) disp->driver.gpu_wait_cb(&disp->driver);

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    /*Round the values in the mask if anti-aliasing is disabled*/
#if LV_ANTIALIAS
    if(mask && disp->driver.antialiasing == 0)
#else
    if(mask)
#endif
    {
        int32_t mask_w = lv_area_get_width(&draw_area);
        int32_t i;
        for(i = 0; i < mask_w; i++)  mask[i] = mask[i] > 128 ? LV_OPA_COVER : LV_OPA_TRANSP;
    }
    if(disp->driver.set_px_cb) {
        map_set_px(disp_area, disp_buf, &draw_area, map_area, map_buf, opa, mask, mask_res);
    }
    else if(mode == LV_BLEND_MODE_NORMAL) {
        map_normal(disp_area, disp_buf, &draw_area, map_area, map_buf, opa, mask, mask_res);
    }
#if LV_USE_BLEND_MODES
    else {
        map_blended(disp_area, disp_buf, &draw_area, map_area, map_buf, opa, mask, mask_res, mode);
    }
#endif
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void fill_set_px(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
                        lv_color_t color, lv_opa_t opa,
                        const lv_opa_t * mask, lv_draw_mask_res_t mask_res)
{

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    /*Get the width of the `disp_area` it will be used to go to the next line*/
    int32_t disp_w = lv_area_get_width(disp_area);

    int32_t x;
    int32_t y;

    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) {
        for(y = draw_area->y1; y <= draw_area->y2; y++) {
            for(x = draw_area->x1; x <= draw_area->x2; x++) {
                disp->driver.set_px_cb(&disp->driver, (void *)disp_buf, disp_w, x, y, color, opa);
            }
        }
    }
    else {
        /* The mask is relative to the clipped area.
         * In the cycles below mask will be indexed from `draw_area.x1`
         * but it corresponds to zero index. So prepare `mask_tmp` accordingly. */
        const lv_opa_t * mask_tmp = mask - draw_area->x1;

        /*Get the width of the `draw_area` it will be used to go to the next line of the mask*/
        int32_t draw_area_w = lv_area_get_width(draw_area);

        for(y = draw_area->y1; y <= draw_area->y2; y++) {
            for(x = draw_area->x1; x <= draw_area->x2; x++) {
                if(mask_tmp[x]) {
                    disp->driver.set_px_cb(&disp->driver, (void *)disp_buf, disp_w, x, y, color,
                                           (uint32_t)((uint32_t)opa * mask_tmp[x]) >> 8);
                }
            }
            mask_tmp += draw_area_w;
        }
    }
}

/**
 * Fill an area with a color
 * @param disp_area the current display area (destination area)
 * @param disp_buf destination buffer
 * @param draw_area fill this area (relative to `disp_area`)
 * @param color fill color
 * @param opa overall opacity in 0x00..0xff range
 * @param mask a mask to apply on every pixel (uint8_t array with 0x00..0xff values).
 *                It fits into draw_area.
 * @param mask_res LV_MASK_RES_COVER: the mask has only 0xff values (no mask),
 *                 LV_MASK_RES_TRANSP: the mask has only 0x00 values (full transparent),
 *                 LV_MASK_RES_CHANGED: the mask has mixed values
 */
LV_ATTRIBUTE_FAST_MEM static void fill_normal(const lv_area_t * disp_area, lv_color_t * disp_buf,
                                              const lv_area_t * draw_area,
                                              lv_color_t color, lv_opa_t opa,
                                              const lv_opa_t * mask, lv_draw_mask_res_t mask_res)
{

#if LV_USE_GPU || LV_COLOR_SCREEN_TRANSP
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
#endif

    /*Get the width of the `disp_area` it will be used to go to the next line*/
    int32_t disp_w = lv_area_get_width(disp_area);

    int32_t draw_area_w = lv_area_get_width(draw_area);
    int32_t draw_area_h = lv_area_get_height(draw_area);

    /*Create a temp. disp_buf which always point to the first pixel of the destination area*/
    lv_color_t * disp_buf_first = disp_buf + disp_w * draw_area->y1 + draw_area->x1;

    int32_t x;
    int32_t y;

    /*Simple fill (maybe with opacity), no masking*/
    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) {
        if(opa > LV_OPA_MAX) {
#if LV_USE_GPU
            if(disp->driver.gpu_fill_cb && lv_area_get_size(draw_area) > GPU_SIZE_LIMIT) {
                disp->driver.gpu_fill_cb(&disp->driver, disp_buf, disp_w, draw_area, color);
                return;
            }
#elif LV_USE_GPU_NXP_PXP
            if(lv_area_get_size(draw_area) >= LV_GPU_NXP_PXP_FILL_SIZE_LIMIT) {
                lv_gpu_nxp_pxp_fill(disp_buf, disp_w, draw_area, color, opa);
                return;
            }
#elif LV_USE_GPU_NXP_VG_LITE
            if(lv_area_get_size(draw_area) >= LV_GPU_NXP_VG_LITE_FILL_SIZE_LIMIT) {
                if(lv_gpu_nxp_vglite_fill(disp_buf, disp_w, lv_area_get_height(disp_area), draw_area, color, opa) == LV_RES_OK) {
                    return;
                }
                /* Fall down to SW render in case of error */
            }
#elif LV_USE_GPU_STM32_DMA2D
            if(lv_area_get_size(draw_area) >= 240) {
                lv_gpu_stm32_dma2d_fill(disp_buf_first, disp_w, color, draw_area_w, draw_area_h);
                return;
            }
#endif
            /*Software rendering*/
            for(y = 0; y < draw_area_h; y++) {
                lv_color_fill(disp_buf_first, color, draw_area_w);
                disp_buf_first += disp_w;
            }
        }
        /*No mask with opacity*/
        else {

#if LV_USE_GPU_NXP_PXP
            if(lv_area_get_size(draw_area) >= LV_GPU_NXP_PXP_FILL_OPA_SIZE_LIMIT) {
                lv_gpu_nxp_pxp_fill(disp_buf, disp_w, draw_area, color, opa);
                return;
            }
#elif LV_USE_GPU_NXP_VG_LITE
            if(lv_area_get_size(draw_area) >= LV_GPU_NXP_VG_LITE_FILL_OPA_SIZE_LIMIT) {
                if(lv_gpu_nxp_vglite_fill(disp_buf, disp_w, lv_area_get_height(disp_area), draw_area, color, opa) == LV_RES_OK) {
                    return;
                }
                /* Fall down to SW render in case of error */
            }
#elif LV_USE_GPU
            if(disp->driver.gpu_blend_cb && lv_area_get_size(draw_area) > GPU_SIZE_LIMIT) {
                for(x = 0; x < draw_area_w ; x++) blend_buf[x].full = color.full;

                for(y = draw_area->y1; y <= draw_area->y2; y++) {
                    disp->driver.gpu_blend_cb(&disp->driver, disp_buf_first, blend_buf, draw_area_w, opa);
                    disp_buf_first += disp_w;
                }
                return;
            }
#endif


#if LV_USE_GPU_STM32_DMA2D
            if(lv_area_get_size(draw_area) >= 240) {
                if(blend_buf[0].full != color.full) lv_color_fill(blend_buf, color, LV_HOR_RES_MAX);

                lv_coord_t line_h = LV_HOR_RES_MAX / draw_area_w;
                for(y = 0; y <= draw_area_h - line_h; y += line_h) {
                    lv_gpu_stm32_dma2d_blend(disp_buf_first, disp_w, blend_buf, opa, draw_area_w, draw_area_w, line_h);
                    lv_gpu_stm32_dma2d_wait_cb(NULL);
                    disp_buf_first += disp_w * line_h;
                }

                if(y != draw_area_h) {
                    lv_gpu_stm32_dma2d_blend(disp_buf_first, disp_w, blend_buf, opa, draw_area_w, draw_area_w, draw_area_h - y);
                }

                return;
            }
#endif
            lv_color_t last_dest_color = LV_COLOR_BLACK;
            lv_color_t last_res_color = lv_color_mix(color, last_dest_color, opa);

            uint16_t color_premult[3];
            lv_color_premult(color, opa, color_premult);
            lv_opa_t opa_inv = 255 - opa;

            for(y = 0; y < draw_area_h; y++) {
                for(x = 0; x < draw_area_w; x++) {
                    if(last_dest_color.full != disp_buf_first[x].full) {
                        last_dest_color = disp_buf_first[x];

#if LV_COLOR_SCREEN_TRANSP
                        if(disp->driver.screen_transp) {
                            lv_color_mix_with_alpha(disp_buf_first[x], disp_buf_first[x].ch.alpha, color, opa, &last_res_color,
                                                    &last_res_color.ch.alpha);
                        }
                        else
#endif
                        {
                            last_res_color = lv_color_mix_premult(color_premult, disp_buf_first[x], opa_inv);
                        }
                    }
                    disp_buf_first[x] = last_res_color;
                }
                disp_buf_first += disp_w;
            }
        }
    }
    /*Masked*/
    else {
        /*DMA2D could be used here but it's much slower than software rendering*/
#if LV_USE_GPU_STM32_DMA2D && 0
        if(lv_area_get_size(draw_area) > 240) {
            lv_gpu_stm32_dma2d_fill_mask(disp_buf_first, disp_w, color, mask, opa, draw_area_w, draw_area_h);
            return;
        }
#endif


        /*Buffer the result color to avoid recalculating the same color*/
        lv_color_t last_dest_color;
        lv_color_t last_res_color;
        lv_opa_t last_mask = LV_OPA_TRANSP;
        last_dest_color.full = disp_buf_first[0].full;
        last_res_color.full = disp_buf_first[0].full;

        int32_t x_end4 = draw_area_w - 4;

        /*Only the mask matters*/
        if(opa > LV_OPA_MAX) {
            for(y = 0; y < draw_area_h; y++) {
                const lv_opa_t * mask_tmp_x = mask;
#if 0
                for(x = 0; x < draw_area_w; x++) {
#if LV_COLOR_SCREEN_TRANSP
                    FILL_NORMAL_MASK_PX_SCR_TRANSP(x, color)
#else
                    FILL_NORMAL_MASK_PX(x, color)
#endif
                }
#else
                for(x = 0; x < draw_area_w && ((lv_uintptr_t)mask_tmp_x & 0x3); x++) {
#if LV_COLOR_SCREEN_TRANSP
                    FILL_NORMAL_MASK_PX_SCR_TRANSP(x, color)
#else
                    FILL_NORMAL_MASK_PX(x, color)
#endif
                }

                uint32_t * mask32 = (uint32_t *) mask_tmp_x;
                for(; x <= x_end4; x += 4) {
                    if(*mask32) {
                        if((*mask32) == 0xFFFFFFFF) {
                            disp_buf_first[x] = color;
                            disp_buf_first[x + 1] = color;
                            disp_buf_first[x + 2] = color;
                            disp_buf_first[x + 3] = color;
                        }
                        else {
                            mask_tmp_x = (const lv_opa_t *)mask32;
#if LV_COLOR_SCREEN_TRANSP
                            FILL_NORMAL_MASK_PX_SCR_TRANSP(x, color)
                            FILL_NORMAL_MASK_PX_SCR_TRANSP(x + 1, color)
                            FILL_NORMAL_MASK_PX_SCR_TRANSP(x + 2, color)
                            FILL_NORMAL_MASK_PX_SCR_TRANSP(x + 3, color)
#else
                            FILL_NORMAL_MASK_PX(x, color)
                            FILL_NORMAL_MASK_PX(x + 1, color)
                            FILL_NORMAL_MASK_PX(x + 2, color)
                            FILL_NORMAL_MASK_PX(x + 3, color)
#endif
                        }
                    }
                    mask32++;
                }

                mask_tmp_x = (const lv_opa_t *)mask32;
                for(; x < draw_area_w ; x++) {
#if LV_COLOR_SCREEN_TRANSP
                    FILL_NORMAL_MASK_PX_SCR_TRANSP(x, color)
#else
                    FILL_NORMAL_MASK_PX(x, color)
#endif
                }
#endif
                disp_buf_first += disp_w;
                mask += draw_area_w;
            }
        }
        /*Handle opa and mask values too*/
        else {
            lv_opa_t opa_tmp = LV_OPA_TRANSP;
            for(y = draw_area->y1; y <= draw_area->y2; y++) {
                const lv_opa_t * mask_tmp_x = mask;
                for(x = 0; x < draw_area_w; x++) {
                    if(*mask_tmp_x) {
                        if(*mask_tmp_x != last_mask) opa_tmp = *mask_tmp_x == LV_OPA_COVER ? opa :
                                                                   (uint32_t)((uint32_t)(*mask_tmp_x) * opa) >> 8;
                        if(*mask_tmp_x != last_mask || last_dest_color.full != disp_buf_first[x].full) {
#if LV_COLOR_SCREEN_TRANSP
                            if(disp->driver.screen_transp) {
                                lv_color_mix_with_alpha(disp_buf_first[x], disp_buf_first[x].ch.alpha, color, opa_tmp, &last_res_color,
                                                        &last_res_color.ch.alpha);
                            }
                            else
#endif
                            {
                                if(opa_tmp == LV_OPA_COVER) last_res_color = color;
                                else last_res_color = lv_color_mix(color, disp_buf_first[x], opa_tmp);
                            }
                            last_mask = *mask_tmp_x;
                            last_dest_color.full = disp_buf_first[x].full;
                        }
                        disp_buf_first[x] = last_res_color;
                    }
                    mask_tmp_x++;
                }
                disp_buf_first += disp_w;
                mask += draw_area_w;
            }
        }
    }
}

#if LV_USE_BLEND_MODES
/**
 * Fill an area with a color but apply blending algorithms
 * @param disp_area the current display area (destination area)
 * @param disp_buf destination buffer
 * @param draw_area fill this area (relative to `disp_area`)
 * @param color fill color
 * @param opa overall opacity in 0x00..0xff range
 * @param mask a mask to apply on every pixel (uint8_t array with 0x00..0xff values).
 *                It fits into draw_area.
 * @param mask_res LV_MASK_RES_COVER: the mask has only 0xff values (no mask),
 *                 LV_MASK_RES_TRANSP: the mask has only 0x00 values (full transparent),
 *                 LV_MASK_RES_CHANGED: the mask has mixed values
 * @param mode blend mode from `lv_blend_mode_t`
 */
static void fill_blended(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
                         lv_color_t color, lv_opa_t opa,
                         const lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_blend_mode_t mode)
{
    /*Get the width of the `disp_area` it will be used to go to the next line*/
    int32_t disp_w = lv_area_get_width(disp_area);

    /*Create a temp. disp_buf which always point to current line to draw*/
    lv_color_t * disp_buf_tmp = disp_buf + disp_w * draw_area->y1;


    lv_color_t (*blend_fp)(lv_color_t, lv_color_t, lv_opa_t);
    switch(mode) {
        case LV_BLEND_MODE_ADDITIVE:
            blend_fp = color_blend_true_color_additive;
            break;
        case LV_BLEND_MODE_SUBTRACTIVE:
            blend_fp = color_blend_true_color_subtractive;
            break;
        default:
            LV_LOG_WARN("fill_blended: unsupported blend mode");
            return;
    }

    int32_t x;
    int32_t y;

    /*Simple fill (maybe with opacity), no masking*/
    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) {
        lv_color_t last_dest_color = LV_COLOR_BLACK;
        lv_color_t last_res_color = lv_color_mix(color, last_dest_color, opa);
        for(y = draw_area->y1; y <= draw_area->y2; y++) {
            for(x = draw_area->x1; x <= draw_area->x2; x++) {
                if(last_dest_color.full != disp_buf_tmp[x].full) {
                    last_dest_color = disp_buf_tmp[x];
                    last_res_color = blend_fp(color, disp_buf_tmp[x], opa);
                }
                disp_buf_tmp[x] = last_res_color;
            }
            disp_buf_tmp += disp_w;
        }
    }
    /*Masked*/
    else {
        /*Get the width of the `draw_area` it will be used to go to the next line of the mask*/
        int32_t draw_area_w = lv_area_get_width(draw_area);

        /* The mask is relative to the clipped area.
         * In the cycles below mask will be indexed from `draw_area.x1`
         * but it corresponds to zero index. So prepare `mask_tmp` accordingly. */
        const lv_opa_t * mask_tmp = mask - draw_area->x1;

        /*Buffer the result color to avoid recalculating the same color*/
        lv_color_t last_dest_color;
        lv_color_t last_res_color;
        lv_opa_t last_mask = LV_OPA_TRANSP;
        last_dest_color.full = disp_buf_tmp[0].full;
        last_res_color.full = disp_buf_tmp[0].full;

        for(y = draw_area->y1; y <= draw_area->y2; y++) {
            for(x = draw_area->x1; x <= draw_area->x2; x++) {
                if(mask_tmp[x] == 0) continue;
                if(mask_tmp[x] != last_mask || last_dest_color.full != disp_buf_tmp[x].full) {
                    lv_opa_t opa_tmp = mask_tmp[x] >= LV_OPA_MAX ? opa : (uint32_t)((uint32_t)mask_tmp[x] * opa) >> 8;

                    last_res_color = blend_fp(color, disp_buf_tmp[x], opa_tmp);
                    last_mask = mask_tmp[x];
                    last_dest_color.full = disp_buf_tmp[x].full;
                }
                disp_buf_tmp[x] = last_res_color;
            }
            disp_buf_tmp += disp_w;
            mask_tmp += draw_area_w;
        }
    }
}
#endif

static void map_set_px(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
                       const lv_area_t * map_area, const lv_color_t * map_buf, lv_opa_t opa,
                       const lv_opa_t * mask, lv_draw_mask_res_t mask_res)

{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    /*Get the width of the `disp_area` it will be used to go to the next line*/
    int32_t disp_w = lv_area_get_width(disp_area);

    /*Get the width of the `draw_area` it will be used to go to the next line of the mask*/
    int32_t draw_area_w = lv_area_get_width(draw_area);

    /*Get the width of the `mask_area` it will be used to go to the next line*/
    int32_t map_w = lv_area_get_width(map_area);

    /*Create a temp. map_buf which always point to current line to draw*/
    const lv_color_t * map_buf_tmp = map_buf + map_w * (draw_area->y1 - (map_area->y1 - disp_area->y1));

    map_buf_tmp += (draw_area->x1 - (map_area->x1 - disp_area->x1));
    map_buf_tmp -= draw_area->x1;
    int32_t x;
    int32_t y;

    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) {
        for(y = draw_area->y1; y <= draw_area->y2; y++) {
            for(x = draw_area->x1; x <= draw_area->x2; x++) {
                disp->driver.set_px_cb(&disp->driver, (void *)disp_buf, disp_w, x, y, map_buf_tmp[x], opa);
            }
            map_buf_tmp += map_w;
        }
    }
    else {
        /* The mask is relative to the clipped area.
         * In the cycles below mask will be indexed from `draw_area.x1`
         * but it corresponds to zero index. So prepare `mask_tmp` accordingly. */
        const lv_opa_t * mask_tmp = mask - draw_area->x1;

        for(y = draw_area->y1; y <= draw_area->y2; y++) {
            for(x = draw_area->x1; x <= draw_area->x2; x++) {
                if(mask_tmp[x]) {
                    disp->driver.set_px_cb(&disp->driver, (void *)disp_buf, disp_w, x, y, map_buf_tmp[x],
                                           (uint32_t)((uint32_t)opa * mask_tmp[x]) >> 8);
                }
            }
            mask_tmp += draw_area_w;
            map_buf_tmp += map_w;
        }
    }
}

/**
 * Copy an image to an area
 * @param disp_area the current display area (destination area)
 * @param disp_buf destination buffer
 * @param map_area coordinates of the map (image) to copy. (absolute coordinates)
 * @param map_buf the pixel of the image
 * @param opa overall opacity in 0x00..0xff range
 * @param mask a mask to apply on every pixel (uint8_t array with 0x00..0xff values).
 *                It fits into draw_area.
 * @param mask_res LV_MASK_RES_COVER: the mask has only 0xff values (no mask),
 *                 LV_MASK_RES_TRANSP: the mask has only 0x00 values (full transparent),
 *                 LV_MASK_RES_CHANGED: the mask has mixed values
 */
LV_ATTRIBUTE_FAST_MEM static void map_normal(const lv_area_t * disp_area, lv_color_t * disp_buf,
                                             const lv_area_t * draw_area,
                                             const lv_area_t * map_area, const lv_color_t * map_buf, lv_opa_t opa,
                                             const lv_opa_t * mask, lv_draw_mask_res_t mask_res)
{

    /*Get the width of the `disp_area` it will be used to go to the next line*/
    int32_t disp_w = lv_area_get_width(disp_area);

    int32_t draw_area_w = lv_area_get_width(draw_area);
    int32_t draw_area_h = lv_area_get_height(draw_area);

    /*Get the width of the `mask_area` it will be used to go to the next line*/
    int32_t map_w = lv_area_get_width(map_area);

    /*Create a temp. disp_buf which always point to first pixel to draw*/
    lv_color_t * disp_buf_first = disp_buf + disp_w * draw_area->y1 + draw_area->x1;

    /*Create a temp. map_buf which always point to first pixel to draw from the map*/
    const lv_color_t * map_buf_first = map_buf + map_w * (draw_area->y1 - (map_area->y1 - disp_area->y1));
    map_buf_first += (draw_area->x1 - (map_area->x1 - disp_area->x1));

#if LV_COLOR_SCREEN_TRANSP || LV_USE_GPU
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
#endif

    int32_t x;
    int32_t y;

    /*Simple fill (maybe with opacity), no masking*/
    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) {
#if LV_USE_GPU
        if(disp->driver.gpu_blend_cb && (lv_area_get_size(draw_area) > GPU_SIZE_LIMIT)) {
            for(y = draw_area->y1; y <= draw_area->y2; y++) {
                disp->driver.gpu_blend_cb(&disp->driver, disp_buf_first, map_buf_first, draw_area_w, opa);
                disp_buf_first += disp_w;
                map_buf_first += map_w;
            }
            return;
        }
#endif

        if(opa > LV_OPA_MAX) {
#if LV_USE_GPU_NXP_PXP
            if(lv_area_get_size(draw_area) >= GPU_NXP_PXP_BLIT_SIZE_LIMIT) {
                lv_gpu_nxp_pxp_blit(disp_buf_first, disp_w, map_buf_first, map_w, draw_area_w, draw_area_h, opa);
                return;
            }
#elif (LV_USE_GPU_NXP_VG_LITE)
            if(lv_area_get_size(draw_area) >= LV_GPU_NXP_VG_LITE_BLIT_SIZE_LIMIT) {

                lv_gpu_nxp_vglite_blit_info_t blit;

                blit.src = map_buf;
                blit.src_width = draw_area_w;
                blit.src_height = draw_area_h;
                blit.src_stride = lv_area_get_width(map_area) * sizeof(lv_color_t);
                blit.src_area.x1 = (draw_area->x1 - (map_area->x1 - disp_area->x1));
                blit.src_area.y1 = (draw_area->y1 - (map_area->y1 - disp_area->y1));
                blit.src_area.x2 = blit.src_area.x1 + draw_area_w;
                blit.src_area.y2 = blit.src_area.y1 + draw_area_h;


                blit.dst = disp_buf;
                blit.dst_width = lv_area_get_width(disp_area);
                blit.dst_height = lv_area_get_height(disp_area);
                blit.dst_stride = lv_area_get_width(disp_area) * sizeof(lv_color_t);
                blit.dst_area.x1 = draw_area->x1;
                blit.dst_area.y1 = draw_area->y1;
                blit.dst_area.x2 = blit.dst_area.x1 + draw_area_w;
                blit.dst_area.y2 = blit.dst_area.y1 + draw_area_h;

                blit.opa = opa;

                if(lv_gpu_nxp_vglite_blit(&blit) == LV_RES_OK) {
                    return;
                }
                /* Fall down to SW render in case of error */
            }
#elif LV_USE_GPU_STM32_DMA2D
            if(lv_area_get_size(draw_area) >= 240) {
                lv_gpu_stm32_dma2d_copy(disp_buf_first, disp_w, map_buf_first, map_w, draw_area_w, draw_area_h);
                return;
            }
#endif

            /*Software rendering*/
            for(y = 0; y < draw_area_h; y++) {
                _lv_memcpy(disp_buf_first, map_buf_first, draw_area_w * sizeof(lv_color_t));
                disp_buf_first += disp_w;
                map_buf_first += map_w;
            }
        }
        else {
#if LV_USE_GPU_NXP_PXP
            if(lv_area_get_size(draw_area) >= LV_GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT) {
                lv_gpu_nxp_pxp_blit(disp_buf_first, disp_w, map_buf_first, map_w, draw_area_w, draw_area_h, opa);
                return;
            }
#elif (LV_USE_GPU_NXP_VG_LITE)
            if(lv_area_get_size(draw_area) >= LV_GPU_NXP_VG_LITE_BLIT_OPA_SIZE_LIMIT) {

                lv_gpu_nxp_vglite_blit_info_t blit;

                blit.src = map_buf;
                blit.src_width = lv_area_get_width(map_area);
                blit.src_height = lv_area_get_height(map_area);
                blit.src_stride = lv_area_get_width(map_area) * sizeof(lv_color_t);
                blit.src_area.x1 = (draw_area->x1 - (map_area->x1 - disp_area->x1));
                blit.src_area.y1 = (draw_area->y1 - (map_area->y1 - disp_area->y1));
                blit.src_area.x2 = blit.src_area.x1 + draw_area_w;
                blit.src_area.y2 = blit.src_area.y1 + draw_area_h;


                blit.dst = disp_buf;
                blit.dst_width = lv_area_get_width(disp_area);
                blit.dst_height = lv_area_get_height(disp_area);
                blit.dst_stride = lv_area_get_width(disp_area) * sizeof(lv_color_t);
                blit.dst_area.x1 = draw_area->x1;
                blit.dst_area.y1 = draw_area->y1;
                blit.dst_area.x2 = blit.dst_area.x1 + draw_area_w;
                blit.dst_area.y2 = blit.dst_area.y1 + draw_area_h;

                blit.opa = opa;

                if(lv_gpu_nxp_vglite_blit(&blit) == LV_RES_OK) {
                    return;
                }
                /* Fall down to SW render in case of error */
            }
#elif LV_USE_GPU_STM32_DMA2D
            if(lv_area_get_size(draw_area) >= 240) {
                lv_gpu_stm32_dma2d_blend(disp_buf_first, disp_w, map_buf_first, opa, map_w, draw_area_w, draw_area_h);
                return;
            }
#endif

            /*Software rendering*/

            for(y = 0; y < draw_area_h; y++) {
                for(x = 0; x < draw_area_w; x++) {
#if LV_COLOR_SCREEN_TRANSP
                    if(disp->driver.screen_transp) {
                        lv_color_mix_with_alpha(disp_buf_first[x], disp_buf_first[x].ch.alpha, map_buf_first[x], opa, &disp_buf_first[x],
                                                &disp_buf_first[x].ch.alpha);
                    }
                    else
#endif
                    {
                        disp_buf_first[x] = lv_color_mix(map_buf_first[x], disp_buf_first[x], opa);
                    }
                }
                disp_buf_first += disp_w;
                map_buf_first += map_w;
            }
        }
    }
    /*Masked*/
    else {
        /*Only the mask matters*/
        if(opa > LV_OPA_MAX) {
            /*Go to the first pixel of the row */

            int32_t x_end4 = draw_area_w - 4;

            for(y = 0; y < draw_area_h; y++) {
                const lv_opa_t * mask_tmp_x = mask;
#if 0
                for(x = 0; x < draw_area_w; x++) {
                    MAP_NORMAL_MASK_PX(x);
                }
#else
                for(x = 0; x < draw_area_w && ((lv_uintptr_t)mask_tmp_x & 0x3); x++) {
#if LV_COLOR_SCREEN_TRANSP
                    MAP_NORMAL_MASK_PX_SCR_TRANSP(x)
#else
                    MAP_NORMAL_MASK_PX(x)
#endif
                }

                uint32_t * mask32 = (uint32_t *) mask_tmp_x;
                for(; x < x_end4; x += 4) {
                    if(*mask32) {
                        if((*mask32) == 0xFFFFFFFF) {
                            disp_buf_first[x] = map_buf_first[x];
                            disp_buf_first[x + 1] = map_buf_first[x + 1];
                            disp_buf_first[x + 2] = map_buf_first[x + 2];
                            disp_buf_first[x + 3] = map_buf_first[x + 3];
                        }
                        else {
                            mask_tmp_x = (const lv_opa_t *)mask32;
#if LV_COLOR_SCREEN_TRANSP
                            MAP_NORMAL_MASK_PX_SCR_TRANSP(x)
                            MAP_NORMAL_MASK_PX_SCR_TRANSP(x + 1)
                            MAP_NORMAL_MASK_PX_SCR_TRANSP(x + 2)
                            MAP_NORMAL_MASK_PX_SCR_TRANSP(x + 3)
#else
                            MAP_NORMAL_MASK_PX(x)
                            MAP_NORMAL_MASK_PX(x + 1)
                            MAP_NORMAL_MASK_PX(x + 2)
                            MAP_NORMAL_MASK_PX(x + 3)
#endif
                        }
                    }
                    mask32++;
                }

                mask_tmp_x = (const lv_opa_t *)mask32;
                for(; x < draw_area_w ; x++) {
#if LV_COLOR_SCREEN_TRANSP
                    MAP_NORMAL_MASK_PX_SCR_TRANSP(x)
#else
                    MAP_NORMAL_MASK_PX(x)
#endif
                }
#endif
                disp_buf_first += disp_w;
                mask += draw_area_w;
                map_buf_first += map_w;
            }
        }
        /*Handle opa and mask values too*/
        else {
            for(y = 0; y < draw_area_h; y++) {
                for(x = 0; x < draw_area_w; x++) {
                    if(mask[x]) {
                        lv_opa_t opa_tmp = mask[x] >= LV_OPA_MAX ? opa : ((opa * mask[x]) >> 8);
#if LV_COLOR_SCREEN_TRANSP
                        if(disp->driver.screen_transp) {
                            lv_color_mix_with_alpha(disp_buf_first[x], disp_buf_first[x].ch.alpha, map_buf_first[x], opa_tmp, &disp_buf_first[x],
                                                    &disp_buf_first[x].ch.alpha);
                        }
                        else
#endif
                        {
                            disp_buf_first[x] = lv_color_mix(map_buf_first[x], disp_buf_first[x], opa_tmp);
                        }
                    }
                }
                disp_buf_first += disp_w;
                mask += draw_area_w;
                map_buf_first += map_w;
            }
        }
    }
}
#if LV_USE_BLEND_MODES
static void map_blended(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
                        const lv_area_t * map_area, const lv_color_t * map_buf, lv_opa_t opa,
                        const lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_blend_mode_t mode)
{

    /*Get the width of the `disp_area` it will be used to go to the next line*/
    int32_t disp_w = lv_area_get_width(disp_area);

    /*Get the width of the `draw_area` it will be used to go to the next line of the mask*/
    int32_t draw_area_w = lv_area_get_width(draw_area);

    /*Get the width of the `mask_area` it will be used to go to the next line*/
    int32_t map_w = lv_area_get_width(map_area);

    /*Create a temp. disp_buf which always point to current line to draw*/
    lv_color_t * disp_buf_tmp = disp_buf + disp_w * draw_area->y1;

    /*Create a temp. map_buf which always point to current line to draw*/
    const lv_color_t * map_buf_tmp = map_buf + map_w * (draw_area->y1 - (map_area->y1 - disp_area->y1));

    lv_color_t (*blend_fp)(lv_color_t, lv_color_t, lv_opa_t);
    switch(mode) {
        case LV_BLEND_MODE_ADDITIVE:
            blend_fp = color_blend_true_color_additive;
            break;
        case LV_BLEND_MODE_SUBTRACTIVE:
            blend_fp = color_blend_true_color_subtractive;
            break;
        default:
            LV_LOG_WARN("fill_blended: unsupported blend mode");
            return;
    }

    int32_t x;
    int32_t y;

    /*Simple fill (maybe with opacity), no masking*/
    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) {
        /*Go to the first px of the row*/
        map_buf_tmp += (draw_area->x1 - (map_area->x1 - disp_area->x1));

        /*The map will be indexed from `draw_area->x1` so compensate it.*/
        map_buf_tmp -= draw_area->x1;

        for(y = draw_area->y1; y <= draw_area->y2; y++) {
            for(x = draw_area->x1; x <= draw_area->x2; x++) {
                disp_buf_tmp[x] = blend_fp(map_buf_tmp[x], disp_buf_tmp[x], opa);
            }
            disp_buf_tmp += disp_w;
            map_buf_tmp += map_w;
        }
    }
    /*Masked*/
    else {
        /* The mask is relative to the clipped area.
         * In the cycles below mask will be indexed from `draw_area.x1`
         * but it corresponds to zero index. So prepare `mask_tmp` accordingly. */
        const lv_opa_t * mask_tmp = mask - draw_area->x1;

        map_buf_tmp -= draw_area->x1;
        for(y = draw_area->y1; y <= draw_area->y2; y++) {
            for(x = draw_area->x1; x <= draw_area->x2; x++) {
                if(mask_tmp[x] == 0) continue;
                lv_opa_t opa_tmp = mask_tmp[x] >= LV_OPA_MAX ? opa : ((opa * mask_tmp[x]) >> 8);
                disp_buf_tmp[x] = blend_fp(map_buf_tmp[x], disp_buf_tmp[x], opa_tmp);
            }
            disp_buf_tmp += disp_w;
            mask_tmp += draw_area_w;
            map_buf_tmp += map_w;
        }
    }
}

static inline lv_color_t color_blend_true_color_additive(lv_color_t fg, lv_color_t bg, lv_opa_t opa)
{

    if(opa <= LV_OPA_MIN) return bg;

    uint32_t tmp;
#if LV_COLOR_DEPTH == 1
    tmp = bg.full + fg.full;
    fg.full = LV_MATH_MIN(tmp, 1);
#else
    tmp = bg.ch.red + fg.ch.red;
#if LV_COLOR_DEPTH == 8
    fg.ch.red = LV_MATH_MIN(tmp, 7);
#elif LV_COLOR_DEPTH == 16
    fg.ch.red = LV_MATH_MIN(tmp, 31);
#elif LV_COLOR_DEPTH == 32
    fg.ch.red = LV_MATH_MIN(tmp, 255);
#endif

#if LV_COLOR_DEPTH == 8
    fg.ch.green = LV_MATH_MIN(tmp, 7);
#elif LV_COLOR_DEPTH == 16
#if LV_COLOR_16_SWAP == 0
    tmp = bg.ch.green + fg.ch.green;
    fg.ch.green = LV_MATH_MIN(tmp, 63);
#else
    tmp = (bg.ch.green_h << 3) + bg.ch.green_l + (fg.ch.green_h << 3) + fg.ch.green_l;
    tmp = LV_MATH_MIN(tmp, 63);
    fg.ch.green_h = tmp >> 3;
    fg.ch.green_l = tmp & 0x7;
#endif

#elif LV_COLOR_DEPTH == 32
    fg.ch.green = LV_MATH_MIN(tmp, 255);
#endif

    tmp = bg.ch.blue + fg.ch.blue;
#if LV_COLOR_DEPTH == 8
    fg.ch.blue = LV_MATH_MIN(tmp, 4);
#elif LV_COLOR_DEPTH == 16
    fg.ch.blue = LV_MATH_MIN(tmp, 31);
#elif LV_COLOR_DEPTH == 32
    fg.ch.blue = LV_MATH_MIN(tmp, 255);
#endif
#endif

    if(opa == LV_OPA_COVER) return fg;

    return lv_color_mix(fg, bg, opa);
}

static inline lv_color_t color_blend_true_color_subtractive(lv_color_t fg, lv_color_t bg, lv_opa_t opa)
{

    if(opa <= LV_OPA_MIN) return bg;

    int32_t tmp;
    tmp = bg.ch.red - fg.ch.red;
    fg.ch.red = LV_MATH_MAX(tmp, 0);

#if LV_COLOR_16_SWAP == 0
    tmp = bg.ch.green - fg.ch.green;
    fg.ch.green = LV_MATH_MAX(tmp, 0);
#else
    tmp = (bg.ch.green_h << 3) + bg.ch.green_l + (fg.ch.green_h << 3) + fg.ch.green_l;
    tmp = LV_MATH_MAX(tmp, 0);
    fg.ch.green_h = tmp >> 3;
    fg.ch.green_l = tmp & 0x7;
#endif

    tmp = bg.ch.blue - fg.ch.blue;
    fg.ch.blue = LV_MATH_MAX(tmp, 0);

    if(opa == LV_OPA_COVER) return fg;

    return lv_color_mix(fg, bg, opa);
}
#endif
