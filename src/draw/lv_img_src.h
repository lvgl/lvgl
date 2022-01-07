/**
 * @file lv_img_src.h
 *
 */

#ifndef LV_IMG_SRC_H
#define LV_IMG_SRC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include <stdint.h>
#include <stddef.h>
#include "../misc/lv_types.h"


/**********************
 *      TYPEDEFS
 **********************/

/**
 * Source type of image.*/
enum {
    LV_IMG_SRC_UNKNOWN  = 0,    /** Unknown source*/
    LV_IMG_SRC_VARIABLE = 1,    /** Binary/C variable */
    LV_IMG_SRC_FILE     = 2,    /** File in filesystem*/
    LV_IMG_SRC_SYMBOL   = 3,    /** Symbol (@ref lv_symbol_def.h)*/
};

typedef uint8_t lv_img_src_t;

/**
 * A generic image source descriptor.
 * You can build an image source via `lv_imgsrc_set_xxx` functions.
 * If using the legacy API `lv_img_set_src`, LVGL will try to deduce the image source type from its content
 * but it's not reliable and waste some resources for binary source since it adds a
 * specific header to each image data (which might already have their own header)
 * Instead, this is straightforward to use */
typedef struct {
    size_t          type : 2;       /**< See `lv_img_src_t` above */
#if UINTPTR_MAX == 0xffffffffffffffff
    size_t          uri_len : 62;   /**< The next URI length in bytes */
#else
    size_t          uri_len : 30;   /**< The next URI length in bytes */
#endif
    const void *    uri;            /**< A pointer on the given unique resource identifier */
    const char*     ext;            /**< If the URI points to a file, this will point to the extension */
} lv_img_src_uri_t;


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMG_DECODER_H*/
