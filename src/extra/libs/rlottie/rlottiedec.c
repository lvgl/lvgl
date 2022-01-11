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
    uint32_t * allocated_buf;
    lv_coord_t lines_in_buf;
    size_t scanline_width;
    lv_coord_t top;
    size_t last_rendered_frame;
} lv_rlottie_dec_context_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t decoder_accept(const lv_img_src_uri_t * src, uint8_t * caps);
static lv_res_t decoder_open(lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags);


static lv_res_t decoder_read_line(lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

static void decoder_close(lv_img_decoder_dsc_t * dsc);

static void set_caps(uint8_t * caps);
static lv_res_t init_dec_ctx(rlottiedec_ctx_t * dec_ctx);
static lv_res_t render_animation(lv_rlottie_dec_context_t * context_ctx, rlottiedec_ctx_t * dec_ctx, lv_coord_t w,
                                 lv_coord_t h);

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
    lv_img_decoder_set_accept_cb(dec, decoder_accept);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_read_line_cb(dec, decoder_read_line);
    lv_img_decoder_set_close_cb(dec, decoder_close);
    rlottiedec_init = 1;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
static void set_caps(uint8_t * caps)
{
    if(caps != NULL) *caps = LV_IMG_DEC_VECTOR | LV_IMG_DEC_ANIMATED | LV_IMG_DEC_SEEKABLE;
}

static lv_res_t init_dec_ctx(rlottiedec_ctx_t * dec_ctx)
{
    if(dec_ctx != NULL) {
        set_caps(&dec_ctx->ctx.caps);
        if(!dec_ctx->max_buf_size)
            dec_ctx->max_buf_size = RLOTTIE_MAX_BUFSIZE;
    }
    return LV_RES_OK;
}
/**
 * Get info about a rlottie image
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RES_OK: no error; LV_RES_INV: can't get the info
 */
static lv_res_t decoder_accept(const lv_img_src_uri_t * src, uint8_t * caps)
{
    /*If it's a rlottie json file...*/
    if(src->type == LV_IMG_SRC_FILE) {
        if(!strncmp(src->ext, ".json", 5)) {              /*Check the extension*/
            set_caps(caps);
            return LV_RES_OK;
        }
    }
    /* rlottie as raw data */
    else if(src->type == LV_IMG_SRC_VARIABLE) {
        const char * str = (const char *)src->uri;
        if(src->uri_len > 2 && str[0] == '{' && str[src->uri_len - 1] == '}') {  /*Is JSON*/
            set_caps(caps);
            return LV_RES_OK;
        }
    }
    return LV_RES_INV;
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


static lv_res_t render_animation(lv_rlottie_dec_context_t * context, rlottiedec_ctx_t * dec_ctx, lv_coord_t w,
                                 lv_coord_t h)
{
    lottie_animation_render_partial(
        dec_ctx->cache,
        dec_ctx->ctx.current_frame,
        context->allocated_buf,
        w,
        h,
        context->top,
        context->top + context->lines_in_buf,
        context->scanline_width
    );
    context->last_rendered_frame = dec_ctx->ctx.current_frame;
#if LV_COLOR_DEPTH == 16
    convert_to_rgba5658(context->allocated_buf, (uint8_t *)context->allocated_buf, w,
                        context->lines_in_buf);
#elif LV_COLOR_DEPTH < 16
    /*todo: Handle other color depth here */
    return LV_RES_INV;
#endif
    return LV_RES_OK;
}

/**
 * Open a rlottie animation image and return the decoded image
 * @param dsc Decoded descriptor for the animation
 * @return LV_RES_OK: no error; LV_RES_INV: can't decode the picture
 */
static lv_res_t decoder_open(lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags)
{
    rlottiedec_ctx_t * dec_ctx = (rlottiedec_ctx_t *)dsc->out.dec_ctx;
    dsc->out.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;


    /* Already exist ? Reuse */
    if(dec_ctx != NULL && dec_ctx->ctx.user_data != NULL) {
        if(dsc->out.header.w == LV_SIZE_CONTENT || !dsc->out.header.w || dsc->out.header.h == LV_SIZE_CONTENT ||
           !dsc->out.header.h) {
            dsc->out.header.w = dsc->in.size_hint.x;
            dsc->out.header.h = dsc->in.size_hint.y;
        }
        set_caps(&dec_ctx->ctx.caps);
        /* If only the frame index changed and we are rendering to the internal buffer,
           let's skip everything and render directly */
        if((dsc->out.header.w * LV_IMG_PX_SIZE_ALPHA_BYTE) * dsc->out.header.h <= dec_ctx->max_buf_size) {
            dec_ctx->ctx.caps |= LV_IMG_DEC_CACHED;
            lv_rlottie_dec_context_t * context = (lv_rlottie_dec_context_t *)dec_ctx->ctx.user_data;
            if(dec_ctx != NULL && dec_ctx->ctx.current_frame != context->last_rendered_frame) {
                /*Shortcut for re-rendering the animation for this new frame*/
                if(render_animation(context, dec_ctx, dsc->out.header.w, dsc->out.header.h) != LV_RES_OK) {
                    return LV_RES_INV;
                }
            }
        }
        return LV_RES_OK;
    }
    else if(flags != LV_IMG_DEC_ONLYMETA) {
        if(dec_ctx == NULL) {
            /* Doesn't exist, let's allocate a context */
            LV_ZALLOC(dsc->out.dec_ctx, sizeof(*dec_ctx));
            dec_ctx = (rlottiedec_ctx_t *)dsc->out.dec_ctx;
            dec_ctx->ctx.auto_allocated = 1;
        }
        if(!dec_ctx || init_dec_ctx(dec_ctx) != LV_RES_OK)
            return LV_RES_INV;
    }

    Lottie_Animation * animation = dec_ctx ? dec_ctx->cache : NULL;
    size_t w = 0, h = 0;

    if(animation == NULL) {
        /*If it's a rlottie json file...*/
        if(dsc->in.src->type == LV_IMG_SRC_FILE) {
            if(!strncmp(dsc->in.src->ext, ".json", 5)) {              /*Check the extension*/
                animation = lottie_animation_from_file(dsc->in.src->uri);
            }
        }
        /* rlottie as raw data */
        else if(dsc->in.src->type == LV_IMG_SRC_VARIABLE) {
            animation = lottie_animation_from_rodata((const char *)dsc->in.src->uri, dsc->in.src->uri_len, "");
        }
        if(animation == NULL && dsc->out.dec_ctx->auto_allocated == 1) {
            lv_mem_free(dec_ctx);
            dsc->out.dec_ctx = 0;
            return LV_RES_INV;
        }

        lottie_animation_get_size(animation, &w, &h);
        dec_ctx->ctx.frame_rate   = lottie_animation_get_framerate(animation);
        dec_ctx->ctx.total_frames = lottie_animation_get_totalframe(animation);
        dec_ctx->ctx.dest_frame   = dec_ctx->ctx.total_frames; /* Mark it invalid on construction */

        if(lv_img_decoder_has_size_hint(&dsc->in)) {
            /*Deduce aspect ratio if one coordinate is to guess*/
            if(dsc->in.size_hint.y == LV_SIZE_CONTENT) dsc->in.size_hint.y = (h * dsc->in.size_hint.x) / w;
            if(dsc->in.size_hint.x == LV_SIZE_CONTENT) dsc->in.size_hint.x = (w * dsc->in.size_hint.y) / h;
            dsc->out.header.w = (uint32_t)dsc->in.size_hint.x;
            dsc->out.header.h = (uint32_t)dsc->in.size_hint.y;
            if(dec_ctx) {
                if(flags != LV_IMG_DEC_ONLYMETA) dec_ctx->cache = animation;
                /*Does the picture fit in the decoder context buffer entirely?*/
                if((dsc->out.header.w * LV_IMG_PX_SIZE_ALPHA_BYTE) * dsc->out.header.h <= dec_ctx->max_buf_size) {
                    dec_ctx->ctx.caps |= LV_IMG_DEC_CACHED;
                }
            }
        }
        else {
            dsc->out.header.w = (uint32_t)w;
            dsc->out.header.h = (uint32_t)h;
        }

        if(flags == LV_IMG_DEC_ONLYMETA) {
            /*If the size wasn't given in, it's unlikely it'll be re-used later, so clean it now*/
            lottie_animation_destroy(animation);

            return LV_RES_OK;
        }
    }

    /*If already opened (in image cache), reuse it */
    if(dsc->out.dec_ctx->user_data != NULL) {
        lv_rlottie_dec_context_t * context = (lv_rlottie_dec_context_t *)dsc->out.dec_ctx->user_data;
        /*Check we need to re-create the context*/
        if(dec_ctx == NULL || dsc->in.size_hint.x != dsc->out.header.w || dsc->in.size_hint.y != dsc->out.header.h) {
            lv_mem_free(context->allocated_buf);
            lv_mem_free(context);
        }
        else if(dec_ctx != NULL && dec_ctx->ctx.current_frame != context->last_rendered_frame
                && (dec_ctx->ctx.caps & LV_IMG_DEC_CACHED) == LV_IMG_DEC_CACHED) {
            /*Shortcut for re-rendering the animation for this new frame*/
            if(render_animation(context, dec_ctx, dsc->out.header.w, dsc->out.header.h) != LV_RES_OK) {
                return LV_RES_INV;
            }
            return LV_RES_OK;
        }
    }

    lv_rlottie_dec_context_t * context = 0;
    LV_ZALLOC(context, sizeof(lv_rlottie_dec_context_t));

    dec_ctx->ctx.user_data = context;
    dec_ctx->ctx.total_frames = lottie_animation_get_totalframe(animation);
    dec_ctx->ctx.frame_rate = lottie_animation_get_framerate(animation);
    dec_ctx->cache = animation;
    w = dsc->out.header.w;
    h = dsc->out.header.h;
    dsc->out.dec_ctx = (lv_img_dec_ctx_t *)dec_ctx;


    /* Compute how many lines we can fit in the maximum buffer size */
    context->scanline_width = w * LV_ARGB32 / 8;
    context->lines_in_buf = dec_ctx->max_buf_size / context->scanline_width;
    if(context->lines_in_buf != 0) {  /*Too big picture to fit the maximum buffer size (default to 1024px) ?*/
        context->allocated_buf = lv_mem_alloc(context->lines_in_buf * context->scanline_width);
        LV_ASSERT_MALLOC(context->allocated_buf);
    }
    if(context->allocated_buf == NULL) {
        if(dec_ctx->ctx.auto_allocated) {
            lottie_animation_destroy(animation);
            dec_ctx->cache = 0;
            lv_mem_free(dec_ctx);
        }
        return LV_RES_INV;
    }

    memset(context->allocated_buf, 0, context->lines_in_buf * context->scanline_width);
    context->last_rendered_frame = lottie_animation_get_totalframe(animation);
    context->top = 0;
    dsc->out.img_data = NULL; /* We want the renderer to call decoder_read_line, even if cached */
    dsc->out.header.always_zero = 0;
    dsc->out.header.w = (uint32_t)w;
    dsc->out.header.h = (uint32_t)h;
    dsc->out.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
    dsc->out.dec_ctx->user_data = context;

    /*Does the picture fit in the decoder context buffer entirely?*/
    if((w * LV_IMG_PX_SIZE_ALPHA_BYTE) * h <= dec_ctx->max_buf_size) {
        dec_ctx->ctx.caps |= LV_IMG_DEC_CACHED;
        /*Render the animation directly here*/
        dsc->out.img_data = (const uint8_t *)context->allocated_buf;
        if(render_animation(context, dec_ctx, dsc->out.header.w, dsc->out.header.h) != LV_RES_OK) {
            return LV_RES_INV;
        }
    }

    return LV_RES_OK;
}



static lv_res_t decoder_read_line(lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    rlottiedec_ctx_t * dec_ctx = (rlottiedec_ctx_t *)dsc->out.dec_ctx;
    lv_rlottie_dec_context_t * context = (lv_rlottie_dec_context_t *)dec_ctx->ctx.user_data;
    if(dec_ctx == NULL || dec_ctx->cache == NULL) {
        return LV_RES_INV;
    }
    /* Check if we already have the right line in our internal buffer */
    if(context->last_rendered_frame != dec_ctx->ctx.current_frame
       ||  context->top > y || (context->top + context->lines_in_buf) <= y) {
        context->top = y;
        /* rlottie does not clip invalid coordinate, let's do it here */
        if(context->top + context->lines_in_buf > dsc->out.header.h) {
            context->top = dsc->out.header.h - context->lines_in_buf;
        }
        if(render_animation(context, dec_ctx, dsc->out.header.w, dsc->out.header.h) != LV_RES_OK) {
            return LV_RES_INV;
        }
    }

    /* Then copy to the output buffer now */
#if LV_COLOR_DEPTH == 32
    lv_memcpy(buf, &context->allocated_buf[x + (y - context->top) * dsc->out.header.w], len * (LV_ARGB32 / 8));
#elif LV_COLOR_DEPTH == 16
    uint8_t * buf_start = ((uint8_t *)context->allocated_buf) + (x + (y - context->top) * dsc->out.header.w) *
                          LV_IMG_PX_SIZE_ALPHA_BYTE;
    lv_memcpy(buf, buf_start, len * LV_IMG_PX_SIZE_ALPHA_BYTE);
#endif

    return LV_RES_OK;
}


/**
 * Free the allocated resources
 */
static void decoder_close(lv_img_decoder_dsc_t * dsc)
{
    rlottiedec_ctx_t * dec_ctx = (rlottiedec_ctx_t *)dsc->out.dec_ctx;
    if(dec_ctx && dec_ctx->ctx.auto_allocated) {
        /*Only free if allocated by ourselves.*/
        lv_rlottie_dec_context_t * context = (lv_rlottie_dec_context_t *)dec_ctx->ctx.user_data;
        context->scanline_width = 0;
        lv_mem_free(context->allocated_buf);
        context->allocated_buf = 0;
        lottie_animation_destroy(dec_ctx->cache);
        dec_ctx->cache = 0;
        lv_mem_free(dec_ctx);
        /*Unlink the decoder context*/
        lv_mem_free(context);
        dec_ctx->ctx.user_data = 0;
    }
    dsc->out.dec_ctx = 0;
}

#endif /*LV_USE_RLOTTIE*/
