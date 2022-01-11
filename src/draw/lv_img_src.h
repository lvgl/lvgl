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
    const void   *  uri;            /**< A pointer on the given unique resource identifier */
    const char   *  ext;            /**< If the URI points to a file, this will point to the extension */
} lv_img_src_uri_t;


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
 * @deprecated This function has many limitations and is inefficient. Use lv_img_src_uri_file/data/symbol instead
 *              It forces to add a LVGL image header on raw encoded image data while there's already such header in the encoded data
 *              It prevents using LV_SYMBOL in the middle of some text, since it use the first byte of the data to figure out if it's a symbol or not
 *              Messy interface hiding the actual type, and requiring multiple deduction each time the source type is required
 */
lv_img_src_t lv_img_src_get_type(const void * src);

/** Build a source descriptor from the old void * format.
 *  @param uri  On output, will be filled with the decoded src
 *  @param src  The src format to parse
 *  @return LV_RES_OK if parsing was possible, LV_RES_INV else.
 *  @warning This does not mean that a source can be actually decoded, only that the parsing
 *           succeeded. The function will accept a lot of invalid data to be understood as a source, so
 *           beware of the output.
 *  @deprecated This function is deprecated. If your code is still using it, consider
 *              upgrading to the lv_img_src_uri_t format instead.
 */
lv_res_t lv_img_src_uri_parse(lv_img_src_uri_t * uri, const void * src);

/** Free a source descriptor.
 *  Only to be called if allocated via lv_img_src_uri_parse
 *  @param src  The src format to free
 */
void lv_img_src_uri_free(lv_img_src_uri_t * src);

/** Set the source of the descriptor to a text with any symbol in it
 *  @param src  The src descriptor to fill
 *  @param symbol An textual strings with symbols
*/
void lv_img_src_uri_symbol(lv_img_src_uri_t * obj, const char * symbol);
/** Set the source of the descriptor to a byte array containing the image encoded data
 *  @param src  The src descriptor to fill
 *  @param data A pointer to the image's data
 *  @param len  The length pointed by data in bytes
*/
void lv_img_src_uri_data(lv_img_src_uri_t * obj, const uint8_t * data, const size_t len);
/** Set the source of the descriptor to a file
 *  @param src  The src descriptor to fill
 *  @param path Path to the file containing the image
 */
void lv_img_src_uri_file(lv_img_src_uri_t * obj, const char * file_path);
/** Copy the source of the descriptor to another descriptor
 *  @param src  The src descriptor to fill
 *  @param path Path to the file containing the image
 */
void lv_img_src_uri_copy(lv_img_src_uri_t * dest, const lv_img_src_uri_t * src);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMG_DECODER_H*/
