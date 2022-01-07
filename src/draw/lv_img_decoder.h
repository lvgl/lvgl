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
    uint8_t  caps;               /**!< The decoder capabilities flags */
    uint8_t  self_allocated : 1; /**!< Is is self allocated (and should be freed by the decoder close function) */
    uint8_t  frame_rate : 7;     /**!< The number of frames per second, if applicable */
    lv_frame_index_t   current_frame;  /**!< The current frame index */
    lv_frame_index_t   total_frames;   /**!< The number of frames (likely filled by the decoder) */
    lv_frame_index_t   dest_frame;     /**!< The destination frame (if appropriate) */
    void   *     user_data;      /**!< Available for per-decoder features */
} lv_img_dec_ctx_t;

/*Decoder function definitions*/
struct _lv_img_decoder_dsc_t;
struct _lv_img_decoder_t;


/**
 * Check if this decoder accepts the given format
 * @param src the image source
 * @param caps If provided will the filled with the decoder capabilities
 * @return LV_RES_OK: the decoder can decode the given source; LV_RES_INV: it can't decode the source
 */
typedef lv_res_t (*lv_img_decoder_accept_f_t)(const lv_img_src_uri_t * src, uint8_t * caps);

/**
 * Open an image for decoding. Prepare it as it is required to read it later
 * @param dsc   pointer to decoder descriptor. `src`, `color` are already initialized in it.
 * @param flags decoding flags used when decoding is long and only few data required. Might be ignored by decoder.
 */
typedef lv_res_t (*lv_img_decoder_open_f_t)(struct _lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags);

/**
 * Decode `len` pixels starting from the given `x`, `y` coordinates and store them in `buf`.
 * Required only if the "open" function can't return with the whole decoded pixel array.
 * @param dsc pointer to decoder descriptor
 * @param x start x coordinate
 * @param y start y coordinate
 * @param len number of pixels to decode
 * @param buf a buffer to store the decoded pixels
 * @return LV_RES_OK: ok; LV_RES_INV: failed
 */
typedef lv_res_t (*lv_img_decoder_read_line_f_t)(struct _lv_img_decoder_dsc_t * dsc,
                                                 lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

/**
 * Close the pending decoding. Free resources etc.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 */
typedef void (*lv_img_decoder_close_f_t)(struct _lv_img_decoder_dsc_t * dsc);


typedef struct _lv_img_decoder_t {
    lv_img_decoder_accept_f_t       accept_cb;
    lv_img_decoder_open_f_t         open_cb;
    lv_img_decoder_read_line_f_t    read_line_cb;
    lv_img_decoder_close_f_t        close_cb;
} lv_img_decoder_t;


/**Describe an image decoding session. Stores data about the decoding*/
typedef struct _lv_img_decoder_dsc_t {
    /**The decoder which was able to open the image source*/
    lv_img_decoder_t * decoder;

    /**The image source. */
    const lv_img_src_uri_t * src;

    /**Color to draw the image. Used when the image has alpha channel only*/
    lv_color_t color;

    /**Size hint for decoders with user settable output size*/
    lv_point_t size_hint;

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
 * This is a wrapper over lv_img_decoder_accept/lv_img_decoder_open calls.
 * This is not very efficient since it's creating a decoder instance to fetch the information required.
 * Try the created image decoder one by one. Once one is able to get info that info will be used.
 * @param src       the image source.
 * @param header    Will be filled with the capabilities for the selected decoder
 * @return LV_RES_OK if the header was filled
 */
lv_res_t lv_img_decoder_get_info(const lv_img_src_uri_t * src, lv_img_header_t * header);


/**
 * Try to find a decoder that's accepting the given image source.
 * Try the created image decoder one by one. Once one is able to get info that info will be used.
 * @param src   the image source.
 * @param caps  If not null, will be filled with the capabilities for the selected decoder
 * @return A pointer to the decoder that's able to decode the image or NULL if none found
 */
lv_img_decoder_t * lv_img_decoder_accept(const lv_img_src_uri_t * src, uint8_t * caps);

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
lv_res_t lv_img_decoder_open(lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags);

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
 * Set a callback to check if a decoder is able to decode the image
 * @param decoder pointer to an image decoder
 * @param accept_cb a function to accept an image (assert if this decoder can decode the image)
 */
void lv_img_decoder_set_accept_cb(lv_img_decoder_t * decoder, lv_img_decoder_accept_f_t accept_cb);

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


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMG_DECODER_H*/
