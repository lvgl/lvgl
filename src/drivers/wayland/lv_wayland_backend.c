/**
 * @file lv_wayland_backend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_backend_private.h"

#if LV_USE_WAYLAND

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_OPENGLES
    extern const lv_wayland_backend_ops_t wl_egl_ops;
    extern const lv_wayland_backend_display_ops_t wl_egl_display_ops;
#endif /*LV_USE_OPENGLES*/

#if LV_USE_G2D
    extern const lv_wayland_backend_ops_t wl_g2d_ops;
    extern const lv_wayland_backend_display_ops_t wl_g2d_display_ops;
#endif /*LV_USE_G2D*/

extern const lv_wayland_backend_ops_t wl_shm_ops;
extern const lv_wayland_backend_display_ops_t wl_shm_display_ops;

/**********************
 *      MACROS
 **********************/

static struct {
    const char * name;
    const lv_wayland_backend_ops_t * ops;
    const lv_wayland_backend_display_ops_t * display_ops;
    void * backend_ctx;
} backends[] = {
#if LV_USE_OPENGLES
    {"EGL", &wl_egl_ops, &wl_egl_display_ops, NULL},
#endif /*LV_USE_OPENGLES*/
#if LV_USE_G2D
    {"G2D", &wl_g2d_ops, &wl_g2d_display_ops, NULL},
#endif /*LV_USE_G2D*/
    {"SHM", &wl_shm_ops, &wl_shm_display_ops, NULL},
};

static const size_t backend_count = (sizeof(backends) / sizeof(backends[0]));

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_wayland_backend_init_all(void)
{
    for(size_t i = 0; i < backend_count; ++i) {
        LV_LOG_INFO("Initializing '%s' wayland backend", backends[i].name);
        backends[i].backend_ctx = backends[i].ops->init();
    }
}

void lv_wayland_backend_deinit_all(void)
{
    for(size_t i = 0; i < backend_count; ++i) {
        LV_LOG_INFO("Deinitializing '%s' wayland backend", backends[i].name);
        backends[i].ops->deinit(backends[i].backend_ctx);
    }
}

void lv_wayland_backend_global_handler(struct wl_registry * registry, uint32_t name,
                                       const char * interface, uint32_t version)
{
    for(size_t i = 0; i < backend_count; ++i) {
        backends[i].ops->global_handler(backends[i].backend_ctx, registry, name, interface, version);
    }
}

lv_result_t lv_wayland_backend_init_display(lv_wayland_backend_display_data_t * backend_ddata,
                                            lv_display_t * display,
                                            int32_t width,
                                            int32_t height)

{
    for(size_t i = 0; i < backend_count; ++i) {
        void * display_data = backends[i].display_ops->init_display(backends[i].backend_ctx, display, width, height);
        if(!display_data) {
            LV_LOG_INFO("Failed to initialize display for '%s' wayland backend", backends[i].name);
            continue;
        }
        backend_ddata->ops = backends[i].display_ops;
        backend_ddata->backend_ctx = backends[i].backend_ctx;
        backend_ddata->display_data = display_data;
        LV_LOG_INFO("Initialized display with '%s' wayland backend", backends[i].name);
        return LV_RESULT_OK;
    }
    return LV_RESULT_INVALID;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_WAYLAND*/
