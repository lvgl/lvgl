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
#include "lv_img_buf.h"
#include "../misc/lv_types.h"

/*********************
 *      MACROS
 *********************/

/* Useful macro to declare a symbol source */
#define LV_DECLARE_SYMBOL_SRC(symbol) \
    static lv_img_src_t symbol ## _src = { ._fixed_hdr = 0xFF, .type = LV_IMG_SRC_SYMBOL, .data_len = sizeof(symbol) - 1, .data = symbol, .ext = 0 }

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_img_dsc_t;

/**
 * Image source types.*/
enum {
    LV_IMG_SRC_UNKNOWN  = 0,    /** Unknown source*/
    LV_IMG_SRC_VARIABLE = 1,    /** Binary/C variable */
    LV_IMG_SRC_FILE     = 2,    /** File in filesystem*/
    LV_IMG_SRC_SYMBOL   = 3,    /** Symbol (@ref lv_symbol_def.h)*/
};

typedef uint8_t lv_img_src_type_t;



/**
 * Image source flags.*/
enum {
    /**
     * 0: can be attached to a single object which frees it if required
     * 1: can be attached to any number of object and none of them will try to free it
     */
    LV_IMG_SRC_FLAG_PERMANENT = 0x01,

    /**
     * only if loose == 1
     * 0: not attached to any objects yet
     * 1: attached to an object, can't be attached again
     */
    _LV_IMG_SRC_FLAG_CAPTURED = 0x02,

    /**
     * 1: The image source was created by lv_img_src_create() so it should be freed in lv_img_src_free
     */
    _LV_IMG_SRC_FLAG_MALLOCED = 0x04,
};

typedef uint8_t lv_img_src_flag_t;

/**
 * A generic image source descriptor.
 * You can build an image source via `lv_img_src_from_xxx` or `lv_img_src_set_src` functions.
 */
typedef struct {
    uint8_t         type: 5;          /**< See `lv_img_src_type_t` above */
    uint8_t         flag: 3;          /**< See `lv_img_src_flag_t` above */
    size_t          data_len;       /**< The data's length in bytes */
    const void   *  data;           /**< A pointer on the given unique resource identifier */
    const char   *  ext;            /**< If the data points to a file, this will point to the extension */
} lv_img_src_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'lv_img_t' variable (image stored internally and compiled into the code)
 *  - a path to a file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. LV_SYMBOL_CLOSE)
 * @return type of the image source LV_IMG_SRC_VARIABLE/FILE/SYMBOL/UNKNOWN
 * @deprecated This function has many limitations and is inefficient. Use lv_img_src_set_file/data/symbol instead
 *              It forces to add a LVGL image header on raw encoded image data while there's already such header in the encoded data
 *              It prevents using LV_SYMBOL in the middle of some text, since it use the first byte of the data to figure out if it's a symbol or not
 *              Messy interface hiding the actual type, and requiring multiple deduction each time the source type is required
 */
lv_img_src_type_t lv_img_src_get_type(const void * src);

/** Free a source descriptor.
 *  Only to be called if allocated via lv_img_src_parse
 *  @param src  The src format to free
 */
void lv_img_src_free(lv_img_src_t * src);

/** Set the source of the descriptor to a text with any symbol in it
 *  @param src  The src descriptor to fill
 *  @param symbol An textual strings with symbols
*/
void lv_img_src_set_symbol(lv_img_src_t * src, const char * symbol);

/** Set the source of the descriptor to a byte array containing the image encoded data
 *  @param src  The src descriptor to fill
 *  @param data A pointer to the image's data
 *  @param len  The length pointed by data in bytes
*/
void lv_img_src_set_data(lv_img_src_t * src, const uint8_t * data, const size_t len);

/** Set the source of the descriptor to a file
 *  @param src  The src descriptor to fill
 *  @param path Path to the file containing the image
 */
void lv_img_src_set_file(lv_img_src_t * src, const char * file_path);

/** Set the source of the descriptor to an image descriptor
 *  @param src  The src descriptor to fill
 *  @param raw  Pointer to a lv_img_dsc_t instance
 */
void lv_img_src_set_raw(lv_img_src_t * src, const lv_img_dsc_t * raw);

/** Get an image source to a text with any symbol in it
 *  @param symbol An textual strings with symbols
 *  @param flags    an element of @lv_img_src_flag_t. 0: to create an image source which can be simply attached to one image.
 *  @return a lv_img_src_move_t object instance that doesn't need to be free is used as argument to a LVGL function
*/
lv_img_src_t * lv_img_src_from_symbol(const char * symbol, lv_img_src_flag_t flags);

/** Get an image source to a byte array containing the image encoded data
 *  @param data A pointer to the image's data
 *  @param len  The length pointed by data in bytes
 *  @param flags    an element of @lv_img_src_flag_t. 0: to create an image source which can be simply attached to one image.
 *  @return a lv_img_src_move_t object instance that doesn't need to be free is used as argument to a LVGL function
*/
lv_img_src_t * lv_img_src_from_data(const uint8_t * data, const size_t len, lv_img_src_flag_t flags);

/**Get an image source to a file
 *  @param path Path to the file containing the image
 *  @param flags    an element of @lv_img_src_flag_t. 0: to create an image source which can be simply attached to one image.
 *  @return a lv_img_src_move_t object instance that doesn't need to be free is used as argument to a LVGL function
 */
lv_img_src_t * lv_img_src_from_file(const char * file_path, lv_img_src_flag_t flags);

/** Get an image source to an image descriptor
 *  @param raw  Pointer to a lv_img_dsc_t instance
 *  @param flags    an element of @lv_img_src_flag_t. 0: to create an image source which can be simply attached to one image.
 *  @return a lv_img_src_move_t object instance that doesn't need to be free is used as argument to a LVGL function
 */
lv_img_src_t * lv_img_src_from_raw(const lv_img_dsc_t * raw, lv_img_src_flag_t flags);

/** Get an image source to an empty object (no image)
 *  @param flags    an element of @lv_img_src_flag_t. 0: to create an image source which can be simply attached to one image.
 *  @return a lv_img_src_move_t object instance that doesn't need to be free is used as argument to a LVGL function
 */
lv_img_src_t * lv_img_src_create(lv_img_src_flag_t flags);

/** Copy the source of the descriptor to another descriptor
 *  @param dest The dest object to fill
 *  @param src  The source object to read from
 */
void lv_img_src_copy(lv_img_src_t * dest, const lv_img_src_t * src);

/** Capture  the source of the descriptor and move to another descriptor
 *  @param dest The dest object to fill
 *  @param src  The source object to move from
 */
void lv_img_src_capture(lv_img_src_t ** dest, lv_img_src_t * src);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMG_DECODER_H*/
