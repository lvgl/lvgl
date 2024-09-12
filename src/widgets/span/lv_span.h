/**
 * @file lv_span.h
 *
 */

#ifndef LV_SPAN_H
#define LV_SPAN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"

#if LV_USE_SPAN != 0

/*********************
 *      DEFINES
 *********************/
#ifndef LV_SPAN_SNIPPET_STACK_SIZE
#define LV_SPAN_SNIPPET_STACK_SIZE 64
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_SPAN_OVERFLOW_CLIP,
    LV_SPAN_OVERFLOW_ELLIPSIS,
    LV_SPAN_OVERFLOW_LAST,  /**< Fence member*/
} lv_span_overflow_t;

typedef enum {
    LV_SPAN_MODE_FIXED,     /**< fixed the obj size */
    LV_SPAN_MODE_EXPAND,    /**< Expand the object size to the text size */
    LV_SPAN_MODE_BREAK,     /**< Keep width, break the too long lines and expand height */
    LV_SPAN_MODE_LAST       /**< Fence member */
} lv_span_mode_t;

/** Coords of a span */
typedef struct _lv_span_coords_t {
    lv_area_t heading;
    lv_area_t middle;
    lv_area_t trailing;
} lv_span_coords_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_spangroup_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_span_stack_init(void);
void lv_span_stack_deinit(void);

/**
 * Create a spangroup object
 * @param parent    pointer to an object, it will be the parent of the new spangroup
 * @return          pointer to the created spangroup
 */
lv_obj_t * lv_spangroup_create(lv_obj_t * parent);

/**
 * Create a span string descriptor and add to spangroup.
 * @param obj       pointer to a spangroup object.
 * @return          pointer to the created span.
 */
lv_span_t * lv_spangroup_new_span(lv_obj_t * obj);

/**
 * Remove the span from the spangroup and free memory.
 * @param obj   pointer to a spangroup object.
 * @param span  pointer to a span.
 */
void lv_spangroup_delete_span(lv_obj_t * obj, lv_span_t * span);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new text for a span. Memory will be allocated to store the text by the span.
 * @param span  pointer to a span.
 * @param text  pointer to a text.
 */
void lv_span_set_text(lv_span_t * span, const char * text);

/**
 * Set a static text. It will not be saved by the span so the 'text' variable
 * has to be 'alive' while the span exist.
 * @param span  pointer to a span.
 * @param text  pointer to a text.
 */
void lv_span_set_text_static(lv_span_t * span, const char * text);

/**
 * Set the align of the spangroup.
 * @param obj   pointer to a spangroup object.
 * @param align see lv_text_align_t for details.
 */
void lv_spangroup_set_align(lv_obj_t * obj, lv_text_align_t align);

/**
 * Set the overflow of the spangroup.
 * @param obj       pointer to a spangroup object.
 * @param overflow  see lv_span_overflow_t for details.
 */
void lv_spangroup_set_overflow(lv_obj_t * obj, lv_span_overflow_t overflow);

/**
 * Set the indent of the spangroup.
 * @param obj       pointer to a spangroup object.
 * @param indent    the first line indentation
 */
void lv_spangroup_set_indent(lv_obj_t * obj, int32_t indent);

/**
 * Set the mode of the spangroup.
 * @param obj       pointer to a spangroup object.
 * @param mode      see lv_span_mode_t for details.
 */
void lv_spangroup_set_mode(lv_obj_t * obj, lv_span_mode_t mode);

/**
 * Set maximum lines of the spangroup.
 * @param obj       pointer to a spangroup object.
 * @param lines     max lines that can be displayed in LV_SPAN_MODE_BREAK mode. < 0 means no limit.
 */
void lv_spangroup_set_max_lines(lv_obj_t * obj, int32_t lines);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get a pointer to the style of a span
 * @param span  pointer to the span
 * @return      pointer to the style. valid as long as the span is valid
*/
lv_style_t * lv_span_get_style(lv_span_t * span);

/**
 * Get a pointer to the text of a span
 * @param span  pointer to the span
 * @return      pointer to the text
*/
const char * lv_span_get_text(lv_span_t * span);

/**
 * Get a spangroup child by its index.
 *
 * @param obj   The spangroup object
 * @param id    the index of the child.
 *              0: the oldest (firstly created) child
 *              1: the second oldest
 *              child count-1: the youngest
 *              -1: the youngest
 *              -2: the second youngest
 * @return      The child span at index `id`, or NULL if the ID does not exist
 */
lv_span_t * lv_spangroup_get_child(const lv_obj_t * obj, int32_t id);

/**
 * Get number of spans
 * @param obj   the spangroup object to get the child count of.
 * @return      the span count of the spangroup.
 */
uint32_t lv_spangroup_get_span_count(const lv_obj_t * obj);

/**
 * Get the align of the spangroup.
 * @param obj   pointer to a spangroup object.
 * @return      the align value.
 */
lv_text_align_t lv_spangroup_get_align(lv_obj_t * obj);

/**
 * Get the overflow of the spangroup.
 * @param obj   pointer to a spangroup object.
 * @return      the overflow value.
 */
lv_span_overflow_t lv_spangroup_get_overflow(lv_obj_t * obj);

/**
 * Get the indent of the spangroup.
 * @param obj   pointer to a spangroup object.
 * @return      the indent value.
 */
int32_t lv_spangroup_get_indent(lv_obj_t * obj);

/**
 * Get the mode of the spangroup.
 * @param obj   pointer to a spangroup object.
 */
lv_span_mode_t lv_spangroup_get_mode(lv_obj_t * obj);

/**
 * Get maximum lines of the spangroup.
 * @param obj   pointer to a spangroup object.
 * @return      the max lines value.
 */
int32_t lv_spangroup_get_max_lines(lv_obj_t * obj);

/**
 * Get max line height of all span in the spangroup.
 * @param obj   pointer to a spangroup object.
 */
int32_t lv_spangroup_get_max_line_height(lv_obj_t * obj);

/**
 * Get the text content width when all span of spangroup on a line.
 * @param obj       pointer to a spangroup object.
 * @param max_width if text content width >= max_width, return max_width
 * to reduce computation, if max_width == 0, returns the text content width.
 * @return text     content width or max_width.
 */
uint32_t lv_spangroup_get_expand_width(lv_obj_t * obj, uint32_t max_width);

/**
 * Get the text content height with width fixed.
 * @param obj       pointer to a spangroup object.
 * @param width     the width of the span group.

 */
int32_t lv_spangroup_get_expand_height(lv_obj_t * obj, int32_t width);

/**
 * Get the span's coords in the spangroup.
 * @note Before calling this function, please make sure that the layout of span group has been updated.
 * Like calling lv_obj_update_layout() like function.
 *
 *     +--------+
 *     |Heading +--->------------------+
 *     |  Pos   |   |     Heading      |
 *     +--------+---+------------------+
 *     |                               |
 *     |                               |
 *     |                               |
 *     |            Middle   +--------+|
 *     |                     |Trailing||
 *     |                   +-|  Pos   ||
 *     |                   | +--------+|
 *     +-------------------v-----------+
 *     |     Trailing      |
 *     +-------------------+
 * @param obj       pointer to a spangroup object.
 * @param span      pointer to a span.
 * @return the span's coords in the spangroup.
 */
lv_span_coords_t lv_spangroup_get_span_coords(lv_obj_t * obj, const lv_span_t * span);

/**
 * Get the span object by point.
 * @param obj       pointer to a spangroup object.
 * @param point     pointer to point containing absolute coordinates
 * @return          pointer to the span under the point or `NULL` if not found.
 */
lv_span_t * lv_spangroup_get_span_by_point(lv_obj_t * obj, const lv_point_t * point);

/*=====================
 * Other functions
 *====================*/

/**
 * Update the mode of the spangroup.
 * @param obj   pointer to a spangroup object.
 */
void lv_spangroup_refr_mode(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SPAN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_SPAN_H*/
