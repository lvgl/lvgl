/**
 * @file lv_opengles_egl_private.h
 *
 */

#ifndef LV_OPENGLES_H
#define LV_OPENGLES_H

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

#include <EGL/egl.h>
#include <EGL/eglplatform.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    EGLint id;
    EGLint max_width;
    EGLint max_height;
    EGLint buffer_size;
    EGLint depth;
    EGLint stencil;
    EGLint samples;
    EGLint surface_type;
    EGLint r_bits;
    EGLint g_bits;
    EGLint b_bits;
    EGLint a_bits;
} lv_egl_config_t;

typedef struct {
    EGLint visual_id;
} lv_egl_native_window_properties_t;

typedef void * (*lv_egl_init_display_t)(void * driver_data, int32_t width, int32_t height);
typedef void * (*lv_egl_get_display_t)(void * driver_data);
typedef void * (*lv_create_window_t)(void * driver_data, const lv_egl_native_window_properties_t * props);
typedef void (*lv_destroy_window_t)(void * driver_data, void * native_window);

typedef void (*lv_egl_set_visible_t)(void * driver_data, bool v);
typedef void (*lv_egl_flip_t)(void * driver_data, bool vsync);
typedef void (*lv_egl_native_state_deinit_t)(void ** driver_data);
typedef size_t (*lv_egl_select_config_t)(void * driver_data, lv_egl_config_t * configs, size_t config_count);

typedef struct {
    lv_egl_select_config_t select_config;
    void * driver_data;
    void * native_display;
    uint16_t egl_platform;
    lv_create_window_t create_window_cb;
    lv_destroy_window_t destroy_window_cb;
    lv_egl_flip_t flip_cb;
} lv_egl_interface_t;


typedef struct  {
    EGLNativeWindowType native_window;
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
    bool vsync;
    bool is_sync_supported;
    bool is_window_initialized;
} lv_opengles_egl_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_opengles_egl_t * lv_opengles_egl_context_create(const lv_egl_interface_t * interface);
lv_color_format_t lv_opengles_egl_color_format_from_egl_config(const lv_egl_config_t * config);

void lv_opengles_egl_update(lv_opengles_egl_t * ctx);
void lv_opengles_egl_clear(lv_opengles_egl_t * ctx);
void lv_opengles_egl_context_destroy(lv_opengles_egl_t * ctx);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_EGL*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_H*/
