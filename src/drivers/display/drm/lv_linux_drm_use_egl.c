/**
 * @file lv_linux_drm_use_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_linux_drm.h"
#if LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL

#include "fullscreen_drm_use_egl/lv_fullscreen_drm_use_egl.h"
#include "../../../display/lv_display.h"

lv_display_t * lv_linux_drm_create(void)
{
    return lv_fullscreen_drm_use_egl_get_display(NULL);
}

void lv_linux_drm_set_file(lv_display_t * disp, const char * file, int64_t connector_id)
{
    LV_UNUSED(disp);
    LV_UNUSED(file);
    LV_UNUSED(connector_id);
    //LV_LOG_INFO(file);
}

#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL*/
