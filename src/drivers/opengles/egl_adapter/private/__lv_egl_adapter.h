/**
 * @file __lv_egl_adapter.h
 *
 */

#ifndef LV_EGL_ADAPTER_PRIVATE_2_H_
#define LV_EGL_ADAPTER_PRIVATE_2_H_

/*********************
 *      INCLUDES
 *********************/

#include "lv_egl_adapter_typedefs.h"
#include "../lv_egl_adapter_config.h"
#include "lv_egl_adapter_output_core.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_egl_adapter {
    EGLNativeDisplayType            egl_native_display;
    EGLNativeWindowType             egl_native_window;
    EGLDisplay                      egl_display;
    EGLConfig                       egl_config;
    EGLContext                      egl_context;
    EGLSurface                      egl_surface;
    lv_egl_adapter_mode_t      *      best_config;
    void * egl_extern_handle;
    void * gl_extern_handle;
    lv_egl_adapter_config_t    *    requested_visual_config;
    lv_egl_adapter_output_core_t  * output_core;
    bool                            owns_config;
};

struct lv_egl_adapter_sync {
    EGLDisplay display;
    EGLSync sync;
};

#ifdef __cplusplus
extern "C" {
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Normally I would put these definitions inside lv_egl_adapter_mode.h,
 * and the current contents of that file would be in lv_egl_adapter_mode_private.h,
 * but since it's only used interally, putting them in this file accomplishes the
 * same result with a cleaner file structure -mk
 */

bool lv_egl_adapter_mode_is_window(void * config_ptr);
EGLint lv_egl_adapter_mode_get_id(void * config_ptr);
void lv_egl_adapter_mode_init(void * config_ptr, EGLDisplay dpy, EGLConfig config);
lv_egl_adapter_mode_t * lv_egl_adapter_mode_blank();
lv_egl_adapter_mode_t * lv_egl_adapter_mode_create(EGLDisplay dpy, EGLConfig config);
lv_egl_adapter_mode_t * lv_egl_adapter_mode_cleanup(void * config_ptr);

void lv_egl_adapter_mode_print(void * config_ptr, bool is_active);
void lv_egl_adapter_mode_print_header();
void lv_egl_adapter_mode_print_bar();

#ifdef __cplusplus
}
#endif

/**********************
 *      MACROS
 **********************/

#endif // LV_EGL_ADAPTER_PRIVATE_2_H_
