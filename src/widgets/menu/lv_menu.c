/**
 * @file lv_menu.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_menu.h"

#if LV_USE_MENU

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_menu_class

#include "../../core/lv_obj.h"
#include "../../layouts/lv_layout.h"
#include "../../stdlib/lv_string.h"
#include "../label/lv_label.h"
#include "../button/lv_button.h"
#include "../image/lv_image.h"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_menu_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_menu_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_menu_page_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_menu_page_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_menu_cont_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_menu_section_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

const lv_obj_class_t lv_menu_class = {
    .constructor_cb = lv_menu_constructor,
    .destructor_cb = lv_menu_destructor,
    .base_class = &lv_obj_class,
    .width_def = (LV_DPI_DEF * 3) / 2,
    .height_def = LV_DPI_DEF * 2,
    .instance_size = sizeof(lv_menu_t),
    .name = "menu",
};
const lv_obj_class_t lv_menu_page_class = {
    .constructor_cb = lv_menu_page_constructor,
    .destructor_cb = lv_menu_page_destructor,
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_menu_page_t),
    .name = "menu-page",
};

const lv_obj_class_t lv_menu_cont_class = {
    .constructor_cb = lv_menu_cont_constructor,
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .name = "menu-cont",
};

const lv_obj_class_t lv_menu_section_class = {
    .constructor_cb = lv_menu_section_constructor,
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .name = "menu-section",
};

const lv_obj_class_t lv_menu_separator_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .name = "menu-separator",
};

const lv_obj_class_t lv_menu_sidebar_cont_class = {
    .base_class = &lv_obj_class,
};

const lv_obj_class_t lv_menu_main_cont_class = {
    .base_class = &lv_obj_class,
};

const lv_obj_class_t lv_menu_main_header_cont_class = {
    .base_class = &lv_obj_class,
};

const lv_obj_class_t lv_menu_sidebar_header_cont_class = {
    .base_class = &lv_obj_class,
};

static void lv_menu_refr(lv_obj_t * obj);
static void lv_menu_refr_sidebar_header_mode(lv_obj_t * obj);
static void lv_menu_refr_main_header_mode(lv_obj_t * obj);
static void lv_menu_load_page_event_cb(lv_event_t * e);
static void lv_menu_obj_delete_event_cb(lv_event_t * e);
static void lv_menu_back_event_cb(lv_event_t * e);
static void lv_menu_value_changed_event_cb(lv_event_t * e);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
bool lv_menu_item_back_button_is_root(lv_obj_t * menu, lv_obj_t * obj);
void lv_menu_clear_history(lv_obj_t * obj);

lv_obj_t * lv_menu_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

lv_obj_t * lv_menu_page_create(lv_obj_t * parent, char const * const title)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_menu_page_class, parent);
    lv_obj_class_init_obj(obj);

    lv_menu_page_t * page = (lv_menu_page_t *)obj;
    /* Initialise the object */
    page->title        = NULL;
    page->static_title = false;
    lv_menu_set_page_title(obj, title);

    return obj;
}

lv_obj_t * lv_menu_cont_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_menu_cont_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

lv_obj_t * lv_menu_section_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_menu_section_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

lv_obj_t * lv_menu_separator_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_menu_separator_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_menu_refr(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    lv_ll_t * history_ll = &(menu->history_ll);

    /* The current menu */
    lv_menu_history_t * act_hist = _lv_ll_get_head(history_ll);

    lv_obj_t * page = NULL;

    if(act_hist != NULL) {
        page = act_hist->page;
        /* Delete the current item from the history */
        _lv_ll_remove(history_ll, act_hist);
        lv_free(act_hist);
        menu->cur_depth--;
    }

    /* Set it */
    lv_menu_set_page(obj, page);
}

/*=====================
 * Setter functions
 *====================*/

void lv_menu_set_page(lv_obj_t * obj, lv_obj_t * page)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    /* Hide previous page */
    if(menu->main_page != NULL) {
        lv_obj_set_parent(menu->main_page, menu->storage);
    }

    if(page != NULL) {
        /* Add a new node */
        lv_ll_t * history_ll = &(menu->history_ll);
        lv_menu_history_t * new_node = _lv_ll_ins_head(history_ll);
        LV_ASSERT_MALLOC(new_node);
        new_node->page = page;
        menu->cur_depth++;

        /* Place page in main */
        lv_obj_set_parent(page, menu->main);
    }
    else {
        /* Empty page, clear history */
        lv_menu_clear_history(obj);
    }

    menu->main_page = page;

    /* If there is a selected tab, update checked state */
    if(menu->selected_tab != NULL) {
        if(menu->sidebar_page != NULL) {
            lv_obj_add_state(menu->selected_tab, LV_STATE_CHECKED);
        }
        else {
            lv_obj_remove_state(menu->selected_tab, LV_STATE_CHECKED);
        }
    }

    /* Back btn management */
    if(menu->sidebar_page != NULL) {
        /* With sidebar enabled */
        if(menu->sidebar_generated) {
            if(menu->mode_root_back_btn == LV_MENU_ROOT_BACK_BUTTON_ENABLED) {
                /* Root back btn is always shown if enabled*/
                lv_obj_remove_flag(menu->sidebar_header_back_btn, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(menu->sidebar_header_back_btn, LV_OBJ_FLAG_CLICKABLE);
            }
            else {
                lv_obj_add_flag(menu->sidebar_header_back_btn, LV_OBJ_FLAG_HIDDEN);
                lv_obj_remove_flag(menu->sidebar_header_back_btn, LV_OBJ_FLAG_CLICKABLE);
            }
        }

        if(menu->cur_depth >= 2) {
            lv_obj_remove_flag(menu->main_header_back_btn, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(menu->main_header_back_btn, LV_OBJ_FLAG_CLICKABLE);
        }
        else {
            lv_obj_add_flag(menu->main_header_back_btn, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(menu->main_header_back_btn, LV_OBJ_FLAG_CLICKABLE);
        }
    }
    else {
        /* With sidebar disabled */
        if(menu->cur_depth >= 2 || menu->mode_root_back_btn == LV_MENU_ROOT_BACK_BUTTON_ENABLED) {
            lv_obj_remove_flag(menu->main_header_back_btn, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(menu->main_header_back_btn, LV_OBJ_FLAG_CLICKABLE);
        }
        else {
            lv_obj_add_flag(menu->main_header_back_btn, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(menu->main_header_back_btn, LV_OBJ_FLAG_CLICKABLE);
        }
    }

    lv_obj_send_event((lv_obj_t *)menu, LV_EVENT_VALUE_CHANGED, NULL);

    lv_menu_refr_main_header_mode(obj);
}

void lv_menu_set_sidebar_page(lv_obj_t * obj, lv_obj_t * page)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    /* Sidebar management*/
    if(page != NULL) {
        /* Sidebar should be enabled */
        if(!menu->sidebar_generated) {
            /* Create sidebar */
            lv_obj_t * sidebar_cont = lv_obj_class_create_obj(&lv_menu_sidebar_cont_class, obj);
            lv_obj_class_init_obj(sidebar_cont);
            lv_obj_move_to_index(sidebar_cont, 1);
            lv_obj_set_size(sidebar_cont, LV_PCT(30), LV_PCT(100));
            lv_obj_set_flex_flow(sidebar_cont, LV_FLEX_FLOW_COLUMN);
            lv_obj_add_flag(sidebar_cont, LV_OBJ_FLAG_EVENT_BUBBLE);
            lv_obj_remove_flag(sidebar_cont, LV_OBJ_FLAG_CLICKABLE);
            menu->sidebar = sidebar_cont;

            lv_obj_t * sidebar_header = lv_obj_class_create_obj(&lv_menu_sidebar_header_cont_class, sidebar_cont);
            lv_obj_class_init_obj(sidebar_header);
            lv_obj_set_size(sidebar_header, LV_PCT(100), LV_SIZE_CONTENT);
            lv_obj_set_flex_flow(sidebar_header, LV_FLEX_FLOW_ROW);
            lv_obj_set_flex_align(sidebar_header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
            lv_obj_remove_flag(sidebar_header, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_flag(sidebar_header, LV_OBJ_FLAG_EVENT_BUBBLE);
            menu->sidebar_header = sidebar_header;

            lv_obj_t * sidebar_header_back_btn = lv_button_create(menu->sidebar_header);
            lv_obj_add_event_cb(sidebar_header_back_btn, lv_menu_back_event_cb, LV_EVENT_CLICKED, menu);
            lv_obj_add_flag(sidebar_header_back_btn, LV_OBJ_FLAG_EVENT_BUBBLE);
            lv_obj_set_flex_flow(sidebar_header_back_btn, LV_FLEX_FLOW_ROW);
            menu->sidebar_header_back_btn = sidebar_header_back_btn;

            lv_obj_t * sidebar_header_back_icon = lv_image_create(menu->sidebar_header_back_btn);
            lv_image_set_src(sidebar_header_back_icon, LV_SYMBOL_LEFT);

            lv_obj_t * sidebar_header_title = lv_label_create(menu->sidebar_header);
            lv_obj_add_flag(sidebar_header_title, LV_OBJ_FLAG_HIDDEN);
            menu->sidebar_header_title = sidebar_header_title;

            menu->sidebar_generated = true;
        }

        lv_obj_set_parent(page, menu->sidebar);

        lv_menu_refr_sidebar_header_mode(obj);
    }
    else {
        /* Sidebar should be disabled */
        if(menu->sidebar_generated) {
            lv_obj_set_parent(menu->sidebar_page, menu->storage);
            lv_obj_delete(menu->sidebar);

            menu->sidebar_generated = false;
        }
    }

    menu->sidebar_page = page;
    lv_menu_refr(obj);
}

void lv_menu_set_mode_header(lv_obj_t * obj, lv_menu_mode_header_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->mode_header != mode) {
        menu->mode_header = mode;
        lv_menu_refr_main_header_mode(obj);
        if(menu->sidebar_generated) lv_menu_refr_sidebar_header_mode(obj);
    }
}

void lv_menu_set_mode_root_back_button(lv_obj_t * obj, lv_menu_mode_root_back_button_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->mode_root_back_btn != mode) {
        menu->mode_root_back_btn = mode;
        lv_menu_refr(obj);
    }
}

void lv_menu_set_load_page_event(lv_obj_t * menu, lv_obj_t * obj, lv_obj_t * page)
{
    LV_ASSERT_OBJ(menu, MY_CLASS);

    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    /* Remove old event */
    uint32_t i;
    uint32_t event_cnt = lv_obj_get_event_count(obj);
    for(i = 0; i < event_cnt; i++) {
        lv_event_dsc_t * event_dsc = lv_obj_get_event_dsc(obj, i);
        if(lv_event_dsc_get_cb(event_dsc) == lv_menu_load_page_event_cb) {
            lv_obj_send_event(obj, LV_EVENT_DELETE, NULL);
            lv_obj_remove_event(obj, i);
            break;
        }
    }

    lv_menu_load_page_event_data_t * event_data = lv_malloc(sizeof(lv_menu_load_page_event_data_t));
    event_data->menu = menu;
    event_data->page = page;

    lv_obj_add_event_cb(obj, lv_menu_load_page_event_cb, LV_EVENT_CLICKED, event_data);
    lv_obj_add_event_cb(obj, lv_menu_obj_delete_event_cb, LV_EVENT_DELETE, event_data);
}

void lv_menu_set_page_title(lv_obj_t * page_obj, char const * const title)
{
    LV_LOG_INFO("begin");
    lv_menu_page_t * page = (lv_menu_page_t *)page_obj;

    /* Cleanup any previous set titles */
    if((!page->static_title) && page->title) {
        lv_free(page->title);
        page->title = NULL;
    }

    if(title) {
        page->static_title = false;
        page->title = lv_strdup(title);
        LV_ASSERT_MALLOC(page->title);
        if(page->title == NULL) {
            return;
        }
    }
    else {
        page->title        = NULL;
        page->static_title = false;
    }
}

void lv_menu_set_page_title_static(lv_obj_t * page_obj, char const * const title)
{
    LV_LOG_INFO("begin");
    lv_menu_page_t * page = (lv_menu_page_t *)page_obj;

    /* Cleanup any previous set titles */
    if((!page->static_title) && page->title) {
        lv_free(page->title);
        page->title = NULL;
    }

    /* Set or clear the static title text */
    if(title) {
        page->title        = (char *) title;
        page->static_title = true;
    }
    else {
        page->title        = NULL;
        page->static_title = false;
    }
}

/*=====================
 * Getter functions
 *====================*/
lv_obj_t * lv_menu_get_cur_main_page(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    return menu->main_page;
}

lv_obj_t * lv_menu_get_cur_sidebar_page(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    return menu->sidebar_page;
}

lv_obj_t * lv_menu_get_main_header(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    return menu->main_header;
}

lv_obj_t * lv_menu_get_main_header_back_button(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    return menu->main_header_back_btn;
}

lv_obj_t * lv_menu_get_sidebar_header(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    return menu->sidebar_header;
}

lv_obj_t * lv_menu_get_sidebar_header_back_button(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    return menu->sidebar_header_back_btn;
}

bool lv_menu_back_button_is_root(lv_obj_t * menu, lv_obj_t * obj)
{
    LV_ASSERT_OBJ(menu, MY_CLASS);

    if(obj == ((lv_menu_t *)menu)->sidebar_header_back_btn) {
        return true;
    }

    if(obj == ((lv_menu_t *)menu)->main_header_back_btn && ((lv_menu_t *)menu)->prev_depth <= 1) {
        return true;
    }

    return false;
}

void lv_menu_clear_history(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    lv_ll_t * history_ll = &(menu->history_ll);

    _lv_ll_clear(history_ll);

    menu->cur_depth = 0;
}

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

    menu->mode_header = LV_MENU_HEADER_TOP_FIXED;
    menu->mode_root_back_btn = LV_MENU_ROOT_BACK_BUTTON_DISABLED;
    menu->cur_depth = 0;
    menu->prev_depth = 0;
    menu->sidebar_generated = false;

    _lv_ll_init(&(menu->history_ll), sizeof(lv_menu_history_t));

    menu->storage = lv_obj_create(obj);
    lv_obj_add_flag(menu->storage, LV_OBJ_FLAG_HIDDEN);

    menu->sidebar = NULL;
    menu->sidebar_header = NULL;
    menu->sidebar_header_back_btn = NULL;
    menu->sidebar_header_title = NULL;
    menu->sidebar_page = NULL;

    lv_obj_t * main_cont = lv_obj_class_create_obj(&lv_menu_main_cont_class, obj);
    lv_obj_class_init_obj(main_cont);
    lv_obj_set_height(main_cont, LV_PCT(100));
    lv_obj_set_flex_grow(main_cont, 1);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(main_cont, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_remove_flag(main_cont, LV_OBJ_FLAG_CLICKABLE);
    menu->main = main_cont;

    lv_obj_t * main_header = lv_obj_class_create_obj(&lv_menu_main_header_cont_class, main_cont);
    lv_obj_class_init_obj(main_header);
    lv_obj_set_size(main_header, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(main_header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(main_header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(main_header, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(main_header, LV_OBJ_FLAG_EVENT_BUBBLE);
    menu->main_header = main_header;

    /* Create the default simple back btn and title */
    lv_obj_t * main_header_back_btn = lv_button_create(menu->main_header);
    lv_obj_add_event_cb(main_header_back_btn, lv_menu_back_event_cb, LV_EVENT_CLICKED, menu);
    lv_obj_add_flag(main_header_back_btn, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_flex_flow(main_header_back_btn, LV_FLEX_FLOW_ROW);
    menu->main_header_back_btn = main_header_back_btn;

    lv_obj_t * main_header_back_icon = lv_image_create(menu->main_header_back_btn);
    lv_image_set_src(main_header_back_icon, LV_SYMBOL_LEFT);

    lv_obj_t * main_header_title = lv_label_create(menu->main_header);
    lv_obj_add_flag(main_header_title, LV_OBJ_FLAG_HIDDEN);
    menu->main_header_title = main_header_title;

    menu->main_page = NULL;
    menu->selected_tab = NULL;

    lv_obj_add_event_cb(obj, lv_menu_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, menu);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_menu_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_menu_t * menu = (lv_menu_t *)obj;
    lv_ll_t * history_ll = &(menu->history_ll);

    _lv_ll_clear(history_ll);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_menu_page_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_menu_t * menu = (lv_menu_t *)lv_obj_get_parent(obj);

    lv_obj_set_parent(obj, ((lv_menu_t *)menu)->storage);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
}

static void lv_menu_page_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_menu_page_t * page = (lv_menu_page_t *)obj;

    if((!page->static_title) && page->title != NULL) {
        lv_free(page->title);
    }
    page->title        = NULL;
    page->static_title = false;
}

static void lv_menu_cont_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
}

static void lv_menu_section_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
}

static void lv_menu_refr_sidebar_header_mode(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->sidebar_header == NULL || menu->sidebar_page == NULL) return;

    switch(menu->mode_header) {
        case LV_MENU_HEADER_TOP_FIXED:
            /* Content should fill the remaining space */
            lv_obj_move_to_index(menu->sidebar_header, 0);
            lv_obj_set_flex_grow(menu->sidebar_page, 1);
            break;
        case LV_MENU_HEADER_TOP_UNFIXED:
            lv_obj_move_to_index(menu->sidebar_header, 0);
            lv_obj_set_flex_grow(menu->sidebar_page, 0);
            break;
        case LV_MENU_HEADER_BOTTOM_FIXED:
            lv_obj_move_to_index(menu->sidebar_header, 1);
            lv_obj_set_flex_grow(menu->sidebar_page, 1);
            break;
    }

    lv_obj_refr_size(menu->sidebar_header);
    lv_obj_refr_size(menu->sidebar_page);

    if(lv_obj_get_content_height(menu->sidebar_header) == 0) {
        lv_obj_add_flag(menu->sidebar_header, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_obj_remove_flag(menu->sidebar_header, LV_OBJ_FLAG_HIDDEN);
    }
}

static void lv_menu_refr_main_header_mode(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->main_header == NULL || menu->main_page == NULL) return;

    switch(menu->mode_header) {
        case LV_MENU_HEADER_TOP_FIXED:
            /* Content should fill the remaining space */
            lv_obj_move_to_index(menu->main_header, 0);
            lv_obj_set_flex_grow(menu->main_page, 1);
            break;
        case LV_MENU_HEADER_TOP_UNFIXED:
            lv_obj_move_to_index(menu->main_header, 0);
            lv_obj_set_flex_grow(menu->main_page, 0);
            break;
        case LV_MENU_HEADER_BOTTOM_FIXED:
            lv_obj_move_to_index(menu->main_header, 1);
            lv_obj_set_flex_grow(menu->main_page, 1);
            break;
    }

    lv_obj_refr_size(menu->main_header);
    lv_obj_refr_size(menu->main_page);
    lv_obj_update_layout(menu->main_header);

    if(lv_obj_get_content_height(menu->main_header) == 0) {
        lv_obj_add_flag(menu->main_header, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_obj_remove_flag(menu->main_header, LV_OBJ_FLAG_HIDDEN);
    }
}

static void lv_menu_load_page_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_menu_load_page_event_data_t * event_data = lv_event_get_user_data(e);
    lv_menu_t * menu = (lv_menu_t *)(event_data->menu);
    lv_obj_t * page = event_data->page;

    if(menu->sidebar_page != NULL) {
        /* Check if clicked obj is in the sidebar */
        bool sidebar = false;
        lv_obj_t * parent = obj;

        while(parent) {
            if(parent == (lv_obj_t *)menu) break;
            if(parent == menu->sidebar) {
                sidebar = true;
                break;
            }
            parent = lv_obj_get_parent(parent);
        }

        if(sidebar) {
            /* Clear checked state of previous obj */
            if(menu->selected_tab != obj && menu->selected_tab != NULL) {
                lv_obj_remove_state(menu->selected_tab, LV_STATE_CHECKED);
            }

            lv_menu_clear_history((lv_obj_t *)menu);

            menu->selected_tab = obj;
        }
    }

    lv_menu_set_page((lv_obj_t *)menu, page);

    if(lv_group_get_default() != NULL && menu->sidebar_page == NULL) {
        /* Sidebar is not supported for now*/
        lv_group_focus_next(lv_group_get_default());
    }
}

static void lv_menu_obj_delete_event_cb(lv_event_t * e)
{
    lv_menu_load_page_event_data_t * event_data = lv_event_get_user_data(e);
    lv_free(event_data);
}

static void lv_menu_back_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    /* LV_EVENT_CLICKED */
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * obj = lv_event_get_current_target(e);
        lv_menu_t * menu = (lv_menu_t *)lv_event_get_user_data(e);

        if(!(obj == menu->main_header_back_btn || obj == menu->sidebar_header_back_btn)) return;

        menu->prev_depth = menu->cur_depth; /* Save the previous value for user event handler */

        if(lv_menu_back_button_is_root((lv_obj_t *)menu, obj)) return;

        lv_ll_t * history_ll = &(menu->history_ll);

        /* The current menu */
        lv_menu_history_t * act_hist = _lv_ll_get_head(history_ll);

        /* The previous menu */
        lv_menu_history_t * prev_hist = _lv_ll_get_next(history_ll, act_hist);

        if(prev_hist != NULL) {
            /* Previous menu exists */
            /* Delete the current item from the history */
            _lv_ll_remove(history_ll, act_hist);
            lv_free(act_hist);
            menu->cur_depth--;
            /* Create the previous menu.
            *  Remove it from the history because `lv_menu_set_page` will add it again */
            _lv_ll_remove(history_ll, prev_hist);
            menu->cur_depth--;
            lv_menu_set_page(&(menu->obj), prev_hist->page);

            lv_free(prev_hist);
        }
    }
}

static void lv_menu_value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_user_data(e);
    lv_menu_t * menu = (lv_menu_t *)obj;

    lv_menu_page_t * main_page = (lv_menu_page_t *)lv_menu_get_cur_main_page(obj);
    if(main_page != NULL && menu->main_header_title != NULL) {
        if(main_page->title != NULL) {
            lv_label_set_text(menu->main_header_title, main_page->title);
            lv_obj_remove_flag(menu->main_header_title, LV_OBJ_FLAG_HIDDEN);
        }
        else {
            lv_obj_add_flag(menu->main_header_title, LV_OBJ_FLAG_HIDDEN);
        }
    }

    lv_menu_page_t * sidebar_page = (lv_menu_page_t *)lv_menu_get_cur_sidebar_page(obj);
    if(sidebar_page != NULL && menu->sidebar_header_title != NULL) {
        if(sidebar_page->title != NULL) {
            lv_label_set_text(menu->sidebar_header_title, sidebar_page->title);
            lv_obj_remove_flag(menu->sidebar_header_title, LV_OBJ_FLAG_HIDDEN);
        }
        else {
            lv_obj_add_flag(menu->sidebar_header_title, LV_OBJ_FLAG_HIDDEN);
        }
    }
}
#endif /*LV_USE_MENU*/
