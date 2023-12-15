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
#include "../../../lvgl.h"

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
enum _lv_span_overflow_t {
    LV_SPAN_OVERFLOW_CLIP,
    LV_SPAN_OVERFLOW_ELLIPSIS,
    _LV_SPAN_OVERFLOW_LAST,  /**< Fence member*/
};

#ifdef DOXYGEN
typedef _lv_span_overflow_t lv_span_overflow_t;
#else
typedef uint32_t lv_span_overflow_t;
#endif /*DOXYGEN*/

enum _lv_span_mode_t {
    LV_SPAN_MODE_FIXED,     /**< fixed the obj size*/
    LV_SPAN_MODE_EXPAND,    /**< Expand the object size to the text size*/
    LV_SPAN_MODE_BREAK,     /**< Keep width, break the too long lines and expand height*/
    _LV_SPAN_MODE_LAST      /**< Fence member*/
};

#ifdef DOXYGEN
typedef _lv_span_mode_t lv_span_mode_t;
#else
typedef uint32_t lv_span_mode_t;
#endif /*DOXYGEN*/

typedef struct {
    char * txt;             /* a pointer to display text */
    lv_obj_t * spangroup;   /* a pointer to spangroup */
    lv_style_t style;       /* display text style */
    uint32_t static_flag : 1;/* the text is static flag */
} lv_span_t;

/** Data of label*/
typedef struct {
    lv_obj_t obj;
    int32_t lines;
    int32_t indent;      /* first line indent */
    int32_t cache_w;     /* the cache automatically calculates the width */
    int32_t cache_h;     /* similar cache_w */
    lv_ll_t  child_ll;
    uint32_t mode : 2;       /* details see lv_span_mode_t */
    uint32_t overflow : 1;   /* details see lv_span_overflow_t */
    uint32_t refresh : 1;    /* the spangroup need refresh cache_w and cache_h */
} lv_spangroup_t;

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
