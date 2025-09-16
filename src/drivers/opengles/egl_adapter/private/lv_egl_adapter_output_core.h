/**
 * @file lv_egl_adapter_output_core.h
 *
 */

#ifndef LV_EGL_ADAPTER_OUTPUT_CORE_H
#define LV_EGL_ADAPTER_OUTPUT_CORE_H

/*********************
 *      INCLUDES
 *********************/
#include "../../../../lv_conf_internal.h"
#include "../../../../misc/lv_array.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#ifndef __LV_EGL_ADAPTER_CORE_DEFINED
    #define __LV_EGL_ADAPTER_CORE_DEFINED
    typedef struct lv_egl_adapter_output_core * lv_egl_adapter_output_core_t;
#endif /* __LV_EGL_ADAPTER_CORE_DEFINED */

// Define the native_state struct
typedef struct native_window_properties * native_window_properties_t;

// Function pointer types for the methods
typedef bool (*init_display_func)(void * self, int * width, int * height, float refresh_rate);
typedef void * (*get_display_func)(void * self);
typedef bool (*create_window_func)(void * self, native_window_properties_t const properties);
typedef void * (*get_window_func)(void * self, native_window_properties_t properties);
typedef void (*set_visible_func)(void * self, bool v);
typedef bool (*should_quit_func)(void * self);
typedef void (*flip_func)(void * self, bool vsync);
typedef void (*native_state_destroy_func)(void ** self);

// Define the native_state struct with function pointers
struct lv_egl_adapter_output_core {
    native_state_destroy_func destroy;
    init_display_func init_display;
    get_display_func display;
    create_window_func create_window;
    get_window_func window;
    set_visible_func visible;
    should_quit_func should_quit;
    flip_func flip;
};

/* Window properties (matches original NativeState::WindowProperties) */
struct native_window_properties {
    int width;
    int height;
    bool fullscreen;
    intptr_t visual_id;
    lv_array_t modifiers; /* ownership: caller owns; lv_egl_adapter_outmod_drm may read it but will not free */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif