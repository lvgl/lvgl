/**
 * @file lv_dropdown.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_dropdown_private.h"
#include "../../misc/lv_area_private.h"
#include "../../core/lv_obj_class_private.h"
#include "../../core/lv_obj.h"
#if LV_USE_DROPDOWN != 0

#include "../../misc/lv_assert.h"
#include "../../draw/lv_draw_private.h"
#include "../../core/lv_group.h"
#include "../../indev/lv_indev.h"
#include "../../display/lv_display.h"
#include "../../font/lv_symbol_def.h"
#include "../../misc/lv_anim.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_text_ap.h"
#include "../../misc/lv_text_private.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_dropdown_class)
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

static void draw_box(lv_obj_t * dropdown_obj, lv_layer_t * layer, uint32_t id, lv_state_t state);
static void draw_box_label(lv_obj_t * dropdown_obj, lv_layer_t * layer, uint32_t id, lv_state_t state);
static lv_result_t btn_release_handler(lv_obj_t * obj);
static lv_result_t list_release_handler(lv_obj_t * list_obj);
static void list_press_handler(lv_obj_t * page);
static uint32_t get_id_on_point(lv_obj_t * dropdown_obj, int32_t y);
static void position_to_selected(lv_obj_t * dropdown_obj, lv_anim_enable_t anim_en);
static lv_obj_t * get_label(const lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_OBJ_PROPERTY
static const lv_property_ops_t properties[] = {
    {
        .id = LV_PROPERTY_DROPDOWN_TEXT,
        .setter = lv_dropdown_set_text,
        .getter = lv_dropdown_get_text,
    },
    {
        .id = LV_PROPERTY_DROPDOWN_OPTIONS,
        .setter = lv_dropdown_set_options,
        .getter = lv_dropdown_get_options,
    },
    {
        .id = LV_PROPERTY_DROPDOWN_OPTION_COUNT,
        .setter = NULL,
        .getter = lv_dropdown_get_option_count,
    },
    {
        .id = LV_PROPERTY_DROPDOWN_SELECTED,
        .setter = lv_dropdown_set_selected,
        .getter = lv_dropdown_get_selected,
    },
    {
        .id = LV_PROPERTY_DROPDOWN_DIR,
        .setter = lv_dropdown_set_dir,
        .getter = lv_dropdown_get_dir,
    },
    {
        .id = LV_PROPERTY_DROPDOWN_SYMBOL,
        .setter = lv_dropdown_set_symbol,
        .getter = lv_dropdown_get_symbol,
    },
    {
        .id = LV_PROPERTY_DROPDOWN_SELECTED_HIGHLIGHT,
        .setter = lv_dropdown_set_selected_highlight,
        .getter = lv_dropdown_get_selected_highlight,
    },
    {
        .id = LV_PROPERTY_DROPDOWN_LIST,
        .setter = NULL,
        .getter = lv_dropdown_get_list,
    },
    {
        .id = LV_PROPERTY_DROPDOWN_IS_OPEN,
        .setter = NULL,
        .getter = lv_dropdown_is_open,
    },
};
#endif

const lv_obj_class_t lv_dropdown_class = {
    .constructor_cb = lv_dropdown_constructor,
    .destructor_cb = lv_dropdown_destructor,
    .event_cb = lv_dropdown_event,
    .width_def = LV_DPI_DEF,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_dropdown_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .base_class = &lv_obj_class,
    .name = "lv_dropdown",
#if LV_USE_OBJ_PROPERTY
    .prop_index_start = LV_PROPERTY_DROPDOWN_START,
    .prop_index_end = LV_PROPERTY_DROPDOWN_END,
    .properties = properties,
    .properties_count = sizeof(properties) / sizeof(properties[0]),

#if LV_USE_OBJ_PROPERTY_NAME
    .property_names = lv_dropdown_property_names,
    .names_count = sizeof(lv_dropdown_property_names) / sizeof(lv_property_name_t),
#endif
#endif
};

const lv_obj_class_t lv_dropdownlist_class = {
    .constructor_cb = lv_dropdownlist_constructor,
    .destructor_cb = lv_dropdownlist_destructor,
    .event_cb = lv_dropdown_list_event,
    .instance_size = sizeof(lv_dropdown_list_t),
    .base_class = &lv_obj_class,
    .name = "lv_dropdown-list",
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
    size_t len = lv_strlen(options) + 1;
#else
    size_t len = lv_text_ap_calc_bytes_count(options) + 1;
#endif

    if(dropdown->options != NULL && dropdown->static_txt == 0) {
        lv_free(dropdown->options);
        dropdown->options = NULL;
    }

    dropdown->options = lv_malloc(len);

    LV_ASSERT_MALLOC(dropdown->options);
    if(dropdown->options == NULL) return;

#if LV_USE_ARABIC_PERSIAN_CHARS == 0
    lv_strcpy(dropdown->options, options);
#else
    lv_text_ap_proc(options, dropdown->options);
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
        lv_free(dropdown->options);
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
        dropdown->options = lv_strdup(static_options);
        LV_ASSERT_MALLOC(dropdown->options);
        if(dropdown->options == NULL) return;
        dropdown->static_txt = 0;
    }

    /*Allocate space for the new option*/
    size_t old_len = (dropdown->options == NULL) ? 0 : lv_strlen(dropdown->options);
#if LV_USE_ARABIC_PERSIAN_CHARS == 0
    size_t ins_len = lv_strlen(option) + 1;
#else
    size_t ins_len = lv_text_ap_calc_bytes_count(option) + 1;
#endif

    size_t new_len = ins_len + old_len + 2; /*+2 for terminating NULL and possible \n*/
    dropdown->options        = lv_realloc(dropdown->options, new_len + 1);
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
        lv_text_ins(dropdown->options, lv_text_encoded_get_char_id(dropdown->options, insert_pos++), "\n");

    /*Insert the new option, adding \n if necessary*/
    char * ins_buf = lv_malloc(ins_len + 2); /*+ 2 for terminating NULL and possible \n*/
    LV_ASSERT_MALLOC(ins_buf);
    if(ins_buf == NULL) return;
#if LV_USE_ARABIC_PERSIAN_CHARS == 0
    lv_strcpy(ins_buf, option);
#else
    lv_text_ap_proc(option, ins_buf);
#endif
    if(pos < dropdown->option_cnt) lv_strcat(ins_buf, "\n");

    lv_text_ins(dropdown->options, lv_text_encoded_get_char_id(dropdown->options, insert_pos), ins_buf);
    lv_free(ins_buf);

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
        lv_free(dropdown->options);

    dropdown->options = NULL;
    dropdown->static_txt = 0;
    dropdown->option_cnt = 0;

    lv_obj_invalidate(obj);
    if(dropdown->list) lv_obj_invalidate(dropdown->list);
}

void lv_dropdown_set_selected(lv_obj_t * obj, uint32_t sel_opt, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    if(dropdown->sel_opt_id == sel_opt) return;

    dropdown->sel_opt_id      = sel_opt < dropdown->option_cnt ? sel_opt : dropdown->option_cnt - 1;
    dropdown->sel_opt_id_orig = dropdown->sel_opt_id;

    if(dropdown->list) {
        position_to_selected(obj, anim);
    }

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

uint32_t lv_dropdown_get_selected(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    return dropdown->sel_opt_id;
}

uint32_t lv_dropdown_get_option_count(const lv_obj_t * obj)
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
        txt_len     = lv_strlen(dropdown->options);
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
            LV_LOG_WARN("the buffer was too small");
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
    const size_t option_len = lv_strlen(option); /*avoid recomputing this multiple times in the loop*/

    while(start[0] != '\0') {
        for(char_i = 0; (start[char_i] != '\n') && (start[char_i] != '\0'); char_i++);

        if(option_len == char_i && lv_memcmp(start, option, LV_MIN(option_len, char_i)) == 0) {
            return opt_i;
        }

        start = &start[char_i];
        if(start[0] == '\n') start++;
        char_i = 0;
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
    lv_obj_remove_flag(dropdown->list, LV_OBJ_FLAG_HIDDEN);

    /*To allow styling the list*/
    lv_obj_send_event(dropdown_obj, LV_EVENT_READY, NULL);

    lv_obj_t * label = get_label(dropdown_obj);
    lv_label_set_text_static(label, dropdown->options);
    lv_obj_set_width(dropdown->list, LV_SIZE_CONTENT);

    lv_obj_update_layout(label);
    /*Set smaller width to the width of the button*/
    if(lv_obj_get_width(dropdown->list) <= lv_obj_get_width(dropdown_obj) &&
       (dropdown->dir == LV_DIR_TOP || dropdown->dir == LV_DIR_BOTTOM)) {
        lv_obj_set_width(dropdown->list, lv_obj_get_width(dropdown_obj));
    }

    int32_t label_h = lv_obj_get_height(label);
    int32_t border_width = lv_obj_get_style_border_width(dropdown->list, LV_PART_MAIN);
    int32_t top = lv_obj_get_style_pad_top(dropdown->list, LV_PART_MAIN) + border_width;
    int32_t bottom = lv_obj_get_style_pad_bottom(dropdown->list, LV_PART_MAIN) + border_width;

    int32_t list_fit_h = label_h + top + bottom;
    int32_t list_h = list_fit_h;

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

    position_to_selected(dropdown_obj, LV_ANIM_OFF);

    if(dir == LV_DIR_BOTTOM)     lv_obj_align_to(dropdown->list, dropdown_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    else if(dir == LV_DIR_TOP)   lv_obj_align_to(dropdown->list, dropdown_obj, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    else if(dir == LV_DIR_LEFT)  lv_obj_align_to(dropdown->list, dropdown_obj, LV_ALIGN_OUT_LEFT_TOP, 0, 0);
    else if(dir == LV_DIR_RIGHT) lv_obj_align_to(dropdown->list, dropdown_obj, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);

    lv_obj_update_layout(dropdown->list);

    if(dropdown->dir == LV_DIR_LEFT || dropdown->dir == LV_DIR_RIGHT) {
        int32_t y1 = lv_obj_get_y(dropdown->list);
        int32_t y2 = lv_obj_get_y2(dropdown->list);
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
            lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
            break;

    }
}

void lv_dropdown_close(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_remove_state(obj, LV_STATE_CHECKED);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    dropdown->pr_opt_id = LV_DROPDOWN_PR_NONE;
    lv_obj_add_flag(dropdown->list, LV_OBJ_FLAG_HIDDEN);

    lv_obj_send_event(obj, LV_EVENT_CANCEL, NULL);
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
#if LV_WIDGETS_HAS_DEFAULT_VALUE
    lv_dropdown_set_options_static(obj, "Option 1\nOption 2\nOption 3");
#endif

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
        lv_obj_delete(dropdown->list);
        dropdown->list = NULL;
    }

    if(!dropdown->static_txt) {
        lv_free(dropdown->options);
        dropdown->options = NULL;
    }
}

static void lv_dropdownlist_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
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

    lv_result_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;

    if(code == LV_EVENT_FOCUSED) {
        lv_group_t * g             = lv_obj_get_group(obj);
        bool editing               = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_active());

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
        if(res != LV_RESULT_OK) return;
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
        uint32_t c = lv_event_get_key(e);
        if(c == LV_KEY_RIGHT || c == LV_KEY_DOWN) {
            if(!lv_dropdown_is_open(obj)) {
                lv_dropdown_open(obj);
            }
            else if(dropdown->sel_opt_id + 1 < dropdown->option_cnt) {
                dropdown->sel_opt_id++;
                position_to_selected(obj, LV_ANIM_ON);
            }
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_UP) {

            if(!lv_dropdown_is_open(obj)) {
                lv_dropdown_open(obj);
            }
            else if(dropdown->sel_opt_id > 0) {
                dropdown->sel_opt_id--;
                position_to_selected(obj, LV_ANIM_ON);
            }
        }
        else if(c == LV_KEY_ESC) {
            dropdown->sel_opt_id = dropdown->sel_opt_id_orig;
            lv_dropdown_close(obj);
        }
        else if(c == LV_KEY_ENTER) {
            /* Handle the ENTER key only if it was send by another object.
             * Do no process it if ENTER is sent by the dropdown because it's handled in LV_EVENT_RELEASED */
            lv_obj_t * indev_obj = lv_indev_get_active_obj();
            if(indev_obj != obj) {
                res = btn_release_handler(obj);
                if(res != LV_RESULT_OK) return;
            }
        }
    }
    else if(code == LV_EVENT_ROTARY) {
        if(!lv_dropdown_is_open(obj)) {
            lv_dropdown_open(obj);
        }
        else {
            int32_t r = lv_event_get_rotary_diff(e);
            int32_t new_id = dropdown->sel_opt_id + r;
            new_id = LV_CLAMP(0, new_id, (int32_t)dropdown->option_cnt - 1);

            dropdown->sel_opt_id = new_id;
            position_to_selected(obj, LV_ANIM_ON);
        }
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_main(e);
    }
}

static void lv_dropdown_list_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /*Call the ancestor's event handler*/
    lv_event_code_t code = lv_event_get_code(e);
    if(code != LV_EVENT_DRAW_POST) {
        res = lv_obj_event_base(MY_CLASS_LIST, e);
        if(res != LV_RESULT_OK) return;
    }
    lv_obj_t * list = lv_event_get_current_target(e);
    lv_obj_t * dropdown_obj = ((lv_dropdown_list_t *)list)->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;

    if(code == LV_EVENT_RELEASED) {
        if(lv_indev_get_scroll_obj(lv_indev_active()) == NULL) {
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
        if(res != LV_RESULT_OK) return;
    }
}

static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    lv_layer_t * layer = lv_event_get_layer(e);

    int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    int32_t left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
    int32_t right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN) + border_width;

    lv_draw_label_dsc_t symbol_dsc;
    lv_draw_label_dsc_init(&symbol_dsc);
    symbol_dsc.base.layer = layer;
    lv_obj_init_draw_label_dsc(obj, LV_PART_INDICATOR, &symbol_dsc);

    /*If no text specified use the selected option*/
    const char * opt_txt;
    char buf[128];
    if(dropdown->text) opt_txt = dropdown->text;
    else {
        lv_dropdown_get_selected_str(obj, buf, 128);
        opt_txt = buf;
    }

    bool symbol_to_left = false;
    if(dropdown->dir == LV_DIR_LEFT) symbol_to_left = true;
    if(lv_obj_get_style_base_dir(obj, LV_PART_MAIN) == LV_BASE_DIR_RTL) symbol_to_left = true;

    if(dropdown->symbol) {
        lv_image_src_t symbol_type = lv_image_src_get_type(dropdown->symbol);
        int32_t symbol_w;
        int32_t symbol_h;
        if(symbol_type == LV_IMAGE_SRC_SYMBOL) {
            lv_point_t size;
            lv_text_get_size(&size, dropdown->symbol, symbol_dsc.font, symbol_dsc.letter_space, symbol_dsc.line_space, LV_COORD_MAX,
                             symbol_dsc.flag);
            symbol_w = size.x;
            symbol_h = size.y;
        }
        else {
            lv_image_header_t header;
            lv_result_t res = lv_image_decoder_get_info(dropdown->symbol, &header);
            if(res == LV_RESULT_OK) {
                symbol_w = header.w;
                symbol_h = header.h;
            }
            else {
                symbol_w = -1;
                symbol_h = -1;
            }
        }

        lv_area_t symbol_area;
        symbol_area.y1 = obj->coords.y1;
        symbol_area.y2 = symbol_area.y1 + symbol_h - 1;
        symbol_area.x1 = obj->coords.x1;
        symbol_area.x2 = symbol_area.x1 + symbol_w - 1;
        if(symbol_to_left) {
            lv_area_align(&obj->coords, &symbol_area, LV_ALIGN_LEFT_MID, left, 0);
        }
        else {
            lv_area_align(&obj->coords, &symbol_area, LV_ALIGN_RIGHT_MID, -right, 0);
        }

        if(symbol_type == LV_IMAGE_SRC_SYMBOL) {
            symbol_dsc.text = dropdown->symbol;
            lv_draw_label(layer, &symbol_dsc, &symbol_area);
        }
        else {
            lv_draw_image_dsc_t img_dsc;
            lv_draw_image_dsc_init(&img_dsc);
            img_dsc.base.layer = layer;
            lv_obj_init_draw_image_dsc(obj, LV_PART_INDICATOR, &img_dsc);
            lv_point_set(&img_dsc.pivot, symbol_w / 2, symbol_h / 2);
            img_dsc.rotation = lv_obj_get_style_transform_rotation(obj, LV_PART_INDICATOR);
            img_dsc.src = dropdown->symbol;
            lv_draw_image(layer, &img_dsc, &symbol_area);
        }
    }

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.base.layer = layer;
    lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &label_dsc);

    lv_point_t size;
    lv_text_get_size(&size, opt_txt, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX,
                     label_dsc.flag);

    lv_area_t txt_area;
    txt_area.x1 = obj->coords.x1;
    txt_area.x2 = txt_area.x1 + size.x - 1;
    txt_area.y1 = obj->coords.y1;
    txt_area.y2 = txt_area.y1 + size.y - 1;
    /*Center align the text if no symbol*/
    if(dropdown->symbol == NULL) {
        lv_area_align(&obj->coords, &txt_area, LV_ALIGN_CENTER, 0, 0);
    }
    else {
        /*Text to the right*/
        if(symbol_to_left) {
            lv_area_align(&obj->coords, &txt_area, LV_ALIGN_RIGHT_MID, -right, 0);
        }
        else {
            lv_area_align(&obj->coords, &txt_area, LV_ALIGN_LEFT_MID, left, 0);
        }
    }

    label_dsc.text = opt_txt;
    if(dropdown->text == NULL) {
        label_dsc.text_local = true;
    }

    lv_draw_label(layer, &label_dsc, &txt_area);
}

static void draw_list(lv_event_t * e)
{
    lv_obj_t * list_obj = lv_event_get_current_target(e);
    lv_dropdown_list_t * list = (lv_dropdown_list_t *)list_obj;
    lv_obj_t * dropdown_obj = list->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;
    lv_layer_t * layer = lv_event_get_layer(e);

    /* Clip area might be too large too to shadow but
     * the selected option can be drawn on only the background*/
    lv_area_t clip_area_core;
    bool has_common;
    has_common = lv_area_intersect(&clip_area_core, &layer->_clip_area, &dropdown->list->coords);
    if(has_common) {
        const lv_area_t clip_area_ori = layer->_clip_area;
        layer->_clip_area = clip_area_core;
        if(dropdown->selected_highlight) {
            if(dropdown->pr_opt_id == dropdown->sel_opt_id) {
                draw_box(dropdown_obj, layer, dropdown->pr_opt_id, LV_STATE_CHECKED | LV_STATE_PRESSED);
                draw_box_label(dropdown_obj, layer, dropdown->pr_opt_id, LV_STATE_CHECKED | LV_STATE_PRESSED);
            }
            else {
                draw_box(dropdown_obj, layer, dropdown->pr_opt_id, LV_STATE_PRESSED);
                draw_box_label(dropdown_obj, layer, dropdown->pr_opt_id, LV_STATE_PRESSED);
                draw_box(dropdown_obj, layer, dropdown->sel_opt_id, LV_STATE_CHECKED);
                draw_box_label(dropdown_obj, layer, dropdown->sel_opt_id, LV_STATE_CHECKED);
            }
        }
        else {
            draw_box(dropdown_obj, layer, dropdown->pr_opt_id, LV_STATE_PRESSED);
            draw_box_label(dropdown_obj, layer, dropdown->pr_opt_id, LV_STATE_PRESSED);
        }
        layer->_clip_area = clip_area_ori;
    }
}

static void draw_box(lv_obj_t * dropdown_obj, lv_layer_t * layer, uint32_t id, lv_state_t state)
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
    int32_t line_space = lv_obj_get_style_text_line_space(list_obj,  LV_PART_SELECTED);
    int32_t font_h         = lv_font_get_line_height(font);

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
    sel_rect.base.layer = layer;
    lv_obj_init_draw_rect_dsc(list_obj,  LV_PART_SELECTED, &sel_rect);
    lv_draw_rect(layer, &sel_rect, &rect_area);

    list_obj->state = state_ori;
    list_obj->skip_trans = 0;
}

static void draw_box_label(lv_obj_t * dropdown_obj, lv_layer_t * layer, uint32_t id, lv_state_t state)
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
    label_dsc.base.layer = layer;
    lv_obj_init_draw_label_dsc(list_obj, LV_PART_SELECTED, &label_dsc);

    label_dsc.line_space = lv_obj_get_style_text_line_space(list_obj,
                                                            LV_PART_SELECTED);  /*Line space should come from the list*/

    lv_obj_t * label = get_label(dropdown_obj);
    if(label == NULL) return;

    int32_t font_h        = lv_font_get_line_height(label_dsc.font);

    lv_area_t area_sel;
    area_sel.y1 = label->coords.y1;
    area_sel.y1 += id * (font_h + label_dsc.line_space);
    area_sel.y1 -= label_dsc.line_space / 2;

    area_sel.y2 = area_sel.y1 + font_h + label_dsc.line_space - 1;
    area_sel.x1 = list_obj->coords.x1;
    area_sel.x2 = list_obj->coords.x2;
    lv_area_t mask_sel;
    bool area_ok;
    area_ok = lv_area_intersect(&mask_sel, &layer->_clip_area, &area_sel);
    if(area_ok) {
        const lv_area_t clip_area_ori = layer->_clip_area;
        layer->_clip_area = mask_sel;
        label_dsc.text = lv_label_get_text(label);
        lv_draw_label(layer, &label_dsc, &label->coords);
        layer->_clip_area = clip_area_ori;
    }
    list_obj->state = state_orig;
    list_obj->skip_trans = 0;
}

static lv_result_t btn_release_handler(lv_obj_t * obj)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    lv_indev_t * indev = lv_indev_active();
    if(lv_indev_get_scroll_obj(indev) == NULL) {
        if(lv_dropdown_is_open(obj)) {
            lv_dropdown_close(obj);
            if(dropdown->sel_opt_id_orig != dropdown->sel_opt_id) {
                dropdown->sel_opt_id_orig = dropdown->sel_opt_id;
                lv_result_t res;
                uint32_t id  = dropdown->sel_opt_id; /*Just to use uint32_t in event data*/
                res = lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, &id);
                if(res != LV_RESULT_OK) return res;
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
    return LV_RESULT_OK;
}

/**
 * Called when a drop down list is released to open it or set new option
 * @param list pointer to the drop down list's list
 * @return LV_RESULT_INVALID if the list is not being deleted in the user callback. Else LV_RESULT_OK
 */
static lv_result_t list_release_handler(lv_obj_t * list_obj)
{
    lv_dropdown_list_t * list = (lv_dropdown_list_t *) list_obj;
    lv_obj_t * dropdown_obj = list->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;

    lv_indev_t * indev = lv_indev_active();
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
    lv_result_t res = lv_obj_send_event(dropdown_obj, LV_EVENT_VALUE_CHANGED, &id);
    if(res != LV_RESULT_OK) return res;

    return LV_RESULT_OK;
}

static void list_press_handler(lv_obj_t * list_obj)
{
    lv_dropdown_list_t * list = (lv_dropdown_list_t *) list_obj;
    lv_obj_t * dropdown_obj = list->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;

    lv_indev_t * indev = lv_indev_active();
    if(indev && (lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER || lv_indev_get_type(indev) == LV_INDEV_TYPE_BUTTON)) {
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        dropdown->pr_opt_id = get_id_on_point(dropdown_obj, p.y);
        lv_obj_invalidate(list_obj);
    }
}

static uint32_t get_id_on_point(lv_obj_t * dropdown_obj, int32_t y)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;
    lv_obj_t * label = get_label(dropdown_obj);
    if(label == NULL) return 0;
    y -= label->coords.y1;

    const lv_font_t * font         = lv_obj_get_style_text_font(label, LV_PART_MAIN);
    int32_t font_h              = lv_font_get_line_height(font);
    int32_t line_space = lv_obj_get_style_text_line_space(label, LV_PART_MAIN);

    y += line_space / 2;
    int32_t h = font_h + line_space;

    uint32_t opt = y / h;

    if(opt >= dropdown->option_cnt) opt = dropdown->option_cnt - 1;
    return opt;
}

/**
 * Set the position of list when it is closed to show the selected item
 * @param ddlist pointer to a drop down list
 */
static void position_to_selected(lv_obj_t * dropdown_obj, lv_anim_enable_t anim_en)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *)dropdown_obj;

    lv_obj_t * label = get_label(dropdown_obj);
    if(label == NULL) return;

    if(lv_obj_get_height(label) <= lv_obj_get_content_height(dropdown_obj)) return;

    const lv_font_t * font         = lv_obj_get_style_text_font(label, LV_PART_MAIN);
    int32_t font_h              = lv_font_get_line_height(font);
    int32_t line_space = lv_obj_get_style_text_line_space(label, LV_PART_MAIN);
    int32_t unit_h = font_h + line_space;
    int32_t line_y1 = dropdown->sel_opt_id * unit_h;

    /*Scroll to the selected option*/
    lv_obj_scroll_to_y(dropdown->list, line_y1, anim_en);
    lv_obj_invalidate(dropdown->list);
}

static lv_obj_t * get_label(const lv_obj_t * obj)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *)obj;
    if(dropdown->list == NULL) return NULL;

    return lv_obj_get_child(dropdown->list, 0);
}

#endif
