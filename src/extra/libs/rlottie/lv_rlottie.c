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
/** definition in lottieanimation_capi.c */
struct Lottie_Animation_S;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_rlottie_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_rlottie_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

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
    lv_obj_set_size(obj, width, height);
    lv_img_set_src_file(obj, path);

    lv_obj_class_init_obj(obj);

    return obj;

}

lv_obj_t * lv_rlottie_create_from_raw(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * rlottie_desc, const size_t len)
{
    lv_rlottie_init();

    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_rlottie_t * rlottie = (lv_rlottie_t *)obj;
    lv_obj_set_size(obj, width, height);
    lv_img_set_src_data(obj, (const uint8_t*)rlottie_desc, len);

    lv_obj_class_init_obj(obj);

    return obj;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_rlottie_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;

    if(rlottie->img_ext.dec_ctx == NULL || rlottie->img_ext.dec_ctx->user_data == NULL) {
        LV_LOG_WARN("The animation can't be opened");
        return;
    }

    /* Capture the context so it's not deleted */
    rlottie->img_ext.dec_ctx->self_allocated = 1;
/*
    rlottie->framerate = (size_t)lottie_animation_get_framerate(rlottie->dec.cache);

    rlottie->play_ctrl = LV_RLOTTIE_CTRL_FORWARD | LV_RLOTTIE_CTRL_PLAY | LV_RLOTTIE_CTRL_LOOP;
    rlottie->dest_frame =
        rlottie->dec.ctx.total_frames; /* invalid destination frame so it's possible to pause on frame 0 */


    lv_obj_update_layout(obj);
}


static void lv_rlottie_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;

    lv_img_cache_invalidate_src(&rlottie->img_ext.src);
    if(rlottie->img_ext.dec_ctx && rlottie->img_ext.dec_ctx->self_allocated) {
        struct Lottie_Animation_S * anim = (struct Lottie_Animation_S *)rlottie->img_ext.dec_ctx->user_data;
        lottie_animation_destroy(anim);
        rlottie->img_ext.dec_ctx->user_data = 0;
    }
}


#endif /*LV_USE_RLOTTIE*/
