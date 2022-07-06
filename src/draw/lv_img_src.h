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
 * Source type of image.*/
enum {
    LV_IMG_SRC_UNKNOWN  = 0,    /** Unknown source*/
    LV_IMG_SRC_VARIABLE = 1,    /** Binary/C variable */
    LV_IMG_SRC_FILE     = 2,    /** File in filesystem*/
    LV_IMG_SRC_SYMBOL   = 3,    /** Symbol (@ref lv_symbol_def.h)*/


    _LV_IMG_SRC_MOVABLE = 0x80, /** A bit flag to tell if an object is a temporary and that can be captured */
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
    const char   *  ext;            /**< If the data points to a file, this will point to the extension */
} lv_img_src_t;

/**
 * When using `lv_img_src_from_xxx` function, you're creating a temporary that must be freed with
 * `lv_img_src_free`. This is costly because for most usage, the temporary object will immediately be
 * copied to an internal member of a larger object.
 *
 * So in order to avoid this, `lv_img_src_from_xxx` functions mark an object as movable and if moved,
 * there's no need to call `lv_img_src_free` on the instance anymore.
 * To avoid confusion with the base object, this structure is used in the API where movable semantic
 * is expected.
 */
typedef struct {
    lv_img_src_t   _src;
} lv_img_src_move_t;

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

/** Build a source descriptor from the old void * format.
 *  @param obj  On output, will be filled with the decoded src
 *  @param src  The src format to parse
 *  @return LV_RES_OK if parsing was possible, LV_RES_INV else.
 *  @warning This does not mean that a source can be actually decoded, only that the parsing
 *           succeeded. The function will accept a lot of invalid data to be understood as a source, so
 *           beware of the output.
 *  @deprecated This function is deprecated. If your code is still using it, consider
 *              upgrading to the lv_img_src_t format instead.
 */
lv_res_t lv_img_src_parse(lv_img_src_t * obj, const void * src);

/** Free a source descriptor.
 *  Only to be called if allocated via lv_img_src_parse
 *  @param src  The src format to free
 */
void lv_img_src_free(lv_img_src_t * src);

/** Set the source of the descriptor to a text with any symbol in it
 *  @param src  The src descriptor to fill
 *  @param symbol An textual strings with symbols
*/
void lv_img_src_set_symbol(lv_img_src_t * obj, const char * symbol);
/** Set the source of the descriptor to a byte array containing the image encoded data
 *  @param src  The src descriptor to fill
 *  @param data A pointer to the image's data
 *  @param len  The length pointed by data in bytes
*/
void lv_img_src_set_data(lv_img_src_t * obj, const uint8_t * data, const size_t len);
/** Set the source of the descriptor to a file
 *  @param src  The src descriptor to fill
 *  @param path Path to the file containing the image
 */
void lv_img_src_set_file(lv_img_src_t * obj, const char * file_path);
/** Set the source of the descriptor to an image descriptor
 *  @param src  The src descriptor to fill
 *  @param raw  Pointer to a lv_img_dsc_t instance
 */
void lv_img_src_set_raw(lv_img_src_t * obj, const lv_img_dsc_t * raw);

/** Get an image source to a text with any symbol in it
 *  @param symbol An textual strings with symbols
 *  @return a lv_img_src_move_t object instance that doesn't need to be free is used as argument to a LVGL function
*/
lv_img_src_move_t lv_img_src_from_symbol(const char * symbol);
/** Get an image source to a byte array containing the image encoded data
 *  @param data A pointer to the image's data
 *  @param len  The length pointed by data in bytes
 *  @return a lv_img_src_move_t object instance that doesn't need to be free is used as argument to a LVGL function
*/
lv_img_src_move_t lv_img_src_from_data(const uint8_t * data, const size_t len);
/**Get an image source to a file
 *  @param path Path to the file containing the image
 *  @return a lv_img_src_move_t object instance that doesn't need to be free is used as argument to a LVGL function
 */
lv_img_src_move_t lv_img_src_from_file(const char * file_path);
/** Get an image source to an image descriptor
 *  @param raw  Pointer to a lv_img_dsc_t instance
 *  @return a lv_img_src_move_t object instance that doesn't need to be free is used as argument to a LVGL function
 */
lv_img_src_move_t lv_img_src_from_raw(const lv_img_dsc_t * raw);
/** Get an image source to an empty object (no image)
 *  @return a lv_img_src_move_t object instance that doesn't need to be free is used as argument to a LVGL function
 */
lv_img_src_move_t lv_img_src_empty(void);



/** Copy the source of the descriptor to another descriptor
 *  @param dest The dest object to fill
 *  @param src  The source object to read from
 */
void lv_img_src_copy(lv_img_src_t * dest, const lv_img_src_t * src);
/** Capture  the source of the descriptor and move to another descriptor
 *  @param dest The dest object to fill
 *  @param src  The source object to move from
 */
void lv_img_src_capture(lv_img_src_t * dest, lv_img_src_move_t * src);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMG_DECODER_H*/
