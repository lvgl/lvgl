/**
 * @file lv_lottie.h
 *
 */

#ifndef LV_LOTTIE_H
#define LV_LOTTIE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_types.h"
#if LV_USE_LOTTIE

/*Testing of dependencies*/
#if LV_USE_CANVAS == 0
#error "lv_lottie: lv_canvas is required. Enable it in lv_conf.h (LV_USE_CANVAS 1)"
#endif

#if LV_USE_THORVG == 0
#error "lv_lottie: ThorVG is required. Enable it in lv_conf.h (LV_USE_THORVG_INTERNAL/EXTERNAL 1)"
#endif

#include "../../draw/lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a lottie animation
 * @param parent    pointer to the parent widget
 * @return          pointer to the created Lottie animation widget
 */
lv_obj_t  * lv_lottie_create(lv_obj_t * parent);

/**
 * Set a buffer for the animation. It also defines the size of the animation
 * @param obj       pointer to a lottie widget
 * @param w         width of the animation and buffer
 * @param h         height of the animation and buffer
 * @param buf       a static buffer with `width x height x 4` byte size
 */
void lv_lottie_set_buffer(lv_obj_t * obj, int32_t w, int32_t h, void * buf);

/**
 * Set a draw buffer for the animation. It also defines the size of the animation
 * @param obj       pointer to a lottie widget
 * @param draw_buf  an initialized draw buffer with ARGB8888 color format
 */
void lv_lottie_set_draw_buf(lv_obj_t * obj, lv_draw_buf_t * draw_buf);

/**
 * Set the source for the animation as an array
 * @param obj       pointer to a lottie widget
 * @param src       the lottie animation converted to an nul terminated array
 * @param src_size  size of the source array in bytes
 */
void lv_lottie_set_src_data(lv_obj_t * obj, const void * src, size_t src_size);

/**
 * Set the source for the animation as a path.
 * Lottie doesn't use LVGL's File System API.
 * @param obj       pointer to a lottie widget
 * @param src       path to a json file, e.g. "path/to/file.json"
 */
void lv_lottie_set_src_file(lv_obj_t * obj, const char * src);

/**
 * Get the LVGL animation which controls the lottie animation
 * @param obj       pointer to a lottie widget
 * @return          the LVGL animation
 */
lv_anim_t * lv_lottie_get_anim(lv_obj_t * obj);

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LOTTIE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LOTTIE_H*/
