/**
 * @file lv_demo_smartwatch_settings.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_SETTINGS_H
#define LV_DEMO_SMARTWATCH_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"

#if LV_USE_DEMO_SMARTWATCH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create the settings page. Called only once.
 */
void lv_demo_smartwatch_settings_create(void);

/**
 * Load the settings screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_settings_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Add a slider to the main settings list
 * @param id        unique id to track changes (recommended > 0x000F)
 * @param name      title for the setting
 * @param img       icon for the setting
 * @param value     default value for the slider
 * @param min       min value for the slider
 * @param max       max value for the slider
 * @return pointer to slider object
 */
lv_obj_t * lv_demo_smartwatch_settings_add_slider(uint16_t id, const char * name, const lv_img_dsc_t * img,
                                                  int32_t value,
                                                  int32_t min, int32_t max);

/**
 * Add a toggle switch to the main settings list
 * @param id        unique id to track changes (recommended > 0x000F)
 * @param name      title for the setting
 * @param img       icon for the setting
 * @param state     default state for the switch
 * @return pointer to switch object
 */
lv_obj_t * lv_demo_smartwatch_settings_add_toggle(uint16_t id, const char * name, const lv_img_dsc_t * img, bool state);

/**
 * Add a dropdowwn to the main settings list
 * @param id        unique id to track changes (recommended > 0x000F)
 * @param name      title for the setting
 * @param img       icon for the setting
 * @param options   the dropdown options
 * @return pointer to dropdown object
 */
lv_obj_t * lv_demo_smartwatch_settings_add_dropdown(uint16_t id, const char * name, const lv_img_dsc_t * img,
                                                    const char * options);

/**
 * Add a label to the main settings list
 * @param id    unique id to track changes (recommended > 0x000F)
 * @param name  text for the label
 * @param img   icon for the label
 * @return pointer to label object
 */
lv_obj_t * lv_demo_smartwatch_settings_add_label(uint16_t id, const char * name, const lv_img_dsc_t * img);

/**
 * Set the callback function for settings actions
 * @param cb callback function
 */
void lv_demo_smartwatch_set_settings_actions_cb(lv_smartwatch_settings_change_cb_t cb);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_HOME_H*/
