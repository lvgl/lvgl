/**
 * @file lv_svg_render.h
 *
 */

#ifndef LV_SVG_RENDER_H
#define LV_SVG_RENDER_H

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_SVG && LV_USE_VECTOR_GRAPHIC
#include "lv_svg.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_svg_render_obj {
    struct _lv_svg_render_obj * next;
    uint32_t flags;
    char * id;
    lv_vector_draw_dsc_t dsc;
    lv_matrix_t matrix;

    /* for url(XXX) reference */
    struct _lv_svg_render_obj * head;
    char * fill_ref;
    char * stroke_ref;
    void (*set_paint_ref)(struct _lv_svg_render_obj * obj, lv_vector_draw_dsc_t * dsc,
                          const struct _lv_svg_render_obj * target_obj, bool fill);

    void (*init)(struct _lv_svg_render_obj * obj, const lv_svg_node_t * node);
    void (*render)(const struct _lv_svg_render_obj * obj, lv_vector_dsc_t * dsc, const lv_matrix_t * matrix);
    void (*set_attr)(struct _lv_svg_render_obj * obj, lv_vector_draw_dsc_t * dsc, const lv_svg_attr_t * attr);
    void (*get_bounds)(const struct _lv_svg_render_obj * obj, lv_area_t * area);
    void (*destroy)(struct _lv_svg_render_obj * obj);
} lv_svg_render_obj_t;

typedef struct _lv_svg_render_hal {
    void (*load_image)(const char * image_url, lv_draw_image_dsc_t * img_dsc);
    const char * (*get_font_path)(const char * font_family);
} lv_svg_render_hal_t;
/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_svg_render_init(const lv_svg_render_hal_t * hal);

lv_svg_render_obj_t * lv_svg_render_create(const lv_svg_node_t * svg_doc);
void lv_svg_render_delete(lv_svg_render_obj_t * render);

void lv_draw_svg_render(lv_layer_t * layer, const lv_svg_render_obj_t * render);
void lv_draw_svg(lv_layer_t * layer, const lv_svg_node_t * svg_doc);

/**********************
 *      MACROS
 **********************/
#define LV_SVG_RENDER_OBJ(n) ((lv_svg_render_obj_t*)(n))

#endif /*LV_USE_SVG*/

#endif /*LV_SVG_RENDER_H*/
