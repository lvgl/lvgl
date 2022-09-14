/**
 * @file lv_radiobtn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_radiobtn.h"

#if LV_USE_RADIOBTN != 0

#include "../../misc/lv_assert.h"
#include "../../core/lv_group.h"
#include "../../draw/lv_draw.h"
#include "../../misc/lv_txt_ap.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_radiobtn_class
#define ITEM_CLASS &lv_radiobtn_item_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_radiobtn_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_radiobtn_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_radiobtn_item_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_radiobtn_item_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_radiobtn_item_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_radiobtn_item_draw(lv_event_t * e);


/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_radiobtn_class = {
    .constructor_cb = lv_radiobtn_constructor,
    .event_cb = lv_radiobtn_event,
    .instance_size = sizeof(lv_radiobtn_t),
    .width_def = (LV_DPI_DEF * 3) / 4,
    .height_def = (LV_DPI_DEF * 3) / 4,
    .base_class = &lv_obj_class
};

const lv_obj_class_t lv_radiobtn_item_class = {
    .constructor_cb = lv_radiobtn_item_constructor,
    .destructor_cb = lv_radiobtn_item_destructor,
    .event_cb = lv_radiobtn_item_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .instance_size = sizeof(lv_radiobtn_item_t),
    .base_class = &lv_obj_class
};



/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_radiobtn_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

lv_obj_t * lv_radiobtn_create_item(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(ITEM_CLASS, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

lv_obj_t * lv_radiobtn_add_item(lv_obj_t * parent, const char * txt)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_radiobtn_create_item(parent);
    lv_obj_class_init_obj(obj);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_pad_all(obj, 3, LV_PART_MAIN);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
    if(txt) {
        lv_radiobtn_set_item_text(obj, txt);
    }

    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_radiobtn_set_item_text(lv_obj_t * obj, const char * txt)
{
    LV_LOG_INFO("begin");
    lv_radiobtn_item_t * rb = (lv_radiobtn_item_t *)obj;
#if LV_USE_ARABIC_PERSIAN_CHARS
    size_t len = _lv_txt_ap_calc_bytes_cnt(txt);
#else
    size_t len = strlen(txt);
#endif

    if(!rb->static_txt) rb->txt = lv_realloc(rb->txt, len + 1);
    else  rb->txt = lv_malloc(len + 1);
#if LV_USE_ARABIC_PERSIAN_CHARS
    _lv_txt_ap_proc(txt, rb->txt);
#else
    if(rb->txt != NULL) strncpy(rb->txt, txt, len + 1);
#endif

    rb->static_txt = 0;

    lv_obj_refresh_self_size(obj);
    lv_obj_invalidate(obj);
}

void lv_radiobtn_set_item_text_static(lv_obj_t * obj, const char * txt)
{
    lv_radiobtn_item_t * rb = (lv_radiobtn_item_t *)obj;

    if(!rb->static_txt) lv_free(rb->txt);

    rb->txt = (char *)txt;
    rb->static_txt = 1;

    lv_obj_refresh_self_size(obj);
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

lv_obj_t * lv_radiobtn_get_item(lv_obj_t * radiobtn, uint32_t index)
{
    LV_LOG_INFO("begin");
    lv_obj_t * item = lv_obj_get_child(radiobtn, index);
    return item;
}

const char * lv_radiobtn_get_item_text(lv_obj_t * radiobtn, lv_obj_t * obj)
{
    LV_UNUSED(radiobtn);

    lv_radiobtn_item_t * rb = (lv_radiobtn_item_t *)obj;
    return rb->txt;
}

uint32_t lv_radiobtn_get_item_num(lv_obj_t * radiobtn)
{
    uint32_t num = lv_obj_get_child_cnt(radiobtn);
    return num;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void lv_radiobtn_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_radiobtn_t * rb = (lv_radiobtn_t *)obj;
    rb->active_index = 0;
    rb->checked_txt = "";
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_radiobtn_item_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_radiobtn_item_t * btn = (lv_radiobtn_item_t *)obj;

    btn->txt = "radio";
    btn->static_txt = 1;
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_radiobtn_item_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_radiobtn_item_t * btn = (lv_radiobtn_item_t *)obj;
    if(!btn->static_txt) {
        lv_free(btn->txt);
        btn->txt = NULL;
    }
    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_radiobtn_item_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;
    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(ITEM_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_GET_SELF_SIZE) {
        lv_point_t * p = lv_event_get_param(e);
        lv_radiobtn_item_t * btn = (lv_radiobtn_item_t *)obj;

        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t font_h = lv_font_get_line_height(font);
        lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);

        lv_point_t txt_size;
        lv_txt_get_size(&txt_size, btn->txt, font, letter_space, line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

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
        lv_coord_t * s = lv_event_get_param(e);
        lv_coord_t m = lv_obj_calculate_ext_draw_size(obj, LV_PART_INDICATOR);
        *s = LV_MAX(*s, m);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        lv_radiobtn_item_draw(e);
    }
}

static void lv_radiobtn_item_draw(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_radiobtn_item_t * btn = (lv_radiobtn_item_t *)obj;

    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
    if(draw_ctx == NULL) return;
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    lv_coord_t font_h = lv_font_get_line_height(font);

    lv_coord_t bg_border = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t bg_topp = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + bg_border;
    lv_coord_t bg_leftp = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + bg_border;
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

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.rect_dsc = &indic_dsc;
    part_draw_dsc.class_p = ITEM_CLASS;
    part_draw_dsc.type = LV_RADIOBTN_DRAW_PART_BOX;
    part_draw_dsc.draw_area = &marker_area_transf;
    part_draw_dsc.part = LV_PART_INDICATOR;

    lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
    lv_draw_rect(draw_ctx, &indic_dsc, &marker_area_transf);
    lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);

    /**draw inner box*/
    lv_draw_rect_dsc_t indic_inner_dsc;
    lv_draw_rect_dsc_init(&indic_inner_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_CUSTOM_FIRST, &indic_inner_dsc);

    lv_area_t marker_inner_area_transf;
    lv_area_copy(&marker_inner_area_transf, &marker_area_transf);
    int16_t dist = ((int16_t)(marker_inner_area_transf.x2 - marker_inner_area_transf.x1) / 4);
    marker_inner_area_transf.x1 += dist;
    marker_inner_area_transf.x2 -= dist;
    marker_inner_area_transf.y1 += dist;
    marker_inner_area_transf.y2 -= dist;

    part_draw_dsc.rect_dsc = &indic_inner_dsc;
    part_draw_dsc.class_p = ITEM_CLASS;
    part_draw_dsc.type = LV_RADIOBTN_DRAW_PART_BOX_INNER;
    part_draw_dsc.draw_area = &marker_inner_area_transf;
    part_draw_dsc.part = LV_PART_CUSTOM_FIRST;

    lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
    lv_draw_rect(draw_ctx, &indic_inner_dsc, &marker_inner_area_transf);
    lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);

    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);

    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, btn->txt, font, letter_space, line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

    lv_draw_label_dsc_t txt_dsc;
    lv_draw_label_dsc_init(&txt_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &txt_dsc);

    lv_coord_t y_ofs = (lv_area_get_height(&marker_area) - font_h) / 2;
    lv_area_t txt_area;
    txt_area.x1 = marker_area.x2 + bg_colp;
    txt_area.x2 = txt_area.x1 + txt_size.x;
    txt_area.y1 = obj->coords.y1 + bg_topp + y_ofs;
    txt_area.y2 = txt_area.y1 + txt_size.y;

    lv_draw_label(draw_ctx, &txt_dsc, &txt_area, btn->txt, NULL);
}

static void lv_radiobtn_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;
    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_obj_t * act_btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        lv_radiobtn_t * rb = (lv_radiobtn_t *)obj;
        uint16_t active_id = rb->active_index;
        lv_obj_t * old_btn = lv_radiobtn_get_item(obj, active_id);

        if(act_btn == obj || old_btn == NULL) return;

        lv_obj_clear_state(old_btn, LV_STATE_CHECKED);
        lv_obj_add_state(act_btn, LV_STATE_CHECKED);

        rb->active_index = lv_obj_get_index(act_btn);
        rb->checked_txt = (char *)lv_radiobtn_get_item_text(obj, act_btn);

        LV_LOG_USER("Selected radio buttons: %d ", (int)rb->active_index);
    }

}

#endif /*LV_USE_RADIOBTN*/
