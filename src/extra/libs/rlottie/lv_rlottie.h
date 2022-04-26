/**
 * @file lv_rlottie.h
 *
 */

#ifndef LV_RLOTTIE_H
#define LV_RLOTTIE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../widgets/lv_img.h"
#if LV_USE_RLOTTIE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_img_t img_ext;
    lv_point_t create_size;
    size_t     max_buf_size;
} lv_rlottie_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/** Load a rlottie animation from a file
 *  This acts like lv_img_set_src but with specific parameters, like creation size and caching attributes.
 *  @param img      The pointer to a lv_img_t instance that's modified
 *  @param width    The image rendering width in pixels
 *  @param height   The image rendering height in pixels
 *  @param path     The file path to use. This skips LVGL FS drivers
 *  @param buf_size Optional. If not 0, set the maximum rendering buffer size to use.
 *                  If set to w * 4 * h, the complete image is rendered at once
 */
lv_res_t lv_rlottie_from_file(lv_obj_t * img, lv_coord_t width, lv_coord_t height, const char * path,
                              const size_t buf_size);

/** Load a rlottie animation from readonly data
 *  This acts like lv_img_set_src but with specific parameters, like creation size and caching attributes.
 *  @param img      The pointer to a lv_img_t instance that's modified
 *  @param width    The image width in pixels
 *  @param height   The image height in pixels
 *  @param desc     A pointed on the Lottie's JSON content
 *  @param len      The JSON content length in bytes
 *  @param buf_size Optional. If not 0, set the maximum rendering buffer size to use.
 *                  If set to w * 4 * h, the complete image is rendered at once
 */
lv_res_t lv_rlottie_from_raw(lv_obj_t * img, lv_coord_t width, lv_coord_t height, const char * desc, const size_t len,
                             const size_t buf_size);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RLOTTIE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_RLOTTIE_H*/
