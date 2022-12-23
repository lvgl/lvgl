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
    LV_IMG_SRC_ARRAY    = 4,    /** A pointer to some lv_img_src_t*/
};

typedef uint8_t lv_img_src_type_t;

/**
 * A generic image source descriptor.
 * You can build an image source via `lv_img_src_from_xxx` or `lv_img_src_set_src` functions.
 */
typedef struct {
    uint8_t         type;           /**< See `lv_img_src_type_t` above */
    size_t          data_len;       /**< The data's length in bytes */
    const void   *  data;           /**< A pointer on the given unique resource identifier */
    union {
        const char * ext;           /**< If the data points to a file, this will point to the extension */
        int         valid_index;    /**< When pointing to array, this is the valid index in the array */
    };
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

/** Get an image source to an empty object (no image)
 *  @return a pointer to a lv_img_src_t object instance that should be freed unless it's captured by a LVGL function
 */
lv_img_src_t * lv_img_src_create(void);

/** Free a source descriptor.
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

/** Set the source of the descriptor to an array of image source that might be only referred
 *  @param src   The src descriptor to fill
 *  @param srcs  A pointer to the image's source array
 *  @param len   The length of the array in elements
 *  @param index When less than len, store the image to use (source array is only referenced), else it's captured
*/
void lv_img_src_set_srcs(lv_img_src_t * src, lv_img_src_t * srcs[], const size_t len, const size_t index);


/** Get an image source from a text with any symbol in it
 *  @param symbol An textual strings with symbols
 *  @return a pointer to a lv_img_src_t object instance that should be freed unless it's captured by a LVGL function
*/
lv_img_src_t * lv_img_src_from_symbol(const char * symbol);

/** Get an image source from a byte array containing the image encoded data
 *  @param data A pointer to the image's data
 *  @param len  The length pointed by data in bytes
 *  @return a pointer to a lv_img_src_t object instance that should be freed unless it's captured by a LVGL function
*/
lv_img_src_t * lv_img_src_from_data(const uint8_t * data, const size_t len);

/**Get an image source from a file path
 *  @param path Path to the file containing the image
 *  @return a pointer to a lv_img_src_t object instance that should be freed unless it's captured by a LVGL function
 */
lv_img_src_t * lv_img_src_from_file(const char * file_path);

/** Get an image source from an image descriptor
 *  @param raw  Pointer to a lv_img_dsc_t instance
 *  @return a pointer to a lv_img_src_t object instance that should be freed unless it's captured by a LVGL function
 */
lv_img_src_t * lv_img_src_from_raw(const lv_img_dsc_t * raw);

/** Get an image source from an image source array.
 *  @param srcs A pointer to lv_img_src_t array that's captured
 *  @param len  The number of pointers in the array
 *  @return a pointer to a lv_img_src_t object instance that should be freed unless it's captured by a LVGL function
*/
lv_img_src_t * lv_img_src_from_srcs(lv_img_src_t * srcs[], const size_t len);

/** Get an image source from a reference to another image source
 *  @param src A pointer to lv_img_src_t that must persist as long as the returned instance is alive
 *  @return a pointer to a lv_img_src_t object instance that should be freed unless it's captured by a LVGL function
*/
lv_img_src_t * lv_img_src_from_src(lv_img_src_t * src);


/** Get an image source from a void *.
 *  @deprecated Don't use this since there are multiple bugs with type erasing and type guessing the image format.
 *  @param raw  Pointer to a void *
 *  @return a pointer to a lv_img_src_t object instance that should be freed unless it's captured by a LVGL function
 */
lv_img_src_t * lv_img_src_parse(const void * raw);




/** Copy the image source to another image source. The destination object is freed if required.
 *  @param dest The dest object to fill
 *  @param src  The source object to read from
 */
void lv_img_src_copy(lv_img_src_t * dest, const lv_img_src_t * src);

/** Allocate and copy an existing source */
lv_img_src_t * lv_img_src_dup(const lv_img_src_t * src);

/** Handful helper function to capture a given image source.
 *  @param dest If it exists, it's freed and then it's set to the source pointer
 *  @param src  The pointer to capture. The pointer shouldn't be freed after calling this method.
 */
void lv_img_src_capture(lv_img_src_t ** dest, lv_img_src_t * src);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMG_DECODER_H*/
