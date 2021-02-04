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
static bool inited = false;

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
    static lv_style_t style_title;
    static lv_style_t style_btnm_bg;
    if(!inited) {
        lv_style_init(&style_title);
        lv_style_set_text_font(&style_title, LV_STATE_DEFAULT, lv_theme_get_font_subtitle());
        lv_style_set_margin_bottom(&style_title, LV_STATE_DEFAULT, LV_DPX(8));

        lv_style_init(&style_btnm_bg);
        lv_style_set_margin_top(&style_btnm_bg, LV_STATE_DEFAULT, LV_DPX(8));

        inited = true;
    }

    lv_obj_t * parent = lv_obj_create(lv_layer_top(), NULL);
    lv_obj_reset_style_list(parent, LV_OBJ_PART_MAIN);
    lv_obj_set_size(parent, LV_COORD_PCT(100), LV_COORD_PCT(100));

    lv_obj_t * mbox = lv_obj_create(parent, NULL);
    LV_ASSERT_MEM(mbox);
    if(mbox == NULL) return NULL;

    lv_coord_t w = lv_obj_get_width_fit(parent);
    if(w > 2 * LV_DPI) w = 2 * LV_DPI;

    lv_obj_set_size(mbox, w, LV_SIZE_AUTO);
    lv_obj_set_flex_dir(mbox, LV_FLEX_DIR_ROW_WRAP);

    lv_obj_t * label;
    label = lv_label_create(mbox, NULL);
    lv_label_set_text(label, title);
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label, add_close_btn ? LV_FLEX_GROW(1) : LV_COORD_PCT(100));
    lv_obj_set_flex_item(label, true);
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style_title);

    if(add_close_btn) {
        lv_obj_t * close_btn = lv_btn_create(mbox, NULL);
        lv_obj_add_style(close_btn, LV_BTN_PART_MAIN, &style_title);
        lv_obj_set_ext_click_area(close_btn, LV_DPX(10), LV_DPX(10), LV_DPX(10), LV_DPX(10));
        lv_obj_set_event_cb(close_btn, msgbox_close_event_cb);
        label = lv_label_create(close_btn, NULL);
        lv_label_set_text(label, LV_SYMBOL_CLOSE);
        lv_coord_t close_btn_size = LV_MAX(lv_obj_get_width(label), lv_obj_get_height(label)) + LV_DPX(10);
        lv_obj_set_size(close_btn, close_btn_size, close_btn_size);
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_flex_item_place(close_btn, LV_FLEX_PLACE_CENTER);
    }

    label = lv_label_create(mbox, NULL);
    lv_label_set_text(label, txt);
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label, LV_COORD_PCT(100));
    lv_obj_set_flex_item(label, LV_FLEX_PLACE_START);

    lv_obj_t * btns = lv_btnmatrix_create(mbox, NULL);
    lv_btnmatrix_set_map(btns, btn_txts);
    lv_obj_reset_style_list(btns, LV_BTNMATRIX_PART_MAIN);
    lv_obj_add_style(btns, LV_BTNMATRIX_PART_MAIN, &style_btnm_bg);
    lv_obj_set_flex_item(btns, LV_FLEX_PLACE_START);

    uint32_t btn_cnt = 0;
    while(btn_txts[btn_cnt][0] != '\0') {
        btn_cnt++;
    }

    const lv_font_t * font = lv_obj_get_style_text_font(btns, LV_BTNMATRIX_PART_BTN);
    lv_coord_t btn_h = lv_font_get_line_height(font) + LV_DPI / 10;
    lv_obj_set_size(btns, btn_cnt * (2 * LV_DPI / 3), btn_h);
    lv_obj_add_flag(btns, LV_OBJ_FLAG_EVENT_BUBBLE);    /*To see the event directly on the message box*/
    return mbox;
}


lv_obj_t * lv_msgbox_get_title(lv_obj_t * mbox)
{
    return lv_obj_get_child_by_id(mbox, 0);
}

lv_obj_t * lv_msgbox_get_close_btn(lv_obj_t * mbox)
{
    uint32_t cnt = lv_obj_count_children(mbox);
    if(cnt == 3) return NULL;   /*No close button*/
    else return lv_obj_get_child_by_id(mbox, 1);
}

lv_obj_t * lv_msgbox_get_text(lv_obj_t * mbox)
{
    return lv_obj_get_child_by_id(mbox, lv_obj_count_children(mbox) - 2);
}

lv_obj_t * lv_msgbox_get_btns(lv_obj_t * mbox)
{
    return lv_obj_get_child_by_id(mbox, lv_obj_count_children(mbox) - 1);
}

const char * lv_msgbox_get_active_btn_text(lv_obj_t * mbox)
{
    lv_obj_t * btnm = lv_msgbox_get_btns(mbox);
    return lv_btnmatrix_get_active_btn_text(btnm);
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
