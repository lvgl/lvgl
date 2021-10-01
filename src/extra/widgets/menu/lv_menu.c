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

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_menu_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_menu_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_menu_page_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_menu_cont_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_menu_section_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

const lv_obj_class_t lv_menu_class = {
    .constructor_cb = lv_menu_constructor,
    .destructor_cb = lv_menu_destructor,
    .base_class = &lv_obj_class,
    .width_def = (LV_DPI_DEF * 3) / 2,
    .height_def = LV_DPI_DEF * 2,
    .instance_size = sizeof(lv_menu_t)
};
const lv_obj_class_t lv_menu_page_class = {
    .constructor_cb = lv_menu_page_constructor,
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT
};

const lv_obj_class_t lv_menu_cont_class = {
    .constructor_cb = lv_menu_cont_constructor,
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT
};

const lv_obj_class_t lv_menu_section_class = {
    .constructor_cb = lv_menu_section_constructor,
    .base_class = &lv_obj_class,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT
};

const lv_obj_class_t lv_menu_separator_class = {
    .base_class = &lv_obj_class,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT
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

static void lv_menu_refr(lv_obj_t * obj);
static void lv_menu_refr_sidebar_header_mode(lv_obj_t * obj);
static void lv_menu_refr_main_header_mode(lv_obj_t * obj);
static void lv_menu_load_page_event_cb(lv_event_t * e);
static void lv_menu_obj_del_event_cb(lv_event_t * e);
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
bool lv_menu_item_back_btn_is_root(lv_obj_t * menu, lv_obj_t * obj);

lv_obj_t * lv_menu_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

lv_obj_t * lv_menu_page_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_menu_page_class, parent);
    lv_obj_class_init_obj(obj);

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

lv_obj_t * lv_menu_separator_create(lv_obj_t * parent){
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

    if(act_hist != NULL) {
        lv_obj_t * page = act_hist->page;
        /* Delete the current item from the history */
        _lv_ll_remove(history_ll, act_hist);
        lv_mem_free(act_hist);
        menu->cur_depth--;
        /* Set it back */
        lv_menu_set_page(obj, page);
    }
}

/*=====================
 * Setter functions
 *====================*/

void lv_menu_set_page(lv_obj_t * obj, lv_obj_t * page)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->main_page != NULL){
        lv_obj_set_parent(menu->main_page, menu->storage);
    }

    lv_ll_t * history_ll = &(menu->history_ll);

    /* Add a new node */
    lv_menu_history_t * new_node = _lv_ll_ins_head(history_ll);
    new_node->page = page;
    menu->cur_depth++;

    /* The root menu */
    lv_menu_history_t * root_hist = _lv_ll_get_tail(history_ll);

    /* Sidebar management*/
    if(menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED) {
        /* Sidebar should be enabled */
        if(!menu->sidebar_generated) {
            /* Create sidebar */
            lv_obj_t * sidebar_cont = lv_obj_class_create_obj(&lv_menu_sidebar_cont_class, obj);
            lv_obj_class_init_obj(sidebar_cont);
            lv_obj_move_to_index(sidebar_cont, 1);
            lv_obj_set_size(sidebar_cont, LV_PCT(30), LV_PCT(100));
            lv_obj_set_flex_flow(sidebar_cont, LV_FLEX_FLOW_COLUMN);
            lv_obj_add_flag(sidebar_cont, LV_OBJ_FLAG_EVENT_BUBBLE);
            lv_obj_clear_flag(sidebar_cont, LV_OBJ_FLAG_CLICKABLE);
            menu->sidebar = sidebar_cont;

            lv_obj_t * sidebar_header = lv_obj_class_create_obj(&lv_menu_sidebar_header_cont_class, sidebar_cont);
            lv_obj_class_init_obj(sidebar_header);
            lv_obj_set_flex_flow(sidebar_header, LV_FLEX_FLOW_ROW);
            lv_obj_set_size(sidebar_header, LV_PCT(100), LV_SIZE_CONTENT);
            lv_obj_add_flag(sidebar_header, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_flag(sidebar_header, LV_OBJ_FLAG_EVENT_BUBBLE);
            lv_obj_add_event_cb(sidebar_header, lv_menu_back_event_cb, LV_EVENT_CLICKED, menu);
            menu->sidebar_header = sidebar_header;

            lv_obj_t * sidebar_header_back_btn = lv_img_create(menu->sidebar_header);
            lv_img_set_src(sidebar_header_back_btn, LV_SYMBOL_LEFT);
            lv_obj_add_flag(sidebar_header_back_btn, LV_OBJ_FLAG_EVENT_BUBBLE);
            menu->sidebar_header_back_btn = menu->sidebar_header;  /* Let the entire header be the back btn */

            /* Place page in sidebar */
            if(root_hist != NULL){
                lv_obj_set_parent(root_hist->page, menu->sidebar);
                menu->sidebar_page = root_hist->page;
            }

            lv_menu_refr_sidebar_header_mode(obj);

            menu->sidebar_generated = true;
        }
        /* Show checked state */
        if(menu->selected_tab != NULL) {
            lv_obj_add_state(menu->selected_tab, LV_STATE_CHECKED);
        }
    }
    else {
        /* Sidebar should be disabled */
        if(menu->sidebar_generated) {
            /* Clear and delete the sidebar */
            if(menu->sidebar_page != NULL){
                lv_obj_set_parent(menu->sidebar_page, menu->storage);
            }
            lv_obj_del(menu->sidebar);
            menu->sidebar_generated = false;
        }
        /* Clear previous checked state */
        if(menu->selected_tab != NULL) {
            lv_obj_clear_state(menu->selected_tab, LV_STATE_CHECKED);
        }
    }

    /* Back btn management */
    if(menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED) {
        /* With sidebar enabled */
        if(menu->mode_root_back_btn == LV_MENU_MODE_ROOT_BACK_BTN_ENABLED) {
            /* Root back btn is always shown if enabled*/
            if(menu->sidebar_generated) lv_obj_clear_flag(menu->sidebar_header_back_btn, LV_OBJ_FLAG_HIDDEN);
        }
        else {
            if(menu->sidebar_generated) lv_obj_add_flag(menu->sidebar_header_back_btn, LV_OBJ_FLAG_HIDDEN);
        }

        if(menu->cur_depth >= 3) {
            lv_obj_clear_flag(menu->main_header_back_btn, LV_OBJ_FLAG_HIDDEN);
        }
        else {
            lv_obj_add_flag(menu->main_header_back_btn, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else {
        /* With sidebar disabled */
        if(menu->cur_depth >= 2 || menu->mode_root_back_btn == LV_MENU_MODE_ROOT_BACK_BTN_ENABLED) {
            lv_obj_clear_flag(menu->main_header_back_btn, LV_OBJ_FLAG_HIDDEN);
        }
        else {
            lv_obj_add_flag(menu->main_header_back_btn, LV_OBJ_FLAG_HIDDEN);
        }
    }

    if(!(menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED && menu->cur_depth <= 1)) {
        /* Place page in main */
        lv_obj_set_parent(page, menu->main);
        menu->main_page = page;
    }

    lv_menu_refr_main_header_mode(obj);
    lv_obj_refr_size(page);
}

void lv_menu_set_mode_header(lv_obj_t * obj, lv_menu_mode_header_t mode_header)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->mode_header != mode_header) {
        menu->mode_header = mode_header;
        lv_menu_refr_main_header_mode(obj);
        if(menu->sidebar_generated) lv_menu_refr_sidebar_header_mode(obj);
    }
}

void lv_menu_set_mode_root_back_btn(lv_obj_t * obj, lv_menu_mode_root_back_btn_t mode_root_back_btn)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->mode_root_back_btn != mode_root_back_btn) {
        menu->mode_root_back_btn = mode_root_back_btn;
        lv_menu_refr(obj);
    }
}

void lv_menu_set_mode_sidebar(lv_obj_t * obj, lv_menu_mode_sidebar_t mode_sidebar)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->mode_sidebar != mode_sidebar) {
        menu->mode_sidebar = mode_sidebar;
        lv_menu_refr(obj);
    }
}

void lv_menu_set_main_header_back_btn(lv_obj_t * menu, lv_obj_t * obj){
    LV_ASSERT_OBJ(menu, MY_CLASS);

    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);

    ((lv_menu_t *)menu)->main_header_back_btn = obj;
}

void lv_menu_set_sidebar_header_back_btn(lv_obj_t * menu, lv_obj_t * obj){
    LV_ASSERT_OBJ(menu, MY_CLASS);

    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);

    ((lv_menu_t *)menu)->sidebar_header_back_btn = obj;
}

void lv_menu_set_load_page_event(lv_obj_t * menu, lv_obj_t * obj, lv_obj_t * page){
    LV_ASSERT_OBJ(menu, MY_CLASS);

    /* Make the object clickable */
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    /* Remove old event */
    if(lv_obj_remove_event_cb(obj, lv_menu_load_page_event_cb)) {
        lv_event_send(obj, LV_EVENT_DELETE, NULL);
        lv_obj_remove_event_cb(obj, lv_menu_obj_del_event_cb);
    }

    lv_menu_load_page_event_data_t * event_data = lv_mem_alloc(sizeof(lv_menu_load_page_event_data_t));
    event_data->menu = menu;
    event_data->page = page;

    lv_obj_add_event_cb(obj, lv_menu_load_page_event_cb, LV_EVENT_CLICKED, event_data);
    lv_obj_add_event_cb(obj, lv_menu_obj_del_event_cb, LV_EVENT_DELETE, event_data);
}

/*=====================
 * Getter functions
 *====================*/
lv_obj_t * lv_menu_get_cur_main_page(lv_obj_t * obj){
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->main_page != NULL){
        return menu->main_page;
    }

    return NULL;
}

lv_obj_t * lv_menu_get_cur_sidebar_page(lv_obj_t * obj){
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->sidebar_page != NULL){
        return menu->sidebar_page;
    }

    return NULL;
}

lv_obj_t * lv_menu_get_main_header(lv_obj_t * obj){
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    return menu->main_header;
}

lv_obj_t * lv_menu_get_main_header_back_btn(lv_obj_t * obj){
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    return menu->main_header_back_btn;
}

lv_obj_t * lv_menu_get_sidebar_header(lv_obj_t * obj){
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    return menu->sidebar_header;
}

lv_obj_t * lv_menu_get_sidebar_header_back_btn(lv_obj_t * obj){
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;
    return menu->sidebar_header_back_btn;
}

bool lv_menu_item_back_btn_is_root(lv_obj_t * menu, lv_obj_t * obj)
{
    LV_ASSERT_OBJ(menu, MY_CLASS);

    if(obj == ((lv_menu_t *)menu)->sidebar_header_back_btn) {
        return true;
    }

    if(obj == ((lv_menu_t *)menu)->main_header_back_btn && ((lv_menu_t *)menu)->prev_depth <= 1){
        return true;
    }

    return false;
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

    menu->mode_header = LV_MENU_MODE_HEADER_TOP_FIXED;
    menu->mode_root_back_btn = LV_MENU_MODE_ROOT_BACK_BTN_DISABLED;
    menu->mode_sidebar = LV_MENU_MODE_SIDEBAR_DISABLED;
    menu->cur_depth = 0;
    menu->prev_depth = 0;
    menu->sidebar_generated = false;

    _lv_ll_init(&(menu->history_ll), sizeof(lv_menu_history_t));

    menu->storage = lv_obj_create(obj);
    lv_obj_add_flag(menu->storage, LV_OBJ_FLAG_HIDDEN);

    menu->sidebar = NULL;
    menu->sidebar_header = NULL;
    menu->sidebar_page = NULL;

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
    lv_obj_set_flex_flow(main_header, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(main_header, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_flag(main_header, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(main_header, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(main_header, lv_menu_back_event_cb, LV_EVENT_CLICKED, menu);
    menu->main_header = main_header;

    menu->main_page = NULL;

    /* Create the default simple back button(s) */
    lv_obj_t * main_header_back_btn = lv_img_create(menu->main_header);
    lv_img_set_src(main_header_back_btn, LV_SYMBOL_LEFT);
    lv_obj_add_flag(main_header_back_btn, LV_OBJ_FLAG_EVENT_BUBBLE);
    menu->main_header_back_btn = menu->main_header; /* Let the entire header be the back btn */
    lv_obj_add_flag(menu->main_header_back_btn, LV_OBJ_FLAG_HIDDEN);

    menu->selected_tab = NULL;

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
    lv_obj_set_flex_align(obj, LV_FLEX_FLOW_ROW, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
}

static void lv_menu_cont_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(obj, LV_FLEX_FLOW_ROW, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
}

static void lv_menu_section_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
}

static void lv_menu_refr_sidebar_header_mode(lv_obj_t * obj){
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->sidebar_header == NULL || menu->sidebar_page == NULL) return;

    switch(menu->mode_header) {
        case LV_MENU_MODE_HEADER_TOP_FIXED:
            /* Content should fill the remaining space */
            lv_obj_move_to_index(menu->sidebar_header, 0);
            lv_obj_set_flex_grow(menu->sidebar_page, 1);
            break;
        case LV_MENU_MODE_HEADER_TOP_UNFIXED:
            lv_obj_move_to_index(menu->sidebar_header, 0);
            lv_obj_set_flex_grow(menu->sidebar_page, 0);
            break;
        case LV_MENU_MODE_HEADER_BOTTOM_FIXED:
            lv_obj_move_to_index(menu->sidebar_header, 1);
            lv_obj_set_flex_grow(menu->sidebar_page, 1);
            break;
    }
}

static void lv_menu_refr_main_header_mode(lv_obj_t * obj){
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_menu_t * menu = (lv_menu_t *)obj;

    if(menu->main_header == NULL || menu->main_page == NULL) return;

    switch(menu->mode_header) {
        case LV_MENU_MODE_HEADER_TOP_FIXED:
            /* Content should fill the remaining space */
            lv_obj_move_to_index(menu->main_header, 0);
            lv_obj_set_flex_grow(menu->main_page, 1);
            break;
        case LV_MENU_MODE_HEADER_TOP_UNFIXED:
            lv_obj_move_to_index(menu->main_header, 0);
            lv_obj_set_flex_grow(menu->main_page, 0);
            break;
        case LV_MENU_MODE_HEADER_BOTTOM_FIXED:
            lv_obj_move_to_index(menu->main_header, 1);
            lv_obj_set_flex_grow(menu->main_page, 1);
            break;
    }
}

static void lv_menu_load_page_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_menu_load_page_event_data_t * event_data = lv_event_get_user_data(e);
    lv_menu_t * menu = (lv_menu_t *)(event_data->menu);
    lv_obj_t * page = event_data->page;

    lv_ll_t * history_ll = &(menu->history_ll);

    /* The root menu */
    lv_menu_history_t * root_hist = _lv_ll_get_tail(history_ll);

    /* The current menu */
    lv_menu_history_t * act_hist = _lv_ll_get_head(history_ll);

    /* The previous menu */
    lv_menu_history_t * prev_hist = _lv_ll_get_next(history_ll, act_hist);

    if(menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED){
        /* Check if clicked obj is in the sidebar */
        uint32_t max_up = 4;
        uint32_t up = 0;
        bool sidebar = false;
        lv_obj_t * parent = obj;

        while(up < max_up) {
            parent = lv_obj_get_parent(parent);

            if(parent == NULL){
                break;
            }

            if(parent == menu->sidebar){
                sidebar = true;
                break;
            }

            up++;
        }

        if(sidebar) {
            if(prev_hist != NULL) {
                /* Clear any checked state of previous obj */
                if(menu->selected_tab != obj && menu->selected_tab != NULL) {
                    lv_obj_clear_state(menu->selected_tab, LV_STATE_CHECKED);
                }

                /* Delete all previous histories except root (depth = 1)*/
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

                menu->cur_depth = 1;
            }

            menu->selected_tab = obj;
        }
    }

    lv_menu_set_page((lv_obj_t *)menu, page);
    lv_event_send((lv_obj_t *)menu, LV_EVENT_VALUE_CHANGED, NULL);
}

static void lv_menu_obj_del_event_cb(lv_event_t * e){
    lv_menu_load_page_event_data_t * event_data = lv_event_get_user_data(e);
    lv_mem_free(event_data);
}

static void lv_menu_back_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    /* LV_EVENT_CLICKED */
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * obj = lv_event_get_target(e);
        lv_menu_t * menu = (lv_menu_t *)lv_event_get_user_data(e);

        if(!(obj == menu->main_header_back_btn || obj == menu->sidebar_header_back_btn)) return;

        menu->prev_depth = menu->cur_depth; /* Save the previous value for user event handler */

        if(lv_menu_item_back_btn_is_root((lv_obj_t *)menu, obj)) return;

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

        if((menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_ENABLED && prev_prev_hist != NULL) ||
           (menu->mode_sidebar == LV_MENU_MODE_SIDEBAR_DISABLED && prev_hist != NULL)) {
            /* Previous menu exists */
            /* Delete the current item from the history */
            _lv_ll_remove(history_ll, act_hist);
            lv_mem_free(act_hist);
            menu->cur_depth--;
            /* Create the previous menu.
            *  Remove it from the history because `lv_menu_set` will add it again */
            _lv_ll_remove(history_ll, prev_hist);
            menu->cur_depth--;
            lv_menu_set_page(&(menu->obj), prev_hist->page);

            lv_mem_free(prev_hist);
            lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
        }
    }
}
#endif /*LV_USE_MENU*/
