/**
 * @file lv_svg.h
 *
 */

#ifndef LV_SVG_H
#define LV_SVG_H

/*********************
 *      INCLUDES
 *********************/
#include "../config/lv_conf_internal.h"
#if LV_USE_SVG

#include "../lv_types.h"

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
 * @brief Loading SVG data and creating the DOM tree
 * @param svg_data pointer to the SVG data
 * @param data_len the SVG data length
 */
lv_svg_node_t * lv_svg_load_data(const char * svg_data, uint32_t data_len);

/**
 * @brief Create an SVG DOM node
 * @param parent pointer to the parent node @nullable.
 *               When null, this function creates the document root
 * @return true: an new SVG DOM node, false: NULL
 */
lv_svg_node_t * lv_svg_node_create(lv_svg_node_t * parent);

/**
 * @brief Draw an SVG document to a layer
 * @param layer pointer to the target layer
 * @param svg_doc pointer to the SVG document to draw
 */
void lv_draw_svg(lv_layer_t * layer, const lv_svg_node_t * svg_doc);


/**
 * @brief Delete an SVG DOM subtree
 * @param node pointer to an SVG DOM subtree @nullable
 */
void lv_svg_node_delete(lv_svg_node_t * node);

/**********************
 *      MACROS
 **********************/
#define LV_SVG_NODE_CHILD(n, i) \
    ((lv_svg_node_t *)(LV_TREE_NODE((n))->children[i]))

#define LV_SVG_NODE(n) ((lv_svg_node_t*)(n))

#endif /*LV_USE_SVG*/

#endif /*LV_SVG_H*/
