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
static void flush_cb(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_disp_t * lv_tft_espi_create(uint32_t hor_res, uint32_t ver_res, void * buf, uint32_t buf_size_bytes)
{
    lv_tft_espi_t * dsc = (lv_tft_espi_t *)lv_malloc(sizeof(lv_tft_espi_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;
    lv_memzero(dsc, sizeof(lv_tft_espi_t));

    lv_disp_t * disp = lv_disp_create(hor_res, ver_res);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }

    dsc->tft = new TFT_eSPI(hor_res, ver_res);
    dsc->tft->begin();          /* TFT init */
    dsc->tft->setRotation( 3 ); /* Landscape orientation, flipped */
    lv_disp_set_driver_data(disp, (void *)dsc);
    lv_disp_set_flush_cb(disp, flush_cb);
    lv_disp_set_draw_buffers(disp, (void*)buf, NULL, buf_size_bytes, LV_DISP_RENDER_MODE_PARTIAL);
    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    lv_tft_espi_t * dsc = (lv_tft_espi_t *)lv_disp_get_driver_data(disp);

    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    dsc->tft->startWrite();
    dsc->tft->setAddrWindow( area->x1, area->y1, w, h );
    dsc->tft->pushColors( ( uint16_t * )&color_p->full, w * h, true );
    dsc->tft->endWrite();

    lv_disp_flush_ready( disp );

}

#endif /*LV_USE_TFT_ESPI*/
