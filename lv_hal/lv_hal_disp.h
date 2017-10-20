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
 * Register Display driver
 *
 * @param driver Display driver structure
 * @return 0 on success, -ve on error
 */
lv_disp_t * lv_disp_register(lv_disp_drv_t *driver);

/**
 * Set Active Display by ID
 *
 * @param id Display ID to set as active
 * @return 0 on success, -ve on error
 */
void lv_disp_set_active(lv_disp_t * disp);

/**
 * Get Active Display
 *
 * @return Active ID of display on success else -ve on error
 */
lv_disp_t * lv_disp_get_active(void);

/**
 * Fill a rectangular area with a color
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color fill color
 */
void lv_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, color_t color);

/**
 * Put a color map to a rectangular area
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_p pointer to an array of colors
 */
void lv_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const color_t * color_p);

/**
 * Copy pixels to destination memory using opacity with GPU (hardware accelerator)
 * @param dest a memory address. Copy 'src' here.
 * @param src pointer to pixel map. Copy it to 'dest'.
 * @param length number of pixels in 'src'
 * @param opa opacity (0, OPA_TRANSP: transparent ... 255, OPA_COVER, fully cover)
 */
void lv_disp_color_cpy(color_t * dest, const color_t * src, uint32_t length, opa_t opa);

bool lv_disp_is_accelerated(void);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
