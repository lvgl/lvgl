/**
 * @file lv_hal_disp.h
 *
 * @description Display Driver HAL interface header file
 *
 */

#ifndef LV_HAL_DISP_H
#define LV_HAL_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include "lv_hal.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_ll.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_INV_BUF_SIZE
#define LV_INV_BUF_SIZE    32    /*Buffer size for invalid areas */
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct _disp_t;

/**
 * Display Driver structure to be registered by HAL
 */
typedef struct _disp_drv_t {
    lv_coord_t hor_res;

    lv_coord_t ver_res;

    /*Write the internal buffer (VDB) to the display. 'lv_flush_ready()' has to be called when finished*/
    void (*disp_flush)(struct _disp_t * disp, const lv_area_t * area, lv_color_t * color_p);

    /*Fill an area with a color on the display*/
    void (*disp_fill)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);

    /*Write pixel map (e.g. image) to the display*/
    void (*disp_map)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p);

    /*Optional interface functions to use GPU*/
#if USE_LV_GPU
    /*Blend two memories using opacity (GPU only)*/
    void (*mem_blend)(lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);

    /*Fill a memory with a color (GPU only)*/
    void (*mem_fill)(lv_color_t * dest, uint32_t length, lv_color_t color);
#endif

#if LV_VDB_SIZE
    /*Optional: Set a pixel in a buffer according to the requirements of the display*/
    void (*vdb_wr)(uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa);
#endif
} lv_disp_drv_t;

struct _lv_obj_t;

typedef struct _disp_t {
    void * user_data;
    lv_disp_drv_t driver;
    lv_ll_t scr_ll;
    struct _lv_obj_t * act_scr;
    struct _lv_obj_t * top_layer;
    struct _lv_obj_t * sys_layer;
    lv_area_t inv_areas[LV_INV_BUF_SIZE];
    uint8_t inv_area_joined[LV_INV_BUF_SIZE];
    uint16_t inv_p        :10;
    uint16_t orientation  :2;
} lv_disp_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a display driver with default values.
 * It is used to surly have known values in the fields ant not memory junk.
 * After it you can set the fields.
 * @param driver pointer to driver variable to initialize
 */
void lv_disp_drv_init(lv_disp_drv_t *driver);

/**
 * Register an initialized display driver.
 * Automatically set the first display as active.
 * @param driver pointer to an initialized 'lv_disp_drv_t' variable (can be local variable)
 * @return pointer to the new display or NULL on error
 */
lv_disp_t * lv_disp_drv_register(lv_disp_drv_t *driver);


lv_disp_t * lv_disp_get_last(void);

/**
 * Get the next display.
 * @param disp pointer to the current display. NULL to initialize.
 * @return the next display or NULL if no more. Give the first display when the parameter is NULL
 */
lv_disp_t * lv_disp_get_next(lv_disp_t * disp);


lv_coord_t lv_disp_get_hor_res(lv_disp_t * disp);
lv_coord_t lv_disp_get_ver_res(lv_disp_t * disp);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
