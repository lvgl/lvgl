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
    handlers->invalidate_cache_cb = invalidate_cache;
}

/**********************
 *   STATIC FUNCTIONS
 *********************/

static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    LV_UNUSED(area);
    /* draw_buf->data is not guaranteed to be aligned to the ESP32-P4 L2
     * cache line size (64 B). Without ESP_CACHE_MSYNC_FLAG_UNALIGNED the
     * kernel rejects the call with:
     *
     *   E cache: esp_cache_msync(122): start address: 0x... or the size:
     *           0x... is(are) not aligned with cache line size (0x40)B
     *
     * Failed flushes leave stale PSRAM bytes that the PPA's DMA reads
     * as garbage, producing visible corruption when rendering through
     * buffer-backed canvases. The UNALIGNED flag lets esp_cache_msync
     * internally handle partial leading/trailing cache lines.
     *
     * Cast the const-pointer to void * — esp_cache_msync's signature
     * takes a non-const buffer pointer. */
    esp_cache_msync((void *)(uintptr_t)draw_buf->data, draw_buf->data_size,
                    ESP_CACHE_MSYNC_FLAG_DIR_C2M
                    | ESP_CACHE_MSYNC_FLAG_TYPE_DATA
                    | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
}
#endif /* LV_USE_PPA */
