/**
 * @file lv_cb.h
 *
 */

#ifndef LV_3D_H
#define LV_3D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#define LV_USE_3D 1

#if LV_USE_3D != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of 3d obj*/

#define OBJ_3D_USE_FLOAT 0

typedef struct
{
    uint16_t * vertex_num;
#if OBJ_3D_USE_FLOAT
    float * vertex_array;
#else
    int * vertex_array;
#endif
    uint16_t * surface_num;
    uint16_t * surface_array;
    lv_color_t * color_array;
}_3d_obj_data;

typedef struct
{
    lv_coord_t widget;
    lv_coord_t height;

    int bfCull;
    int orient;
    int polyMode;
    int numVisible;
    int numPolys;
    int lightShade;

    int Str_dis_to_obj; //distance
    int Str_rotx;
    int Str_roty;       //rotate angle
    int cam_z;
}_3d_obj_para;

typedef struct {
    /*New data for this type */
    lv_obj_t * label;  /*Pointer to label*/
    lv_coord_t widget;
    lv_coord_t height;
    _3d_obj_data obj_data;
    _3d_obj_para obj_para;
} lv_3d_ext_t;

/** Checkbox styles. */
enum {
    LV_3D_PART_BG = LV_OBJ_PART_MAIN,  /**< Style of object background. */
    _LV_3D_PART_VIRTUAL_LAST,
    LV_3D_PART_BULLET = _LV_OBJ_PART_REAL_LAST, /**< Style of box (released). */
    _LV_3D_PART_REAL_LAST
};
typedef uint8_t lv_3d_style_t;

lv_obj_t * lv_3d_create(lv_obj_t * par, const lv_obj_t * copy);
void lv_3d_set_angle(lv_obj_t * obj, uint16_t x,uint16_t y,uint16_t z);
void lv_3d_set_distance(lv_obj_t * obj, int dis);

#endif /*LV_USE_3D*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_3D*/
