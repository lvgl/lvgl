/**
 * @file lv_gif.h
 *
 */

#ifndef LV_GIF_H
#define LV_GIF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#include "../../misc/lv_types.h"
#include LV_STDBOOL_INCLUDE
#include LV_STDINT_INCLUDE
#if LV_USE_GIF

#include "../../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_gif_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a gif object
 * @param parent    pointer to an object, it will be the parent of the new gif.
 * @return          pointer to the gif obj
 */
lv_obj_t * lv_gif_create(lv_obj_t * parent);

/**
 * Set the color format of the internally allocated framebuffer that the gif
 * will be decoded to. The default is LV_COLOR_FORMAT_ARGB8888.
 * Call this before `lv_gif_set_src` to avoid reallocating the framebuffer.
 * @param obj            pointer to a gif object
 * @param color_format   the color format of the gif framebuffer
 */
void lv_gif_set_color_format(lv_obj_t * obj, lv_color_format_t color_format);

/**
 * Set the gif data to display on the object
 * @param obj       pointer to a gif object
 * @param src       1) pointer to an ::lv_image_dsc_t descriptor (which contains gif raw data) or
 *                  2) path to a gif file (e.g. "S:/dir/anim.gif")
 */
void lv_gif_set_src(lv_obj_t * obj, const void * src);

/**
 * Restart a gif animation.
 * @param obj pointer to a gif obj
 */
void lv_gif_restart(lv_obj_t * obj);

/**
 * Pause a gif animation.
 * @param obj pointer to a gif obj
 */
void lv_gif_pause(lv_obj_t * obj);

/**
 * Resume a gif animation.
 * @param obj pointer to a gif obj
 */
void lv_gif_resume(lv_obj_t * obj);

/**
 * Checks if the GIF was loaded correctly.
 * @param obj pointer to a gif obj
 */
bool lv_gif_is_loaded(lv_obj_t * obj);

/**
 * Get the loop count for the GIF.
 * @param obj pointer to a gif obj
 */
int32_t lv_gif_get_loop_count(lv_obj_t * obj);

/**
 * Set the loop count for the GIF.
 * @param obj   pointer to a gif obj
 * @param count the loop count to set
 */
void lv_gif_set_loop_count(lv_obj_t * obj, int32_t count);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GIF*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GIF_H*/
