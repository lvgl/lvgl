/**
 * @file lv_image_encoder.h
 *
 */

#ifndef LV_IMAGE_ENCODER_H
#define LV_IMAGE_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_types.h"
#include "../draw/lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Image encoder args.
 * It determines how to encode an image, e.g. JPEG quality or PNG compression level.
 */
typedef struct {
    int32_t quality;            /**< JPEG quality 1-100, default 85. Ignored by PNG. */
    int32_t compression_level;  /**< PNG compression 0-9, default 6. Ignored by JPEG. */
} lv_image_encoder_args_t;

/**
 * Encoder callback. Return LV_RESULT_OK on success,
 * LV_RESULT_INVALID if this encoder doesn't handle the given path/format.
 */
typedef lv_result_t (*lv_image_encoder_save_cb_t)(lv_image_encoder_t * encoder,
                                                  const lv_draw_buf_t * draw_buf,
                                                  const char * path,
                                                  const lv_image_encoder_args_t * args);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a new image encoder and add it to the encoder chain.
 * @return pointer to the new encoder
 */
lv_image_encoder_t * lv_image_encoder_create(void);

/**
 * Delete an image encoder from the chain.
 * @param encoder pointer to an image encoder, NULL is ignored @nullable
 */
void lv_image_encoder_delete(lv_image_encoder_t * encoder);

/**
 * Get the next encoder in the chain.
 * @param encoder pointer to an encoder or NULL to get the first one @nullable
 * @return the next encoder or NULL if no more
 */
lv_image_encoder_t * lv_image_encoder_get_next(lv_image_encoder_t * encoder);

/**
 * Set the save callback for an encoder.
 * @param encoder pointer to an image encoder
 * @param save_cb save callback
 */
void lv_image_encoder_set_save_cb(lv_image_encoder_t * encoder, lv_image_encoder_save_cb_t save_cb);

/**
 * Set the name of an encoder (for debug/profiling).
 * @param encoder pointer to an image encoder
 * @param name encoder name string (not copied, must be static/persistent) @nullable
 */
void lv_image_encoder_set_name(lv_image_encoder_t * encoder, const char * name);

/**
 * Set user data for an encoder.
 * @param encoder pointer to an image encoder
 * @param user_data pointer to user data @nullable
 */
void lv_image_encoder_set_user_data(lv_image_encoder_t * encoder, void * user_data);

/**
 * Get the user data of an encoder, e.g. in its save callback.
 * @param encoder pointer to an image encoder
 * @return        the user data set by `lv_image_encoder_set_user_data`, NULL if none
 */
void * lv_image_encoder_get_user_data(const lv_image_encoder_t * encoder);

/**
 * Initialize encoder args with default values.
 * @param args pointer to encoder args to initialize
 */
void lv_image_encoder_args_init(lv_image_encoder_args_t * args);

/**
 * Encode a draw buffer and save to a file.
 * Iterates through the encoder chain until one succeeds.
 * Automatically drops image cache for the path on success.
 * @param draw_buf pointer to the draw buffer to encode
 * @param path     output file path (extension determines format)
 * @param args     encoder args, NULL for defaults @nullable
 * @return LV_RESULT_OK on success, LV_RESULT_INVALID on error
 */
lv_result_t lv_image_encoder_save(const lv_draw_buf_t * draw_buf,
                                  const char * path,
                                  const lv_image_encoder_args_t * args);

/**
 * Save a draw buffer to a file with the default encoder args.
 * Same as `lv_image_encoder_save(draw_buf, path, NULL)`.
 * @param draw_buf  pointer to a draw buffer
 * @param path      path to the file to save, the extension selects the format
 * @return          LV_RESULT_OK: success; LV_RESULT_INVALID: error
 */
lv_result_t lv_draw_buf_save_to_file(const lv_draw_buf_t * draw_buf, const char * path);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGE_ENCODER_H*/
