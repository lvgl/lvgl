/**
 * @file lv_arc.h
 *
 */


#ifndef LV_ARC_H
#define LV_ARC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if USE_LV_ARC != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of arc*/
typedef struct {
    /*New data for this type */
    lv_coord_t angle_start;
    lv_coord_t angle_end;
} lv_arc_ext_t;


/*Styles*/
enum {
    LV_ARC_STYLE_MAIN,
};
typedef uint8_t lv_arc_style_t;



/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a arc objects
 * @param par pointer to an object, it will be the parent of the new arc
 * @param copy pointer to a arc object, if not NULL then the new object will be copied from it
 * @return pointer to the created arc
 */
lv_obj_t * lv_arc_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/


/*=====================
 * Setter functions
 *====================*/

/**
 * Set the start and end angles of an arc. 0 deg: bottom, 90 deg: right etc.
 * @param arc pointer to an arc object
 * @param start the start angle [0..360]
 * @param end the end angle [0..360]
 */
void lv_arc_set_angles(lv_obj_t * arc, uint16_t start, uint16_t end);

/**
 * Set a style of a arc.
 * @param arc pointer to arc object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_arc_set_style(lv_obj_t * arc, lv_arc_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the start angle of an arc.
 * @param arc pointer to an arc object
 * @return the start angle [0..360]
 */
uint16_t lv_arc_get_angle_start(lv_obj_t * arc);

/**
 * Get the end angle of an arc.
 * @param arc pointer to an arc object
 * @return the end angle [0..360]
 */
uint16_t lv_arc_get_angle_end(lv_obj_t * arc);

/**
 * Get style of a arc.
 * @param arc pointer to arc object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
lv_style_t * lv_arc_get_style(const lv_obj_t * arc, lv_arc_style_t type);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_ARC*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_ARC_H*/
