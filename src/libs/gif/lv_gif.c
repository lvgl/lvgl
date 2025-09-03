/**
 * @file lv_gif.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gif_private.h"
#if LV_USE_GIF
#include "../../misc/lv_timer_private.h"
#include "../../misc/cache/lv_cache.h"
#include "../../core/lv_obj_class_private.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_gif_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_gif_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_gif_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
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

void lv_gif_set_src(lv_obj_t * obj, const void * src)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;
    GIFIMAGE * gif = &gifobj->gif;

    /*Close previous gif if any*/
    if(gifobj->is_open) {
        lv_image_cache_drop(lv_image_get_src(obj));

        void * framebuffer = gif->pFrameBuffer;
        GIF_close(gif);
        lv_free(framebuffer);
        gifobj->is_open = 0;
        gifobj->imgdsc.data = NULL;
    }

    GIF_begin(gif, GIF_PALETTE_RGB565_LE);

    if(lv_image_src_get_type(src) == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        gifobj->is_open = GIF_openRAM(gif, (uint8_t *) img_dsc->data, img_dsc->data_size, NULL);
    }
    else if(lv_image_src_get_type(src) == LV_IMAGE_SRC_FILE) {
        gifobj->is_open = GIF_openFile(gif, src, NULL);
    }
    if(gifobj->is_open == 0) {
        LV_LOG_WARN("Couldn't load the source");
        return;
    }

    uint32_t width = GIF_getCanvasWidth(gif);
    uint32_t height = GIF_getCanvasHeight(gif);
    gif->pFrameBuffer = lv_malloc(width * height * 3);
    gif->ucDrawType = GIF_DRAW_COOKED;
    LV_ASSERT_MALLOC(gif->pFrameBuffer);
    if(gif->pFrameBuffer == NULL) {
        LV_LOG_WARN("Couldn't allocate a buffer for a GIF");
        GIF_close(gif);
        gifobj->is_open = 0;
        return;
    }

    gifobj->imgdsc.data = gif->pFrameBuffer + width * height;
    gifobj->imgdsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    gifobj->imgdsc.header.flags = LV_IMAGE_FLAGS_MODIFIABLE;
    gifobj->imgdsc.header.cf = LV_COLOR_FORMAT_RGB565;
    gifobj->imgdsc.header.h = height;
    gifobj->imgdsc.header.w = width;
    gifobj->imgdsc.header.stride = width * 2;
    gifobj->imgdsc.data_size = width * height * 2;

    lv_image_set_src(obj, &gifobj->imgdsc);

    gifobj->loop_count = GIF_getLoopCount(&gifobj->gif);

    lv_timer_resume(gifobj->timer);
    lv_timer_reset(gifobj->timer);

    next_frame_task_cb(gifobj->timer);

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
        void * framebuffer = gifobj->gif.pFrameBuffer;
        GIF_close(&gifobj->gif);
        lv_free(framebuffer);
    }
    lv_timer_delete(gifobj->timer);
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

/**********************
 *   INLINE INCLUDES
 **********************/

#include "AnimatedGIF/src/gif.inl"

#endif /*LV_USE_GIF*/
