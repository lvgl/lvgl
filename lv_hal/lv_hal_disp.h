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


typedef struct
{
    void * buf1;
    void * buf2;

    /*Used by the library*/
    void * buf_act;
    uint32_t size;                      /*In pixel count*/
    lv_area_t area;
    uint32_t flushing   :1;
}lv_disp_buf_t;


/**
 * Display Driver structure to be registered by HAL
 */
typedef struct _disp_drv_t {
    int user_data;

    lv_coord_t hor_res;

    lv_coord_t ver_res;

    lv_disp_buf_t * buffer;

    /*Write the internal buffer (VDB) to the display. 'lv_flush_ready()' has to be called when finished*/
    void (*disp_flush)(struct _disp_t * disp, const lv_area_t * area, lv_color_t * color_p);

    /*Optional interface functions to use GPU*/
#if USE_LV_GPU
    /*Blend two memories using opacity (GPU only)*/
    void (*mem_blend)(lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);

    /*Fill a memory with a color (GPU only)*/
    void (*mem_fill)(lv_color_t * dest, uint32_t length, lv_color_t color);
#endif

    /*Optional: Set a pixel in a buffer according to the requirements of the display*/
    void (*vdb_wr)(uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa);
} lv_disp_drv_t;

struct _lv_obj_t;

typedef struct _disp_t {
    lv_disp_drv_t driver;
    lv_ll_t scr_ll;
    struct _lv_obj_t * act_scr;
    struct _lv_obj_t * top_layer;
    struct _lv_obj_t * sys_layer;
    lv_area_t inv_areas[LV_INV_BUF_SIZE];
    uint8_t inv_area_joined[LV_INV_BUF_SIZE];
    uint32_t inv_p        :10;
    uint32_t orientation  :2;
    uint32_t vdb_flushing :1;
    uint32_t vdb_act      :1;
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


/**
 * Initialize a display buffer
 * @param disp_buf pointer `lv_disp_buf_t` variable to initialize
 * @param buf1 A buffer to be used by LittlevGL to draw the image.
 *             Always has to specified and can't be NULL.
 *             Can be an array allocated by the user. E.g. `static lv_color_t disp_buf1[1024 * 10]`
 *             Or a memory address e.g. in external SRAM
 * @param buf2 Optionally specify a second buffer to make image rendering and image flushing
 *             (sending to the display) parallel.
 *             In the `disp_drv->flush` you should use DMA or similar hardware to send
 *             the image to the display in the background.
 *             It lets LittlevGL to render next frame into the other buffer while previous is being sent.
 *             Set to `NULL` if unused.
 * @param size size of the `buf1` and `buf2` in pixel count.
 */
void lv_disp_buf_init(lv_disp_buf_t * disp_buf, void * buf1, void * buf2, uint32_t size);


void lv_disp_set_default(lv_disp_t * disp);

lv_disp_t * lv_disp_get_default(void);

lv_disp_buf_t * lv_disp_get_vdb(lv_disp_t * disp);
/**
 * Get the next display.
 * @param disp pointer to the current display. NULL to initialize.
 * @return the next display or NULL if no more. Give the first display when the parameter is NULL
 */
lv_disp_t * lv_disp_get_next(lv_disp_t * disp);


lv_coord_t lv_disp_get_hor_res(lv_disp_t * disp);
lv_coord_t lv_disp_get_ver_res(lv_disp_t * disp);

bool lv_disp_is_double_vdb(lv_disp_t * disp);

bool lv_disp_is_true_double_buffered(lv_disp_t * disp);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
