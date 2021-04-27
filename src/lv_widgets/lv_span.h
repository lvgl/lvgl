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
#include "../lv_conf_internal.h"

#if LV_USE_SPAN != 0

#include <stdarg.h>
#include "../lv_core/lv_obj.h"
#include "../lv_font/lv_font.h"
#include "../lv_font/lv_symbol_def.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define LV_SPAN_SNIPPET_STACK_SIZE 64

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_SPAN_ALIGN_LEFT,    /**< Align text to left */
    LV_SPAN_ALIGN_CENTER,  /**< Align text to center */
    LV_SPAN_ALIGN_RIGHT,   /**< Align text to right */
    LV_SPAN_ALIGN_AUTO,    /**< Use LEFT or RIGHT depending on the direction of the text (LTR/RTL)*/
};
typedef uint8_t lv_span_align_t;

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
    char * txt;
    lv_style_t style;
} lv_span_t;

/** Data of label*/
typedef struct {
    lv_coord_t indent;
    lv_ll_t  child_ll;
    uint8_t mode : 2;
    uint8_t align : 2;
    uint8_t overflow : 1;
} lv_span_ext_t;

/** Label styles*/
enum {
    LV_SPAN_PART_MAIN,
};
typedef uint8_t lv_span_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a spangroup objects
 * @param par pointer to an object, it will be the parent of the new spangroup
 * @param copy pointer to a spangroup object, if not NULL then the new object will be copied from it
 * @return pointer to the created spangroup
 */
lv_obj_t * lv_spangroup_create(lv_obj_t * par, const lv_obj_t * copy);

/**
 * Create a span string descriptor and add to spangroup.
 * @param spans pointer to a spangroup object.
 * @param text '\0' terminated character string.
 * @return pointer to the created span.
 */
lv_span_t * lv_span_create(lv_obj_t * spans, const char * text);

/**
 * Remove the span from the spangroup and free memory.
 * @param spans pointer to a spangroup object.
 * @param span pointer to a span.
 */
void lv_span_del(lv_obj_t * spans, lv_span_t * span);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the align of the spangroup.
 * @param spans pointer to a spangroup object.
 * @param align see lv_span_align_t for details.
 */
void lv_span_set_align(lv_obj_t * spans, lv_span_align_t align);

/**
 * Set the overflow of the spangroup.
 * @param spans pointer to a spangroup object.
 * @param overflow see lv_span_overflow_t for details.
 */
void lv_span_set_overflow(lv_obj_t * spans, lv_span_overflow_t overflow);

/**
 * Set the indent of the spangroup.
 * @param spans pointer to a spangroup object.
 * @param indent The first line indentation
 */
void lv_span_set_indent(lv_obj_t * spans, lv_coord_t indent);

/**
 * Set the mode of the spangroup.
 * @param spans pointer to a spangroup object.
 * @param mode see lv_span_mode_t for details.
 */
void lv_span_set_mode(lv_obj_t * spans, lv_span_mode_t mode);

/*=====================
 * Getter functions
 *====================*/

/**
 * get the align of the spangroup.
 * @param spans pointer to a spangroup object.
 * @return the align value.
 */
lv_span_align_t lv_span_get_align(lv_obj_t * spans);

/**
 * get the overflow of the spangroup.
 * @param spans pointer to a spangroup object.
 * @return the overflow value.
 */
lv_span_overflow_t lv_span_get_overflow(lv_obj_t * spans);

/**
 * get the indent of the spangroup.
 * @param spans pointer to a spangroup object.
 * @return the indent value.
 */
lv_coord_t lv_span_get_indent(lv_obj_t * spans);

/**
 * get the mode of the spangroup.
 * @param spans pointer to a spangroup object.
 */
lv_span_mode_t lv_span_get_mode(lv_obj_t * spans);

/**
 * get max line height of all span in the spangroup.
 * @param spans pointer to a spangroup object.
 */
lv_coord_t lv_span_get_max_line_h(lv_obj_t * spans);

/**
 * get the width when all span of spangroup on a line. include spangroup pad.
 * @param spans pointer to a spangroup object.
 */
lv_coord_t lv_span_get_expend_width(lv_obj_t * spans);

/**
 * get the height with width fixed. the height include spangroup pad.
 * @param spans pointer to a spangroup object.
 */
lv_coord_t lv_span_get_expend_height(lv_obj_t * spans, lv_coord_t width);


/*=====================
 * Other functions
 *====================*/

/**
 * update the mode of the spangroup.
 * @param spans pointer to a spangroup object.
 */
void lv_span_refr_mode(lv_obj_t * spans);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SPAN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_SPAN_H*/
