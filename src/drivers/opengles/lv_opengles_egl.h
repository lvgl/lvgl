/**
 * @file lv_opengles_egl.h
 *
 */

#ifndef LV_OPENGLES_EGL_H
#define LV_OPENGLES_EGL_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_EGL

#include "../../misc/lv_types.h"
#include "../../misc/lv_color.h"
#include "../../misc/lv_array.h"

#include "egl_adapter/private/glad/include/glad/gles2.h"

#include <EGL/egl.h>
#include <EGL/eglplatform.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


typedef struct {
    int32_t hres;
    int32_t vres;
    lv_color_format_t cf;
} lv_egl_config_t;

typedef struct  {
    int width;
    int height;
    bool fullscreen;
    intptr_t visual_id;
    lv_array_t modifiers; /* ownership: caller owns; lv_egl_adapter_outmod_drm may read it but will not free */
} lv_native_window_properties_t;

typedef void * (*lv_egl_init_display_t)(void * driver_data, int32_t width, int32_t height);
typedef void * (*lv_egl_get_display_t)(void * driver_data);
typedef bool (*lv_create_window_t)(void * driver_data, const lv_native_window_properties_t  * properties);
typedef void * (*lv_egl_get_window_t)(void * driver_data, const lv_native_window_properties_t * properties);
typedef void (*lv_egl_set_visible_t)(void * driver_data, bool v);
typedef void (*lv_egl_flip_t)(void * driver_data, bool vsync);
typedef void (*lv_egl_native_state_deinit_t)(void ** driver_data);
typedef size_t (*lv_egl_select_config_t)(void * driver_data, lv_egl_config_t * configs, size_t config_count);

typedef struct {
    lv_egl_select_config_t select_config;
    void * driver_data;
    uint16_t egl_platform;
} lv_egl_interface_t;

typedef struct {
    EGLNativeDisplayType native_display;
    EGLNativeWindowType egl_native_window;
    EGLDisplay egl_display;
    EGLConfig  egl_config;
    EGLContext egl_context;
    EGLSurface egl_surface;
    void * egl_lib_handle;
    void * opengl_lib_handle;
    lv_egl_interface_t interface;

    int32_t width;
    int32_t height;
    float refresh_rate;
    unsigned int offscreen_fbo_count;
    int offscreen_fbo_index;
    bool is_sync_supported;
    bool is_window_initialized;
    lv_array_t fbos;
    lv_array_t fbos_syncs;
    GLuint depth_texture;
    GLenum format_color;
    GLenum format_depth;
} lv_egl_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


lv_egl_ctx_t * lv_opengles_egl_context_create(const lv_egl_interface_t * interface);
void lv_opengles_egl_update(lv_egl_ctx_t * ctx);

void lv_opengles_egl_clear(lv_egl_ctx_t * ctx);
void lv_opengles_egl_context_deinit(lv_egl_ctx_t * ctx);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_EGL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_EGL_H*/
