/**
 * @file lv_cb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_checkbox.h"
#if LV_USE_CHECKBOX != 0

#include "../misc/lv_assert.h"
#include "../misc/lv_txt_ap.h"
#include "../core/lv_group.h"
#include "../draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_checkbox_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_checkbox_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_checkbox_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_checkbox_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_checkbox_draw(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_checkbox_class = {
    .constructor_cb = lv_checkbox_constructor,
    .destructor_cb = lv_checkbox_destructor,
    .event_cb = lv_checkbox_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .instance_size = sizeof(lv_checkbox_t),
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_checkbox_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin")
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_checkbox_set_text(lv_obj_t * obj, const char * txt)
{
    lv_checkbox_t * cb = (lv_checkbox_t *)obj;
#if LV_USE_ARABIC_PERSIAN_CHARS
    size_t len = _lv_txt_ap_calc_bytes_cnt(txt);
#else
    size_t len = strlen(txt);
#endif

    if(!cb->static_txt) cb->txt = lv_mem_realloc(cb->txt, len + 1);
    else  cb->txt = lv_mem_alloc(len + 1);
#if LV_USE_ARABIC_PERSIAN_CHARS
    _lv_txt_ap_proc(txt, cb->txt);
#else
    strcpy(cb->txt, txt);
#endif

    cb->static_txt = 0;

    lv_obj_refresh_self_size(obj);
}

void lv_checkbox_set_text_static(lv_obj_t * obj, const char * txt)
{
    lv_checkbox_t * cb = (lv_checkbox_t *)obj;

    if(!cb->static_txt) lv_mem_free(cb->txt);

    cb->txt = (char*)txt;
    cb->static_txt = 1;

    lv_obj_refresh_self_size(obj);
}

/*=====================
 * Getter functions
 *====================*/

const char * lv_checkbox_get_text(const lv_obj_t * obj)
{
    lv_checkbox_t * cb = (lv_checkbox_t *)obj;
    return cb->txt;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_checkbox_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_checkbox_t * cb = (lv_checkbox_t *)obj;

    cb->txt = "Check box";
    cb->static_txt = 1;
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_checkbox_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_checkbox_t * cb = (lv_checkbox_t *)obj;
    if(!cb->static_txt) {
        lv_mem_free(cb->txt);
        cb->txt = NULL;
    }
    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_checkbox_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;
    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if (code == LV_EVENT_PRESSED || code == LV_EVENT_RELEASED) {
       lv_obj_invalidate(obj);
    }
    else if (code == LV_EVENT_GET_SELF_SIZE) {
        lv_point_t * p = lv_event_get_param(e);
        lv_checkbox_t * cb = (lv_checkbox_t *)obj;

        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t font_h = lv_font_get_line_height(font);
        lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);

        lv_point_t txt_size;
        lv_txt_get_size(&txt_size, cb->txt, font, letter_space, line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

        lv_coord_t bg_colp = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);
        lv_coord_t marker_leftp = lv_obj_get_style_pad_left(obj, LV_PART_INDICATOR);
        lv_coord_t marker_rightp = lv_obj_get_style_pad_right(obj, LV_PART_INDICATOR);
        lv_coord_t marker_topp = lv_obj_get_style_pad_top(obj, LV_PART_INDICATOR);
        lv_coord_t marker_bottomp = lv_obj_get_style_pad_bottom(obj, LV_PART_INDICATOR);
        lv_point_t marker_size;
        marker_size.x = font_h + marker_leftp + marker_rightp;
        marker_size.y = font_h + marker_topp + marker_bottomp;

        p->x = marker_size.x + txt_size.x + bg_colp;
        p->y = LV_MAX(marker_size.y, txt_size.y);
    }
    else if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t *s = lv_event_get_param(e);
        lv_coord_t m = lv_obj_calculate_ext_draw_size(obj, LV_PART_INDICATOR);
        *s = LV_MAX(*s, m);
    }
    else if(code == LV_EVENT_RELEASED) {
        uint32_t v = lv_obj_get_state(obj) & LV_STATE_CHECKED ? 1 : 0;
        res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &v);
        if(res != LV_RES_OK) return;

        lv_obj_invalidate(obj);
    } else if(code == LV_EVENT_DRAW_MAIN) {
        lv_checkbox_draw(e);
    }
}

static void lv_checkbox_draw(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_checkbox_t * cb = (lv_checkbox_t *)obj;

    const lv_area_t * clip_area = lv_event_get_param(e);
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    lv_coord_t font_h = lv_font_get_line_height(font);

    lv_coord_t bg_topp = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t bg_leftp = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t bg_colp = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);

    lv_coord_t marker_leftp = lv_obj_get_style_pad_left(obj, LV_PART_INDICATOR);
    lv_coord_t marker_rightp = lv_obj_get_style_pad_right(obj, LV_PART_INDICATOR);
    lv_coord_t marker_topp = lv_obj_get_style_pad_top(obj, LV_PART_INDICATOR);
    lv_coord_t marker_bottomp = lv_obj_get_style_pad_bottom(obj, LV_PART_INDICATOR);

    lv_coord_t transf_w = lv_obj_get_style_transform_width(obj, LV_PART_INDICATOR);
    lv_coord_t transf_h = lv_obj_get_style_transform_height(obj, LV_PART_INDICATOR);

    lv_draw_rect_dsc_t indic_dsc;
    lv_draw_rect_dsc_init(&indic_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &indic_dsc);
    lv_area_t marker_area;
    marker_area.x1 = obj->coords.x1 + bg_leftp;
    marker_area.x2 = marker_area.x1 + font_h + marker_leftp + marker_rightp - 1;
    marker_area.y1 = obj->coords.y1 + bg_topp;
    marker_area.y2 = marker_area.y1 + font_h + marker_topp + marker_bottomp - 1;

    lv_area_t marker_area_transf;
    lv_area_copy(&marker_area_transf, &marker_area);
    marker_area_transf.x1 -= transf_w;
    marker_area_transf.x2 += transf_w;
    marker_area_transf.y1 -= transf_h;
    marker_area_transf.y2 += transf_h;

    lv_obj_draw_part_dsc_t obj_draw_dsc;
    lv_obj_draw_dsc_init(&obj_draw_dsc, clip_area);
    obj_draw_dsc.rect_dsc = &indic_dsc;
    obj_draw_dsc.draw_area = &marker_area_transf;
    obj_draw_dsc.part = LV_PART_INDICATOR;

    lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &obj_draw_dsc);
    lv_draw_rect(&marker_area_transf, clip_area, &indic_dsc);
    lv_event_send(obj, LV_EVENT_DRAW_PART_END, &obj_draw_dsc);

    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);

    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, cb->txt, font, letter_space, line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

    lv_draw_label_dsc_t txt_dsc;
    lv_draw_label_dsc_init(&txt_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &txt_dsc);

    lv_coord_t y_ofs = (lv_area_get_height(&marker_area) - font_h) / 2;
    lv_area_t txt_area;
    txt_area.x1 = marker_area.x2 + bg_colp;
    txt_area.x2 = txt_area.x1 + txt_size.x;
    txt_area.y1 = obj->coords.y1 + bg_topp + y_ofs;
    txt_area.y2 = txt_area.y1 + txt_size.y;

    lv_draw_label(&txt_area, clip_area, &txt_dsc, cb->txt, NULL);
}
#endif
