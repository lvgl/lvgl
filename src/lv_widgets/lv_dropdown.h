/**
 * @file lv_ddlist.h
 *
 */

#ifndef LV_DROPDOWN_H
#define LV_DROPDOWN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_DROPDOWN != 0

/*Testing of dependencies*/
#if LV_USE_PAGE == 0
#error "lv_ddlist: lv_page is required. Enable it in lv_conf.h (LV_USE_PAGE 1)"
#endif

#if LV_USE_LABEL == 0
#error "lv_ddlist: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL 1)"
#endif

#include "../lv_widgets/lv_page.h"
#include "../lv_widgets/lv_label.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DROPDOWN_POS_LAST 0xFFFF

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_DROPDOWN_DIR_DOWN,
    LV_DROPDOWN_DIR_UP,
    LV_DROPDOWN_DIR_LEFT,
    LV_DROPDOWN_DIR_RIGHT,
};

typedef uint8_t lv_dropdown_dir_t;

/*Data of drop down list*/
typedef struct {
    /*New data for this type */
    lv_obj_t * page;             /*The dropped down list*/
    const char * text;           /*Text to display on the ddlist's button*/
    const char * symbol;         /*Arrow or other icon when the drop-down list is closed*/
    char * options;
    lv_style_list_t style_selected; /*Style of the selected option*/
    lv_style_list_t style_page;     /*Style of the dropped down list*/
    lv_style_list_t style_scrlbar; /*Style of the scroll bar*/
    lv_coord_t max_height;        /*Height of the ddlist when opened. (0: auto-size)*/
    uint16_t option_cnt;          /*Number of options*/
    uint16_t sel_opt_id;          /*Index of the currently selected option*/
    uint16_t sel_opt_id_orig;     /*Store the original index on focus*/
    uint16_t pr_opt_id;             /*Index of the currently pressed option*/
    lv_dropdown_dir_t dir         : 2;
    uint8_t show_selected  : 1;
    uint8_t static_txt : 1;
} lv_dropdown_ext_t;

enum {
    LV_DROPDOWN_PART_MAIN = LV_OBJ_PART_MAIN,
    LV_DROPDOWN_PART_LIST = _LV_OBJ_PART_REAL_LAST,
    LV_DROPDOWN_PART_SCROLLBAR,
    LV_DROPDOWN_PART_SELECTED,
};
typedef uint8_t lv_dropdown_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
 * Create a drop down list objects
 * @param par pointer to an object, it will be the parent of the new drop down list
 * @param copy pointer to a drop down list object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created drop down list
 */
lv_obj_t * lv_dropdown_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set text of the ddlist (Displayed on the button if `show_selected = false`)
 * @param ddlist pointer to a drop down list object
 * @param txt the text as a string (Only it's pointer is saved)
 */
void lv_dropdown_set_text(lv_obj_t * ddlist, const char * txt);

/**
 * Clear any options in a drop down list.  Static or dynamic.
 * @param ddlist pointer to drop down list object
 */
void lv_dropdown_clear_options(lv_obj_t * ddlist);

/**
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 * The options string can be destroyed after calling this function
 */
void lv_dropdown_set_options(lv_obj_t * ddlist, const char * options);

/**
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a static string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void lv_dropdown_set_options_static(lv_obj_t * ddlist, const char * options);

/**
 * Add an options to a drop down list from a string.  Only works for dynamic options.
 * @param ddlist pointer to drop down list object
 * @param option a string without '\n'. E.g. "Four"
 * @param pos the insert position, indexed from 0, LV_DROPDOWN_POS_LAST = end of string
 */
void lv_dropdown_add_option(lv_obj_t * ddlist, const char * option, uint32_t pos);

/**
 * Set the selected option
 * @param ddlist pointer to drop down list object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 */
void lv_dropdown_set_selected(lv_obj_t * ddlist, uint16_t sel_opt);

/**
 * Set the direction of the a drop down list
 * @param ddlist pointer to a drop down list object
 * @param dir LV_DROPDOWN_DIR_LEF/RIGHT/TOP/BOTTOM
 */
void lv_dropdown_set_dir(lv_obj_t * ddlist, lv_dropdown_dir_t dir);

/**
 * Set the maximal height for the drop down list
 * @param ddlist pointer to a drop down list
 * @param h the maximal height
 */
void lv_dropdown_set_max_height(lv_obj_t * ddlist, lv_coord_t h);

/**
 * Set an arrow or other symbol to display when the drop-down list is closed.
 * @param ddlist pointer to drop down list object
 * @param symbol a text like `LV_SYMBOL_DOWN` or NULL to not draw icon
 */
void lv_dropdown_set_symbol(lv_obj_t * ddlist, const char * symbol);

/**
 * Set whether the ddlist highlight the last selected option and display its text or not
 * @param ddlist pointer to a drop down list object
 * @param show true/false
 */
void lv_dropdown_set_show_selected(lv_obj_t * ddlist, bool show);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get text of the ddlist (Displayed on the button if `show_selected = false`)
 * @param ddlist pointer to a drop down list object
 * @return the text string
 */
const char * lv_dropdown_get_text(lv_obj_t * ddlist);

/**
 * Get the options of a drop down list
 * @param ddlist pointer to drop down list object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * lv_dropdown_get_options(const lv_obj_t * ddlist);

/**
 * Get the selected option
 * @param ddlist pointer to drop down list object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_dropdown_get_selected(const lv_obj_t * ddlist);

/**
 * Get the total number of options
 * @param ddlist pointer to drop down list object
 * @return the total number of options in the list
 */
uint16_t lv_dropdown_get_option_cnt(const lv_obj_t * ddlist);

/**
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 * @param buf_size size of `buf` in bytes. 0: to ignore it.
 */
void lv_dropdown_get_selected_str(const lv_obj_t * ddlist, char * buf, uint32_t buf_size);

/**
 * Get the fix height value.
 * @param ddlist pointer to a drop down list object
 * @return the height if the ddlist is opened (0: auto size)
 */
lv_coord_t lv_dropdown_get_max_height(const lv_obj_t * ddlist);

/**
 * Get the symbol to draw when the drop-down list is closed
 * @param ddlist pointer to drop down list object
 * @return the symbol or NULL if not enabled
 */
const char * lv_dropdown_get_symbol(lv_obj_t * ddlist);

/**
 * Get the symbol to draw when the drop-down list is closed
 * @param ddlist pointer to drop down list object
 * @return the symbol or NULL if not enabled
 */
lv_dropdown_dir_t lv_dropdown_get_dir(const lv_obj_t * ddlist);

/**
 * Get whether the ddlist highlight the last selected option and display its text or not
 * @param ddlist pointer to a drop down list object
 * @return true/false
 */
bool lv_dropdown_get_show_selected(lv_obj_t * ddlist);

/*=====================
 * Other functions
 *====================*/

/**
 * Open the drop down list with or without animation
 * @param ddlist pointer to drop down list object
 */
void lv_dropdown_open(lv_obj_t * ddlist);

/**
 * Close (Collapse) the drop down list
 * @param ddlist pointer to drop down list object
 * @param anim_en LV_ANIM_ON: use animation; LV_ANOM_OFF: not use animations
 */
void lv_dropdown_close(lv_obj_t * ddlist);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DROPDOWN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DROPDOWN_H*/
