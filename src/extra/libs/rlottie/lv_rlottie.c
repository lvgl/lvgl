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

lv_obj_t * lv_rlottie_create_from_file(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * path,
                                       const size_t buf_size)
{
    lv_rlottie_init();

    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_rlottie_t * rlottie = (lv_rlottie_t *)obj;
    lv_img_src_set_file(&rlottie->img_ext.src, path);
    /* We can't use the object yet, since it's not constructed, so we need to store the size elsewhere*/
    rlottie->create_size.x = width;
    rlottie->create_size.y = height;
    rlottie->max_buf_size = buf_size;
    lv_obj_class_init_obj(obj);

    return obj;

}

lv_obj_t * lv_rlottie_create_from_raw(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * rlottie_desc,
                                      const size_t len, const size_t buf_size)
{
    lv_rlottie_init();

    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_rlottie_t * rlottie = (lv_rlottie_t *)obj;
    lv_img_src_set_data(&rlottie->img_ext.src, (const uint8_t *)rlottie_desc, len);
    /* We can't use the object yet, since it's not constructed, so we need to store the size elsewhere*/
    rlottie->create_size.x = width;
    rlottie->create_size.y = height;
    rlottie->max_buf_size = buf_size;
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
    if(rlottie->max_buf_size)
        ((rlottiedec_ctx_t *)rlottie->img_ext.dec_ctx)->max_buf_size = rlottie->max_buf_size;
    lv_img_accept_src(obj, &rlottie->img_ext.src, 0);

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
/*
    if(rlottie->img_ext.dec_ctx && !rlottie->img_ext.dec_ctx->auto_allocated) {
        rlottie->img_ext.dec_ctx->auto_allocated = 1; // Let decoder do all the work 
        lv_img_decoder_t * decoder = lv_img_decoder_accept(&rlottie->img_ext.src, NULL);
        lv_img_decoder_dsc_t dsc;
        dsc.decoder = decoder;
        dsc.dec_ctx = rlottie->img_ext.dec_ctx;
        lv_img_decoder_close(&dsc);
    }
  */      
    //lv_img_cache_invalidate_src(&rlottie->img_ext.src, rlottie->img_ext.dec_ctx);
}


#endif /*LV_USE_RLOTTIE*/
