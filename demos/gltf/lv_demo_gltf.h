/**
 * @file lv_demo_gltf.h
 *
 */

#ifndef LV_DEMO_GLTF_H
#define LV_DEMO_GLTF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_demos.h"

#if LV_USE_DEMO_GLTF

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_demo_gltf(const char * path);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_GLTF*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_GLTF_H*/
