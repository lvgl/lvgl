/**
 * @file lv_draw_vg_lite.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"
#include "../sw/lv_draw_sw_private.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_path.h"
#include "lv_vg_lite_utils.h"
#include "lv_vg_lite_decoder.h"
#include "lv_vg_lite_grad.h"
#include "lv_vg_lite_pending.h"
#include "lv_vg_lite_stroke.h"

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t wait_for_finish(lv_draw_unit_t * draw_unit);

static bool is_dest_supported(const lv_layer_t * layer);
static bool can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc);
static bool can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc);
static bool can_draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_box_shadow_dsc_t * dsc);
static bool can_draw_letter(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_letter_dsc_t * dsc);
static bool can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc);
static bool can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool can_draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc);
static bool can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc);
static bool can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_triangle_dsc_t * dsc);
static bool can_draw_mask_rect(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc);
static bool can_draw_blur(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_blur_dsc_t * dsc);
#if LV_USE_VECTOR_GRAPHIC
    static bool can_draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_vector_dsc_t * dsc);
#endif

static int32_t draw_delete(lv_draw_unit_t * draw_unit);

static void draw_event_cb(lv_event_t * e);

static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);
static void draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);
static void draw_letter(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords);
static void draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);
static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
static void draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
static void draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);
static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc);
static void draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_triangle_dsc_t * dsc);
static void draw_mask_rect(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_mask_rect_dsc_t * dsc);
static void draw_blur(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords);
#if LV_USE_VECTOR_GRAPHIC
static void draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_vector_dsc_t * dsc);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_draw_unit_draw_cbs_t draw_cbs = {
    .draw_fill_cb = draw_fill,
    .can_draw_fill_cb = can_draw_fill,
    .draw_border_cb = draw_border,
    .can_draw_border_cb = can_draw_border,
    .draw_box_shadow_cb = draw_box_shadow,
    .can_draw_box_shadow_cb = can_draw_box_shadow,
    .draw_letter_cb = draw_letter,
    .can_draw_letter_cb = can_draw_letter,
    .draw_label_cb = draw_label,
    .can_draw_label_cb = can_draw_label,
    .draw_image_cb = draw_image,
    .can_draw_image_cb = can_draw_image,
    .draw_layer_cb = draw_layer,
    .can_draw_layer_cb = can_draw_layer,
    .draw_arc_cb = draw_arc,
    .can_draw_arc_cb = can_draw_arc,
    .draw_line_cb = draw_line,
    .can_draw_line_cb = can_draw_line,
    .draw_triangle_cb = draw_triangle,
    .can_draw_triangle_cb = can_draw_triangle,
    .draw_mask_rect_cb = draw_mask_rect,
    .can_draw_mask_rect_cb = can_draw_mask_rect,
    .draw_blur_cb = draw_blur,
    .can_draw_blur_cb = can_draw_blur,
#if LV_USE_VECTOR_GRAPHIC
    .draw_vector_cb = draw_vector,
    .can_draw_vector_cb = can_draw_vector,
#endif
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_vg_lite_init(void)
{
#if LV_VG_LITE_USE_GPU_INIT
    extern void gpu_init(void);
    static bool inited = false;
    if(!inited) {
        gpu_init();
        inited = true;
    }
#endif

    lv_vg_lite_dump_info();
    lv_vg_lite_utils_init();

    lv_draw_buf_vg_lite_init_handlers();

    lv_draw_vg_lite_unit_t * unit = lv_draw_create_unit(sizeof(lv_draw_vg_lite_unit_t));
    unit->base_unit.wait_for_finish_cb = wait_for_finish;
    unit->base_unit.delete_cb = draw_delete;
    unit->base_unit.event_cb = draw_event_cb;
    unit->base_unit.draw_cbs = &draw_cbs;
    unit->base_unit.fallback = lv_draw_sw_get_draw_unit();
    unit->base_unit.name = "VG_LITE";

    lv_vg_lite_image_dsc_init(unit);
#if LV_USE_VECTOR_GRAPHIC
    unit->grad_ctx = lv_vg_lite_grad_ctx_create(LV_VG_LITE_GRAD_CACHE_CNT, unit);
    lv_vg_lite_stroke_init(unit, LV_VG_LITE_STROKE_CACHE_CNT);
#endif
    lv_vg_lite_path_init(unit);
    lv_vg_lite_decoder_init();
    lv_draw_vg_lite_label_init(unit);
}

void lv_draw_vg_lite_deinit(void)
{
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool check_image_is_supported(const lv_draw_image_dsc_t * dsc)
{
    return lv_vg_lite_is_src_cf_supported(dsc->header.cf);
}

static bool check_arc_is_supported(const lv_draw_arc_dsc_t * dsc)
{
    if(dsc->img_src == NULL) {
        return true;
    }

    lv_image_header_t header;
    lv_result_t res = lv_image_decoder_get_info(dsc->img_src, &header);
    if(res != LV_RESULT_OK) {
        LV_LOG_TRACE("get image info failed");
        return false;
    }

    return lv_vg_lite_is_src_cf_supported(header.cf);
}

/**
 * Make `layer` the target of the next VG-Lite draws: target buffer, global matrix and scissor
 */
static void draw_begin(lv_draw_vg_lite_unit_t * u, lv_layer_t * layer)
{
    lv_vg_lite_buffer_from_draw_buf(&u->target_buffer, layer->draw_buf);

    /* VG-Lite will output premultiplied image, set the flag correspondingly. */
    lv_draw_buf_set_flag(layer->draw_buf, LV_IMAGE_FLAGS_PREMULTIPLIED);

    vg_lite_identity(&u->global_matrix);
    if(layer->buf_area.x1 || layer->buf_area.y1) {
        vg_lite_translate(-layer->buf_area.x1, -layer->buf_area.y1, &u->global_matrix);
    }

#if LV_DRAW_TRANSFORM_USE_MATRIX
    vg_lite_matrix_t layer_matrix;
    lv_vg_lite_matrix(&layer_matrix, &layer->matrix);
    lv_vg_lite_matrix_multiply(&u->global_matrix, &layer_matrix);
#endif

    if(vg_lite_query_feature(gcFEATURE_BIT_VG_SCISSOR)) {
        /* Crop out extra pixels drawn due to scaling accuracy issues */
        lv_area_t scissor_area = layer->phy_clip_area;
        lv_area_move(&scissor_area, -layer->buf_area.x1, -layer->buf_area.y1);
        lv_vg_lite_set_scissor_area(u, &scissor_area);
    }
}

static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_fill(draw_unit, layer, clip_area, dsc, coords);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_border(draw_unit, layer, clip_area, dsc, coords);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

static void draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_box_shadow(draw_unit, layer, clip_area, dsc, coords);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

static void draw_letter(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_letter(draw_unit, layer, clip_area, dsc, coords);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

static void draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_label(draw_unit, layer, clip_area, dsc, coords);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_img(draw_unit, layer, clip_area, dsc, coords, false);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

static void draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_layer(draw_unit, layer, clip_area, dsc, coords);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

static void draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_arc(draw_unit, layer, clip_area, dsc, coords);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_line_iterate(layer, clip_area, dsc, lv_draw_vg_lite_line, draw_unit);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

static void draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_triangle_dsc_t * dsc)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_triangle(draw_unit, layer, clip_area, dsc);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

static void draw_mask_rect(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_mask_rect_dsc_t * dsc)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_mask_rect(draw_unit, layer, clip_area, dsc);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

static void draw_blur(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_blur(draw_unit, layer, clip_area, dsc, coords);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}

#if LV_USE_VECTOR_GRAPHIC
static void draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_vector_dsc_t * dsc)
{
    draw_begin((lv_draw_vg_lite_unit_t *)draw_unit, layer);
    lv_draw_vg_lite_vector(draw_unit, layer, clip_area, dsc);
    lv_vg_lite_flush((lv_draw_vg_lite_unit_t *)draw_unit);
}
#endif

static int32_t wait_for_finish(lv_draw_unit_t * draw_unit)
{
    /*Wait until the GPU has finished everything submitted so far*/
    lv_vg_lite_finish((lv_draw_vg_lite_unit_t *)draw_unit);
    return 0;
}

static bool is_dest_supported(const lv_layer_t * layer)
{
    return lv_vg_lite_is_dest_cf_supported(layer->color_format);
}

static bool can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return is_dest_supported(layer);
}

static bool can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return is_dest_supported(layer);
}

static bool can_draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_box_shadow_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
#if LV_VG_LITE_USE_BOX_SHADOW
    return is_dest_supported(layer);
#else
    LV_UNUSED(layer);
    return false;
#endif
}

static bool can_draw_letter(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_letter_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return is_dest_supported(layer);
}

static bool can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return is_dest_supported(layer);
}

static bool can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    return is_dest_supported(layer) && check_image_is_supported(dsc);
}

static bool can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return is_dest_supported(layer);
}

static bool can_draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    return is_dest_supported(layer) && check_arc_is_supported(dsc);
}

static bool can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return is_dest_supported(layer);
}

static bool can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_triangle_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return is_dest_supported(layer);
}

static bool can_draw_mask_rect(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return is_dest_supported(layer);
}

static bool can_draw_blur(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_blur_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return is_dest_supported(layer);
}

#if LV_USE_VECTOR_GRAPHIC
static bool can_draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_vector_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    return is_dest_supported(layer);
}
#endif

static int32_t draw_delete(lv_draw_unit_t * draw_unit)
{
    lv_draw_vg_lite_unit_t * unit = (lv_draw_vg_lite_unit_t *)draw_unit;

    lv_vg_lite_image_dsc_deinit(unit);
#if LV_USE_VECTOR_GRAPHIC
    lv_vg_lite_grad_ctx_delete(unit->grad_ctx);
    lv_vg_lite_stroke_deinit(unit);
#endif
    lv_vg_lite_path_deinit(unit);
    lv_vg_lite_decoder_deinit();
    lv_draw_vg_lite_label_deinit(unit);
    return 1;
}

static void draw_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    switch(code) {
        case LV_EVENT_CANCEL: {
#if LV_USE_VECTOR_GRAPHIC
                /**
                 * Because VG-Lite will deinitialize the context (including the GPU independent heap)
                 * before the GPU goes to sleep, it is necessary to first discard and dereference
                 * all caches that depend on the independent heap.
                 */
                lv_draw_vg_lite_unit_t * unit = lv_event_get_current_target(e);
                lv_cache_drop_all(lv_vg_lite_grad_ctx_get_cache(unit->grad_ctx), NULL);
                lv_cache_drop_all(unit->stroke_cache, NULL);
                LV_LOG_INFO("dropt all cache");
#endif
            }
            break;
        case LV_EVENT_FOCUSED:
            lv_vg_lite_set_dump_param_enable(true);
            break;
        case LV_EVENT_DEFOCUSED:
            lv_vg_lite_set_dump_param_enable(false);
            break;
        case LV_EVENT_HIT_TEST:
            lv_vg_lite_dump_info();
            break;
        default:
            break;
    }
}

#endif /*LV_USE_DRAW_VG_LITE*/
