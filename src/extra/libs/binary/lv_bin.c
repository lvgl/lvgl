/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_img_decoder_built_in_line_true_color(lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y,
                                                        lv_coord_t len, uint8_t * buf);
static lv_res_t lv_img_decoder_built_in_line_alpha(lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y,
                                                   lv_coord_t len, uint8_t * buf);
static lv_res_t lv_img_decoder_built_in_line_indexed(lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y,
                                                     lv_coord_t len, uint8_t * buf);



void lv_bin_init()
{
    /*Create a decoder for the built in color format*/
    lv_img_decoder_t * decoder = lv_img_decoder_create();
    LV_ASSERT_MALLOC(decoder);
    if(decoder == NULL) {
        LV_LOG_WARN("lv_img_decoder_init: out of memory");
        return;
    }

    lv_img_decoder_set_accept_cb(decoder, lv_img_decoder_built_in_accept);
    lv_img_decoder_set_open_cb(decoder, lv_img_decoder_built_in_open);
    lv_img_decoder_set_read_line_cb(decoder, lv_img_decoder_built_in_read_line);
    lv_img_decoder_set_close_cb(decoder, lv_img_decoder_built_in_close);
}

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
