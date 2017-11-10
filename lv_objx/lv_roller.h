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
#include "lv_conf.h"
#if USE_LV_ROLLER != 0

#include "../lv_obj/lv_obj.h"
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
 * Set the open/close animation time.
 * @param roller pointer to a roller object
 * @param anim_time: open/close animation time [ms]
 */
static inline void lv_roller_set_anim_time(lv_obj_t *roller, uint16_t anim_time)
{
    lv_ddlist_set_anim_time(roller, anim_time);
}

/**
 * Set the style of a roller
 * @param roller pointer to a roller object
 * @param bg pointer to the new style of the background
 * @param sel pointer to the new style of the select rectangle
 */
static inline void lv_roller_set_style(lv_obj_t *roller, lv_style_t *bg, lv_style_t *sel)
{
    lv_ddlist_set_style(roller, bg, NULL, sel);
}
/**
 * Enable/disable to set the width of the roller manually (by lv_obj_Set_width())
 * @param roller pointer to a roller object
 * @param fit_en: true: enable auto size; false: use manual width settings
 */
void lv_roller_set_hor_fit(lv_obj_t *roller, bool fit_en);

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
    lv_roller_get_selected_str(roller, buf);
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
* Get the style of the roller's background
* @param roller pointer to a roller object
* @return pointer to the background's style
*/
static inline lv_style_t * lv_roller_get_style_bg(lv_obj_t *roller)
{
    return lv_ddlist_get_style_bg(roller);
}

/**
* Get the style of the roller's selected rectangle
* @param roller pointer to a roller object
* @return pointer to the selected rectangle's style
*/
static inline lv_style_t * lv_roller_get_style_selected(lv_obj_t *roller)
{
    return lv_ddlist_get_style_select(roller);
}

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_ROLLER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_ROLLER_H*/
