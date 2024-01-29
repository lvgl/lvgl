/**
 * @file lv_tabview.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tabview.h"
#if LV_USE_TABVIEW

#include "../../misc/lv_assert.h"
#include "../../indev/lv_indev_private.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_tabview_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tabview_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_tabview_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void button_clicked_event_cb(lv_event_t * e);
static void cont_scroll_end_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_tabview_class = {
    .constructor_cb = lv_tabview_constructor,
    .event_cb = lv_tabview_event,
    .width_def = LV_PCT(100),
    .height_def = LV_PCT(100),
    .base_class = &lv_obj_class,
    .instance_size = sizeof(lv_tabview_t),
    .name = "tabview",
};

typedef struct {
    lv_dir_t tab_pos;
    int32_t tab_size;
} lv_tabview_create_info_t;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_tabview_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");

    lv_obj_t * obj = lv_obj_class_create_obj(&lv_tabview_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

lv_obj_t * lv_tabview_add_tab(lv_obj_t * obj, const char * name)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_t * cont = lv_tabview_get_content(obj);

    lv_obj_t * page = lv_obj_create(cont);
    lv_obj_set_size(page, lv_pct(100), lv_pct(100));
    uint32_t tab_idx = lv_obj_get_child_count(cont);

    lv_obj_t * tab_bar = lv_tabview_get_tab_bar(obj);

    lv_obj_t * button = lv_button_create(tab_bar);
    lv_obj_set_flex_grow(button, 1);
    lv_obj_set_size(button, lv_pct(100), lv_pct(100));
    lv_obj_add_event_cb(button, button_clicked_event_cb, LV_EVENT_CLICKED, NULL);
    lv_group_t * g = lv_group_get_default();
    if(g) lv_group_add_obj(g, button);

    lv_obj_t * label = lv_label_create(button);
    lv_label_set_text(label, name);
    lv_obj_center(label);

    if(tab_idx == 1) {
        lv_tabview_set_active(obj, 0, LV_ANIM_OFF);
    }

    return page;
}

void lv_tabview_rename_tab(lv_obj_t * obj, uint32_t idx, const char * new_name)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_t * tab_bar = lv_tabview_get_tab_bar(obj);
    lv_obj_t * button = lv_obj_get_child_by_type(tab_bar, idx, &lv_button_class);
    lv_obj_t * label = lv_obj_get_child_by_type(button, 0, &lv_label_class);
    lv_label_set_text(label, new_name);
}

void lv_tabview_set_active(lv_obj_t * obj, uint32_t idx, lv_anim_enable_t anim_en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_tabview_t * tabview = (lv_tabview_t *)obj;

    lv_obj_t * cont = lv_tabview_get_content(obj);
    lv_obj_t * tab_bar = lv_tabview_get_tab_bar(obj);

    uint32_t tab_cnt = lv_tabview_get_tab_count(obj);
    if(idx >= tab_cnt) {
        idx = tab_cnt - 1;
    }

    /*To be sure lv_obj_get_content_width will return valid value*/
    lv_obj_update_layout(obj);

    if(cont == NULL) return;

    if((tabview->tab_pos & LV_DIR_VER) != 0) {
        int32_t gap = lv_obj_get_style_pad_column(cont, LV_PART_MAIN);
        int32_t w = lv_obj_get_content_width(cont);
        if(lv_obj_get_style_base_dir(obj, LV_PART_MAIN) != LV_BASE_DIR_RTL) {
            lv_obj_scroll_to_x(cont, idx * (gap + w), anim_en);
        }
        else {
            int32_t id_rtl = -(int32_t)idx;
            lv_obj_scroll_to_x(cont, (gap + w) * id_rtl, anim_en);
        }
    }
    else {
        int32_t gap = lv_obj_get_style_pad_row(cont, LV_PART_MAIN);
        int32_t h = lv_obj_get_content_height(cont);
        lv_obj_scroll_to_y(cont, idx * (gap + h), anim_en);
    }

    uint32_t i = 0;
    lv_obj_t * button = lv_obj_get_child_by_type(tab_bar, i, &lv_button_class);
    while(button) {
        lv_obj_set_state(button, LV_STATE_CHECKED, i == idx);
        i++;
        button = lv_obj_get_child_by_type(tab_bar, (int32_t)i, &lv_button_class);
    }

    tabview->tab_cur = idx;
}

void lv_tabview_set_tab_bar_position(lv_obj_t * obj, lv_dir_t dir)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_tabview_t * tabview = (lv_tabview_t *)obj;

    switch(dir) {
        case LV_DIR_TOP:
            lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
            break;
        case LV_DIR_BOTTOM:
            lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN_REVERSE);
            break;
        case LV_DIR_LEFT:
            lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
            break;
        case LV_DIR_RIGHT:
            lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW_REVERSE);
            break;
    }

    lv_obj_t * tab_bar = lv_tabview_get_tab_bar(obj);
    lv_obj_t * cont = lv_tabview_get_content(obj);

    switch(dir) {
        case LV_DIR_TOP:
        case LV_DIR_BOTTOM:
            lv_obj_set_width(cont, LV_PCT(100));
            lv_obj_set_flex_grow(cont, 1);
            lv_obj_set_flex_flow(tab_bar, LV_FLEX_FLOW_ROW);
            lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
            lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
            lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_NONE);
            break;
        case LV_DIR_LEFT:
        case LV_DIR_RIGHT:
            lv_obj_set_height(cont, LV_PCT(100));
            lv_obj_set_flex_grow(cont, 1);
            lv_obj_set_flex_flow(tab_bar, LV_FLEX_FLOW_COLUMN);
            lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
            lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_NONE);
            lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
            break;
    }

    bool was_ver = tabview->tab_pos & LV_DIR_VER;
    bool now_ver = dir & LV_DIR_VER;

    if(was_ver != now_ver) {
        int32_t dpi = lv_display_get_dpi(lv_obj_get_display(obj));
        if(now_ver) {
            lv_obj_set_size(tab_bar, lv_pct(100), dpi / 2);
        }
        else {
            lv_obj_set_size(tab_bar, dpi, lv_pct(100));
        }
    }
    tabview->tab_pos = dir;
}

void lv_tabview_set_tab_bar_size(lv_obj_t * obj, int32_t size)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_tabview_t * tabview = (lv_tabview_t *)obj;

    lv_obj_t * tab_bar = lv_tabview_get_tab_bar(obj);
    if(tabview->tab_pos & LV_DIR_VER) {
        lv_obj_set_height(tab_bar, size);
    }
    else {
        lv_obj_set_width(tab_bar, size);
    }

}

uint32_t lv_tabview_get_tab_active(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_tabview_t * tabview = (lv_tabview_t *)obj;
    return tabview->tab_cur;
}

uint32_t lv_tabview_get_tab_count(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_t * tab_bar = lv_tabview_get_tab_bar(obj);
    return lv_obj_get_child_count_by_type(tab_bar, &lv_button_class);
}

lv_obj_t * lv_tabview_get_content(lv_obj_t * tv)
{
    return lv_obj_get_child(tv, 1);
}

lv_obj_t * lv_tabview_get_tab_bar(lv_obj_t * tv)
{
    return lv_obj_get_child(tv, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_tabview_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_tabview_t * tabview = (lv_tabview_t *)obj;
    tabview->tab_pos = LV_DIR_NONE;  /*Invalid value to apply the default TOP direction correctly*/

    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));

    lv_obj_t * cont;

    lv_obj_create(obj);
    cont = lv_obj_create(obj);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);

    lv_obj_add_event_cb(cont, cont_scroll_end_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
    lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);

    lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
}

static void lv_tabview_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    lv_result_t res = lv_obj_event_base(&lv_tabview_class, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_current_target(e);

    if(code == LV_EVENT_SIZE_CHANGED) {
        lv_tabview_set_active(target, lv_tabview_get_tab_active(target), LV_ANIM_OFF);
    }
}

static void button_clicked_event_cb(lv_event_t * e)
{
    lv_obj_t * button = lv_event_get_current_target(e);

    lv_obj_t * tv = lv_obj_get_parent(lv_obj_get_parent(button));
    int32_t idx = lv_obj_get_index_by_type(button, &lv_button_class);
    lv_tabview_set_active(tv, idx, LV_ANIM_OFF);
}

static void cont_scroll_end_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_current_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    lv_obj_t * tv = lv_obj_get_parent(cont);
    lv_tabview_t * tv_obj = (lv_tabview_t *)tv;
    if(code == LV_EVENT_LAYOUT_CHANGED) {
        lv_tabview_set_active(tv, lv_tabview_get_tab_active(tv), LV_ANIM_OFF);
    }
    else if(code == LV_EVENT_SCROLL_END) {
        lv_indev_t * indev = lv_indev_active();
        if(indev && indev->state == LV_INDEV_STATE_PRESSED) {
            return;
        }

        lv_point_t p;
        lv_obj_get_scroll_end(cont, &p);

        int32_t t;
        if((tv_obj->tab_pos & LV_DIR_VER) != 0) {
            int32_t w = lv_obj_get_content_width(cont);
            if(lv_obj_get_style_base_dir(tv, LV_PART_MAIN) == LV_BASE_DIR_RTL)  t = -(p.x - w / 2) / w;
            else t = (p.x + w / 2) / w;
        }
        else {
            int32_t h = lv_obj_get_content_height(cont);
            t = (p.y + h / 2) / h;
        }

        if(t < 0) t = 0;
        bool new_tab = false;
        if(t != (int32_t)lv_tabview_get_tab_active(tv)) new_tab = true;

        /*If not scrolled by an indev set the tab immediately*/
        if(lv_indev_active()) {
            lv_tabview_set_active(tv, t, LV_ANIM_ON);
        }
        else {
            lv_tabview_set_active(tv, t, LV_ANIM_OFF);
        }

        if(new_tab) lv_obj_send_event(tv, LV_EVENT_VALUE_CHANGED, NULL);
    }
}
#endif /*LV_USE_TABVIEW*/
