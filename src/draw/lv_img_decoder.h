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
#include "lv_img_src.h"
#include "../misc/lv_fs.h"
#include "../misc/lv_types.h"
#include "../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
#if defined(LV_IMG_FRAME_INDEX_LARGE_SIZE)
typedef uint32_t lv_frame_index_t;
#else
typedef uint16_t lv_frame_index_t;
#endif

/**
 * Capabilities of an image decoder context.
 * Set by the decoder when extracting info or being opened */
typedef enum {
    LV_IMG_DEC_DEFAULT      = 0x00, /**!< Default format, no specificities */
    LV_IMG_DEC_VECTOR       = 0x01, /**!< Image format is vector based, size independant */
    LV_IMG_DEC_ANIMATED     = 0x02, /**!< Image format stores an animation */
    LV_IMG_DEC_SEEKABLE     = 0x04, /**!< Animation is seekable */
    LV_IMG_DEC_CACHED       = 0x08, /**!< The complete image can be cached (used for rotation and zoom) */
    LV_IMG_DEC_VFR          = 0x10, /**!< The animation has a variable frame rate */
    LV_IMG_DEC_LOOPING      = 0x20, /**!< The animation is looping */
} lv_img_decoder_caps_t;

typedef enum {
    LV_IMG_DEC_ALL          = 0,    /**!< Decode everything */
    LV_IMG_DEC_ONLYMETA     = 1,    /**!< Only decode metadata (like width & height, color format, frame count...) */
} lv_img_dec_flags_t;

/**
 * Base type for a decoder context.
 * You'll likely bootstrap from this to make your own, if you need too
 */
typedef struct {
    uint16_t  auto_allocated : 1; /**!< Is is self allocated (and should be freed by the decoder close function) */
    uint16_t  frame_rate : 15;    /**!< The number of frames per second, if applicable (can be 0 for VFR) */
    lv_frame_index_t   current_frame;  /**!< The current frame index */
    lv_frame_index_t   total_frames;   /**!< The number of frames (likely filled by the decoder) */
    lv_frame_index_t   dest_frame;     /**!< The destination frame (if appropriate) */
    uint16_t     last_rendering; /**!< The last rendering time */
    uint16_t     frame_delay;    /**!< The delay for the current frame in ms */
    void    *    user_data;      /**!< Available for per-decoder features */
} lv_img_dec_ctx_t;

/*Decoder function definitions*/
struct _lv_img_decoder_dsc_t;
struct _lv_img_decoder_t;

/**
 * Get info from an image and store in the `header`
 * @param src the image source. Can be a pointer to a C array or a file name (Use
 * `lv_img_src_get_type` to determine the type)
 * @param header store the info here
 * @return LV_RES_OK: info written correctly; LV_RES_INV: failed
 */
typedef lv_res_t (*lv_img_decoder_info_f_t)(struct _lv_img_decoder_t * decoder, const void * src,
                                            lv_img_header_t * header);

/**
 * Open an image for decoding. Prepare it as it is required to read it later
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor. `src`, `color` are already initialized in it.
 */
typedef lv_res_t (*lv_img_decoder_open_f_t)(struct _lv_img_decoder_t * decoder, struct _lv_img_decoder_dsc_t * dsc);

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

    /**Color to draw the image. USed when the image has alpha channel only*/
    lv_color_t color;

    /**Frame of the image, using with animated images*/
    int32_t frame_id;

    /**Type of the source: file or variable. Can be set in `open` function if required*/
    lv_img_src_type_t src_type;

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
 * @return LV_RES_OK: success; LV_RES_INV: wasn't able to get info about the image
 */
lv_res_t lv_img_decoder_get_info(const void * src, lv_img_header_t * header);

/**
 * Open an image.
 * Try the created image decoder one by one. Once one is able to open the image that decoder is save in `dsc`
 * @param dsc describe a decoding session. Simply a pointer to an `lv_img_decoder_dsc_t` variable.
 * @param src the image source. Can be
 *  1) File name: E.g. "S:folder/img1.png" (The drivers needs to registered via `lv_fs_add_drv()`)
 *  2) Variable: Pointer to an `lv_img_dsc_t` variable
 *  3) Symbol: E.g. `LV_SYMBOL_OK`
 * @param color The color of the image with `LV_IMG_CF_ALPHA_...`
 * @param frame_id the index of the frame. Used only with animated images, set 0 for normal images
 * @return LV_RES_OK: opened the image. `dsc->img_data` and `dsc->header` are set.
 *         LV_RES_INV: none of the registered image decoders were able to open the image.
 */
lv_res_t lv_img_decoder_open(lv_img_decoder_dsc_t * dsc, const void * src, lv_color_t color, int32_t frame_id);

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
 * @return LV_RES_OK: the info is successfully stored in `header`; LV_RES_INV: unknown format or other error.
 */
lv_res_t lv_img_decoder_built_in_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header);

/**
 * Open a built in image
 * @param decoder the decoder where this function belongs
 * @param dsc pointer to decoder descriptor. `src`, `style` are already initialized in it.
 * @return LV_RES_OK: the info is successfully stored in `header`; LV_RES_INV: unknown format or other error.
 */
lv_res_t lv_img_decoder_built_in_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc);

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
