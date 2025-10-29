/**
 * @file lv_gif.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gif.h"
#if LV_USE_GIF
#include "../../misc/lv_timer_private.h"
#include "../../misc/cache/lv_cache.h"
#include "../../core/lv_obj_class_private.h"
#include "../../widgets/image/lv_image_private.h"
#include "AnimatedGIF/src/AnimatedGIF.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_gif_class)

/**********************
 *      TYPEDEFS
 **********************/

/* the type of the AnimatedGIF pallete type passed to `GIF_begin` */
typedef unsigned char animatedgif_color_format_t;

typedef struct {
    lv_image_t img;
    GIFIMAGE gif;
    const void * src;
    lv_color_format_t color_format;
    lv_timer_t * timer;
    lv_draw_buf_t * draw_buf;
    int32_t loop_count;
    uint32_t is_open : 1;
} lv_gif_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_gif_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_gif_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void draw_raw_cb(GIFDRAW * pDraw);
static void initialize(lv_gif_t * gifobj);
static void next_frame_task_cb(lv_timer_t * t);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_gif_class = {
    .constructor_cb = lv_gif_constructor,
    .destructor_cb = lv_gif_destructor,
    .instance_size = sizeof(lv_gif_t),
    .base_class = &lv_image_class,
    .name = "lv_gif",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_gif_create(lv_obj_t * parent)
{

    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_gif_set_color_format(lv_obj_t * obj, lv_color_format_t color_format)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    if(gifobj->color_format == color_format) {
        return;
    }

    switch(color_format) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB565_SWAPPED:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_ARGB8888:
            break;
        default:
            LV_LOG_WARN("gif widget does not support this color format");
            return;
    }

    gifobj->color_format = color_format;

    if(gifobj->src != NULL) {
        initialize(gifobj);
    }
}

void lv_gif_set_src(lv_obj_t * obj, const void * src)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    gifobj->src = src;

    initialize(gifobj);
}

void lv_gif_restart(lv_obj_t * obj)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    if(!gifobj->is_open) {
        LV_LOG_WARN("Gif resource not loaded correctly");
        return;
    }

    GIF_reset(&gifobj->gif);
    gifobj->loop_count = -1; /* match the behavior of the old library */
    lv_timer_resume(gifobj->timer);
    lv_timer_reset(gifobj->timer);
}

void lv_gif_pause(lv_obj_t * obj)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;
    lv_timer_pause(gifobj->timer);
}

void lv_gif_resume(lv_obj_t * obj)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    if(!gifobj->is_open) {
        LV_LOG_WARN("Gif resource not loaded correctly");
        return;
    }

    lv_timer_resume(gifobj->timer);
}

bool lv_gif_is_loaded(lv_obj_t * obj)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    return gifobj->is_open;
}

int32_t lv_gif_get_loop_count(lv_obj_t * obj)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    if(!gifobj->is_open) {
        return -1;
    }

    return gifobj->loop_count;
}

void lv_gif_set_loop_count(lv_obj_t * obj, int32_t count)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    if(!gifobj->is_open) {
        LV_LOG_WARN("Gif resource not loaded correctly");
        return;
    }

    gifobj->loop_count = count;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_gif_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_gif_t * gifobj = (lv_gif_t *) obj;

    gifobj->color_format = LV_COLOR_FORMAT_ARGB8888;
    gifobj->is_open = 0;
    gifobj->timer = lv_timer_create(next_frame_task_cb, 10, obj);
    lv_timer_pause(gifobj->timer);
}

static void lv_gif_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    lv_image_cache_drop(lv_image_get_src(obj));

    if(gifobj->is_open) {
        GIF_close(&gifobj->gif);
        lv_draw_buf_destroy(gifobj->draw_buf);
    }
    lv_timer_delete(gifobj->timer);
}

static inline void lv_gif_blend_to_rgb565(GIFDRAW * pDraw, lv_draw_buf_t * draw_buf)
{
    uint8_t pixel;
    uint8_t * src = pDraw->pPixels;
    uint8_t * end = src + pDraw->iWidth;
    uint16_t * pal = pDraw->pPalette;
    uint16_t * dst = (uint16_t *)((uint8_t *)draw_buf->data + ((pDraw->iY + pDraw->y) * draw_buf->header.stride + pDraw->iX
                                                               * 2));

    if(pDraw->ucHasTransparency) {
        if(pDraw->ucDisposalMethod == 2) {
            while(src < end) {
                pixel = *src++;
                if(pixel == pDraw->ucTransparent) {
                    pixel = pDraw->ucBackground;
                }
                *dst++ = pal[pixel];
            }
        }
        else {
            while(src < end) {
                pixel = *src++;
                if(pixel != pDraw->ucTransparent) {
                    *dst = pal[pixel];
                }
                dst++;
            }
        }
    }
    else {
        while(src < end) {
            pixel = *src++;
            *dst++ = pal[pixel];
        }
    }
}

static inline void lv_gif_blend_to_rgb888(GIFDRAW * pDraw, lv_draw_buf_t * draw_buf)
{
    uint8_t pixel;
    uint8_t * src = pDraw->pPixels;
    uint8_t * end = src + pDraw->iWidth;
    uint8_t * pal = pDraw->pPalette24;
    uint8_t * dst = (uint8_t *)draw_buf->data + ((pDraw->iY + pDraw->y) * draw_buf->header.stride + pDraw->iX * 3);

    if(pDraw->ucHasTransparency) {
        if(pDraw->ucDisposalMethod == 2) {
            while(src < end) {
                pixel = *src++;
                if(pixel == pDraw->ucTransparent) {
                    pixel = pDraw->ucBackground;
                }
                dst[0] = pal[(pixel * 3) + 2];
                dst[1] = pal[(pixel * 3) + 1];
                dst[2] = pal[(pixel * 3) + 0];
                dst += 3;
            }
        }
        else {
            while(src < end) {
                pixel = *src++;
                if(pixel != pDraw->ucTransparent) {
                    dst[0] = pal[(pixel * 3) + 2];
                    dst[1] = pal[(pixel * 3) + 1];
                    dst[2] = pal[(pixel * 3) + 0];
                }
                dst += 3;
            }
        }
    }
    else {
        while(src < end) {
            pixel = *src++;
            dst[0] = pal[(pixel * 3) + 2];
            dst[1] = pal[(pixel * 3) + 1];
            dst[2] = pal[(pixel * 3) + 0];
            dst += 3;
        }
    }
}

static inline void lv_gif_blend_to_argb8888(GIFDRAW * pDraw, lv_draw_buf_t * draw_buf)
{
    uint8_t pixel;
    uint8_t * src = pDraw->pPixels;
    uint8_t * end = src + pDraw->iWidth;
    uint8_t * pal = pDraw->pPalette24;
    uint8_t * dst = (uint8_t *)draw_buf->data + ((pDraw->iY + pDraw->y) * draw_buf->header.stride + pDraw->iX * 4);

    if(pDraw->ucHasTransparency) {
        if(pDraw->ucDisposalMethod == 2) {
            while(src < end) {
                pixel = *src++;
                if(pixel != pDraw->ucTransparent) {
                    dst[0] = pal[(pixel * 3) + 2];
                    dst[1] = pal[(pixel * 3) + 1];
                    dst[2] = pal[(pixel * 3) + 0];
                    dst[3] = 0xFF;
                }
                else {
                    dst[3] = 0x00;
                }
                dst += 4;
            }
        }
        else {
            while(src < end) {
                pixel = *src++;
                if(pixel != pDraw->ucTransparent) {
                    dst[0] = pal[(pixel * 3) + 2];
                    dst[1] = pal[(pixel * 3) + 1];
                    dst[2] = pal[(pixel * 3) + 0];
                    dst[3] = 0xFF;
                }
                dst += 4;
            }
        }
    }
    else {
        while(src < end) {
            pixel = *src++;
            dst[0] = pal[(pixel * 3) + 2];
            dst[1] = pal[(pixel * 3) + 1];
            dst[2] = pal[(pixel * 3) + 0];
            dst[3] = 0xFF;
            dst += 4;
        }
    }
}

static void draw_raw_cb(GIFDRAW * pDraw)
{
    lv_gif_t * gifobj = (lv_gif_t *) pDraw->pUser;

    switch(pDraw->ucPaletteType) {
        case GIF_PALETTE_RGB565_LE:
        case GIF_PALETTE_RGB565_BE:
            lv_gif_blend_to_rgb565(pDraw, gifobj->draw_buf);
            break;
        case GIF_PALETTE_RGB888:
            lv_gif_blend_to_rgb888(pDraw, gifobj->draw_buf);
            break;
        case GIF_PALETTE_RGB8888:
            lv_gif_blend_to_argb8888(pDraw, gifobj->draw_buf);
            break;
    }
}

static void initialize(lv_gif_t * gifobj)
{
    GIFIMAGE * gif = &gifobj->gif;

    /*Close previous gif if any*/
    if(gifobj->is_open) {
        lv_image_cache_drop(lv_image_get_src((lv_obj_t *) gifobj));
        GIF_close(gif);
        lv_draw_buf_destroy(gifobj->draw_buf);
        gifobj->draw_buf = NULL;
        gifobj->is_open = 0;
    }

    animatedgif_color_format_t decoder_cf;
    switch(gifobj->color_format) {
        case LV_COLOR_FORMAT_RGB565:
            decoder_cf = GIF_PALETTE_RGB565_LE;
            break;
        case LV_COLOR_FORMAT_RGB565_SWAPPED:
            decoder_cf = GIF_PALETTE_RGB565_BE;
            break;
        case LV_COLOR_FORMAT_RGB888:
            decoder_cf = GIF_PALETTE_RGB888;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            decoder_cf = GIF_PALETTE_RGB8888;
            break;
        default:
            return;
    }

    GIF_begin(gif, decoder_cf);

    if(lv_image_src_get_type(gifobj->src) == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = gifobj->src;
        gifobj->is_open = GIF_openRAM(gif, (uint8_t *) img_dsc->data, img_dsc->data_size, draw_raw_cb);
    }
    else if(lv_image_src_get_type(gifobj->src) == LV_IMAGE_SRC_FILE) {
        gifobj->is_open = GIF_openFile(gif, gifobj->src, draw_raw_cb);
    }
    if(gifobj->is_open == 0) {
        LV_LOG_WARN("Couldn't load the source");
        return;
    }

    uint32_t width = GIF_getCanvasWidth(gif);
    uint32_t height = GIF_getCanvasHeight(gif);
    gif->ucDrawType = GIF_DRAW_RAW;

    gifobj->draw_buf = lv_draw_buf_create(width, height, gifobj->color_format, LV_STRIDE_AUTO);

    if(gifobj->draw_buf == NULL) {
        LV_LOG_WARN("Couldn't allocate memory for the gif with width: %d and height: %d", width, height);
        GIF_close(gif);
        gifobj->is_open = 0;
        return;
    }

    lv_image_set_src((lv_obj_t *) gifobj, gifobj->draw_buf);

    gifobj->loop_count = GIF_getLoopCount(&gifobj->gif);

    lv_timer_resume(gifobj->timer);
    lv_timer_reset(gifobj->timer);

    next_frame_task_cb(gifobj->timer);

}

static void next_frame_task_cb(lv_timer_t * t)
{
    lv_obj_t * obj = t->user_data;
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    int ms_delay_next;
    int has_next = GIF_playFrame(&gifobj->gif, &ms_delay_next, gifobj);
    if(has_next <= 0) {
        /*It was the last repeat*/
        lv_result_t res = lv_obj_send_event(obj, LV_EVENT_READY, NULL);
        if(gifobj->loop_count > 0) {
            if(gifobj->loop_count == 1) {
                lv_timer_pause(t);
            }
            else {
                gifobj->loop_count--;
            }
        }
        if(res != LV_RESULT_OK) return;
    }
    else {
        lv_timer_set_period(gifobj->timer, ms_delay_next);
    }

    lv_image_cache_drop(lv_image_get_src(obj));
    lv_obj_invalidate(obj);
}

#endif /*LV_USE_GIF*/
