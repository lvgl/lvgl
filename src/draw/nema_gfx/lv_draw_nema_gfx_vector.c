/**
 * @file lv_draw_nema_gfx_vector.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nema_gfx.h"
#if LV_USE_NEMA_GFX && LV_USE_VECTOR_GRAPHIC && LV_USE_NEMA_VG

#include "lv_nema_gfx_path.h"
#include "../lv_draw_vector_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_nema_gfx_unit_t * u;
    float rel_translate_x;
    float rel_translate_y;
} ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void task_draw_cb(void * ctx, const lv_vector_path_t * path, const lv_vector_path_ctx_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_nema_gfx_vector(lv_draw_task_t * t, const lv_draw_vector_dsc_t * dsc,
                             const lv_area_t * coords)
{
    ctx_t c;
    c.u = (lv_draw_nema_gfx_unit_t *) t->draw_unit;

    lv_layer_t * layer = t->target_layer;
    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, &t->clip_area);
    lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    c.rel_translate_x = -layer->buf_area.x1;
    c.rel_translate_y = -layer->buf_area.y1;

    nema_set_clip(rel_clip_area.x1, rel_clip_area.y1, lv_area_get_width(&rel_clip_area),
                  lv_area_get_height(&rel_clip_area));

    lv_color_format_t dst_cf = layer->draw_buf->header.cf;
    uint32_t dst_nema_cf = lv_nemagfx_cf_to_nema(dst_cf);

    /* the stride should be computed internally for NEMA_TSC images and images missing a stride value */
    int32_t stride = (dst_cf >= LV_COLOR_FORMAT_NEMA_TSC_START && dst_cf <= LV_COLOR_FORMAT_NEMA_TSC_END) ?
                     -1 : lv_area_get_width(&(layer->buf_area)) * lv_color_format_get_size(dst_cf);

    nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(layer->draw_buf->data), lv_area_get_width(&(layer->buf_area)),
                      lv_area_get_height(&(layer->buf_area)), dst_nema_cf, stride);

    nema_vg_set_blend(NEMA_BL_SRC_OVER | NEMA_BLOP_SRC_PREMULT);

    lv_vector_for_each_destroy_tasks(dsc->task_list, task_draw_cb, &c);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void task_draw_cb(void * ctx, const lv_vector_path_t * path, const lv_vector_path_ctx_t * dsc)
{
    ctx_t * c = ctx;
    lv_draw_nema_gfx_unit_t * u = c->u;

    if(!path) return;

    nema_vg_path_clear(u->path);
    nema_vg_paint_clear(u->paint);

    nema_vg_paint_set_type(u->paint, NEMA_VG_PAINT_COLOR);

    lv_matrix_t matrix = dsc->matrix;
    matrix.m[0][2] += c->rel_translate_x;
    matrix.m[1][2] += c->rel_translate_y;
    nema_vg_path_set_matrix(c->u->path, (void *) &matrix);

    /* the path ops array needs to be translated to nema's opcodes */
    lv_vector_path_op_t * ops = lv_array_front(&path->ops);
    uint32_t op_count = lv_array_size(&path->ops);
    uint8_t * nema_ops = lv_malloc(op_count * sizeof(*nema_ops));
    LV_ASSERT_MALLOC(nema_ops);
    for(uint32_t i = 0; i < op_count; i++) {
        nema_ops[i] = ops[i] == LV_VECTOR_PATH_OP_MOVE_TO  ? NEMA_VG_PRIM_MOVE :
                      ops[i] == LV_VECTOR_PATH_OP_LINE_TO  ? NEMA_VG_PRIM_LINE :
                      ops[i] == LV_VECTOR_PATH_OP_QUAD_TO  ? NEMA_VG_PRIM_BEZIER_QUAD :
                      ops[i] == LV_VECTOR_PATH_OP_CUBIC_TO ? NEMA_VG_PRIM_BEZIER_CUBIC :
                      /*LV_VECTOR_PATH_OP_CLOSE*/            NEMA_VG_PRIM_CLOSE;
    }

    /* the path points array is in the right format for nema to use as-is */
    uint32_t point_count = lv_array_size(&path->points);
    float * points = lv_array_front(&path->points);

    nema_vg_path_set_shape(u->path, op_count, nema_ops, point_count, points);

    nema_vg_set_quality(path->quality == LV_VECTOR_PATH_QUALITY_LOW    ? NEMA_VG_QUALITY_FASTER :
                        path->quality == LV_VECTOR_PATH_QUALITY_MEDIUM ? NEMA_VG_QUALITY_BETTER :
                        /*LV_VECTOR_PATH_QUALITY_HIGH*/                  NEMA_VG_QUALITY_MAXIMUM);

    if(dsc->fill_dsc.opa) {
        nema_vg_set_fill_rule(dsc->fill_dsc.fill_rule == LV_VECTOR_FILL_NONZERO ? NEMA_VG_FILL_NON_ZERO :
                              /*LV_VECTOR_FILL_EVENODD*/                          NEMA_VG_FILL_EVEN_ODD);

        uint32_t nema_dsc_color = nema_rgba(dsc->fill_dsc.color.red, dsc->fill_dsc.color.green,
                                            dsc->fill_dsc.color.blue, dsc->fill_dsc.color.alpha);
        nema_vg_paint_set_paint_color(u->paint, nema_dsc_color);

        nema_vg_draw_path(u->path, u->paint);
    }

    if(dsc->stroke_dsc.opa) {
        nema_vg_set_fill_rule(NEMA_VG_STROKE);
        nema_vg_stroke_set_width(dsc->stroke_dsc.width);
        uint8_t cap = dsc->stroke_dsc.cap == LV_VECTOR_STROKE_CAP_BUTT   ? NEMA_VG_CAP_BUTT :
                      dsc->stroke_dsc.cap == LV_VECTOR_STROKE_CAP_SQUARE ? NEMA_VG_CAP_SQUARE :
                      /*LV_VECTOR_STROKE_CAP_ROUND*/                       NEMA_VG_CAP_ROUND;
        nema_vg_stroke_set_cap_style(cap, cap);
        uint8_t join = dsc->stroke_dsc.join == LV_VECTOR_STROKE_JOIN_MITER ? NEMA_VG_JOIN_MITER :
                       dsc->stroke_dsc.join == LV_VECTOR_STROKE_JOIN_BEVEL ? NEMA_VG_JOIN_BEVEL :
                       /*LV_VECTOR_STROKE_JOIN_ROUND*/                       NEMA_VG_JOIN_ROUND;
        nema_vg_stroke_set_join_style(join);
        nema_vg_stroke_set_miter_limit(dsc->stroke_dsc.miter_limit);

        uint32_t nema_dsc_color = nema_rgba(dsc->stroke_dsc.color.red, dsc->stroke_dsc.color.green,
                                            dsc->stroke_dsc.color.blue, dsc->stroke_dsc.color.alpha);
        nema_vg_paint_set_paint_color(u->paint, nema_dsc_color);

        nema_vg_draw_path(u->path, u->paint);
    }

    nema_cl_submit(&c->u->cl);
    nema_cl_wait(&u->cl);

    lv_free(nema_ops);
}

#endif /*LV_USE_NEMA_GFX && LV_USE_VECTOR_GRAPHIC && LV_USE_NEMA_VG*/
