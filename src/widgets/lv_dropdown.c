/**
 * @file lv_dropdown.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"
#include "lv_dropdown.h"
#if LV_USE_DROPDOWN != 0

#include "../misc/lv_assert.h"
#include "../draw/lv_draw.h"
#include "../core/lv_group.h"
#include "../core/lv_indev.h"
#include "../core/lv_disp.h"
#include "../font/lv_symbol_def.h"
#include "../misc/lv_anim.h"
#include "../misc/lv_math.h"
#include "../misc/lv_txt_ap.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_dropdown_class
#define MY_CLASS_LIST &lv_dropdownlist_class

#define LV_DROPDOWN_PR_NONE 0xFFFF

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t * lv_dropdown_list_create(lv_obj_t * parent);
static void lv_dropdown_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_dropdown_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_dropdown_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_main(lv_event_t * e);

static void lv_dropdownlist_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_dropdownlist_destructor(const lv_obj_class_t * class_p, lv_obj_t * list_obj);
static void lv_dropdown_list_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_list(lv_event_t * e);

static void draw_box(lv_obj_t * dropdown_obj, lv_draw_ctx_t * draw_ctx, uint16_t id, lv_state_t state);
static void draw_box_label(lv_obj_t * dropdown_obj, lv_draw_ctx_t * draw_ctx, uint16_t id, lv_state_t state);
static lv_res_t btn_release_handler(lv_obj_t * obj);
static lv_res_t list_release_handler(lv_obj_t * list_obj);
static void list_press_handler(lv_obj_t * page);
static uint16_t get_id_on_point(lv_obj_t * dropdown_obj, lv_coord_t y);
static void position_to_selected(lv_obj_t * obj);
static lv_obj_t * get_label(const lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_dropdown_class = {
    .constructor_cb = lv_dropdown_constructor,
    .destructor_cb = lv_dropdown_destructor,
    .event_cb = lv_dropdown_event,
    .width_def = LV_DPI_DEF,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_dropdown_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .base_class = &lv_obj_class
};

const lv_obj_class_t lv_dropdownlist_class = {
    .constructor_cb = lv_dropdownlist_constructor,
    .destructor_cb = lv_dropdownlist_destructor,
    .event_cb = lv_dropdown_list_event,
    .instance_size = sizeof(lv_dropdown_list_t),
    .base_class = &lv_obj_class
};


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_dropdown_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_dropdown_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_dropdown_set_text(lv_obj_t * obj, const char * txt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    if(dropdown->text == txt) return;

    dropdown->text = txt;

    lv_obj_invalidate(obj);
}

void lv_dropdown_set_options(lv_obj_t * obj, const char * options)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(options);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    /*Count the '\n'-s to determine the number of options*/
    dropdown->option_cnt = 0;
    uint32_t i;
    for(i = 0; options[i] != '\0'; i++) {
        if(options[i] == '\n') dropdown->option_cnt++;
    }
    dropdown->option_cnt++;   /*Last option has no `\n`*/
    dropdown->sel_opt_id      = 0;
    dropdown->sel_opt_id_orig = 0;

    /*Allocate space for the new text*/
#if LV_USE_ARABIC_PERSIAN_CHARS == 0
    size_t len = strlen(options) + 1;
#else
    size_t len = _lv_txt_ap_calc_bytes_cnt(options) + 1;
#endif

    if(dropdown->options != NULL && dropdown->static_txt == 0) {
        lv_mem_free(dropdown->options);
        dropdown->options = NULL;
    }

    dropdown->options = lv_mem_alloc(len);

    LV_ASSERT_MALLOC(dropdown->options);
    if(dropdown->options == NULL) return;

#if LV_USE_ARABIC_PERSIAN_CHARS == 0
    strcpy(dropdown->options, options);
#else
    _lv_txt_ap_proc(options, dropdown->options);
#endif

    /*Now the text is dynamically allocated*/
    dropdown->static_txt = 0;

    lv_obj_invalidate(obj);
    if(dropdown->list) lv_obj_invalidate(dropdown->list);
}

void lv_dropdown_set_options_static(lv_obj_t * obj, const char * options)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(options);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    /*Count the '\n'-s to determine the number of options*/
    dropdown->option_cnt = 0;
    uint32_t i;
    for(i = 0; options[i] != '\0'; i++) {
        if(options[i] == '\n') dropdown->option_cnt++;
    }
    dropdown->option_cnt++;   /*Last option has no `\n`*/
    dropdown->sel_opt_id      = 0;
    dropdown->sel_opt_id_orig = 0;

    if(dropdown->static_txt == 0 && dropdown->options != NULL) {
        lv_mem_free(dropdown->options);
        dropdown->options = NULL;
    }

    dropdown->static_txt = 1;
    dropdown->options = (char *)options;

    lv_obj_invalidate(obj);
    if(dropdown->list) lv_obj_invalidate(dropdown->list);
}

void lv_dropdown_add_option(lv_obj_t * obj, const char * option, uint32_t pos)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(option);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    /*Convert static options to dynamic*/
    if(dropdown->static_txt != 0) {
        char * static_options = dropdown->options;
        size_t len = strlen(static_options) + 1;

        dropdown->options = lv_mem_alloc(len);
        LV_ASSERT_MALLOC(dropdown->options);
        if(dropdown->options == NULL) return;

        strcpy(dropdown->options, static_options);
        dropdown->static_txt = 0;
    }

    /*Allocate space for the new option*/
    size_t old_len = (dropdown->options == NULL) ? 0 : strlen(dropdown->options);
#if LV_USE_ARABIC_PERSIAN_CHARS == 0
    size_t ins_len = strlen(option) + 1;
#else
    size_t ins_len = _lv_txt_ap_calc_bytes_cnt(option) + 1;
#endif

    size_t new_len = ins_len + old_len + 2; /*+2 for terminating NULL and possible \n*/
    dropdown->options        = lv_mem_realloc(dropdown->options, new_len + 1);
    LV_ASSERT_MALLOC(dropdown->options);
    if(dropdown->options == NULL) return;

    dropdown->options[old_len] = '\0';

    /*Find the insert character position*/
    uint32_t insert_pos = old_len;
    if(pos != LV_DROPDOWN_POS_LAST) {
        uint32_t opcnt = 0;
        for(insert_pos = 0; dropdown->options[insert_pos] != 0; insert_pos++) {
            if(opcnt == pos)
                break;
            if(dropdown->options[insert_pos] == '\n')
                opcnt++;
        }
    }

    /*Add delimiter to existing options*/
    if((insert_pos > 0) && (pos >= dropdown->option_cnt))
        _lv_txt_ins(dropdown->options, _lv_txt_encoded_get_char_id(dropdown->options, insert_pos++), "\n");

    /*Insert the new option, adding \n if necessary*/
    char * ins_buf = lv_mem_buf_get(ins_len + 2); /*+ 2 for terminating NULL and possible \n*/
    LV_ASSERT_MALLOC(ins_buf);
    if(ins_buf == NULL) return;
#if LV_USE_ARABIC_PERSIAN_CHARS == 0
    strcpy(ins_buf, option);
#else
    _lv_txt_ap_proc(option, ins_buf);
#endif
    if(pos < dropdown->option_cnt) strcat(ins_buf, "\n");

    _lv_txt_ins(dropdown->options, _lv_txt_encoded_get_char_id(dropdown->options, insert_pos), ins_buf);
    lv_mem_buf_release(ins_buf);

    dropdown->option_cnt++;

    lv_obj_invalidate(obj);
    if(dropdown->list) lv_obj_invalidate(dropdown->list);
}

void lv_dropdown_clear_options(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    if(dropdown->options == NULL) return;

    if(dropdown->static_txt == 0)
        lv_mem_free(dropdown->options);

    dropdown->options = NULL;
    dropdown->static_txt = 0;
    dropdown->option_cnt = 0;

    lv_obj_invalidate(obj);
    if(dropdown->list) lv_obj_invalidate(dropdown->list);
}

void lv_dropdown_set_selected(lv_obj_t * obj, uint16_t sel_opt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    if(dropdown->sel_opt_id == sel_opt) return;

    dropdown->sel_opt_id      = sel_opt < dropdown->option_cnt ? sel_opt : dropdown->option_cnt - 1;
    dropdown->sel_opt_id_orig = dropdown->sel_opt_id;

    lv_obj_invalidate(obj);
}

void lv_dropdown_set_dir(lv_obj_t * obj, lv_dir_t dir)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    if(dropdown->dir == dir) return;

    dropdown->dir = dir;

    lv_obj_invalidate(obj);
}

void lv_dropdown_set_symbol(lv_obj_t * obj, const void * symbol)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    dropdown->symbol = symbol;
    lv_obj_invalidate(obj);
}

void lv_dropdown_set_selected_highlight(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    dropdown->selected_highlight = en;
    if(dropdown->list) lv_obj_invalidate(dropdown->list);
}

/*=====================
 * Getter functions
 *====================*/

lv_obj_t * lv_dropdown_get_list(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    return dropdown->list;
}

const char * lv_dropdown_get_text(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    return dropdown->text;
}

const char * lv_dropdown_get_options(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    return dropdown->options == NULL ? "" : dropdown->options;
}

uint16_t lv_dropdown_get_selected(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    return dropdown->sel_opt_id;
}

uint16_t lv_dropdown_get_option_cnt(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    return dropdown->option_cnt;
}

void lv_dropdown_get_selected_str(const lv_obj_t * obj, char * buf, uint32_t buf_size)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    uint32_t i;
    uint32_t line        = 0;
    size_t txt_len;

    if(dropdown->options)  {
        txt_len     = strlen(dropdown->options);
    }
    else {
        buf[0] = '\0';
        return;
    }

    for(i = 0; i < txt_len && line != dropdown->sel_opt_id_orig; i++) {
        if(dropdown->options[i] == '\n') line++;
    }

    uint32_t c;
    for(c = 0; i < txt_len && dropdown->options[i] != '\n'; c++, i++) {
        if(buf_size && c >= buf_size - 1) {
            LV_LOG_WARN("lv_dropdown_get_selected_str: the buffer was too small");
            break;
        }
        buf[c] = dropdown->options[i];
    }

    buf[c] = '\0';
}

int32_t lv_dropdown_get_option_index(lv_obj_t * obj, const char * option)
{
    const char * opts = lv_dropdown_get_options(obj);
    uint32_t char_i = 0;
    uint32_t opt_i = 0;
    const char * start = opts;

    while(start[char_i] != '\0') {
        for(char_i = 0; (start[char_i] != '\n') && (start[char_i] != '\0'); char_i++);

        if(memcmp(start, option, LV_MIN(strlen(option), char_i)) == 0) return opt_i;
        start = &start[char_i];
        if(start[0] == '\n') start++;
        opt_i++;
    }

    return -1;
}


const char * lv_dropdown_get_symbol(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    return dropdown->symbol;
}

bool lv_dropdown_get_selected_highlight(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    return dropdown->selected_highlight;
}

lv_dir_t lv_dropdown_get_dir(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    return dropdown->dir;
}

/*=====================
 * Other functions
 *====================*/

void lv_dropdown_open(lv_obj_t * dropdown_obj)
{
    LV_ASSERT_OBJ(dropdown_obj, MY_CLASS);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;

    lv_obj_add_state(dropdown_obj, LV_STATE_CHECKED);
    lv_obj_set_parent(dropdown->list, lv_obj_get_screen(dropdown_obj));
    lv_obj_move_to_index(dropdown->list, -1);
    lv_obj_clear_flag(dropdown->list, LV_OBJ_FLAG_HIDDEN);

    /*To allow styling the list*/
    lv_event_send(dropdown_obj, LV_EVENT_READY, NULL);

    lv_obj_t * label = get_label(dropdown_obj);
    lv_label_set_text_static(label, dropdown->options);
    lv_obj_set_width(dropdown->list, LV_SIZE_CONTENT);

    lv_obj_update_layout(label);
    /*Set smaller width to the width of the button*/
    if(lv_obj_get_width(dropdown->list) <= lv_obj_get_width(dropdown_obj) &&
       (dropdown->dir == LV_DIR_TOP || dropdown->dir == LV_DIR_BOTTOM)) {
        lv_obj_set_width(dropdown->list, lv_obj_get_width(dropdown_obj));
    }

    lv_coord_t label_h = lv_obj_get_height(label);
    lv_coord_t border_width = lv_obj_get_style_border_width(dropdown->list, LV_PART_MAIN);
    lv_coord_t top = lv_obj_get_style_pad_top(dropdown->list, LV_PART_MAIN) + border_width;
    lv_coord_t bottom = lv_obj_get_style_pad_bottom(dropdown->list, LV_PART_MAIN) + border_width;

    lv_coord_t list_fit_h = label_h + top + bottom;
    lv_coord_t list_h = list_fit_h;

    lv_dir_t dir = dropdown->dir;
    /*No space on the bottom? See if top is better.*/
    if(dropdown->dir == LV_DIR_BOTTOM) {
        if(dropdown_obj->coords.y2 + list_h > LV_VER_RES) {
            if(dropdown_obj->coords.y1 > LV_VER_RES - dropdown_obj->coords.y2) {
                /*There is more space on the top, so make it drop up*/
                dir = LV_DIR_TOP;
                list_h = dropdown_obj->coords.y1 - 1;
            }
            else {
                list_h = LV_VER_RES - dropdown_obj->coords.y2 - 1 ;
            }
        }
    }
    /*No space on the top? See if bottom is better.*/
    else if(dropdown->dir == LV_DIR_TOP) {
        if(dropdown_obj->coords.y1 - list_h < 0) {
            if(dropdown_obj->coords.y1 < LV_VER_RES - dropdown_obj->coords.y2) {
                /*There is more space on the top, so make it drop up*/
                dir = LV_DIR_BOTTOM;
                list_h = LV_VER_RES - dropdown_obj->coords.y2;
            }
            else {
                list_h = dropdown_obj->coords.y1;
            }
        }
    }

    if(list_h > list_fit_h) list_h = list_fit_h;
    lv_obj_set_height(dropdown->list, list_h);

    position_to_selected(dropdown_obj);

    if(dir == LV_DIR_BOTTOM)     lv_obj_align_to(dropdown->list, dropdown_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    else if(dir == LV_DIR_TOP)   lv_obj_align_to(dropdown->list, dropdown_obj, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    else if(dir == LV_DIR_LEFT)  lv_obj_align_to(dropdown->list, dropdown_obj, LV_ALIGN_OUT_LEFT_TOP, 0, 0);
    else if(dir == LV_DIR_RIGHT) lv_obj_align_to(dropdown->list, dropdown_obj, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);

    lv_obj_update_layout(dropdown->list);

    if(dropdown->dir == LV_DIR_LEFT || dropdown->dir == LV_DIR_RIGHT) {
        lv_coord_t y1 = lv_obj_get_y(dropdown->list);
        lv_coord_t y2 = lv_obj_get_y2(dropdown->list);
        if(y2 >= LV_VER_RES) {
            lv_obj_set_y(dropdown->list, y1 - (y2 - LV_VER_RES) - 1);
        }
    }

    lv_text_align_t align = lv_obj_calculate_style_text_align(label, LV_PART_MAIN, dropdown->options);

    switch(align) {
        default:
        case LV_TEXT_ALIGN_LEFT:
            lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
            break;
        case LV_TEXT_ALIGN_RIGHT:
            lv_obj_align(label, LV_ALIGN_TOP_RIGHT, 0, 0);
            break;
        case LV_TEXT_ALIGN_CENTER:
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
            break;

    }
}

void lv_dropdown_close(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_clear_state(obj, LV_STATE_CHECKED);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    dropdown->pr_opt_id = LV_DROPDOWN_PR_NONE;
    lv_obj_add_flag(dropdown->list, LV_OBJ_FLAG_HIDDEN);

    lv_event_send(obj, LV_EVENT_CANCEL, NULL);
}

bool lv_dropdown_is_open(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    return lv_obj_has_flag(dropdown->list, LV_OBJ_FLAG_HIDDEN) ? false : true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * lv_dropdown_list_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_dropdownlist_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

static void lv_dropdown_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    /*Initialize the allocated 'ext'*/
    dropdown->list          = NULL;
    dropdown->options     = NULL;
    dropdown->symbol         = LV_SYMBOL_DOWN;
    dropdown->text         = NULL;
    dropdown->static_txt = 1;
    dropdown->selected_highlight = 1;
    dropdown->sel_opt_id      = 0;
    dropdown->sel_opt_id_orig = 0;
    dropdown->pr_opt_id = LV_DROPDOWN_PR_NONE;
    dropdown->option_cnt      = 0;
    dropdown->dir = LV_DIR_BOTTOM;

    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_dropdown_set_options_static(obj, "Option 1\nOption 2\nOption 3");

    dropdown->list = lv_dropdown_list_create(lv_obj_get_screen(obj));
    lv_dropdown_list_t * list = (lv_dropdown_list_t *)dropdown->list;
    list->dropdown = obj;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_dropdown_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    if(dropdown->list) {
        lv_obj_del(dropdown->list);
        dropdown->list = NULL;
    }

    if(!dropdown->static_txt) {
        lv_mem_free(dropdown->options);
        dropdown->options = NULL;
    }
}

static void lv_dropdownlist_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);

    lv_label_create(obj);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_dropdownlist_destructor(const lv_obj_class_t * class_p, lv_obj_t * list_obj)
{
    LV_UNUSED(class_p);
    lv_dropdown_list_t * list = (lv_dropdown_list_t *)list_obj;
    lv_obj_t * dropdown_obj = list->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;
    dropdown->list = NULL;
}

static void lv_dropdown_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    if(code == LV_EVENT_FOCUSED) {
        lv_group_t * g             = lv_obj_get_group(obj);
        bool editing               = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

        /*Encoders need special handling*/
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            /*Open the list if editing*/
            if(editing) {
                lv_dropdown_open(obj);
            }
            /*Close the list if navigating*/
            else {
                dropdown->sel_opt_id = dropdown->sel_opt_id_orig;
                lv_dropdown_close(obj);
            }
        }
    }
    else if(code == LV_EVENT_DEFOCUSED || code == LV_EVENT_LEAVE) {
        lv_dropdown_close(obj);
    }
    else if(code == LV_EVENT_RELEASED) {
        res = btn_release_handler(obj);
        if(res != LV_RES_OK) return;
    }
    else if(code == LV_EVENT_STYLE_CHANGED) {
        lv_obj_refresh_self_size(obj);
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        lv_obj_refresh_self_size(obj);
    }
    else if(code == LV_EVENT_GET_SELF_SIZE) {
        lv_point_t * p = lv_event_get_param(e);
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        p->y = lv_font_get_line_height(font);
    }
    else if(code == LV_EVENT_KEY) {
        char c = *((char *)lv_event_get_param(e));
        if(c == LV_KEY_RIGHT || c == LV_KEY_DOWN) {
            if(!lv_dropdown_is_open(obj)) {
                lv_dropdown_open(obj);
            }
            else if(dropdown->sel_opt_id + 1 < dropdown->option_cnt) {
                dropdown->sel_opt_id++;
                position_to_selected(obj);
            }
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_UP) {

            if(!lv_dropdown_is_open(obj)) {
                lv_dropdown_open(obj);
            }
            else if(dropdown->sel_opt_id > 0) {
                dropdown->sel_opt_id--;
                position_to_selected(obj);
            }
        }
        else if(c == LV_KEY_ESC) {
            dropdown->sel_opt_id = dropdown->sel_opt_id_orig;
            lv_dropdown_close(obj);
        }
        else if(c == LV_KEY_ENTER) {
            /* Handle the ENTER key only if it was send by an other object.
             * Do no process it if ENTER is sent by the dropdown because it's handled in LV_EVENT_RELEASED */
            lv_obj_t * indev_obj = lv_indev_get_obj_act();
            if(indev_obj != obj) {
                res = btn_release_handler(obj);
                if(res != LV_RES_OK) return;
            }
        }
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_main(e);
    }
}

static void lv_dropdown_list_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /*Call the ancestor's event handler*/
    lv_event_code_t code = lv_event_get_code(e);
    if(code != LV_EVENT_DRAW_POST) {
        res = lv_obj_event_base(MY_CLASS_LIST, e);
        if(res != LV_RES_OK) return;
    }
    lv_obj_t * list = lv_event_get_target(e);
    lv_obj_t * dropdown_obj = ((lv_dropdown_list_t *)list)->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;

    if(code == LV_EVENT_RELEASED) {
        if(lv_indev_get_scroll_obj(lv_indev_get_act()) == NULL) {
            list_release_handler(list);
        }
    }
    else if(code == LV_EVENT_PRESSED) {
        list_press_handler(list);
    }
    else if(code == LV_EVENT_SCROLL_BEGIN) {
        dropdown->pr_opt_id = LV_DROPDOWN_PR_NONE;
        lv_obj_invalidate(list);
    }
    else if(code == LV_EVENT_DRAW_POST) {
        draw_list(e);
        res = lv_obj_event_base(MY_CLASS_LIST, e);
        if(res != LV_RES_OK) return;
    }
}


static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);

    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
    lv_coord_t right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN) + border_width;
    lv_coord_t top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;

    lv_draw_label_dsc_t symbol_dsc;
    lv_draw_label_dsc_init(&symbol_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_INDICATOR, &symbol_dsc);

    /*If no text specified use the selected option*/
    const char * opt_txt;
    if(dropdown->text) opt_txt = dropdown->text;
    else {
        char * buf = lv_mem_buf_get(128);
        lv_dropdown_get_selected_str(obj, buf, 128);
        opt_txt = buf;
    }

    bool symbol_to_left = false;
    if(dropdown->dir == LV_DIR_LEFT) symbol_to_left = true;
    if(lv_obj_get_style_base_dir(obj, LV_PART_MAIN) == LV_BASE_DIR_RTL) symbol_to_left = true;

    if(dropdown->symbol) {
        lv_img_src_t symbol_type = lv_img_src_get_type(dropdown->symbol);
        lv_coord_t symbol_w;
        lv_coord_t symbol_h;
        if(symbol_type == LV_IMG_SRC_SYMBOL) {
            lv_point_t size;
            lv_txt_get_size(&size, dropdown->symbol, symbol_dsc.font, symbol_dsc.letter_space, symbol_dsc.line_space, LV_COORD_MAX,
                            symbol_dsc.flag);
            symbol_w = size.x;
            symbol_h = size.y;
        }
        else {
            lv_img_header_t header;
            lv_res_t res = lv_img_decoder_get_info(dropdown->symbol, &header);
            if(res == LV_RES_OK) {
                symbol_w = header.w;
                symbol_h = header.h;
            }
            else {
                symbol_w = -1;
                symbol_h = -1;
            }
        }

        lv_area_t symbol_area;
        if(symbol_to_left) {
            symbol_area.x1 = obj->coords.x1 + left;
            symbol_area.x2 = symbol_area.x1 + symbol_w - 1;
        }
        else {
            symbol_area.x1 = obj->coords.x2 - right - symbol_w;
            symbol_area.x2 = symbol_area.x1 + symbol_w - 1;
        }

        if(symbol_type == LV_IMG_SRC_SYMBOL) {
            symbol_area.y1 = obj->coords.y1 + top;
            symbol_area.y2 = symbol_area.y1 + symbol_h - 1;
            lv_draw_label(draw_ctx, &symbol_dsc, &symbol_area, dropdown->symbol, NULL);
        }
        else {
            symbol_area.y1 = obj->coords.y1 + (lv_obj_get_height(obj) - symbol_h) / 2;
            symbol_area.y2 = symbol_area.y1 + symbol_h - 1;
            lv_draw_img_dsc_t img_dsc;
            lv_draw_img_dsc_init(&img_dsc);
            lv_obj_init_draw_img_dsc(obj, LV_PART_INDICATOR, &img_dsc);
            img_dsc.pivot.x = symbol_w / 2;
            img_dsc.pivot.y = symbol_h / 2;
            img_dsc.angle = lv_obj_get_style_transform_angle(obj, LV_PART_INDICATOR);
            lv_draw_img(draw_ctx, &img_dsc, &symbol_area, dropdown->symbol);
        }
    }

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &label_dsc);

    lv_point_t size;
    lv_txt_get_size(&size, opt_txt, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX,
                    label_dsc.flag);

    lv_area_t txt_area;
    txt_area.y1 = obj->coords.y1 + top;
    txt_area.y2 = txt_area.y1 + size.y;
    /*Center align the text if no symbol*/
    if(dropdown->symbol == NULL) {
        txt_area.x1 = obj->coords.x1 + (lv_obj_get_width(obj) - size.x) / 2;
        txt_area.x2 = txt_area.x1 + size.x;
    }
    else {
        /*Text to the right*/
        if(symbol_to_left) {
            txt_area.x1 = obj->coords.x2 - right - size.x;
            txt_area.x2 = txt_area.x1 + size.x;
        }
        else {
            txt_area.x1 = obj->coords.x1 + left;
            txt_area.x2 = txt_area.x1 + size.x;
        }
    }
    lv_draw_label(draw_ctx, &label_dsc, &txt_area, opt_txt, NULL);

    if(dropdown->text == NULL) {
        lv_mem_buf_release((char *)opt_txt);
    }
}

static void draw_list(lv_event_t * e)
{
    lv_obj_t * list_obj = lv_event_get_target(e);
    lv_dropdown_list_t * list = (lv_dropdown_list_t *)list_obj;
    lv_obj_t * dropdown_obj = list->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);

    /* Clip area might be too large too to shadow but
     * the selected option can be drawn on only the background*/
    lv_area_t clip_area_core;
    bool has_common;
    has_common = _lv_area_intersect(&clip_area_core, draw_ctx->clip_area, &dropdown->list->coords);
    if(has_common) {
        const lv_area_t * clip_area_ori = draw_ctx->clip_area;
        draw_ctx->clip_area = &clip_area_core;
        if(dropdown->selected_highlight) {
            if(dropdown->pr_opt_id == dropdown->sel_opt_id) {
                draw_box(dropdown_obj, draw_ctx, dropdown->pr_opt_id, LV_STATE_CHECKED | LV_STATE_PRESSED);
                draw_box_label(dropdown_obj, draw_ctx, dropdown->pr_opt_id, LV_STATE_CHECKED | LV_STATE_PRESSED);
            }
            else {
                draw_box(dropdown_obj, draw_ctx, dropdown->pr_opt_id, LV_STATE_PRESSED);
                draw_box_label(dropdown_obj, draw_ctx, dropdown->pr_opt_id, LV_STATE_PRESSED);
                draw_box(dropdown_obj, draw_ctx, dropdown->sel_opt_id, LV_STATE_CHECKED);
                draw_box_label(dropdown_obj, draw_ctx, dropdown->sel_opt_id, LV_STATE_CHECKED);
            }
        }
        else {
            draw_box(dropdown_obj, draw_ctx, dropdown->pr_opt_id, LV_STATE_PRESSED);
            draw_box_label(dropdown_obj, draw_ctx, dropdown->pr_opt_id, LV_STATE_PRESSED);
        }
        draw_ctx->clip_area = clip_area_ori;
    }
}

static void draw_box(lv_obj_t * dropdown_obj, lv_draw_ctx_t * draw_ctx, uint16_t id, lv_state_t state)
{
    if(id == LV_DROPDOWN_PR_NONE) return;

    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;
    lv_obj_t * list_obj = dropdown->list;
    lv_state_t state_ori = list_obj->state;

    if(state != list_obj->state) {
        list_obj->state = state;
        list_obj->skip_trans = 1;
    }

    /*Draw a rectangle under the selected item*/
    const lv_font_t * font    = lv_obj_get_style_text_font(list_obj, LV_PART_SELECTED);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(list_obj,  LV_PART_SELECTED);
    lv_coord_t font_h         = lv_font_get_line_height(font);

    /*Draw the selected*/
    lv_obj_t * label = get_label(dropdown_obj);
    lv_area_t rect_area;
    rect_area.y1 = label->coords.y1;
    rect_area.y1 += id * (font_h + line_space);
    rect_area.y1 -= line_space / 2;

    rect_area.y2 = rect_area.y1 + font_h + line_space - 1;
    rect_area.x1 = dropdown->list->coords.x1;
    rect_area.x2 = dropdown->list->coords.x2;

    lv_draw_rect_dsc_t sel_rect;
    lv_draw_rect_dsc_init(&sel_rect);
    lv_obj_init_draw_rect_dsc(list_obj,  LV_PART_SELECTED, &sel_rect);
    lv_draw_rect(draw_ctx, &sel_rect, &rect_area);

    list_obj->state = state_ori;
    list_obj->skip_trans = 0;
}

static void draw_box_label(lv_obj_t * dropdown_obj, lv_draw_ctx_t * draw_ctx, uint16_t id, lv_state_t state)
{
    if(id == LV_DROPDOWN_PR_NONE) return;

    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;
    lv_obj_t * list_obj = dropdown->list;
    lv_state_t state_orig = list_obj->state;

    if(state != list_obj->state) {
        list_obj->state = state;
        list_obj->skip_trans = 1;
    }

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(list_obj, LV_PART_SELECTED, &label_dsc);

    label_dsc.line_space = lv_obj_get_style_text_line_space(list_obj,
                                                            LV_PART_SELECTED);  /*Line space should come from the list*/

    lv_obj_t * label = get_label(dropdown_obj);
    if(label == NULL) return;

    lv_coord_t font_h        = lv_font_get_line_height(label_dsc.font);

    lv_area_t area_sel;
    area_sel.y1 = label->coords.y1;
    area_sel.y1 += id * (font_h + label_dsc.line_space);
    area_sel.y1 -= label_dsc.line_space / 2;

    area_sel.y2 = area_sel.y1 + font_h + label_dsc.line_space - 1;
    area_sel.x1 = list_obj->coords.x1;
    area_sel.x2 = list_obj->coords.x2;
    lv_area_t mask_sel;
    bool area_ok;
    area_ok = _lv_area_intersect(&mask_sel, draw_ctx->clip_area, &area_sel);
    if(area_ok) {
        const lv_area_t * clip_area_ori = draw_ctx->clip_area;
        draw_ctx->clip_area = &mask_sel;
        lv_draw_label(draw_ctx, &label_dsc, &label->coords, lv_label_get_text(label), NULL);
        draw_ctx->clip_area = clip_area_ori;
    }
    list_obj->state = state_orig;
    list_obj->skip_trans = 0;
}


static lv_res_t btn_release_handler(lv_obj_t * obj)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    lv_indev_t * indev = lv_indev_get_act();
    if(lv_indev_get_scroll_obj(indev) == NULL) {
        if(lv_dropdown_is_open(obj)) {
            lv_dropdown_close(obj);
            if(dropdown->sel_opt_id_orig != dropdown->sel_opt_id) {
                dropdown->sel_opt_id_orig = dropdown->sel_opt_id;
                lv_res_t res;
                uint32_t id  = dropdown->sel_opt_id; /*Just to use uint32_t in event data*/
                res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &id);
                if(res != LV_RES_OK) return res;
                lv_obj_invalidate(obj);
            }
            lv_indev_type_t indev_type = lv_indev_get_type(indev);
            if(indev_type == LV_INDEV_TYPE_ENCODER) {
                lv_group_set_editing(lv_obj_get_group(obj), false);
            }
        }
        else {
            lv_dropdown_open(obj);
        }
    }
    else {
        dropdown->sel_opt_id = dropdown->sel_opt_id_orig;
        lv_obj_invalidate(obj);
    }
    return LV_RES_OK;
}

/**
 * Called when a drop down list is released to open it or set new option
 * @param list pointer to the drop down list's list
 * @return LV_RES_INV if the list is not being deleted in the user callback. Else LV_RES_OK
 */
static lv_res_t list_release_handler(lv_obj_t * list_obj)
{
    lv_dropdown_list_t * list = (lv_dropdown_list_t *) list_obj;
    lv_obj_t * dropdown_obj = list->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;

    lv_indev_t * indev = lv_indev_get_act();
    /*Leave edit mode once a new item is selected*/
    if(lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) {
        dropdown->sel_opt_id_orig = dropdown->sel_opt_id;
        lv_group_t * g      = lv_obj_get_group(dropdown_obj);
        if(lv_group_get_editing(g)) {
            lv_group_set_editing(g, false);
        }
    }

    /*Search the clicked option (For KEYPAD and ENCODER the new value should be already set)*/
    if(lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER || lv_indev_get_type(indev) == LV_INDEV_TYPE_BUTTON) {
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        dropdown->sel_opt_id     = get_id_on_point(dropdown_obj, p.y);
        dropdown->sel_opt_id_orig = dropdown->sel_opt_id;
    }

    lv_dropdown_close(dropdown_obj);

    /*Invalidate to refresh the text*/
    if(dropdown->text == NULL) lv_obj_invalidate(dropdown_obj);

    uint32_t id  = dropdown->sel_opt_id; /*Just to use uint32_t in event data*/
    lv_res_t res = lv_event_send(dropdown_obj, LV_EVENT_VALUE_CHANGED, &id);
    if(res != LV_RES_OK) return res;

    return LV_RES_OK;
}

static void list_press_handler(lv_obj_t * list_obj)
{
    lv_dropdown_list_t * list = (lv_dropdown_list_t *) list_obj;
    lv_obj_t * dropdown_obj = list->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;

    lv_indev_t * indev = lv_indev_get_act();
    if(indev && (lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER || lv_indev_get_type(indev) == LV_INDEV_TYPE_BUTTON)) {
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        dropdown->pr_opt_id = get_id_on_point(dropdown_obj, p.y);
        lv_obj_invalidate(list_obj);
    }
}

static uint16_t get_id_on_point(lv_obj_t * dropdown_obj, lv_coord_t y)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;
    lv_obj_t * label = get_label(dropdown_obj);
    if(label == NULL) return 0;
    y -= label->coords.y1;

    const lv_font_t * font         = lv_obj_get_style_text_font(label, LV_PART_MAIN);
    lv_coord_t font_h              = lv_font_get_line_height(font);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(label, LV_PART_MAIN);

    y += line_space / 2;
    lv_coord_t h = font_h + line_space;

    uint16_t opt = y / h;

    if(opt >= dropdown->option_cnt) opt = dropdown->option_cnt - 1;
    return opt;
}

/**
 * Set the position of list when it is closed to show the selected item
 * @param ddlist pointer to a drop down list
 */
static void position_to_selected(lv_obj_t * dropdown_obj)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;

    lv_obj_t * label = get_label(dropdown_obj);
    if(label == NULL) return;

    if(lv_obj_get_height(label) <= lv_obj_get_content_height(dropdown_obj)) return;

    const lv_font_t * font         = lv_obj_get_style_text_font(label, LV_PART_MAIN);
    lv_coord_t font_h              = lv_font_get_line_height(font);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(label, LV_PART_MAIN);
    lv_coord_t unit_h = font_h + line_space;
    lv_coord_t line_y1 = dropdown->sel_opt_id * unit_h;

    /*Scroll to the selected option*/
    lv_obj_scroll_to_y(dropdown->list, line_y1, LV_ANIM_OFF);
    lv_obj_invalidate(dropdown->list);
}

static lv_obj_t * get_label(const lv_obj_t * obj)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    if(dropdown->list == NULL) return NULL;

    return lv_obj_get_child(dropdown->list, 0);
}

#endif
