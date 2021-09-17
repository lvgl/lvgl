/**
 * @file lv_dialog.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_dialog.h"
#if LV_USE_DIALOG


/*********************
 *      DEFINES
 *********************/
#define LV_DIALOG_FLAG_AUTO_PARENT  LV_OBJ_FLAG_WIDGET_1        /*Mark that the parent was automatically created*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void dialog_close_click_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_dialog_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_DPI_DEF * 2,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_dialog_t)
};

const lv_obj_class_t lv_dialog_content_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_obj_t)
};

const lv_obj_class_t lv_dialog_backdrop_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_PCT(100),
    .instance_size = sizeof(lv_obj_t)
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_dialog_create(lv_obj_t * parent, const char * title, const char * btn_txts[], bool add_close_btn)
{
    LV_LOG_INFO("begin");
    bool auto_parent = false;
    if(parent == NULL) {
        auto_parent = true;
        parent = lv_obj_class_create_obj(&lv_dialog_backdrop_class, lv_layer_top());
        lv_obj_clear_flag(parent, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_obj_set_size(parent, LV_PCT(100), LV_PCT(100));
    }

    lv_obj_t * obj = lv_obj_class_create_obj(&lv_dialog_class, parent);
    LV_ASSERT_MALLOC(obj);
    lv_obj_class_init_obj(obj);
    if(obj == NULL) return NULL;
    lv_dialog_t * dialog = (lv_dialog_t *)obj;

    if(auto_parent) lv_obj_add_flag(obj, LV_DIALOG_FLAG_AUTO_PARENT);

    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, 0);
    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_START, 0);

    bool has_title = title && strlen(title) > 0;

    /*When a close button is required, we need the empty label as spacer to push the button to the right*/
    if(add_close_btn || has_title) {
        dialog->title = lv_label_create(obj);
        lv_label_set_text(dialog->title, has_title ? title : "");
        lv_label_set_long_mode(dialog->title, LV_LABEL_LONG_SCROLL_CIRCULAR);
        if(add_close_btn) lv_obj_set_flex_grow(dialog->title, 1);
        else lv_obj_set_width(dialog->title, LV_PCT(100));
    }

    if(add_close_btn) {
        dialog->close_btn = lv_btn_create(obj);
        lv_obj_set_ext_click_area(dialog->close_btn, LV_DPX(10));
        lv_obj_add_event_cb(dialog->close_btn, dialog_close_click_event_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_t * label = lv_label_create(dialog->close_btn);
        lv_label_set_text(label, LV_SYMBOL_CLOSE);
        const lv_font_t * font = lv_obj_get_style_text_font(dialog->close_btn, LV_PART_MAIN);
        lv_coord_t close_btn_size = lv_font_get_line_height(font) + LV_DPX(10);
        lv_obj_set_size(dialog->close_btn, close_btn_size, close_btn_size);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }

    dialog->content = lv_obj_class_create_obj(&lv_dialog_content_class, obj);

    if(btn_txts) {
        dialog->btns = lv_btnmatrix_create(obj);
        lv_btnmatrix_set_map(dialog->btns, btn_txts);
        lv_btnmatrix_set_btn_ctrl_all(dialog->btns, LV_BTNMATRIX_CTRL_CLICK_TRIG | LV_BTNMATRIX_CTRL_NO_REPEAT);

        uint32_t btn_cnt = 0;
        while(btn_txts[btn_cnt] && btn_txts[btn_cnt][0] != '\0') {
            btn_cnt++;
        }

        const lv_font_t * font = lv_obj_get_style_text_font(dialog->btns, LV_PART_ITEMS);
        lv_coord_t btn_h = lv_font_get_line_height(font) + LV_DPI_DEF / 10;
        lv_obj_set_size(dialog->btns, btn_cnt * (2 * LV_DPI_DEF / 3), btn_h);
        lv_obj_add_flag(dialog->btns, LV_OBJ_FLAG_EVENT_BUBBLE);    /*To see the event directly on the message box*/
    }

    return obj;
}


lv_obj_t * lv_dialog_get_title(lv_obj_t * obj)
{
    lv_dialog_t * dialog = (lv_dialog_t *)obj;
    return dialog->title;
}

lv_obj_t * lv_dialog_get_close_btn(lv_obj_t * obj)
{
    lv_dialog_t * dialog = (lv_dialog_t *)obj;
    return dialog->close_btn;
}

lv_obj_t * lv_dialog_get_content(lv_obj_t * obj)
{
    lv_dialog_t * dialog = (lv_dialog_t *)obj;
    return dialog->content;
}

lv_obj_t * lv_dialog_get_btns(lv_obj_t * obj)
{
    lv_dialog_t * dialog = (lv_dialog_t *)obj;
    return dialog->btns;
}

uint16_t lv_dialog_get_active_btn(lv_obj_t * dialog)
{
    lv_obj_t * btnm = lv_dialog_get_btns(dialog);
    return lv_btnmatrix_get_selected_btn(btnm);
}

const char * lv_dialog_get_active_btn_text(lv_obj_t * dialog)
{
    lv_obj_t * btnm = lv_dialog_get_btns(dialog);
    return lv_btnmatrix_get_btn_text(btnm, lv_btnmatrix_get_selected_btn(btnm));
}

void lv_dialog_close(lv_obj_t * dialog)
{
    if(lv_obj_has_flag(dialog, LV_DIALOG_FLAG_AUTO_PARENT)) lv_obj_del(lv_obj_get_parent(dialog));
    else lv_obj_del(dialog);
}

void lv_dialog_close_async(lv_obj_t * dialog)
{
    if(lv_obj_has_flag(dialog, LV_DIALOG_FLAG_AUTO_PARENT)) lv_obj_del_async(lv_obj_get_parent(dialog));
    else lv_obj_del_async(dialog);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void dialog_close_click_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * dialog = lv_obj_get_parent(btn);
    lv_dialog_close(dialog);
}

#endif /*LV_USE_DIALOG*/
