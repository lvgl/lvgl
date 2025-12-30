/**
 * @file lv_svg_render.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"
#include "lv_svg_render.h"
#if LV_USE_SVG

#include "../../misc/lv_text_private.h"
#include <math.h>
#include <string.h>

#if LV_USE_FREETYPE
    #include "../../libs/freetype/lv_freetype_private.h"
#endif

/*********************
*      DEFINES
*********************/
#ifndef M_PI
    #define M_PI 3.1415926f
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define ABS(a) fabsf(a)

#define CALC_BOUNDS(p, bounds) \
    do { \
        if((p).x < (bounds).x1) (bounds).x1 = (int32_t)(p).x; \
        if((p).y < (bounds).y1) (bounds).y1 = (int32_t)(p).y; \
        if((p).x > (bounds).x2) (bounds).x2 = (int32_t)(p).x; \
        if((p).y > (bounds).y2) (bounds).y2 = (int32_t)(p).y; \
    } while(0)

#define PCT_TO_PX(v, base) ((v) > 1 ? (v) : ((v) * (base)))

enum {
    _RENDER_NORMAL = 0,
    _RENDER_IN_DEFS = 1,
    _RENDER_IN_GROUP = 2,
};

enum {
    _RENDER_ATTR_FILL         = (4 << 1),
    _RENDER_ATTR_FILL_RULE    = (4 << 2),
    _RENDER_ATTR_FILL_OPACITY = (4 << 3),
    _RENDER_ATTR_STROKE       = (4 << 4),
    _RENDER_ATTR_STROKE_OPACITY = (4 << 5),
    _RENDER_ATTR_STROKE_WIDTH   = (4 << 6),
    _RENDER_ATTR_STROKE_LINECAP = (4 << 7),
    _RENDER_ATTR_STROKE_LINEJOIN    = (4 << 8),
    _RENDER_ATTR_STROKE_MITER_LIMIT = (4 << 9),
    _RENDER_ATTR_STROKE_DASH_ARRAY  = (4 << 10),
};

/**********************
*      TYPEDEFS
**********************/
static void _set_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr);
static void _init_draw_dsc(lv_vector_path_ctx_t * dsc);
static void _deinit_draw_dsc(lv_vector_path_ctx_t * dsc);
static void _copy_draw_dsc(lv_vector_path_ctx_t * dst, const lv_vector_path_ctx_t * src);
static void _prepare_render(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc);
static void _special_render(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc);
#if LV_USE_FREETYPE
    static void _freetype_outline_cb(lv_event_t * e);
#endif

typedef struct {
    lv_svg_render_obj_t base;
    float width;
    float height;
    bool viewport_fill;
} lv_svg_render_viewport_t;

typedef struct {
    lv_svg_render_obj_t base;
    lv_array_t items;
} lv_svg_render_group_t;

typedef struct {
    lv_svg_render_obj_t base;
    float x;
    float y;
    char * xlink;
} lv_svg_render_use_t;

typedef struct {
    lv_svg_render_obj_t base;
    lv_color_t color;
    float opacity;
} lv_svg_render_solid_t;

typedef struct {
    lv_svg_render_obj_t base;
    lv_vector_gradient_t dsc;
    lv_svg_gradient_units_t units;
} lv_svg_render_gradient_t;

typedef struct {
    lv_svg_render_obj_t base;
    float x;
    float y;
    float width;
    float height;
    float rx;
    float ry;
} lv_svg_render_rect_t;

typedef struct {
    lv_svg_render_obj_t base;
    float cx;
    float cy;
    float r;
} lv_svg_render_circle_t;

typedef struct {
    lv_svg_render_obj_t base;
    float cx;
    float cy;
    float rx;
    float ry;
} lv_svg_render_ellipse_t;

typedef struct {
    lv_svg_render_obj_t base;
    float x1;
    float y1;
    float x2;
    float y2;
} lv_svg_render_line_t;

typedef struct {
    lv_svg_render_obj_t base;
    lv_vector_path_t * path;
    lv_area_t bounds;
} lv_svg_render_poly_t;

typedef struct {
    lv_svg_render_obj_t base;
    float x;
    float y;
    float width;
    float height;
    lv_draw_image_dsc_t img_dsc;
    lv_svg_aspect_ratio_t ratio;
} lv_svg_render_image_t;

#if LV_USE_FREETYPE
typedef struct {
    lv_svg_render_obj_t base;
    lv_array_t contents;
    char * family;
    float size;
    lv_freetype_font_style_t style;
    lv_font_t * font;
    float x;
    float y;
    lv_vector_path_t * path;
    lv_area_t bounds;
} lv_svg_render_text_t;

typedef struct _lv_svg_render_content {
    lv_svg_render_obj_t base;
    void (*render_content)(const struct _lv_svg_render_content * content,
                           lv_draw_vector_dsc_t * dsc, lv_matrix_t * matrix);
    uint32_t * letters;
    uint32_t count;
} lv_svg_render_content_t;

typedef struct {
    lv_svg_render_content_t base;
    char * family;
    float size;
    lv_freetype_font_style_t style;
    lv_font_t * font;
    lv_vector_path_t * path;
    lv_area_t bounds;
} lv_svg_render_tspan_t;
#endif

struct _lv_svg_draw_dsc {
    struct _lv_svg_draw_dsc * next;
    lv_vector_path_ctx_t dsc;
    const char * fill_ref;
    const char * stroke_ref;
};

struct _lv_svg_drawing_builder_state {
    const lv_svg_node_t * doc;
    struct _lv_svg_draw_dsc * draw_dsc;
    int in_group_deps;
    bool in_defs;
#if LV_USE_FREETYPE
    bool in_text;
    lv_svg_node_t * cur_text;
#endif
    lv_svg_render_obj_t * list;
    lv_svg_render_obj_t * tail;
};

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_svg_render_hal_t hal_funcs = {NULL, NULL};

/**********************
 *  STATIC PROTOTYPES
 **********************/
void lv_svg_render_init(const lv_svg_render_hal_t * hal)
{
    if(hal) {
        hal_funcs = *hal;
#if LV_USE_FREETYPE
        lv_freetype_outline_add_event(_freetype_outline_cb, LV_EVENT_ALL, NULL);
#endif
    }
}

static struct _lv_svg_draw_dsc * _lv_svg_draw_dsc_create(void)
{
    struct _lv_svg_draw_dsc * dsc = lv_zalloc(sizeof(struct _lv_svg_draw_dsc));
    LV_ASSERT_MALLOC(dsc);
    _init_draw_dsc(&(dsc->dsc));
    return dsc;
}

static void _lv_svg_draw_dsc_delete(struct _lv_svg_draw_dsc * dsc)
{
    while(dsc) {
        struct _lv_svg_draw_dsc * cur = dsc;
        dsc = dsc->next;
        _deinit_draw_dsc(&(cur->dsc));
        lv_free(cur);
    }
}

static struct _lv_svg_draw_dsc * _lv_svg_draw_dsc_push(struct _lv_svg_draw_dsc * dsc)
{
    if(!dsc) return NULL;
    struct _lv_svg_draw_dsc * cur = lv_zalloc(sizeof(struct _lv_svg_draw_dsc));
    LV_ASSERT_MALLOC(cur);
    _copy_draw_dsc(&(cur->dsc), &(dsc->dsc));
    cur->fill_ref = dsc->fill_ref;
    cur->stroke_ref = dsc->stroke_ref;
    cur->next = dsc;
    return cur;
}

static struct _lv_svg_draw_dsc * _lv_svg_draw_dsc_pop(struct _lv_svg_draw_dsc * dsc)
{
    if(!dsc) return NULL;
    struct _lv_svg_draw_dsc * cur = dsc->next;
    lv_free(dsc);
    return cur;
}

static void _set_viewport_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    lv_svg_render_viewport_t * view = (lv_svg_render_viewport_t *)obj;
    switch(attr->id) {
        case LV_SVG_ATTR_WIDTH:
            view->width = attr->value.fval;
            break;
        case LV_SVG_ATTR_HEIGHT:
            view->height = attr->value.fval;
            break;
        case LV_SVG_ATTR_VIEWBOX: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_INITIAL) {
                    float * vals = attr->value.val;
                    float scale_x = 1.0f;
                    float scale_y = 1.0f;
                    float trans_x = vals[0];
                    float trans_y = vals[1];

                    if(view->width > 0 && vals[2] > 0) {
                        scale_x = view->width / vals[2];
                    }
                    if(view->height > 0 && vals[3] > 0) {
                        scale_y = view->height / vals[3];
                    }
                    view->width = scale_x * vals[2];
                    view->height = scale_y * vals[3];

                    lv_matrix_scale(&obj->matrix, scale_x, scale_y);
                    lv_matrix_translate(&obj->matrix, -trans_x, -trans_y);
                }
            }
            break;
        case LV_SVG_ATTR_VIEWPORT_FILL: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_INITIAL
                   && attr->val_type == LV_SVG_ATTR_VALUE_DATA) {
                    dsc->fill_dsc.color = lv_color_to_32(lv_color_hex(attr->value.uval), 0xFF);
                    view->viewport_fill = true;
                }
                else if(attr->class_type == LV_SVG_ATTR_VALUE_NONE) {
                    view->viewport_fill = false;
                }
            }
            break;
        case LV_SVG_ATTR_VIEWPORT_FILL_OPACITY: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_INITIAL) {
                    dsc->fill_dsc.opa = (lv_opa_t)(attr->value.fval * 255.0f);
                }
            }
            break;
    }
}

static void _set_use_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    _set_attr(obj, dsc, attr);
    lv_svg_render_use_t * use = (lv_svg_render_use_t *)obj;
    switch(attr->id) {
        case LV_SVG_ATTR_X:
            use->x = attr->value.fval;
            break;
        case LV_SVG_ATTR_Y:
            use->y = attr->value.fval;
            break;
        case LV_SVG_ATTR_XLINK_HREF: {
                if(use->xlink) lv_free(use->xlink);
                use->xlink = lv_strdup(attr->value.sval);
            }
            break;
    }
}

static void _set_solid_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    LV_UNUSED(dsc);
    lv_svg_render_solid_t * solid = (lv_svg_render_solid_t *)obj;
    switch(attr->id) {
        case LV_SVG_ATTR_SOLID_COLOR:
            solid->color = lv_color_hex(attr->value.uval);
            break;
        case LV_SVG_ATTR_SOLID_OPACITY:
            solid->opacity = attr->value.fval;
            break;
    }
}

static void _set_gradient_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    LV_UNUSED(dsc);
    lv_svg_render_gradient_t * grad = (lv_svg_render_gradient_t *)obj;
    switch(attr->id) {
        case LV_SVG_ATTR_CX:
            grad->dsc.cx = attr->value.fval;
            break;
        case LV_SVG_ATTR_CY:
            grad->dsc.cy = attr->value.fval;
            break;
        case LV_SVG_ATTR_R:
            grad->dsc.cr = attr->value.fval;
            break;
        case LV_SVG_ATTR_X1:
            grad->dsc.x1 = attr->value.fval;
            break;
        case LV_SVG_ATTR_Y1:
            grad->dsc.y1 = attr->value.fval;
            break;
        case LV_SVG_ATTR_X2:
            grad->dsc.x2 = attr->value.fval;
            break;
        case LV_SVG_ATTR_Y2:
            grad->dsc.y2 = attr->value.fval;
            break;
        case LV_SVG_ATTR_GRADIENT_UNITS:
            grad->units = attr->value.ival;
            break;
    }
}

static void _set_rect_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    _set_attr(obj, dsc, attr);
    lv_svg_render_rect_t * rect = (lv_svg_render_rect_t *)obj;
    switch(attr->id) {
        case LV_SVG_ATTR_X:
            rect->x = attr->value.fval;
            break;
        case LV_SVG_ATTR_Y:
            rect->y = attr->value.fval;
            break;
        case LV_SVG_ATTR_WIDTH:
            rect->width = attr->value.fval;
            break;
        case LV_SVG_ATTR_HEIGHT:
            rect->height = attr->value.fval;
            break;
        case LV_SVG_ATTR_RX:
            rect->rx = attr->value.fval;
            break;
        case LV_SVG_ATTR_RY:
            rect->ry = attr->value.fval;
            break;
    }
}

static void _set_circle_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    _set_attr(obj, dsc, attr);
    lv_svg_render_circle_t * circle = (lv_svg_render_circle_t *)obj;
    switch(attr->id) {
        case LV_SVG_ATTR_CX:
            circle->cx = attr->value.fval;
            break;
        case LV_SVG_ATTR_CY:
            circle->cy = attr->value.fval;
            break;
        case LV_SVG_ATTR_R:
            circle->r = attr->value.fval;
            break;
    }
}

static void _set_ellipse_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    _set_attr(obj, dsc, attr);
    lv_svg_render_ellipse_t * ellipse = (lv_svg_render_ellipse_t *)obj;
    switch(attr->id) {
        case LV_SVG_ATTR_CX:
            ellipse->cx = attr->value.fval;
            break;
        case LV_SVG_ATTR_CY:
            ellipse->cy = attr->value.fval;
            break;
        case LV_SVG_ATTR_RX:
            ellipse->rx = attr->value.fval;
            break;
        case LV_SVG_ATTR_RY:
            ellipse->ry = attr->value.fval;
            break;
    }
}

static void _set_line_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    _set_attr(obj, dsc, attr);
    lv_svg_render_line_t * line = (lv_svg_render_line_t *)obj;
    switch(attr->id) {
        case LV_SVG_ATTR_X1:
            line->x1 = attr->value.fval;
            break;
        case LV_SVG_ATTR_Y1:
            line->y1 = attr->value.fval;
            break;
        case LV_SVG_ATTR_X2:
            line->x2 = attr->value.fval;
            break;
        case LV_SVG_ATTR_Y2:
            line->y2 = attr->value.fval;
            break;
    }
}

static void _set_polyline_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    _set_attr(obj, dsc, attr);
    lv_svg_render_poly_t * poly = (lv_svg_render_poly_t *)obj;
    if(attr->id == LV_SVG_ATTR_POINTS) {
        lv_vector_path_clear(poly->path);
        lv_svg_attr_values_list_t * vals = (lv_svg_attr_values_list_t *)(attr->value.val);
        uint32_t len = vals->length;
        lv_svg_point_t * points = (lv_svg_point_t *)(&vals->data);

        CALC_BOUNDS(points[0], poly->bounds);
        lv_fpoint_t pt = {points[0].x, points[0].y};
        lv_vector_path_move_to(poly->path, &pt);
        for(uint32_t i = 1; i < len; i++) {
            pt.x = points[i].x;
            pt.y = points[i].y;
            lv_vector_path_line_to(poly->path, &pt);
            CALC_BOUNDS(pt, poly->bounds);
        }
    }
}

static void _set_polygon_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    _set_polyline_attr(obj, dsc, attr);
    lv_svg_render_poly_t * poly = (lv_svg_render_poly_t *)obj;
    if(attr->id == LV_SVG_ATTR_POINTS) {
        lv_vector_path_close(poly->path);
    }
}

static size_t _get_path_seg_size(uint32_t cmd)
{
    switch(cmd) {
        case LV_SVG_PATH_CMD_MOVE_TO:
        case LV_SVG_PATH_CMD_LINE_TO:
        case LV_SVG_PATH_CMD_CLOSE:
            return sizeof(lv_svg_point_t) + sizeof(uint32_t);
        case LV_SVG_PATH_CMD_QUAD_TO:
            return sizeof(lv_svg_point_t) * 2 + sizeof(uint32_t);
        case LV_SVG_PATH_CMD_CURVE_TO:
            return sizeof(lv_svg_point_t) * 3 + sizeof(uint32_t);
        case LV_SVG_PATH_CMD_ARC_TO:
            return sizeof(lv_svg_point_t) * 4 + sizeof(uint32_t);
        default:
            return 0;
    }
}

static void _set_path_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    _set_attr(obj, dsc, attr);
    lv_svg_render_poly_t * poly = (lv_svg_render_poly_t *)obj;
    if(attr->id == LV_SVG_ATTR_D) {
        lv_vector_path_clear(poly->path);
        lv_svg_attr_values_list_t * vals = (lv_svg_attr_values_list_t *)(attr->value.val);
        uint32_t len = vals->length;
        uint8_t * data_ptr = (uint8_t *)(&vals->data);

        for(uint32_t i = 0; i < len; i++) {
            lv_svg_attr_path_value_t * path_seg = (lv_svg_attr_path_value_t *)data_ptr;
            uint32_t cmd = path_seg->cmd;
            lv_svg_point_t * points = (lv_svg_point_t *)(&path_seg->data);
            switch(cmd) {
                case LV_SVG_PATH_CMD_MOVE_TO: {
                        lv_fpoint_t pt = {points[0].x, points[0].y};
                        lv_vector_path_move_to(poly->path, &pt);
                        CALC_BOUNDS(pt, poly->bounds);
                    }
                    break;
                case LV_SVG_PATH_CMD_LINE_TO: {
                        lv_fpoint_t pt = {points[0].x, points[0].y};
                        lv_vector_path_line_to(poly->path, &pt);
                        CALC_BOUNDS(pt, poly->bounds);
                    }
                    break;
                case LV_SVG_PATH_CMD_QUAD_TO: {
                        lv_fpoint_t pt[2] = {
                            {points[0].x, points[0].y},
                            {points[1].x, points[1].y}
                        };
                        lv_vector_path_quad_to(poly->path, &pt[0], &pt[1]);
                        CALC_BOUNDS(pt[0], poly->bounds);
                        CALC_BOUNDS(pt[1], poly->bounds);
                    }
                    break;
                case LV_SVG_PATH_CMD_CURVE_TO: {
                        lv_fpoint_t pt[3] = {
                            {points[0].x, points[0].y},
                            {points[1].x, points[1].y},
                            {points[2].x, points[2].y}
                        };
                        lv_vector_path_cubic_to(poly->path, &pt[0], &pt[1], &pt[2]);
                        CALC_BOUNDS(pt[0], poly->bounds);
                        CALC_BOUNDS(pt[1], poly->bounds);
                        CALC_BOUNDS(pt[2], poly->bounds);
                    }
                    break;
                case LV_SVG_PATH_CMD_ARC_TO: {
                        lv_fpoint_t pt[4] = {
                            {points[0].x, points[0].y},
                            {points[1].x, points[1].y},
                            {points[2].x, points[2].y},
                            {points[3].x, points[3].y}
                        };
                        lv_vector_path_arc_to(poly->path, pt[0].x, pt[0].y, pt[1].x, pt[2].x > 0, pt[2].y > 0, &pt[3]);
                        lv_vector_path_get_bounding(poly->path, &poly->bounds);
                    }
                    break;
                case LV_SVG_PATH_CMD_CLOSE: {
                        lv_vector_path_close(poly->path);
                    }
                    break;
            }
            size_t mem_inc = _get_path_seg_size(cmd);
            data_ptr += mem_inc;
        }
    }
}

#if LV_USE_FREETYPE
#define SET_FONT_ATTRS(obj, attr) \
    do { \
        switch(attr->id) { \
            case LV_SVG_ATTR_FONT_FAMILY: { \
                    if(attr->val_type == LV_SVG_ATTR_VALUE_PTR) { \
                        if((obj)->font) { \
                            lv_freetype_font_delete((obj)->font); \
                            (obj)->font = NULL; \
                        } \
                        lv_vector_path_clear((obj)->path); \
                        if((obj)->family) lv_free((obj)->family); \
                        (obj)->family = lv_strdup(attr->value.sval); \
                    } \
                } \
                break; \
            case LV_SVG_ATTR_FONT_SIZE: \
                if(attr->class_type == LV_SVG_ATTR_VALUE_INITIAL) { \
                    if(attr->val_type == LV_SVG_ATTR_VALUE_DATA) { \
                        if((obj)->font) { \
                            lv_freetype_font_delete((obj)->font); \
                            (obj)->font = NULL; \
                        } \
                        (obj)->size = attr->value.fval; \
                        lv_vector_path_clear((obj)->path); \
                    } \
                } \
                break; \
            case LV_SVG_ATTR_FONT_STYLE: \
                if(attr->class_type == LV_SVG_ATTR_VALUE_INITIAL) { \
                    if(attr->val_type == LV_SVG_ATTR_VALUE_PTR) { \
                        if((obj)->font) { \
                            lv_freetype_font_delete((obj)->font); \
                            (obj)->font = NULL; \
                        } \
                        lv_vector_path_clear((obj)->path); \
                        if(strncmp(attr->value.sval, "italic", 6) == 0) { \
                            (obj)->style = LV_FREETYPE_FONT_STYLE_ITALIC; \
                        } \
                    } \
                } \
                break; \
            case LV_SVG_ATTR_FONT_WEIGHT: \
                if(attr->class_type == LV_SVG_ATTR_VALUE_INITIAL) { \
                    if(attr->val_type == LV_SVG_ATTR_VALUE_PTR) { \
                        if((obj)->font) { \
                            lv_freetype_font_delete((obj)->font); \
                            (obj)->font = NULL; \
                        } \
                        lv_vector_path_clear((obj)->path); \
                        if(strncmp(attr->value.sval, "bold", 4) == 0) { \
                            (obj)->style = LV_FREETYPE_FONT_STYLE_BOLD; \
                        } \
                    } \
                } \
                break; \
            case LV_SVG_ATTR_FONT_VARIANT: \
                if(attr->class_type == LV_SVG_ATTR_VALUE_INITIAL) { \
                    if(attr->val_type == LV_SVG_ATTR_VALUE_PTR) { \
                        if((obj)->font) { \
                            lv_freetype_font_delete((obj)->font); \
                            (obj)->font = NULL; \
                        } \
                        lv_vector_path_clear((obj)->path); \
                        if(strncmp(attr->value.sval, "small-caps", 10) == 0) { \
                            (obj)->size /= 2; \
                        } \
                    } \
                } \
                break; \
        } \
    } while(0)

static void _set_tspan_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    _set_attr(obj, dsc, attr);
    lv_svg_render_tspan_t * tspan = (lv_svg_render_tspan_t *)obj;

    SET_FONT_ATTRS(tspan, attr);
}

static void _set_text_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    _set_attr(obj, dsc, attr);
    lv_svg_render_text_t * text = (lv_svg_render_text_t *)obj;

    SET_FONT_ATTRS(text, attr);

    switch(attr->id) {
        case LV_SVG_ATTR_X:
            text->x = attr->value.fval;
            break;
        case LV_SVG_ATTR_Y:
            text->y = attr->value.fval;
            break;
    }
}
#endif

static void _set_image_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    _set_attr(obj, dsc, attr);
    lv_svg_render_image_t * image = (lv_svg_render_image_t *)obj;
    switch(attr->id) {
        case LV_SVG_ATTR_X:
            image->x = attr->value.fval;
            break;
        case LV_SVG_ATTR_Y:
            image->y = attr->value.fval;
            break;
        case LV_SVG_ATTR_HEIGHT:
            image->height = attr->value.fval;
            break;
        case LV_SVG_ATTR_WIDTH:
            image->width = attr->value.fval;
            break;
        case LV_SVG_ATTR_OPACITY:
            image->img_dsc.opa = (lv_opa_t)(attr->value.fval * 255.0f);
            break;
        case LV_SVG_ATTR_XLINK_HREF: {
                const char * xlink = attr->value.sval;
                if(hal_funcs.load_image) {
                    hal_funcs.load_image(xlink, &image->img_dsc);
                }
            }
            break;
        case LV_SVG_ATTR_PRESERVE_ASPECT_RATIO: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_INITIAL) {
                    image->ratio = attr->value.uval;
                }
            }
            break;
    }
}

static void _set_attr(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc, const lv_svg_attr_t * attr)
{
    LV_UNUSED(obj);
    switch(attr->id) {
        case LV_SVG_ATTR_FILL: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_NONE) {
                    dsc->fill_dsc.opa = LV_OPA_0;
                    obj->flags |= _RENDER_ATTR_FILL_OPACITY;
                    obj->flags |= _RENDER_ATTR_FILL;
                    return;
                }
                else if(attr->class_type == LV_SVG_ATTR_VALUE_INHERIT) {
                    obj->flags &= ~_RENDER_ATTR_FILL;
                    return;
                }
                else {
                    if(obj->fill_ref) {
                        lv_free(obj->fill_ref);
                        obj->fill_ref = NULL;
                    }
                    if(attr->val_type == LV_SVG_ATTR_VALUE_PTR) {
                        obj->fill_ref = lv_strdup(attr->value.sval);
                    }
                    else {   /* color */
                        dsc->fill_dsc.style = LV_VECTOR_DRAW_STYLE_SOLID;
                        dsc->fill_dsc.color = lv_color_to_32(lv_color_hex(attr->value.uval), 0xFF);
                    }
                    obj->flags |= _RENDER_ATTR_FILL;
                    if(obj->dsc.fill_dsc.opa == LV_OPA_0) {
                        dsc->fill_dsc.opa = LV_OPA_COVER;
                        obj->flags |= _RENDER_ATTR_FILL_OPACITY;
                    }
                }
            }
            break;
        case LV_SVG_ATTR_STROKE: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_NONE) {
                    dsc->stroke_dsc.opa = LV_OPA_0;
                    obj->flags |= _RENDER_ATTR_STROKE_OPACITY;
                    obj->flags |= _RENDER_ATTR_STROKE;
                    return;
                }
                else if(attr->class_type == LV_SVG_ATTR_VALUE_INHERIT) {
                    obj->flags &= ~_RENDER_ATTR_STROKE;
                    return;
                }
                else {
                    if(obj->stroke_ref) {
                        lv_free(obj->stroke_ref);
                        obj->stroke_ref = NULL;
                    }
                    if(attr->val_type == LV_SVG_ATTR_VALUE_PTR) {
                        obj->stroke_ref = lv_strdup(attr->value.sval);
                    }
                    else {   /* color */
                        dsc->stroke_dsc.style = LV_VECTOR_DRAW_STYLE_SOLID;
                        dsc->stroke_dsc.color = lv_color_to_32(lv_color_hex(attr->value.uval), 0xFF);
                    }
                }

                obj->flags |= _RENDER_ATTR_STROKE;
                if(obj->dsc.stroke_dsc.opa == LV_OPA_0) {
                    dsc->stroke_dsc.opa = LV_OPA_COVER;
                    obj->flags |= _RENDER_ATTR_STROKE_OPACITY;
                }
            }
            break;
        case LV_SVG_ATTR_FILL_OPACITY: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_INHERIT) {
                    obj->flags &= ~_RENDER_ATTR_FILL_OPACITY;
                    return;
                }
                dsc->fill_dsc.opa = (lv_opa_t)(attr->value.fval * 255.0f);
                obj->flags |= _RENDER_ATTR_FILL_OPACITY;
            }
            break;
        case LV_SVG_ATTR_STROKE_OPACITY: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_INHERIT) {
                    obj->flags &= ~_RENDER_ATTR_STROKE_OPACITY;
                    return;
                }
                dsc->stroke_dsc.opa = (lv_opa_t)(attr->value.fval * 255.0f);
                obj->flags |= _RENDER_ATTR_STROKE_OPACITY;
            }
            break;
        case LV_SVG_ATTR_FILL_RULE: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_INHERIT) {
                    obj->flags &= ~_RENDER_ATTR_FILL_RULE;
                    return;
                }
                dsc->fill_dsc.fill_rule = attr->value.ival;
                obj->flags |= _RENDER_ATTR_FILL_RULE;
            }
            break;
        case LV_SVG_ATTR_STROKE_WIDTH: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_INHERIT) {
                    obj->flags &= ~_RENDER_ATTR_STROKE_WIDTH;
                    return;
                }
                dsc->stroke_dsc.width = attr->value.fval;
                obj->flags |= _RENDER_ATTR_STROKE_WIDTH;
            }
            break;
        case LV_SVG_ATTR_STROKE_LINECAP: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_INHERIT) {
                    obj->flags &= ~_RENDER_ATTR_STROKE_LINECAP;
                    return;
                }
                dsc->stroke_dsc.cap = attr->value.ival;
                obj->flags |= _RENDER_ATTR_STROKE_LINECAP;
            }
            break;
        case LV_SVG_ATTR_STROKE_LINEJOIN: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_INHERIT) {
                    obj->flags &= ~_RENDER_ATTR_STROKE_LINEJOIN;
                    return;
                }
                dsc->stroke_dsc.join = attr->value.ival;
                obj->flags |= _RENDER_ATTR_STROKE_LINEJOIN;
            }
            break;
        case LV_SVG_ATTR_STROKE_MITER_LIMIT: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_INHERIT) {
                    obj->flags &= ~_RENDER_ATTR_STROKE_MITER_LIMIT;
                    return;
                }
                dsc->stroke_dsc.miter_limit = attr->value.ival;
                obj->flags |= _RENDER_ATTR_STROKE_MITER_LIMIT;
            }
            break;
        case LV_SVG_ATTR_STROKE_DASH_ARRAY: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_NONE) {
                    lv_array_clear(&(dsc->stroke_dsc.dash_pattern));
                    obj->flags |= _RENDER_ATTR_STROKE_DASH_ARRAY;
                    return;
                }
                else if(attr->class_type == LV_SVG_ATTR_VALUE_INHERIT) {
                    obj->flags &= ~_RENDER_ATTR_STROKE_DASH_ARRAY;
                    return;
                }
                else {
                    lv_array_t * dash_array = &(dsc->stroke_dsc.dash_pattern);

                    lv_svg_attr_values_list_t * vals = (lv_svg_attr_values_list_t *)(attr->value.val);
                    uint32_t len = vals->length;
                    float * dashs = (float *)(&vals->data);
                    lv_array_clear(dash_array);

                    obj->flags |= _RENDER_ATTR_STROKE_DASH_ARRAY;
                    if(len) {
                        if(lv_array_capacity(dash_array) == 0) {
                            lv_array_init(dash_array, len, sizeof(float));
                        }
                        else {
                            lv_array_resize(dash_array, len);
                        }
                        for(uint32_t i = 0; i < len; i++) {
                            lv_array_push_back(dash_array, (uint8_t *)(&dashs[i]));
                        }
                    }
                }
            }
            break;
        case LV_SVG_ATTR_TRANSFORM: {
                if(attr->class_type == LV_SVG_ATTR_VALUE_NONE) {
                    return;
                }
                lv_memcpy(&(obj->matrix), attr->value.val, sizeof(lv_matrix_t));
            }
            break;
        case LV_SVG_ATTR_STROKE_DASH_OFFSET:
            /* not support yet */
            break;
    }
}

static void _set_solid_ref(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc,
                           const lv_svg_render_obj_t * target_obj, bool fill)
{
    LV_UNUSED(target_obj);
    lv_svg_render_solid_t * solid = (lv_svg_render_solid_t *)obj;
    if(fill) {
        dsc->fill_dsc.style = LV_VECTOR_DRAW_STYLE_SOLID;
        dsc->fill_dsc.color = lv_color_to_32(solid->color, 0xFF);
        dsc->fill_dsc.opa = (lv_opa_t)(solid->opacity * 255.0f);
    }
    else {
        dsc->stroke_dsc.style = LV_VECTOR_DRAW_STYLE_SOLID;
        dsc->stroke_dsc.color = lv_color_to_32(solid->color, 0xFF);
        dsc->stroke_dsc.opa = (lv_opa_t)(solid->opacity * 255.0f);
    }
}

static void _set_gradient_ref(lv_svg_render_obj_t * obj, lv_vector_path_ctx_t * dsc,
                              const lv_svg_render_obj_t * target_obj, bool fill)
{
    if(!target_obj->clz->get_bounds) {
        return;
    }

    lv_svg_render_gradient_t * grad = (lv_svg_render_gradient_t *)obj;
    lv_vector_gradient_t * grad_dsc = NULL;
    lv_matrix_t * mtx = NULL;

    if(fill) {
        dsc->fill_dsc.style = LV_VECTOR_DRAW_STYLE_GRADIENT;
        grad_dsc = &dsc->fill_dsc.gradient;
        mtx = &dsc->fill_dsc.matrix;
    }
    else {
        dsc->stroke_dsc.style = LV_VECTOR_DRAW_STYLE_GRADIENT;
        grad_dsc = &dsc->stroke_dsc.gradient;
        mtx = &dsc->stroke_dsc.matrix;
    }

    if(grad->units == LV_SVG_GRADIENT_UNITS_USER_SPACE) {
        lv_svg_render_obj_t * list = obj->head;
        while(list) {
            if(list->tag == LV_SVG_TAG_SVG) {
                target_obj = list; /* viewport */
                break;
            }
            list = list->next;
        }
    }

    lv_memcpy(grad_dsc, &grad->dsc, sizeof(lv_vector_gradient_t));

    lv_area_t bounds = {0, 0, 0, 0};
    target_obj->clz->get_bounds(target_obj, &bounds);

    int32_t w = bounds.x2 - bounds.x1;
    int32_t h = bounds.y2 - bounds.y1;
    if(grad->dsc.style == LV_VECTOR_GRADIENT_STYLE_RADIAL) {
        grad_dsc->cx = PCT_TO_PX(grad_dsc->cx, w);
        grad_dsc->cy = PCT_TO_PX(grad_dsc->cy, h);
        grad_dsc->cr = PCT_TO_PX(grad_dsc->cr, MAX(w, h));
        if(grad->units == LV_SVG_GRADIENT_UNITS_OBJECT) {
            lv_matrix_translate(mtx, bounds.x1, bounds.y1);
        }
    }
    else {   /* LV_VECTOR_GRADIENT_STYLE_LINEAR */
        grad_dsc->x1 = PCT_TO_PX(grad_dsc->x1, w);
        grad_dsc->y1 = PCT_TO_PX(grad_dsc->y1, h);
        grad_dsc->x2 = PCT_TO_PX(grad_dsc->x2, w);
        grad_dsc->y2 = PCT_TO_PX(grad_dsc->y2, h);
        if(grad->units == LV_SVG_GRADIENT_UNITS_OBJECT) {
            lv_matrix_translate(mtx, bounds.x1, bounds.y1);
        }
    }
}

static void _init_draw_dsc(lv_vector_path_ctx_t * dsc)
{
    lv_vector_fill_dsc_t * fill_dsc = &(dsc->fill_dsc);
    fill_dsc->style = LV_VECTOR_DRAW_STYLE_SOLID;
    fill_dsc->color = lv_color_to_32(lv_color_black(), 0xFF);
    fill_dsc->opa = LV_OPA_COVER;
    fill_dsc->fill_rule = LV_VECTOR_FILL_NONZERO;
    lv_matrix_identity(&(fill_dsc->matrix)); /* identity matrix */

    lv_vector_stroke_dsc_t * stroke_dsc = &(dsc->stroke_dsc);
    stroke_dsc->style = LV_VECTOR_DRAW_STYLE_SOLID;
    stroke_dsc->color = lv_color_to_32(lv_color_black(), 0xFF);
    stroke_dsc->opa = LV_OPA_0; /* default no stroke */
    stroke_dsc->width = 1.0f;
    stroke_dsc->cap = LV_VECTOR_STROKE_CAP_BUTT;
    stroke_dsc->join = LV_VECTOR_STROKE_JOIN_MITER;
    stroke_dsc->miter_limit = 4.0f;
    lv_matrix_identity(&(stroke_dsc->matrix));

    dsc->blend_mode = LV_VECTOR_BLEND_SRC_OVER;
    lv_matrix_identity(&(dsc->matrix));
}

static void _deinit_draw_dsc(lv_vector_path_ctx_t * dsc)
{
    lv_array_deinit(&(dsc->stroke_dsc.dash_pattern));
}

static void _copy_draw_dsc(lv_vector_path_ctx_t * dst, const lv_vector_path_ctx_t * src)
{
    lv_memcpy(&dst->fill_dsc, &src->fill_dsc, sizeof(lv_vector_fill_dsc_t));

    dst->stroke_dsc.style = src->stroke_dsc.style;
    dst->stroke_dsc.color = src->stroke_dsc.color;
    dst->stroke_dsc.opa = src->stroke_dsc.opa;
    dst->stroke_dsc.width = src->stroke_dsc.width;
    dst->stroke_dsc.cap = src->stroke_dsc.cap;
    dst->stroke_dsc.join = src->stroke_dsc.join;
    dst->stroke_dsc.miter_limit = src->stroke_dsc.miter_limit;
    lv_array_clear(&(dst->stroke_dsc.dash_pattern));
    lv_array_copy(&(dst->stroke_dsc.dash_pattern), &(src->stroke_dsc.dash_pattern));
    lv_memcpy(&(dst->stroke_dsc.gradient), &(src->stroke_dsc.gradient), sizeof(lv_vector_gradient_t));
    lv_memcpy(&(dst->stroke_dsc.matrix), &(src->stroke_dsc.matrix), sizeof(lv_matrix_t));

    dst->blend_mode = src->blend_mode;
}

static void _copy_draw_dsc_from_ref(lv_draw_vector_dsc_t * dsc, const lv_svg_render_obj_t * obj)
{
    lv_vector_path_ctx_t * dst = dsc->ctx;
    if(obj->fill_ref) {
        lv_svg_render_obj_t * list = obj->head;
        while(list) {
            if(list->id) {
                if(strcmp(obj->fill_ref, list->id) == 0) {
                    list->clz->set_paint_ref(list, dst, obj, true);
                    break;
                }
            }
            list = list->next;
        }
    }

    if(obj->stroke_ref) {
        lv_svg_render_obj_t * list = obj->head;
        while(list) {
            if(list->id) {
                if(strcmp(obj->stroke_ref, list->id) == 0) {
                    list->clz->set_paint_ref(list, dst, obj, false);
                    break;
                }
            }
            list = list->next;
        }
    }
}

static void _set_render_attrs(lv_svg_render_obj_t * obj, const lv_svg_node_t * node,
                              struct _lv_svg_drawing_builder_state * state)
{
    obj->tag = node->type;
    if((node->type != LV_SVG_TAG_CONTENT) && node->xml_id) {
        obj->id = lv_strdup(node->xml_id);
    }
    if(obj->clz->init) {
        obj->clz->init(obj, node);
    }
    if(state->draw_dsc->fill_ref) {
        obj->fill_ref = lv_strdup(state->draw_dsc->fill_ref);
    }
    if(state->draw_dsc->stroke_ref) {
        obj->stroke_ref = lv_strdup(state->draw_dsc->stroke_ref);
    }

    uint32_t len = lv_array_size(&node->attrs);
    for(uint32_t i = 0; i < len; i++) {
        lv_svg_attr_t * attr = lv_array_at(&node->attrs, i);
        obj->clz->set_attr(obj, &(state->draw_dsc->dsc), attr);
    }
    if(node->type == LV_SVG_TAG_G) { /* only <g> need store it */
        state->draw_dsc->fill_ref = obj->fill_ref;
        state->draw_dsc->stroke_ref = obj->stroke_ref;
    }
    obj->head = state->list;
}

/* init functions */

static void _init_obj(lv_svg_render_obj_t * obj, const lv_svg_node_t * node)
{
    LV_UNUSED(node);
    lv_matrix_identity(&obj->matrix);
}

static void _init_viewport(lv_svg_render_obj_t * obj, const lv_svg_node_t * node)
{
    _init_obj(obj, node);
    lv_svg_render_viewport_t * view = (lv_svg_render_viewport_t *)obj;
    view->viewport_fill = false;
}

static void _init_group(lv_svg_render_obj_t * obj, const lv_svg_node_t * node)
{
    _init_obj(obj, node);
    lv_svg_render_group_t * group = (lv_svg_render_group_t *)obj;
    lv_array_init(&group->items, LV_TREE_NODE(node)->child_cnt, sizeof(lv_svg_render_obj_t *));
}

static void _init_image(lv_svg_render_obj_t * obj, const lv_svg_node_t * node)
{
    _init_obj(obj, node);
    lv_svg_render_image_t * image = (lv_svg_render_image_t *)obj;
    lv_draw_image_dsc_init(&image->img_dsc);
    image->ratio = LV_SVG_ASPECT_RATIO_XMID_YMID | LV_SVG_ASPECT_RATIO_OPT_MEET;
}

static void _init_poly(lv_svg_render_obj_t * obj, const lv_svg_node_t * node)
{
    _init_obj(obj, node);
    lv_svg_render_poly_t * poly = (lv_svg_render_poly_t *)obj;
    poly->path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_area_set(&poly->bounds, INT_MAX, INT_MAX, INT_MIN, INT_MIN);
}

#if LV_USE_FREETYPE
static void _init_text(lv_svg_render_obj_t * obj, const lv_svg_node_t * node)
{
    _init_obj(obj, node);
    lv_svg_render_text_t * text = (lv_svg_render_text_t *)obj;
    text->family = lv_strdup("sans-serif");
    text->size = 16.0f;
    text->style = LV_FREETYPE_FONT_STYLE_NORMAL;
    text->font = NULL;
    text->x = text->y = 0.0f;
    lv_array_init(&text->contents, LV_TREE_NODE(node)->child_cnt, sizeof(lv_svg_render_obj_t *));
    text->path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
}

static void _init_content(lv_svg_render_obj_t * obj, const lv_svg_node_t * node)
{
    _init_obj(obj, node);
    lv_svg_render_content_t * content = (lv_svg_render_content_t *)obj;
    const char * str = node->xml_id;
    content->count = lv_text_get_encoded_length(str);
    content->letters = lv_malloc(sizeof(uint32_t) * content->count);
    LV_ASSERT_MALLOC(content->letters);
    uint32_t offset = 0;
    for(uint32_t i = 0; i < content->count; i++) {
        content->letters[i] = lv_text_encoded_next(str, &offset);
    }
}

static void _init_tspan(lv_svg_render_obj_t * obj, const lv_svg_node_t * node)
{
    _init_obj(obj, node);
    lv_svg_render_tspan_t * span = (lv_svg_render_tspan_t *)obj;
    lv_svg_node_t * parent = LV_SVG_NODE(LV_TREE_NODE(node)->parent);
    if(parent->type != LV_SVG_TAG_TEXT) {
        return;
    }

    lv_svg_render_text_t * text = (lv_svg_render_text_t *)parent->render_obj;
    span->family = lv_strdup(text->family);
    span->size = text->size;
    span->style = text->style;
    span->path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    lv_svg_node_t * content_node = LV_SVG_NODE(LV_TREE_NODE(node)->children[0]);
    _init_content(obj, content_node);
}
#endif

static void _init_solid(lv_svg_render_obj_t * obj, const lv_svg_node_t * node)
{
    _init_obj(obj, node);
    lv_svg_render_solid_t * solid = (lv_svg_render_solid_t *)obj;
    solid->opacity = 1.0f;
}

static void _init_gradient(lv_svg_render_obj_t * obj, const lv_svg_node_t * node)
{
    _init_obj(obj, node);
    lv_svg_render_gradient_t * grad = (lv_svg_render_gradient_t *)obj;
    grad->units = LV_SVG_GRADIENT_UNITS_OBJECT;
    grad->dsc.cx = 0.5f;
    grad->dsc.cy = 0.5f;
    grad->dsc.cr = 0.5f;
    grad->dsc.x1 = 0.0f;
    grad->dsc.y1 = 0.0f;
    grad->dsc.x2 = 1.0f;
    grad->dsc.y2 = 0.0f;
    grad->dsc.spread = LV_VECTOR_GRADIENT_SPREAD_PAD;

    uint32_t count = LV_TREE_NODE(node)->child_cnt;
    uint32_t stop_count = 0;

    for(uint32_t i = 0; i < count; i++) {
        lv_svg_node_t * child_node = LV_SVG_NODE_CHILD(node, i);
        uint32_t len = lv_array_size(&child_node->attrs);

        bool is_stop = false;
        lv_color_t stop_color = lv_color_black();
        lv_opa_t stop_opa = LV_OPA_COVER;
        uint8_t stop_frac = 0;

        for(uint32_t j = 0; j < len; j++) {
            lv_svg_attr_t * attr = lv_array_at(&child_node->attrs, j);
            switch(attr->id) {
                case LV_SVG_ATTR_GRADIENT_STOP_COLOR: {
                        stop_color = lv_color_hex(attr->value.uval);
                        is_stop = true;
                    }
                    break;
                case LV_SVG_ATTR_GRADIENT_STOP_OPACITY: {
                        stop_opa = (lv_opa_t)(attr->value.fval * 255.0f);
                        is_stop = true;
                    }
                    break;
                case LV_SVG_ATTR_GRADIENT_STOP_OFFSET: {
                        stop_frac = (uint8_t)(attr->value.fval * 255.0f);
                        is_stop = true;
                    }
                    break;
            }
        }

        if(is_stop) {
            grad->dsc.stops[stop_count].opa = stop_opa;
            grad->dsc.stops[stop_count].frac = stop_frac;
            grad->dsc.stops[stop_count].color = stop_color;
            stop_count++;
        }

        if(stop_count == LV_GRADIENT_MAX_STOPS) {
            break;
        }
    }
    grad->dsc.stops_count = stop_count;
}

static void _setup_matrix(lv_matrix_t * matrix, lv_draw_vector_dsc_t * dsc, const lv_svg_render_obj_t * obj)
{
    lv_memcpy(matrix, &dsc->ctx->matrix, sizeof(lv_matrix_t));
    lv_matrix_multiply(&dsc->ctx->matrix, &obj->matrix);
}

static void _restore_matrix(lv_matrix_t * matrix, lv_draw_vector_dsc_t * dsc)
{
    lv_memcpy(&dsc->ctx->matrix, matrix, sizeof(lv_matrix_t));
}

static void _prepare_render(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc)
{
    _copy_draw_dsc(dsc->ctx, &(obj->dsc));
}

static void _special_render(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc)
{
    const lv_vector_path_ctx_t * src = &(obj->dsc);
    lv_vector_path_ctx_t * dst = dsc->ctx;

    if(obj->flags & _RENDER_ATTR_FILL) {
        lv_memcpy(&(dst->fill_dsc), &(src->fill_dsc), sizeof(lv_vector_fill_dsc_t));
        dst->blend_mode = src->blend_mode;
    }

    if(obj->flags & _RENDER_ATTR_FILL_OPACITY) {
        dst->fill_dsc.opa = src->fill_dsc.opa;
    }

    if(obj->flags & _RENDER_ATTR_FILL_RULE) {
        dst->fill_dsc.fill_rule = src->fill_dsc.fill_rule;
    }

    if(obj->flags & _RENDER_ATTR_STROKE) {
        dst->stroke_dsc.style = src->stroke_dsc.style;
        dst->stroke_dsc.color = src->stroke_dsc.color;
        lv_memcpy(&(dst->stroke_dsc.gradient), &(src->stroke_dsc.gradient), sizeof(lv_vector_gradient_t));
        lv_memcpy(&(dst->stroke_dsc.matrix), &(src->stroke_dsc.matrix), sizeof(lv_matrix_t));
        dst->blend_mode = src->blend_mode;
    }

    if(obj->flags & _RENDER_ATTR_STROKE_OPACITY) {
        dst->stroke_dsc.opa = src->stroke_dsc.opa;
    }

    if(obj->flags & _RENDER_ATTR_STROKE_WIDTH) {
        dst->stroke_dsc.width = src->stroke_dsc.width;
    }
    if(obj->flags & _RENDER_ATTR_STROKE_LINECAP) {
        dst->stroke_dsc.cap = src->stroke_dsc.cap;
    }
    if(obj->flags & _RENDER_ATTR_STROKE_LINEJOIN) {
        dst->stroke_dsc.join = src->stroke_dsc.join;
    }
    if(obj->flags & _RENDER_ATTR_STROKE_MITER_LIMIT) {
        dst->stroke_dsc.miter_limit = src->stroke_dsc.miter_limit;
    }
    if(obj->flags & _RENDER_ATTR_STROKE_DASH_ARRAY) {
        lv_array_clear(&(dst->stroke_dsc.dash_pattern));
        lv_array_copy(&(dst->stroke_dsc.dash_pattern), &(src->stroke_dsc.dash_pattern));
    }
}

/* render functions */
static void _render_viewport(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    LV_UNUSED(matrix);

    lv_svg_render_viewport_t * view = (lv_svg_render_viewport_t *)obj;
    lv_matrix_multiply(&dsc->ctx->matrix, &obj->matrix);
    if(view->viewport_fill) {
        lv_area_t rc = {0, 0, (int32_t)view->width, (int32_t)view->height};
        lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
        lv_vector_path_append_rect(path, &rc, 0, 0);
        lv_draw_vector_dsc_add_path(dsc, path);
        lv_vector_path_delete(path);
    }
}

static void _render_rect(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    lv_matrix_t mtx;
    _setup_matrix(&mtx, dsc, obj);

    if(matrix) {
        lv_matrix_multiply(&dsc->ctx->matrix, matrix);
    }

    lv_svg_render_rect_t * rect = (lv_svg_render_rect_t *)obj;

    if(rect->rx > 0 && rect->ry == 0) rect->ry = rect->rx;
    else if(rect->ry > 0 && rect->rx == 0) rect->rx = rect->ry;

    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_area_t rc = {(int32_t)rect->x, (int32_t)rect->y, (int32_t)(rect->x + rect->width - 1), (int32_t)(rect->y + rect->height - 1)};
    lv_vector_path_append_rect(path, &rc, rect->rx, rect->ry);

    _copy_draw_dsc_from_ref(dsc, obj);
    lv_draw_vector_dsc_add_path(dsc, path);
    lv_vector_path_delete(path);

    _restore_matrix(&mtx, dsc);
}

static void _render_circle(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    lv_matrix_t mtx;
    _setup_matrix(&mtx, dsc, obj);

    if(matrix) {
        lv_matrix_multiply(&dsc->ctx->matrix, matrix);
    }

    lv_svg_render_circle_t * circle = (lv_svg_render_circle_t *)obj;
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_fpoint_t cp = {circle->cx, circle->cy};
    lv_vector_path_append_circle(path, &cp, circle->r, circle->r);

    _copy_draw_dsc_from_ref(dsc, obj);
    lv_draw_vector_dsc_add_path(dsc, path);
    lv_vector_path_delete(path);

    _restore_matrix(&mtx, dsc);
}

static void _render_ellipse(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    lv_matrix_t mtx;
    _setup_matrix(&mtx, dsc, obj);

    if(matrix) {
        lv_matrix_multiply(&dsc->ctx->matrix, matrix);
    }

    lv_svg_render_ellipse_t * ellipse = (lv_svg_render_ellipse_t *)obj;
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_fpoint_t cp = {ellipse->cx, ellipse->cy};
    lv_vector_path_append_circle(path, &cp, ellipse->rx, ellipse->ry);

    _copy_draw_dsc_from_ref(dsc, obj);
    lv_draw_vector_dsc_add_path(dsc, path);
    lv_vector_path_delete(path);

    _restore_matrix(&mtx, dsc);
}

static void _render_line(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    lv_matrix_t mtx;
    _setup_matrix(&mtx, dsc, obj);

    if(matrix) {
        lv_matrix_multiply(&dsc->ctx->matrix, matrix);
    }

    lv_svg_render_line_t * line = (lv_svg_render_line_t *)obj;
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_fpoint_t sp = {line->x1, line->y1};
    lv_vector_path_move_to(path, &sp);
    lv_fpoint_t ep = {line->x2, line->y2};
    lv_vector_path_line_to(path, &ep);

    _copy_draw_dsc_from_ref(dsc, obj);
    lv_draw_vector_dsc_add_path(dsc, path);
    lv_vector_path_delete(path);

    _restore_matrix(&mtx, dsc);
}

static void _render_poly(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    lv_matrix_t mtx;
    _setup_matrix(&mtx, dsc, obj);

    if(matrix) {
        lv_matrix_multiply(&dsc->ctx->matrix, matrix);
    }

    lv_svg_render_poly_t * poly = (lv_svg_render_poly_t *)obj;

    _copy_draw_dsc_from_ref(dsc, obj);
    lv_draw_vector_dsc_add_path(dsc, poly->path);

    _restore_matrix(&mtx, dsc);
}

static void _render_group(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    lv_svg_render_group_t * group = (lv_svg_render_group_t *)obj;
    lv_matrix_t mtx;
    _setup_matrix(&mtx, dsc, obj);

    struct _lv_svg_draw_dsc save_dsc;
    lv_memzero(&save_dsc, sizeof(struct _lv_svg_draw_dsc));

    for(uint32_t i = 0; i < group->items.size; i++) {
        lv_svg_render_obj_t * list = *((lv_svg_render_obj_t **)lv_array_at(&group->items, i));

        if(list->clz->render && (list->flags & _RENDER_IN_GROUP)) {
            _copy_draw_dsc(&(save_dsc.dsc), dsc->ctx);
            _special_render(list, dsc);
            list->clz->render(list, dsc, matrix);
            _copy_draw_dsc(dsc->ctx, &(save_dsc.dsc));
        }
    }

    _restore_matrix(&mtx, dsc);
}

static void _render_image(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    lv_matrix_t imtx;
    _setup_matrix(&imtx, dsc, obj);

    if(matrix) {
        lv_matrix_multiply(&dsc->ctx->matrix, matrix);
    }

    lv_svg_render_image_t * image = (lv_svg_render_image_t *)obj;
    if(!image->img_dsc.header.w || !image->img_dsc.header.h || !image->img_dsc.src) {
        return;
    }

    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_area_t rc = {(int32_t)image->x, (int32_t)image->y, (int32_t)(image->x + image->width - 1), (int32_t)(image->y + image->height - 1)};
    lv_vector_path_append_rect(path, &rc, 0, 0);

    lv_matrix_t mtx;
    lv_matrix_identity(&mtx);

    float img_w = (float)image->img_dsc.header.w;
    float img_h = (float)image->img_dsc.header.h;
    float scale_x = image->width / img_w;
    float scale_y = image->height / img_h;
    float scale = 1.0f;

    if((image->ratio & 0x1) == LV_SVG_ASPECT_RATIO_OPT_SLICE) {
        scale = MAX(scale_x, scale_y);
    }
    else if((image->ratio & 0x1) == LV_SVG_ASPECT_RATIO_OPT_MEET) {
        scale = MIN(scale_x, scale_y);
    }

    uint32_t align = image->ratio & ~0x1;

    switch(align) {
        case LV_SVG_ASPECT_RATIO_NONE:
            lv_matrix_scale(&mtx, scale_x, scale_y);
            break;
        case LV_SVG_ASPECT_RATIO_XMIN_YMIN:
            lv_matrix_scale(&mtx, scale, scale);
            break;
        case LV_SVG_ASPECT_RATIO_XMID_YMIN: {
                float tx = (image->width - img_w * scale) / 2;
                lv_matrix_translate(&mtx, tx, 0);
                lv_matrix_scale(&mtx, scale, scale);
            }
            break;
        case LV_SVG_ASPECT_RATIO_XMAX_YMIN: {
                float tx = image->width - img_w * scale;
                lv_matrix_translate(&mtx, tx, 0);
                lv_matrix_scale(&mtx, scale, scale);
            }
            break;
        case LV_SVG_ASPECT_RATIO_XMIN_YMID: {
                float ty = (image->height - img_h * scale) / 2;
                lv_matrix_translate(&mtx, 0, ty);
                lv_matrix_scale(&mtx, scale, scale);
            }
            break;
        case LV_SVG_ASPECT_RATIO_XMID_YMID: {
                float tx = (image->width - img_w * scale) / 2;
                float ty = (image->height - img_h * scale) / 2;
                lv_matrix_translate(&mtx, tx, ty);
                lv_matrix_scale(&mtx, scale, scale);
            }
            break;
        case LV_SVG_ASPECT_RATIO_XMAX_YMID: {
                float tx = image->width - img_w * scale;
                float ty = (image->height - img_h * scale) / 2;
                lv_matrix_translate(&mtx, tx, ty);
                lv_matrix_scale(&mtx, scale, scale);
            }
            break;
        case LV_SVG_ASPECT_RATIO_XMIN_YMAX: {
                float ty = image->height - img_h * scale;
                lv_matrix_translate(&mtx, 0, ty);
                lv_matrix_scale(&mtx, scale, scale);
            }
            break;
        case LV_SVG_ASPECT_RATIO_XMID_YMAX: {
                float tx = (image->width - img_w * scale) / 2;
                float ty = image->height - img_h * scale;
                lv_matrix_translate(&mtx, tx, ty);
                lv_matrix_scale(&mtx, scale, scale);
            }
            break;
        case LV_SVG_ASPECT_RATIO_XMAX_YMAX: {
                float tx = image->width - img_w * scale;
                float ty = image->height - img_h * scale;
                lv_matrix_translate(&mtx, tx, ty);
                lv_matrix_scale(&mtx, scale, scale);
            }
            break;
    }

    lv_draw_vector_dsc_set_fill_transform(dsc, &mtx);
    lv_draw_vector_dsc_set_fill_image(dsc, &image->img_dsc);

    _copy_draw_dsc_from_ref(dsc, obj);
    lv_draw_vector_dsc_add_path(dsc, path);
    lv_vector_path_delete(path);

    _restore_matrix(&imtx, dsc);
}

static void _render_use(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    LV_UNUSED(matrix);
    lv_matrix_t imtx;
    _setup_matrix(&imtx, dsc, obj);

    lv_svg_render_use_t * use = (lv_svg_render_use_t *)obj;

    lv_matrix_t mtx;
    lv_matrix_identity(&mtx);
    lv_matrix_translate(&mtx, use->x, use->y);

    lv_svg_render_obj_t * list = obj->head;
    while(list) {
        if(list->id) {
            if(strcmp(use->xlink, list->id) == 0) {
                if(list->clz->render) {
                    _prepare_render(list, dsc);
                    _special_render(obj, dsc);
                    _copy_draw_dsc_from_ref(dsc, obj);
                    list->clz->render(list, dsc, &mtx);
                }
                break;
            }
        }
        list = list->next;
    }

    _restore_matrix(&imtx, dsc);
}

#if LV_USE_FREETYPE
static bool _is_control_character(uint32_t ch)
{
    return ch == '\n' || ch == '\t' || ch == '\r';
}

static void _render_text(const lv_svg_render_obj_t * obj, lv_draw_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    lv_svg_render_text_t * text = (lv_svg_render_text_t *)obj;
    if(!text->font) {
        if(!hal_funcs.get_font_path) {
            return;
        }
        const char * font_path = hal_funcs.get_font_path(text->family);
        if(!font_path) {
            return;
        }
        text->font = lv_freetype_font_create(font_path, LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, (uint32_t)text->size,
                                             text->style);
    }

    if(!text->font || !lv_freetype_is_outline_font(text->font)) {
        LV_LOG_ERROR("svg current font is not outline font!");
        return;
    }

    lv_matrix_t tmtx;
    _setup_matrix(&tmtx, dsc, obj);

    if(matrix) {
        lv_matrix_multiply(&dsc->ctx->matrix, matrix);
    }

    bool build_path = false;
    if(lv_array_size(&text->path->ops) == 0) { /* empty path */
        build_path = true;
    }

    /* draw text contents and spans */
    lv_matrix_t mtx;
    lv_matrix_identity(&mtx);
    lv_matrix_translate(&mtx, text->x, text->y);
    for(uint32_t i = 0; i < lv_array_size(&text->contents); i++) {
        lv_svg_render_obj_t * ptext = *((lv_svg_render_obj_t **)lv_array_at(&text->contents, i));
        lv_svg_render_content_t * content = (lv_svg_render_content_t *)ptext;

        if(content->render_content) {
            content->render_content(content, dsc, &mtx);
        }
        else {
            if(build_path) {
                lv_vector_path_t * glyph_path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

                float scale = text->size / 128.0f;
                for(uint32_t j = 0; j < content->count; j++) {

                    uint32_t letter = content->letters[j];
                    if(_is_control_character(letter)) {
                        continue;
                    }
                    lv_font_glyph_dsc_t g;
                    lv_font_get_glyph_dsc(text->font, &g, content->letters[j], '\0');
                    lv_vector_path_t * p = (lv_vector_path_t *)lv_font_get_glyph_bitmap(&g, NULL);
                    lv_vector_path_clear(glyph_path);
                    lv_vector_path_copy(glyph_path, p);
                    uint32_t letter_w = g.box_w > 0 ? g.box_w : g.adv_w;

                    lv_matrix_t scale_matrix = mtx;
                    lv_matrix_translate(&mtx, g.ofs_x, 0);
                    lv_matrix_scale(&scale_matrix, scale, scale);
                    lv_matrix_transform_path(&scale_matrix, glyph_path);

                    lv_vector_path_append_path(text->path, glyph_path);
                    text->font->release_glyph(text->font, &g);
                    lv_matrix_translate(&mtx, letter_w, 0);
                }

                lv_vector_path_delete(glyph_path);
                lv_vector_path_get_bounding(text->path, &text->bounds);
            }
        }
    }

    _copy_draw_dsc_from_ref(dsc, obj);
    lv_draw_vector_dsc_add_path(dsc, text->path);

    _restore_matrix(&tmtx, dsc);
}

static void _render_span(const lv_svg_render_content_t * content, lv_draw_vector_dsc_t * dsc, lv_matrix_t * matrix)
{
    lv_svg_render_obj_t * obj = (lv_svg_render_obj_t *)content;

    lv_svg_render_tspan_t * span = (lv_svg_render_tspan_t *)content;
    if(!span->font) {
        if(!hal_funcs.get_font_path) {
            return;
        }
        const char * font_path = hal_funcs.get_font_path(span->family);
        if(!font_path) {
            return;
        }
        span->font = lv_freetype_font_create(font_path, LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, (uint32_t)span->size,
                                             span->style);
    }

    if(!span->font || !lv_freetype_is_outline_font(span->font)) {
        LV_LOG_ERROR("svg current font is not outline font!");
        return;
    }

    struct _lv_svg_draw_dsc save_dsc;
    lv_memzero(&save_dsc, sizeof(struct _lv_svg_draw_dsc));
    _copy_draw_dsc(&(save_dsc.dsc), dsc->ctx);

    _copy_draw_dsc(dsc->ctx, &(obj->dsc));

    if(lv_array_size(&span->path->ops) == 0) { /* empty path */
        lv_vector_path_t * glyph_path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
        /* draw text contents and spans */
        lv_matrix_t * mtx = matrix;

        float scale = span->size / 128.0f;
        for(uint32_t j = 0; j < content->count; j++) {

            uint32_t letter = content->letters[j];
            if(_is_control_character(letter)) {
                continue;
            }
            lv_font_glyph_dsc_t g;
            lv_font_get_glyph_dsc(span->font, &g, content->letters[j], '\0');
            lv_vector_path_t * p = (lv_vector_path_t *)lv_font_get_glyph_bitmap(&g, NULL);
            lv_vector_path_clear(glyph_path);
            lv_vector_path_copy(glyph_path, p);
            uint32_t letter_w = g.box_w > 0 ? g.box_w : g.adv_w;

            lv_matrix_t scale_matrix = *mtx;
            lv_matrix_translate(mtx, g.ofs_x, 0);
            lv_matrix_scale(&scale_matrix, scale, scale);
            lv_matrix_transform_path(&scale_matrix, glyph_path);

            lv_vector_path_append_path(span->path, glyph_path);
            span->font->release_glyph(span->font, &g);
            lv_matrix_translate(mtx, letter_w, 0);
        }
        lv_vector_path_delete(glyph_path);
        lv_vector_path_get_bounding(span->path, &span->bounds);
    }
    _copy_draw_dsc_from_ref(dsc, obj);
    lv_draw_vector_dsc_add_path(dsc, span->path);

    _copy_draw_dsc(dsc->ctx, &(save_dsc.dsc));
}
#endif

/* get bounds functions */

static void _get_viewport_bounds(const lv_svg_render_obj_t * obj, lv_area_t * area)
{
    lv_svg_render_viewport_t * viewport = (lv_svg_render_viewport_t *)obj;
    area->x1 = 0;
    area->y1 = 0;
    area->x2 = (int32_t)roundf(viewport->width);
    area->y2 = (int32_t)roundf(viewport->height);
}

static void _get_rect_bounds(const lv_svg_render_obj_t * obj, lv_area_t * area)
{
    lv_svg_render_rect_t * rect = (lv_svg_render_rect_t *)obj;
    area->x1 = (int32_t)rect->x;
    area->y1 = (int32_t)rect->y;
    area->x2 = (int32_t)(rect->x + rect->width);
    area->y2 = (int32_t)(rect->y + rect->height);
}

static void _get_circle_bounds(const lv_svg_render_obj_t * obj, lv_area_t * area)
{
    lv_svg_render_circle_t * circle = (lv_svg_render_circle_t *)obj;
    area->x1 = (int32_t)(circle->cx - circle->r);
    area->y1 = (int32_t)(circle->cy - circle->r);
    area->x2 = (int32_t)(circle->cx + circle->r);
    area->y2 = (int32_t)(circle->cy + circle->r);
}

static void _get_ellipse_bounds(const lv_svg_render_obj_t * obj, lv_area_t * area)
{
    lv_svg_render_ellipse_t * ellipse = (lv_svg_render_ellipse_t *)obj;
    area->x1 = (int32_t)(ellipse->cx - ellipse->rx);
    area->y1 = (int32_t)(ellipse->cy - ellipse->ry);
    area->x2 = (int32_t)(ellipse->cx + ellipse->rx);
    area->y2 = (int32_t)(ellipse->cy + ellipse->ry);
}

static void _get_line_bounds(const lv_svg_render_obj_t * obj, lv_area_t * area)
{
    lv_svg_render_line_t * line = (lv_svg_render_line_t *)obj;
    area->x1 = (int32_t)(line->x1);
    area->y1 = (int32_t)(line->y1);
    area->x2 = (int32_t)(line->x2);
    area->y2 = (int32_t)(line->y2);
}

static void _get_poly_bounds(const lv_svg_render_obj_t * obj, lv_area_t * area)
{
    lv_svg_render_poly_t * poly = (lv_svg_render_poly_t *)obj;
    lv_area_copy(area, &poly->bounds);
}

static void _get_group_bounds(const lv_svg_render_obj_t * obj, lv_area_t * area)
{
    lv_svg_render_group_t * group = (lv_svg_render_group_t *)obj;

    int32_t x1 = 0;
    int32_t y1 = 0;
    int32_t x2 = 0;
    int32_t y2 = 0;

    for(uint32_t i = 0; i < group->items.size; i++) {
        lv_svg_render_obj_t * list = *((lv_svg_render_obj_t **)lv_array_at(&group->items, i));

        lv_area_t tc = {0};
        if(list->clz->get_bounds) {
            list->clz->get_bounds(list, &tc);

            x1 = MIN(tc.x1, x1);
            y1 = MIN(tc.y1, y1);
            x2 = MAX(tc.x2, x2);
            y2 = MAX(tc.y2, y2);
        }
    }

    area->x1 = x1;
    area->y1 = y1;
    area->x2 = x2;
    area->y2 = y2;
}

static void _get_use_bounds(const lv_svg_render_obj_t * obj, lv_area_t * area)
{
    lv_svg_render_use_t * use = (lv_svg_render_use_t *)obj;

    lv_svg_render_obj_t * list = obj->head;
    while(list) {
        if(list->id && strcmp(use->xlink, list->id) == 0) {
            if(list->clz->get_bounds) {
                list->clz->get_bounds(list, area);
            }
            break;
        }
        list = list->next;
    }
}

#if LV_USE_FREETYPE
static void _get_text_bounds(const lv_svg_render_obj_t * obj, lv_area_t * area)
{
    lv_svg_render_text_t * text = (lv_svg_render_text_t *)obj;
    lv_area_copy(area, &text->bounds);
}

static void _get_tspan_bounds(const lv_svg_render_obj_t * obj, lv_area_t * area)
{
    lv_svg_render_tspan_t * tspan = (lv_svg_render_tspan_t *)obj;
    lv_area_copy(area, &tspan->bounds);
}
#endif

/* get size fucctions */
static uint32_t _calc_path_data_size(lv_vector_path_t * path)
{
    uint32_t size = 0;
    size += path->ops.capacity * path->ops.element_size;
    size += path->points.capacity * path->points.element_size;
    size += sizeof(int32_t);
    return size;
}

static void _get_obj_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    *size += sizeof(lv_svg_render_obj_t);
    if(obj->id) {
        *size += strlen(obj->id);
    }
    if(obj->fill_ref) {
        *size += strlen(obj->fill_ref);
    }
    if(obj->stroke_ref) {
        *size += strlen(obj->stroke_ref);
    }
}

static void _get_viewport_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    *size += sizeof(float) * 2;
    *size += sizeof(bool);
}

static void _get_rect_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    *size += sizeof(float) * 6;
}

static void _get_circle_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    *size += sizeof(float) * 3;
}

static void _get_ellipse_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    *size += sizeof(float) * 4;
}

static void _get_line_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    *size += sizeof(float) * 4;
}

static void _get_poly_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    lv_svg_render_poly_t * poly = (lv_svg_render_poly_t *)obj;
    lv_vector_path_t * path = poly->path;

    *size += _calc_path_data_size(path);
    *size += sizeof(lv_vector_path_t);
    *size += sizeof(lv_area_t);
    *size += sizeof(void *);
}

static void _get_use_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    lv_svg_render_use_t * use = (lv_svg_render_use_t *)obj;
    if(use->xlink) {
        *size += lv_strlen(use->xlink);
    }
    *size += sizeof(float) * 2;
    *size += sizeof(void *);

}

static void _get_image_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    *size += sizeof(lv_draw_image_dsc_t);
    *size += sizeof(lv_svg_aspect_ratio_t);
    *size += sizeof(float) * 4;
}

static void _get_solid_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    *size += sizeof(lv_color_t);
    *size += sizeof(float);
}

static void _get_grad_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    *size += sizeof(lv_vector_gradient_t);
    *size += sizeof(lv_svg_gradient_units_t);
}

#if LV_USE_FREETYPE
static void _get_span_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    lv_svg_render_tspan_t * span = (lv_svg_render_tspan_t *)obj;
    if(span->family) {
        *size += lv_strlen(span->family);
    }

    *size += _calc_path_data_size(span->path);
    *size += sizeof(float);
    *size += sizeof(lv_freetype_font_style_t);
    *size += sizeof(lv_vector_path_t);
    *size += sizeof(lv_area_t);
    *size += sizeof(void *) * 3;
}

static void _get_txt_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    lv_svg_render_text_t * txt = (lv_svg_render_text_t *)obj;
    if(txt->family) {
        *size += lv_strlen(txt->family);
    }

    *size += lv_array_capacity(&txt->contents) * sizeof(void *);
    *size += _calc_path_data_size(txt->path);
    *size += sizeof(float) * 3;
    *size += sizeof(lv_freetype_font_style_t);
    *size += sizeof(lv_vector_path_t);
    *size += sizeof(lv_area_t);
    *size += sizeof(void *) * 3;
}

static void _get_content_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    lv_svg_render_content_t * content = (lv_svg_render_content_t *)obj;
    *size += sizeof(uint32_t) * (content->count + 1);
}
#endif

static void _get_group_size(const struct _lv_svg_render_obj * obj, uint32_t * size)
{
    _get_obj_size(obj, size);
    lv_svg_render_group_t * group = (lv_svg_render_group_t *)obj;
    *size += lv_array_capacity(&group->items) * sizeof(void *);
}

/* destroy functions */
static void _destroy_poly(lv_svg_render_obj_t * obj)
{
    lv_svg_render_poly_t * poly = (lv_svg_render_poly_t *)obj;
    lv_vector_path_delete(poly->path);
}

static void _destroy_use(lv_svg_render_obj_t * obj)
{
    lv_svg_render_use_t * use = (lv_svg_render_use_t *)obj;
    if(use->xlink) {
        lv_free(use->xlink);
    }
}

static void _destroy_group(lv_svg_render_obj_t * obj)
{
    lv_svg_render_group_t * group = (lv_svg_render_group_t *)obj;
    lv_array_deinit(&group->items);
}

#if LV_USE_FREETYPE
static void _destroy_text(lv_svg_render_obj_t * obj)
{
    lv_svg_render_text_t * text = (lv_svg_render_text_t *)obj;
    if(text->font) {
        lv_freetype_font_delete(text->font);
    }
    if(text->family) {
        lv_free(text->family);
    }
    lv_array_deinit(&text->contents);
    lv_vector_path_delete(text->path);
}

static void _destroy_content(lv_svg_render_obj_t * obj)
{
    lv_svg_render_content_t * content = (lv_svg_render_content_t *)obj;
    if(content->letters) {
        lv_free(content->letters);
    }
}

static void _destroy_tspan(lv_svg_render_obj_t * obj)
{
    lv_svg_render_tspan_t * span = (lv_svg_render_tspan_t *)obj;
    if(span->font) {
        lv_freetype_font_delete(span->font);
    }

    if(span->family) {
        lv_free(span->family);
    }

    _destroy_content(obj);
    lv_vector_path_delete(span->path);
}

#endif

static lv_svg_render_class svg_viewport_class = {
    .init = _init_viewport,
    .render = _render_viewport,
    .set_attr = _set_viewport_attr,
    .get_bounds = _get_viewport_bounds,
    .get_size = _get_viewport_size,
};

static lv_svg_render_class svg_rect_class = {
    .init = _init_obj,
    .render = _render_rect,
    .set_attr = _set_rect_attr,
    .get_bounds = _get_rect_bounds,
    .get_size = _get_rect_size,
};

static lv_svg_render_class svg_circle_class = {
    .init = _init_obj,
    .render = _render_circle,
    .set_attr = _set_circle_attr,
    .get_bounds = _get_circle_bounds,
    .get_size = _get_circle_size,
};

static lv_svg_render_class svg_ellipse_class = {
    .init = _init_obj,
    .render = _render_ellipse,
    .set_attr = _set_ellipse_attr,
    .get_bounds = _get_ellipse_bounds,
    .get_size = _get_ellipse_size,
};

static lv_svg_render_class svg_line_class = {
    .init = _init_obj,
    .render = _render_line,
    .set_attr = _set_line_attr,
    .get_bounds = _get_line_bounds,
    .get_size = _get_line_size,
};

static lv_svg_render_class svg_polyline_class = {
    .init = _init_poly,
    .render = _render_poly,
    .set_attr = _set_polyline_attr,
    .get_bounds = _get_poly_bounds,
    .destroy = _destroy_poly,
    .get_size = _get_poly_size,
};

static lv_svg_render_class svg_polygon_class = {
    .init = _init_poly,
    .render = _render_poly,
    .set_attr = _set_polygon_attr,
    .get_bounds = _get_poly_bounds,
    .destroy = _destroy_poly,
    .get_size = _get_poly_size,
};

static lv_svg_render_class svg_path_class = {
    .init = _init_poly,
    .render = _render_poly,
    .set_attr = _set_path_attr,
    .get_bounds = _get_poly_bounds,
    .destroy = _destroy_poly,
    .get_size = _get_poly_size,
};

#if LV_USE_FREETYPE
static lv_svg_render_class svg_text_class = {
    .init = _init_text,
    .set_attr = _set_text_attr,
    .render = _render_text,
    .get_bounds = _get_text_bounds,
    .destroy = _destroy_text,
    .get_size = _get_txt_size,
};

static lv_svg_render_class svg_tspan_class = {
    .init = _init_tspan,
    .set_attr = _set_tspan_attr,
    .get_bounds = _get_tspan_bounds,
    .destroy = _destroy_tspan,
    .get_size = _get_span_size,
};

static lv_svg_render_class svg_content_class = {
    .init = _init_content,
    .destroy = _destroy_content,
    .get_size = _get_content_size,
};
#endif

static lv_svg_render_class svg_image_class = {
    .init = _init_image,
    .render = _render_image,
    .set_attr = _set_image_attr,
    .get_size = _get_image_size,
};

static lv_svg_render_class svg_use_class = {
    .init = _init_obj,
    .set_attr = _set_use_attr,
    .render = _render_use,
    .destroy = _destroy_use,
    .get_bounds = _get_use_bounds,
    .get_size = _get_use_size,
};

static lv_svg_render_class svg_solid_class = {
    .init = _init_solid,
    .set_attr = _set_solid_attr,
    .set_paint_ref = _set_solid_ref,
    .get_size = _get_solid_size,
};

static lv_svg_render_class svg_grad_class = {
    .init = _init_gradient,
    .set_attr = _set_gradient_attr,
    .set_paint_ref = _set_gradient_ref,
    .get_size = _get_grad_size,
};

static lv_svg_render_class svg_group_class = {
    .init = _init_group,
    .set_attr = _set_attr,
    .render = _render_group,
    .destroy = _destroy_group,
    .get_bounds = _get_group_bounds,
    .get_size = _get_group_size,
};

static lv_svg_render_obj_t * _lv_svg_render_create(const lv_svg_node_t * node,
                                                   struct _lv_svg_drawing_builder_state * state)
{
    switch(node->type) {
        case LV_SVG_TAG_SVG: {
                lv_svg_render_viewport_t * view = lv_zalloc(sizeof(lv_svg_render_viewport_t));
                LV_ASSERT_MALLOC(view);
                view->base.clz = &svg_viewport_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(view), node, state);
                return LV_SVG_RENDER_OBJ(view);
            }
        case LV_SVG_TAG_RECT: {
                lv_svg_render_rect_t * rect = lv_zalloc(sizeof(lv_svg_render_rect_t));
                LV_ASSERT_MALLOC(rect);
                rect->base.clz = &svg_rect_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(rect), node, state);
                return LV_SVG_RENDER_OBJ(rect);
            }
        case LV_SVG_TAG_CIRCLE: {
                lv_svg_render_circle_t * circle = lv_zalloc(sizeof(lv_svg_render_circle_t));
                LV_ASSERT_MALLOC(circle);
                circle->base.clz = &svg_circle_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(circle), node, state);
                return LV_SVG_RENDER_OBJ(circle);
            }
        case LV_SVG_TAG_ELLIPSE: {
                lv_svg_render_ellipse_t * ellipse = lv_zalloc(sizeof(lv_svg_render_ellipse_t));
                LV_ASSERT_MALLOC(ellipse);
                ellipse->base.clz = &svg_ellipse_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(ellipse), node, state);
                return LV_SVG_RENDER_OBJ(ellipse);
            }
        case LV_SVG_TAG_LINE: {
                lv_svg_render_line_t * line = lv_zalloc(sizeof(lv_svg_render_line_t));
                LV_ASSERT_MALLOC(line);
                line->base.clz = &svg_line_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(line), node, state);
                return LV_SVG_RENDER_OBJ(line);
            }
        case LV_SVG_TAG_POLYLINE: {
                lv_svg_render_poly_t * poly = lv_zalloc(sizeof(lv_svg_render_poly_t));
                LV_ASSERT_MALLOC(poly);
                poly->base.clz = &svg_polyline_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(poly), node, state);
                return LV_SVG_RENDER_OBJ(poly);
            }
        case LV_SVG_TAG_POLYGON: {
                lv_svg_render_poly_t * poly = lv_zalloc(sizeof(lv_svg_render_poly_t));
                LV_ASSERT_MALLOC(poly);
                poly->base.clz = &svg_polygon_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(poly), node, state);
                return LV_SVG_RENDER_OBJ(poly);
            }
        case LV_SVG_TAG_PATH: {
                lv_svg_render_poly_t * poly = lv_zalloc(sizeof(lv_svg_render_poly_t));
                LV_ASSERT_MALLOC(poly);
                poly->base.clz = &svg_path_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(poly), node, state);
                return LV_SVG_RENDER_OBJ(poly);
            }
#if LV_USE_FREETYPE
        case LV_SVG_TAG_TEXT: {
                lv_svg_render_text_t * txt = lv_zalloc(sizeof(lv_svg_render_text_t));
                LV_ASSERT_MALLOC(txt);
                txt->base.clz = &svg_text_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(txt), node, state);
                return LV_SVG_RENDER_OBJ(txt);
            }
        case LV_SVG_TAG_TSPAN: {
                lv_svg_render_tspan_t * span = lv_zalloc(sizeof(lv_svg_render_tspan_t));
                LV_ASSERT_MALLOC(span);
                lv_svg_render_content_t * content = (lv_svg_render_content_t *)span;
                content->render_content = _render_span;
                content->base.clz = &svg_tspan_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(span), node, state);
                return LV_SVG_RENDER_OBJ(span);
            }
        case LV_SVG_TAG_CONTENT: {
                lv_svg_render_content_t * content = lv_zalloc(sizeof(lv_svg_render_content_t));
                LV_ASSERT_MALLOC(content);
                content->base.clz = &svg_content_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(content), node, state);
                return LV_SVG_RENDER_OBJ(content);
            }
#endif
        case LV_SVG_TAG_IMAGE: {
                lv_svg_render_image_t * image = lv_zalloc(sizeof(lv_svg_render_image_t));
                LV_ASSERT_MALLOC(image);
                image->base.clz = &svg_image_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(image), node, state);
                return LV_SVG_RENDER_OBJ(image);
            }
        case LV_SVG_TAG_USE: {
                lv_svg_render_use_t * use = lv_zalloc(sizeof(lv_svg_render_use_t));
                LV_ASSERT_MALLOC(use);
                use->base.clz = &svg_use_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(use), node, state);
                return LV_SVG_RENDER_OBJ(use);
            }
        case LV_SVG_TAG_SOLID_COLOR: {
                lv_svg_render_solid_t * solid = lv_zalloc(sizeof(lv_svg_render_solid_t));
                LV_ASSERT_MALLOC(solid);
                solid->base.clz = &svg_solid_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(solid), node, state);
                return LV_SVG_RENDER_OBJ(solid);
            }
        case LV_SVG_TAG_RADIAL_GRADIENT:
        case LV_SVG_TAG_LINEAR_GRADIENT: {
                lv_svg_render_gradient_t * grad = lv_zalloc(sizeof(lv_svg_render_gradient_t));
                LV_ASSERT_MALLOC(grad);
                grad->base.clz = &svg_grad_class;
                if(node->type == LV_SVG_TAG_LINEAR_GRADIENT) {
                    grad->dsc.style = LV_VECTOR_GRADIENT_STYLE_LINEAR;
                }
                else {   /* radial gradient */
                    grad->dsc.style = LV_VECTOR_GRADIENT_STYLE_RADIAL;
                }
                _set_render_attrs(LV_SVG_RENDER_OBJ(grad), node, state);
                return LV_SVG_RENDER_OBJ(grad);
            }
        case LV_SVG_TAG_G: {
                lv_svg_render_group_t * group = lv_zalloc(sizeof(lv_svg_render_group_t));
                LV_ASSERT_MALLOC(group);
                group->base.clz = &svg_group_class;
                _set_render_attrs(LV_SVG_RENDER_OBJ(group), node, state);
                return LV_SVG_RENDER_OBJ(group);
            }
        default:
            return NULL;
    }
}

static bool _lv_svg_doc_walk_cb(const lv_tree_node_t * node, void * data)
{
    struct _lv_svg_drawing_builder_state * state = (struct _lv_svg_drawing_builder_state *)data;
    lv_svg_render_obj_t * obj = _lv_svg_render_create(LV_SVG_NODE(node), state);
    if(!obj) {
        return true;
    }

    if(state->in_defs) {
        obj->flags |= _RENDER_IN_DEFS;
    }
    if(state->in_group_deps > 0) {
        obj->flags |= _RENDER_IN_GROUP;
    }

    if(state->list == NULL) {
        state->list = obj;
        state->tail = obj;
    }
    else {
        state->tail->next = obj;
        state->tail = obj;
    }
    LV_SVG_NODE(node)->render_obj = obj;
    return true;
}

static bool _lv_svg_doc_walk_before_cb(const lv_tree_node_t * node, void * data)
{
    struct _lv_svg_drawing_builder_state * state = (struct _lv_svg_drawing_builder_state *)data;
    lv_svg_node_t * svg_node = LV_SVG_NODE(node);
#if LV_USE_FREETYPE
    if(svg_node->type == LV_SVG_TAG_TEXT) {
        state->in_text = true;
        state->cur_text = svg_node;
    }
#endif
    if(svg_node->type == LV_SVG_TAG_DEFS) {
        state->in_defs = true;
    }

    if(svg_node->type == LV_SVG_TAG_G) {
        state->in_group_deps++;
    }
    state->draw_dsc = _lv_svg_draw_dsc_push(state->draw_dsc);
    return true;
}

static void _lv_svg_doc_walk_after_cb(const lv_tree_node_t * node, void * data)
{
    struct _lv_svg_drawing_builder_state * state = (struct _lv_svg_drawing_builder_state *)data;
    lv_svg_node_t * svg_node = LV_SVG_NODE(node);
    if(svg_node->render_obj) {
        _copy_draw_dsc(&(LV_SVG_NODE(node)->render_obj->dsc), &(state->draw_dsc->dsc));
    }
#if LV_USE_FREETYPE
    if(state->in_text) {
        if(svg_node->type == LV_SVG_TAG_TSPAN || svg_node->type == LV_SVG_TAG_CONTENT) {
            if(LV_TREE_NODE(svg_node)->parent == LV_TREE_NODE(state->cur_text)) {
                lv_svg_render_text_t * text = (lv_svg_render_text_t *)state->cur_text->render_obj;
                if((lv_array_size(&text->contents) + 1) > lv_array_capacity(&text->contents)) {
                    lv_array_resize(&text->contents, text->contents.capacity << 1);
                }
                lv_array_push_back(&text->contents, (uint8_t *)(&svg_node->render_obj));
            }
        }
    }
    if(svg_node->type == LV_SVG_TAG_TEXT) {
        state->in_text = false;
        state->cur_text = NULL;
    }
#endif
    if(svg_node->type == LV_SVG_TAG_G) {
        lv_svg_render_group_t * group = (lv_svg_render_group_t *)svg_node->render_obj;
        uint32_t count = LV_TREE_NODE(node)->child_cnt;
        for(uint32_t i = 0; i < count; i++) {
            lv_svg_node_t * child = LV_SVG_NODE_CHILD(node, i);
            if(child->render_obj) { /* not defs */
                lv_array_push_back(&group->items, (uint8_t *)(&child->render_obj));
            }
        }

        state->in_group_deps--;
        if(state->in_group_deps == 0) {
            group->base.flags &= ~_RENDER_IN_GROUP;
        }
    }
    if(svg_node->type == LV_SVG_TAG_DEFS) {
        state->in_defs = false;
    }
    state->draw_dsc = _lv_svg_draw_dsc_pop(state->draw_dsc);
}

#if LV_USE_FREETYPE
static void _freetype_outline_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_freetype_outline_event_param_t * param = lv_event_get_param(e);
    switch(code) {
        case LV_EVENT_CREATE:
            param->outline = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
            break;
        case LV_EVENT_DELETE:
            lv_vector_path_delete(param->outline);
            break;
        case LV_EVENT_INSERT: {
                if(param->type == LV_FREETYPE_OUTLINE_MOVE_TO) {
                    lv_fpoint_t pt = {0};
                    pt.x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.x);
                    pt.y = -LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.y);
                    lv_vector_path_move_to(param->outline, &pt);
                }
                else if(param->type == LV_FREETYPE_OUTLINE_LINE_TO) {
                    lv_fpoint_t pt = {0};
                    pt.x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.x);
                    pt.y = -LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.y);
                    lv_vector_path_line_to(param->outline, &pt);
                }
                else if(param->type == LV_FREETYPE_OUTLINE_CUBIC_TO) {
                    lv_fpoint_t pt[3] = {0};
                    pt[0].x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->control1.x);
                    pt[0].y = -LV_FREETYPE_F26DOT6_TO_FLOAT(param->control1.y);
                    pt[1].x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->control2.x);
                    pt[1].y = -LV_FREETYPE_F26DOT6_TO_FLOAT(param->control2.y);
                    pt[2].x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.x);
                    pt[2].y = -LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.y);
                    lv_vector_path_cubic_to(param->outline, &pt[0], &pt[1], &pt[2]);
                }
                else if(param->type == LV_FREETYPE_OUTLINE_CONIC_TO) {
                    lv_fpoint_t pt[2] = {0};
                    pt[0].x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->control1.x);
                    pt[0].y = -LV_FREETYPE_F26DOT6_TO_FLOAT(param->control1.y);
                    pt[1].x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.x);
                    pt[1].y = -LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.y);
                    lv_vector_path_quad_to(param->outline, &pt[0], &pt[1]);
                }
                else if(param->type == LV_FREETYPE_OUTLINE_END) {
                    lv_vector_path_close(param->outline);
                }
            }
            break;
        default:
            LV_LOG_WARN("unknown event code: %d", code);
            break;
    }
}
#endif
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_svg_render_obj_t * lv_svg_render_create(const lv_svg_node_t * svg_doc)
{
    if(!svg_doc) {
        return NULL;
    }

    struct _lv_svg_draw_dsc * dsc = _lv_svg_draw_dsc_create();
    struct _lv_svg_drawing_builder_state state = {
        .doc = svg_doc,
        .draw_dsc = dsc,
        .in_group_deps = 0,
        .in_defs = false,
#if LV_USE_FREETYPE
        .in_text = false,
        .cur_text = NULL,
#endif
        .list = NULL, .tail = NULL
    };

    lv_tree_walk(LV_TREE_NODE(svg_doc), LV_TREE_WALK_PRE_ORDER, _lv_svg_doc_walk_cb, _lv_svg_doc_walk_before_cb,
                 _lv_svg_doc_walk_after_cb, &state);
    _lv_svg_draw_dsc_delete(dsc);
    return state.list;
}

void lv_svg_render_delete(lv_svg_render_obj_t * list)
{
    while(list) {
        lv_svg_render_obj_t * obj = list;
        list = list->next;

        _deinit_draw_dsc(&(obj->dsc));

        if(obj->clz->destroy) {
            obj->clz->destroy(obj);
        }
        if(obj->id) {
            lv_free(obj->id);
        }
        if(obj->fill_ref) {
            lv_free(obj->fill_ref);
        }
        if(obj->stroke_ref) {
            lv_free(obj->stroke_ref);
        }
        lv_free(obj);
    }
}

uint32_t lv_svg_render_get_size(const lv_svg_render_obj_t * render)
{
    if(!render) {
        return 0;
    }

    uint32_t size = 0;
    const lv_svg_render_obj_t * cur = render;
    while(cur) {
        if(cur->clz->get_size) {
            cur->clz->get_size(cur, &size);
        }
        cur = cur->next;
    }
    return size;
}

lv_result_t lv_svg_render_get_viewport_size(const lv_svg_render_obj_t * render, float * width, float * height)
{
    if(!render) {
        LV_LOG_WARN("Invalid render object");
        return LV_RESULT_INVALID;
    }

    if(render->clz != &svg_viewport_class) {
        LV_LOG_WARN("Invalid render object type");
        return LV_RESULT_INVALID;
    }
    const lv_svg_render_viewport_t * cur = (const lv_svg_render_viewport_t *)render;
    if(width) {
        *width = cur->width;
    }
    if(height) {
        *height = cur->height;
    }
    return LV_RESULT_OK;
}

void lv_draw_svg_render(lv_draw_vector_dsc_t * dsc, const lv_svg_render_obj_t * render)
{
    if(!render || !dsc) {
        return;
    }

    const lv_svg_render_obj_t * cur = render;
    while(cur) {
        if(cur->clz->render && ((cur->flags & 3) == _RENDER_NORMAL)) {
            _prepare_render(cur, dsc);
            cur->clz->render(cur, dsc, NULL);
        }
        cur = cur->next;
    }
}

void lv_draw_svg(lv_layer_t * layer, const lv_svg_node_t * svg_doc)
{
    if(!svg_doc) {
        return;
    }

    lv_draw_vector_dsc_t * dsc = lv_draw_vector_dsc_create(layer);
    lv_svg_render_obj_t * list = lv_svg_render_create(svg_doc);
    lv_draw_svg_render(dsc, list);
    lv_draw_vector(dsc);
    lv_svg_render_delete(list);
    lv_draw_vector_dsc_delete(dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_SVG*/
