/**
 * @file lv_egl_adapter.h
 * @brief EGL adapter for LVGL — creation and cleanup helpers.
 */

#ifndef LV_EGL_ADAPTER_H_
#define LV_EGL_ADAPTER_H_


/*********************
 *      DEFINES
 *********************/
//#define LV_EXTRA_EGL_INFO
//#define LV_EXTRA_EGL_INFO_MORE
#ifdef LV_EXTRA_EGL_INFO_MORE
    #ifndef LV_EXTRA_EGL_INFO
        #define LV_EXTRA_EGL_INFO
    #endif
#endif

/*********************
 *      INCLUDES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#ifndef __LV_EGL_ADAPTER_CONFIG_DEFINED
    #define __LV_EGL_ADAPTER_CONFIG_DEFINED
    typedef struct lv_egl_adapter_config * lv_egl_adapter_config_t;
#endif /* __LV_EGL_ADAPTER_CONFIG_DEFINED */

#ifndef __LV_EGL_ADAPTER_DEFINED
    #define __LV_EGL_ADAPTER_DEFINED
    typedef struct lv_egl_adapter * lv_egl_adapter_t;
#endif /* __LV_EGL_ADAPTER_CONFIG_DEFINED */

#ifdef __cplusplus
extern "C" {
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Create and initialize an lv_egl_adapter instance.
 * @param config Visual and platform configuration used to construct the adapter.
 *               The config is an opaque handle describing windowing, GL/EGL,
 *               and visual properties required by the adapter.
 * @return Opaque handle to the newly created adapter, or NULL on failure.
 *
 * The returned adapter handle must be managed by the caller. Use
 * lv_egl_adapter_cleanup() to release the adapter and any associated resources
 * when no longer needed.
 */
lv_egl_adapter_t lv_egl_adapter_create(lv_egl_adapter_config_t config);

/**
 * @brief Create and initialize an lv_egl_adapter instance with automatic configuration.
 * @return Opaque handle to the newly created adapter, or NULL on failure.
 *
 * The returned adapter handle must be managed by the caller. Use
 * lv_egl_adapter_cleanup() to release the adapter and any associated resources
 * when no longer needed.  Unlike the other create() function, the output
 * of this one assumes ownership of the lv_egl_adapter_config_t and will
 * free it automatically during cleanup.
 */
lv_egl_adapter_t lv_egl_adapter_create_auto(void);

/**
 * @brief Cleanup and free adapter, config, and canvas resources.
 * @param adapter_ptr Pointer to a variable holding the adapter opaque pointer.
 *                    On return, *adapter_ptr will be set to NULL.
 * @param config_ptr  Pointer to a variable holding the config opaque pointer.
 *                    On return, *config_ptr will be set to NULL.
 * @param cnvs_ptr    Pointer to a variable holding the canvas opaque pointer.
 *                    On return, *cnvs_ptr will be set to NULL.
 *
 * Safely releases resources associated with the adapter, its visual config,
 * and the canvas instance. Any of the three pointer arguments may be NULL,
 * in which case they are ignored. This function attempts to handle partially
 * initialized states gracefully. After return, any non-NULL pointers passed
 * will have been freed and nulled.
 */
void lv_egl_adapter_cleanup(void ** adapter_ptr, void ** config_ptr, void ** cnvs_ptr);

#ifdef __cplusplus
}
#endif

/**********************
 *      MACROS
 **********************/

#endif // LV_EGL_ADAPTER_H_
