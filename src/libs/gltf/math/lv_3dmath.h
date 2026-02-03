/**
 * @file lv_3dmath.h
 *
 */

#ifndef LV_3DMATH_H
#define LV_3DMATH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    float x;
    float y;
    float z;
} lv_3dpoint_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} lv_quaternion_t;

typedef struct {
    lv_3dpoint_t origin;
    lv_3dpoint_t direction;
} lv_3dplane_t;

typedef lv_3dplane_t lv_3dray_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get a plane that faces upward, centered at a given height
 * @param elevation elevation of the ground plane, in world units. this is usually zero
 * @return ground plane
 */
lv_3dplane_t lv_get_ground_plane(float elevation);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLTF*/

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif /*LV_3DMATH_H*/
