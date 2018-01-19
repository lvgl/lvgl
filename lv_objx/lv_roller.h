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
#include "../../lv_conf.h"
#if USE_LV_ROLLER != 0

/*Testing of dependencies*/
#if USE_LV_DDLIST == 0
#error "lv_roller: lv_ddlist is required. Enable it in lv_conf.h (USE_LV_DDLIST  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_ddlist.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of roller*/
typedef struct {
    lv_ddlist_ext_t ddlist; /*Ext. of ancestor*/
    /*New data for this type */
}lv_roller_ext_t;

typedef enum {
    LV_ROLLER_STYLE_BG,
    LV_ROLLER_STYLE_SEL,
}lv_roller_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a roller object
 * @param par pointer to an object, it will be the parent of the new roller
 * @param copy pointer to a roller object, if not NULL then the new object will be copied from it
 * @return pointer to the created roller
 */
lv_obj_t * lv_roller_create(lv_obj_t * par, lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the options on a roller
 * @param roller pointer to roller object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
static inline void lv_roller_set_options(lv_obj_t * roller, const char * options)
{
    lv_ddlist_set_options(roller, options);
}

/**
 * Set the selected option
 * @param roller pointer to a roller object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 * @param anim_en true: set with animation; false set immediately
 */
void lv_roller_set_selected(lv_obj_t *roller, uint16_t sel_opt, bool anim_en);


/**
 * Set a function to call when a new option is chosen
 * @param roller pointer to a roller
 * @param action pointer to a callback function
 */
static inline void lv_roller_set_action(lv_obj_t * roller, lv_action_t action)
{
    lv_ddlist_set_action(roller, action);
}

/**
 * Set the height to show the given number of rows (options)
 * @param roller pointer to a roller object
 * @param row_cnt number of desired visible rows
 */
void lv_roller_set_visible_row_count(lv_obj_t *roller, uint8_t row_cnt);

/**
 * Enable or disable the horizontal fit to the content
 * @param roller pointer to a roller
 * @param fit en true: enable auto fit; false: disable auto fit
 */
static inline void lv_roller_set_hor_fit(lv_obj_t * roller, bool fit_en)
{
    lv_ddlist_set_hor_fit(roller, fit_en);
}

/**
 * Set the open/close animation time.
 * @param roller pointer to a roller object
 * @param anim_time: open/close animation time [ms]
 */
static inline void lv_roller_set_anim_time(lv_obj_t *roller, uint16_t anim_time)
{
    lv_ddlist_set_anim_time(roller, anim_time);
}


/**
 * Set a style of a roller
 * @param roller pointer to a roller object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_roller_set_style(lv_obj_t *roller, lv_roller_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the options of a roller
 * @param roller pointer to roller object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
static inline const char * lv_roller_get_options(lv_obj_t *roller)
{
    return lv_ddlist_get_options(roller);
}

/**
 * Get the id of the selected option
 * @param roller pointer to a roller object
 * @return id of the selected option (0 ... number of option - 1);
 */
static inline uint16_t lv_roller_get_selected(lv_obj_t *roller)
{
    return lv_ddlist_get_selected(roller);
}

/**
 * Get the current selected option as a string
 * @param roller pointer to roller object
 * @param buf pointer to an array to store the string
 */
static inline void lv_roller_get_selected_str(lv_obj_t * roller, char * buf)
{
    lv_ddlist_get_selected_str(roller, buf);
}

/**
 * Get the "option selected" callback function
 * @param roller pointer to a roller
 * @return  pointer to the call back function
 */
static inline lv_action_t lv_roller_get_action(lv_obj_t * roller)
{
    return lv_ddlist_get_action(roller);
}

/**
 * Get the open/close animation time.
 * @param roller pointer to a roller
 * @return open/close animation time [ms]
 */
static inline uint16_t lv_roller_get_anim_time(lv_obj_t * roller)
{
    return lv_ddlist_get_anim_time(roller);
}

/**
 * Get the auto width set attribute
 * @param roller pointer to a roller object
 * @return true: auto size enabled; false: manual width settings enabled
 */
bool lv_roller_get_hor_fit(lv_obj_t *roller);

/**
 * Get a style of a roller
 * @param roller pointer to a roller object
 * @param type which style should be get
 * @return style pointer to a style
 *  */
lv_style_t * lv_roller_get_style(lv_obj_t *roller, lv_roller_style_t type);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_ROLLER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_ROLLER_H*/
