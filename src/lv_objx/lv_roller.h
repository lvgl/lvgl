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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#if LV_USE_ROLLER != 0

/*Testing of dependencies*/
#if LV_USE_DDLIST == 0
#error "lv_roller: lv_ddlist is required. Enable it in lv_conf.h (LV_USE_DDLIST  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_ddlist.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_ROLLER_MODE_NORMAL,
    LV_ROLLER_MODE_INIFINITE,
};

typedef uint8_t lv_roller_mode_t;

/*Data of roller*/
typedef struct
{
    lv_ddlist_ext_t ddlist; /*Ext. of ancestor*/
    /*New data for this type */
    lv_roller_mode_t mode : 1;
} lv_roller_ext_t;

enum {
    LV_ROLLER_STYLE_BG,
    LV_ROLLER_STYLE_SEL,
};
typedef uint8_t lv_roller_style_t;

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
 * @param anim LV_ANOM_ON: set with animation; LV_ANIM_OFF set immediately
 */
void lv_roller_set_selected(lv_obj_t * roller, uint16_t sel_opt, lv_anim_enable_t anim);

/**
 * Set the height to show the given number of rows (options)
 * @param roller pointer to a roller object
 * @param row_cnt number of desired visible rows
 */
void lv_roller_set_visible_row_count(lv_obj_t * roller, uint8_t row_cnt);

/**
 * Set a fix width for the drop down list
 * @param roller pointer to a roller obejct
 * @param w the width when the list is opened (0: auto size)
 */
static inline void lv_roller_set_fix_width(lv_obj_t * roller, lv_coord_t w)
{
    lv_ddlist_set_fix_width(roller, w);
}

/**
 * Set the open/close animation time.
 * @param roller pointer to a roller object
 * @param anim_time: open/close animation time [ms]
 */
static inline void lv_roller_set_anim_time(lv_obj_t * roller, uint16_t anim_time)
{
    lv_ddlist_set_anim_time(roller, anim_time);
}

/**
 * Set a style of a roller
 * @param roller pointer to a roller object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_roller_set_style(lv_obj_t * roller, lv_roller_style_t type, const lv_style_t * style);

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
 * Get the current selected option as a string
 * @param roller pointer to roller object
 * @param buf pointer to an array to store the string
 * @param buf_size size of `buf` in bytes. 0: to ignore it.
 */
static inline void lv_roller_get_selected_str(const lv_obj_t * roller, char * buf, uint16_t buf_size)
{
    lv_ddlist_get_selected_str(roller, buf, buf_size);
}

/**
 * Get the align attribute. Default alignment after _create is LV_LABEL_ALIGN_CENTER
 * @param roller pointer to a roller object
 * @return LV_LABEL_ALIGN_LEFT, LV_LABEL_ALIGN_RIGHT or LV_LABEL_ALIGN_CENTER
 */
lv_label_align_t lv_roller_get_align(const lv_obj_t * roller);

/**
 * Get the options of a roller
 * @param roller pointer to roller object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
static inline const char * lv_roller_get_options(const lv_obj_t * roller)
{
    return lv_ddlist_get_options(roller);
}

/**
 * Get the open/close animation time.
 * @param roller pointer to a roller
 * @return open/close animation time [ms]
 */
static inline uint16_t lv_roller_get_anim_time(const lv_obj_t * roller)
{
    return lv_ddlist_get_anim_time(roller);
}

/**
 * Get the auto width set attribute
 * @param roller pointer to a roller object
 * @return true: auto size enabled; false: manual width settings enabled
 */
bool lv_roller_get_hor_fit(const lv_obj_t * roller);

/**
 * Get a style of a roller
 * @param roller pointer to a roller object
 * @param type which style should be get
 * @return style pointer to a style
 *  */
const lv_style_t * lv_roller_get_style(const lv_obj_t * roller, lv_roller_style_t type);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ROLLER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_ROLLER_H*/
