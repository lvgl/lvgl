/**
 * @file lv_msgbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_msgbox.h"
#if LV_USE_MSGBOX


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void msgbox_close_event_cb(lv_obj_t * btn, lv_event_t e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_msgbox_class = {.base_class = &lv_obj_class};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a message box objects
 * @param par pointer to an object, it will be the parent of the new message box
 * @param copy pointer to a message box object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created message box
 */
lv_obj_t * lv_msgbox_create(const char * title, const char * txt, const char * btn_txts[], bool add_close_btn)
{
    lv_obj_t * parent = lv_obj_create(lv_layer_top(), NULL);
    lv_obj_set_size(parent, LV_SIZE_PCT(100), LV_SIZE_PCT(100));

    lv_obj_remove_style(parent, LV_PART_ANY, LV_STATE_ANY, NULL);
    lv_obj_set_style_bg_color(parent, LV_PART_MAIN, LV_STATE_DEFAULT, lv_color_grey());
    lv_obj_set_style_bg_opa(parent, LV_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_50);

    lv_obj_t * mbox = lv_obj_create_from_class(&lv_msgbox_class, parent, NULL);
    LV_ASSERT_MALLOC(mbox);
    if(mbox == NULL) return NULL;

    lv_coord_t w = lv_obj_get_width_fit(parent);
    if(w > 2 * LV_DPI_DEF) w = 2 * LV_DPI_DEF;

    lv_obj_set_size(mbox, w, LV_SIZE_CONTENT);
    lv_obj_set_layout(mbox, &lv_flex_inline);

    lv_obj_t * label;
    label = lv_label_create(mbox, NULL);
    lv_label_set_text(label, title);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SROLL_CIRC);
    if(add_close_btn) lv_obj_set_flex_grow(label, 1);
    else lv_obj_set_width(label, LV_SIZE_PCT(100));

    if(add_close_btn) {
        lv_obj_t * close_btn = lv_btn_create(mbox, NULL);
        lv_obj_set_ext_click_area(close_btn, LV_DPX(10));
        lv_obj_add_event_cb(close_btn, msgbox_close_event_cb, NULL);
        label = lv_label_create(close_btn, NULL);
        lv_label_set_text(label, LV_SYMBOL_CLOSE);
        lv_coord_t close_btn_size = LV_MAX(lv_obj_get_width(label), lv_obj_get_height(label)) + LV_DPX(10);
        lv_obj_set_size(close_btn, close_btn_size, close_btn_size);
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    }

    label = lv_label_create(mbox, NULL);
    lv_label_set_text(label, txt);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, LV_SIZE_PCT(100));

    lv_obj_t * btns = lv_btnmatrix_create(mbox, NULL);
    lv_btnmatrix_set_map(btns, btn_txts);

    uint32_t btn_cnt = 0;
    while(btn_txts[btn_cnt][0] != '\0') {
        btn_cnt++;
    }

    const lv_font_t * font = lv_obj_get_style_text_font(btns, LV_PART_ITEMS);
    lv_coord_t btn_h = lv_font_get_line_height(font) + LV_DPI_DEF / 10;
    lv_obj_set_size(btns, btn_cnt * (2 * LV_DPI_DEF / 3), btn_h);
    lv_obj_add_flag(btns, LV_OBJ_FLAG_EVENT_BUBBLE);    /*To see the event directly on the message box*/
    return mbox;
}


lv_obj_t * lv_msgbox_get_title(lv_obj_t * mbox)
{
    return lv_obj_get_child(mbox, 0);
}

lv_obj_t * lv_msgbox_get_close_btn(lv_obj_t * mbox)
{
    lv_obj_t * obj = lv_obj_get_child(mbox, 1);
    if(lv_obj_check_type(obj, &lv_btn_class)) return obj;
    else return NULL;
}

lv_obj_t * lv_msgbox_get_text(lv_obj_t * mbox)
{
    return lv_obj_get_child(mbox, lv_obj_get_child_cnt(mbox) - 2);
}

lv_obj_t * lv_msgbox_get_btns(lv_obj_t * mbox)
{
    return lv_obj_get_child(mbox, lv_obj_get_child_cnt(mbox) - 1);
}

const char * lv_msgbox_get_active_btn_text(lv_obj_t * mbox)
{
    lv_obj_t * btnm = lv_msgbox_get_btns(mbox);
    return lv_btnmatrix_get_btn_text(btnm, lv_btnmatrix_get_active_btn(btnm));
}

void lv_msgbox_close(lv_obj_t * mbox)
{
    lv_obj_del(lv_obj_get_parent(mbox));
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void msgbox_close_event_cb(lv_obj_t * btn, lv_event_t e)
{
    if(e == LV_EVENT_CLICKED) {
        lv_obj_t * mbox = lv_obj_get_parent(btn);
        lv_msgbox_close(mbox);
    }
}

#endif /*LV_USE_MSGBOX*/
