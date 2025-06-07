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

#define LV_SCALE_LABEL_ROTATE_MATCH_TICKS  0x100000
LV_EXPORT_CONST_INT(LV_SCALE_LABEL_ROTATE_MATCH_TICKS);

#define LV_SCALE_LABEL_ROTATE_KEEP_UPRIGHT 0x80000
LV_EXPORT_CONST_INT(LV_SCALE_LABEL_ROTATE_KEEP_UPRIGHT);

#define LV_SCALE_ROTATION_ANGLE_MASK 0x7FFFF
LV_EXPORT_CONST_INT(LV_SCALE_ROTATION_ANGLE_MASK);

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_scale_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an scale object
 * @param parent    pointer to an object, it will be the parent of the new scale
 * @return          pointer to created Scale Widget
 */
lv_obj_t * lv_scale_create(lv_obj_t * parent);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set scale mode. See lv_scale_mode_t.
 * @param obj       pointer to Scale Widget
 * @param mode      the new scale mode
 */
void lv_scale_set_mode(lv_obj_t * obj, lv_scale_mode_t mode);

/**
 * Set scale total tick count (including minor and major ticks).
 * @param obj       pointer to Scale Widget
 * @param total_tick_count    New total tick count
 */
void lv_scale_set_total_tick_count(lv_obj_t * obj, uint32_t total_tick_count);

/**
 * Sets how often major ticks are drawn.
 * @param obj                 pointer to Scale Widget
 * @param major_tick_every    the new count for major tick drawing
 */
void lv_scale_set_major_tick_every(lv_obj_t * obj, uint32_t major_tick_every);

/**
 * Sets label visibility.
 * @param obj           pointer to Scale Widget
 * @param show_label    true/false to enable tick label
 */
void lv_scale_set_label_show(lv_obj_t * obj, bool show_label);

/**
 * Set minimum and maximum values on Scale.
 * @param obj       pointer to Scale Widget
 * @param min       minimum value of Scale
 * @param max       maximum value of Scale
 */
void lv_scale_set_range(lv_obj_t * obj, int32_t min, int32_t max);

/**
 * Set minimum values on Scale.
 * @param obj       pointer to Scale Widget
 * @param min       minimum value of Scale
 */
void lv_scale_set_min_value(lv_obj_t * obj, int32_t min);

/**
 * Set maximum values on Scale.
 * @param obj       pointer to Scale Widget
 * @param min       minimum value of Scale
 */
void lv_scale_set_max_value(lv_obj_t * obj, int32_t max);

/**
 * Set angle between the low end and the high end of the Scale.
 * (Applies only to round Scales.)
 * @param obj         pointer to Scale Widget
 * @param max_angle   angle in degrees from Scale minimum where top end of Scale will be drawn
 */
void lv_scale_set_angle_range(lv_obj_t * obj, uint32_t angle_range);

/**
 * Set angular offset from the 3-o'clock position of the low end of the Scale.
 * (Applies only to round Scales.)
 * @param obj       pointer to Scale Widget
 * @param rotation  clockwise angular offset (in degrees) from the 3-o'clock position
 *                  of the low end of the scale; negative and >360 values are first normalized
 *                  to range [0..360].
 *                  Examples:
 *                      -   0 = 3 o'clock (right side)
 *                      -  30 = 4 o'clock
 *                      -  60 = 5 o'clock
 *                      -  90 = 6 o'clock
 *                      - 135 = midway between 7 and 8 o'clock (default)
 *                      - 180 = 9 o'clock
 *                      - 270 = 12 o'clock
 *                      - 300 = 1 o'clock
 *                      - 330 = 2 o'clock
 *                      - -30 = 2 o'clock
 *                      - 390 = 4 o'clock
 */
void lv_scale_set_rotation(lv_obj_t * obj, int32_t rotation);

/**
 * Point line needle to specified value.
 * @param obj              pointer to Scale Widget
 * @param needle_line      needle_line of the Scale. The line points will be allocated and
 *                         managed by the Scale unless the line point array was previously set
 *                         using `lv_line_set_points_mutable`.
 * @param needle_length    length of the needle
 *                         - needle_length>0: needle_length=needle_length;
 *                         - needle_length<0: needle_length=radius-|needle_length|;
 * @param value            Scale value needle will point to
 */
void lv_scale_set_line_needle_value(lv_obj_t * obj, lv_obj_t * needle_line, int32_t needle_length,
                                    int32_t value);

/**
 * Point image needle to specified value;
   image must point to the right. E.g. -O------>
 * @param obj              pointer to Scale Widget
 * @param needle_img       pointer to needle's Image
 * @param value            Scale value needle will point to
 */
void lv_scale_set_image_needle_value(lv_obj_t * obj, lv_obj_t * needle_img, int32_t value);

/**
 * Set custom text source for major ticks labels.
 * @param obj       pointer to Scale Widget
 * @param txt_src   pointer to an array of strings which will be display at major ticks;
 *                  last element must be a NULL pointer.
 */
void lv_scale_set_text_src(lv_obj_t * obj, const char * txt_src[]);

/**
 * Draw Scale after all its children are drawn.
 * @param obj       pointer to Scale Widget
 * @param en        true: enable post draw
 */
void lv_scale_set_post_draw(lv_obj_t * obj, bool en);

/**
 * Draw Scale ticks on top of all other parts.
 * @param obj       pointer to Scale Widget
 * @param en        true: enable draw ticks on top of all parts
 */
void lv_scale_set_draw_ticks_on_top(lv_obj_t * obj, bool en);

/**
 * Add a Section to specified Scale.  Section will not be drawn until
 * a valid range is set for it using `lv_scale_set_section_range()`.
 * @param obj       pointer to Scale Widget
 * @return          pointer to new Section
 */
lv_scale_section_t * lv_scale_add_section(lv_obj_t * obj);

/**
 * DEPRECATED, use lv_scale_set_section_range instead.
 * Set range for specified Scale Section
 * @param section       pointer to Section
 * @param range_min     Section new minimum value
 * @param range_max     Section new maximum value
 */
void lv_scale_section_set_range(lv_scale_section_t * section, int32_t min, int32_t max);

/**
 * Set the range of a scale section
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param range_min     the section's new minimum value
 * @param range_max     the section's new maximum value
 */
void lv_scale_set_section_range(lv_obj_t * scale, lv_scale_section_t * section, int32_t min, int32_t max);

/**
 * Set the minimum value of a scale section
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param min           the section's new minimum value
 */
void lv_scale_set_section_min_value(lv_obj_t * scale, lv_scale_section_t * section, int32_t min);

/**
 * Set the maximum value of a scale section
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param max           the section's new maximum value
 */
void lv_scale_set_section_max_value(lv_obj_t * scale, lv_scale_section_t * section, int32_t max);

/**
 * DEPRECATED, use lv_scale_set_section_style_main/indicator/items instead.
 * Set style for specified part of Section.
 * @param section             pointer to Section
 * @param part                the part of the Scale the style will apply to, e.g. LV_PART_INDICATOR
 * @param section_part_style  pointer to style to apply
 */
void lv_scale_section_set_style(lv_scale_section_t * section, lv_part_t part, lv_style_t * section_part_style);

/**
 * Set the style of the line on a section.
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param style         point to a style
 */
void lv_scale_set_section_style_main(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style);

/**
 * Set the style of the major ticks and label on a section.
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param style         point to a style
 */
void lv_scale_set_section_style_indicator(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style);

/**
 * Set the style of the minor ticks on a section.
 * @param scale         pointer to scale
 * @param section       pointer to section
 * @param style         point to a style
 */
void lv_scale_set_section_style_items(lv_obj_t * scale, lv_scale_section_t * section, const lv_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get scale mode. See lv_scale_mode_t
 * @param obj   pointer to Scale Widget
 * @return      Scale mode
 */
lv_scale_mode_t lv_scale_get_mode(lv_obj_t * obj);

/**
 * Get scale total tick count (including minor and major ticks)
 * @param obj   pointer to Scale Widget
 * @return      Scale total tick count
 */
int32_t lv_scale_get_total_tick_count(lv_obj_t * obj);

/**
 * Get how often the major tick will be drawn
 * @param obj   pointer to Scale Widget
 * @return      Scale major tick every count
 */
int32_t lv_scale_get_major_tick_every(lv_obj_t * obj);

/**
 * Get angular location of low end of Scale.
 * @param obj   pointer to Scale Widget
 * @return      Scale low end anglular location
 */
int32_t lv_scale_get_rotation(lv_obj_t * obj);

/**
 * Gets label visibility
 * @param obj   pointer to Scale Widget
 * @return      true if tick label is enabled, false otherwise
 */
bool lv_scale_get_label_show(lv_obj_t * obj);

/**
 * Get Scale's range in degrees
 * @param obj   pointer to Scale Widget
 * @return      Scale's angle_range
 */
uint32_t lv_scale_get_angle_range(lv_obj_t * obj);

/**
 * Get minimum value for Scale
 * @param obj   pointer to Scale Widget
 * @return      Scale's minimum value
 */
int32_t lv_scale_get_range_min_value(lv_obj_t * obj);

/**
 * Get maximum value for Scale
 * @param obj   pointer to Scale Widget
 * @return      Scale's maximum value
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
