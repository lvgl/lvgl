/**
 * @file lv_rlottie.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_rlottie.h"
#if LV_USE_RLOTTIE

#include "rlottiedec.h"
#include <rlottie_capi.h>

/*********************
*      DEFINES
*********************/
#define MY_CLASS &lv_rlottie_class
#define LV_ARGB32   32

/**********************
*      TYPEDEFS
**********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_rlottie_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_rlottie_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void next_frame_task_cb(lv_timer_t * t);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_rlottie_class = {
    .constructor_cb = lv_rlottie_constructor,
    .destructor_cb = lv_rlottie_destructor,
    .instance_size = sizeof(lv_rlottie_t),
    .base_class = &lv_img_class
};


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_rlottie_create_from_file(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * path)
{
    lv_rlottie_init();


    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_rlottie_t * rlottie = (lv_rlottie_t *)obj;
    lv_memset(&rlottie->dec, 0, sizeof(&rlottie->dec));
    rlottie->dec.ctx.magic_id = LV_RLOTTIE_ID;
    rlottie->dec.create_width = width;
    rlottie->dec.create_height = height;
    rlottie->dec.create_src = path;

    lv_obj_class_init_obj(obj);

    return obj;

}

lv_obj_t * lv_rlottie_create_from_raw(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * rlottie_desc)
{
    lv_rlottie_init();

    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_rlottie_t * rlottie = (lv_rlottie_t *)obj;
    lv_memset(&rlottie->dec, 0, sizeof(&rlottie->dec));
    rlottie->dec.ctx.magic_id = LV_RLOTTIE_ID;
    rlottie->dec.create_width = width;
    rlottie->dec.create_height = height;
    rlottie->dec.create_src = rlottie_desc;

    lv_obj_class_init_obj(obj);

    return obj;
}

void lv_rlottie_set_play_mode(lv_obj_t * obj, const lv_rlottie_ctrl_t ctrl)
{
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;
    rlottie->play_ctrl = ctrl;

    if(rlottie->task && (rlottie->dest_frame != rlottie->dec.ctx.current_frame ||
                         (rlottie->play_ctrl & LV_RLOTTIE_CTRL_PAUSE) == LV_RLOTTIE_CTRL_PLAY)) {
        lv_timer_resume(rlottie->task);
    }
}

void lv_rlottie_set_current_frame(lv_obj_t * obj, const uint16_t goto_frame)
{
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;
    rlottie->dec.ctx.current_frame = goto_frame < rlottie->dec.ctx.total_frames ? goto_frame :
                                     (uint16_t)(rlottie->dec.ctx.total_frames - 1);
}

void lv_rlottie_stopat_frame(lv_obj_t * obj, const uint16_t goto_frame, const int forward)
{
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;

    rlottie->dest_frame = goto_frame < rlottie->dec.ctx.total_frames ? goto_frame :
                          (uint16_t)(rlottie->dec.ctx.total_frames - 1);
    rlottie->play_ctrl = LV_RLOTTIE_CTRL_PLAY | LV_RLOTTIE_CTRL_STOPAT | (forward ? LV_RLOTTIE_CTRL_FORWARD :
                                                                          LV_RLOTTIE_CTRL_BACKWARD);
    if(rlottie->task && rlottie->dest_frame != rlottie->dec.ctx.current_frame) {
        lv_timer_resume(rlottie->task);
    }
}

size_t lv_rlottie_get_totalframes(lv_obj_t * obj)
{
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;
    return rlottie->dec.ctx.total_frames;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_rlottie_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;
    lv_img_set_src_ex(obj, rlottie->dec.create_src, (lv_img_dec_ctx_t *)&rlottie->dec);

    if(rlottie->dec.cache == NULL) {
        LV_LOG_WARN("The animation can't be opened");
        return;
    }

    rlottie->framerate = (size_t)lottie_animation_get_framerate(rlottie->dec.cache);

    rlottie->play_ctrl = LV_RLOTTIE_CTRL_FORWARD | LV_RLOTTIE_CTRL_PLAY | LV_RLOTTIE_CTRL_LOOP;
    rlottie->dest_frame =
        rlottie->dec.ctx.total_frames; /* invalid destination frame so it's possible to pause on frame 0 */

    rlottie->task = lv_timer_create(next_frame_task_cb, 1000 / rlottie->framerate, obj);

    lv_obj_update_layout(obj);
}


static void lv_rlottie_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;

    if(rlottie->framerate) {
        rlottie->framerate = 0;
    }

    if(rlottie->task) {
        lv_timer_del(rlottie->task);
        rlottie->task = NULL;
        rlottie->play_ctrl = LV_RLOTTIE_CTRL_FORWARD;
        rlottie->dest_frame = 0;
    }

    lv_img_cache_invalidate_src(rlottie->dec.create_src);
    if(rlottie->dec.cache) {
        lottie_animation_destroy(rlottie->dec.cache);
        rlottie->dec.cache = 0;
    }
}



static void next_frame_task_cb(lv_timer_t * t)
{
    lv_obj_t * obj = t->user_data;
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;

    if((rlottie->play_ctrl & LV_RLOTTIE_CTRL_PAUSE) == LV_RLOTTIE_CTRL_PAUSE) {
        if(rlottie->dec.ctx.current_frame == rlottie->dest_frame) {
            /* Pause the timer too when it has run once to avoid CPU consumption */
            lv_timer_pause(t);
            return;
        }
        rlottie->dest_frame = rlottie->dec.ctx.current_frame;
    }
    else {
        if((rlottie->play_ctrl & LV_RLOTTIE_CTRL_STOPAT) == LV_RLOTTIE_CTRL_STOPAT
           && rlottie->dec.ctx.current_frame == rlottie->dest_frame) {
            lv_timer_pause(t);
            rlottie->play_ctrl ^= LV_RLOTTIE_CTRL_STOPAT | LV_RLOTTIE_CTRL_PAUSE;
            lv_event_send(obj, LV_EVENT_READY, NULL);
        }
        else if((rlottie->play_ctrl & LV_RLOTTIE_CTRL_BACKWARD) == LV_RLOTTIE_CTRL_BACKWARD) {
            if(rlottie->dec.ctx.current_frame > 0)
                --rlottie->dec.ctx.current_frame;
            else { /* Looping ? */
                if((rlottie->play_ctrl & LV_RLOTTIE_CTRL_LOOP) == LV_RLOTTIE_CTRL_LOOP)
                    rlottie->dec.ctx.current_frame = rlottie->dec.ctx.total_frames - 1;
                else {
                    lv_event_send(obj, LV_EVENT_READY, NULL);
                    lv_timer_pause(t);
                }
            }
        }
        else {
            if(rlottie->dec.ctx.current_frame < rlottie->dec.ctx.total_frames)
                ++rlottie->dec.ctx.current_frame;
            else { /* Looping ? */
                if((rlottie->play_ctrl & LV_RLOTTIE_CTRL_LOOP) == 0) {
                    rlottie->dec.ctx.current_frame--; /*Pause on the last frame*/
                    lv_event_send(obj, LV_EVENT_READY, NULL);
                    lv_timer_pause(t);
                }
                else
                    rlottie->dec.ctx.current_frame = 0;
            }
        }
    }

    lv_obj_invalidate(obj);
}

#endif /*LV_USE_RLOTTIE*/
