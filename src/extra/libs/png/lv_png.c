/**
 * @file lv_png.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_PNG

#include "lv_png.h"
#include "lodepng.h"
#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t decoder_accept(const lv_img_src_t * src, uint8_t * caps);
static lv_res_t decoder_open(lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags);
static void decoder_close(lv_img_decoder_dsc_t * dsc);
static void convert_color_depth(uint8_t * img, uint32_t px_cnt);

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
 * Register the PNG decoder functions in LVGL
 */
void lv_png_init(void)
{
    lv_img_decoder_t * dec = lv_img_decoder_create();
    lv_img_decoder_set_accept_cb(dec, decoder_accept);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_close_cb(dec, decoder_close);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t decoder_accept(const lv_img_src_t * src, uint8_t * caps)
{
    /*If it's a PNG file...*/
    if(src->type == LV_IMG_SRC_FILE) {
        /*Support only "*.png" files*/
        if(!src->ext || strcmp(src->ext, ".png")) return LV_RES_INV;
        if(caps) *caps = LV_IMG_DEC_CACHED;  /*Image is not cached for files*/

        /*Check file exists*/
        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, src->uri, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) return LV_RES_INV;
        lv_fs_close(&f);

        return LV_RES_OK;
    }
    else if (src->type == LV_IMG_SRC_VARIABLE) {
        const uint8_t magic[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
        if(src->uri_len > sizeof(magic) || memcmp(magic, src->uri, sizeof(magic)) == 0) return LV_RES_OK;
    }
    return LV_RES_INV;
}
static lv_res_t decoder_open(lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags)
{
    uint8_t * caps = &dsc->caps;

    /*If it's a PNG file...*/
    uint32_t size[2];
    uint32_t error;

    unsigned char * png_data;      /*Pointer to the loaded data. Same as the original file just loaded into the RAM*/
    size_t png_data_size;          /*Size of `png_data` in bytes*/
    bool   free_data = false;
    if(dsc->input.src->type == LV_IMG_SRC_FILE) {
        if(!dsc->input.src->ext || strcmp(dsc->input.src->ext, ".png")) return LV_RES_INV;

        if (flags == LV_IMG_DEC_ONLYMETA) {
            /* Read the width and height from the file. They have a constant location:
            * [16..23]: width
            * [24..27]: height
            */
            lv_fs_file_t f;
            lv_fs_res_t res = lv_fs_open(&f, (const char*)dsc->input.src->uri, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) return LV_RES_INV;
            lv_fs_seek(&f, 16, LV_FS_SEEK_SET);
            uint32_t rn;
            lv_fs_read(&f, &size, 8, &rn);
            lv_fs_close(&f);
            if(rn != 8) return LV_RES_INV;
        } else {
            /*Load the PNG file into buffer. It's still compressed (not decoded)*/
            error = lodepng_load_file(&png_data, &png_data_size, dsc->input.src->uri);   /*Load the file*/
            if(error) {
                LV_LOG_WARN("error %u: %s\n", error, lodepng_error_text(error));
                return LV_RES_INV;
            }
            free_data = true;
        }
    }
    else if (dsc->input.src->type == LV_IMG_SRC_VARIABLE) {
        if (flags == LV_IMG_DEC_ONLYMETA) {
            memcpy(size, (const char*)dsc->input.src->uri + 16, 8);
        } else {
            png_data_size = dsc->input.src->uri_len;
            png_data = (unsigned char*)dsc->input.src->uri;
        }
    }
    *caps = LV_IMG_DEC_CACHED;

    if (flags == LV_IMG_DEC_ONLYMETA) {
        dsc->header.always_zero = 0;
        dsc->header.cf = LV_IMG_CF_RAW_ALPHA;
        /*The width and height are stored in Big endian format so convert them to little endian*/
        dsc->header.w = (lv_coord_t)((size[0] & 0xff000000) >> 24) + ((size[0] & 0x00ff0000) >> 8);
        dsc->header.h = (lv_coord_t)((size[1] & 0xff000000) >> 24) + ((size[1] & 0x00ff0000) >> 8);
        return LV_RES_OK;
    }

    /*Decode the PNG image*/
    uint32_t png_width;             /*Will be the width of the decoded image*/
    uint32_t png_height;            /*Will be the height of the decoded image*/

    /*Decode the loaded image in ARGB8888 */
    uint8_t * img_data = (uint8_t*)dsc->img_data;
    error = lodepng_decode32(&img_data, &png_width, &png_height, png_data, png_data_size);
    if(free_data) lv_mem_free(png_data); /*Free the loaded file*/
    if(error) {
        LV_LOG_WARN("error %u: %s\n", error, lodepng_error_text(error));
        return LV_RES_INV;
    }

    /*Convert the image to the system's color depth*/
    convert_color_depth(img_data, png_width * png_height);
    dsc->header.w = png_width;
    dsc->header.h = png_height;
    dsc->header.cf = LV_IMG_CF_RAW_ALPHA;
    dsc->img_data = img_data;
    return LV_RES_OK;
}

/**
 * Free the allocated resources
 */
static void decoder_close(lv_img_decoder_dsc_t * dsc)
{
    if(dsc->img_data) {
        lv_mem_free((uint8_t *)dsc->img_data); /*Not sure here it's allocated with lv_mem_alloc ?*/
        dsc->img_data = NULL;
    }
}

/**
 * If the display is not in 32 bit format (ARGB888) then covert the image to the current color depth
 * @param img the ARGB888 image
 * @param px_cnt number of pixels in `img`
 */
static void convert_color_depth(uint8_t * img, uint32_t px_cnt)
{
#if LV_COLOR_DEPTH == 32
    lv_color32_t * img_argb = (lv_color32_t *)img;
    lv_color_t c;
    lv_color_t * img_c = (lv_color_t *) img;
    uint32_t i;
    for(i = 0; i < px_cnt; i++) {
        c = lv_color_make(img_argb[i].ch.red, img_argb[i].ch.green, img_argb[i].ch.blue);
        img_c[i].ch.red = c.ch.blue;
        img_c[i].ch.blue = c.ch.red;
    }
#elif LV_COLOR_DEPTH == 16
    lv_color32_t * img_argb = (lv_color32_t *)img;
    lv_color_t c;
    uint32_t i;
    for(i = 0; i < px_cnt; i++) {
        c = lv_color_make(img_argb[i].ch.blue, img_argb[i].ch.green, img_argb[i].ch.red);
        img[i * 3 + 2] = img_argb[i].ch.alpha;
        img[i * 3 + 1] = c.full >> 8;
        img[i * 3 + 0] = c.full & 0xFF;
    }
#elif LV_COLOR_DEPTH == 8
    lv_color32_t * img_argb = (lv_color32_t *)img;
    lv_color_t c;
    uint32_t i;
    for(i = 0; i < px_cnt; i++) {
        c = lv_color_make(img_argb[i].ch.red, img_argb[i].ch.green, img_argb[i].ch.blue);
        img[i * 2 + 1] = img_argb[i].ch.alpha;
        img[i * 2 + 0] = c.full;
    }
#elif LV_COLOR_DEPTH == 1
    lv_color32_t * img_argb = (lv_color32_t *)img;
    uint8_t b;
    uint32_t i;
    for(i = 0; i < px_cnt; i++) {
        b = img_argb[i].ch.red | img_argb[i].ch.green | img_argb[i].ch.blue;
        img[i * 2 + 1] = img_argb[i].ch.alpha;
        img[i * 2 + 0] = b > 128 ? 1 : 0;
    }
#endif
}

#endif /*LV_USE_PNG*/


