/**
 * @file lv_gifenc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gif.h"
#if LV_USE_GIF

#include "gifdec.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS    &lv_gif_class

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
    .base_class = &lv_img_class
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

    /*Close previous gif if any*/
    if(gifobj->gif) {
        lv_img_cache_invalidate_src(&gifobj->imgdsc);
        gd_close_gif(gifobj->gif);
        gifobj->gif = NULL;
        gifobj->imgdsc.data = NULL;
    }

    if(lv_img_src_get_type(src) == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = src;
       gifobj->gif = gd_open_gif_data(img_dsc->data);
    }
    else if(lv_img_src_get_type(src) == LV_IMG_SRC_FILE) {
       gifobj->gif = gd_open_gif_file(src);
    }
    if(gifobj->gif == NULL) {
        LV_LOG_WARN("Could't load the source");
        return;
    }

   gifobj->imgdsc.data = gifobj->gif->canvas;
   gifobj->imgdsc.header.always_zero = 0;
   gifobj->imgdsc.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
   gifobj->imgdsc.header.h = gifobj->gif->height;
   gifobj->imgdsc.header.w = gifobj->gif->width;
   gifobj->last_call = lv_tick_get();

   lv_img_set_src(obj, &gifobj->imgdsc);

   lv_timer_resume(gifobj->timer);
   lv_timer_reset(gifobj->timer);

   next_frame_task_cb(gifobj->timer);

}

void lv_gif_restart(lv_obj_t * obj)
{
    lv_gif_t * gifobj = (lv_gif_t *) obj;
    gd_rewind(gifobj->gif);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_gif_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_gif_t * gifobj = (lv_gif_t *) obj;

    gifobj->timer = lv_timer_create(next_frame_task_cb, 10, obj);
    lv_timer_pause(gifobj->timer);
}

static void lv_gif_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_gif_t * gifobj = (lv_gif_t *) obj;
    lv_img_cache_invalidate_src(&gifobj->imgdsc);
    gd_close_gif(gifobj->gif);
    lv_timer_del(gifobj->timer);
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
        if(gifobj->gif->loop_count == 1) {
            lv_res_t res = lv_event_send(obj, LV_EVENT_READY, NULL);
            if(res != LV_FS_RES_OK) return;
        } else {
            if(gifobj->gif->loop_count > 1)  gifobj->gif->loop_count--;
            gd_rewind(gifobj->gif);
        }
    }

    gd_render_frame(gifobj->gif, (uint8_t *)gifobj->imgdsc.data);

    lv_img_cache_invalidate_src(lv_img_get_src(obj));
    lv_obj_invalidate(obj);
}

#endif /*LV_USE_GIF*/
