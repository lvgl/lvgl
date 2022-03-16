/**
 * @file lv_btnmatrix.h
 *
 */

#ifndef LV_BTNMATRIX_H
#define LV_BTNMATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_BTNMATRIX != 0

#include "../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/
#define LV_BTNMATRIX_BTN_NONE 0xFFFF
LV_EXPORT_CONST_INT(LV_BTNMATRIX_BTN_NONE);

/**********************
 *      TYPEDEFS
 **********************/

/** Type to store button control bits (disabled, hidden etc.)
 * The first 3 bits are used to store the width*/
enum {
    _LV_BTNMATRIX_WIDTH     = 0x0007,      /**< Reserved to stire the size units*/
    LV_BTNMATRIX_CTRL_HIDDEN     = 0x0008, /**< Button hidden*/
    LV_BTNMATRIX_CTRL_NO_REPEAT  = 0x0010, /**< Do not repeat press this button.*/
    LV_BTNMATRIX_CTRL_DISABLED   = 0x0020, /**< Disable this button.*/
    LV_BTNMATRIX_CTRL_CHECKABLE  = 0x0040, /**< The button can be toggled.*/
    LV_BTNMATRIX_CTRL_CHECKED    = 0x0080, /**< Button is currently toggled (e.g. checked).*/
    LV_BTNMATRIX_CTRL_CLICK_TRIG = 0x0100, /**< 1: Send LV_EVENT_VALUE_CHANGE on CLICK, 0: Send LV_EVENT_VALUE_CHANGE on PRESS*/
    LV_BTNMATRIX_CTRL_POPOVER    = 0x0200, /**< Show a popover when pressing this key*/
    LV_BTNMATRIX_CTRL_RECOLOR    = 0x1000, /**< Enable text recoloring with `#color`*/
    _LV_BTNMATRIX_CTRL_RESERVED  = 0x2000, /**< Reserved for later use*/
    LV_BTNMATRIX_CTRL_CUSTOM_1   = 0x4000, /**< Custom free to use flag*/
    LV_BTNMATRIX_CTRL_CUSTOM_2   = 0x8000, /**< Custom free to use flag*/
};

typedef uint16_t lv_btnmatrix_ctrl_t;

typedef bool (*lv_btnmatrix_btn_draw_cb_t)(lv_obj_t * btnm, uint32_t btn_id, const lv_area_t * draw_area,
                                           const lv_area_t * clip_area);

/*Data of button matrix*/
typedef struct {
    lv_obj_t obj;
    const char ** map_p;                              /*Pointer to the current map*/
    lv_area_t * button_areas;                         /*Array of areas of buttons*/
    lv_btnmatrix_ctrl_t * ctrl_bits;                       /*Array of control bytes*/
    uint16_t btn_cnt;                                 /*Number of button in 'map_p'(Handled by the library)*/
    uint16_t row_cnt;                                 /*Number of rows in 'map_p'(Handled by the library)*/
    uint16_t btn_id_sel;    /*Index of the active button (being pressed/released etc) or LV_BTNMATRIX_BTN_NONE*/
    uint8_t one_check : 1;  /*Single button toggled at once*/
} lv_btnmatrix_t;

extern const lv_obj_class_t lv_btnmatrix_class;

/**
 * `type` field in `lv_obj_draw_part_dsc_t` if `class_p = lv_btnmatrix_class`
 * Used in `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END`
 */
typedef enum {
    LV_BTNMATRIX_DRAW_PART_BTN,    /**< The rectangle and label of buttons*/
} lv_btnmatrix_draw_part_type_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a button matrix object
 * @param parent    pointer to an object, it will be the parent of the new button matrix
 * @return          pointer to the created button matrix
 */
lv_obj_t * lv_btnmatrix_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new map. Buttons will be created/deleted according to the map. The
 * button matrix keeps a reference to the map and so the string array must not
 * be deallocated during the life of the matrix.
 * @param obj       pointer to a button matrix object
 * @param map       pointer a string array. The last string has to be: "". Use "\n" to make a line break.
 */
void lv_btnmatrix_set_map(lv_obj_t * obj, const char * map[]);

/**
 * Set the button control map (hidden, disabled etc.) for a button matrix.
 * The control map array will be copied and so may be deallocated after this
 * function returns.
 * @param obj       pointer to a button matrix object
 * @param ctrl_map  pointer to an array of `lv_btn_ctrl_t` control bytes. The
 *                  length of the array and position of the elements must match
 *                  the number and order of the individual buttons (i.e. excludes
 *                  newline entries).
 *                  An element of the map should look like e.g.:
 *                 `ctrl_map[0] = width | LV_BTNMATRIX_CTRL_NO_REPEAT |  LV_BTNMATRIX_CTRL_TGL_ENABLE`
 */
void lv_btnmatrix_set_ctrl_map(lv_obj_t * obj, const lv_btnmatrix_ctrl_t ctrl_map[]);

/**
 * Set the selected buttons
 * @param obj        pointer to button matrix object
 * @param btn_id         0 based index of the button to modify. (Not counting new lines)
 */
void lv_btnmatrix_set_selected_btn(lv_obj_t * obj, uint16_t btn_id);

/**
 * Set the attributes of a button of the button matrix
 * @param obj       pointer to button matrix object
 * @param btn_id    0 based index of the button to modify. (Not counting new lines)
 * @param ctrl      OR-ed attributs. E.g. `LV_BTNMATRIX_CTRL_NO_REPEAT | LV_BTNMATRIX_CTRL_CHECKABLE`
 */
void lv_btnmatrix_set_btn_ctrl(lv_obj_t * obj, uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl);

/**
 * Clear the attributes of a button of the button matrix
 * @param obj       pointer to button matrix object
 * @param btn_id    0 based index of the button to modify. (Not counting new lines)
 * @param ctrl      OR-ed attributs. E.g. `LV_BTNMATRIX_CTRL_NO_REPEAT | LV_BTNMATRIX_CTRL_CHECKABLE`
 */
void lv_btnmatrix_clear_btn_ctrl(lv_obj_t * obj, uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl);

/**
 * Set attributes of all buttons of a button matrix
 * @param obj       pointer to a button matrix object
 * @param ctrl      attribute(s) to set from `lv_btnmatrix_ctrl_t`. Values can be ORed.
 */
void lv_btnmatrix_set_btn_ctrl_all(lv_obj_t * obj, lv_btnmatrix_ctrl_t ctrl);

/**
 * Clear the attributes of all buttons of a button matrix
 * @param obj       pointer to a button matrix object
 * @param ctrl      attribute(s) to set from `lv_btnmatrix_ctrl_t`. Values can be ORed.
 * @param en        true: set the attributes; false: clear the attributes
 */
void lv_btnmatrix_clear_btn_ctrl_all(lv_obj_t * obj, lv_btnmatrix_ctrl_t ctrl);

/**
 * Set a single button's relative width.
 * This method will cause the matrix be regenerated and is a relatively
 * expensive operation. It is recommended that initial width be specified using
 * `lv_btnmatrix_set_ctrl_map` and this method only be used for dynamic changes.
 * @param obj       pointer to button matrix object
 * @param btn_id    0 based index of the button to modify.
 * @param width     relative width compared to the buttons in the same row. [1..7]
 */
void lv_btnmatrix_set_btn_width(lv_obj_t * obj, uint16_t btn_id, uint8_t width);

/**
 * Make the button matrix like a selector widget (only one button may be checked at a time).
 * `LV_BTNMATRIX_CTRL_CHECKABLE` must be enabled on the buttons to be selected using
 * `lv_btnmatrix_set_ctrl()` or `lv_btnmatrix_set_btn_ctrl_all()`.
 * @param obj       pointer to a button matrix object
 * @param en        whether "one check" mode is enabled
 */
void lv_btnmatrix_set_one_checked(lv_obj_t * obj, bool en);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current map of a button matrix
 * @param obj       pointer to a button matrix object
 * @return          the current map
 */
const char ** lv_btnmatrix_get_map(const lv_obj_t * obj);

/**
 * Get the index of the lastly "activated" button by the user (pressed, released, focused etc)
 * Useful in the `event_cb` to get the text of the button, check if hidden etc.
 * @param obj       pointer to button matrix object
 * @return          index of the last released button (LV_BTNMATRIX_BTN_NONE: if unset)
 */
uint16_t lv_btnmatrix_get_selected_btn(const lv_obj_t * obj);

/**
 * Get the button's text
 * @param obj       pointer to button matrix object
 * @param btn_id    the index a button not counting new line characters.
 * @return          text of btn_index` button
 */
const char * lv_btnmatrix_get_btn_text(const lv_obj_t * obj, uint16_t btn_id);

/**
 * Get the whether a control value is enabled or disabled for button of a button matrix
 * @param obj       pointer to a button matrix object
 * @param btn_id    the index of a button not counting new line characters.
 * @param ctrl      control values to check (ORed value can be used)
 * @return          true: the control attribute is enabled false: disabled
 */
bool lv_btnmatrix_has_btn_ctrl(lv_obj_t * obj, uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl);

/**
 * Tell whether "one check" mode is enabled or not.
 * @param obj       Button matrix object
 * @return          true: "one check" mode is enabled; false: disabled
 */
bool lv_btnmatrix_get_one_checked(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BTNMATRIX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BTNMATRIX_H*/
