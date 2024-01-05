/**
 * @file lv_rlottie.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_rlottie.h"
#if LV_USE_RLOTTIE

#include <rlottie_capi.h>

/*********************
*      DEFINES
*********************/
#define MY_CLASS &lv_rlottie_class
#define LV_ARGB32   32

/**********************
*      TYPEDEFS
**********************/
#define LV_ARGB32   32

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
    .base_class = &lv_image_class,
    .name = "rlottie",
};

typedef struct {
    int32_t width;
    int32_t height;
    const char * rlottie_desc;
    const char * path;
} lv_rlottie_create_info_t;

/*Only used in lv_obj_class_create_obj, no affect multiple instances*/
static lv_rlottie_create_info_t create_info;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_rlottie_create_from_file(lv_obj_t * parent, int32_t width, int32_t height, const char * path)
{
    create_info.width = width;
    create_info.height = height;
    create_info.path = path;
    create_info.rlottie_desc = NULL;

    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

lv_obj_t * lv_rlottie_create_from_raw(lv_obj_t * parent, int32_t width, int32_t height, const char * rlottie_desc)
{
    create_info.width = width;
    create_info.height = height;
    create_info.rlottie_desc = rlottie_desc;
    create_info.path = NULL;

    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

void lv_rlottie_set_play_mode(lv_obj_t * obj, const lv_rlottie_ctrl_t ctrl)
{
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;
    rlottie->play_ctrl = ctrl;

    if(rlottie->task && (rlottie->dest_frame != rlottie->current_frame ||
                         (rlottie->play_ctrl & LV_RLOTTIE_CTRL_PAUSE) == LV_RLOTTIE_CTRL_PLAY)) {
        lv_timer_resume(rlottie->task);
    }
}

void lv_rlottie_set_current_frame(lv_obj_t * obj, const size_t goto_frame)
{
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;
    rlottie->current_frame = goto_frame < rlottie->total_frames ? goto_frame : rlottie->total_frames - 1;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_rlottie_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;

    if(create_info.rlottie_desc) {
        rlottie->animation = lottie_animation_from_data(create_info.rlottie_desc, create_info.rlottie_desc, "");
    }
    else if(create_info.path) {
        rlottie->animation = lottie_animation_from_file(create_info.path);
    }
    if(rlottie->animation == NULL) {
        LV_LOG_WARN("The aniamtion can't be opened");
        return;
    }

    rlottie->total_frames = lottie_animation_get_totalframe(rlottie->animation);
    rlottie->framerate = (size_t)lottie_animation_get_framerate(rlottie->animation);
    rlottie->current_frame = 0;

    rlottie->scanline_width = create_info.width * LV_ARGB32 / 8;

    size_t allocaled_buf_size = (create_info.width * create_info.height * LV_ARGB32 / 8);
    rlottie->allocated_buf = lv_malloc(allocaled_buf_size);
    if(rlottie->allocated_buf != NULL) {
        rlottie->allocated_buffer_size = allocaled_buf_size;
        memset(rlottie->allocated_buf, 0, allocaled_buf_size);
    }

    rlottie->imgdsc.header.cf = LV_COLOR_FORMAT_ARGB8888;
    rlottie->imgdsc.header.h = create_info.height;
    rlottie->imgdsc.header.w = create_info.width;
    rlottie->imgdsc.data = (void *)rlottie->allocated_buf;
    rlottie->imgdsc.data_size = allocaled_buf_size;

    lv_image_set_src(obj, &rlottie->imgdsc);

    rlottie->play_ctrl = LV_RLOTTIE_CTRL_FORWARD | LV_RLOTTIE_CTRL_PLAY | LV_RLOTTIE_CTRL_LOOP;
    rlottie->dest_frame = rlottie->total_frames; /* invalid destination frame so it's possible to pause on frame 0 */

    rlottie->task = lv_timer_create(next_frame_task_cb, 1000 / rlottie->framerate, obj);

    lv_obj_update_layout(obj);
}

static void lv_rlottie_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;

    if(rlottie->animation) {
        lottie_animation_destroy(rlottie->animation);
        rlottie->animation = 0;
        rlottie->current_frame = 0;
        rlottie->framerate = 0;
        rlottie->scanline_width = 0;
        rlottie->total_frames = 0;
    }

    if(rlottie->task) {
        lv_timer_delete(rlottie->task);
        rlottie->task = NULL;
        rlottie->play_ctrl = LV_RLOTTIE_CTRL_FORWARD;
        rlottie->dest_frame = 0;
    }

    lv_image_cache_drop(&rlottie->imgdsc);

    if(rlottie->allocated_buf) {
        lv_free(rlottie->allocated_buf);
        rlottie->allocated_buf = NULL;
        rlottie->allocated_buffer_size = 0;
    }

}

static void next_frame_task_cb(lv_timer_t * t)
{
    lv_obj_t * obj = t->user_data;
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;

    if((rlottie->play_ctrl & LV_RLOTTIE_CTRL_PAUSE) == LV_RLOTTIE_CTRL_PAUSE) {
        if(rlottie->current_frame == rlottie->dest_frame) {
            /* Pause the timer too when it has run once to avoid CPU consumption */
            lv_timer_pause(t);
            return;
        }
        rlottie->dest_frame = rlottie->current_frame;
    }
    else {
        if((rlottie->play_ctrl & LV_RLOTTIE_CTRL_BACKWARD) == LV_RLOTTIE_CTRL_BACKWARD) {
            if(rlottie->current_frame > 0)
                --rlottie->current_frame;
            else { /* Looping ? */
                if((rlottie->play_ctrl & LV_RLOTTIE_CTRL_LOOP) == LV_RLOTTIE_CTRL_LOOP)
                    rlottie->current_frame = rlottie->total_frames - 1;
                else {
                    lv_obj_send_event(obj, LV_EVENT_READY, NULL);
                    lv_timer_pause(t);
                    return;
                }
            }
        }
        else {
            if(rlottie->current_frame < rlottie->total_frames)
                ++rlottie->current_frame;
            else { /* Looping ? */
                if((rlottie->play_ctrl & LV_RLOTTIE_CTRL_LOOP) == LV_RLOTTIE_CTRL_LOOP)
                    rlottie->current_frame = 0;
                else {
                    lv_obj_send_event(obj, LV_EVENT_READY, NULL);
                    lv_timer_pause(t);
                    return;
                }
            }
        }
    }

    lottie_animation_render(
        rlottie->animation,
        rlottie->current_frame,
        rlottie->allocated_buf,
        rlottie->imgdsc.header.w,
        rlottie->imgdsc.header.h,
        rlottie->scanline_width
    );

    lv_obj_invalidate(obj);
}

#endif /*LV_USE_RLOTTIE*/
