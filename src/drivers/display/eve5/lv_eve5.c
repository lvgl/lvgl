/**
 * @file lv_eve5.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_eve5.h"

#if LV_USE_EVE5

#include "../../../core/lv_refr.h"
#include "../../../stdlib/lv_mem.h"

/*********************
 *      DEFINES
 *********************/

#define MAX_REGIONS 128

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    Esd_GpuHandle handle; // GPU memory handle for this region
    lv_area_t area; // Screen coordinates
} rendered_region_t;

typedef struct
{
    EVE_HalContext *hal;
    Esd_GpuAlloc *allocator;
    uint32_t frame_buffer_0;
    uint32_t frame_buffer_1;
    uint32_t current_fb; // 0 or 1 - which buffer we're rendering to
    rendered_region_t pending_regions[MAX_REGIONS];
    int pending_count;
} lv_eve5_driver_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void wait_cb(lv_display_t *disp);
static void composite_to_framebuffer(lv_eve5_driver_t *drvr);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/* Determine bitmap format and bytes per pixel based on LVGL color depth */
#if LV_COLOR_DEPTH == 16
#define EVE_BITMAP_FORMAT RGB565
#define BYTES_PER_PIXEL 2
#elif LV_COLOR_DEPTH == 24
#define EVE_BITMAP_FORMAT RGB888
#define BYTES_PER_PIXEL 3
#elif LV_COLOR_DEPTH == 32
#define EVE_BITMAP_FORMAT ARGB8
#define BYTES_PER_PIXEL 4
#else
#error "Unsupported LV_COLOR_DEPTH - must be 16, 24, or 32"
#endif

/* Framebuffer format is RGB888 (3 bytes per pixel) */
#define FB_BYTES_PER_PIXEL 3

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t *lv_eve5_create(EVE_HalContext *hal, Esd_GpuAlloc *allocator)
{
    EVE_HalContext *phost = hal;

    lv_display_t *disp = lv_display_create(phost->Width, phost->Height);
    if (disp == NULL)
        return NULL;

    /* Allocate driver data */
    lv_eve5_driver_t *drvr = lv_malloc_zeroed(sizeof(lv_eve5_driver_t));
    if (drvr == NULL)
    {
        lv_display_delete(disp);
        return NULL;
    }
    drvr->hal = hal;
    drvr->allocator = allocator;
    drvr->pending_count = 0;

    lv_display_set_driver_data(disp, drvr);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_flush_wait_cb(disp, wait_cb);

    /* Allocate render buffer for LVGL - use partial rendering */
    uint32_t buf_size = phost->Width * 64; /* 64 lines buffer */
    void *buf1 = lv_malloc(buf_size * sizeof(lv_color_t));

    if (buf1 == NULL)
    {
        lv_free(drvr);
        lv_display_delete(disp);
        return NULL;
    }

    lv_display_set_buffers(disp, buf1, NULL, buf_size * sizeof(lv_color_t),
        LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* The HAL has already allocated two framebuffers for the swapchain */
    drvr->frame_buffer_0 = EVE_Hal_rd32(phost, REG_SC0_PTR0);
    drvr->frame_buffer_1 = EVE_Hal_rd32(phost, REG_SC0_PTR1);
    drvr->current_fb = 0;

    /* Initialize allocator */
    allocator->TotalMemorySize = RAM_G_SIZE;
    allocator->TotalReserved = RAM_G_SIZE - RAM_G_AVAILABLE;
    Esd_GpuAlloc_Reset(allocator);

    /* Initial clear of BOTH framebuffers */
    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_clearColorRgb_ex(phost, 0x008000); // Dark green
    EVE_CoDl_clearTag(phost, EVE_TAG_NONE);
    EVE_CoDl_clear(phost, true, true, true);
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_Cmd_waitFlush(phost);

    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_clearColorRgb_ex(phost, 0x008000); // Dark green
    EVE_CoDl_clearTag(phost, EVE_TAG_NONE);
    EVE_CoDl_clear(phost, true, true, true);
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_Cmd_waitFlush(phost);

    return disp;
}

EVE_HalContext *lv_eve5_get_hal(lv_display_t *disp)
{
    lv_eve5_driver_t *drvr = lv_display_get_driver_data(disp);
    return drvr ? drvr->hal : NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    lv_eve5_driver_t *drvr = lv_display_get_driver_data(disp);

    if (drvr == NULL || drvr->hal == NULL)
    {
        lv_display_flush_ready(disp);
        return;
    }

    EVE_HalContext *phost = drvr->hal;

    /* Calculate area dimensions */
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    uint32_t pixel_count = w * h;
    uint32_t size = pixel_count * BYTES_PER_PIXEL;

    /* Check if we have room for more regions */
    if (drvr->pending_count >= MAX_REGIONS)
    {
        lv_display_flush_ready(disp);
        return;
    }

    /* Allocate GPU memory for this region */
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(drvr->allocator, size, GA_ALIGN_128);

    /* Get GPU memory address */
    uint32_t gpu_addr = Esd_GpuAlloc_Get(drvr->allocator, handle);
    if (gpu_addr == GA_INVALID)
    {
        lv_display_flush_ready(disp);
        return;
    }

    /* Copy LVGL rendered data to GPU memory */
    EVE_Hal_wrMem(phost, gpu_addr, px_map, size);

    /* Store this region for compositing */
    drvr->pending_regions[drvr->pending_count].handle = handle;
    drvr->pending_regions[drvr->pending_count].area = *area;
    drvr->pending_count++;

    /* Check if this is the last region for this frame */
    if (lv_display_flush_is_last(disp))
    {
        /* Composite all regions to framebuffer */
        composite_to_framebuffer(drvr);
    }

    /* Tell LVGL we're done with this buffer */
    lv_display_flush_ready(disp);
}

static void composite_to_framebuffer(lv_eve5_driver_t *drvr)
{
    EVE_HalContext *phost = drvr->hal;

    /* Determine which buffer to read from (previous) and write to (current) */
    uint32_t read_fb = drvr->current_fb == 0 ? drvr->frame_buffer_1 : drvr->frame_buffer_0;

    /* Start a new display list */
    EVE_CoCmd_dlStart(phost);

    /* Use scratch bitmap handle for compositing */
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);

    /* First, blit the PREVIOUS framebuffer as the base layer */
    /* This preserves all unchanged areas of the screen */
    EVE_CoDl_bitmapSource(phost, read_fb);
    EVE_CoDl_bitmapLayout(phost, RGB8,
        phost->Width * FB_BYTES_PER_PIXEL, phost->Height);
    EVE_CoDl_bitmapSize(phost, NEAREST,
        BORDER, BORDER,
        phost->Width, phost->Height);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_end(phost);

    /* Now composite each updated region on top */
    for (int i = 0; i < drvr->pending_count; i++)
    {
        rendered_region_t *region = &drvr->pending_regions[i];
        uint32_t gpu_addr = Esd_GpuAlloc_Get(drvr->allocator, region->handle);
        if (gpu_addr == GA_INVALID)
            continue;

        int32_t w = lv_area_get_width(&region->area);
        int32_t h = lv_area_get_height(&region->area);

        /* Setup bitmap for this region */
        EVE_CoDl_bitmapSource(phost, gpu_addr);
        EVE_CoDl_bitmapLayout(phost, EVE_BITMAP_FORMAT,
            w * BYTES_PER_PIXEL, h);
        EVE_CoDl_bitmapSize(phost, NEAREST,
            BORDER, BORDER,
            w, h);

        /* Draw at the correct screen position */
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, region->area.x1, region->area.y1);
        EVE_CoDl_end(phost);
    }

    /* Finish display list and swap */
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);

    /* Toggle current buffer for next frame */
    drvr->current_fb = (drvr->current_fb == 0) ? 1 : 0;

    /* Wait for composition to complete before proceeding */
    EVE_CoCmd_graphicsFinish(phost);
    EVE_Cmd_waitFlush(phost);

    /* Free all temporary GPU memory */
    for (int i = 0; i < drvr->pending_count; i++)
    {
        Esd_GpuAlloc_Free(drvr->allocator, drvr->pending_regions[i].handle);
    }
    drvr->pending_count = 0;
}

static void wait_cb(lv_display_t *disp)
{
    lv_eve5_driver_t *drvr = lv_display_get_driver_data(disp);

    if (drvr == NULL || drvr->hal == NULL)
    {
        return;
    }

    /* Wait for any pending EVE rendering operations to complete */
    EVE_Cmd_waitFlush(drvr->hal);
}

#endif /* LV_USE_EVE5 */