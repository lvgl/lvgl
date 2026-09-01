/**
 * @file lv_test_display_egl.c
 *
 * Headless EGL display for NanoVG unit testing.
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl_public.h"

#if LV_USE_TEST && LV_USE_DRAW_NANOVG && LV_USE_NANOVG_TEST_HEADLESS

#include "../../drivers/opengles/lv_opengles_pbuffer.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void egl_resolution_changed_cb(lv_event_t * e);
static void egl_color_format_changed_cb(lv_event_t * e);
static void delete_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

typedef struct {
    int32_t fixed_hor_res;
    int32_t fixed_ver_res;
} lv_test_display_egl_data_t;


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_test_display_egl_create(int32_t hor_res, int32_t ver_res)
{
    lv_display_t * disp = lv_opengles_pbuffer_create(hor_res, ver_res);
    if(!disp) {
        LV_LOG_ERROR("Failed to create the EGL pbuffer display for NanoVG headless testing");
        return NULL;
    }
    lv_test_display_egl_data_t * ddata = lv_malloc(sizeof(*ddata));
    if(!ddata) {
        LV_LOG_ERROR("failed to allocate display data");
        lv_opengles_pbuffer_t * pbuffer = lv_opengles_pbuffer_get_handle(disp);
        lv_display_delete(disp);
        lv_opengles_pbuffer_destroy(pbuffer);
        return NULL;
    }

    ddata->fixed_hor_res = hor_res;
    ddata->fixed_ver_res = ver_res;

    lv_display_set_user_data(disp, ddata);

    /* We don't support resolution nor color format changes but bind the events anyway
     * to catch and fail in case that happens
     */
    lv_display_add_event_cb(disp, egl_resolution_changed_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(disp, egl_color_format_changed_cb, LV_EVENT_COLOR_FORMAT_CHANGED, NULL);
    lv_display_add_event_cb(disp, delete_event_cb, LV_EVENT_DELETE, NULL);

    if(lv_opengles_pbuffer_get_gles_version(disp) < 3) {
        LV_LOG_WARN("Only an OpenGL ES 2 context was available; glTF tests will not run");
    }

    LV_LOG_INFO("EGL headless test display created (%" LV_PRId32 "x%" LV_PRId32 ")", hor_res, ver_res);
    return disp;
}

void * lv_test_display_egl_get_context(lv_display_t * disp)
{
    return lv_opengles_pbuffer_get_handle(disp);
}

void lv_test_display_egl_cleanup(void * egl_ctx)
{
    /* Called after lv_deinit(), when the NanoVG draw unit has already been destroyed
     * and can no longer issue GL calls. */
    lv_opengles_pbuffer_destroy(egl_ctx);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void egl_resolution_changed_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    int32_t hor_res = lv_display_get_original_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_original_vertical_resolution(disp);
    lv_test_display_egl_data_t * ddata = lv_display_get_user_data(disp);
    LV_ASSERT(ddata != NULL);

    /* Resizing would require recreating the FBO and the draw buffer, which this
     * headless test display does not support. Fail loudly so the offending test is
     * caught rather than silently producing corrupt screenshots. */
    LV_ASSERT_FORMAT_MSG(hor_res == ddata->fixed_hor_res && ver_res == ddata->fixed_ver_res,
                         "NanoVG headless test display does not support changing resolution "
                         "(%" LV_PRId32 "x%" LV_PRId32 " -> %" LV_PRId32 "x%" LV_PRId32 "); "
                         "skip or adjust this test for NanoVG",
                         ddata->fixed_hor_res, ddata->fixed_ver_res, hor_res, ver_res);
}

static void egl_color_format_changed_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_color_format_t cf = lv_display_get_color_format(disp);

    /* The readback path always reads the FBO back as straight-alpha 32bpp BGRA
     * (XRGB8888/ARGB8888) using a fixed draw buffer, ignoring disp->color_format.
     * Other formats (incl. premultiplied) can't be represented, so reject them. */
    LV_ASSERT_FORMAT_MSG(cf == LV_COLOR_FORMAT_XRGB8888 || cf == LV_COLOR_FORMAT_ARGB8888,
                         "NanoVG headless test display only supports XRGB8888/ARGB8888 "
                         "(got color format %d); skip or adjust this test for NanoVG",
                         (int)cf);
}

static void delete_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_free(lv_display_get_user_data(disp));
    lv_display_set_user_data(disp, NULL);
}

#endif /* LV_USE_TEST && LV_USE_DRAW_NANOVG && LV_USE_NANOVG_TEST_HEADLESS */
