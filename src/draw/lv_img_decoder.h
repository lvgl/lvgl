/**
 * @file lv_img_decoder.h
 *
 */

#ifndef LV_IMG_DECODER_H
#define LV_IMG_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include <stdint.h>
#include "lv_img_buf.h"
#include "../misc/lv_fs.h"
#include "../misc/lv_types.h"
#include "../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/
#define LV_BUILTIN_ID   0xB1

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Source of image.*/
enum {
    LV_IMG_SRC_VARIABLE, /** Binary/C variable*/
    LV_IMG_SRC_FILE, /** File in filesystem*/
    LV_IMG_SRC_SYMBOL, /** Symbol (@ref lv_symbol_def.h)*/
    LV_IMG_SRC_UNKNOWN, /** Unknown source*/
};

typedef uint8_t lv_img_src_t;


/**
 * Capabilities of an image decoder context.
   Set by the decoder when extracting info or being opened */
typedef enum {
    LV_IMG_DEC_DEFAULT      = 0x00, /**!< Default format, no specificities */
    LV_IMG_DEC_VECTOR       = 0x01, /**!< Image format is vector based, size independant */
    LV_IMG_DEC_ANIMATED     = 0x02, /**!< Image format stores an animation */
    LV_IMG_DEC_SEEKABLE     = 0x04, /**!< Animation is seekable */
    LV_IMG_DEC_CACHED       = 0x08, /**!< The complete image can be cached (used for rotation and zoom) */
} lv_img_decoder_caps_t;

/**
 * Base type for a decoder context.
 * You'll likely bootstrap from this to make your own, if you need too
 */
typedef struct {
    uint8_t    magic_id;       /**!< Used to ensure a decoder context is not used in another decoder context */
    uint8_t    caps;           /**!< The decoder capabilities flags */
    uint16_t   current_frame;  /**!< The current frame index */
    uint16_t   total_frames;   /**!< The number of frames (likely filled by the decoder) */
    uint16_t   self_allocated; /**!< Is is self allocated (and should be freed by the decoder close function) */
    void   *   user_data;      /**!< Available for per-decoder features */
} lv_img_dec_ctx_t;

/*Decoder function definitions*/
struct _lv_img_decoder_dsc_t;
struct _lv_img_decoder_t;

/**
 * Get info from an image and store in the `header`
 * @param decoder Pointer to the image decoder object
 * @param src the image source. Can be a pointer to a C array or a file name (Use
 * `lv_img_src_get_type` to determine the type)
 * @param header store the info here
 * @param dec_ctx Pointer to decoder initialization context. Can be NULL.
 * @return LV_RES_OK: info written correctly; LV_RES_INV: failed
 */
typedef lv_res_t (*lv_img_decoder_info_f_t)(struct _lv_img_decoder_t * decoder, const void * src,
                                            lv_img_header_t * header, lv_img_dec_ctx_t * dec_ctx);

/**
 * Open an image for decoding. Prepare it as it is required to read it later
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor. `src`, `color` are already initialized in it.
 * @param dec_ctx Pointer to decoder initialization context. Can be NULL.
 */
typedef lv_res_t (*lv_img_decoder_open_f_t)(struct _lv_img_decoder_t * decoder, struct _lv_img_decoder_dsc_t * dsc,
                                            lv_img_dec_ctx_t * dec_ctx);

/**
 * Decode `len` pixels starting from the given `x`, `y` coordinates and store them in `buf`.
 * Required only if the "open" function can't return with the whole decoded pixel array.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 * @param x start x coordinate
 * @param y start y coordinate
 * @param len number of pixels to decode
 * @param buf a buffer to store the decoded pixels
 * @return LV_RES_OK: ok; LV_RES_INV: failed
 */
typedef lv_res_t (*lv_img_decoder_read_line_f_t)(struct _lv_img_decoder_t * decoder, struct _lv_img_decoder_dsc_t * dsc,
                                                 lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

/**
 * Close the pending decoding. Free resources etc.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 */
typedef void (*lv_img_decoder_close_f_t)(struct _lv_img_decoder_t * decoder, struct _lv_img_decoder_dsc_t * dsc);


typedef struct _lv_img_decoder_t {
    lv_img_decoder_info_f_t info_cb;
    lv_img_decoder_open_f_t open_cb;
    lv_img_decoder_read_line_f_t read_line_cb;
    lv_img_decoder_close_f_t close_cb;

#if LV_USE_USER_DATA
    void * user_data;
#endif
} lv_img_decoder_t;


/**Describe an image decoding session. Stores data about the decoding*/
typedef struct _lv_img_decoder_dsc_t {
    /**The decoder which was able to open the image source*/
    lv_img_decoder_t * decoder;

    /**The image source. A file path like "S:my_img.png" or pointer to an `lv_img_dsc_t` variable*/
    const void * src;

    /**Color to draw the image. Used when the image has alpha channel only*/
    lv_color_t color;

    /**Size hint for decoders with user settable output size*/
    lv_point_t size_hint;

    /**Type of the source: file or variable. Can be set in `open` function if required*/
    lv_img_src_t src_type;

    /**Info about the opened image: color format, size, etc. MUST be set in `open` function*/
    lv_img_header_t header;

    /** Pointer to a buffer where the image's data (pixels) are stored in a decoded, plain format.
     *  MUST be set in `open` function*/
    const uint8_t * img_data;

    /** How much time did it take to open the image. [ms]
     *  If not set `lv_img_cache` will measure and set the time to open*/
    uint32_t time_to_open;

    /**A text to display instead of the image when the image can't be opened.
     * Can be set in `open` function or set NULL.*/
    const char * error_msg;

    /**Store any custom data here is required*/
    void * user_data;

    /**Initialization context for decoder*/
    lv_img_dec_ctx_t * dec_ctx;
} lv_img_decoder_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the image decoder module
 */
void _lv_img_decoder_init(void);

/**
 * Get information about an image.
 * Try the created image decoder one by one. Once one is able to get info that info will be used.
 * @param src the image source. Can be
 *  1) File name: E.g. "S:folder/img1.png" (The drivers needs to registered via `lv_fs_add_drv()`)
 *  2) Variable: Pointer to an `lv_img_dsc_t` variable
 *  3) Symbol: E.g. `LV_SYMBOL_OK`
 * @param header the image info will be stored here
 * @param dec_ctx A context to configure the decoder. Can be NULL for default behavior.
 * @return LV_RES_OK: success; LV_RES_INV: wasn't able to get info about the image
 */
lv_res_t lv_img_decoder_get_info(const void * src, lv_img_header_t * header, lv_img_dec_ctx_t * dec_ctx);

/**
 * Open an image.
 * Try the created image decoder one by one. Once one is able to open the image that decoder is save in `dsc`
 * @param dsc describe a decoding session. Simply a pointer to an `lv_img_decoder_dsc_t` variable.
 * @param src the image source. Can be
 *  1) File name: E.g. "S:folder/img1.png" (The drivers needs to registered via `lv_fs_add_drv()`)
 *  2) Variable: Pointer to an `lv_img_dsc_t` variable
 *  3) Symbol: E.g. `LV_SYMBOL_OK`
 * @param color The color of the image with `LV_IMG_CF_ALPHA_...`
 * @param size_hint If provided, contains the user requested size for the output image
 * @param dec_ctx A context to configure the decoder. Can be NULL for default behavior.
 * @return LV_RES_OK: opened the image. `dsc->img_data` and `dsc->header` are set.
 *         LV_RES_INV: none of the registered image decoders were able to open the image.
 */
lv_res_t lv_img_decoder_open(lv_img_decoder_dsc_t * dsc, const void * src, lv_color_t color, lv_point_t size_hint,
                             lv_img_dec_ctx_t * dec_ctx);

/**
 * Read a line from an opened image
 * @param dsc pointer to `lv_img_decoder_dsc_t` used in `lv_img_decoder_open`
 * @param x start X coordinate (from left)
 * @param y start Y coordinate (from top)
 * @param len number of pixels to read
 * @param buf store the data here
 * @return LV_RES_OK: success; LV_RES_INV: an error occurred
 */
lv_res_t lv_img_decoder_read_line(lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y, lv_coord_t len,
                                  uint8_t * buf);

/**
 * Close a decoding session
 * @param dsc pointer to `lv_img_decoder_dsc_t` used in `lv_img_decoder_open`
 */
void lv_img_decoder_close(lv_img_decoder_dsc_t * dsc);

/**
 * Create a new image decoder
 * @return pointer to the new image decoder
 */
lv_img_decoder_t * lv_img_decoder_create(void);

/**
 * Delete an image decoder
 * @param decoder pointer to an image decoder
 */
void lv_img_decoder_delete(lv_img_decoder_t * decoder);

/**
 * Set a callback to get information about the image
 * @param decoder pointer to an image decoder
 * @param info_cb a function to collect info about an image (fill an `lv_img_header_t` struct)
 */
void lv_img_decoder_set_info_cb(lv_img_decoder_t * decoder, lv_img_decoder_info_f_t info_cb);

/**
 * Set a callback to open an image
 * @param decoder pointer to an image decoder
 * @param open_cb a function to open an image
 */
void lv_img_decoder_set_open_cb(lv_img_decoder_t * decoder, lv_img_decoder_open_f_t open_cb);

/**
 * Set a callback to a decoded line of an image
 * @param decoder pointer to an image decoder
 * @param read_line_cb a function to read a line of an image
 */
void lv_img_decoder_set_read_line_cb(lv_img_decoder_t * decoder, lv_img_decoder_read_line_f_t read_line_cb);

/**
 * Set a callback to close a decoding session. E.g. close files and free other resources.
 * @param decoder pointer to an image decoder
 * @param close_cb a function to close a decoding session
 */
void lv_img_decoder_set_close_cb(lv_img_decoder_t * decoder, lv_img_decoder_close_f_t close_cb);

/**
 * Get info about a built-in image
 * @param decoder the decoder where this function belongs
 * @param src the image source: pointer to an `lv_img_dsc_t` variable, a file path or a symbol
 * @param header store the image data here
 * @param dec_ctx Pointer to decoder initialization context. Can be NULL.
 * @return LV_RES_OK: the info is successfully stored in `header`; LV_RES_INV: unknown format or other error.
 */
lv_res_t lv_img_decoder_built_in_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header,
                                      lv_img_dec_ctx_t * dec_ctx);

/**
 * Open a built in image
 * @param decoder the decoder where this function belongs
 * @param dsc pointer to decoder descriptor. `src`, `style` are already initialized in it.
 * @param dec_ctx Pointer to decoder initialization context. Can be NULL.
 * @return LV_RES_OK: the info is successfully stored in `header`; LV_RES_INV: unknown format or other error.
 */
lv_res_t lv_img_decoder_built_in_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc,
                                      lv_img_dec_ctx_t * dec_ctx);

/**
 * Decode `len` pixels starting from the given `x`, `y` coordinates and store them in `buf`.
 * Required only if the "open" function can't return with the whole decoded pixel array.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 * @param x start x coordinate
 * @param y start y coordinate
 * @param len number of pixels to decode
 * @param buf a buffer to store the decoded pixels
 * @return LV_RES_OK: ok; LV_RES_INV: failed
 */
lv_res_t lv_img_decoder_built_in_read_line(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc, lv_coord_t x,
                                           lv_coord_t y, lv_coord_t len, uint8_t * buf);

/**
 * Close the pending decoding. Free resources etc.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 */
void lv_img_decoder_built_in_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMG_DECODER_H*/
