
/**
 * @file lv_demo_vector_graphic.h
 *
 */

#ifndef LV_DEMO_VECTOR_GRAPHIC_H
#define LV_DEMO_VECTOR_GRAPHIC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_demos.h"

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
 * Draw many vector based shapes and paths to canvas.
 * It requires a large amount of RAM for the buffer of the canvas
 */
void lv_demo_vector_graphic_buffered(void);

/**
 * Draw many vector based shapes and paths to canvas directly to the screen.
 * It's slower as the graphics needs to rendered on each rendering cycle.
 */
void lv_demo_vector_graphic_not_buffered(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_VECTOR_GRAPHIC_H*/
