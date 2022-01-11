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
    lv_img_src_uri_file(&rlottie->img_ext.src, path);
    /* We can't use the object yet, since it's not constructed, so we need to store the size elsewhere*/
    rlottie->create_size.x = width;
    rlottie->create_size.y = height;
    lv_obj_class_init_obj(obj);

    return obj;

}

lv_obj_t * lv_rlottie_create_from_raw(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * rlottie_desc,
                                      const size_t len)
{
    lv_rlottie_init();

    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_rlottie_t * rlottie = (lv_rlottie_t *)obj;
    lv_img_src_uri_data(&rlottie->img_ext.src, (const uint8_t *)rlottie_desc, len);
    /* We can't use the object yet, since it's not constructed, so we need to store the size elsewhere*/
    rlottie->create_size.x = width;
    rlottie->create_size.y = height;

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

    lv_obj_set_size(obj, rlottie->create_size.x, rlottie->create_size.y);
    /* We want to keep the decoder context initialized here, so allocate and mark it*/
    LV_ZALLOC(rlottie->img_ext.dec_ctx, sizeof(rlottiedec_ctx_t));
    rlottie->img_ext.dec_ctx->auto_allocated = 0;
    lv_img_set_src_uri(obj, &rlottie->img_ext.src);

    if(rlottie->img_ext.dec_ctx == NULL || ((rlottiedec_ctx_t *)rlottie->img_ext.dec_ctx)->cache == NULL) {
        LV_LOG_WARN("The animation can't be opened");
        return;
    }

    lv_obj_update_layout(obj);
}


static void lv_rlottie_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_rlottie_t * rlottie = (lv_rlottie_t *) obj;

    lv_img_cache_invalidate_src(&rlottie->img_ext.src);
    if(rlottie->img_ext.dec_ctx && !rlottie->img_ext.dec_ctx->auto_allocated) {
        struct Lottie_Animation_S * anim = (struct Lottie_Animation_S *)rlottie->img_ext.dec_ctx->user_data;
        lottie_animation_destroy(anim);
        rlottie->img_ext.dec_ctx->user_data = 0;
    }
}


#endif /*LV_USE_RLOTTIE*/
