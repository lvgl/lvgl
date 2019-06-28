/**
 * @file lv_kb.h
 *
 */

#ifndef LV_KB_H
#define LV_KB_H

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

#if LV_USE_KB != 0

/*Testing of dependencies*/
#if LV_USE_BTNM == 0
#error "lv_kb: lv_btnm is required. Enable it in lv_conf.h (LV_USE_BTNM  1) "
#endif

#if LV_USE_TA == 0
#error "lv_kb: lv_ta is required. Enable it in lv_conf.h (LV_USE_TA  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_btnm.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Current keyboard mode. */
enum {
    LV_KB_MODE_TEXT,
    LV_KB_MODE_NUM,
};
typedef uint8_t lv_kb_mode_t;

/*Data of keyboard*/
typedef struct
{
    lv_btnm_ext_t btnm; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * ta;          /*Pointer to the assigned text area*/
    lv_kb_mode_t mode;      /*Key map type*/
    uint8_t cursor_mng : 1; /*1: automatically show/hide cursor when a text area is assigned or left*/
} lv_kb_ext_t;

enum {
    LV_KB_STYLE_BG,
    LV_KB_STYLE_BTN_REL,
    LV_KB_STYLE_BTN_PR,
    LV_KB_STYLE_BTN_TGL_REL,
    LV_KB_STYLE_BTN_TGL_PR,
    LV_KB_STYLE_BTN_INA,
};
typedef uint8_t lv_kb_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a keyboard objects
 * @param par pointer to an object, it will be the parent of the new keyboard
 * @param copy pointer to a keyboard object, if not NULL then the new object will be copied from it
 * @return pointer to the created keyboard
 */
lv_obj_t * lv_kb_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @param ta pointer to a Text Area object to write there
 */
void lv_kb_set_ta(lv_obj_t * kb, lv_obj_t * ta);

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @param mode the mode from 'lv_kb_mode_t'
 */
void lv_kb_set_mode(lv_obj_t * kb, lv_kb_mode_t mode);

/**
 * Automatically hide or show the cursor of the current Text Area
 * @param kb pointer to a Keyboard object
 * @param en true: show cursor on the current text area, false: hide cursor
 */
void lv_kb_set_cursor_manage(lv_obj_t * kb, bool en);

/**
 * Set a new map for the keyboard
 * @param kb pointer to a Keyboard object
 * @param map pointer to a string array to describe the map.
 *            See 'lv_btnm_set_map()' for more info.
 */
static inline void lv_kb_set_map(lv_obj_t * kb, const char * map[])
{
    lv_btnm_set_map(kb, map);
}

/**
 * Set the button control map (hidden, disabled etc.) for the keyboard. The
 * control map array will be copied and so may be deallocated after this
 * function returns.
 * @param kb pointer to a keyboard object
 * @param ctrl_map pointer to an array of `lv_btn_ctrl_t` control bytes.
 *                 See: `lv_btnm_set_ctrl_map` for more details.
 */
static inline void lv_kb_set_ctrl_map(lv_obj_t * kb, const lv_btnm_ctrl_t ctrl_map[])
{
    lv_btnm_set_ctrl_map(kb, ctrl_map);
}

/**
 * Set a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_kb_set_style(lv_obj_t * kb, lv_kb_style_t type, const lv_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @return pointer to the assigned Text Area object
 */
lv_obj_t * lv_kb_get_ta(const lv_obj_t * kb);

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'lv_kb_mode_t'
 */
lv_kb_mode_t lv_kb_get_mode(const lv_obj_t * kb);

/**
 * Get the current cursor manage mode.
 * @param kb pointer to a Keyboard object
 * @return true: show cursor on the current text area, false: hide cursor
 */
bool lv_kb_get_cursor_manage(const lv_obj_t * kb);

/**
 * Get the current map of a keyboard
 * @param kb pointer to a keyboard object
 * @return the current map
 */
static inline const char ** lv_kb_get_map_array(const lv_obj_t * kb)
{
    return lv_btnm_get_map_array(kb);
}

/**
 * Get a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_kb_get_style(const lv_obj_t * kb, lv_kb_style_t type);

/*=====================
 * Other functions
 *====================*/

/**
 * Default keyboard event to add characters to the Text area and change the map.
 * If a custom `event_cb` is added to the keyboard this function be called from it to handle the
 * button clicks
 * @param kb pointer to a  keyboard
 * @param event the triggering event
 */
void lv_kb_def_event_cb(lv_obj_t * kb, lv_event_t event);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_KB*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_KB_H*/
