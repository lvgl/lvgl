
/**
 * @file lv_rotary.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_rotary.h"
#if LV_USE_ROTARY != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_core/lv_group.h"
#include "../lv_core/lv_indev.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_math.h"
#include "lv_img.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_rotary"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_rotary_design(lv_obj_t * rotary, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_rotary_signal(lv_obj_t * rotary, lv_signal_t sign, void * param);
static lv_style_list_t * lv_rotary_get_style(lv_obj_t * rotary, uint8_t part);
// static void draw_bg(lv_obj_t * rotary, const lv_area_t * clip_area);
// static void draw_indic(lv_obj_t * rotary, const lv_area_t * clip_area);
static void draw_knob(lv_obj_t * rotary, const lv_area_t * clip_area);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_cb_t ancestor_design;
static lv_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a rotary objects
 * @param par pointer to an object, it will be the parent of the new rotary
 * @param copy pointer to a rotary object, if not NULL then the new object will be copied from it
 * @return pointer to the created rotary
 */
lv_obj_t * lv_rotary_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("rotary create started");

    /*Create the ancestor rotary*/
    lv_obj_t * rotary = lv_arc_create(par, copy);
    LV_ASSERT_MEM(rotary);
    if(rotary == NULL) return NULL;

    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(rotary);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(rotary);

    /*Allocate the rotary type specific extended data*/
    lv_rotary_ext_t * ext = lv_obj_allocate_ext_attr(rotary, sizeof(lv_rotary_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(rotary);
        return NULL;
    }

    /*Initialize the allocated 'ext' */
    ext->arc.rotation_angle = 0;
    ext->arc.bg_angle_start = 135;
    ext->arc.bg_angle_end = 45;
    ext->arc.arc_angle_start = 135;
    ext->arc.arc_angle_end = 270;
    ext->cur_value = 0;
    ext->min_value = 0;
    ext->max_value = 0;
    ext->dragging = false;
    ext->checkable = 0;
    lv_style_list_init(&ext->style_knob);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(rotary, lv_rotary_signal);
    lv_obj_set_design_cb(rotary, lv_rotary_design);

    /*Init the new rotary rotary*/
    if(copy == NULL) {
        lv_obj_set_click(rotary, true);
        lv_obj_add_protect(rotary, LV_PROTECT_PRESS_LOST);
        lv_obj_set_ext_click_area(rotary, 0, 0, LV_DPI / 10, LV_DPI / 10);

        lv_theme_apply(rotary, LV_THEME_ROTARY);
    }
    /*Copy an existing rotary*/
    else {
        lv_rotary_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->cur_value = copy_ext->cur_value;
        ext->min_value = copy_ext->min_value;
        ext->max_value = copy_ext->max_value;
        ext->dragging = copy_ext->dragging;
        ext->sym = copy_ext->sym;
        ext->checkable = copy_ext->checkable;
        lv_style_list_copy(&ext->style_knob, &copy_ext->style_knob);

        lv_obj_refresh_style(rotary, LV_OBJ_PART_ALL);
    }

    LV_LOG_INFO("rotary created");

    return rotary;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the rotary
 * @param rotary pointer to a rotary object
 * @param value new value
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_rotary_set_value(lv_obj_t * rotary, int16_t value, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(rotary, LV_OBJX_NAME);

    lv_rotary_ext_t * ext = (lv_rotary_ext_t *)lv_obj_get_ext_attr(rotary);
    if(ext->cur_value == value) return;

    int16_t new_value;
    new_value = value > ext->max_value ? ext->max_value : value;
    new_value = new_value < ext->min_value ? ext->min_value : new_value;

    if(ext->cur_value == new_value) return;
    
    ext->cur_value = new_value;

    lv_arc_set_end_angle(
        rotary,
        _lv_map(ext->cur_value, ext->min_value, ext->max_value, 
                ext->arc.arc_angle_start, 360 + ext->arc.bg_angle_end)
    );
}

/**
 * Set minimum and the maximum values of a rotary
 * @param rotary pointer to the rotary object
 * @param min minimum value
 * @param max maximum value
 */
void lv_rotary_set_range(lv_obj_t * rotary, int16_t min, int16_t max)
{
     LV_ASSERT_OBJ(rotary, LV_OBJX_NAME);

    lv_rotary_ext_t * ext = (lv_rotary_ext_t *)lv_obj_get_ext_attr(rotary);
    if(ext->min_value == min && ext->max_value == max) return;

    ext->min_value = min;
    ext->max_value = max;

    if(ext->cur_value < min) {
        ext->cur_value = min;
    }
    if(ext->cur_value > max) {
        ext->cur_value = max;
    }
    
    lv_rotary_set_value(rotary, ext->cur_value, false);
}

/**
 * Make the rotary symmetric to zero. The indicator will grow from zero instead of the minimum
 * position.
 * @param rotary pointer to a rotary object
 * @param en true: enable disable symmetric behavior; false: disable
 */
void lv_rotary_set_symmetric(lv_obj_t * rotary, bool en)
{
    LV_ASSERT_OBJ(rotary, LV_OBJX_NAME);

    lv_rotary_ext_t *ext = (lv_rotary_ext_t *)lv_obj_get_ext_attr(rotary);
    ext->sym = en;

    lv_obj_invalidate(rotary);
}

/**
 * Enable the toggled states
 * @param rotary pointer to a button object
 * @param tgl true: enable toggled states, false: disable
 */
void lv_rotary_set_checkable(lv_obj_t * rotary, bool tgl)
{
    LV_ASSERT_OBJ(rotary, LV_OBJX_NAME);

    lv_rotary_ext_t * ext = (lv_rotary_ext_t *)lv_obj_get_ext_attr(rotary);

    ext->checkable = tgl != false ? 1 : 0;
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a rotary
 * @param rotary pointer to a rotary object
 * @return the value of the rotary
 */
int16_t lv_rotary_get_value(const lv_obj_t * rotary)
{
    LV_ASSERT_OBJ(rotary, LV_OBJX_NAME);

    lv_rotary_ext_t * ext = lv_obj_get_ext_attr(rotary);
    return ext->cur_value;
}

/**
 * Get the minimum value of a rotary
 * @param rotary pointer to a rotary object
 * @return the minimum value of the rotary
 */
int16_t lv_rotary_get_min_value(const lv_obj_t * rotary)
{
    LV_ASSERT_OBJ(rotary, LV_OBJX_NAME);

    lv_rotary_ext_t *ext = (lv_rotary_ext_t *)lv_obj_get_ext_attr(rotary);
    return ext->min_value;
}

/**
 * Get the maximum value of a rotary
 * @param rotary pointer to a rotary object
 * @return the maximum value of the rotary
 */
int16_t lv_rotary_get_max_value(const lv_obj_t * rotary)
{
    LV_ASSERT_OBJ(rotary, LV_OBJX_NAME);

    lv_rotary_ext_t *ext = (lv_rotary_ext_t *)lv_obj_get_ext_attr(rotary);
    return ext->max_value;
}

/**
 * Give the rotary is being dragged or not
 * @param rotary pointer to a rotary object
 * @return true: drag in progress false: not dragged
 */
bool lv_rotary_is_dragged(const lv_obj_t * rotary)
{
    LV_ASSERT_OBJ(rotary, LV_OBJX_NAME);

    lv_rotary_ext_t * ext = lv_obj_get_ext_attr(rotary);
    return ext->dragging;
}

/**
 * Get the toggle enable attribute of the rotary
 * @param rotary pointer to a rotary object
 * @return true: toggle enabled, false: disabled
 */
bool lv_rotary_get_checkable(const lv_obj_t * rotary)
{
    LV_ASSERT_OBJ(rotary, LV_OBJX_NAME);

    lv_rotary_ext_t * ext = (lv_rotary_ext_t *)lv_obj_get_ext_attr(rotary);

    return ext->checkable != 0 ? true : false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the rotarys
 * @param rotary pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_rotary_design(lv_obj_t * rotary, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return LV_DESIGN_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        /*The ancestor design function will draw the background and the indicator.*/
        ancestor_design(rotary, clip_area, mode);
        draw_knob(rotary, clip_area);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        return ancestor_design(rotary, clip_area, mode);
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the rotary
 * @param rotary pointer to a rotary object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_rotary_signal(lv_obj_t * rotary, lv_signal_t sign, void * param)
{
    lv_res_t res;

    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_rotary_get_style(rotary, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(rotary, sign, param);
    }

    /* Include the ancient signal function */
    res = ancestor_signal(rotary, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    bool tgl = lv_rotary_get_checkable(rotary);
    lv_rotary_ext_t * ext = lv_obj_get_ext_attr(rotary);
    lv_point_t p;

    if(sign == LV_SIGNAL_PRESSED) {
        ext->dragging = true;
    }
    else if(sign == LV_SIGNAL_PRESSING && ext->value_to_set != NULL) {
        lv_indev_get_point(param, &p);

        // TODO: get new_value and set
    }
    else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        ext->dragging = false;
        ext->value_to_set = NULL;

        /*If not dragged and it was not long press action then
         *change state and run the action*/
        if(lv_indev_is_dragging(param) == false && tgl) {
            uint32_t toggled = 0;
            if(lv_obj_get_state(rotary, LV_ROTARY_PART_KNOB) & LV_STATE_CHECKED) {
                lv_btn_set_state(rotary, LV_ROTARY_STATE_RELEASED);
                toggled = 0;
            }
            else {
                lv_btn_set_state(rotary, LV_ROTARY_STATE_CHECKED_RELEASED);
                toggled = 1;
            }

            res = lv_event_send(rotary, LV_EVENT_VALUE_CHANGED, &toggled); /** non-NULL indicates toggle event*/
            if(res != LV_RES_OK) return res;
        }

#if LV_USE_GROUP
        /*Leave edit mode if released. (No need to wait for LONG_PRESS) */
        lv_group_t * g             = lv_obj_get_group(rotary);
        bool editing               = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            if(editing) lv_group_set_editing(g, false);
        }
#endif

    }
    else if(sign == LV_SIGNAL_CONTROL) {
        char c = *((char *)param);

        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            lv_rotary_set_value(rotary, lv_rotary_get_value(rotary) + 1, LV_ANIM_ON);
            res = lv_event_send(rotary, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            lv_rotary_set_value(rotary, lv_rotary_get_value(rotary) - 1, LV_ANIM_ON);
            res = lv_event_send(rotary, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_CLEANUP) {
        lv_obj_clean_style_list(rotary, LV_ROTARY_PART_KNOB);
    }
    else if(sign == LV_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = true;
    }

    return res;
}


static lv_style_list_t * lv_rotary_get_style(lv_obj_t * rotary, uint8_t part)
{
    LV_ASSERT_OBJ(rotary, LV_OBJX_NAME);

    lv_rotary_ext_t * ext = lv_obj_get_ext_attr(rotary);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_ROTARY_PART_KNOB:
            style_dsc_p = &ext->style_knob;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}

static void draw_knob(lv_obj_t * rotary, const lv_area_t * clip_area)
{
    lv_coord_t left_bg = lv_obj_get_style_pad_left(rotary, LV_ROTARY_PART_BG);
    lv_coord_t right_bg = lv_obj_get_style_pad_right(rotary, LV_ROTARY_PART_BG);
    lv_coord_t top_bg = lv_obj_get_style_pad_top(rotary, LV_ROTARY_PART_BG);
    lv_coord_t bottom_bg = lv_obj_get_style_pad_bottom(rotary, LV_ROTARY_PART_BG);
    lv_coord_t r = (LV_MATH_MIN(lv_obj_get_width(rotary) - left_bg - right_bg,
                                lv_obj_get_height(rotary) - top_bg - bottom_bg)) / 2;
    lv_coord_t indic_width = lv_obj_get_style_line_width(rotary, LV_ROTARY_PART_INDIC);
    lv_area_t knob_area;
    lv_draw_rect_dsc_t knob_rect_dsc;
    lv_coord_t center_x = rotary->coords.x1 + r + left_bg;
    lv_coord_t center_y = rotary->coords.y1 + r + top_bg;
    lv_coord_t knob_inner = lv_obj_get_style_pad_inner(rotary, LV_ROTARY_PART_KNOB);

    if(r > 0) {
        knob_area.x1 = center_x - r + indic_width + knob_inner;
        knob_area.x2 = center_x + r - indic_width - knob_inner;
        knob_area.y1 = center_y - r + indic_width + knob_inner;
        knob_area.y2 = center_y + r - indic_width - knob_inner;

        lv_draw_rect_dsc_init(&knob_rect_dsc);
        lv_obj_init_draw_rect_dsc(rotary, LV_ROTARY_PART_KNOB, &knob_rect_dsc);

        lv_draw_rect(&knob_area, clip_area, &knob_rect_dsc);
    }
}

#endif
