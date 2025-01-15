/**
 * @file lv_draw_sw_letter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "blend/lv_draw_sw_blend_private.h"
#include "../lv_draw_label_private.h"
#include "../../draw/lv_draw_private.h"
#include "lv_draw_sw.h"

#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC

#include "../../libs/freetype/lv_freetype_private.h"

#endif

#if LV_USE_DRAW_SW

#include "../../display/lv_display.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_area.h"
#include "../../misc/lv_style.h"
#include "../../font/lv_font.h"
#include "../../core/lv_refr_private.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void /* LV_ATTRIBUTE_FAST_MEM */ draw_letter_cb(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                                       lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC

static void freetype_outline_event_cb(lv_event_t * e);
static void draw_letter_outline(lv_draw_unit_t * draw_unit, const lv_draw_glyph_dsc_t * dsc);

#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_label(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    LV_PROFILER_DRAW_BEGIN;

#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC
    static bool is_init = false;
    if(!is_init) {
        lv_freetype_outline_add_event(freetype_outline_event_cb, LV_EVENT_ALL, draw_unit);
        is_init = true;
    }
#endif

    lv_draw_label_iterate_characters(draw_unit, dsc, coords, draw_letter_cb);
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void LV_ATTRIBUTE_FAST_MEM draw_letter_cb(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                                 lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    if(glyph_draw_dsc) {
        switch(glyph_draw_dsc->format) {
            case LV_FONT_GLYPH_FORMAT_NONE: {
#if LV_USE_FONT_PLACEHOLDER
                    /* Draw a placeholder rectangle*/
                    lv_draw_border_dsc_t border_draw_dsc;
                    lv_draw_border_dsc_init(&border_draw_dsc);
                    border_draw_dsc.opa = glyph_draw_dsc->opa;
                    border_draw_dsc.color = glyph_draw_dsc->color;
                    border_draw_dsc.width = 1;
                    lv_draw_sw_border(draw_unit, &border_draw_dsc, glyph_draw_dsc->bg_coords);
#endif
                }
                break;
            case LV_FONT_GLYPH_FORMAT_A1:
            case LV_FONT_GLYPH_FORMAT_A2:
            case LV_FONT_GLYPH_FORMAT_A3:
            case LV_FONT_GLYPH_FORMAT_A4:
            case LV_FONT_GLYPH_FORMAT_A8:
            case LV_FONT_GLYPH_FORMAT_A1_ALIGNED:
            case LV_FONT_GLYPH_FORMAT_A2_ALIGNED:
            case LV_FONT_GLYPH_FORMAT_A4_ALIGNED:
            case LV_FONT_GLYPH_FORMAT_A8_ALIGNED: {
                    glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                    lv_area_t mask_area = *glyph_draw_dsc->letter_coords;
                    mask_area.x2 = mask_area.x1 + lv_draw_buf_width_to_stride(lv_area_get_width(&mask_area), LV_COLOR_FORMAT_A8) - 1;
                    lv_draw_sw_blend_dsc_t blend_dsc;
                    lv_memzero(&blend_dsc, sizeof(blend_dsc));
                    blend_dsc.color = glyph_draw_dsc->color;
                    blend_dsc.opa = glyph_draw_dsc->opa;
                    const lv_draw_buf_t * draw_buf = glyph_draw_dsc->glyph_data;
                    blend_dsc.mask_buf = draw_buf->data;
                    blend_dsc.mask_area = &mask_area;
                    blend_dsc.mask_stride = draw_buf->header.stride;
                    blend_dsc.blend_area = glyph_draw_dsc->letter_coords;
                    blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;

                    lv_draw_sw_blend(draw_unit, &blend_dsc);
                }
                break;
            case LV_FONT_GLYPH_FORMAT_IMAGE: {
                    glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                    lv_draw_image_dsc_t img_dsc;
                    lv_draw_image_dsc_init(&img_dsc);
                    img_dsc.rotation = 0;
                    img_dsc.scale_x = LV_SCALE_NONE;
                    img_dsc.scale_y = LV_SCALE_NONE;
                    img_dsc.opa = glyph_draw_dsc->opa;
                    img_dsc.src = glyph_draw_dsc->glyph_data;
                    lv_draw_sw_image(draw_unit, &img_dsc, glyph_draw_dsc->letter_coords);
                }
                break;
#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC
            case LV_FONT_GLYPH_FORMAT_VECTOR: {
                    draw_letter_outline(draw_unit, glyph_draw_dsc);
              }
              break;
#endif
            default:
                break;
        }

    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_sw_fill(draw_unit, fill_draw_dsc, fill_area);
    }
}

#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC

static void draw_letter_outline(lv_draw_unit_t * draw_unit, const lv_draw_glyph_dsc_t * glyph_dsc)
{

    uint32_t stride;
    uint8_t *buf;
    uint32_t w, h;
    bool dispatched;
    float scale;
    lv_vector_path_t *glyph_path;
    lv_vector_dsc_t *vector_dsc;
    lv_draw_buf_t *draw_buf;
    lv_matrix_t matrix;
    lv_layer_t layer;
    lv_area_t buf_area;
    lv_draw_sw_blend_dsc_t blend_dsc;
    int cf;

    glyph_path = (lv_vector_path_t *)glyph_dsc->glyph_data;
    LV_ASSERT_NULL(glyph_path);

#if 0
    /* A8 is more efficient - currently ThorVG has only partial
     * support for Grayscale - It works for simple shapes
     */
    cf = LV_COLOR_FORMAT_A8;
#else
    cf = LV_COLOR_FORMAT_ARGB8888;
#endif


    w = glyph_dsc->g->box_w;
    h = glyph_dsc->g->box_h;
    scale = LV_FREETYPE_F26DOT6_TO_FLOAT(lv_freetype_outline_get_scale(glyph_dsc->g->resolved_font));
    buf_area.x1 = 0;
    buf_area.y1 = 0;
    buf_area.y2 = h;
    buf_area.x2 = w;

    stride = lv_draw_buf_width_to_stride(w, cf);
    buf = lv_malloc(stride * h);
    LV_ASSERT_NULL(buf);

    draw_buf = lv_malloc(sizeof(lv_draw_buf_t));
    lv_draw_buf_init(draw_buf, w, h, cf, stride, buf, stride * h);

    lv_memset(buf, 0x0, stride * h);

    lv_memzero(&layer, sizeof(lv_layer_t));
    layer.draw_buf = draw_buf;
    layer.color_format = cf;
    layer.buf_area = buf_area;
    layer.phy_clip_area = buf_area;
    layer._clip_area = buf_area;

    lv_memzero(&matrix, sizeof(lv_matrix_t));

    lv_matrix_identity(&matrix);

    vector_dsc = lv_vector_dsc_create(&layer);

    /* Invert Y-Axis - Freetype's origin point is in the bottom left corner */
    lv_matrix_scale(&matrix, 1, -1);
    lv_matrix_translate(&matrix, -glyph_dsc->g->ofs_x, -glyph_dsc->g->box_h + (glyph_dsc->g->ofs_y * -1));
    lv_matrix_scale(&matrix, scale, scale);

    lv_vector_dsc_set_transform(vector_dsc, &matrix);

    if (cf == LV_COLOR_FORMAT_ARGB8888) {

        lv_vector_dsc_set_stroke_color(vector_dsc, glyph_dsc->color);
        lv_vector_dsc_set_stroke_opa(vector_dsc, glyph_dsc->opa);
        lv_vector_dsc_set_stroke_width(vector_dsc, 1);
        lv_vector_dsc_set_fill_color(vector_dsc, glyph_dsc->color);
        lv_vector_dsc_set_fill_opa(vector_dsc, glyph_dsc->opa);
    }

    lv_vector_dsc_add_path(vector_dsc, glyph_path);

    lv_draw_vector(vector_dsc);
    LV_ASSERT_NULL(layer.draw_task_head);

    lv_draw_sw_vector(draw_unit, (lv_draw_vector_task_dsc_t *)layer.draw_task_head->draw_dsc);

    lv_memzero(&blend_dsc, sizeof(blend_dsc));

    if (cf == LV_COLOR_FORMAT_A8) {

        blend_dsc.color = glyph_dsc->color;
        blend_dsc.opa = glyph_dsc->opa;
        blend_dsc.mask_buf = draw_buf->data;
        blend_dsc.mask_area = glyph_dsc->letter_coords;
        blend_dsc.mask_stride = stride;
        blend_dsc.blend_area = glyph_dsc->letter_coords;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;

        lv_draw_sw_blend(draw_unit, &blend_dsc);

    } else {

        lv_draw_image_dsc_t img_dsc;
        lv_draw_image_dsc_init(&img_dsc);
        img_dsc.rotation = 0;
        img_dsc.scale_x = LV_SCALE_NONE;
        img_dsc.scale_y = LV_SCALE_NONE;
        img_dsc.opa = LV_OPA_100;
        img_dsc.src = draw_buf;
        lv_draw_sw_image(draw_unit, &img_dsc, glyph_dsc->letter_coords);
    }


    lv_vector_dsc_delete(vector_dsc);

    lv_free(buf);

}

static void freetype_outline_event_cb(lv_event_t * e)
{

    lv_fpoint_t pnt;
    lv_fpoint_t ctrl_pnt1;
    lv_fpoint_t ctrl_pnt2;
    lv_vector_path_t *path;
    float scale;
    lv_freetype_outline_event_param_t * outline_event;

    outline_event = lv_event_get_param(e);
    pnt.x = FT_F26DOT6_TO_INT(outline_event->to.x);
    pnt.y = FT_F26DOT6_TO_INT(outline_event->to.y);

    if (lv_event_get_code(e) == LV_EVENT_CREATE) {
        outline_event->outline = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
        return;

    } else if (lv_event_get_code(e) == LV_EVENT_DELETE) {
        lv_vector_path_clear(outline_event->outline);
        lv_vector_path_delete(outline_event->outline);
        return;
    }

    path = outline_event->outline;

    switch (outline_event->type) {
        case LV_FREETYPE_OUTLINE_END:
            lv_vector_path_close(path);
            break;

        case LV_FREETYPE_OUTLINE_MOVE_TO:

            lv_vector_path_move_to(path, &pnt);
            break;

        case LV_FREETYPE_OUTLINE_LINE_TO:
            lv_vector_path_line_to(path, &pnt);
            break;

        case LV_FREETYPE_OUTLINE_CUBIC_TO:
            ctrl_pnt1.x = FT_F26DOT6_TO_INT(outline_event->control1.x);
            ctrl_pnt1.y = FT_F26DOT6_TO_INT(outline_event->control1.y);
            ctrl_pnt2.x = FT_F26DOT6_TO_INT(outline_event->control2.x);
            ctrl_pnt2.y = FT_F26DOT6_TO_INT(outline_event->control2.y);
            lv_vector_path_cubic_to(path, &ctrl_pnt1, &ctrl_pnt2, &pnt);
            break;

        case LV_FREETYPE_OUTLINE_CONIC_TO:
            ctrl_pnt1.x = FT_F26DOT6_TO_INT(outline_event->control1.x);
            ctrl_pnt1.y = FT_F26DOT6_TO_INT(outline_event->control1.y);
            lv_vector_path_quad_to(path, &ctrl_pnt1, &pnt);
            break;
    }

}

#endif /* LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC */

#endif /*LV_USE_DRAW_SW*/
