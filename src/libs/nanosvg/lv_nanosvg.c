/**
 * @file lv_nanosvg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../draw/lv_image_decoder_private.h"
#include "../../../lvgl.h"

#if LV_USE_NANOSVG

#include "lv_nanosvg.h"
#include "nanosvg.h"
#include "nanosvgrast.h"
#include <string.h>
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define DECODER_NAME    "SVG"

#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * src, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * dec, lv_image_decoder_dsc_t * dsc);

static lv_draw_buf_t * decode_svg_data(NSVGimage * nsvgimage);
static void convert_color_depth(uint8_t * img_p, uint32_t px_cnt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_nanosvg_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_close_cb(dec, decoder_close);

    dec->name = DECODER_NAME;
}

void lv_nanosvg_deinit(void)
{
    lv_image_decoder_t * dec = NULL;
    while((dec = lv_image_decoder_get_next(dec)) != NULL) {
        if(dec->info_cb == decoder_info) {
            lv_image_decoder_delete(dec);
            break;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Get info about a SVG image
 * @param dsc image descriptor containing the source and type of the image and other info.
 * @param header store the info here
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't get the info
 */
static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header)
{
    static NSVGimage * g_image = NULL;
    LV_UNUSED(decoder);

    const void * src = dsc->src;
    lv_image_src_t src_type = dsc->src_type;          /*Get the source type*/

    /*If it's a SVG file...*/
    if(src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = src;
        if(lv_strcmp(lv_fs_get_ext(fn), "svg") == 0) {              /*Check the extension*/
            g_image = nsvgParseFromFile(fn, "px", 96.0f);

            if(g_image == NULL)
                return LV_RESULT_INVALID;
            header->w = (uint32_t)g_image->width;
            header->h = (uint32_t)g_image->height;
            nsvgDelete(g_image);
            return LV_RESULT_OK;
        }
    }
    /* SVG file as data not supported for simplicity.
     * Convert them to LVGL compatible C arrays directly. */
    else if(src_type == LV_IMAGE_SRC_VARIABLE) {
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_INVALID;         /*If didn't succeeded earlier then it's an error*/
}

/**
 * Open a SVG image and return the decided image
 * @param decoder pointer to the decoder
 * @param dsc     pointer to the decoder descriptor
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
 */
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    static NSVGimage * g_image = NULL;
    LV_UNUSED(decoder);

    /*If it's a SVG file...*/
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;

        if(lv_strcmp(lv_fs_get_ext(fn), "svg") != 0) {
            return LV_RESULT_INVALID;       /*Check the extension*/
        }

        g_image = nsvgParseFromFile(fn, "px", 96.0f);
        if(g_image == NULL)
            return LV_RESULT_INVALID;

        lv_draw_buf_t * decoded = decode_svg_data(g_image);
        nsvgDelete(g_image);
        if(!decoded) {
            LV_LOG_WARN("Error decoding SVG");
            LV_PROFILER_DECODER_END_TAG("lv_nanosvg_decoder_open");
            return LV_RESULT_INVALID;
        }

        lv_draw_buf_t * adjusted = lv_image_decoder_post_process(dsc, decoded);
        if(adjusted == NULL) {
            lv_draw_buf_destroy(decoded);
            LV_PROFILER_DECODER_END_TAG("lv_nanosvg_decoder_open");
            return LV_RESULT_INVALID;
        }

        /*The adjusted draw buffer is newly allocated.*/
        if(adjusted != decoded) {
            lv_draw_buf_destroy(decoded);
            decoded = adjusted;
        }

        dsc->decoded = decoded;

        if(dsc->args.no_cache) {
            LV_PROFILER_DECODER_END_TAG("lv_nanosvg_decoder_open");
            return LV_RESULT_OK;
        }

        /*If the image cache is disabled, just return the decoded image*/
        if(!lv_image_cache_is_enabled()) {
            LV_PROFILER_DECODER_END_TAG("lv_nanosvg_decoder_open");
            return LV_RESULT_OK;
        }

        /*Add the decoded image to the cache*/
        lv_image_cache_data_t search_key;
        search_key.src_type = dsc->src_type;
        search_key.src = dsc->src;
        search_key.slot.size = decoded->data_size;

        lv_cache_entry_t * entry = lv_image_decoder_add_to_cache(decoder, &search_key, decoded, NULL);

        if(entry == NULL) {
            LV_PROFILER_DECODER_END_TAG("lv_nanosvg_decoder_open");
            return LV_RESULT_INVALID;
        }
        dsc->cache_entry = entry;

        LV_PROFILER_DECODER_END_TAG("lv_nanosvg_decoder_open");
        return LV_RESULT_OK;    /*If not returned earlier then it failed*/
    }
    /* SVG file as data not supported for simplicity.
     * Convert them to LVGL compatible C arrays directly. */
    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_INVALID;    /*If not returned earlier then it failed*/
}

/**
 * Free the allocated resources
 */
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    if(dsc->args.no_cache ||
       !lv_image_cache_is_enabled()) lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
}

static lv_draw_buf_t * decode_svg_data(NSVGimage * nsvgimage)
{
    lv_draw_buf_t * decoded = NULL;
    NSVGrasterizer * rast = NULL;
    int w, h;

    rast = nsvgCreateRasterizer();
    w = (int)nsvgimage->width;
    h = (int)nsvgimage->height;

    decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, w, h, LV_COLOR_FORMAT_ARGB8888,
                                    LV_STRIDE_AUTO);

    nsvgRasterize(rast, nsvgimage, 0, 0, 1, decoded->data, w, h, w * 4);

    nsvgDeleteRasterizer(rast);
    convert_color_depth(decoded->data, w * h);

    return decoded;
}

static void convert_color_depth(uint8_t * img_p, uint32_t px_cnt)
{
    lv_color32_t * img_argb = (lv_color32_t *)img_p;
    uint32_t i;
    for(i = 0; i < px_cnt; i++) {
        uint8_t blue = img_argb[i].blue;
        img_argb[i].blue = img_argb[i].red;
        img_argb[i].red = blue;
    }
}

#endif /*LV_USE_NANOSVG*/
