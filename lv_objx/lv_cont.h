/**
 * @file lv_cont.h
 *
 */

#ifndef LV_CONT_H
#define LV_CONT_H

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

#if USE_LV_CONT != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Layout options*/
enum
{
    LV_LAYOUT_OFF = 0,
    LV_LAYOUT_CENTER,
    LV_LAYOUT_COL_L,    /*Column left align*/
    LV_LAYOUT_COL_M,    /*Column middle align*/
    LV_LAYOUT_COL_R,    /*Column right align*/
    LV_LAYOUT_ROW_T,    /*Row top align*/
    LV_LAYOUT_ROW_M,    /*Row middle align*/
    LV_LAYOUT_ROW_B,    /*Row bottom align*/
    LV_LAYOUT_PRETTY,   /*Put as many object as possible in row and begin a new row*/
    LV_LAYOUT_GRID,     /*Align same-sized object into a grid*/
};
typedef uint8_t lv_layout_t;

typedef struct
{
    /*Inherited from 'base_obj' so no inherited ext. */ /*Ext. of ancestor*/
    /*New data for this type */
    uint8_t layout  :4;     /*A layout from 'lv_cont_layout_t' enum*/
    uint8_t hor_fit :1;     /*1: Enable horizontal fit to involve all children*/
    uint8_t ver_fit :1;     /*1: Enable horizontal fit to involve all children*/
} lv_cont_ext_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a container objects
 * @param par pointer to an object, it will be the parent of the new container
 * @param copy pointer to a container object, if not NULL then the new object will be copied from it
 * @return pointer to the created container
 */
lv_obj_t * lv_cont_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a layout on a container
 * @param cont pointer to a container object
 * @param layout a layout from 'lv_cont_layout_t'
 */
void lv_cont_set_layout(lv_obj_t * cont, lv_layout_t layout);


/**
 * Enable the horizontal or vertical fit.
 * The container size will be set to involve the children horizontally or vertically.
 * @param cont pointer to a container object
 * @param hor_en true: enable the horizontal fit
 * @param ver_en true: enable the vertical fit
 */
void lv_cont_set_fit(lv_obj_t * cont, bool hor_en, bool ver_en);

/**
 * Set the style of a container
 * @param cont pointer to a container object
 * @param style pointer to the new style
 */
static inline void lv_cont_set_style(lv_obj_t *cont, lv_style_t * style)
{
    lv_obj_set_style(cont, style);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the layout of a container
 * @param cont pointer to container object
 * @return the layout from 'lv_cont_layout_t'
 */
lv_layout_t lv_cont_get_layout(const lv_obj_t * cont);

/**
 * Get horizontal fit enable attribute of a container
 * @param cont pointer to a container object
 * @return true: horizontal fit is enabled; false: disabled
 */
bool lv_cont_get_hor_fit(const lv_obj_t * cont);

/**
 * Get vertical fit enable attribute of a container
 * @param cont pointer to a container object
 * @return true: vertical fit is enabled; false: disabled
 */
bool lv_cont_get_ver_fit(const lv_obj_t * cont);


/**
 * Get that width reduced by the horizontal padding. Useful if a layout is used.
 * @param cont pointer to a container object
 * @return the width which still fits into the container
 */
lv_coord_t lv_cont_get_fit_width(lv_obj_t * cont);

/**
 * Get that height reduced by the vertical padding. Useful if a layout is used.
 * @param cont pointer to a container object
 * @return the height which still fits into the container
 */
lv_coord_t lv_cont_get_fit_height(lv_obj_t * cont);

/**
 * Get the style of a container
 * @param cont pointer to a container object
 * @return pointer to the container's style
 */
static inline lv_style_t * lv_cont_get_style(const lv_obj_t *cont)
{
    return lv_obj_get_style(cont);
}

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_CONT*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_CONT_H*/
