/**
 * @file lv_cb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_checkbox.h"
#if LV_USE_CHECKBOX != 0

#include "../lv_misc/lv_assert.h"
#include "../lv_misc/lv_txt_ap.h"
#include "../lv_core/lv_group.h"
#include "../lv_draw/lv_draw.h"

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
static void lv_checkbox_constructor(lv_obj_t * obj, const lv_obj_t * copy);
static void lv_checkbox_destructor(lv_obj_t * obj);
static lv_draw_res_t lv_checkbox_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_checkbox_signal(lv_obj_t * obj, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_checkbox_class = {
    .constructor_cb = lv_checkbox_constructor,
    .destructor_cb = lv_checkbox_destructor,
    .signal_cb = lv_checkbox_signal,
    .draw_cb = lv_checkbox_draw,
    .instance_size = sizeof(lv_checkbox_t),
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_checkbox_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    return lv_obj_create_from_class(&lv_checkbox_class, parent, copy);
}

/*=====================
 * Setter functions
 *====================*/

void lv_checkbox_set_text(lv_obj_t * obj, const char * txt)
{
    lv_checkbox_t * cb = (lv_checkbox_t *) obj;
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

    lv_obj_handle_self_size_chg(obj);
}

void lv_checkbox_set_text_static(lv_obj_t * obj, const char * txt)
{
    lv_checkbox_t * cb = (lv_checkbox_t *) obj;

    if(!cb->static_txt) lv_mem_free(cb->txt);

    cb->txt = (char*)txt;
    cb->static_txt = 1;

    lv_obj_handle_self_size_chg(obj);
}

/*=====================
 * Getter functions
 *====================*/

const char * lv_checkbox_get_text(const lv_obj_t * obj)
{
    lv_checkbox_t * cb = (lv_checkbox_t *) obj;
    return cb->txt;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_checkbox_constructor(lv_obj_t * obj, const lv_obj_t * copy)
{
    LV_LOG_TRACE("lv_checkbox create started");

    lv_checkbox_t * cb = (lv_checkbox_t *) obj;

    /*Init the new checkbox object*/
    if(copy == NULL) {
        cb->txt = "Check box";
        cb->static_txt = 1;
        lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
        lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    }
    else {
//        const lv_checkbox_t * copy_ext = (const lv_checkbox_t *)copy;
    }

    LV_LOG_INFO("lv_checkbox created");
}

static void lv_checkbox_destructor(lv_obj_t * obj)
{
    lv_checkbox_t * cb = (lv_checkbox_t *) obj;
    if(!cb->static_txt) {
        lv_mem_free(cb->txt);
        cb->txt = NULL;
    }
}

static lv_draw_res_t lv_checkbox_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    /* A label never covers an area */
    if(mode == LV_DRAW_MODE_COVER_CHECK)
        return lv_obj_draw_base(MY_CLASS, obj, clip_area, mode);
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {
        /*Draw the background*/
        lv_obj_draw_base(MY_CLASS, obj, clip_area, mode);

        lv_checkbox_t * cb = (lv_checkbox_t *) obj;

        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t font_h = lv_font_get_line_height(font);

        lv_coord_t bg_topp = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
        lv_coord_t bg_leftp = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        lv_coord_t bg_colp = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);

        lv_coord_t marker_leftp = lv_obj_get_style_pad_left(obj, LV_PART_MARKER);
        lv_coord_t marker_rightp = lv_obj_get_style_pad_right(obj, LV_PART_MARKER);
        lv_coord_t marker_topp = lv_obj_get_style_pad_top(obj, LV_PART_MARKER);
        lv_coord_t marker_bottomp = lv_obj_get_style_pad_bottom(obj, LV_PART_MARKER);

        lv_coord_t transf_w = lv_obj_get_style_transform_width(obj, LV_PART_MARKER);
        lv_coord_t transf_h = lv_obj_get_style_transform_height(obj, LV_PART_MARKER);

        lv_draw_rect_dsc_t marker_dsc;
        lv_draw_rect_dsc_init(&marker_dsc);
        lv_obj_init_draw_rect_dsc(obj, LV_PART_MARKER, &marker_dsc);
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

        lv_draw_rect(&marker_area_transf, clip_area, &marker_dsc);

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
    } else {
        lv_obj_draw_base(MY_CLASS, obj, clip_area, mode);
    }

    return LV_DRAW_RES_OK;
}

static lv_res_t lv_checkbox_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    lv_res_t res;
    /* Include the ancient signal function */
    res = lv_obj_signal_base(MY_CLASS, obj, sign, param);
    if(res != LV_RES_OK) return res;

    if (sign == LV_SIGNAL_GET_SELF_SIZE) {
        lv_point_t * p = param;
        lv_checkbox_t * cb = (lv_checkbox_t *) obj;

        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t font_h = lv_font_get_line_height(font);
        lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);

        lv_point_t txt_size;
        lv_txt_get_size(&txt_size, cb->txt, font, letter_space, line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);


        lv_coord_t bg_colp = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);
        lv_coord_t marker_leftp = lv_obj_get_style_pad_left(obj, LV_PART_MARKER);
        lv_coord_t marker_rightp = lv_obj_get_style_pad_right(obj, LV_PART_MARKER);
        lv_coord_t marker_topp = lv_obj_get_style_pad_top(obj, LV_PART_MARKER);
        lv_coord_t marker_bottomp = lv_obj_get_style_pad_bottom(obj, LV_PART_MARKER);
        lv_point_t marker_size;
        marker_size.x = font_h + marker_leftp + marker_rightp;
        marker_size.y = font_h + marker_topp + marker_bottomp;

        p->x = marker_size.x + txt_size.x + bg_colp;
        p->y = LV_MAX(marker_size.y, txt_size.y);
    }
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_SIZE) {
        lv_coord_t *s = param;
        lv_coord_t m = lv_obj_calculate_ext_draw_size(obj, LV_PART_MARKER);
        *s = LV_MAX(*s, m);
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        uint32_t v = lv_obj_get_state(obj) & LV_STATE_CHECKED ? 1 : 0;
        res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &v);
        if(res != LV_RES_OK) return res;

        lv_obj_invalidate(obj);
    }

    return res;
}

#endif
