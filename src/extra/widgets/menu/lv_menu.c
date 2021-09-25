/**
 * @file lv_menu.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_menu.h"

#define LV_USE_MENU 1
#if LV_USE_MENU

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_menu_class

#include "../../../core/lv_obj.h"
#include "../../layouts/flex/lv_flex.h"
#include "../../../widgets/lv_label.h"
#include "../../../widgets/lv_img.h"
#include "../../../widgets/lv_btn.h"

#if LV_USE_SLIDER
    #include "../../../widgets/lv_slider.h"
#endif

#if LV_USE_SWITCH
    #include "../../../widgets/lv_switch.h"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_menu_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_menu_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

const lv_obj_class_t lv_menu_class = {
    .constructor_cb = lv_menu_constructor,
    .destructor_cb = lv_menu_destructor,
    .base_class = &lv_obj_class,
    .width_def = (LV_DPI_DEF * 3) / 2,
    .height_def = LV_DPI_DEF * 2,
    .instance_size = sizeof(lv_menu_t)
};

const lv_obj_class_t lv_menu_cont_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_menu_cont_t)
};

const lv_obj_class_t lv_menu_section_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT
};

const lv_obj_class_t lv_menu_seperator_class = {
    .base_class = &lv_obj_class
};

const lv_obj_class_t lv_menu_sidebar_cont_class = {
    .base_class = &lv_obj_class
};

const lv_obj_class_t lv_menu_main_cont_class = {
    .base_class = &lv_obj_class
};

const lv_obj_class_t lv_menu_main_header_cont_class = {
    .base_class = &lv_obj_class
};

const lv_obj_class_t lv_menu_sidebar_header_cont_class = {
    .base_class = &lv_obj_class
};

const lv_obj_class_t lv_menu_sidebar_content_cont_class = {
    .base_class = &lv_obj_class
};

const lv_obj_class_t lv_menu_main_content_cont_class = {
    .base_class = &lv_obj_class
};

static void lv_menu_item_del_internal(lv_menu_item_t * menu_item, bool recursive, bool del_child);
static void lv_menu_generate(lv_obj_t * obj, lv_menu_item_t * menu_item, bool sidebar);
static void lv_menu_clear_contents(lv_obj_t * obj, lv_obj_t * target_content);
static void lv_menu_traverse_event_cb(lv_event_t * e);
static void lv_menu_back_event_cb(lv_event_t * e);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_menu_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_menu_set(lv_obj_t * obj, lv_menu_item_t * menu_item)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    /* Clean or move objects on screen */
    lv_menu_clear_contents(obj, menu->main_content);

    if(menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_DISABLED) {
        lv_menu_clear_contents(obj, menu->sidebar_content);
    }

    lv_ll_t * history_ll = &(menu->history_ll);

    /* The root menu */
    lv_menu_history_t * root_hist = _lv_ll_get_tail(history_ll);

    /* The current menu */
    lv_menu_history_t * act_hist = _lv_ll_get_head(history_ll);

    /* The previous menu */
    lv_menu_history_t * prev_hist = NULL;
    if(act_hist != NULL) {
        prev_hist = _lv_ll_get_next(history_ll, act_hist);
    }

    /* Add a new node */
    lv_menu_history_t * new_node = _lv_ll_ins_head(history_ll);
    new_node->menu_item = menu_item;

    /* Back btn management */
    /* Show/hide btns if required */
    if(menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED) {
        /* With sidebar enabled */
        lv_obj_clear_flag(menu->sidebar, LV_OBJ_FLAG_HIDDEN);

        if(menu->mode_root_back_btn == LV_MENU_MODE_ROOT_BACK_BTN_ENABLED) {
            /* Root back btn is always shown if enabled*/
            lv_obj_clear_flag(menu->sidebar_back_btn, LV_OBJ_FLAG_HIDDEN);
        }
        else {
            lv_obj_add_flag(menu->sidebar_back_btn, LV_OBJ_FLAG_HIDDEN);
        }

        if(prev_hist != NULL) {
            lv_obj_clear_flag(menu->main_back_btn, LV_OBJ_FLAG_HIDDEN);
        }
        else {
            lv_obj_add_flag(menu->main_back_btn, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else {
        /* With sidebar disabled */
        lv_obj_add_flag(menu->sidebar, LV_OBJ_FLAG_HIDDEN);

        if(act_hist != NULL || menu->mode_root_back_btn == LV_MENU_MODE_ROOT_BACK_BTN_ENABLED) {
            lv_obj_clear_flag(menu->main_back_btn, LV_OBJ_FLAG_HIDDEN);
        }
        else {
            lv_obj_add_flag(menu->main_back_btn, LV_OBJ_FLAG_HIDDEN);
        }
    }

    lv_menu_generate(obj, menu_item, menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED && act_hist == NULL ? true : false);

    if(menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED) {
        /* Check that sidebar contents are not removed, if not regenerate them */
        if(root_hist != NULL) {
            uint32_t menu_content_child_cnt = lv_obj_get_child_cnt(menu->sidebar_content);
            uint32_t menu_content_actual_child_cnt = 0;
            for(uint32_t  i = 0; i < menu_content_child_cnt; i++) {
                lv_obj_t * child = lv_obj_get_child(menu->sidebar_content, i);
                menu_content_actual_child_cnt = lv_obj_check_type(child,
                                                                  &lv_menu_section_class) ? menu_content_actual_child_cnt + lv_obj_get_child_cnt(child) : menu_content_actual_child_cnt +
                                                1;
            }

            if(root_hist->menu_item->child_cnt != menu_content_actual_child_cnt) {
                lv_menu_generate(obj, root_hist->menu_item, true);
            }

            /* Check selected obj */
            if(menu->selected_tab != NULL) {
                lv_obj_add_state(menu->selected_tab, LV_STATE_CHECKED);
            }
        }
    }

    /* Refresh size */
    lv_obj_refr_size(menu->main_header);
    lv_obj_refr_size(menu->sidebar_header);
}

void lv_menu_set_mode_header(lv_obj_t * obj, lv_menu_mode_header_t mode_header)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->mode_header != mode_header) {
        menu->mode_header = mode_header;
        if(menu->mode_header == LV_MENU_MODE_HEADER_FIXED) {
            /* Fill the remaining space */
            lv_obj_set_flex_grow(menu->main_content, 1);
            lv_obj_set_flex_grow(menu->sidebar_content, 1);
        }
        else {
            /* Allow it to be larger */
            lv_obj_set_flex_grow(menu->main_content, 0);
            lv_obj_set_flex_grow(menu->sidebar_content, 0);
        }
    }
}

void lv_menu_set_mode_root_back_btn(lv_obj_t * obj, lv_menu_mode_root_back_btn_t mode_root_back_btn)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->mode_root_back_btn != mode_root_back_btn) {
        menu->mode_root_back_btn = mode_root_back_btn;
        lv_menu_refresh(obj);
    }
}

void lv_menu_set_mode_sidebar(lv_obj_t * obj, lv_menu_mode_sidebar_t mode_sidebar)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->mode_sidebar != mode_sidebar) {
        if(menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED) {
            /* Clear previous checked state */
            if(menu->selected_tab != NULL) {
                lv_obj_clear_state(menu->selected_tab, LV_STATE_CHECKED);
            }
        }

        menu->mode_sidebar = mode_sidebar;

        lv_menu_refresh(obj);
    }
}

bool lv_menu_item_back_btn_is_root(lv_obj_t * obj)
{
    if(lv_obj_check_type(obj, &lv_menu_cont_class)) {
        lv_menu_cont_t * cont = (lv_menu_cont_t *)obj;
        if(cont->mode_root_back_btn == LV_MENU_MODE_ROOT_BACK_BTN_ENABLED) {
            return true;
        }
    }
    return false;
}

void lv_menu_set_main_back_btn_text(lv_obj_t * obj, const char * icon, const char * txt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_t * img = lv_obj_get_child(((lv_menu_t *)(obj))->main_back_btn, 0);
    lv_obj_t * label = lv_obj_get_child(((lv_menu_t *)(obj))->main_back_btn, 1);

    if(img != NULL) {
        lv_img_set_src(img, icon);
    }

    if(label != NULL) {
        lv_label_set_text(label, txt);
    }
}

void lv_menu_set_sidebar_back_btn_text(lv_obj_t * obj, const char * icon, const char * txt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_t * img = lv_obj_get_child(((lv_menu_t *)(obj))->sidebar_back_btn, 0);
    lv_obj_t * label = lv_obj_get_child(((lv_menu_t *)(obj))->sidebar_back_btn, 1);

    if(img != NULL) {
        lv_img_set_src(img, icon);
    }

    if(label != NULL) {
        lv_label_set_text(label, txt);
    }
}

/* Core menu item functions */
lv_menu_item_t * lv_menu_item_create()
{
    lv_menu_item_t * menu_item = lv_mem_alloc(sizeof(lv_menu_item_t));
    menu_item->obj = NULL;
    menu_item->items = NULL;
    menu_item->child_cnt = 0;
    menu_item->section = LV_MENU_SECTION_ON;
    return menu_item;
}

void lv_menu_item_del(lv_menu_item_t * menu_item)
{
    lv_menu_item_del_internal(menu_item, false, true);
}

void lv_menu_item_del_recursive(lv_menu_item_t * menu_item)
{
    lv_menu_item_del_internal(menu_item, true, true);
}

lv_obj_t * lv_menu_item_set_obj(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_obj_t * obj)
{
    LV_ASSERT_OBJ(menu, MY_CLASS);
    LV_ASSERT_OBJ(cont, &lv_menu_cont_class); /* Must be `lv_menu_cont_class` */
    menu_item->obj = obj;

    /* Make the object clickable */
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    lv_menu_cont_t * cont = (lv_menu_cont_t *)obj;
    cont->menu = menu;
    cont->menu_item = menu_item;
    cont->mode_sidebar = LV_MENU_MODE_SIDEBAR_DISABLED;
    cont->mode_root_back_btn = LV_MENU_MODE_ROOT_BACK_BTN_DISABLED;

    lv_obj_add_event_cb(obj, lv_menu_traverse_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_set_parent(obj, ((lv_menu_t *)menu)->storage);

    return obj;
}

lv_obj_t * lv_menu_item_add_obj(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                lv_obj_t * obj)
{
    LV_ASSERT_OBJ(menu, MY_CLASS);

    menu_item->child_cnt++;

    menu_item->items = lv_mem_realloc(menu_item->items, menu_item->child_cnt * sizeof(lv_menu_item_t *));

    lv_menu_item_t * new_menu_item = lv_menu_item_create();
    new_menu_item->obj = obj;
    new_menu_item->items = NULL;
    new_menu_item->child_cnt = 0;
    new_menu_item->section = section;
    menu_item->items[menu_item->child_cnt - 1] = new_menu_item;

    lv_obj_set_parent(menu_item->items[menu_item->child_cnt - 1]->obj, ((lv_menu_t *)menu)->storage);

    return menu_item->items[menu_item->child_cnt - 1]->obj;
}

bool lv_menu_item_remove_obj(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_obj_t * obj)
{
    LV_ASSERT_OBJ(menu, MY_CLASS);

    uint32_t menu_item_remove_pos = 0xFFFFFFFF;

    for(uint32_t i = 0; i < menu_item->child_cnt; i++) {
        if(menu_item->items[i]->obj == obj) {
            menu_item_remove_pos = i;
        }
    }

    if(menu_item_remove_pos == 0xFFFFFFFF) {
        return false;
    }

    lv_mem_free(menu_item->items[menu_item_remove_pos]);
    lv_obj_del(menu_item->items[menu_item_remove_pos]->obj);

    menu_item->child_cnt--;

    for(uint32_t i = menu_item_remove_pos; i < menu_item->child_cnt; i++) {
        lv_memcpy_small(&menu_item->items[i], &menu_item->items[i + 1], sizeof(lv_menu_item_t *));
    }

    menu_item->items = lv_mem_realloc(menu_item->items, menu_item->child_cnt * sizeof(lv_menu_item_t *));

    return true;
}

lv_obj_t * lv_menu_item_add_menu(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                 lv_menu_item_t * new_menu_item)
{
    LV_ASSERT_OBJ(menu, MY_CLASS);

    menu_item->child_cnt++;

    menu_item->items = lv_mem_realloc(menu_item->items, menu_item->child_cnt * sizeof(lv_menu_item_t *));

    menu_item->items[menu_item->child_cnt - 1] = new_menu_item;

    lv_obj_set_parent(menu_item->items[menu_item->child_cnt - 1]->obj, ((lv_menu_t *)menu)->storage);

    return menu_item->items[menu_item->child_cnt - 1]->obj;
}

bool lv_menu_item_remove_menu(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_t * menu_item_remove)
{
    LV_ASSERT_OBJ(menu, MY_CLASS);

    uint32_t menu_item_remove_pos = 0xFFFFFFFF;

    for(uint32_t i = 0; i < menu_item->child_cnt; i++) {
        if(menu_item->items[i] == menu_item_remove) {
            menu_item_remove_pos = i;
        }
    }

    if(menu_item_remove_pos == 0xFFFFFFFF) {
        return false;
    }

    /* But do not free child menu */

    menu_item->child_cnt--;

    for(uint32_t i = menu_item_remove_pos; i < menu_item->child_cnt; i++) {
        lv_memcpy_small(&menu_item->items[i], &menu_item->items[i + 1], sizeof(lv_menu_item_t *));
    }

    menu_item->items = lv_mem_realloc(menu_item->items, menu_item->child_cnt * sizeof(lv_menu_item_t *));
    return true;
}

void lv_menu_refresh(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    lv_ll_t * history_ll = &(menu->history_ll);

    /* The current menu */
    lv_menu_history_t * act_hist = _lv_ll_get_head(history_ll);

    if(act_hist != NULL) {
        lv_menu_item_t * menu_item = act_hist->menu_item;
        /* Delete the current item from the history */
        _lv_ll_remove(history_ll, act_hist);
        lv_mem_free(act_hist);
        /* Set it back */
        lv_menu_set(obj, menu_item);
    }
}

/* Icon + Label */
lv_obj_t * lv_menu_item_create_text(lv_obj_t * parent, const char * icon, const char * txt,
                                        lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t * obj = lv_menu_cont_create(parent);
    lv_obj_set_flex_align(obj, LV_FLEX_FLOW_ROW, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * img = NULL;
    lv_obj_t * label = NULL;

    if(icon) {
        img = lv_img_create(obj);
        lv_img_set_src(img, icon);
    }

    if(txt) {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    if(builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

lv_obj_t * lv_menu_cont_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_menu_cont_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/* Extra convenience menu item functions */
lv_obj_t * lv_menu_item_set_text(lv_obj_t * menu, lv_menu_item_t * menu_item, const char * icon, const char * txt)
{
    if(menu_item->obj != NULL) {
        lv_obj_del(menu_item->obj);
    }

    lv_obj_t * obj = lv_menu_item_create_text(((lv_menu_t *)menu)->storage, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_menu_item_set_obj(menu, menu_item, obj);

    return obj;
}

lv_obj_t * lv_menu_item_add_text(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                 const char * icon, const char * txt)
{
    lv_obj_t * obj = lv_menu_item_create_text(((lv_menu_t *)menu)->storage, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    lv_menu_item_add_obj(menu, menu_item, section, obj);

    return obj;
}

lv_obj_t * lv_menu_item_add_btn(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                const char * icon, const char * txt)
{
    lv_obj_t * obj = lv_menu_item_create_text(((lv_menu_t *)menu)->storage, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_menu_item_add_obj(menu, menu_item, section, obj);

    return obj;
}

lv_obj_t * lv_menu_item_add_seperator(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section)
{
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_menu_seperator_class, ((lv_menu_t *)menu)->storage);
    lv_obj_class_init_obj(obj);

    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_menu_item_add_obj(menu, menu_item, section, obj);

    return obj;
}

#if LV_USE_SLIDER
lv_obj_t * lv_menu_item_add_slider(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                   const char * icon, const char * txt, int32_t min, int32_t max, int32_t val)
{
    lv_obj_t * obj = lv_menu_item_create_text(((lv_menu_t *)menu)->storage, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t * slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);

    if(icon == NULL) {
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    lv_menu_item_add_obj(menu, menu_item, section, obj);

    return obj;
}
#endif

#if LV_USE_SWITCH
lv_obj_t * lv_menu_item_add_switch(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                   const char * icon, const char * txt, bool chk)
{
    lv_obj_t * obj = lv_menu_item_create_text(((lv_menu_t *)menu)->storage, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t * sw = lv_switch_create(obj);
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : 0);

    lv_menu_item_add_obj(menu, menu_item, section, obj);

    return obj;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_menu_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_set_layout(obj, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);

    lv_menu_t * menu = (lv_menu_t *)obj;

    menu->mode_header = LV_MENU_MODE_HEADER_FIXED;
    menu->mode_root_back_btn = LV_MENU_MODE_ROOT_BACK_BTN_ENABLED;
    menu->mode_sidebar = LV_MENU_MODE_SIDEBAR_DISABLED;

    _lv_ll_init(&(menu->history_ll), sizeof(lv_menu_history_t));

    menu->storage = lv_obj_create(obj);
    lv_obj_add_flag(menu->storage, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t * sidebar_cont = lv_obj_class_create_obj(&lv_menu_sidebar_cont_class, obj);
    lv_obj_class_init_obj(sidebar_cont);
    lv_obj_set_size(sidebar_cont, LV_PCT(30), LV_PCT(100));
    lv_obj_set_flex_flow(sidebar_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(sidebar_cont, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_clear_flag(sidebar_cont, LV_OBJ_FLAG_CLICKABLE);
    menu->sidebar = sidebar_cont;

    lv_obj_t * sidebar_header = lv_obj_class_create_obj(&lv_menu_sidebar_header_cont_class, sidebar_cont);
    lv_obj_class_init_obj(sidebar_header);
    lv_obj_set_size(sidebar_header, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_flag(sidebar_header, LV_OBJ_FLAG_EVENT_BUBBLE);
    menu->sidebar_header = sidebar_header;

    lv_obj_t * sidebar_content = lv_obj_class_create_obj(&lv_menu_sidebar_content_cont_class, sidebar_cont);
    lv_obj_class_init_obj(sidebar_content);
    lv_obj_set_size(sidebar_content, LV_PCT(100), LV_SIZE_CONTENT);
    if(menu->mode_header == LV_MENU_MODE_HEADER_FIXED) {
        /* Content should fill the remaining space */
        lv_obj_set_flex_grow(sidebar_content, 1);
    }
    lv_obj_set_flex_flow(sidebar_content, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(sidebar_content, LV_OBJ_FLAG_EVENT_BUBBLE);
    menu->sidebar_content = sidebar_content;

    lv_obj_t * main_cont = lv_obj_class_create_obj(&lv_menu_main_cont_class, obj);
    lv_obj_class_init_obj(main_cont);
    lv_obj_set_height(main_cont, LV_PCT(100));
    lv_obj_set_flex_grow(main_cont, 1);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(main_cont, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_clear_flag(main_cont, LV_OBJ_FLAG_CLICKABLE);
    menu->main = main_cont;

    lv_obj_t * main_header = lv_obj_class_create_obj(&lv_menu_main_header_cont_class, main_cont);
    lv_obj_class_init_obj(main_header);
    lv_obj_set_size(main_header, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_flag(main_header, LV_OBJ_FLAG_EVENT_BUBBLE);
    menu->main_header = main_header;

    lv_obj_t * main_content = lv_obj_class_create_obj(&lv_menu_main_content_cont_class, main_cont);
    lv_obj_class_init_obj(main_content);
    lv_obj_set_size(main_content, LV_PCT(100), LV_SIZE_CONTENT);;
    if(menu->mode_header == LV_MENU_MODE_HEADER_FIXED) {
        /* Content should fill the remaining space */
        lv_obj_set_flex_grow(main_content, 1);
    }
    lv_obj_set_flex_flow(main_content, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(main_content, LV_OBJ_FLAG_EVENT_BUBBLE);
    menu->main_content = main_content;

    /* Create the back button(s) */
    lv_obj_t * main_back_btn = lv_menu_item_create_text(menu->main_header, LV_SYMBOL_LEFT, "Back", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_obj_add_event_cb(main_back_btn, lv_menu_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(main_back_btn, LV_OBJ_FLAG_EVENT_BUBBLE);
    ((lv_menu_cont_t *)main_back_btn)->menu = obj;
    menu->main_back_btn = main_back_btn;

    lv_obj_t * sidebar_back_btn = lv_menu_item_create_text(menu->sidebar_header, LV_SYMBOL_LEFT, "Back", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_obj_add_event_cb(sidebar_back_btn, lv_menu_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(sidebar_back_btn, LV_OBJ_FLAG_EVENT_BUBBLE);
    ((lv_menu_cont_t *)sidebar_back_btn)->mode_root_back_btn = LV_MENU_MODE_ROOT_BACK_BTN_ENABLED; /* Sidebar back btn is always root */
    ((lv_menu_cont_t *)sidebar_back_btn)->mode_sidebar = LV_MENU_MODE_SIDEBAR_ENABLED;
    ((lv_menu_cont_t *)sidebar_back_btn)->menu = obj;
    menu->sidebar_back_btn = sidebar_back_btn;

    menu->selected_tab = NULL;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_menu_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_menu_t * menu = (lv_menu_t *)obj;
    lv_ll_t * history_ll = &(menu->history_ll);

    /* The root menu */
    lv_menu_history_t * root_hist = _lv_ll_get_tail(history_ll);

    if(root_hist != NULL){
        lv_menu_item_del_internal(root_hist->menu_item, true, false);
    }

    _lv_ll_clear(history_ll);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_menu_item_del_internal(lv_menu_item_t * menu_item, bool recursive, bool del_child)
{
    if(menu_item != NULL) {
        for(uint32_t i = 0; i < menu_item->child_cnt; i++) {
            /* Free the children items */
            if(recursive) {
                lv_menu_item_del_internal(menu_item->items[i], recursive, del_child);
            }
        }

        if(menu_item->obj != NULL && del_child) {
            lv_obj_del(menu_item->obj);
        }

        /* Free the pointers to children items */
        lv_mem_free(menu_item->items);

        lv_mem_free(menu_item);
    }
}

static void lv_menu_generate(lv_obj_t * obj, lv_menu_item_t * menu_item, bool sidebar)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    lv_obj_t * section_obj = NULL;
    lv_menu_item_section_type_t prev_section = LV_MENU_SECTION_OFF;

    lv_obj_t * target_content = sidebar ? menu->sidebar_content : menu->main_content;

    for(uint32_t i = 0; i < menu_item->child_cnt; i++) {
        if(prev_section == LV_MENU_SECTION_OFF && menu_item->items[i]->section == LV_MENU_SECTION_ON) {
            section_obj = lv_obj_class_create_obj(&lv_menu_section_class, target_content);
            lv_obj_class_init_obj(section_obj);
            lv_obj_set_flex_flow(section_obj, LV_FLEX_FLOW_COLUMN);
        }

        prev_section = menu_item->items[i]->section;

        if(menu_item->items[i]->section == LV_MENU_SECTION_ON) {
            lv_obj_set_parent(menu_item->items[i]->obj, section_obj);
        }
        else {
            lv_obj_set_parent(menu_item->items[i]->obj, target_content);
            lv_obj_refr_size(menu_item->items[i]->obj);
        }

        /* Set data on whether menu item is in the sidebar */
        if(lv_obj_check_type(menu_item->items[i]->obj, &lv_menu_cont_class)) {
            lv_menu_cont_t * cont = (lv_menu_cont_t *)(menu_item->items[i]->obj);

            cont->mode_sidebar = sidebar ? LV_MENU_MODE_SIDEBAR_ENABLED : LV_MENU_MODE_SIDEBAR_DISABLED;
        }

        /* Check if next item is present */
        if(i < menu_item->child_cnt - 1) {
            if((menu_item->items[i]->section == LV_MENU_SECTION_OFF && menu_item->items[i + 1]->section == LV_MENU_SECTION_OFF) ||
               (menu_item->items[i]->section == LV_MENU_SECTION_ON && menu_item->items[i + 1]->section == LV_MENU_SECTION_ON)) {
                /* Apply bottom border */
                lv_obj_set_style_border_side(menu_item->items[i]->obj, LV_BORDER_SIDE_BOTTOM, 0);
            }
            else {
                lv_obj_set_style_border_side(menu_item->items[i]->obj, LV_BORDER_SIDE_NONE, 0);
            }
        }
        else {
            lv_obj_set_style_border_side(menu_item->items[i]->obj, LV_BORDER_SIDE_NONE, 0);
        }
    }
}

static void lv_menu_clear_contents(lv_obj_t * obj, lv_obj_t * target_content)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    uint32_t menu_content_child_cnt = lv_obj_get_child_cnt(target_content);
    for(uint32_t  i = 0; i < menu_content_child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(target_content, -1);

        if(lv_obj_check_type(child, &lv_menu_section_class)) {
            uint32_t  section_child_cnt = lv_obj_get_child_cnt(child);
            for(uint32_t  i = 0; i < section_child_cnt; i++) {
                lv_obj_set_parent(lv_obj_get_child(child, -1), menu->storage);
            }
            lv_obj_del(child);
        }
        else {
            lv_obj_set_parent(child, menu->storage);
        }
    }
}

static void lv_menu_traverse_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    /* LV_EVENT_CLICKED */
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * obj = lv_event_get_target(e);
        lv_menu_cont_t * cont = (lv_menu_cont_t *)obj;
        lv_menu_t * menu = (lv_menu_t *)(cont->menu);

        lv_ll_t * history_ll = &(menu->history_ll);

        /* The root menu */
        lv_menu_history_t * root_hist = _lv_ll_get_tail(history_ll);

        /* The current menu */
        lv_menu_history_t * act_hist = _lv_ll_get_head(history_ll);

        /* The previous menu */
        lv_menu_history_t * prev_hist = _lv_ll_get_next(history_ll, act_hist);

        if(cont->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED) {
            if(prev_hist != NULL) {
                /* Clear any checked state of previous obj */
                if(menu->selected_tab != obj && menu->selected_tab != NULL) {
                    lv_obj_clear_state(menu->selected_tab, LV_STATE_CHECKED);
                }

                /* Delete all previous histories except root*/
                void * i;
                void * i_next;

                i      = _lv_ll_get_head(history_ll);
                i_next = NULL;

                while(i != NULL && i != root_hist) {
                    i_next = _lv_ll_get_next(history_ll, i);

                    _lv_ll_remove(history_ll, i);
                    lv_mem_free(i);

                    i = i_next;
                }
            }

            menu->selected_tab = obj;
        }

        lv_menu_set(cont->menu, cont->menu_item);
    }
}

static void lv_menu_back_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    /* LV_EVENT_CLICKED */
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * obj = lv_event_get_target(e);
        lv_menu_cont_t * cont = (lv_menu_cont_t *)obj;
        lv_menu_t * menu = (lv_menu_t *)(cont->menu);
        lv_ll_t * history_ll = &(menu->history_ll);

        /* The current menu */
        lv_menu_history_t * act_hist = _lv_ll_get_head(history_ll);

        /* The previous menu */
        lv_menu_history_t * prev_hist = _lv_ll_get_next(history_ll, act_hist);

        /* The previous previous menu */
        lv_menu_history_t * prev_prev_hist = NULL;
        if(prev_hist != NULL) {
            prev_prev_hist = _lv_ll_get_next(history_ll, prev_hist);
        }

        if(menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED){
           /* Main back btn cannot be root when sidebar is enabled */
            ((lv_menu_cont_t *)menu->main_back_btn)->mode_root_back_btn = LV_MENU_MODE_ROOT_BACK_BTN_DISABLED;
        }else{
            if(prev_hist == NULL){
                ((lv_menu_cont_t *)menu->main_back_btn)->mode_root_back_btn = LV_MENU_MODE_ROOT_BACK_BTN_ENABLED;
            }else{
                ((lv_menu_cont_t *)menu->main_back_btn)->mode_root_back_btn = LV_MENU_MODE_ROOT_BACK_BTN_DISABLED;
            }
        }

        if((menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED && prev_prev_hist != NULL && cont->mode_root_back_btn == LV_MENU_MODE_ROOT_BACK_BTN_DISABLED) ||
           (menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_DISABLED && prev_hist != NULL && cont->mode_root_back_btn == LV_MENU_MODE_ROOT_BACK_BTN_DISABLED)) {
            /* Previous menu exists */
            /* Delete the current item from the history */
            _lv_ll_remove(history_ll, act_hist);
            lv_mem_free(act_hist);
            /* Create the previous menu.
            *  Remove it from the history because `lv_menu_set` will add it again */
            _lv_ll_remove(history_ll, prev_hist);
            lv_menu_set(&(menu->obj), prev_hist->menu_item);

            lv_mem_free(prev_hist);
        }
    }
}
#endif /*LV_USE_MENU*/
