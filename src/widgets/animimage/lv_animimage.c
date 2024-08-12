/**
 * @file lv_animimg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_animimage_private.h"
#include "../../core/lv_obj_class_private.h"
#if LV_USE_ANIMIMG != 0

/*Testing of dependencies*/
#if LV_USE_IMAGE == 0
    #error "lv_animimg: lv_img is required. Enable it in lv_conf.h (LV_USE_IMAGE  1) "
#endif

#include "../../draw/lv_image_decoder.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_fs.h"
#include "../../misc/lv_text.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_log.h"
#include "../../misc/lv_anim.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_animimg"

#define MY_CLASS (&lv_animimg_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void index_change(lv_obj_t * obj, int32_t idx);
static void lv_animimg_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_animimg_class = {
    .constructor_cb = lv_animimg_constructor,
    .instance_size = sizeof(lv_animimg_t),
    .base_class = &lv_image_class,
    .name = "animimg",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_animimg_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_animimg_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_animimg_set_src(lv_obj_t * obj, const void * dsc[], size_t num)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    animimg->dsc = dsc;
    animimg->pic_count = num;
    lv_anim_set_values(&animimg->anim, 0, (int32_t)num);
}

void lv_animimg_start(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_start(&animimg->anim);
}

/*=====================
 * Setter functions
 *====================*/

void lv_animimg_set_duration(lv_obj_t * obj, uint32_t duration)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_set_duration(&animimg->anim, duration);
    lv_anim_set_playback_delay(&animimg->anim, duration);
}

void lv_animimg_set_repeat_count(lv_obj_t * obj, uint32_t count)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_set_repeat_count(&animimg->anim, count);
}

/*=====================
 * Getter functions
 *====================*/

const void ** lv_animimg_get_src(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    return animimg->dsc;
}

uint8_t lv_animimg_get_src_count(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    return animimg->pic_count;
}

uint32_t lv_animimg_get_duration(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    return lv_anim_get_time(&animimg->anim);
}

uint32_t lv_animimg_get_repeat_count(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    return lv_anim_get_repeat_count(&animimg->anim);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_animimg_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_TRACE_OBJ_CREATE("begin");

    LV_UNUSED(class_p);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;

    animimg->dsc = NULL;
    animimg->pic_count = -1;

    /*initial animation*/
    lv_anim_init(&animimg->anim);
    lv_anim_set_var(&animimg->anim, obj);
    lv_anim_set_duration(&animimg->anim, 30);
    lv_anim_set_exec_cb(&animimg->anim, (lv_anim_exec_xcb_t)index_change);
    lv_anim_set_values(&animimg->anim, 0, 1);
    lv_anim_set_repeat_count(&animimg->anim, LV_ANIM_REPEAT_INFINITE);
}

static void index_change(lv_obj_t * obj, int32_t idx)
{
    lv_animimg_t * animimg = (lv_animimg_t *)obj;

    if(animimg->dsc == NULL) {
        LV_LOG_WARN("dsc is null");
        return;
    }

    if(idx >= animimg->pic_count) idx =  animimg->pic_count - 1;

    lv_image_set_src(obj, animimg->dsc[idx]);
}

#endif
