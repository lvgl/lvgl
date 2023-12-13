/**
 * @file lv_image_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_image_decoder.h"
#include "../misc/lv_assert.h"
#include "../draw/lv_draw_image.h"
#include "../misc/lv_ll.h"
#include "../stdlib/lv_string.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define img_decoder_ll_p &(LV_GLOBAL_DEFAULT()->img_decoder_ll)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t img_width_to_stride(lv_image_header_t * header);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the image decoder module
 */
void _lv_image_decoder_init(void)
{
    _lv_ll_init(img_decoder_ll_p, sizeof(lv_image_decoder_t));
}

/**
 * Deinitialize the image decoder module
 */
void _lv_image_decoder_deinit(void)
{
    _lv_ll_clear(img_decoder_ll_p);
}

/**
 * Get information about an image.
 * Try the created image decoder one by one. Once one is able to get info that info will be used.
 * @param src the image source. E.g. file name or variable.
 * @param header the image info will be stored here
 * @return LV_RESULT_OK: success; LV_RESULT_INVALID: wasn't able to get info about the image
 */
lv_result_t lv_image_decoder_get_info(const void * src, lv_image_header_t * header)
{
    lv_memzero(header, sizeof(lv_image_header_t));

    if(src == NULL) return LV_RESULT_INVALID;

    lv_image_src_t src_type = lv_image_src_get_type(src);
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        if(img_dsc->data == NULL) return LV_RESULT_INVALID;
    }

    lv_result_t res = LV_RESULT_INVALID;
    lv_image_decoder_t * decoder;
    _LV_LL_READ(img_decoder_ll_p, decoder) {
        if(decoder->info_cb) {
            res = decoder->info_cb(decoder, src, header);
            if(res == LV_RESULT_OK) {
                if(header->stride == 0) header->stride = img_width_to_stride(header);
                break;
            }
        }
    }

    return res;
}

lv_result_t lv_image_decoder_open(lv_image_decoder_dsc_t * dsc, const void * src, const lv_image_decoder_args_t * args)
{
    lv_memzero(dsc, sizeof(lv_image_decoder_dsc_t));

    if(src == NULL) return LV_RESULT_INVALID;
    lv_image_src_t src_type = lv_image_src_get_type(src);
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        if(img_dsc->data == NULL) return LV_RESULT_INVALID;
    }

    dsc->src_type = src_type;

    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        size_t fnlen = lv_strlen(src);
        dsc->src = lv_malloc(fnlen + 1);
        LV_ASSERT_MALLOC(dsc->src);
        if(dsc->src == NULL) {
            LV_LOG_WARN("Out of memory");
            return LV_RESULT_INVALID;
        }
        lv_strcpy((char *)dsc->src, src);
    }
    else {
        dsc->src = src;
    }

    lv_result_t res = LV_RESULT_INVALID;

    lv_image_decoder_t * decoder;
    lv_image_decoder_args_t * args_copy = NULL;

    /*Make a copy of args */
    if(args) {
        args_copy = lv_malloc(sizeof(lv_image_decoder_args_t));
        LV_ASSERT_MALLOC(args_copy);
        if(args_copy == NULL) {
            LV_LOG_WARN("Out of memory");
            return LV_RESULT_INVALID;
        }
        lv_memcpy(args_copy, args, sizeof(lv_image_decoder_args_t));
        dsc->args = args_copy;
    }

    _LV_LL_READ(img_decoder_ll_p, decoder) {
        /*Info and Open callbacks are required*/
        if(decoder->info_cb == NULL || decoder->open_cb == NULL) continue;

        res = decoder->info_cb(decoder, src, &dsc->header);
        if(res != LV_RESULT_OK) continue;

        if(dsc->header.stride == 0) dsc->header.stride = img_width_to_stride(&dsc->header);

        dsc->decoder = decoder;
        res = decoder->open_cb(decoder, dsc, args);

        /*Opened successfully. It is a good decoder for this image source*/
        if(res == LV_RESULT_OK) return res;

        /*Prepare for the next loop*/
        lv_memzero(&dsc->header, sizeof(lv_image_header_t));

        dsc->error_msg = NULL;
        dsc->img_data  = NULL;
        dsc->decoded  = NULL;
        dsc->cache_entry = NULL;
        dsc->user_data = NULL;
        dsc->time_to_open = 0;
    }

    if(dsc->src_type == LV_IMAGE_SRC_FILE)
        lv_free((void *)dsc->src);

    if(args_copy) lv_free(args_copy);

    return res;
}

/**
 * Decode an area of image
 * @param decoder      pointer to the decoder where this function belongs
 * @param dsc          pointer to `lv_image_decoder_dsc_t` used in `lv_image_decoder_open`
 * @param full_area    full image area information
 * @param decoded_area area information to decode (x1, y1, x2, y2)
 * @return             LV_RESULT_OK: no error; LV_RESULT_INVALID: can't decode image area
 */
lv_result_t lv_image_decoder_get_area(lv_image_decoder_dsc_t * dsc, const lv_area_t * full_area,
                                      lv_area_t * decoded_area)
{
    lv_result_t res = LV_RESULT_INVALID;
    if(dsc->decoder->get_area_cb) res = dsc->decoder->get_area_cb(dsc->decoder, dsc, full_area, decoded_area);

    return res;
}

/**
 * Close a decoding session
 * @param dsc pointer to `lv_image_decoder_dsc_t` used in `lv_image_decoder_open`
 */
void lv_image_decoder_close(lv_image_decoder_dsc_t * dsc)
{
    if(dsc->decoder) {
        if(dsc->decoder->close_cb) dsc->decoder->close_cb(dsc->decoder, dsc);
        if(dsc->args) lv_free(dsc->args);

        if(dsc->src_type == LV_IMAGE_SRC_FILE) {
            lv_free((void *)dsc->src);
            dsc->src = NULL;
        }
    }
}

/**
 * Create a new image decoder
 * @return pointer to the new image decoder
 */
lv_image_decoder_t * lv_image_decoder_create(void)
{
    lv_image_decoder_t * decoder;
    decoder = _lv_ll_ins_head(img_decoder_ll_p);
    LV_ASSERT_MALLOC(decoder);
    if(decoder == NULL) return NULL;

    lv_memzero(decoder, sizeof(lv_image_decoder_t));

    return decoder;
}

/**
 * Delete an image decoder
 * @param decoder pointer to an image decoder
 */
void lv_image_decoder_delete(lv_image_decoder_t * decoder)
{
    _lv_ll_remove(img_decoder_ll_p, decoder);
    lv_free(decoder);
}

/**
 * Get the next image decoder in the linked list of image decoders
 * @param decoder pointer to an image decoder
 * @return the next image decoder or NULL if no more image decoder exists
 */
lv_image_decoder_t * lv_image_decoder_get_next(lv_image_decoder_t * decoder)
{
    if(decoder == NULL)
        return _lv_ll_get_head(img_decoder_ll_p);
    else
        return _lv_ll_get_next(img_decoder_ll_p, decoder);
}

/**
 * Set a callback to get information about the image
 * @param decoder pointer to an image decoder
 * @param info_cb a function to collect info about an image (fill an `lv_image_header_t` struct)
 */
void lv_image_decoder_set_info_cb(lv_image_decoder_t * decoder, lv_image_decoder_info_f_t info_cb)
{
    decoder->info_cb = info_cb;
}

/**
 * Set a callback to open an image
 * @param decoder pointer to an image decoder
 * @param open_cb a function to open an image
 */
void lv_image_decoder_set_open_cb(lv_image_decoder_t * decoder, lv_image_decoder_open_f_t open_cb)
{
    decoder->open_cb = open_cb;
}

/**
 * Set a callback to get decoded area of an image
 * @param decoder       pointer to an image decoder
 * @param get_area_cb   a function to get area of an image
 */
void lv_image_decoder_set_get_area_cb(lv_image_decoder_t * decoder, lv_image_decoder_get_area_cb_t get_area_cb)
{
    decoder->get_area_cb = get_area_cb;
}

/**
 * Set a callback to close a decoding session. E.g. close files and free other resources.
 * @param decoder pointer to an image decoder
 * @param close_cb a function to close a decoding session
 */
void lv_image_decoder_set_close_cb(lv_image_decoder_t * decoder, lv_image_decoder_close_f_t close_cb)
{
    decoder->close_cb = close_cb;
}

static uint32_t img_width_to_stride(lv_image_header_t * header)
{
    if(header->cf == LV_COLOR_FORMAT_RGB565A8) {
        return header->w * 2;
    }
    else {
        return ((uint32_t)header->w * lv_color_format_get_bpp(header->cf) + 7) >> 3;
    }
}

