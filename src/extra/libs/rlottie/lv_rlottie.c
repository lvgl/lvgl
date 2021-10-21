/**
 * @file lv_rlottie.c
 *
 */

 /*********************
  *      INCLUDES
  *********************/

#include "lv_rlottie.h"
#if LV_USE_RLOTTIE

/*********************
*      DEFINES
*********************/
#define MY_CLASS &lv_rlottie_class

/**********************
*      TYPEDEFS
**********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_rlottie_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_rlottie_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void next_frame_task_cb(lv_timer_t* t);

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
static const char* rlottie_desc_create;
static const char* path_create;

/**********************
 *      MACROS
 **********************/

 /**********************
  *   GLOBAL FUNCTIONS
  **********************/

lv_obj_t* lv_rlottie_create_from_file(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, const char* path)
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

lv_obj_t* lv_rlottie_create_from_raw(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, const char* rlottie_desc)
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

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_rlottie_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{

    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;

    if(rlottie_desc_create) {
        rlottie->animation = lottie_animation_from_data(rlottie_desc_create, rlottie_desc_create,"");
    }
    else if(path_create) {
        rlottie->animation = lottie_animation_from_file(path_create);
    }
    if (rlottie->animation == NULL) {
        LV_LOG_WARN("The aniamtion can't be opened");
        return;
    }

    rlottie->total_frames = lottie_animation_get_totalframe(rlottie->animation);
    rlottie->framerate = lottie_animation_get_framerate(rlottie->animation);
    rlottie->current_frame = 0;

    lv_obj_t * parent = lv_obj_get_parent(obj);

    rlottie->scanline_width = create_width * LV_COLOR_DEPTH / 8;

    size_t allocaled_buf_size = (create_width * create_height * LV_COLOR_DEPTH / 8);
    rlottie->allocated_buf = lv_mem_alloc(allocaled_buf_size);
    if (rlottie->allocated_buf != NULL)
    {
        rlottie->allocated_buffer_size = allocaled_buf_size;
        memset(rlottie->allocated_buf, 0, allocaled_buf_size);
    }

    rlottie->imgdsc.header.always_zero = 0;
    rlottie->imgdsc.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
    rlottie->imgdsc.header.h = create_height;
    rlottie->imgdsc.header.w = create_width;
    rlottie->imgdsc.data = rlottie->allocated_buf;
    rlottie->imgdsc.data_size = allocaled_buf_size;

    lv_img_set_src(obj, &rlottie->imgdsc);

    rlottie->task = lv_timer_create(next_frame_task_cb, 1000 / rlottie->framerate, obj);

    lv_obj_update_layout(obj);
}


static void lv_rlottie_destructor(const lv_obj_class_t * class_p, lv_obj_t* obj)
{

    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;

    if (rlottie->animation) {
        lottie_animation_destroy(rlottie->animation);
        rlottie->animation = 0;
        rlottie->current_frame = 0;
        rlottie->framerate = 0;
        rlottie->scanline_width = 0;
        rlottie->total_frames = 0;
    }

    if (rlottie->task){
        lv_timer_del(rlottie->task);
        rlottie->task = NULL;
    }

    if (rlottie->allocated_buf) {
        lv_mem_free(rlottie->allocated_buf);
        rlottie->allocated_buf = NULL;
        rlottie->allocated_buffer_size = 0;
    }

}

static void next_frame_task_cb(lv_timer_t* t)
{
    lv_obj_t* obj = t->user_data;
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;
    if (rlottie->current_frame == rlottie->total_frames)
        rlottie->current_frame = 0;
    else
        ++rlottie->current_frame;

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
