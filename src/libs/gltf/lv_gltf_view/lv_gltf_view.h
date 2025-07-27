/**
 * @file lv_gltf_view.h
 *
 */

#ifndef LV_GLTF_VIEW_H
#define LV_GLTF_VIEW_H

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include "../../../misc/lv_types.h"
#include "../lv_gltf_data/lv_gltf_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t *lv_gltf_view_create(lv_obj_t *parent);
lv_gltf_data_t *lv_gltf_load_model_from_file(lv_obj_t *obj, const char *path);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
}
#endif

#endif /*LV_USE_GLTF*/

#endif /*LV_GLTF_VIEW_H*/
