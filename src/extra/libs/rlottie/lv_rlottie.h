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
} lv_rlottie_t;

extern const lv_obj_class_t lv_rlottie_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_rlottie_create_from_file(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * path);

lv_obj_t * lv_rlottie_create_from_raw(lv_obj_t * parent, lv_coord_t width, lv_coord_t height,
                                      const char * rlottie_desc, const size_t len);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RLOTTIE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_RLOTTIE_H*/
