/**
 * @file lv_tileview.h
 *
 */


#ifndef LV_TILEVIEW_H
#define LV_TILEVIEW_H

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

#if USE_LV_TILEVIEW != 0

#include "../lv_objx/lv_page.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/



/* parametes: pointer to a tileview object, x, y (tile coordinates to load)
 * return: LV_RES_INV: to prevent the loading of the tab;  LV_RES_OK: if everything is fine*/
typedef lv_res_t (*lv_tileview_action_t)(lv_obj_t *, lv_coord_t, lv_coord_t);

/*Data of tileview*/
typedef struct {
    lv_page_ext_t page;
    /*New data for this type */
    const lv_point_t * valid_pos;
    uint16_t anim_time;
    lv_tileview_action_t action;
    lv_point_t act_id;
    uint8_t drag_top_en     :1;
    uint8_t drag_bottom_en  :1;
    uint8_t drag_left_en    :1;
    uint8_t drag_right_en   :1;
    uint8_t drag_hor   :1;
    uint8_t drag_ver   :1;
} lv_tileview_ext_t;


/*Styles*/
enum {
    LV_TILEVIEW_STYLE_BG,
};
typedef uint8_t lv_tileview_style_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a tileview objects
 * @param par pointer to an object, it will be the parent of the new tileview
 * @param copy pointer to a tileview object, if not NULL then the new object will be copied from it
 * @return pointer to the created tileview
 */
lv_obj_t * lv_tileview_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Register an object on the tileview. The register object will able to slide the tileview
 * @param element pointer to an object
 */
void lv_tileview_add_element(lv_obj_t * element);

/*=====================
 * Setter functions
 *====================*/


/**
 * Set the valid position's indices. The scrolling will be possible only to these positions.
 * @param tileview pointer to a Tileview object
 * @param valid_pos array width the indices. E.g. `lv_point_t p[] = {{0,0}, {1,0}, {1,1}, {LV_COORD_MIN, LV_COORD_MIN}};`
 * Must be closed with `{LV_COORD_MIN, LV_COORD_MIN}`. Only the pointer is saved so can't be a local variable.
 */
void lv_tileview_set_valid_positions(lv_obj_t * tileview, const lv_point_t * valid_pos);

/**
 * Set the tile to be shown
 * @param tileview pointer to a tileview object
 * @param x column id (0, 1, 2...)
 * @param y line id (0, 1, 2...)
 * @param anim_en true: move with animation
 */
void lv_tileview_set_tile_act(lv_obj_t * tileview, lv_coord_t x, lv_coord_t y, bool anim_en);

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param tileview pointer to a Tileview
 * @param en true or false to enable/disable end flash
 */
static inline void lv_tileview_set_edge_flash(lv_obj_t * tileview, bool en)
{
    lv_page_set_edge_flash(tileview, en);
}

/**
 * Set a style of a tileview.
 * @param tileview pointer to tileview object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_tileview_set_style(lv_obj_t * tileview, lv_tileview_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the scroll propagation property
 * @param tileview pointer to a Tileview
 * @return true or false
 */
static inline bool lv_tileview_get_edge_flash(lv_obj_t * tileview)
{
    return lv_page_get_edge_flash(tileview);
}

/**
 * Get style of a tileview.
 * @param tileview pointer to tileview object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_tileview_get_style(const lv_obj_t * tileview, lv_tileview_style_t type);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_TILEVIEW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_TILEVIEW_H*/
