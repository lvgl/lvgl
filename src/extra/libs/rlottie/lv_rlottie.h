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

extern const lv_obj_class_t lv_rlottie_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/** Create a rlottie animation from a file
 *  You can set the file path with lv_img_src_file.
 *  However this function allows more control on the object creation size and caching
 *  @param parent   The parent object to create into
 *  @param width    The image width in pixels
 *  @param height   The image height in pixels
 *  @param path     The file path to use. This skips LVGL FS drivers
 *  @param buf_size Optional. If not 0, set the maximum rendering buffer size to use.
 *                  If set to w * 4 * h, the complete image is rendered at once
 */
lv_obj_t * lv_rlottie_create_from_file(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * path,
                                       const size_t buf_size);

/** Create a rlottie animation from readonly data
 *  You can set the data with lv_img_src_data.
 *  However this function allows more control on the object creation size and caching
 *  @param parent   The parent object to create into
 *  @param width    The image width in pixels
 *  @param height   The image height in pixels
 *  @param desc     A pointed on the Lottie's JSON content
 *  @param len      The JSON content length in bytes
 *  @param buf_size Optional. If not 0, set the maximum rendering buffer size to use.
 *                  If set to w * 4 * h, the complete image is rendered at once
 */
lv_obj_t * lv_rlottie_create_from_raw(lv_obj_t * parent, lv_coord_t width, lv_coord_t height,
                                      const char * desc, const size_t len, const size_t buf_size);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RLOTTIE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_RLOTTIE_H*/
