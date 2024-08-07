/**
 * @file lv_scale.h
 *
 */

#ifndef LV_SCALE_H
#define LV_SCALE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_SCALE != 0

#include "../../core/lv_obj.h"
#include "../line/lv_line.h"
#include "../image/lv_image.h"

/*********************
 *      DEFINES
 *********************/

/**Default value of total minor ticks. */
#define LV_SCALE_TOTAL_TICK_COUNT_DEFAULT (11U)
LV_EXPORT_CONST_INT(LV_SCALE_TOTAL_TICK_COUNT_DEFAULT);

/**Default value of major tick every nth ticks. */
#define LV_SCALE_MAJOR_TICK_EVERY_DEFAULT (5U)
LV_EXPORT_CONST_INT(LV_SCALE_MAJOR_TICK_EVERY_DEFAULT);

/**Default value of scale label enabled. */
#define LV_SCALE_LABEL_ENABLED_DEFAULT (1U)
LV_EXPORT_CONST_INT(LV_SCALE_LABEL_ENABLED_DEFAULT);

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Scale mode
 */
typedef enum {
    LV_SCALE_MODE_HORIZONTAL_TOP    = 0x00U,
    LV_SCALE_MODE_HORIZONTAL_BOTTOM = 0x01U,
    LV_SCALE_MODE_VERTICAL_LEFT     = 0x02U,
    LV_SCALE_MODE_VERTICAL_RIGHT    = 0x04U,
    LV_SCALE_MODE_ROUND_INNER       = 0x08U,
    LV_SCALE_MODE_ROUND_OUTER      = 0x10U,
    LV_SCALE_MODE_LAST
} lv_scale_mode_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_scale_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an scale object
 * @param parent    pointer to an object, it will be the parent of the new scale
 * @return          pointer to the created scale
 */
lv_obj_t * lv_scale_create(lv_obj_t * parent);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set scale mode. See lv_scale_mode_t
 * @param obj       pointer the scale object
 * @param mode      the new scale mode
 */
void lv_scale_set_mode(lv_obj_t * obj, lv_scale_mode_t mode);

/**
 * Set scale total tick count (including minor and major ticks)
 * @param obj       pointer the scale object
 * @param total_tick_count    New total tick count
 */
void lv_scale_set_total_tick_count(lv_obj_t * obj, uint32_t total_tick_count);

/**
 * Sets how often the major tick will be drawn
 * @param obj                 pointer the scale object
 * @param major_tick_every    the new count for major tick drawing
 */
void lv_scale_set_major_tick_every(lv_obj_t * obj, uint32_t major_tick_every);

/**
 * Sets label visibility
 * @param obj           pointer the scale object
 * @param show_label    true/false to enable tick label
 */
void lv_scale_set_label_show(lv_obj_t * obj, bool show_label);

/**
 * Set the minimal and maximal values on a scale
 * @param obj       pointer to a scale object
 * @param min       minimum value of the scale
 * @param max       maximum value of the scale
 */
void lv_scale_set_range(lv_obj_t * obj, int32_t min, int32_t max);

/**
 * Set properties specific to round scale
 * @param obj           pointer to a scale object
 * @param angle_range   the angular range of the scale
 */
void lv_scale_set_angle_range(lv_obj_t * obj, uint32_t angle_range);

/**
 * Set properties specific to round scale
 * @param obj       pointer to a scale object
 * @param rotation  the angular offset from the 3 o'clock position (clock-wise)
 */
void lv_scale_set_rotation(lv_obj_t * obj, int32_t rotation);

/**
 * Point the needle to the corresponding value through the line
 * @param obj              pointer to a scale object
 * @param needle_line      needle_line of the scale. The line points will be allocated and
 *                         managed by the scale unless the line point array was previously set
 *                         using `lv_line_set_points_mutable`.
 * @param needle_length    length of the needle
 *                         needle_length>0 needle_length=needle_length;
 *                         needle_length<0 needle_length=radius-|needle_length|;
 * @param value            needle to point to the corresponding value
 */
void lv_scale_set_line_needle_value(lv_obj_t * obj, lv_obj_t * needle_line, int32_t needle_length,
                                    int32_t value);

/**
 * Point the needle to the corresponding value through the image,
   image must point to the right. E.g. -O------>
 * @param obj              pointer to a scale object
 * @param needle_img       needle_img of the scale
 * @param value            needle to point to the corresponding value
 */
void lv_scale_set_image_needle_value(lv_obj_t * obj, lv_obj_t * needle_img, int32_t value);

/**
 * Set custom text source for major ticks labels
 * @param obj       pointer to a scale object
 * @param txt_src   pointer to an array of strings which will be display at major ticks
 */
void lv_scale_set_text_src(lv_obj_t * obj, const char * txt_src[]);

/**
 * Draw the scale after all the children are drawn
 * @param obj       pointer to a scale object
 * @param en        true: enable post draw
 */
void lv_scale_set_post_draw(lv_obj_t * obj, bool en);

/**
 * Draw the scale ticks on top of all parts
 * @param obj       pointer to a scale object
 * @param en        true: enable draw ticks on top of all parts
 */
void lv_scale_set_draw_ticks_on_top(lv_obj_t * obj, bool en);

/**
 * Add a section to the given scale
 * @param obj       pointer to a scale object
 * @return          pointer to the new section
 */
lv_scale_section_t * lv_scale_add_section(lv_obj_t * obj);

/**
 * Set the range for the given scale section
 * @param section       pointer to a scale section object
 * @param minor_range   section new minor range
 * @param major_range   section new major range
 */
void lv_scale_section_set_range(lv_scale_section_t * section, int32_t minor_range, int32_t major_range);

/**
 * Set the style of the part for the given scale section
 * @param section   pointer to a scale section object
 * @param part      the part for the section, e.g. LV_PART_INDICATOR
 * @param section_part_style Pointer to the section part style
 */
void lv_scale_section_set_style(lv_scale_section_t * section, lv_part_t part, lv_style_t * section_part_style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get scale mode. See lv_scale_mode_t
 * @param obj   pointer the scale object
 * @return      Scale mode
 */
lv_scale_mode_t lv_scale_get_mode(lv_obj_t * obj);

/**
 * Get scale total tick count (including minor and major ticks)
 * @param obj   pointer the scale object
 * @return      Scale total tick count
 */
int32_t lv_scale_get_total_tick_count(lv_obj_t * obj);

/**
 * Gets how often the major tick will be drawn
 * @param obj   pointer the scale object
 * @return      Scale major tick every count
 */
int32_t lv_scale_get_major_tick_every(lv_obj_t * obj);

/**
 * Gets label visibility
 * @param obj   pointer the scale object
 * @return      true if tick label is enabled, false otherwise
 */
bool lv_scale_get_label_show(lv_obj_t * obj);

/**
 * Get angle range of a round scale
 * @param obj   pointer to a scale object
 * @return      Scale angle_range
 */
uint32_t lv_scale_get_angle_range(lv_obj_t * obj);

/**
 * Get the min range for the given scale section
 * @param obj   pointer to a scale section object
 * @return      section minor range
 */
int32_t lv_scale_get_range_min_value(lv_obj_t * obj);

/**
 * Get the max range for the given scale section
 * @param obj   pointer to a scale section object
 * @return      section max range
 */
int32_t lv_scale_get_range_max_value(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SCALE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SCALE_H*/
