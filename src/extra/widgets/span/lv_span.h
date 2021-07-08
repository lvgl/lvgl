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
enum {
    LV_SPAN_OVERFLOW_CLIP,
    LV_SPAN_OVERFLOW_ELLIPSIS,
};
typedef uint8_t lv_span_overflow_t;

enum {
    LV_SPAN_MODE_FIXED,     /**< fixed the obj size*/
    LV_SPAN_MODE_EXPAND,    /**< Expand the object size to the text size*/
    LV_SPAN_MODE_BREAK,     /**< Keep width, break the too long lines and expand height*/
};
typedef uint8_t lv_span_mode_t;

typedef struct {
    char * txt;             /* a pointer to display text */
    lv_obj_t * spangroup;   /* a pointer to spangroup */
    lv_style_t style;       /* display text style */
    uint8_t static_flag : 1;/* the text is static flag */
} lv_span_t;

/** Data of label*/
typedef struct {
    lv_obj_t obj;
    lv_coord_t indent;      /* first line indent */
    lv_coord_t cache_w;     /* the cache automatically calculates the width */
    lv_coord_t cache_h;     /* similar cache_w */
    lv_ll_t  child_ll;
    uint8_t mode : 2;       /* details see lv_span_mode_t */
    uint8_t overflow : 1;   /* details see lv_span_overflow_t */
    uint8_t refresh : 1;    /* the spangroup need refresh cache_w and cache_h */
} lv_spangroup_t;

extern const lv_obj_class_t lv_spangroup_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a spangroup objects
 * @param par pointer to an object, it will be the parent of the new spangroup
 * @return pointer to the created spangroup
 */
lv_obj_t * lv_spangroup_create(lv_obj_t * par);

/**
 * Create a span string descriptor and add to spangroup.
 * @param obj pointer to a spangroup object.
 * @return pointer to the created span.
 */
lv_span_t * lv_spangroup_new_span(lv_obj_t * obj);

/**
 * Remove the span from the spangroup and free memory.
 * @param obj pointer to a spangroup object.
 * @param span pointer to a span.
 */
void lv_spangroup_del_span(lv_obj_t * obj, lv_span_t * span);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new text for a span. Memory will be allocated to store the text by the span.
 * @param span pointer to a span.
 * @param text pointer to a text.
 */
void lv_span_set_text(lv_span_t * span, const char * text);

/**
 * Set a static text. It will not be saved by the span so the 'text' variable
 * has to be 'alive' while the span exist.
 * @param span pointer to a span.
 * @param text pointer to a text.
 */
void lv_span_set_text_static(lv_span_t * span, const char * text);

/**
 * Set the align of the spangroup.
 * @param obj pointer to a spangroup object.
 * @param align see lv_text_align_t for details.
 */
void lv_spangroup_set_align(lv_obj_t * obj, lv_text_align_t align);

/**
 * Set the overflow of the spangroup.
 * @param obj pointer to a spangroup object.
 * @param overflow see lv_span_overflow_t for details.
 */
void lv_spangroup_set_overflow(lv_obj_t * obj, lv_span_overflow_t overflow);

/**
 * Set the indent of the spangroup.
 * @param obj pointer to a spangroup object.
 * @param indent The first line indentation
 */
void lv_spangroup_set_indent(lv_obj_t * obj, lv_coord_t indent);

/**
 * Set the mode of the spangroup.
 * @param obj pointer to a spangroup object.
 * @param mode see lv_span_mode_t for details.
 */
void lv_spangroup_set_mode(lv_obj_t * obj, lv_span_mode_t mode);

/*=====================
 * Getter functions
 *====================*/

/**
 * get the align of the spangroup.
 * @param obj pointer to a spangroup object.
 * @return the align value.
 */
lv_text_align_t lv_spangroup_get_align(lv_obj_t * obj);

/**
 * get the overflow of the spangroup.
 * @param obj pointer to a spangroup object.
 * @return the overflow value.
 */
lv_span_overflow_t lv_spangroup_get_overflow(lv_obj_t * obj);

/**
 * get the indent of the spangroup.
 * @param obj pointer to a spangroup object.
 * @return the indent value.
 */
lv_coord_t lv_spangroup_get_indent(lv_obj_t * obj);

/**
 * get the mode of the spangroup.
 * @param obj pointer to a spangroup object.
 */
lv_span_mode_t lv_spangroup_get_mode(lv_obj_t * obj);

/**
 * get max line height of all span in the spangroup.
 * @param obj pointer to a spangroup object.
 */
lv_coord_t lv_spangroup_get_max_line_h(lv_obj_t * obj);

/**
 * get the width when all span of spangroup on a line. include spangroup pad.
 * @param obj pointer to a spangroup object.
 */
lv_coord_t lv_spangroup_get_expand_width(lv_obj_t * obj);

/**
 * get the height with width fixed. the height include spangroup pad.
 * @param obj pointer to a spangroup object.
 */
lv_coord_t lv_spangroup_get_expand_height(lv_obj_t * obj, lv_coord_t width);


/*=====================
 * Other functions
 *====================*/

/**
 * update the mode of the spangroup.
 * @param obj pointer to a spangroup object.
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
