/**
 * @file lv_msgbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_msgbox.h"
#if LV_USE_MSGBOX

#include "../label/lv_label.h"
#include "../button/lv_button.h"
#include "../buttonmatrix/lv_buttonmatrix.h"
#include "../../misc/lv_assert.h"
#include "../../disp/lv_disp.h"
#include "../../layouts/flex/lv_flex.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define LV_MSGBOX_FLAG_AUTO_PARENT  LV_OBJ_FLAG_WIDGET_1        /*Mark that the parent was automatically created*/
#define MY_CLASS    &lv_msgbox_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void msgbox_close_click_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_msgbox_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_DPI_DEF * 2,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_msgbox_t)
};

const lv_obj_class_t lv_msgbox_content_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_obj_t)
};

const lv_obj_class_t lv_msgbox_backdrop_class = {
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

lv_obj_t * lv_msgbox_create(lv_obj_t * parent, const char * title, const char * txt, const char * btn_txts[],
                            bool add_close_btn)
{
    LV_LOG_INFO("begin");
    bool auto_parent = false;
    if(parent == NULL) {
        auto_parent = true;
        parent = lv_obj_class_create_obj(&lv_msgbox_backdrop_class, lv_layer_top());
        LV_ASSERT_MALLOC(parent);
        lv_obj_class_init_obj(parent);
        lv_obj_clear_flag(parent, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_obj_set_size(parent, LV_PCT(100), LV_PCT(100));
    }

    lv_obj_t * obj = lv_obj_class_create_obj(&lv_msgbox_class, parent);
    LV_ASSERT_MALLOC(obj);
    if(obj == NULL) return NULL;
    lv_obj_class_init_obj(obj);
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;

    if(auto_parent) lv_obj_add_flag(obj, LV_MSGBOX_FLAG_AUTO_PARENT);

    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW_WRAP);

    bool has_title = title && lv_strlen(title) > 0;

    /*When a close button is required, we need the empty label as spacer to push the button to the right*/
    if(add_close_btn || has_title) {
        mbox->title = lv_label_create(obj);
        lv_label_set_text(mbox->title, has_title ? title : "");
        lv_label_set_long_mode(mbox->title, LV_LABEL_LONG_SCROLL_CIRCULAR);
        if(add_close_btn) lv_obj_set_flex_grow(mbox->title, 1);
        else lv_obj_set_width(mbox->title, LV_PCT(100));
    }

    if(add_close_btn) {
        mbox->close_btn = lv_button_create(obj);
        lv_obj_set_ext_click_area(mbox->close_btn, LV_DPX(10));
        lv_obj_add_event(mbox->close_btn, msgbox_close_click_event_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_t * label = lv_label_create(mbox->close_btn);
        lv_label_set_text(label, LV_SYMBOL_CLOSE);
        const lv_font_t * font = lv_obj_get_style_text_font(mbox->close_btn, LV_PART_MAIN);
        lv_coord_t close_button_size = lv_font_get_line_height(font) + LV_DPX(10);
        lv_obj_set_size(mbox->close_btn, close_button_size, close_button_size);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }

    mbox->content = lv_obj_class_create_obj(&lv_msgbox_content_class, obj);
    LV_ASSERT_MALLOC(obj);
    if(mbox->content == NULL) return NULL;
    lv_obj_class_init_obj(mbox->content);

    bool has_txt = txt && lv_strlen(txt) > 0;
    if(has_txt) {
        mbox->text = lv_label_create(mbox->content);
        lv_label_set_text(mbox->text, txt);
        lv_label_set_long_mode(mbox->text, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(mbox->text, lv_pct(100));
    }

    if(btn_txts) {
        mbox->buttons = lv_buttonmatrix_create(obj);
        lv_buttonmatrix_set_map(mbox->buttons, btn_txts);
        lv_buttonmatrix_set_button_ctrl_all(mbox->buttons, LV_BUTTONMATRIX_CTRL_CLICK_TRIG | LV_BUTTONMATRIX_CTRL_NO_REPEAT);

        uint32_t btn_cnt = 0;
        while(btn_txts[btn_cnt] && btn_txts[btn_cnt][0] != '\0') {
            btn_cnt++;
        }

        const lv_font_t * font = lv_obj_get_style_text_font(mbox->buttons, LV_PART_ITEMS);
        lv_coord_t btn_h = lv_font_get_line_height(font) + LV_DPI_DEF / 10;
        lv_obj_set_size(mbox->buttons, btn_cnt * (2 * LV_DPI_DEF / 3), btn_h);
        lv_obj_set_style_max_width(mbox->buttons, lv_pct(100), 0);
        lv_obj_add_flag(mbox->buttons, LV_OBJ_FLAG_EVENT_BUBBLE);    /*To see the event directly on the message box*/
    }

    return obj;
}


lv_obj_t * lv_msgbox_get_title(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    return mbox->title;
}

lv_obj_t * lv_msgbox_get_close_button(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    return mbox->close_btn;
}

lv_obj_t * lv_msgbox_get_text(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    return mbox->text;
}

lv_obj_t * lv_msgbox_get_content(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    return mbox->content;
}

lv_obj_t * lv_msgbox_get_buttons(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    return mbox->buttons;
}

uint16_t lv_msgbox_get_active_button(lv_obj_t * mbox)
{
    lv_obj_t * btnm = lv_msgbox_get_buttons(mbox);
    return lv_buttonmatrix_get_selected_button(btnm);
}

const char * lv_msgbox_get_active_button_text(lv_obj_t * mbox)
{
    lv_obj_t * btnm = lv_msgbox_get_buttons(mbox);
    return lv_buttonmatrix_get_button_text(btnm, lv_buttonmatrix_get_selected_button(btnm));
}

void lv_msgbox_close(lv_obj_t * mbox)
{
    if(lv_obj_has_flag(mbox, LV_MSGBOX_FLAG_AUTO_PARENT)) lv_obj_del(lv_obj_get_parent(mbox));
    else lv_obj_del(mbox);
}

void lv_msgbox_close_async(lv_obj_t * dialog)
{
    if(lv_obj_has_flag(dialog, LV_MSGBOX_FLAG_AUTO_PARENT)) lv_obj_del_async(lv_obj_get_parent(dialog));
    else lv_obj_del_async(dialog);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void msgbox_close_click_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * mbox = lv_obj_get_parent(btn);
    lv_msgbox_close(mbox);
}

#endif /*LV_USE_MSGBOX*/
