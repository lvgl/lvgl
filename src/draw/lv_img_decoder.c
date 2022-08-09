/**
 * @file lv_img_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img_decoder.h"
#include "../misc/lv_assert.h"
#include "../draw/lv_draw_img.h"
#include "../misc/lv_ll.h"
#include "../misc/lv_gc.h"

/*********************
 *      DEFINES
 *********************/
#define CF_BUILT_IN_FIRST   LV_IMG_CF_TRUE_COLOR
#define CF_BUILT_IN_LAST    LV_IMG_CF_RGB565A8

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_bin_init(void);
void _lv_img_decoder_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_img_decoder_ll), sizeof(lv_img_decoder_t));

    /*Must be the last one in the list, since it's not restrictive in what it accepts*/
    lv_bin_init();
}

void lv_img_dec_dsc_in_init(lv_img_dec_dsc_in_t * desc, const lv_img_src_t * src, lv_color32_t * color,
                            lv_point_t * size_hint)
{
    lv_memzero(desc, sizeof(*desc));
    desc->src = src;
    if(color) desc->color = *color;
    if(size_hint) desc->size_hint = *size_hint;
}

lv_res_t lv_img_decoder_get_info(const lv_img_dec_dsc_in_t * dsc_in, lv_img_header_t * header)
{
    /*TODO: We should search the cache here, since we might already have a decoder with the information we need */
    lv_img_dec_dsc_t dsc = {0};
    lv_memcpy(&dsc.input, dsc_in, sizeof(*dsc_in));
    if(lv_img_decoder_open(&dsc, LV_IMG_DEC_ONLYMETA) == LV_RES_OK) {
        lv_memcpy(header, &dsc.header, sizeof(*header));
        return LV_RES_OK;
    }
    return LV_RES_INV;
}

lv_img_decoder_t * lv_img_decoder_accept(const lv_img_src_t * src, uint8_t * caps)
{
    if(src == NULL) return NULL;

    lv_res_t res = LV_RES_INV;
    lv_img_decoder_t * d;
    _LV_LL_READ(&LV_GC_ROOT(_lv_img_decoder_ll), d) {
        if(d->accept_cb) {
            uint8_t dummy_caps;
            if(caps == NULL) caps = &dummy_caps;
            res = d->accept_cb(src, caps, d->user_data);
            if(res == LV_RES_OK) return d;
        }
    }

    return NULL;
}

lv_res_t lv_img_decoder_open(lv_img_dec_dsc_t * dsc, const lv_img_dec_flags_t flags)
{
    if(dsc == NULL) return LV_RES_INV;

    lv_res_t res = LV_RES_INV;

    lv_img_decoder_t * decoder = dsc->decoder;
    if(decoder == NULL) {
        _LV_LL_READ(&LV_GC_ROOT(_lv_img_decoder_ll), decoder) {
            uint8_t dummy_caps;

            /*Accept is required to find the decoder that would accept this*/
            if(decoder->accept_cb == NULL) continue;

            res = decoder->accept_cb(dsc->input.src, &dummy_caps, decoder->user_data);
            if(res == LV_RES_OK) break;
        }
    }
    if(decoder == NULL) return LV_RES_INV;

    dsc->decoder = decoder;
    return decoder->open_cb(dsc, flags, decoder->user_data);
}

lv_res_t lv_img_decoder_read_line(lv_img_dec_dsc_t * dsc, lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    lv_res_t res = LV_RES_INV;
    if(dsc->decoder->read_line_cb) res = dsc->decoder->read_line_cb(dsc, x, y, len, buf, dsc->decoder->user_data);

    return res;
}

void lv_img_decoder_close(lv_img_dec_dsc_t * dsc)
{
    if(dsc->decoder) {
        if(dsc->decoder->close_cb) dsc->decoder->close_cb(dsc, dsc->decoder->user_data);
        dsc->input.src = NULL;
        dsc->decoder = NULL;
    }
}


lv_img_decoder_t * lv_img_decoder_create(void * user_data)
{
    lv_img_decoder_t * decoder;
    decoder = _lv_ll_ins_head(&LV_GC_ROOT(_lv_img_decoder_ll));
    LV_ASSERT_MALLOC(decoder);
    if(decoder == NULL) return NULL;

    lv_memzero(decoder, sizeof(lv_img_decoder_t));
    decoder->user_data = user_data;
    return decoder;
}


void lv_img_decoder_delete(lv_img_decoder_t * decoder)
{
    _lv_ll_remove(&LV_GC_ROOT(_lv_img_decoder_ll), decoder);
    lv_free(decoder);
}

void lv_img_decoder_set_accept_cb(lv_img_decoder_t * decoder, lv_img_decoder_accept_f_t accept_cb)
{
    decoder->accept_cb = accept_cb;
}

/**
 * Set a callback to open an image
 * @param decoder pointer to an image decoder
 * @param open_cb a function to open an image
 */
void lv_img_decoder_set_open_cb(lv_img_decoder_t * decoder, lv_img_decoder_open_f_t open_cb)
{
    decoder->open_cb = open_cb;
}

/**
 * Set a callback to a decoded line of an image
 * @param decoder pointer to an image decoder
 * @param read_line_cb a function to read a line of an image
 */
void lv_img_decoder_set_read_line_cb(lv_img_decoder_t * decoder, lv_img_decoder_read_line_f_t read_line_cb)
{
    decoder->read_line_cb = read_line_cb;
}

/**
 * Set a callback to close a decoding session. E.g. close files and free other resources.
 * @param decoder pointer to an image decoder
 * @param close_cb a function to close a decoding session
 */
void lv_img_decoder_set_close_cb(lv_img_decoder_t * decoder, lv_img_decoder_close_f_t close_cb)
{
    decoder->close_cb = close_cb;
}

bool lv_img_decoder_has_size_hint(const lv_img_dec_dsc_in_t * dsc)
{
    /* Invalid is if both set to LV_SIZE_CONTENT or if any is 0*/
    return !((dsc->size_hint.x == LV_SIZE_CONTENT && dsc->size_hint.y == LV_SIZE_CONTENT)
             || dsc->size_hint.x == 0 || dsc->size_hint.y == 0);
}

bool _lv_is_raw_decoder(lv_img_decoder_t * decoder)
{
    lv_img_decoder_t * d = _lv_ll_get_tail(&LV_GC_ROOT(_lv_img_decoder_ll));
    return d == decoder;
}
