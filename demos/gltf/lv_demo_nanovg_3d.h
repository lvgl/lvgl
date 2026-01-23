/**
 * @file lv_demo_nanovg_3d.h
 * Demo for NanoVG 3D extension with cgltf
 */

#ifndef LV_DEMO_NANOVG_3D_H
#define LV_DEMO_NANOVG_3D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_demos.h"

#if LV_USE_DEMO_NANOVG_GLTF

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
 * Create NanoVG 3D demo
 * @param path Path to glTF file
 * @return The created object
 */
lv_obj_t * lv_demo_nanovg_3d(const char * path);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DEMO_NANOVG_GLTF */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DEMO_NANOVG_3D_H */
