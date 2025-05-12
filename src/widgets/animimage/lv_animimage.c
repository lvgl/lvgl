/**
 * @file lv_animimg.c
 *
 */

/**
 * Modified by NXP in 2025
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
static void lv_animimg_set_src_inner(lv_obj_t * obj, const void * dsc[], size_t num, bool reverse);

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_OBJ_PROPERTY
static const lv_property_ops_t properties[] = {
    {
        .id = LV_PROPERTY_ANIMIMAGE_SRC,
        .setter = lv_animimg_set_src,
        .getter = lv_animimg_get_src,
    },
    {
        .id = LV_PROPERTY_ANIMIMAGE_DURATION,
        .setter = lv_animimg_set_duration,
        .getter = lv_animimg_get_duration,
    },
    {
        .id = LV_PROPERTY_ANIMIMAGE_REPEAT_COUNT,
        .setter = lv_animimg_set_repeat_count,
        .getter = lv_animimg_get_repeat_count,
    },
    {
        .id = LV_PROPERTY_ANIMIMAGE_SRC_COUNT,
        .setter = NULL,
        .getter = lv_animimg_get_src_count,
    },
};
#endif

const lv_obj_class_t lv_animimg_class = {
    .constructor_cb = lv_animimg_constructor,
    .instance_size = sizeof(lv_animimg_t),
    .base_class = &lv_image_class,
    .name = "lv_animimg",
#if LV_USE_OBJ_PROPERTY
    .prop_index_start = LV_PROPERTY_ANIMIMAGE_START,
    .prop_index_end = LV_PROPERTY_ANIMIMAGE_END,
    .properties = properties,
    .properties_count = sizeof(properties) / sizeof(properties[0]),

#if LV_USE_OBJ_PROPERTY_NAME
    .property_names = lv_animimage_property_names,
    .names_count = sizeof(lv_animimage_property_names) / sizeof(lv_property_name_t),
#endif
#endif
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
    lv_animimg_set_src_inner(obj, dsc, num, false);
}

void lv_animimg_set_src_reverse(lv_obj_t * obj, const void * dsc[], size_t num)
{
    lv_animimg_set_src_inner(obj, dsc, num, true);
}

void lv_animimg_start(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_start(&animimg->anim);
}

bool lv_animimg_delete(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    return lv_anim_delete(animimg, NULL);
}

/*=====================
 * Setter functions
 *====================*/

void lv_animimg_set_duration(lv_obj_t * obj, uint32_t duration)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_set_duration(&animimg->anim, duration);
    lv_anim_set_reverse_delay(&animimg->anim, duration);
}

void lv_animimg_set_repeat_count(lv_obj_t * obj, uint32_t count)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_set_repeat_count(&animimg->anim, count);
}

void lv_animimg_set_reverse_duration(lv_obj_t * obj, uint32_t duration)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_set_reverse_duration(&animimg->anim, duration);
}

void lv_animimg_set_reverse_delay(lv_obj_t * obj, uint32_t duration)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_set_reverse_delay(&animimg->anim, duration);
}

void lv_animimg_set_start_cb(lv_obj_t * obj, lv_anim_start_cb_t start_cb)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_set_start_cb(&animimg->anim, start_cb);
}

void lv_animimg_set_completed_cb(lv_obj_t * obj, lv_anim_completed_cb_t completed_cb)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_set_completed_cb(&animimg->anim, completed_cb);
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

lv_anim_t * lv_animimg_get_anim(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    return &animimg->anim;
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

static void lv_animimg_set_src_inner(lv_obj_t * obj, const void * dsc[], size_t num, bool reverse)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    animimg->dsc = dsc;
    animimg->pic_count = num;
    if(reverse) {
        lv_anim_set_values(&animimg->anim, (int32_t)num, 0);
    }
    else {
        lv_anim_set_values(&animimg->anim, 0, (int32_t)num);
    }
}

#endif
