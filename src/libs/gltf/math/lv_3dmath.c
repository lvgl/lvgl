/**
 * @file lv_3dmath.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_3dmath.h"
#if LV_USE_GLTF

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_3dplane_t lv_get_ground_plane(float elevation)
{
    return (lv_3dplane_t) {
        .origin = {0.f, elevation, 0.f},
        .direction = {0.f, 1.f, 0.f}
    };
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GLTF*/
