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

/*********************
 *      DEFINES
 *********************/
#define FILL_DIRECT_LEN     32
#define FILL_DIRECT_MASK    0x1F

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void fill_true_color_normal(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
        lv_color_t color, lv_opa_t opa,
        const lv_opa_t * mask, lv_draw_mask_res_t mask_res);

static void fill_true_color_blended(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
        lv_color_t color, lv_opa_t opa,
        const lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_blend_mode_t mode);



static inline lv_color_t color_blend_true_color_additive(lv_color_t bg, lv_color_t fg, lv_opa_t opa);
static inline lv_color_t color_blend_true_color_subtractive(lv_color_t fg, lv_color_t bg, lv_opa_t opa);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


/**
 *
 * @param disp_area
 * @param clip_area already truncated to disp_arae
 * @param fill_area
 * @param disp_buf
 * @param cf
 * @param color
 * @param mask
 * @param mask_res
 * @param opa
 * @param mode
 */
void lv_blend_fill(const lv_area_t * clip_area, const lv_area_t * fill_area,
        lv_color_t color, lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_opa_t opa,
        lv_blend_mode_t mode)
{
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;
    if(mask_res == LV_DRAW_MASK_RES_FULL_TRANSP) return;

    lv_disp_t * disp = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);
    const lv_area_t * disp_area = &vdb->area;
    lv_color_t * disp_buf = vdb->buf_act;
    lv_img_cf_t cf = LV_IMG_CF_TRUE_COLOR;


    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = lv_area_intersect(&draw_area, clip_area, fill_area);
    if(!is_common) return;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    if(cf == LV_IMG_CF_TRUE_COLOR && mode == LV_BLEND_MODE_NORMAL) {
        fill_true_color_normal(disp_area, disp_buf, &draw_area, color, opa, mask, mask_res);
    }
    else {
        fill_true_color_blended(disp_area, disp_buf, &draw_area, color, opa, mask, mask_res, mode);

    }


}




void lv_blend_map(const lv_area_t * clip_area, const lv_area_t * map_area, const lv_color_t * map_buf,
        const lv_opa_t * mask, lv_draw_mask_res_t mask_res,
        lv_opa_t opa, lv_blend_mode_t mode)
{
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;
    if(mask_res == LV_DRAW_MASK_RES_FULL_TRANSP) return;

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = lv_area_intersect(&draw_area, clip_area, map_area);
    if(!is_common) return;

    lv_disp_t * disp = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);
    const lv_area_t * disp_area = &vdb->area;
    lv_color_t * disp_buf = vdb->buf_act;
    lv_img_cf_t cf = LV_IMG_CF_TRUE_COLOR;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    /*Get the width of the `disp_area` it will be used to go to the next line*/
    lv_coord_t disp_w = lv_area_get_width(disp_area);

    /*Get the width of the `mask_area` it will be used to go to the next line*/
    lv_coord_t map_w = lv_area_get_width(map_area);

    /*Create a temp. disp_buf which always point to current line to draw*/
    lv_color_t * disp_buf_tmp = disp_buf + disp_w * draw_area.y1;

    /*Create a temp. map_buf which always point to current line to draw*/
    const lv_color_t * map_buf_tmp = map_buf + map_w * (draw_area.y1 - (map_area->y1 - disp_area->y1));

    lv_coord_t x;
    lv_coord_t y;

    /*Simple fill (maybe with opacity), no masking*/
    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) {
        /*Go to the first px of the map*/
        map_buf_tmp += (draw_area.x1 - (map_area->x1 - disp_area->x1));
        if(opa > LV_OPA_MAX) {
            lv_coord_t draw_area_w  = lv_area_get_width(&draw_area);

            for(y = draw_area.y1; y <= draw_area.y2; y++) {
                memcpy(&disp_buf_tmp[draw_area.x1], map_buf_tmp, draw_area_w * sizeof(lv_color_t));
                disp_buf_tmp += disp_w;
                map_buf_tmp += map_w;
            }
        }
        else {

        }
    }
    /*Masked*/
    else {
        /*Get the width of the `draw_area` it will be used to go to the next line of the mask*/
        lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

        /* The mask is relative to the clipped area.
         * In the cycles below mask will be indexed from `draw_area.x1`
         * but it corresponds to zero index. So prepare `mask_tmp` accordingly. */
        const lv_opa_t * mask_tmp = mask - draw_area.x1;

        /*Buffer the result color to avoid recalculating the same color*/
        lv_color_t last_dest_color;
        lv_color_t last_map_color;
        lv_color_t last_res_color;
        lv_opa_t last_mask = LV_OPA_TRANSP;
        last_dest_color.full = disp_buf_tmp[0].full;
        last_map_color.full = disp_buf_tmp[0].full;
        last_res_color.full = disp_buf_tmp[0].full;

        /*Only the mask matters*/
        if(opa > LV_OPA_MAX) {
            map_buf_tmp += (draw_area.x1 - (map_area->x1 - disp_area->x1)) - draw_area.x1;
            for(y = draw_area.y1; y <= draw_area.y2; y++) {
                for(x = draw_area.x1; x <= draw_area.x2; x++) {
                    if(mask_tmp[x] == 0) continue;
                    if(mask_tmp[x] != last_mask || last_dest_color.full != disp_buf_tmp[x].full || last_map_color.full != map_buf_tmp[x].full) {
                        if(mask_tmp[x] > LV_OPA_MAX) last_res_color = map_buf_tmp[x];
                        else if(mask_tmp[x] < LV_OPA_MIN) last_res_color = disp_buf_tmp[x];
                        else last_res_color = lv_color_mix(map_buf_tmp[x], disp_buf_tmp[x], mask_tmp[x]);
                        last_mask = mask_tmp[x];
                        last_dest_color.full = disp_buf_tmp[x].full;
                        last_map_color.full = map_buf_tmp[x].full;
                    }
                    disp_buf_tmp[x] = last_res_color;
                }
                disp_buf_tmp += disp_w;
                mask_tmp += draw_area_w;
                map_buf_tmp += map_w;
            }
        }
        /*Handle opa and mask values too*/
        else {

        }
    }


}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void fill_true_color_normal(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
        lv_color_t color, lv_opa_t opa,
        const lv_opa_t * mask, lv_draw_mask_res_t mask_res)
{

    /*Get the width of the `disp_area` it will be used to go to the next line*/
    lv_coord_t disp_w = lv_area_get_width(disp_area);

    /*Create a temp. disp_buf which always point to current line to draw*/
    lv_color_t * disp_buf_tmp = disp_buf + disp_w * draw_area->y1;

    lv_coord_t x;
    lv_coord_t y;

    /*Simple fill (maybe with opacity), no masking*/
    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) {
        if(opa > LV_OPA_MAX) {
            lv_coord_t draw_area_w  = lv_area_get_width(draw_area);
            lv_color_t * disp_buf_tmp_ori =  disp_buf_tmp;

            /*Fill the first line. Use `memcpy` because it's faster then simple value assignment*/
            /*Set the first pixels manually*/
            lv_coord_t direct_fill_end = LV_MATH_MIN(draw_area->x2, draw_area->x1 + FILL_DIRECT_LEN + (draw_area_w & FILL_DIRECT_MASK) - 1);
            for(x = draw_area->x1; x <= direct_fill_end ; x++) {
                disp_buf_tmp[x].full = color.full;
            }

            for(; x <= draw_area->x2; x += FILL_DIRECT_LEN) {
                memcpy(&disp_buf_tmp[x], &disp_buf_tmp[draw_area->x1], FILL_DIRECT_LEN * sizeof(lv_color_t));
            }

            disp_buf_tmp += disp_w;

            for(y = draw_area->y1 + 1; y <= draw_area->y2; y++) {
                memcpy(&disp_buf_tmp[draw_area->x1], &disp_buf_tmp_ori[draw_area->x1], draw_area_w * sizeof(lv_color_t));
                disp_buf_tmp += disp_w;
            }
        }
        else {
            lv_color_t last_dest_color = LV_COLOR_BLACK;
            lv_color_t last_res_color = lv_color_mix(color, last_dest_color, opa);
            for(y = draw_area->y1; y <= draw_area->y2; y++) {
                for(x = draw_area->x1; x <= draw_area->x2; x++) {
                    if(last_dest_color.full != disp_buf_tmp[x].full) {
                        last_dest_color = disp_buf_tmp[x];
                        last_res_color = lv_color_mix(color, disp_buf_tmp[x], opa);
                    }
                    disp_buf_tmp[x] = last_res_color;
                }
                disp_buf_tmp += disp_w;
            }
        }
    }
    /*Masked*/
    else {
        /*Get the width of the `draw_area` it will be used to go to the next line of the mask*/
        lv_coord_t draw_area_w = lv_area_get_width(draw_area);

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

        /*Only the mask matters*/
        if(opa > LV_OPA_MAX) {
            for(y = draw_area->y1; y <= draw_area->y2; y++) {
                for(x = draw_area->x1; x <= draw_area->x2; x++) {
                    if(mask_tmp[x] == 0) continue;
                    if(mask_tmp[x] != last_mask || last_dest_color.full != disp_buf_tmp[x].full) {
                        if(mask_tmp[x] > LV_OPA_MAX) last_res_color = color;
                        else if(mask_tmp[x] < LV_OPA_MIN) last_res_color = disp_buf_tmp[x];
                        else if(disp_buf_tmp[x].full == color.full) last_res_color = disp_buf_tmp[x];
                        else last_res_color = lv_color_mix(color, disp_buf_tmp[x], mask_tmp[x]);
                        last_mask = mask_tmp[x];
                        last_dest_color.full = disp_buf_tmp[x].full;
                    }
                    disp_buf_tmp[x] = last_res_color;
                }
                disp_buf_tmp += disp_w;
                mask_tmp += draw_area_w;
            }
        }
        /*Handle opa and mask values too*/
        else {
            for(y = draw_area->y1; y <= draw_area->y2; y++) {
                for(x = draw_area->x1; x <= draw_area->x2; x++) {
                    if(mask_tmp[x] == 0) continue;
                    if(mask_tmp[x] != last_mask || last_dest_color.full != disp_buf_tmp[x].full) {
                        lv_opa_t opa_tmp = (uint16_t)((uint16_t)mask_tmp[x] * opa) >> 8;

                        if(opa_tmp > LV_OPA_MAX) last_res_color = lv_color_mix(color, disp_buf_tmp[x], mask_tmp[x]);
                        else if(opa_tmp < LV_OPA_MIN) last_res_color = disp_buf_tmp[x];
                        else if(disp_buf_tmp[x].full == color.full) last_res_color = disp_buf_tmp[x];
                        else last_res_color = lv_color_mix(color, disp_buf_tmp[x],opa_tmp);
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
}


static void fill_true_color_blended(const lv_area_t * disp_area, lv_color_t * disp_buf,  const lv_area_t * draw_area,
        lv_color_t color, lv_opa_t opa,
        const lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_blend_mode_t mode)
{
    /*Get the width of the `disp_area` it will be used to go to the next line*/
    lv_coord_t disp_w = lv_area_get_width(disp_area);

    /*Create a temp. disp_buf which always point to current line to draw*/
    lv_color_t * disp_buf_tmp = disp_buf + disp_w * draw_area->y1;


    lv_color_t (*blend_fp)(lv_color_t, lv_color_t, lv_opa_t);
    switch (mode) {
        case LV_BLEND_MODE_ADDITIVE:
            blend_fp = color_blend_true_color_additive;
            break;
        case LV_BLEND_MODE_SUBTRACTIVE:
            blend_fp = color_blend_true_color_subtractive;
            break;
        default:
            LV_LOG_WARN("fill_true_color_blended: unsupported blend mode");
            return;
            break;
    }

    lv_coord_t x;
    lv_coord_t y;

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
        lv_coord_t draw_area_w = lv_area_get_width(draw_area);

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
                    lv_opa_t opa_tmp = (uint16_t)((uint16_t)mask_tmp[x] * opa) >> 8;

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
#if 0
/**
 * Mix two colors. Both color can have alpha value. It requires ARGB888 colors.
 * @param bg_color background color
 * @param bg_opa alpha of the background color
 * @param fg_color foreground color
 * @param fg_opa alpha of the foreground color
 * @return the mixed color. the alpha channel (color.alpha) contains the result alpha
 */
static inline lv_color_t color_mix_with_alpha(lv_color_t bg_color, lv_opa_t bg_opa, lv_color_t fg_color, lv_opa_t fg_opa)
{
    /* Pick the foreground if it's fully opaque or the Background is fully transparent*/
    if(fg_opa > LV_OPA_MAX || bg_opa <= LV_OPA_MIN) {
        fg_color.ch.alpha = fg_opa;
        return fg_color;
    }
    /*Transparent foreground: use the Background*/
    else if(fg_opa <= LV_OPA_MIN) {
        return bg_color;
    }
    /*Opaque background: use simple mix*/
    else if(bg_opa >= LV_OPA_MAX) {
        return lv_color_mix(fg_color, bg_color, fg_opa);
    }
    /*Both colors have alpha. Expensive calculation need to be applied*/
    else {
        /*Save the parameters and the result. If they will be asked again don't compute again*/
        static lv_opa_t fg_opa_save     = 0;
        static lv_opa_t bg_opa_save     = 0;
        static lv_color_t fg_color_save = {{0}};
        static lv_color_t bg_color_save = {{0}};
        static lv_color_t c             = {{0}};

        if(fg_opa != fg_opa_save || bg_opa != bg_opa_save || fg_color.full != fg_color_save.full ||
                bg_color.full != bg_color_save.full) {
            fg_opa_save        = fg_opa;
            bg_opa_save        = bg_opa;
            fg_color_save.full = fg_color.full;
            bg_color_save.full = bg_color.full;
            /*Info:
             * https://en.wikipedia.org/wiki/Alpha_compositing#Analytical_derivation_of_the_over_operator*/
            lv_opa_t alpha_res = 255 - ((uint16_t)((uint16_t)(255 - fg_opa) * (255 - bg_opa)) >> 8);
            if(alpha_res == 0) {
                while(1)
                    ;
            }
            lv_opa_t ratio = (uint16_t)((uint16_t)fg_opa * 255) / alpha_res;
            c              = lv_color_mix(fg_color, bg_color, ratio);
            c.ch.alpha     = alpha_res;
        }
        return c;
    }
}
#endif

static inline lv_color_t color_blend_true_color_additive(lv_color_t fg, lv_color_t bg, lv_opa_t opa)
{

    if(opa <= LV_OPA_MIN) return bg;

    uint16_t tmp;
    tmp = bg.ch.red + fg.ch.red;
#if LV_COLOR_DEPTH == 8
    fg.ch.red = LV_MATH_MIN(tmp, 7);
#elif LV_COLOR_DEPTH == 16
    fg.ch.red = LV_MATH_MIN(tmp, 31);
#elif LV_COLOR_DEPTH == 32
    fg.ch.red = LV_MATH_MIN(tmp, 255);
#endif

    tmp = bg.ch.green + fg.ch.green;
#if LV_COLOR_DEPTH == 8
    fg.ch.green = LV_MATH_MIN(tmp, 7);
#elif LV_COLOR_DEPTH == 16
#if LV_COLOR_16_SWAP == 0
    fg.ch.green = LV_MATH_MIN(tmp, 63);
#else
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

    if(opa == LV_OPA_COVER) return fg;

    return lv_color_mix(fg, bg, opa);
}

static inline lv_color_t color_blend_true_color_subtractive(lv_color_t fg, lv_color_t bg, lv_opa_t opa)
{

    if(opa <= LV_OPA_MIN) return bg;

    int16_t tmp;
    tmp = bg.ch.red - fg.ch.red;
    fg.ch.red = LV_MATH_MAX(tmp, 0);

    tmp = bg.ch.green - fg.ch.green;
#if LV_COLOR_16_SWAP == 0
    fg.ch.green = LV_MATH_MAX(tmp, 0);
#else
    tmp = LV_MATH_MAX(tmp, 0);
    fg.ch.green_h = tmp >> 3;
    fg.ch.green_l = tmp & 0x7;
#endif

    tmp = bg.ch.blue - fg.ch.blue;
    fg.ch.blue = LV_MATH_MAX(tmp, 0);

    if(opa == LV_OPA_COVER) return fg;

    return lv_color_mix(fg, bg, opa);
}
