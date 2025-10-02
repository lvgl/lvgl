/**
 * @file lv_wl_backend.h
 *
 */

#ifndef LV_WL_BACKEND_H
#define LV_WL_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland.h"

#if LV_USE_WAYLAND

#include <wayland-client.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void * (*lv_wayland_backend_init_t)(void);
typedef void (*lv_wayland_backend_deinit_t)(void * backend_data);

typedef void * (*lv_wayland_backend_init_display_t)(void * backend_data, lv_display_t * display, int32_t width,
                                                    int32_t height);
typedef void * (*lv_wayland_backend_resize_display_t)(void * backend_data, lv_display_t * display);
typedef void (*lv_wayland_backend_destroy_display_t)(void * backend_data, lv_display_t * display);

typedef void (*lv_wayland_backend_global_handler_t)(void * backend_data, struct wl_registry * registry, uint32_t name,
                                                    const char * interface, uint32_t version);

typedef struct {
    lv_wayland_backend_init_t init;
    lv_wayland_backend_global_handler_t global_handler;
    lv_wayland_backend_init_display_t init_display;
    lv_wayland_backend_resize_display_t resize_display;
    lv_wayland_backend_destroy_display_t deinit_display;
    lv_wayland_backend_deinit_t deinit;
} lv_wayland_backend_ops_t;

extern const lv_wayland_backend_ops_t wl_backend_ops;

void * lv_wayland_get_backend_display_data(lv_display_t * display);
struct wl_surface * lv_wayland_get_window_surface(lv_display_t * display);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_WAYLAND*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WL_BACKEND_H*/
