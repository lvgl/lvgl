/**
 * @file lv_draw_ppa_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"

#if LV_USE_PPA
#include LV_STDINT_INCLUDE
#include "../../lv_draw_buf_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 *********************/
static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

/**********************
 *   GLOBAL FUNCTIONS
 *********************/

void lv_draw_buf_ppa_init_handlers(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    lv_draw_buf_handlers_t * image_handlers = lv_draw_buf_get_image_handlers();

    handlers->invalidate_cache_cb       = invalidate_cache;
    image_handlers->invalidate_cache_cb = invalidate_cache;
}

/**********************
 *   STATIC FUNCTIONS
 *********************/

static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    esp_cache_msync((void *)PPA_PTR_ALIGN_DOWN(draw_buf->data, CONFIG_CACHE_L1_CACHE_LINE_SIZE),
                    PPA_ALIGN_DOWN(draw_buf->data_size, CONFIG_CACHE_L1_CACHE_LINE_SIZE),
                    ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_TYPE_DATA);
}
#endif /* LV_USE_PPA */
