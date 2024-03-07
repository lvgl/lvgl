/**
 * @file lv_tft_espi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tft_espi.h"
#if LV_USE_TFT_ESPI

#include <TFT_eSPI.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    TFT_eSPI * tft;
} lv_tft_espi_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_tft_espi_create(uint32_t hor_res, uint32_t ver_res, void * buf, uint32_t buf_size_bytes)
{
    lv_tft_espi_t * dsc = (lv_tft_espi_t *)lv_malloc_zeroed(sizeof(lv_tft_espi_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }

    dsc->tft = new TFT_eSPI(hor_res, ver_res);
    dsc->tft->begin();          /* TFT init */
    dsc->tft->setRotation(3);   /* Landscape orientation, flipped */
    lv_display_set_driver_data(disp, (void *)dsc);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, (void *)buf, NULL, buf_size_bytes, LV_DISPLAY_RENDER_MODE_PARTIAL);
    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_tft_espi_t * dsc = (lv_tft_espi_t *)lv_display_get_driver_data(disp);

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    dsc->tft->startWrite();
    dsc->tft->setAddrWindow(area->x1, area->y1, w, h);
    dsc->tft->pushColors((uint16_t *)px_map, w * h, true);
    dsc->tft->endWrite();

    lv_display_flush_ready(disp);

}

#endif /*LV_USE_TFT_ESPI*/
