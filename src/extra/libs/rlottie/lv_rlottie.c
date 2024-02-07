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
    .base_class = &lv_img_class
};

static lv_coord_t create_width;
static lv_coord_t create_height;
static const char * rlottie_desc_create;
static const char * path_create;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_rlottie_create_from_file(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * path)
{

    create_width = width;
    create_height = height;
    path_create = path;
    rlottie_desc_create = NULL;

    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    return obj;

}

lv_obj_t * lv_rlottie_create_from_raw(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * rlottie_desc)
{

    create_width = width;
    create_height = height;
    rlottie_desc_create = rlottie_desc;
    path_create = NULL;

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

    if(rlottie_desc_create) {
        rlottie->animation = lottie_animation_from_data(rlottie_desc_create, rlottie_desc_create, "");
    }
    else if(path_create) {
        rlottie->animation = lottie_animation_from_file(path_create);
    }
    if(rlottie->animation == NULL) {
        LV_LOG_WARN("The aniamtion can't be opened");
        return;
    }

    rlottie->total_frames = lottie_animation_get_totalframe(rlottie->animation);
    rlottie->framerate = (size_t)lottie_animation_get_framerate(rlottie->animation);
    rlottie->current_frame = 0;

    rlottie->scanline_width = create_width * LV_ARGB32 / 8;

    size_t allocaled_buf_size = (create_width * create_height * LV_ARGB32 / 8);
    rlottie->allocated_buf = lv_mem_alloc(allocaled_buf_size);
    if(rlottie->allocated_buf != NULL) {
        rlottie->allocated_buffer_size = allocaled_buf_size;
        memset(rlottie->allocated_buf, 0, allocaled_buf_size);
    }

    rlottie->imgdsc.header.always_zero = 0;
    rlottie->imgdsc.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
    rlottie->imgdsc.header.h = create_height;
    rlottie->imgdsc.header.w = create_width;
    rlottie->imgdsc.data = (void *)rlottie->allocated_buf;
    rlottie->imgdsc.data_size = allocaled_buf_size;

    lv_img_set_src(obj, &rlottie->imgdsc);

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
        lv_timer_del(rlottie->task);
        rlottie->task = NULL;
        rlottie->play_ctrl = LV_RLOTTIE_CTRL_FORWARD;
        rlottie->dest_frame = 0;
    }

    lv_img_cache_invalidate_src(&rlottie->imgdsc);
    if(rlottie->allocated_buf) {
        lv_mem_free(rlottie->allocated_buf);
        rlottie->allocated_buf = NULL;
        rlottie->allocated_buffer_size = 0;
    }

}

#if LV_COLOR_DEPTH == 16
static void convert_to_rgba5658(uint32_t * pix, const size_t width, const size_t height)
{
    /* rlottie draws in ARGB32 format, but LVGL only deal with RGB565 format with (optional 8 bit alpha channel)
       so convert in place here the received buffer to LVGL format. */
    uint8_t * dest = (uint8_t *)pix;
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
                    lv_event_send(obj, LV_EVENT_READY, NULL);
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
                    lv_event_send(obj, LV_EVENT_READY, NULL);
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

#if LV_COLOR_DEPTH == 16
    convert_to_rgba5658(rlottie->allocated_buf, rlottie->imgdsc.header.w, rlottie->imgdsc.header.h);
#endif

    lv_obj_invalidate(obj);
}

#endif /*LV_USE_RLOTTIE*/
