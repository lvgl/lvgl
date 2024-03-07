/**
 * @file lv_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"

#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
#if LV_USE_THORVG_EXTERNAL
    #include <thorvg_capi.h>
#else
    #include "../../libs/thorvg/thorvg_capi.h"
#endif
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    float x;
    float y;
    float w;
    float h;
} _tvg_rect;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} _tvg_color;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

static void _lv_area_to_tvg(_tvg_rect * rect, const lv_area_t * area)
{
    rect->x = area->x1;
    rect->y = area->y1;
    rect->w = lv_area_get_width(area);
    rect->h = lv_area_get_height(area);
}

static void _lv_color_to_tvg(_tvg_color * color, const lv_color32_t * c, lv_opa_t opa)
{
    color->r = c->red;
    color->g = c->green;
    color->b = c->blue;
    color->a = LV_OPA_MIX2(c->alpha, opa);
}

static void _lv_matrix_to_tvg(Tvg_Matrix * tm, const lv_matrix_t * m)
{
    tm->e11 = m->m[0][0];
    tm->e12 = m->m[0][1];
    tm->e13 = m->m[0][2];
    tm->e21 = m->m[1][0];
    tm->e22 = m->m[1][1];
    tm->e23 = m->m[1][2];
    tm->e31 = m->m[2][0];
    tm->e32 = m->m[2][1];
    tm->e33 = m->m[2][2];
}

static void _set_paint_matrix(Tvg_Paint * obj, const Tvg_Matrix * m)
{
    tvg_paint_set_transform(obj, m);
}

static void _set_paint_shape(Tvg_Paint * obj, const lv_vector_path_t * p)
{
    uint32_t pidx = 0;
    lv_vector_path_op_t * op = lv_array_front(&p->ops);
    uint32_t size = lv_array_size(&p->ops);
    for(uint32_t i = 0; i < size; i++) {
        switch(op[i]) {
            case LV_VECTOR_PATH_OP_MOVE_TO: {
                    lv_fpoint_t * pt = lv_array_at(&p->points, pidx);
                    tvg_shape_move_to(obj, pt->x, pt->y);
                    pidx += 1;
                }
                break;
            case LV_VECTOR_PATH_OP_LINE_TO: {
                    lv_fpoint_t * pt = lv_array_at(&p->points, pidx);
                    tvg_shape_line_to(obj, pt->x, pt->y);
                    pidx += 1;
                }
                break;
            case LV_VECTOR_PATH_OP_QUAD_TO: {
                    lv_fpoint_t * pt1 = lv_array_at(&p->points, pidx);
                    lv_fpoint_t * pt2 = lv_array_at(&p->points, pidx + 1);

                    lv_fpoint_t * last_pt = lv_array_at(&p->points, pidx - 1);

                    lv_fpoint_t cp[2];
                    cp[0].x = (last_pt->x + 2 * pt1->x) * (1.0f / 3.0f);
                    cp[0].y = (last_pt->y + 2 * pt1->y) * (1.0f / 3.0f);
                    cp[1].x = (pt2->x + 2 * pt1->x) * (1.0f / 3.0f);
                    cp[1].y = (pt2->y + 2 * pt1->y) * (1.0f / 3.0f);

                    tvg_shape_cubic_to(obj, cp[0].x, cp[0].y, cp[1].x, cp[1].y, pt2->x, pt2->y);
                    pidx += 2;
                }
                break;
            case LV_VECTOR_PATH_OP_CUBIC_TO: {
                    lv_fpoint_t * pt1 = lv_array_at(&p->points, pidx);
                    lv_fpoint_t * pt2 = lv_array_at(&p->points, pidx + 1);
                    lv_fpoint_t * pt3 = lv_array_at(&p->points, pidx + 2);

                    tvg_shape_cubic_to(obj, pt1->x, pt1->y, pt2->x, pt2->y, pt3->x, pt3->y);
                    pidx += 3;
                }
                break;
            case LV_VECTOR_PATH_OP_CLOSE: {
                    tvg_shape_close(obj);
                }
                break;
        }
    }
}

static Tvg_Stroke_Cap _lv_stroke_cap_to_tvg(lv_vector_stroke_cap_t cap)
{
    switch(cap) {
        case LV_VECTOR_STROKE_CAP_SQUARE:
            return TVG_STROKE_CAP_SQUARE;
        case LV_VECTOR_STROKE_CAP_ROUND:
            return TVG_STROKE_CAP_ROUND;
        case LV_VECTOR_STROKE_CAP_BUTT:
            return TVG_STROKE_CAP_BUTT;
        default:
            return TVG_STROKE_CAP_SQUARE;
    }
}

static Tvg_Stroke_Join _lv_stroke_join_to_tvg(lv_vector_stroke_join_t join)
{
    switch(join) {
        case LV_VECTOR_STROKE_JOIN_BEVEL:
            return TVG_STROKE_JOIN_BEVEL;
        case LV_VECTOR_STROKE_JOIN_ROUND:
            return TVG_STROKE_JOIN_ROUND;
        case LV_VECTOR_STROKE_JOIN_MITER:
            return TVG_STROKE_JOIN_MITER;
        default:
            return TVG_STROKE_JOIN_BEVEL;
    }
}

static Tvg_Stroke_Fill _lv_spread_to_tvg(lv_vector_gradient_spread_t sp)
{
    switch(sp) {
        case LV_VECTOR_GRADIENT_SPREAD_PAD:
            return TVG_STROKE_FILL_PAD;
        case LV_VECTOR_GRADIENT_SPREAD_REPEAT:
            return TVG_STROKE_FILL_REPEAT;
        case LV_VECTOR_GRADIENT_SPREAD_REFLECT:
            return TVG_STROKE_FILL_REFLECT;
        default:
            return TVG_STROKE_FILL_PAD;
    }
}

static void _setup_gradient(Tvg_Gradient * gradient, const lv_vector_gradient_t * grad,
                            const lv_matrix_t * matrix)
{
    const lv_grad_dsc_t * g = &grad->grad;
    Tvg_Color_Stop * stops = (Tvg_Color_Stop *)lv_malloc(sizeof(Tvg_Color_Stop) * g->stops_count);
    for(uint8_t i = 0; i < g->stops_count; i++) {
        const lv_gradient_stop_t * s = &(g->stops[i]);

        stops[i].offset = s->frac / 255.0f;
        stops[i].r = s->color.red;
        stops[i].g = s->color.green;
        stops[i].b = s->color.blue;
        stops[i].a = s->opa;
    }

    tvg_gradient_set_color_stops(gradient, stops, g->stops_count);
    tvg_gradient_set_spread(gradient, _lv_spread_to_tvg(grad->spread));
    Tvg_Matrix mtx;
    _lv_matrix_to_tvg(&mtx, matrix);
    tvg_gradient_set_transform(gradient, &mtx);
    lv_free(stops);
}

static void _set_paint_stroke_gradient(Tvg_Paint * obj, const lv_vector_gradient_t * g, const lv_matrix_t * m)
{
    float x, y, w, h;
    tvg_paint_get_bounds(obj, &x, &y, &w, &h, false);

    Tvg_Gradient * grad = NULL;
    if(g->style == LV_VECTOR_GRADIENT_STYLE_RADIAL) {
        grad = tvg_radial_gradient_new();
        tvg_radial_gradient_set(grad, g->cx + x, g->cy + y, g->cr);
        _setup_gradient(grad, g, m);
        tvg_shape_set_stroke_radial_gradient(obj, grad);
    }
    else {
        grad = tvg_linear_gradient_new();

        if(g->grad.dir == LV_GRAD_DIR_VER) {
            tvg_linear_gradient_set(grad, x, y, x, y + h);
        }
        else {
            tvg_linear_gradient_set(grad, x, y, x + w, y);
        }

        _setup_gradient(grad, g, m);
        tvg_shape_set_stroke_linear_gradient(obj, grad);
    }
}

static void _set_paint_stroke(Tvg_Paint * obj, const lv_vector_stroke_dsc_t * dsc)
{
    if(dsc->style == LV_VECTOR_DRAW_STYLE_SOLID) {
        _tvg_color c;
        _lv_color_to_tvg(&c, &dsc->color, dsc->opa);
        tvg_shape_set_stroke_color(obj, c.r, c.g, c.b, c.a);
    }
    else {   /*gradient*/
        _set_paint_stroke_gradient(obj, &dsc->gradient, &dsc->matrix);
    }

    tvg_shape_set_stroke_width(obj, dsc->width);
    tvg_shape_set_stroke_miterlimit(obj, dsc->miter_limit);
    tvg_shape_set_stroke_cap(obj, _lv_stroke_cap_to_tvg(dsc->cap));
    tvg_shape_set_stroke_join(obj, _lv_stroke_join_to_tvg(dsc->join));

    if(!lv_array_is_empty(&dsc->dash_pattern)) {
        float * dash_array = lv_array_front(&dsc->dash_pattern);
        tvg_shape_set_stroke_dash(obj, dash_array, dsc->dash_pattern.size);
    }
}

static Tvg_Fill_Rule _lv_fill_rule_to_tvg(lv_vector_fill_t rule)
{
    switch(rule) {
        case LV_VECTOR_FILL_NONZERO:
            return TVG_FILL_RULE_WINDING;
        case LV_VECTOR_FILL_EVENODD:
            return TVG_FILL_RULE_EVEN_ODD;
        default:
            return TVG_FILL_RULE_WINDING;
    }
}

static void _set_paint_fill_gradient(Tvg_Paint * obj, const lv_vector_gradient_t * g, const lv_matrix_t * m)
{
    float x, y, w, h;
    tvg_paint_get_bounds(obj, &x, &y, &w, &h, false);

    Tvg_Gradient * grad = NULL;
    if(g->style == LV_VECTOR_GRADIENT_STYLE_RADIAL) {
        grad = tvg_radial_gradient_new();
        tvg_radial_gradient_set(grad, g->cx + x, g->cy + y, g->cr);
        _setup_gradient(grad, g, m);
        tvg_shape_set_radial_gradient(obj, grad);
    }
    else {
        grad = tvg_linear_gradient_new();

        if(g->grad.dir == LV_GRAD_DIR_VER) {
            tvg_linear_gradient_set(grad, x, y, x, y + h);
        }
        else {
            tvg_linear_gradient_set(grad, x, y, x + w, y);
        }

        _setup_gradient(grad, g, m);
        tvg_shape_set_linear_gradient(obj, grad);
    }
}

static void _set_paint_fill_pattern(Tvg_Paint * obj, Tvg_Canvas * canvas, const lv_draw_image_dsc_t * p,
                                    const lv_matrix_t * m)
{
    lv_image_decoder_dsc_t decoder_dsc;
    lv_image_decoder_args_t args = { 0 };
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, p->src, &args);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to open image");
        return;
    }

    if(!decoder_dsc.decoded) {
        lv_image_decoder_close(&decoder_dsc);
        LV_LOG_ERROR("Image not ready");
        return;
    }

    const uint8_t * src_buf = decoder_dsc.decoded->data;
    const lv_image_header_t * header = &decoder_dsc.decoded->header;
    lv_color_format_t cf = header->cf;

    if(cf != LV_COLOR_FORMAT_ARGB8888) {
        lv_image_decoder_close(&decoder_dsc);
        LV_LOG_ERROR("Not support image format");
        return;
    }

    Tvg_Paint * img = tvg_picture_new();
    tvg_picture_load_raw(img, (uint32_t *)src_buf, header->w, header->h, true);
    Tvg_Paint * clip_path = tvg_paint_duplicate(obj);
    tvg_paint_set_composite_method(img, clip_path, TVG_COMPOSITE_METHOD_CLIP_PATH);
    tvg_paint_set_opacity(img, p->opa);

    Tvg_Matrix mtx;
    _lv_matrix_to_tvg(&mtx, m);
    tvg_paint_set_transform(img, &mtx);
    tvg_canvas_push(canvas, img);
    lv_image_decoder_close(&decoder_dsc);
}

static void _set_paint_fill(Tvg_Paint * obj, Tvg_Canvas * canvas, const lv_vector_fill_dsc_t * dsc,
                            const lv_matrix_t * matrix)
{
    tvg_shape_set_fill_rule(obj, _lv_fill_rule_to_tvg(dsc->fill_rule));

    if(dsc->style == LV_VECTOR_DRAW_STYLE_SOLID) {
        _tvg_color c;
        _lv_color_to_tvg(&c, &dsc->color, dsc->opa);
        tvg_shape_set_fill_color(obj, c.r, c.g, c.b, c.a);
    }
    else if(dsc->style == LV_VECTOR_DRAW_STYLE_PATTERN) {
        float x, y, w, h;
        tvg_paint_get_bounds(obj, &x, &y, &w, &h, false);

        lv_matrix_t imx;
        lv_memcpy(&imx, matrix, sizeof(lv_matrix_t));
        lv_matrix_translate(&imx, x, y);
        lv_matrix_multiply(&imx, &dsc->matrix);
        _set_paint_fill_pattern(obj, canvas, &dsc->img_dsc, &imx);
    }
    else if(dsc->style == LV_VECTOR_DRAW_STYLE_GRADIENT) {
        _set_paint_fill_gradient(obj, &dsc->gradient, &dsc->matrix);
    }
}

static Tvg_Blend_Method _lv_blend_to_tvg(lv_vector_blend_t blend)
{
    switch(blend) {
        case LV_VECTOR_BLEND_SRC_OVER:
            return TVG_BLEND_METHOD_NORMAL;
        case LV_VECTOR_BLEND_SCREEN:
            return TVG_BLEND_METHOD_SCREEN;
        case LV_VECTOR_BLEND_MULTIPLY:
            return TVG_BLEND_METHOD_MULTIPLY;
        case LV_VECTOR_BLEND_NONE:
            return TVG_BLEND_METHOD_SRCOVER;
        case LV_VECTOR_BLEND_ADDITIVE:
            return TVG_BLEND_METHOD_ADD;
        case LV_VECTOR_BLEND_SRC_IN:
        case LV_VECTOR_BLEND_DST_OVER:
        case LV_VECTOR_BLEND_DST_IN:
        case LV_VECTOR_BLEND_SUBTRACTIVE:
        /*not support yet.*/
        default:
            return TVG_BLEND_METHOD_NORMAL;
    }
}

static void _set_paint_blend_mode(Tvg_Paint * obj, lv_vector_blend_t blend)
{
    tvg_paint_set_blend_method(obj, _lv_blend_to_tvg(blend));
}

static void _task_draw_cb(void * ctx, const lv_vector_path_t * path, const lv_vector_draw_dsc_t * dsc)
{
    Tvg_Canvas * canvas = (Tvg_Canvas *)ctx;

    Tvg_Paint * obj = tvg_shape_new();

    if(!path) {  /*clear*/
        _tvg_rect rc;
        _lv_area_to_tvg(&rc, &dsc->scissor_area);

        _tvg_color c;
        _lv_color_to_tvg(&c, &dsc->fill_dsc.color, dsc->fill_dsc.opa);

        Tvg_Matrix mtx = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
        };
        _set_paint_matrix(obj, &mtx);
        tvg_shape_append_rect(obj, rc.x, rc.y, rc.w, rc.h, 0, 0);
        tvg_shape_set_fill_color(obj, c.r, c.g, c.b, c.a);
    }
    else {
        Tvg_Matrix mtx;
        _lv_matrix_to_tvg(&mtx, &dsc->matrix);
        _set_paint_matrix(obj, &mtx);

        _set_paint_shape(obj, path);

        _set_paint_fill(obj, canvas, &dsc->fill_dsc, &dsc->matrix);
        _set_paint_stroke(obj, &dsc->stroke_dsc);
        _set_paint_blend_mode(obj, dsc->blend_mode);
    }

    tvg_canvas_push(canvas, obj);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_sw_vector(lv_draw_unit_t * draw_unit, const lv_draw_vector_task_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);

    if(dsc->task_list == NULL)
        return;

    lv_layer_t * layer = dsc->base.layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;
    if(draw_buf == NULL)
        return;

    lv_color_format_t cf = draw_buf->header.cf;

    if(cf != LV_COLOR_FORMAT_ARGB8888 && \
       cf != LV_COLOR_FORMAT_XRGB8888) {
        LV_LOG_ERROR("unsupported layer color: %d", cf);
        return;
    }

    void * buf = draw_buf->data;
    int32_t width = lv_area_get_width(&layer->buf_area);
    int32_t height = lv_area_get_height(&layer->buf_area);
    uint32_t stride = draw_buf->header.stride;
    Tvg_Canvas * canvas = tvg_swcanvas_create();
    tvg_swcanvas_set_target(canvas, buf, stride / 4, width, height, TVG_COLORSPACE_ARGB8888);

    lv_ll_t * task_list = dsc->task_list;
    _lv_vector_for_each_destroy_tasks(task_list, _task_draw_cb, canvas);

    if(tvg_canvas_draw(canvas) == TVG_RESULT_SUCCESS) {
        tvg_canvas_sync(canvas);
    }

    tvg_canvas_destroy(canvas);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_SW*/
