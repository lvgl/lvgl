/**
 * @file lv_animimg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_animimg.h"
#if LV_USE_ANIMIMG != 0

/*Testing of dependencies*/
#if LV_USE_IMG == 0
    #error "lv_animimg: lv_img is required. Enable it in lv_conf.h (LV_USE_IMG  1) "
#endif

#include "../../../misc/lv_assert.h"
#include "../../../draw/lv_img_decoder.h"
#include "../../../misc/lv_fs.h"
#include "../../../misc/lv_txt.h"
#include "../../../misc/lv_math.h"
#include "../../../misc/lv_log.h"
#include "../../../misc/lv_anim.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_animimg"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void index_change(lv_obj_t * obj, int32_t index);
static void lv_animimg_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_animimg_class = {
    .constructor_cb = lv_animimg_constructor,
    .instance_size = sizeof(lv_animimg_t),
    .base_class = &lv_img_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create an animation image objects
 * @param par pointer to an object, it will be the parent of the new animation image
 * @param copy pointer to a image object, if not NULL then the new object will be copied from it
 * @return pointer to the created animation image
 */
lv_obj_t * lv_animimg_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin")
    return lv_obj_create_from_class(&lv_animimg_class, parent);
}

/**
 * Set the animation images source.
 * @param img pointer to an animation image object
 * @param dsc pointer to a series images
 * @param num images' number
 */
void lv_animimg_set_src(lv_obj_t * obj,  lv_img_dsc_t ** dsc, uint8_t num)
{
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    animimg->dsc = dsc;
    animimg->pic_count = num;
    lv_anim_set_values(&animimg->anim, 0 , num);
}

/**
 * Startup the  image animation.
 * @param img pointer to an animation image object
 */
void lv_animimg_start(lv_obj_t * obj)
{
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_start(&animimg->anim);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the  image animation wait before repeat. unit:ms
 * @param img pointer to an animation image object
 */
void lv_animimg_set_repeat_delay(lv_obj_t * obj, uint32_t delay)
{
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    animimg->anim.repeat_delay = delay;
}

/**
 * Set the  image animation duration time. unit:ms
 * @param img pointer to an animation image object
 */
void lv_animimg_set_duration(lv_obj_t * obj, uint32_t duration)
{
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_set_time(&animimg->anim, duration);
    lv_anim_set_playback_time(&animimg->anim, duration);
}

/**
 * Set the image animation reapeatly play times.
 * @param img pointer to an animation image object
 */
void lv_animimg_set_repeat_count(lv_obj_t * obj, uint16_t count)
{
    lv_animimg_t * animimg = (lv_animimg_t *)obj;
    lv_anim_set_repeat_count(&animimg->anim, count);
}

/*=====================
 * Getter functions
 *====================*/

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
    //initial animation
    lv_anim_init(&animimg->anim);
    lv_anim_set_var(&animimg->anim, obj);
    lv_anim_set_time(&animimg->anim, 30);
    lv_anim_set_exec_cb(&animimg->anim, (lv_anim_exec_xcb_t)index_change);
    lv_anim_set_values(&animimg->anim, 0 , 1);
    lv_anim_set_playback_time(&animimg->anim, 100);
    lv_anim_set_repeat_count(&animimg->anim, LV_ANIM_REPEAT_INFINITE);
}

/**
 * Handle the drawing related tasks of the images
 * @param img pointer to an image animationobject
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */

static void index_change(lv_obj_t * obj, int32_t index)
{
    lv_coord_t idx;
    lv_animimg_t * animimg = (lv_animimg_t *)obj;

    idx = index % animimg->pic_count;

    lv_img_set_src(obj, animimg->dsc[idx]);
}

#endif
