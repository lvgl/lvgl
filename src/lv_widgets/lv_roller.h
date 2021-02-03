/**
 * @file lv_roller.h
 *
 */

#ifndef LV_ROLLER_H
#define LV_ROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_ROLLER != 0

/*Testing of dependencies*/
#if LV_USE_PAGE == 0
#error "lv_roller: lv_page is required. Enable it in lv_conf.h (LV_USE_PAGE 1)"
#endif

#include "../lv_core/lv_obj.h"
#include "lv_page.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Roller mode. */
enum {
    LV_ROLLER_MODE_NORMAL, /**< Normal mode (roller ends at the end of the options). */
    LV_ROLLER_MODE_INFINITE, /**< Infinite mode (roller can be scrolled forever). */
};

typedef uint8_t lv_roller_mode_t;

/*Data of roller*/
typedef struct {
    lv_page_ext_t page; /*Ext. of ancestor*/

    /*New data for this type */
    lv_style_list_t style_sel; /*Style of the selected option*/
    uint16_t option_cnt;          /*Number of options*/
    uint16_t sel_opt_id;          /*Index of the current option*/
    uint16_t sel_opt_id_ori;      /*Store the original index on focus*/
    lv_roller_mode_t mode : 1;
    uint8_t auto_fit : 1;         /*1: Automatically set the width*/
} lv_roller_ext_t;

enum {
    LV_ROLLER_PART_BG = LV_PAGE_PART_BG,
    LV_ROLLER_PART_SELECTED = _LV_PAGE_PART_VIRTUAL_LAST,
    _LV_ROLLER_PART_VIRTUAL_LAST,
};
typedef uint8_t lv_roller_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a roller object
 * @param par pointer to an object, it will be the parent of the new roller
 * @param copy pointer to a roller object, if not NULL then the new object will be copied from it
 * @return pointer to the created roller
 */
lv_obj_t * lv_roller_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the options on a roller
 * @param roller pointer to roller object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 * @param mode `LV_ROLLER_MODE_NORMAL` or `LV_ROLLER_MODE_INFINITE`
 */
void lv_roller_set_options(lv_obj_t * roller, const char * options, lv_roller_mode_t mode);

/**
 * Set the align of the roller's options (left, right or center[default])
 * @param roller - pointer to a roller object
 * @param align - one of lv_label_align_t values (left, right, center)
 */
void lv_roller_set_align(lv_obj_t * roller, lv_label_align_t align);

/**
 * Set the selected option
 * @param roller pointer to a roller object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 * @param anim LV_ANIM_ON: set with animation; LV_ANIM_OFF set immediately
 */
void lv_roller_set_selected(lv_obj_t * roller, uint16_t sel_opt, lv_anim_enable_t anim);

/**
 * Set the height to show the given number of rows (options)
 * @param roller pointer to a roller object
 * @param row_cnt number of desired visible rows
 */
void lv_roller_set_visible_row_count(lv_obj_t * roller, uint8_t row_cnt);

/**
 * Allow automatically setting the width of roller according to it's content.
 * @param roller pointer to a roller object
 * @param auto_fit true: enable auto fit
 */
void lv_roller_set_auto_fit(lv_obj_t * roller, bool auto_fit);

/**
 * Set the open/close animation time.
 * @param roller pointer to a roller object
 * @param anim_time: open/close animation time [ms]
 */
static inline void lv_roller_set_anim_time(lv_obj_t * roller, uint16_t anim_time)
{
    lv_page_set_anim_time(roller, anim_time);
}

/*=====================
 * Getter functions
 *====================*/
/**
 * Get the id of the selected option
 * @param roller pointer to a roller object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_roller_get_selected(const lv_obj_t * roller);

/**
 * Get the total number of options
 * @param roller pointer to a roller object
 * @return the total number of options in the list
 */
uint16_t lv_roller_get_option_cnt(const lv_obj_t * roller);

/**
 * Get the current selected option as a string
 * @param roller pointer to roller object
 * @param buf pointer to an array to store the string
 * @param buf_size size of `buf` in bytes. 0: to ignore it.
 */
void lv_roller_get_selected_str(const lv_obj_t * roller, char * buf, uint32_t buf_size);

/**
 * Get the align attribute. Default alignment after _create is LV_LABEL_ALIGN_CENTER
 * @param roller pointer to a roller object
 * @return LV_LABEL_ALIGN_LEFT, LV_LABEL_ALIGN_RIGHT or LV_LABEL_ALIGN_CENTER
 */
lv_label_align_t lv_roller_get_align(const lv_obj_t * roller);

/**
 * Get whether the auto fit option is enabled or not.
 * @param roller pointer to a roller object
 * @return true: auto fit is enabled
 */
bool lv_roller_get_auto_fit(lv_obj_t * roller);

/**
 * Get the options of a roller
 * @param roller pointer to roller object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * lv_roller_get_options(const lv_obj_t * roller);

/**
 * Get the open/close animation time.
 * @param roller pointer to a roller
 * @return open/close animation time [ms]
 */
static inline uint16_t lv_roller_get_anim_time(const lv_obj_t * roller)
{
    return lv_page_get_anim_time(roller);
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ROLLER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_ROLLER_H*/
