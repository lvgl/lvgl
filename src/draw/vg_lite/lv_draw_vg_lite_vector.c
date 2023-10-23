/**
 * @file lv_draw_vg_lite_vector.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_path.h"
#include "lv_vg_lite_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void task_draw_cb(void * ctx, const lv_vector_path_t * path, const lv_vector_draw_dsc_t * dsc);
static void lv_matrix_to_vg(vg_lite_matrix_t * desy, const lv_matrix_t * src);
static void lv_path_to_vg(lv_vg_lite_path_t * dest, const lv_vector_path_t * src);
static vg_lite_blend_t lv_blend_to_vg(lv_vector_blend_t blend);
static vg_lite_fill_t lv_fill_to_vg(lv_vector_fill_t fill_rule);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vg_lite_vector(lv_draw_unit_t * draw_unit, const lv_draw_vector_task_dsc_t * dsc)
{
    if(dsc->task_list == NULL)
        return;

    lv_layer_t * layer = dsc->base.layer;
    if(layer->buf == NULL)
        return;

    _lv_vector_for_each_destroy_tasks(dsc->task_list, task_draw_cb, draw_unit);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void task_draw_cb(void * ctx, const lv_vector_path_t * path, const lv_vector_draw_dsc_t * dsc)
{
    lv_draw_vg_lite_unit_t * u = ctx;
    lv_color_t color = lv_color_make(dsc->fill_dsc.color.red, dsc->fill_dsc.color.green, dsc->fill_dsc.color.blue);
    vg_lite_color_t vg_color = lv_vg_lite_color(color, dsc->fill_dsc.opa, true);

    LV_VG_LITE_ASSERT_BUFFER(&u->target_buffer);

    if(!path) {  // clear
        vg_lite_rectangle_t rect;
        lv_vg_lite_rect(&rect, &dsc->scissor_area);
        LV_VG_LITE_CHECK_ERROR(vg_lite_clear(&u->target_buffer, &rect, vg_color));
    }
    else {
        vg_lite_matrix_t matrix;
        lv_matrix_to_vg(&matrix, &dsc->matrix);
        lv_vg_lite_matrix_multiply(&matrix, &u->global_matrix);

        lv_vg_lite_path_t * lv_vg_path = lv_vg_lite_path_get(u, VG_LITE_FP32);

        lv_path_to_vg(lv_vg_path, path);
        lv_vg_lite_path_end(lv_vg_path);

        vg_lite_path_t * vg_path = lv_vg_lite_path_get_path(lv_vg_path);
        LV_VG_LITE_ASSERT_PATH(vg_path);

        vg_lite_blend_t blend = lv_blend_to_vg(dsc->blend_mode);
        vg_lite_fill_t fill = lv_fill_to_vg(dsc->fill_dsc.fill_rule);

        const void * image_src = dsc->fill_dsc.img_dsc.src;

        bool has_gradient = dsc->fill_dsc.gradient.grad.dir != LV_GRAD_DIR_NONE;

        if(image_src) {
            vg_lite_buffer_t image_buffer;
            if(lv_vg_lite_buffer_load_image(&image_buffer, image_src, lv_color_black())) {
                vg_lite_matrix_t path_matrix;
                vg_lite_identity(&path_matrix);
                LV_VG_LITE_CHECK_ERROR(vg_lite_draw_pattern(
                                           &u->target_buffer,
                                           vg_path,
                                           fill,
                                           &path_matrix,
                                           &image_buffer,
                                           &matrix,
                                           blend,
                                           VG_LITE_PATTERN_COLOR,
                                           vg_color,
                                           VG_LITE_FILTER_BI_LINEAR));
            }
        }
        else if(has_gradient) {
            lv_vg_lite_draw_grad(
                &u->target_buffer,
                vg_path,
                &dsc->scissor_area,
                &dsc->fill_dsc.gradient.grad,
                &matrix,
                fill,
                blend);
        }
        else {
            LV_VG_LITE_CHECK_ERROR(vg_lite_draw(
                                       &u->target_buffer,
                                       vg_path,
                                       fill,
                                       &matrix,
                                       blend,
                                       vg_color));
        }

        lv_vg_lite_path_drop(u, lv_vg_path);
    }
}

static void lv_matrix_to_vg(vg_lite_matrix_t * dest, const lv_matrix_t * src)
{
    lv_memcpy(dest, src, sizeof(vg_lite_matrix_t));
}

static vg_lite_quality_t lv_quality_to_vg(lv_vector_path_quality_t quality)
{
    switch(quality) {
        case LV_VECTOR_PATH_QUALITY_LOW:
            return VG_LITE_LOW;
        case LV_VECTOR_PATH_QUALITY_MEDIUM:
            return VG_LITE_MEDIUM;
        case LV_VECTOR_PATH_QUALITY_HIGH:
            return VG_LITE_HIGH;
        default:
            return VG_LITE_MEDIUM;
    }
}

static void lv_path_to_vg(lv_vg_lite_path_t * dest, const lv_vector_path_t * src)
{
    lv_vg_lite_path_set_quality(dest, lv_quality_to_vg(src->quality));

    /* no bonding box */
    lv_vg_lite_path_set_bonding_box(dest, __FLT_MIN__, __FLT_MIN__, __FLT_MAX__, __FLT_MAX__);
    uint32_t pidx = 0;
    for(uint32_t i = 0; i < src->ops.size; i++) {
        lv_vector_path_op_t * op = LV_ARRAY_GET(&src->ops, i, uint8_t);
        switch(*op) {
            case LV_VECTOR_PATH_OP_MOVE_TO: {
                    lv_fpoint_t * pt = LV_ARRAY_GET(&src->points, pidx, lv_fpoint_t);
                    lv_vg_lite_path_move_to(dest, pt->x, pt->y);
                    pidx += 1;
                }
                break;
            case LV_VECTOR_PATH_OP_LINE_TO: {
                    lv_fpoint_t * pt = LV_ARRAY_GET(&src->points, pidx, lv_fpoint_t);
                    lv_vg_lite_path_line_to(dest, pt->x, pt->y);
                    pidx += 1;
                }
                break;
            case LV_VECTOR_PATH_OP_QUAD_TO: {
                    lv_fpoint_t * pt1 = LV_ARRAY_GET(&src->points, pidx, lv_fpoint_t);
                    lv_fpoint_t * pt2 = LV_ARRAY_GET(&src->points, pidx + 1, lv_fpoint_t);
                    lv_vg_lite_path_quad_to(dest, pt1->x, pt1->y, pt2->x, pt2->y);
                    pidx += 2;
                }
                break;
            case LV_VECTOR_PATH_OP_CUBIC_TO: {
                    lv_fpoint_t * pt1 = LV_ARRAY_GET(&src->points, pidx, lv_fpoint_t);
                    lv_fpoint_t * pt2 = LV_ARRAY_GET(&src->points, pidx + 1, lv_fpoint_t);
                    lv_fpoint_t * pt3 = LV_ARRAY_GET(&src->points, pidx + 2, lv_fpoint_t);
                    lv_vg_lite_path_cubic_to(dest, pt1->x, pt1->y, pt2->x, pt2->y, pt3->x, pt3->y);
                    pidx += 3;
                }
                break;
            case LV_VECTOR_PATH_OP_CLOSE: {
                    lv_vg_lite_path_close(dest);
                }
                break;
        }
    }
}

static vg_lite_blend_t lv_blend_to_vg(lv_vector_blend_t blend)
{
    switch(blend) {
        case LV_VECTOR_BLEND_SRC_OVER:
            return VG_LITE_BLEND_SRC_OVER;
        case LV_VECTOR_BLEND_SCREEN:
            return VG_LITE_BLEND_SCREEN;
        case LV_VECTOR_BLEND_MULTIPLY:
            return VG_LITE_BLEND_MULTIPLY;
        case LV_VECTOR_BLEND_NONE:
            return VG_LITE_BLEND_NONE;
        case LV_VECTOR_BLEND_ADDITIVE:
            return VG_LITE_BLEND_ADDITIVE;
        case LV_VECTOR_BLEND_SRC_IN:
            return VG_LITE_BLEND_SRC_IN;
        case LV_VECTOR_BLEND_DST_OVER:
            return VG_LITE_BLEND_DST_OVER;
        case LV_VECTOR_BLEND_DST_IN:
            return VG_LITE_BLEND_DST_IN;
        case LV_VECTOR_BLEND_SUBTRACTIVE:
            return VG_LITE_BLEND_SUBTRACT;
        default:
            return VG_LITE_BLEND_SRC_OVER;
    }
}

static vg_lite_fill_t lv_fill_to_vg(lv_vector_fill_t fill_rule)
{
    switch(fill_rule) {
        case LV_VECTOR_FILL_NONZERO:
            return VG_LITE_FILL_NON_ZERO;

        case LV_VECTOR_FILL_EVENODD:
            return VG_LITE_FILL_EVEN_ODD;

        default:
            return VG_LITE_FILL_NON_ZERO;
    }
}

#endif /*LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC*/
