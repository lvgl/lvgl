/**
 * @file hal_disp.h
 *
 * @description Display Driver HAL interface header file
 *
 */

#ifndef HAL_DISP_H
#define HAL_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include "lv_hal.h"
#include "misc/gfx/color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Display Driver structure to be registered by HAL
 */
typedef struct _disp_drv_t {
    int32_t hor_res;
    int32_t ver_res;
    void (*fill)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_t color);
    void (*map)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const color_t * color_p);
    void (*copy)(color_t * dest, const color_t * src, uint32_t length, opa_t opa);
} lv_disp_drv_t;

typedef struct _disp_t {
    lv_disp_drv_t driver;
    struct _disp_t *next;
} lv_disp_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * Register an initialized display driver.
 * Automatically set the first display as active.
 * @param driver pointer to an initialized 'lv_disp_drv_t' variable (can be local variable)
 * @return pointer to the new display or NULL on error
 */
lv_disp_t * lv_disp_register(lv_disp_drv_t *driver);

/**
 * Set the active display
 * @param disp pointer to a display (return value of 'lv_disp_register')
 */
void lv_disp_set_active(lv_disp_t * disp);

/**
 * Get a pointer to the active display
 * @return pointer to the active display
 */
lv_disp_t * lv_disp_get_active(void);

/**
 * Get the next display.
 * @param disp pointer to the current display. NULL to initialize.
 * @return the next display or NULL if no more. Give the first display when the parameter is NULL
 */
lv_disp_t * lv_disp_next(lv_disp_t * disp);

/**
 * Fill a rectangular area with a color on the active display
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color fill color
 */
void lv_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_t color);

/**
 * Put a color map to a rectangular area on the active display
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_map pointer to an array of colors
 */
void lv_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const color_t * color_map);

/**
 * Copy pixels to a destination memory applying opacity
 * In 'lv_disp_drv_t' 'copy' is optional. (NULL to use the built-in copy function)
 * @param dest a memory address. Copy 'src' here.
 * @param src pointer to pixel map. Copy it to 'dest'.
 * @param length number of pixels in 'src'
 * @param opa opacity (0, OPA_TRANSP: transparent ... 255, OPA_COVER, fully cover)
 */
void lv_disp_copy(color_t * dest, const color_t * src, uint32_t length, opa_t opa);

/**
 * Shows if 'copy' is supported or not
 * @return false: 'copy' is not supported in the drover; true: 'copy' is supported in the driver
 */
bool lv_disp_is_copy_supported(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
