/**
 * @file lv_gifenc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gif_private.h"
#if LV_USE_GIF
#include "../../misc/lv_timer_private.h"
#include "../../misc/cache/lv_image_cache.h"
#include "../../core/lv_obj_class_private.h"

#include "gifdec.h"

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
    .name = "gif",
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
    gd_GIF * gif = gifobj->gif;

    /*Close previous gif if any*/
    if(gif != NULL) {
        lv_image_cache_drop(lv_image_get_src(obj));

        gd_close_gif(gif);
        gifobj->gif = NULL;
        gifobj->imgdsc.data = NULL;
    }

    if(lv_image_src_get_type(src) == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        gif = gd_open_gif_data(img_dsc->data);
    }
    else if(lv_image_src_get_type(src) == LV_IMAGE_SRC_FILE) {
        gif = gd_open_gif_file(src);
    }
    if(gif == NULL) {
        LV_LOG_WARN("Couldn't load the source");
        return;
    }

    gifobj->gif = gif;
    gifobj->imgdsc.data = gif->canvas;
    gifobj->imgdsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    gifobj->imgdsc.header.flags = LV_IMAGE_FLAGS_MODIFIABLE;
    gifobj->imgdsc.header.cf = LV_COLOR_FORMAT_ARGB8888;
    gifobj->imgdsc.header.h = gif->height;
    gifobj->imgdsc.header.w = gif->width;
    gifobj->imgdsc.header.stride = gif->width * 4;
    gifobj->imgdsc.data_size = gif->width * gif->height * 4;

    gifobj->last_call = lv_tick_get();

    lv_image_set_src(obj, &gifobj->imgdsc);

    lv_timer_resume(gifobj->timer);
    lv_timer_reset(gifobj->timer);

    next_frame_task_cb(gifobj->timer);

}

void lv_gif_restart(lv_obj_t * obj)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    if(gifobj->gif == NULL) {
        LV_LOG_WARN("Gif resource not loaded correctly");
        return;
    }

    gd_rewind(gifobj->gif);
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

    if(gifobj->gif == NULL) {
        LV_LOG_WARN("Gif resource not loaded correctly");
        return;
    }

    lv_timer_resume(gifobj->timer);
}

bool lv_gif_is_loaded(lv_obj_t * obj)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    return (gifobj->gif != NULL);
}

int32_t lv_gif_get_loop_count(lv_obj_t * obj)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    if(gifobj->gif == NULL) {
        return -1;
    }

    return gifobj->gif->loop_count;
}

void lv_gif_set_loop_count(lv_obj_t * obj, int32_t count)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    if(gifobj->gif == NULL) {
        LV_LOG_WARN("Gif resource not loaded correctly");
        return;
    }

    gifobj->gif->loop_count = count;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_gif_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_gif_t * gifobj = (lv_gif_t *) obj;

    gifobj->gif = NULL;
    gifobj->timer = lv_timer_create(next_frame_task_cb, 10, obj);
    lv_timer_pause(gifobj->timer);
}

static void lv_gif_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_gif_t * gifobj = (lv_gif_t *) obj;

    lv_image_cache_drop(lv_image_get_src(obj));

    if(gifobj->gif)
        gd_close_gif(gifobj->gif);
    lv_timer_delete(gifobj->timer);
}

static void next_frame_task_cb(lv_timer_t * t)
{
    lv_obj_t * obj = t->user_data;
    lv_gif_t * gifobj = (lv_gif_t *) obj;
    uint32_t elaps = lv_tick_elaps(gifobj->last_call);
    if(elaps < gifobj->gif->gce.delay * 10) return;

    gifobj->last_call = lv_tick_get();

    int has_next = gd_get_frame(gifobj->gif);
    if(has_next == 0) {
        /*It was the last repeat*/
        lv_result_t res = lv_obj_send_event(obj, LV_EVENT_READY, NULL);
        lv_timer_pause(t);
        if(res != LV_RESULT_OK) return;
    }

    gd_render_frame(gifobj->gif, (uint8_t *)gifobj->imgdsc.data);

    lv_image_cache_drop(lv_image_get_src(obj));
    lv_obj_invalidate(obj);
}

#endif /*LV_USE_GIF*/
