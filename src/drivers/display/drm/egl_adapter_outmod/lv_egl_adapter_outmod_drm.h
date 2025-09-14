/**
 * @file lv_egl_adapter_outmod_drm.h
 *
 */

#ifndef LV_EGL_ADAPTER_OUTMOD_DRM_H_
#define LV_EGL_ADAPTER_OUTMOD_DRM_H_

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_egl_adapter_outmod_drm * lv_egl_adapter_outmod_drm_t;

#ifndef __LV_EGL_ADAPTER_CORE_DEFINED
    #define __LV_EGL_ADAPTER_CORE_DEFINED
    typedef struct lv_egl_adapter_output_core * lv_egl_adapter_output_core_t;
#endif /* __LV_EGL_ADAPTER_CORE_DEFINED */

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_egl_adapter_outmod_drm_t lv_egl_adapter_outmod_drm_create(void);
lv_egl_adapter_output_core_t lv_egl_adapter_outmod_drm_get_core(void * nativedrm_ptr);

/**********************
 *      MACROS
 **********************/

#endif /* LV_EGL_ADAPTER_OUTMOD_DRM_H_ */
