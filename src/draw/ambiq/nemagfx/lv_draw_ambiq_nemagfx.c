/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_ambiq_nemagfx.h"

#if LV_USE_GPU_AMBIQ_NEMAGFX

#include "../../sw/lv_draw_sw.h"
#include "nema_core.h"
#include "apollo4x_nema.h"

/*********************
 *      DEFINES
 *********************/

#define NEMA_COLOR_INVALID (NEMA_DITHER | NEMA_FORMAT_MASK)
#define NEMA_COLOR(cf)  (cf == LV_COLOR_FORMAT_XRGB8888) ? NEMA_BGRX8888 : \
    (cf == LV_COLOR_FORMAT_ARGB8888) ? NEMA_BGRA8888 : \
    (cf == LV_COLOR_FORMAT_RGB888) ? NEMA_BGR24 : \
    (cf == LV_COLOR_FORMAT_RGB565 || cf == LV_COLOR_FORMAT_RGB565A8) ? NEMA_RGB565 : \
    (cf == LV_COLOR_FORMAT_A8) ? NEMA_A8 : \
    (cf == LV_COLOR_FORMAT_A4) ? NEMA_A4 : \
    (cf == LV_COLOR_FORMAT_A2) ? NEMA_A2 : \
    (cf == LV_COLOR_FORMAT_A1) ? NEMA_A1 : \
    (cf == LV_COLOR_FORMAT_I8 || cf == LV_COLOR_FORMAT_L8) ? NEMA_L8 : \
    (cf == LV_COLOR_FORMAT_I4) ? NEMA_L4 : \
    (cf == LV_COLOR_FORMAT_I2) ? NEMA_L2 : \
    (cf == LV_COLOR_FORMAT_I1) ? NEMA_L1 : NEMA_COLOR_INVALID

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    struct _lv_draw_task_t * task_act;
    nema_cmdlist_t cl;
#if LV_USE_OS
    lv_thread_sync_t sync;
    lv_thread_t thread;
#endif
    uint32_t idx;
} lv_draw_nema_unit_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline uint32_t lv_color32_swaprb(lv_color32_t c);
static int32_t lv_draw_nema_dispatch(lv_draw_unit_t * draw_unit,
                                     lv_layer_t * layer);
static void execute_drawing(lv_draw_nema_unit_t * u);
static void lv_draw_nema_fill(lv_draw_unit_t * draw_unit,
                              const lv_draw_fill_dsc_t * dsc,
                              const lv_area_t * coords);
static void lv_draw_nema_border(lv_draw_unit_t * draw_unit,
                                const lv_draw_border_dsc_t * dsc,
                                const lv_area_t * coords);
static void lv_draw_nema_img(lv_draw_unit_t * draw_unit,
                             const lv_draw_img_dsc_t * dsc,
                             const lv_area_t * coords);

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Drawing alpha only images needs one byte of 0xFF in SSRAM.
 * If no such place then allocate one during initialization.
 */
#ifndef NEMA_CONST_TEX
    static uint8_t * g_tex;
    #define G_TEX ((uintptr_t)g_tex)
#else
    #define G_TEX NEMA_CONST_TEX
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_ambiq_nemagfx_init(void)
{
    lv_draw_nema_unit_t * u = lv_draw_create_unit(sizeof(lv_draw_nema_unit_t));
    u->base_unit.dispatch = lv_draw_nema_dispatch;
    u->cl = nema_cl_create();
#ifndef NEMA_CONST_TEX
    g_tex = nema_malloc(sizeof(*g_tex));
    *g_tex = ~0;
#endif
}

void lv_draw_ambiq_nemagfx_deinit(void)
{
#ifndef NEMA_CONST_TEX
    if(g_tex) nema_free(g_tex);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline uint32_t lv_color32_swaprb(lv_color32_t c)
{
    c.red ^= c.blue;
    c.blue ^= c.red;
    c.red ^= c.blue;
    return *(uint32_t *)&c;
}

static int32_t lv_draw_nema_dispatch(lv_draw_unit_t * draw_unit,
                                     lv_layer_t * layer)
{
    lv_draw_nema_unit_t * u = (lv_draw_nema_unit_t *) draw_unit;

    /*Wait for command to finish if it's busy with previous draw task*/
    if(u->task_act) {
        nema_cl_wait(&u->cl);
        nema_cl_rewind(&u->cl);
        u->task_act->state = LV_DRAW_TASK_STATE_READY;
        u->task_act = NULL;
    }

    lv_draw_task_t * t = NULL;
    t = lv_draw_get_next_available_task(layer, NULL);
    if(t == NULL) return -1;

    /*If the buffer of the layer is not allocated yet, allocate it now*/
    if(layer->buf == NULL) {
        uint32_t px_size = lv_color_format_get_size(layer->color_format);
        uint32_t layer_size_byte = lv_area_get_size(&layer->buf_area) * px_size;

        uint8_t * buf = lv_malloc(layer_size_byte);
        if(buf == NULL) {
            LV_LOG_WARN("Allocating %"LV_PRIu32" bytes of layer buffer failed. Try later",
                        layer_size_byte);
            return -1;
        }
        LV_ASSERT_MALLOC(buf);
        lv_draw_add_used_layer_size(layer_size_byte < 1024 ? 1 : layer_size_byte >> 10);

        layer->buf = buf;

        if(lv_color_format_has_alpha(layer->color_format)) {
            layer->buffer_clear(layer, &layer->buf_area);
        }
    }

    if(t->type == LV_DRAW_TASK_TYPE_FILL ||
       t->type == LV_DRAW_TASK_TYPE_BORDER ||
       t->type == LV_DRAW_TASK_TYPE_IMAGE) {
        t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
        u->base_unit.target_layer = layer;
        u->base_unit.clip_area = &t->clip_area;
        u->task_act = t;

#if LV_USE_OS
        /*Let the render thread work*/
        lv_thread_sync_signal(&u->sync);
#else
        execute_drawing(u);
#endif
    }

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();

    return 1;
}


static void execute_drawing(lv_draw_nema_unit_t * u)
{
    /*Render the draw task*/
    lv_draw_task_t * t = u->task_act;
    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_nema_fill((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_nema_border((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_nema_img((lv_draw_unit_t *)u, t->draw_dsc, &t->area);
            break;
        default:
            break;
    }
}

static void lv_draw_nema_fill(lv_draw_unit_t * draw_unit,
                              const lv_draw_fill_dsc_t * dsc,
                              const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    lv_draw_nema_unit_t * u = (lv_draw_nema_unit_t *)draw_unit;

    lv_area_t bg_coords;
    lv_area_copy(&bg_coords, coords);

    lv_area_t clipped_coords;
    if(!_lv_area_intersect(&clipped_coords, &bg_coords, draw_unit->clip_area)) return;
    int32_t clipped_w = lv_area_get_width(&clipped_coords);
    int32_t clipped_h = lv_area_get_height(&clipped_coords);

    lv_grad_dir_t grad_dir = dsc->grad.dir;
    lv_color_t bg_color = grad_dir == LV_GRAD_DIR_NONE ? dsc->color :
                          dsc->grad.stops[0].color;
    lv_color32_t bg_color32 = lv_color_to_32(bg_color, dsc->opa);

    /*Get the real radius. Can't be larger than the half of the shortest side*/
    lv_coord_t coords_bg_w = lv_area_get_width(&bg_coords);
    lv_coord_t coords_bg_h = lv_area_get_height(&bg_coords);
    int32_t short_side = LV_MIN(coords_bg_w, coords_bg_h);
    int32_t rout = LV_MIN(dsc->radius, short_side >> 1);

    lv_layer_t * dst_layer = draw_unit->target_layer;
    lv_area_move(&clipped_coords, -dst_layer->buf_area.x1, -dst_layer->buf_area.y1);

    lv_area_move(&bg_coords, -dst_layer->buf_area.x1, -dst_layer->buf_area.y1);
    lv_coord_t dst_w = lv_area_get_width(&dst_layer->buf_area);
    lv_coord_t dst_h = lv_area_get_height(&dst_layer->buf_area);

    nema_cl_bind(&u->cl);
    nema_bind_dst_tex((uintptr_t)dst_layer->buf, dst_w, dst_h,
                      NEMA_COLOR(dst_layer->color_format), -1);
    nema_set_clip(clipped_coords.x1, clipped_coords.y1, clipped_w, clipped_h);
    nema_set_blend_fill(NEMA_BL_SIMPLE);
    if(lv_color_eq(bg_color, dsc->grad.stops[1].color)) grad_dir = LV_GRAD_DIR_NONE;

    if(grad_dir != LV_GRAD_DIR_NONE) {
        bool hor = grad_dir == LV_GRAD_DIR_HOR;
        lv_color32_t c0 = lv_color_to_32(dsc->grad.stops[0].color, 0xFF);
        lv_color32_t c1 = c0;
        color_var_t c0v = { .r = c0.red, .g = c0.green, .b = c0.blue, .a = c0.alpha };
        color_var_t c1v = c0v;
        color_var_t * c2v_p = hor ? &c1v : &c0v;
        lv_coord_t start = hor ? bg_coords.x1 : bg_coords.y1;
        lv_coord_t end = hor ? bg_coords.x2 : bg_coords.y2;
        int32_t delta = end - start;
        lv_coord_t p0 = dsc->grad.stops[0].frac * delta / 255;
        lv_coord_t p1 = p0;
        if(p0 > 0) {
            /*Fill the first part with solid color*/
            nema_fill_rect(bg_coords.x1, bg_coords.y1,
                           hor ? p0 : coords_bg_w,
                           hor ? coords_bg_h : p0,
                           lv_color32_swaprb(c0));
        }
        nema_enable_gradient(1);
        for(size_t i = 1; i < dsc->grad.stops_count; i++) {
            p0 = p1;
            p1 = dsc->grad.stops[i].frac * delta / 255;
            c0 = c1;
            c1 = lv_color_to_32(dsc->grad.stops[i].color, 0xFF);
            c0v = (color_var_t) {
                .r = c0.red, .g = c0.green, .b = c0.blue, .a = c0.alpha
            };
            c1v = (color_var_t) {
                .r = c1.red, .g = c1.green, .b = c1.blue, .a = c1.alpha
            };
            lv_coord_t px = hor ? p0 + start : bg_coords.x1;
            lv_coord_t py = hor ? bg_coords.y1 : p0 + start;
            lv_coord_t pw = hor ? p1 - p0 : coords_bg_w;
            lv_coord_t ph = hor ? coords_bg_h : p1 - p0;
            nema_interpolate_rect_colors(px, py, pw, ph, &c0v, c2v_p, &c1v);
            nema_fill_rect(px, py, pw, ph, 0);
        }
        nema_enable_gradient(0);
        /*Fill the last part (fill a line if last frac == 255)*/
        if(hor) {
            bg_coords.x1 = p1 < delta ? p1 + start : bg_coords.x2;
            coords_bg_w = p1 < delta ? delta - p1 : 1;
        }
        else {
            bg_coords.y1 = p1 < delta ? p1 + start : bg_coords.y2;
            coords_bg_h = p1 < delta ? delta - p1 : 1;
        }
        bg_color32 = c1;
    }

    uint32_t bg_color_argb = lv_color32_swaprb(bg_color32);
    if(dsc->radius != 0) {
        nema_fill_rounded_rect(bg_coords.x1, bg_coords.y1, coords_bg_w, coords_bg_h,
                               rout, bg_color_argb);
    }
    else {
        nema_fill_rect(bg_coords.x1, bg_coords.y1, coords_bg_w, coords_bg_h, bg_color_argb);
    }
    nema_cl_submit(&u->cl);
}

static void lv_draw_nema_border(lv_draw_unit_t * draw_unit,
                                const lv_draw_border_dsc_t * dsc,
                                const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->side == LV_BORDER_SIDE_NONE) return;

    lv_draw_nema_unit_t * u = (lv_draw_nema_unit_t *)draw_unit;
    int32_t coords_w = lv_area_get_width(coords);
    int32_t coords_h = lv_area_get_height(coords);
    int32_t rout = dsc->radius;
    int32_t short_side = LV_MIN(coords_w, coords_h);
    int32_t width = dsc->width;
    if(rout > short_side >> 1) rout = short_side >> 1;
    int32_t clip_ofs = LV_MAX(rout, width);

    /*Get the inner area*/
    lv_area_t draw_area;
    lv_area_copy(&draw_area, coords);
    draw_area.x1 += (dsc->side & LV_BORDER_SIDE_LEFT) ? 0 : clip_ofs;
    draw_area.x2 -= (dsc->side & LV_BORDER_SIDE_RIGHT) ? 0 : clip_ofs;
    draw_area.y1 += (dsc->side & LV_BORDER_SIDE_TOP) ? 0 : clip_ofs;
    draw_area.y2 -= (dsc->side & LV_BORDER_SIDE_BOTTOM) ? 0 : clip_ofs;

    lv_area_t clip_area;
    if(!_lv_area_intersect(&clip_area, &draw_area, draw_unit->clip_area)) return;

    lv_layer_t * dst_layer = draw_unit->target_layer;
    lv_coord_t dst_w  = lv_area_get_width(&dst_layer->buf_area);
    lv_coord_t dst_h  = lv_area_get_height(&dst_layer->buf_area);
    lv_coord_t clip_w = lv_area_get_width(&clip_area);
    lv_coord_t clip_h = lv_area_get_height(&clip_area);
    lv_color32_t c32  = lv_color_to_32(dsc->color, dsc->opa);

    nema_cl_bind(&u->cl);
    lv_area_copy(&draw_area, coords);
    lv_coord_t draw_w = lv_area_get_width(&draw_area);
    lv_coord_t draw_h = lv_area_get_height(&draw_area);
    lv_area_move(&draw_area, -dst_layer->buf_area.x1, -dst_layer->buf_area.y1);
    lv_area_move(&clip_area, -dst_layer->buf_area.x1, -dst_layer->buf_area.y1);
    nema_set_clip(clip_area.x1, clip_area.y1, clip_w, clip_h);
    nema_bind_dst_tex((uintptr_t)dst_layer->buf, dst_w, dst_h,
                      NEMA_COLOR(dst_layer->color_format), -1);
    nema_set_blend_fill(NEMA_BL_SIMPLE);
    uint32_t color = lv_color32_swaprb(c32);
    /*NemaGFX only provides API for drawing rounded rect with 1px width, uhhh*/
    for(size_t i = 0; i < width; i++) {
        nema_draw_rounded_rect(draw_area.x1 + i, draw_area.y1 + i,
                               draw_w - i * 2, draw_h - i * 2,
                               LV_MAX(0, rout - i), color);
    }

    nema_cl_submit(&u->cl);
}

static void lv_draw_nema_img(lv_draw_unit_t * draw_unit,
                             const lv_draw_img_dsc_t * dsc,
                             const lv_area_t * coords)
{
    lv_draw_nema_unit_t * u = (lv_draw_nema_unit_t *)draw_unit;

    /*Use the clip area as draw area*/
    lv_area_t blend_area;
    lv_area_copy(&blend_area, draw_unit->clip_area);

    lv_layer_t * dst_layer = draw_unit->target_layer;
    lv_coord_t dst_w = lv_area_get_width(&dst_layer->buf_area);
    lv_coord_t dst_h = lv_area_get_height(&dst_layer->buf_area);
    lv_coord_t blend_w = lv_area_get_width(&blend_area);
    lv_coord_t blend_h = lv_area_get_height(&blend_area);

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -dst_layer->buf_area.x1, -dst_layer->buf_area.y1);
    lv_img_decoder_dsc_t decoder_dsc;
    lv_img_decoder_open(&decoder_dsc, dsc->src, dsc->recolor, -1);
    const uint8_t * src_buf = decoder_dsc.img_data;
    lv_color_format_t cf = decoder_dsc.header.cf;
    bool indexed = LV_COLOR_FORMAT_IS_INDEXED(cf);
    const lv_color32_t * palette = indexed ? decoder_dsc.palette : NULL;

    /* Check if src_buf and palette are in SSRAM or PSRAM */
    if(!nema_capable(src_buf) || (palette && !nema_capable(palette))) {
        LV_LOG_INFO("Image address not supported by NEMA GPU");
        lv_img_decoder_close(&decoder_dsc);
        u->task_act->state = LV_DRAW_TASK_STATE_QUEUED;
        return;
    }
    nema_tex_format_t src_cf = NEMA_COLOR(cf);
    if(src_cf == NEMA_COLOR_INVALID) {
        lv_img_decoder_close(&decoder_dsc);
        u->task_act->state = LV_DRAW_TASK_STATE_QUEUED;
        return;
    }

    uint16_t angle = dsc->angle;
    uint16_t zoom = dsc->zoom;
    lv_point_t pivot = dsc->pivot;
    bool transformed = (angle != 0) || (zoom != LV_ZOOM_NONE);
    lv_area_t map_area;
    lv_area_copy(&map_area, coords);
    lv_area_move(&map_area, -dst_layer->buf_area.x1, -dst_layer->buf_area.y1);
    lv_coord_t map_w = lv_area_get_width(coords);
    lv_coord_t map_h = lv_area_get_height(coords);
    const lv_opa_t * mask_buf = NULL;
    lv_coord_t mask_stride = map_w;
    if(cf == LV_COLOR_FORMAT_RGB565A8) {
        if(!transformed) {
            /* Use 'A' from RGB565A8 */
            mask_buf = src_buf + map_w * map_h * 2;
        }
        else {
            LV_LOG_INFO("BGR565A8 transformed not supported by NEMA GPU");
            lv_img_decoder_close(&decoder_dsc);
            u->task_act->state = LV_DRAW_TASK_STATE_QUEUED;
            return;
        }
    }

    nema_cl_bind(&u->cl);
    nema_set_clip(blend_area.x1, blend_area.y1, blend_w, blend_h);
    uint32_t x_op = NEMA_BLOP_NONE;
    uint32_t const_color = 0;

    if(indexed) {
        nema_bind_lut_tex((uintptr_t)src_buf, map_w, map_h, src_cf, -1, NEMA_FILTER_PS,
                          (uintptr_t)palette, NEMA_BGRA8888);
        x_op |= NEMA_BLOP_LUT;
    }
    else if(cf == LV_COLOR_FORMAT_A8) {
        const_color = lv_color32_swaprb(lv_color_to_32(decoder_dsc.color, 0));
        nema_bind_src_tex(G_TEX, 8, 1, NEMA_L1, -1, NEMA_FILTER_PS);
        nema_set_const_color(const_color);
        x_op |= NEMA_BLOP_MODULATE_RGB;
        mask_buf = src_buf;
    }
    else {
        nema_bind_src_tex((uintptr_t)src_buf, map_w, map_h, src_cf, -1, NEMA_FILTER_PS);
    }
    nema_bind_dst_tex((uintptr_t)dst_layer->buf, dst_w, dst_h,
                      NEMA_COLOR(dst_layer->color_format), -1);
    if(mask_buf) {
        /* Use TEX3 for mask */
        nema_bind_tex(NEMA_TEX3, (uintptr_t)mask_buf, map_w, map_h,
                      NEMA_A8, mask_stride, NEMA_FILTER_PS);
        x_op |= NEMA_BLOP_STENCIL_TXTY;
    }
    if(lv_color_to_int(dsc->chroma_key_color) > 0) {
        nema_set_src_color_key(lv_color32_swaprb(lv_color_to_32(dsc->chroma_key_color, 0xFF)));
        x_op |= NEMA_BLOP_SRC_CKEY;
    }

    if(dsc->recolor_opa > LV_OPA_MIN) {
        if(cf == LV_COLOR_FORMAT_A8) {
            /*Alpha recolor only needs to replace const color*/
            const_color = lv_color32_swaprb(lv_color_to_32(dsc->recolor, dsc->recolor_opa));
        }
        else {
            /* Sc' = Sc * (1 - Ra) + Rc * Ra
             * Dc' = Sc' * Sa + (1 - Sa) * Dc
             *     = Sc * Sa * (1 - Ra) + Rc * Ra * Sa + (1 - Sa) * Dc
             *     = BLIT(Sa [c*(1 - Ra)], 1 - Sa) + Rc * Ra * Sa
             * So recolored image will be drawn in two runs:
             *     a (1 - Ra)-modulated blit and an additive fill(or blit for indexed and BGRA8888).
             */
            lv_opa_t inv_recolor_opa = 255 - dsc->recolor_opa;
            const_color = inv_recolor_opa | inv_recolor_opa << 8 | inv_recolor_opa << 16 |
                          dsc->opa << 24;
        }
        nema_set_const_color(const_color);
        x_op |= NEMA_BLOP_MODULATE_RGB | NEMA_BLOP_MODULATE_A;
    }
    else if(dsc->opa < LV_OPA_COVER) {
        const_color |= dsc->opa << 24;
        nema_set_const_color(const_color);
        x_op |= NEMA_BLOP_MODULATE_A;
    }

    /* Set blend factor */
    nema_set_blend_blit(nema_blending_mode(NEMA_BF_SRCALPHA, NEMA_BF_INVSRCALPHA, x_op));
    if(!transformed) {
        /* Simple blit */
        nema_blit_subrect(blend_area.x1, blend_area.y1, blend_w, blend_h,
                          blend_area.x1 - map_area.x1, blend_area.y1 - map_area.y1);
    }
    else if(zoom == LV_ZOOM_NONE) {
        /* Rotation w/o zoom*/
        nema_blit_rotate_pivot(blend_area.x1 + pivot.x, blend_area.y1 + pivot.y,
                               pivot.x, pivot.y, angle * 0.1f);
    }
    else {
        /* Arbitrary transform with zoom */
        float m[3][3];
        float scale = zoom / 256.0f;
        float x[4] = { map_area.x1, map_area.x2, map_area.x2, map_area.x1 };
        float y[4] = { map_area.y1, map_area.y1, map_area.y2, map_area.y2 };
        pivot.x += map_area.x1;
        pivot.y += map_area.y1;
        nema_mat3x3_load_identity(m);
        nema_mat3x3_translate(m, -pivot.x, -pivot.y);
        nema_mat3x3_scale(m, scale, scale);
        if(angle != 0) {
            nema_mat3x3_rotate(m, angle * 0.1f);
        }
        nema_mat3x3_translate(m, pivot.x, pivot.y);
        nema_mat3x3_mul_vec(m, &x[0], &y[0]);
        nema_mat3x3_mul_vec(m, &x[1], &y[1]);
        nema_mat3x3_mul_vec(m, &x[2], &y[2]);
        nema_mat3x3_mul_vec(m, &x[3], &y[3]);
        nema_blit_quad_fit(x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3]);
    }

    if(dsc->recolor_opa > LV_OPA_MIN && cf != LV_COLOR_FORMAT_A8) {
        /*
         * Recolor 2nd phase: Rc * Ra * Sa. Since there is no good way of extracting Sa,
         * we are ignoring alpha channels in source image (except for RGB565A8) and it
         * causes a little glitch.
         */
        nema_cl_bind(&u->cl);
        nema_set_clip(blend_area.x1, blend_area.y1, blend_w, blend_h);
        lv_opa_t opa2 = LV_UDIV255(dsc->opa * dsc->recolor_opa);
        const_color = lv_color32_swaprb(lv_color_to_32(dsc->recolor, opa2));
        uint32_t x_op = NEMA_BLOP_SRC_PREMULT;
        nema_bind_dst_tex((uintptr_t)dst_layer->buf, dst_w, dst_h,
                          NEMA_COLOR(dst_layer->color_format), -1);
        if(mask_buf) {
            nema_bind_tex(NEMA_TEX3, (uintptr_t)mask_buf, blend_w, blend_h,
                          NEMA_A8, mask_stride, NEMA_FILTER_PS);
            x_op |= NEMA_BLOP_STENCIL_TXTY;
        }
        nema_set_blend_fill(nema_blending_mode(NEMA_BF_ONE, NEMA_BF_ONE, x_op));
        nema_fill_rect(blend_area.x1, blend_area.y1, blend_w, blend_h, const_color);
    }
    nema_cl_submit(&u->cl);
    nema_cl_wait(&u->cl);
    lv_img_decoder_close(&decoder_dsc);
}

#endif /*LV_USE_GPU_AMBIQ_NEMA*/
