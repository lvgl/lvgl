/**
 * @file lv_ddlist.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_dropdown.h"
#if LV_USE_DROPDOWN != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_core/lv_group.h"
#include "../lv_core/lv_indev.h"
#include "../lv_core/lv_disp.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_font/lv_symbol_def.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_txt_ap.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_dropdown"

#if LV_USE_ANIMATION == 0
    #undef LV_DROPDOWN_DEF_ANIM_TIME
    #define LV_DROPDOWN_DEF_ANIM_TIME 0 /*No animation*/
#endif

#define LV_DROPDOWN_PR_NONE 0xFFFF

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_dropdown_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_dropdown_destructor(lv_obj_t * obj);
static lv_draw_res_t lv_dropdown_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_dropdown_signal(lv_obj_t * obj, lv_signal_t sign, void * param);

static void lv_dropdown_list_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_dropdown_list_destructor(lv_obj_t * obj);
static lv_draw_res_t lv_dropdown_list_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_dropdown_list_signal(lv_obj_t * list, lv_signal_t sign, void * param);
static void draw_box(lv_obj_t * dropdown_obj, const lv_area_t * clip_area, uint16_t id, lv_state_t state);
static void draw_box_label(lv_obj_t * dropdown_obj, const lv_area_t * clip_area, uint16_t id, lv_state_t state);
static lv_res_t list_release_handler(lv_obj_t * list_obj);
static void page_press_handler(lv_obj_t * page);
static uint16_t get_id_on_point(lv_obj_t * dropdown_obj, lv_coord_t y);
static void position_to_selected(lv_obj_t * obj);
static lv_obj_t * get_label(const lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_dropdown = {
    .constructor = lv_dropdown_constructor,
    .destructor = lv_dropdown_destructor,
    .signal_cb = lv_dropdown_signal,
    .draw_cb = lv_dropdown_draw,
    .instance_size = sizeof(lv_dropdown_t),
    .base_class = &lv_obj
};

const lv_obj_class_t lv_dropdown_list = {
    .constructor = lv_dropdown_list_constructor,
    .destructor = lv_dropdown_list_destructor,
    .signal_cb = lv_dropdown_list_signal,
    .draw_cb = lv_dropdown_list_draw,
    .instance_size = sizeof(lv_dropdown_list_t),
    .base_class = &lv_obj
};


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a drop down list objects
 * @param par pointer to an object, it will be the parent of the new drop down list
 * @param copy pointer to a drop down list object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created drop down list
 */
lv_obj_t * lv_dropdown_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    return lv_obj_create_from_class(&lv_dropdown, parent, copy);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set text of the ddlist (Displayed on the button if `show_selected = false`)
 * @param ddlist pointer to a drop down list object
 * @param txt the text as a string (Only it's pointer is saved)
 */
void lv_dropdown_set_text(lv_obj_t * obj, const char * txt)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;
    if(dropdown->text == txt) return;

    dropdown->text = txt;

    lv_obj_invalidate(obj);
}

/**
 * Clear all options in a drop down list.  Static or dynamic.
 * @param ddlist pointer to drop down list object
 */
void lv_dropdown_clear_options(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;
    if(dropdown->options == NULL) return;

    if(dropdown->static_txt == 0)
        lv_mem_free(dropdown->options);

    dropdown->options = NULL;
    dropdown->static_txt = 0;
    dropdown->option_cnt = 0;

    lv_obj_invalidate(obj);
}

/**
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 * The options string can be destroyed after calling this function
 */
void lv_dropdown_set_options(lv_obj_t * obj, const char * options)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_STR(options);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

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

    LV_ASSERT_MEM(dropdown->options);
    if(dropdown->options == NULL) return;

#if LV_USE_ARABIC_PERSIAN_CHARS == 0
    strcpy(dropdown->options, options);
#else
    _lv_txt_ap_proc(options, dropdown->options);
#endif

    /*Now the text is dynamically allocated*/
    dropdown->static_txt = 0;
}

/**
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a static string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void lv_dropdown_set_options_static(lv_obj_t * obj, const char * options)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_STR(options);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

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
}

/**
 * Add an options to a drop down list from a string.  Only works for dynamic options.
 * @param ddlist pointer to drop down list object
 * @param option a string without '\n'. E.g. "Four"
 * @param pos the insert position, indexed from 0, LV_DROPDOWN_POS_LAST = end of string
 */
void lv_dropdown_add_option(lv_obj_t * obj, const char * option, uint32_t pos)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_STR(option);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

    /*Convert static options to dynamic*/
    if(dropdown->static_txt != 0) {
        char * static_options = dropdown->options;
        size_t len = strlen(static_options) + 1;

        dropdown->options = lv_mem_alloc(len);
        LV_ASSERT_MEM(dropdown->options);
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

    size_t new_len = ins_len + old_len + 2; /* +2 for terminating NULL and possible \n */
    dropdown->options        = lv_mem_realloc(dropdown->options, new_len + 1);
    LV_ASSERT_MEM(dropdown->options);
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
    char * ins_buf = lv_mem_buf_get(ins_len + 2); /* + 2 for terminating NULL and possible \n */
    LV_ASSERT_MEM(ins_buf);
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
}

/**
 * Set the selected option
 * @param ddlist pointer to drop down list object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 */
void lv_dropdown_set_selected(lv_obj_t * obj, uint16_t sel_opt)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;
    if(dropdown->sel_opt_id == sel_opt) return;

    dropdown->sel_opt_id      = sel_opt < dropdown->option_cnt ? sel_opt : dropdown->option_cnt - 1;
    dropdown->sel_opt_id_orig = dropdown->sel_opt_id;

    lv_obj_invalidate(obj);
}

/**
 * Set the direction of the a drop down list
 * @param ddlist pointer to a drop down list object
 * @param dir LV_DIR_LEFT/RIGHT/TOP/BOTTOM
 */
void lv_dropdown_set_dir(lv_obj_t * obj, lv_dir_t dir)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;
    if(dropdown->dir == dir) return;

    dropdown->dir = dir;

    lv_obj_invalidate(obj);
}

/**
 * Set the maximal height for the drop down list
 * @param ddlist pointer to a drop down list
 * @param h the maximal height
 */
void lv_dropdown_set_max_height(lv_obj_t * obj, lv_coord_t h)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;
    if(dropdown->max_height == h) return;

    dropdown->max_height = h;
}

/**
 * Set an arrow or other symbol to display when the drop-down list is closed.
 * @param ddlist pointer to drop down list object
 * @param symbol a text like `LV_SYMBOL_DOWN` or NULL to not draw icon
 */
void lv_dropdown_set_symbol(lv_obj_t * obj, const void * symbol)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;
    dropdown->symbol = symbol;
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get text of the ddlist (Displayed on the button if `show_selected = false`)
 * @param ddlist pointer to a drop down list object
 * @return the text string
 */
const char * lv_dropdown_get_text(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

    return dropdown->text;
}

/**
 * Get the options of a drop down list
 * @param ddlist pointer to drop down list object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * lv_dropdown_get_options(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;
    return dropdown->options;
}

/**
 * Get the selected option
 * @param ddlist pointer to drop down list object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_dropdown_get_selected(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

    return dropdown->sel_opt_id;
}

/**
 * Get the total number of options
 * @param ddlist pointer to drop down list object
 * @return the total number of options in the list
 */
uint16_t lv_dropdown_get_option_cnt(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

    return dropdown->option_cnt;
}

/**
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 * @param buf_size size of `buf` in bytes. 0: to ignore it.
 */
void lv_dropdown_get_selected_str(const lv_obj_t * obj, char * buf, uint32_t buf_size)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

    uint32_t i;
    uint32_t line        = 0;
    size_t txt_len     = strlen(dropdown->options);

    for(i = 0; i < txt_len && line != dropdown->sel_opt_id_orig; i++) {
        if(dropdown->options[i] == '\n') line++;
    }

    uint32_t c;
    for(c = 0; i < txt_len && dropdown->options[i] != '\n'; c++, i++) {
        if(buf_size && c >= buf_size - 1) {
            LV_LOG_WARN("lv_dropdown_get_selected_str: the buffer was too small")
            break;
        }
        buf[c] = dropdown->options[i];
    }

    buf[c] = '\0';
}

/**
 * Get the fix height value.
 * @param ddlist pointer to a drop down list object
 * @return the height if the ddlist is opened (0: auto size)
 */
lv_coord_t lv_dropdown_get_max_height(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;
    return dropdown->max_height;
}

/**
 * Get the symbol to draw when the drop-down list is closed
 * @param ddlist pointer to drop down list object
 * @return the symbol or NULL if not enabled
 */
const char * lv_dropdown_get_symbol(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

    return dropdown->symbol;
}

/**
 * Get the direction of the drop down list
 * @param ddlist pointer to a drop down list object
 * @return LV_DIR_LEF/RIGHT/TOP/BOTTOM
 */
lv_dir_t lv_dropdown_get_dir(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

    return dropdown->dir;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Open the drop down list with or without animation
 * @param ddlist pointer to drop down list object
 */
void lv_dropdown_open(lv_obj_t * dropdown_obj)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *) dropdown_obj;

    lv_obj_add_state(dropdown_obj, LV_STATE_CHECKED);

    lv_obj_clear_flag(dropdown->list, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(dropdown->list, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_parent(dropdown->list, lv_obj_get_screen(dropdown_obj));
    /*Set smaller width to the width of the button*/
    if(lv_obj_get_width(dropdown->list) <= lv_obj_get_width(dropdown_obj) &&
       (dropdown->dir == LV_DIR_TOP || dropdown->dir == LV_DIR_BOTTOM)) {
        lv_obj_set_width(dropdown->list, lv_obj_get_width(dropdown_obj));
    } else {
        lv_obj_set_width(dropdown->list, LV_SIZE_AUTO);
    }

    lv_obj_t * label = get_label(dropdown_obj);
    lv_label_set_text_static(label, dropdown->options);

    lv_coord_t label_h = lv_obj_get_height(label);
    lv_coord_t top = lv_obj_get_style_pad_top(dropdown->list, LV_PART_MAIN);
    lv_coord_t bottom = lv_obj_get_style_pad_bottom(dropdown->list, LV_PART_MAIN);

    lv_coord_t list_fit_h = label_h + top + bottom;
    lv_coord_t list_h = list_fit_h;
    if(list_h > dropdown->max_height) list_h = dropdown->max_height;

    lv_dir_t dir = dropdown->dir;
    /*No space on the bottom? See if top is better.*/
    if(dropdown->dir == LV_DIR_BOTTOM) {
        if(dropdown_obj->coords.y2 + list_h > LV_VER_RES) {
            if(dropdown_obj->coords.y1 > LV_VER_RES - dropdown_obj->coords.y2) {
                /*There is more space on the top, so make it drop up*/
                dir = LV_DIR_TOP;
                list_h = dropdown_obj->coords.y1;
            }
            else {
                list_h = LV_VER_RES - dropdown_obj->coords.y2;
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
    if(list_h > dropdown->max_height) list_h = dropdown->max_height;

    lv_obj_set_height(dropdown->list, list_h);

    position_to_selected(dropdown_obj);

    if(dir == LV_DIR_BOTTOM)      lv_obj_align(dropdown->list, dropdown_obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    else if(dir == LV_DIR_TOP)   lv_obj_align(dropdown->list, dropdown_obj, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    else if(dir == LV_DIR_LEFT) lv_obj_align(dropdown->list, dropdown_obj, LV_ALIGN_OUT_LEFT_TOP, 0, 0);
    else if(dir == LV_DIR_RIGHT)lv_obj_align(dropdown->list, dropdown_obj, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);

    if(dropdown->dir == LV_DIR_LEFT || dropdown->dir == LV_DIR_RIGHT) {
        if(dropdown->list->coords.y2 > LV_VER_RES) {
            lv_obj_set_y(dropdown->list, lv_obj_get_y(dropdown->list) - (dropdown->list->coords.y2 - LV_VER_RES));
        }
    }

    lv_text_align_t align = lv_obj_get_style_text_align(label, LV_PART_MAIN);
    switch(align) {
    default:
    case LV_TEXT_ALIGN_LEFT:
        lv_obj_set_x(label, 0);
        break;
    case LV_TEXT_ALIGN_RIGHT:
        lv_obj_set_x(label, lv_obj_get_width_fit(dropdown->list) - lv_obj_get_width(label));
        break;
    case LV_TEXT_ALIGN_CENTER:
        lv_obj_set_x(label, lv_obj_get_width_fit(dropdown->list) / 2 - lv_obj_get_width(label) / 2);
        break;

    }
}

/**
 * Close (Collapse) the drop down list
 * @param ddlist pointer to drop down list object
 */
void lv_dropdown_close(lv_obj_t * obj)
{
    lv_obj_clear_state(obj, LV_STATE_CHECKED);
    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

    dropdown->pr_opt_id = LV_DROPDOWN_PR_NONE;
    lv_obj_add_flag(dropdown->list, LV_OBJ_FLAG_HIDDEN);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Create a switch objects
 * @param parent pointer to an object, it will be the parent of the new switch
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other switch to copy.
 * @return pointer to the created switch
 */
lv_obj_t * lv_dropdown_list_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    return lv_obj_create_from_class(&lv_dropdown_list, parent, copy);
}

static void lv_dropdown_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("dropdown create started");

    lv_obj_construct_base(obj, parent, copy);

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

    /*Initialize the allocated 'ext' */
    dropdown->list          = NULL;
    dropdown->options     = NULL;
    dropdown->symbol         = LV_SYMBOL_DOWN;
    dropdown->text         = NULL;
    dropdown->static_txt = 1;
    dropdown->sel_opt_id      = 0;
    dropdown->sel_opt_id_orig = 0;
    dropdown->pr_opt_id = LV_DROPDOWN_PR_NONE;
    dropdown->option_cnt      = 0;
    dropdown->dir = LV_DIR_BOTTOM;
    dropdown->max_height = (3 * lv_disp_get_ver_res(NULL)) / 4;

    lv_obj_t * list_obj = lv_dropdown_list_create(parent, copy);
    ((lv_dropdown_list_t*)list_obj)->dropdown = (lv_obj_t *) dropdown;
    dropdown->list = list_obj;
    lv_obj_add_flag(dropdown->list, LV_OBJ_FLAG_HIDDEN);

    if(copy == NULL) {
        lv_obj_set_width(obj, LV_DPX(150));
        lv_dropdown_set_options_static(obj, "Option 1\nOption 2\nOption 3");
    }
    /*Copy an existing drop down list*/
    else {
//        lv_dropdown_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
//        if(copy_dropdown->static_txt == 0)
//            lv_dropdown_set_options(ddlist, lv_dropdown_get_options(copy));
//        else
//            lv_dropdown_set_options_static(ddlist, lv_dropdown_get_options(copy));
//        dropdown->option_cnt        = copy_dropdown->option_cnt;
//        dropdown->sel_opt_id     = copy_dropdown->sel_opt_id;
//        dropdown->sel_opt_id_orig = copy_dropdown->sel_opt_id;
//        dropdown->symbol           = copy_dropdown->symbol;
//        dropdown->max_height      = copy_dropdown->max_height;
//        dropdown->text      = copy_dropdown->text;
//        dropdown->dir      = copy_dropdown->dir;
    }

   LV_LOG_INFO("dropdown created");
}

static void lv_dropdown_destructor(lv_obj_t * obj)
{
//    lv_bar_t * bar = obj;
//
//    _lv_obj_reset_style_list_no_refr(obj, LV_BAR_PART_INDIC);
//    _lv_obj_reset_style_list_no_refr(sw, LV_PART_KNOB);
//
//    bar->class_p->base_p->destructor(obj);
}

/**
 * Handle the drawing related tasks of the drop down list
 * @param ddlist pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DRAW_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DRAW_DRAW: draw the object (always return 'true')
 *             LV_DRAW_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_draw_res_t`
 */
static lv_draw_res_t lv_dropdown_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DRAW_MODE_COVER_CHECK) {
        return lv_obj.draw_cb(obj, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {
        lv_obj.draw_cb(obj, clip_area, mode);

        lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

        lv_coord_t left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        lv_coord_t right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
        lv_coord_t top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &label_dsc);

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
        if(lv_obj_get_base_dir(obj) == LV_BIDI_DIR_RTL) symbol_to_left = true;

        if(dropdown->symbol) {
            lv_img_src_t symbol_type = lv_img_src_get_type(dropdown->symbol);
            lv_coord_t symbol_w;
            lv_coord_t symbol_h;
            if(symbol_type == LV_IMG_SRC_SYMBOL) {
                lv_point_t size;
                _lv_txt_get_size(&size, dropdown->symbol, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX,
                                           label_dsc.flag);
                symbol_w = size.x;
                symbol_h = size.y;
            } else {
                lv_img_header_t header;
                lv_res_t res = lv_img_decoder_get_info(dropdown->symbol, &header);
                if(res == LV_RES_OK) {
                    symbol_w = header.w;
                    symbol_h = header.h;
                } else {
                    symbol_w = -1;
                    symbol_h = -1;
                }
            }

            lv_area_t symbol_area;
            symbol_area.y1 = obj->coords.y1 + top;
            symbol_area.y2 = symbol_area.y1 + symbol_h - 1;
            if(symbol_to_left) {
                symbol_area.x1 = obj->coords.x1 + left;
                symbol_area.x2 = symbol_area.x1 + symbol_w - 1;
            } else {
                symbol_area.x1 = obj->coords.x2 - right - symbol_w;
                symbol_area.x2 = symbol_area.x1 + symbol_w - 1;
            }

            if(symbol_type == LV_IMG_SRC_SYMBOL) {
                lv_draw_label(&symbol_area, clip_area, &label_dsc, dropdown->symbol, NULL);
            } else {
                lv_draw_img_dsc_t img_dsc;
                lv_draw_img_dsc_init(&img_dsc);
                lv_obj_init_draw_img_dsc(obj, LV_PART_MAIN, &img_dsc);
                img_dsc.pivot.x = symbol_w / 2;
                img_dsc.pivot.y = symbol_h / 2;
                img_dsc.angle = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
                lv_draw_img(&symbol_area, clip_area, dropdown->symbol, &img_dsc);
            }

            lv_point_t size;
            _lv_txt_get_size(&size, opt_txt, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX,
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
                } else {
                    txt_area.x1 = obj->coords.x1 + left;
                    txt_area.x2 = txt_area.x1 + size.x;
                }
            }
            lv_draw_label(&txt_area, clip_area, &label_dsc, opt_txt, NULL);
        }

        if(dropdown->text == NULL) {
            lv_mem_buf_release((char *)opt_txt);
        }

    }
    else if(mode == LV_DRAW_MODE_POST_DRAW) {
        lv_obj.draw_cb(obj, clip_area, mode);
    }

    return LV_DRAW_RES_OK;
}

/**
 * Handle the drawing related tasks of the drop down list's list
 * @param list pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DRAW_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DRAW_DRAW: draw the object (always return 'true')
 *             LV_DRAW_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_draw_res_t`
 */
static lv_draw_res_t lv_dropdown_list_draw(lv_obj_t * list_obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DRAW_MODE_COVER_CHECK) {
        return lv_obj.draw_cb(list_obj, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {
        lv_obj.draw_cb(list_obj, clip_area, mode);

        lv_dropdown_list_t * list = (lv_dropdown_list_t *)list_obj;
        lv_obj_t * dropdown_obj = list->dropdown;
        lv_dropdown_t * dropdown = (lv_dropdown_t *) dropdown_obj;

        /*Draw the boxes if the page is not being deleted*/
        if(dropdown->list) {
            /* Clip area might be too large too to shadow but
             * the selected option can be drawn on only the background*/
            lv_area_t clip_area_core;
            bool has_common;
            has_common = _lv_area_intersect(&clip_area_core, clip_area, &dropdown->list->coords);
            if(has_common) {
                if(dropdown->pr_opt_id != LV_DROPDOWN_PR_NONE) {
                    draw_box(dropdown_obj, &clip_area_core, dropdown->pr_opt_id, LV_STATE_PRESSED);
                }

                draw_box(dropdown_obj, &clip_area_core, dropdown->sel_opt_id, LV_STATE_DEFAULT);
            }
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DRAW_MODE_POST_DRAW) {
        lv_obj.draw_cb(list_obj, clip_area, mode);

        lv_dropdown_list_t * list = (lv_dropdown_list_t *)list_obj;
        lv_obj_t * dropdown_obj = list->dropdown;
        lv_dropdown_t * dropdown = (lv_dropdown_t *) dropdown_obj;

        /*Draw the box labels if the list is not being deleted*/
        if(dropdown->list) {
            /* Clip area might be too large too to shadow but
             * the selected option can be drawn on only the background*/
            lv_area_t clip_area_core;
            bool has_common;
            has_common = _lv_area_intersect(&clip_area_core, clip_area, &dropdown->list->coords);
            if(has_common) {
                if(dropdown->pr_opt_id != LV_DROPDOWN_PR_NONE) {
                    draw_box_label(dropdown_obj, &clip_area_core, dropdown->pr_opt_id, LV_STATE_PRESSED);
                }

                draw_box_label(dropdown_obj, &clip_area_core, dropdown->sel_opt_id, LV_STATE_DEFAULT);
            }
        }
    }

    return LV_DRAW_RES_OK;
}

static void lv_dropdown_list_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    lv_obj_construct_base(obj, parent, copy);

    lv_label_create(obj, NULL);
}

static void lv_dropdown_list_destructor(lv_obj_t * obj)
{

}

/**
 * Signal function of the drop down list
 * @param ddlist pointer to a drop down list object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_dropdown_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = lv_obj.signal_cb(obj, sign, param);
    if(res != LV_RES_OK) return res;

    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;

    if(sign == LV_SIGNAL_FOCUS) {
#if LV_USE_GROUP
        lv_group_t * g             = lv_obj_get_group(ddlist);
        bool editing               = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

        /*Encoders need special handling*/
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            /*Open the list if editing*/
            if(editing) lv_dropdown_open(ddlist);
            /*Close the list if navigating*/
            else
                lv_dropdown_close(ddlist);
        }
#endif
    }
    else if(sign == LV_SIGNAL_DEFOCUS || sign == LV_SIGNAL_LEAVE) {
        lv_dropdown_close(obj);
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        lv_indev_t * indev = lv_indev_get_act();
        if(lv_indev_get_scroll_obj(indev) == NULL) {
            if(!lv_obj_has_flag(dropdown->list, LV_OBJ_FLAG_HIDDEN)) {
                lv_dropdown_close(obj);
                if(dropdown->sel_opt_id_orig != dropdown->sel_opt_id) {
                    dropdown->sel_opt_id_orig = dropdown->sel_opt_id;
                    uint32_t id  = dropdown->sel_opt_id; /*Just to use uint32_t in event data*/
                    res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &id);
                    if(res != LV_RES_OK) return res;
                    lv_obj_invalidate(obj);
                }
#if LV_USE_GROUP
                lv_indev_type_t indev_type = lv_indev_get_type(indev);
                if(indev_type == LV_INDEV_TYPE_ENCODER) {
                    lv_group_set_editing(lv_obj_get_group(ddlist), false);
                }
#endif
            }
            else {
                lv_dropdown_open(obj);
            }
        }
        else {
            dropdown->sel_opt_id = dropdown->sel_opt_id_orig;
            lv_obj_invalidate(obj);
        }
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {
        if(dropdown->list) lv_dropdown_close(obj);
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_coord_t top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
        lv_coord_t bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_obj_set_height(obj, top + bottom + lv_font_get_line_height(font));
    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT || c == LV_KEY_DOWN) {
            if(dropdown->list == NULL) {
                lv_dropdown_open(ddlist);
            }
            else if(dropdown->sel_opt_id + 1 < dropdown->option_cnt) {
                dropdown->sel_opt_id++;
                position_to_selected(ddlist);
            }
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_UP) {

            if(dropdown->list == NULL) {
                lv_dropdown_open(ddlist);
            }
            else if(dropdown->sel_opt_id > 0) {
                dropdown->sel_opt_id--;
                position_to_selected(ddlist);
            }
        }
        else if(c == LV_KEY_ESC) {
            dropdown->sel_opt_id = dropdown->sel_opt_id_orig;
            lv_dropdown_close(ddlist);
        }
#endif
    }
    else if(sign == LV_SIGNAL_GET_EDITABLE) {
#if LV_USE_GROUP
        bool * editable = (bool *)param;
        *editable       = true;
#endif
    }

    return res;
}

/**
 * Signal function of the drop down list's scrollable part
 * @param scrl pointer to a drop down list's scrollable part
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_dropdown_list_signal(lv_obj_t * list, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = lv_obj.signal_cb(list, sign, param);
    if(res != LV_RES_OK) return res;

    lv_obj_t * dropdown_obj = ((lv_dropdown_list_t *)list)->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *) dropdown_obj;

    if(sign == LV_SIGNAL_RELEASED) {
        if(lv_indev_get_scroll_obj(lv_indev_get_act()) == NULL) {
            list_release_handler(list);
        }
    }
    else if(sign == LV_SIGNAL_PRESSED) {
        page_press_handler(list);
    }
    else if(sign == LV_SIGNAL_SCROLL_BEGIN) {
        dropdown->pr_opt_id = LV_DROPDOWN_PR_NONE;
        lv_obj_invalidate(list);
    }
    return res;
}

static void draw_box(lv_obj_t * dropdown_obj, const lv_area_t * clip_area, uint16_t id, lv_state_t state)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *) dropdown_obj;
    lv_obj_t * list_obj = dropdown->list;
    lv_state_t state_orig = list_obj->state;

    if(state != list_obj->state) {
        list_obj->state = state;
    }

    /*Draw a rectangle under the selected item*/
    const lv_font_t * font    = lv_obj_get_style_text_font(list_obj, LV_PART_HIGHLIGHT);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(list_obj,  LV_PART_HIGHLIGHT);
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
    lv_obj_init_draw_rect_dsc(list_obj,  LV_PART_HIGHLIGHT, &sel_rect);
    lv_draw_rect(&rect_area, clip_area, &sel_rect);

    list_obj->state = state_orig;
}

static void draw_box_label(lv_obj_t * dropdown_obj, const lv_area_t * clip_area, uint16_t id, lv_state_t state)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *) dropdown_obj;
    lv_obj_t * list_obj = dropdown->list;
    lv_state_t state_orig = list_obj->state;

    if(state != list_obj->state) {
        list_obj->state =  state;
    }

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(list_obj, LV_PART_HIGHLIGHT, &label_dsc);

    label_dsc.line_space = lv_obj_get_style_text_line_space(list_obj, LV_PART_HIGHLIGHT);  /*Line space should come from the list*/

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
    area_ok = _lv_area_intersect(&mask_sel, clip_area, &area_sel);
    if(area_ok) {
        lv_draw_label(&label->coords, &mask_sel, &label_dsc, lv_label_get_text(label), NULL);
    }
    list_obj->state = state_orig;
}

/**
 * Called when a drop down list is released to open it or set new option
 * @param list pointer to the drop down list's list
 * @return LV_RES_INV if the list is not being deleted in the user callback. Else LV_RES_OK
 */
static lv_res_t list_release_handler(lv_obj_t * list_obj)
{
    lv_dropdown_list_t * list = (lv_dropdown_list_t*) list_obj;
    lv_obj_t * dropdown_obj = list->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *) dropdown_obj;

    lv_indev_t * indev = lv_indev_get_act();
#if LV_USE_GROUP
    /*Leave edit mode once a new item is selected*/
    if(lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) {
        dropdown->sel_opt_id_orig = dropdown->sel_opt_id;
        lv_group_t * g      = lv_obj_get_group(ddlist);
        if(lv_group_get_editing(g)) {
            lv_group_set_editing(g, false);
        }
    }
#endif

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

static void page_press_handler(lv_obj_t * list_obj)
{
    lv_dropdown_list_t * list = (lv_dropdown_list_t*) list_obj;
    lv_obj_t * dropdown_obj = list->dropdown;
    lv_dropdown_t * dropdown = (lv_dropdown_t *) dropdown_obj;

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
    lv_dropdown_t * dropdown = (lv_dropdown_t *) dropdown_obj;
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
    lv_dropdown_t * dropdown = (lv_dropdown_t *) dropdown_obj;
    lv_obj_t * list_obj = dropdown->list;

    lv_obj_t * label = get_label(dropdown_obj);
    if(label == NULL) return;

    if(lv_obj_get_height(label) <= lv_obj_get_height_fit(dropdown_obj)) return;

    const lv_font_t * font         = lv_obj_get_style_text_font(label, LV_PART_MAIN);
    lv_coord_t font_h              = lv_font_get_line_height(font);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(label, LV_PART_MAIN);

    lv_coord_t line_y1 = dropdown->sel_opt_id * (font_h + line_space);

    /*Do not allow scrolling in*/
    lv_coord_t bottom_diff = dropdown->list->coords.y2 - lv_obj_get_style_pad_bottom(dropdown->list, LV_PART_MAIN) - (label->coords.y2 - line_y1);
    if(bottom_diff > 0) line_y1 -= bottom_diff;

    /*Scroll to the selected option*/
    lv_obj_scroll_to_y(dropdown->list, line_y1, LV_ANIM_OFF);
    lv_obj_invalidate(dropdown->list);
}

static lv_obj_t * get_label(const lv_obj_t * obj)
{
    lv_dropdown_t * dropdown = (lv_dropdown_t *) obj;
    if(dropdown->list == NULL) return NULL;

    return lv_obj_get_child(dropdown->list, 0);
}

#endif
