/**
 * @file lv_egl_adapter_protected.h
 *
 */

#ifndef LV_EGL_ADAPTER_PROTECTED_H_
#define LV_EGL_ADAPTER_PROTECTED_H_

/*********************
 *      DEFINES
 *********************/

/*********************
 *      INCLUDES
 *********************/

#include "private/glad/include/glad/egl.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#include "private/lv_egl_adapter_output_core.h"

/**********************
 *      TYPEDEFS
 **********************/

#ifndef __LV_EGL_ADAPTER_CONFIG_DEFINED
    #define __LV_EGL_ADAPTER_CONFIG_DEFINED
    typedef struct lv_egl_adapter_config * lv_egl_adapter_config_t;
#endif /* __LV_EGL_ADAPTER_CONFIG_DEFINED */

struct lv_egl_adapter_sync {
    EGLDisplay display;
    EGLSync sync;
};

#ifndef __LV_EGL_ADAPTER_SYNC_DEFINED
    #define __LV_EGL_ADAPTER_SYNC_DEFINED
    typedef struct lv_egl_adapter_sync * lv_egl_adapter_sync_t;
#endif /* __LV_EGL_ADAPTER_SYNC_DEFINED */

#ifdef __cplusplus
extern "C" {
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_egl_adapter_sync_wait(void *);
struct lv_egl_adapter_sync * lv_egl_adapter_create_sync(void * adapter_ptr);
void lv_egl_adapter_destroy_sync(struct lv_egl_adapter_sync * s);
#ifdef __cplusplus
}
#endif

/* ------------------------- */

#ifndef __LV_EGL_ADAPTER_MODE_DEFINED
    #define __LV_EGL_ADAPTER_MODE_DEFINED
    typedef struct lv_egl_adapter_mode * lv_egl_adapter_mode_t;
#endif /* __LV_EGL_ADAPTER_MODE_DEFINED */

#ifdef __cplusplus
extern "C" {
#endif
void lv_egl_adapter_mode_print(void *, bool is_active);
void lv_egl_adapter_mode_print_header();
void lv_egl_adapter_mode_print_bar();
bool lv_egl_adapter_mode_is_window(void *);
EGLint lv_egl_adapter_mode_get_id(void *);
#ifdef __cplusplus
}
#endif

/* ------------------------- */

#ifndef __LV_EGL_ADAPTER_DEFINED
    #define __LV_EGL_ADAPTER_DEFINED
    typedef struct lv_egl_adapter * lv_egl_adapter_t;
#endif /* __LV_EGL_ADAPTER_CONFIG_DEFINED */

#ifdef __cplusplus
extern "C" {
#endif
bool lv_egl_adapter_init_display(void * adapter_ptr, void * native_display);
bool lv_egl_adapter_init_surface(void * adapter_ptr, void * native_window);
bool lv_egl_adapter_init_extensions(void * adapter_ptr);
lv_egl_adapter_output_core_t lv_egl_adapter_get_output_core(void * adapter_ptr);
void lv_egl_adapter_set_output_core(void * adapter_ptr, lv_egl_adapter_output_core_t core_ptr);
/* Performs a config search, returning a native visual ID on success */
bool lv_egl_adapter_got_native_config(void * adapter_ptr, EGLint * vid, lv_array_t * mods);
void lv_egl_adapter_got_visual_config(void * adapter_ptr, lv_egl_adapter_config_t vc);
bool lv_egl_adapter_is_valid(void * adapter_ptr);
bool lv_egl_adapter_reset(void * adapter_ptr);
void lv_egl_adapter_swap(void * adapter_ptr);
bool lv_egl_adapter_prefers_sync(void * adapter_ptr);
bool lv_egl_adapter_supports_sync();
#ifdef __cplusplus
}
#endif

/**********************
 *      MACROS
 **********************/

#endif // LV_EGL_ADAPTER_PROTECTED_H_
