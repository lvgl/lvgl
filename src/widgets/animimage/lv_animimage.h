/**
 * @file lv_animimage.h
 *
 */

#ifndef LV_ANIMIMAGE_H
#define LV_ANIMIMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../image/lv_image.h"
#include "../../misc/lv_types.h"

#if LV_USE_ANIMIMG != 0

/*Testing of dependencies*/
#if LV_USE_IMAGE == 0
#error "lv_animimg: lv_img is required. Enable it in lv_conf.h (LV_USE_IMAGE 1)"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_animimg_class;

/** Image parts */
typedef enum {
    LV_ANIM_IMAGE_PART_MAIN,
} lv_animimg_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an animation image objects
 * @param parent pointer to an object, it will be the parent of the new button
 * @return pointer to the created animation image object
 */
lv_obj_t * lv_animimg_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the image animation images source.
 * @param img   pointer to an animation image object
 * @param dsc   pointer to a series images
 * @param num   images' number
 */
void lv_animimg_set_src(lv_obj_t * img, const void * dsc[], size_t num);

/**
 * Startup the image animation.
 * @param obj   pointer to an animation image object
 */
void lv_animimg_start(lv_obj_t * obj);

/**
 * Set the image animation duration time. unit:ms
 * @param img       pointer to an animation image object
 * @param duration  the duration in milliseconds
 */
void lv_animimg_set_duration(lv_obj_t * img, uint32_t duration);

/**
 * Set the image animation repeatedly play times.
 * @param img       pointer to an animation image object
 * @param count     the number of times to repeat the animation
 */
void lv_animimg_set_repeat_count(lv_obj_t * img, uint32_t count);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the image animation images source.
 * @param img   pointer to an animation image object
 * @return a    pointer that will point to a series images
 */
const void ** lv_animimg_get_src(lv_obj_t * img);

/**
 * Get the image animation images source.
 * @param img   pointer to an animation image object
 * @return      the number of source images
 */
uint8_t lv_animimg_get_src_count(lv_obj_t * img);

/**
 * Get the image animation duration time. unit:ms
 * @param img   pointer to an animation image object
 * @return      the animation duration time
 */
uint32_t lv_animimg_get_duration(lv_obj_t * img);

/**
 * Get the image animation repeat play times.
 * @param img   pointer to an animation image object
 * @return      the repeat count
 */
uint32_t lv_animimg_get_repeat_count(lv_obj_t * img);

/**
 * Get the image animation underlying animation.
 * @param img   pointer to an animation image object
 * @return      the animation reference
 */
lv_anim_t * lv_animimg_get_anim(lv_obj_t * img);

#endif /*LV_USE_ANIMIMG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_ANIMIMAGE_H*/
