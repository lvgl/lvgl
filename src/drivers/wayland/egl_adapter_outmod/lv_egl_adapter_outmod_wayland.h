/**
 * @file lv_egl_adapter_outmod_wayland.h
 *
 */

#ifndef LV_EGL_ADAPTER_OUTMOD_WAYLAND_H_
#define LV_EGL_ADAPTER_OUTMOD_WAYLAND_H_

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_egl_adapter_outmod_wayland * lv_egl_adapter_outmod_wayland_t;

#ifndef __LV_EGL_ADAPTER_CORE_DEFINED
    #define __LV_EGL_ADAPTER_CORE_DEFINED
    typedef struct lv_egl_adapter_output_core * lv_egl_adapter_output_core_t;
#endif /* __LV_EGL_ADAPTER_CORE_DEFINED */

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_egl_adapter_outmod_wayland_t lv_egl_adapter_outmod_wayland_create();
lv_egl_adapter_output_core_t lv_egl_adapter_outmod_wayland_get_core(void * self);

/**********************
 *      MACROS
 **********************/

#endif /* LV_EGL_ADAPTER_OUTMOD_WAYLAND_H_ */
