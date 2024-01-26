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
#include "../image/lv_image.h"
#include "../../misc/lv_assert.h"
#include "../../display/lv_display.h"
#include "../../layouts/flex/lv_flex.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define LV_MSGBOX_FLAG_AUTO_PARENT  LV_OBJ_FLAG_WIDGET_1        /*Mark that the parent was automatically created*/
#define MY_CLASS (&lv_msgbox_class)

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
    .instance_size = sizeof(lv_msgbox_t),
    .name = "msgbox",
};

const lv_obj_class_t lv_msgbox_header_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_DPI_DEF / 3,
    .instance_size = sizeof(lv_obj_t),
    .name = "msgbox-header",
};

const lv_obj_class_t lv_msgbox_content_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_obj_t),
    .name = "msgbox-content",
};

const lv_obj_class_t lv_msgbox_footer_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_DPI_DEF / 3,
    .instance_size = sizeof(lv_obj_t),
    .name = "msgbox-footer",
};

const lv_obj_class_t lv_msgbox_footer_button_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_PCT(100),
    .instance_size = sizeof(lv_obj_t),
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .name = "msgbox-footer-button",
};

const lv_obj_class_t lv_msgbox_header_button_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_DPI_DEF / 3,
    .height_def = LV_PCT(100),
    .instance_size = sizeof(lv_obj_t),
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .name = "msgbox-header-button",
};

const lv_obj_class_t lv_msgbox_backdrop_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_PCT(100),
    .instance_size = sizeof(lv_obj_t),
    .name = "msgbox-backdrop",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_msgbox_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    bool auto_parent = false;
    if(parent == NULL) {
        auto_parent = true;
        parent = lv_obj_class_create_obj(&lv_msgbox_backdrop_class, lv_layer_top());
        LV_ASSERT_MALLOC(parent);
        lv_obj_class_init_obj(parent);
        lv_obj_remove_flag(parent, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_obj_set_size(parent, LV_PCT(100), LV_PCT(100));
    }

    lv_obj_t * obj = lv_obj_class_create_obj(&lv_msgbox_class, parent);
    LV_ASSERT_MALLOC(obj);
    if(obj == NULL) return NULL;
    lv_obj_class_init_obj(obj);
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);

    if(auto_parent) lv_obj_add_flag(obj, LV_MSGBOX_FLAG_AUTO_PARENT);

    mbox->content = lv_obj_class_create_obj(&lv_msgbox_content_class, obj);
    LV_ASSERT_MALLOC(obj);
    if(mbox->content == NULL) return NULL;
    lv_obj_class_init_obj(mbox->content);
    lv_obj_set_flex_flow(mbox->content, LV_FLEX_FLOW_COLUMN);

    lv_obj_center(obj);
    return obj;
}

lv_obj_t * lv_msgbox_add_title(lv_obj_t * obj, const char * title)
{
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    if(mbox->header == NULL) {
        mbox->header = lv_obj_class_create_obj(&lv_msgbox_header_class, obj);
        LV_ASSERT_MALLOC(obj);
        if(mbox->header == NULL) return NULL;
        lv_obj_class_init_obj(mbox->header);

        lv_obj_set_size(mbox->header, lv_pct(100), lv_display_get_dpi(lv_obj_get_display(obj)) / 3);
        lv_obj_set_flex_flow(mbox->header, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(mbox->header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_move_to_index(mbox->header, 0);
    }

    if(mbox->title == NULL) {
        mbox->title = lv_label_create(mbox->header);
        lv_obj_set_flex_grow(mbox->title, 1);
    }

    lv_label_set_text(mbox->title, title);

    return mbox->title;
}

lv_obj_t * lv_msgbox_add_header_button(lv_obj_t * obj, const void * icon)
{
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    if(mbox->header == NULL) {
        lv_msgbox_add_title(obj, ""); /*Just to push the buttons to the right*/
    }

    lv_obj_t * btn = lv_obj_class_create_obj(&lv_msgbox_header_button_class, mbox->header);
    LV_ASSERT_MALLOC(obj);
    if(btn == NULL) return NULL;
    lv_obj_class_init_obj(btn);

    if(icon) {
        lv_obj_t * img = lv_image_create(btn);
        lv_image_set_src(img, icon);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    }

    return btn;
}

lv_obj_t * lv_msgbox_add_text(lv_obj_t * obj, const char * text)
{
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;

    lv_obj_t * label = lv_label_create(mbox->content);
    lv_label_set_text(label, text);
    lv_obj_set_width(label, lv_pct(100));

    return label;
}

lv_obj_t * lv_msgbox_add_footer_button(lv_obj_t * obj, const char * text)
{
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    if(mbox->footer == NULL) {
        mbox->footer = lv_obj_class_create_obj(&lv_msgbox_footer_class, obj);
        LV_ASSERT_MALLOC(obj);
        if(mbox->footer == NULL) return NULL;
        lv_obj_class_init_obj(mbox->footer);

        lv_obj_set_flex_flow(mbox->footer, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(mbox->footer, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    }

    lv_obj_t * btn = lv_obj_class_create_obj(&lv_msgbox_footer_button_class, mbox->footer);
    LV_ASSERT_MALLOC(obj);
    if(btn == NULL) return NULL;
    lv_obj_class_init_obj(btn);

    if(text) {
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, text);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }

    return btn;
}

lv_obj_t * lv_msgbox_add_close_button(lv_obj_t * obj)
{
    lv_obj_t * btn = lv_msgbox_add_header_button(obj, LV_SYMBOL_CLOSE);
    lv_obj_add_event_cb(btn, msgbox_close_click_event_cb, LV_EVENT_CLICKED, NULL);
    return btn;
}

lv_obj_t * lv_msgbox_get_header(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    return mbox->header;
}

lv_obj_t * lv_msgbox_get_footer(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    return mbox->header;
}

lv_obj_t * lv_msgbox_get_content(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    return mbox->content;
}

lv_obj_t * lv_msgbox_get_title(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_msgbox_t * mbox = (lv_msgbox_t *)obj;
    return mbox->title;
}

void lv_msgbox_close(lv_obj_t * obj)
{
    if(lv_obj_has_flag(obj, LV_MSGBOX_FLAG_AUTO_PARENT)) lv_obj_delete(lv_obj_get_parent(obj));
    else lv_obj_delete(obj);
}

void lv_msgbox_close_async(lv_obj_t * obj)
{
    if(lv_obj_has_flag(obj, LV_MSGBOX_FLAG_AUTO_PARENT)) lv_obj_delete_async(lv_obj_get_parent(obj));
    else lv_obj_delete_async(obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void msgbox_close_click_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_current_target(e);
    lv_obj_t * mbox = lv_obj_get_parent(lv_obj_get_parent(btn));
    lv_msgbox_close(mbox);
}

#endif /*LV_USE_MSGBOX*/
