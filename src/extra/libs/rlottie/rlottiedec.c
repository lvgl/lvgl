/**
 * @file rlottiedec.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#include <string.h>

#if LV_USE_RLOTTIE
#include "rlottiedec.h"
#include <rlottie_capi.h>





/*********************
 *      DEFINES
 *********************/
#define LV_ARGB32   32
#define RLOTTIE_MAX_BUFSIZE     4096


/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_img_header_t * header;
    uint32_t * allocated_buf;
    size_t lines_in_buf;
    size_t scanline_width;
    size_t top;
    size_t last_rendered_frame;
    rlottiedec_ctx_t * dec_ctx;
} lv_rlottie_dec_context_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t decoder_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header,
                             const void * dec_ctx);
static lv_res_t decoder_open(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc, const void * dec_ctx);


static lv_res_t decoder_read_line(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

static void decoder_close(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);
static size_t max_buf_size = RLOTTIE_MAX_BUFSIZE;
/**********************
 *  STATIC VARIABLES
 **********************/
static int rlottiedec_init = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_rlottie_init(void)
{
    if(rlottiedec_init)
        return;

    lv_img_decoder_t * dec = lv_img_decoder_create();
    lv_img_decoder_set_info_cb(dec, decoder_info);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_read_line_cb(dec, decoder_read_line);
    lv_img_decoder_set_close_cb(dec, decoder_close);
    rlottiedec_init = 1;
}

void lv_rlottie_set_max_buffer_size(size_t size_bytes)
{
    max_buf_size = size_bytes;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Get info about a rlottie image
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RES_OK: no error; LV_RES_INV: can't get the info
 */
static lv_res_t decoder_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header,
                             const void * _dec_ctx)
{
    LV_UNUSED(decoder);

    lv_img_src_t src_type = lv_img_src_get_type(src);          /*Get the source type*/
    rlottiedec_ctx_t * dec_ctx = (rlottiedec_ctx_t *)_dec_ctx;
    Lottie_Animation * animation = NULL;
    size_t w, h;

    if(dec_ctx != NULL && dec_ctx->cache != NULL) {
        /*Already opened, reuse it*/
        header->w = (uint32_t)dec_ctx->create_width;
        header->h = (uint32_t)dec_ctx->create_height;
        header->always_zero = 0;
        header->cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
        return LV_RES_OK;
    }
    /*If it's a rlottie json file...*/
    if(src_type == LV_IMG_SRC_FILE) {
        const char * fn = src;
        if(!strcmp(&fn[strlen(fn) - 4], "json")) {              /*Check the extension*/
            animation = lottie_animation_from_file(src);
        }
    }
    /* rlottie as raw data */
    else if(src_type == LV_IMG_SRC_VARIABLE) {
        animation = lottie_animation_from_data(src, src, "");
    }
    if(animation == NULL)
        return LV_RES_INV;

    lottie_animation_get_size(animation, &w, &h);

    if(dec_ctx != NULL) {
        header->w = (uint32_t)dec_ctx->create_width;
        header->h = (uint32_t)dec_ctx->create_height;
        dec_ctx->total_frames = lottie_animation_get_totalframe(animation);
        dec_ctx->cache = animation;
    }
    else {
        header->w = (uint32_t)w;
        header->h = (uint32_t)h;
        lottie_animation_destroy(animation);
    }
    header->always_zero = 0;
    header->cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
    return LV_RES_OK;
}


/**
 * Open a rlottie animation image and return the decoded image
 * @param dsc Decoded descriptor for the animation
 * @return LV_RES_OK: no error; LV_RES_INV: can't decode the picture
 */
static lv_res_t decoder_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc, const void * _dec_ctx)
{
    LV_UNUSED(decoder);
    rlottiedec_ctx_t * dec_ctx = (rlottiedec_ctx_t *)_dec_ctx;

    Lottie_Animation * animation = NULL;
    size_t w, h;
    /* If already opened, reuse it */
    if(dec_ctx != NULL && dec_ctx->cache != NULL)
        animation = dec_ctx->cache;
    /*If it's a rlottie JSON file...*/
    else if(dsc->src_type == LV_IMG_SRC_FILE) {
        const char * fn = dsc->src;

        if(strcmp(&fn[strlen(fn) - 4], "json")) return LV_RES_INV;       /*Check the extension*/
        animation = lottie_animation_from_file(fn);
    }
    /* rlottie as raw data */
    else if(dsc->src_type == LV_IMG_SRC_VARIABLE) {
        const char * fn = dsc->src;
        animation = lottie_animation_from_data(fn, fn, "");
    }
    if(animation == NULL)
        return LV_RES_INV;

    lv_rlottie_dec_context_t * context = lv_mem_alloc(sizeof(lv_rlottie_dec_context_t));
    if(dec_ctx == NULL) {
        lottie_animation_get_size(animation, &w, &h);
        dec_ctx = lv_mem_alloc(sizeof(rlottiedec_ctx_t));
        dec_ctx->create_width = w;
        dec_ctx->create_height = h;
        dec_ctx->should_free = 1;
    }

    dec_ctx->total_frames = lottie_animation_get_totalframe(animation);
    dec_ctx->cache = animation;
    w = dec_ctx->create_width;
    h = dec_ctx->create_height;
    context->dec_ctx = dec_ctx;

    /* Compute how many lines we can fit in the maximum buffer size */
    context->scanline_width = w * LV_ARGB32 / 8;
    context->lines_in_buf = max_buf_size / context->scanline_width;
    if(context->lines_in_buf != 0) {  /*Too big picture to fit the maximum buffer size (default to 1024px) ?*/
        context->allocated_buf = lv_mem_alloc(context->lines_in_buf * context->scanline_width);
    }
    if(context->allocated_buf == NULL) {
        if(dec_ctx->should_free) {
            lottie_animation_destroy(animation);
            dec_ctx->cache = 0;
            lv_mem_free(dec_ctx);
        }
        return LV_RES_INV;
    }

    memset(context->allocated_buf, 0, context->lines_in_buf * context->scanline_width);
    context->last_rendered_frame = lottie_animation_get_totalframe(animation);
    context->top = 0;
    dsc->user_data = context;
    dsc->decoder = decoder;
    dsc->img_data = NULL; /* We want the renderer to call decoder_read_line */
    dsc->header.always_zero = 0;
    dsc->header.w = (uint32_t)w;
    dsc->header.h = (uint32_t)h;
    dsc->header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
    context->header = &dsc->header;

    return LV_RES_OK;
}

#if LV_COLOR_DEPTH == 16
static void convert_to_rgba5658(uint32_t * pix, uint8_t * dest, const size_t width, const size_t height)
{
    /* rlottie draws in ARGB32 format, but LVGL only deal with RGB565 format with (optional 8 bit alpha channel)
       so convert in place here the received buffer to LVGL format. */
    uint32_t * src = pix;
    for(size_t y = 0; y < height; y++) {
        /* Convert a 4 bytes per pixel in format ARGB to R5G6B5A8 format
            naive way:
                        r = ((c & 0xFF0000) >> 19)
                        g = ((c & 0xFF00) >> 10)
                        b = ((c & 0xFF) >> 3)
                        rgb565 = (r << 11) | (g << 5) | b
                        a = c >> 24;
            That's 3 mask, 6 bitshift and 2 or operations

            A bit better:
                        r = ((c & 0xF80000) >> 8)
                        g = ((c & 0xFC00) >> 5)
                        b = ((c & 0xFF) >> 3)
                        rgb565 = r | g | b
                        a = c >> 24;
            That's 3 mask, 3 bitshifts and 2 or operations */
        for(size_t x = 0; x < width; x++) {
            uint32_t in = src[x];
#if LV_COLOR_16_SWAP == 0
            uint16_t r = (uint16_t)(((in & 0xF80000) >> 8) | ((in & 0xFC00) >> 5) | ((in & 0xFF) >> 3));
#else
            /* We want: rrrr rrrr GGGg gggg bbbb bbbb => gggb bbbb rrrr rGGG */
            uint16_t r = (uint16_t)(((in & 0xF80000) >> 16) | ((in & 0xFC00) >> 13) | ((in & 0x1C00) << 3) | ((in & 0xF8) << 5));
#endif

            lv_memcpy(dest, &r, sizeof(r));
            dest[sizeof(r)] = (uint8_t)(in >> 24);
            dest += LV_IMG_PX_SIZE_ALPHA_BYTE;
        }
        src += width;
    }
}
#endif


static lv_res_t decoder_read_line(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    LV_UNUSED(decoder);

    lv_rlottie_dec_context_t * context = (lv_rlottie_dec_context_t *)dsc->user_data;
    rlottiedec_ctx_t * dec_ctx = (rlottiedec_ctx_t *)context->dec_ctx;
    size_t lines = (len * LV_ARGB32 / 8) / context->scanline_width;
    /* Check if we already have the right line in our internal buffer */
    if(context->last_rendered_frame != dec_ctx->current_frame
       ||  context->top > y || (context->top + context->lines_in_buf) <= y) {
        context->top = y;
        /* rlottie does not clip invalid coordinate, let's do it here */
        if(context->top + context->lines_in_buf > dsc->header.h) {
            context->top = dsc->header.h - context->lines_in_buf;
        }
        /* Need to render partially to the buffer here */
        lottie_animation_render_partial(
            dec_ctx->cache,
            dec_ctx->current_frame,
            context->allocated_buf,
            dsc->header.w,
            dsc->header.h,
            context->top,
            context->top + context->lines_in_buf,
            context->scanline_width
        );
        context->last_rendered_frame = dec_ctx->current_frame;
    }

    /* Then copy to the output buffer now */
#if LV_COLOR_DEPTH == 32
    lv_memcpy(buf, &context->allocated_buf[(x * LV_ARGB32 / 8) + (y - context->top) * context->scanline_width],
              len * (LV_ARGB32 / 8));
#elif LV_COLOR_DEPTH == 16
    uint32_t * buf_start = (uint32_t *)&context->allocated_buf[x + (y - context->top) * dsc->header.w];
    convert_to_rgba5658(buf_start, buf, len, 1);
#else
    /*todo: Handle other color depth here */
    return LV_RES_INV;
#endif

    return LV_RES_OK;
}


/**
 * Free the allocated resources
 */
static void decoder_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    lv_rlottie_dec_context_t * context = (lv_rlottie_dec_context_t *)dsc->user_data;
    rlottiedec_ctx_t * dec_ctx = (rlottiedec_ctx_t *)context->dec_ctx;
    context->scanline_width = 0;
    lv_mem_free(context->allocated_buf);
    context->allocated_buf = 0;
    /*Only free if allocated by ourselves.*/
    if(dec_ctx && dec_ctx->should_free) {
        lottie_animation_destroy(dec_ctx->cache);
        dec_ctx->cache = 0;
        lv_mem_free(dec_ctx);
    }
    context->dec_ctx = 0;

    lv_mem_free(dsc->user_data);
}

#endif /*LV_USE_BMP*/
