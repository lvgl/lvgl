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

#if LV_WAYLAND_USE_G2D
    extern const lv_wayland_backend_ops_t wl_g2d_ops;
    extern const lv_wayland_backend_display_ops_t wl_g2d_display_ops;
#endif /*LV_WAYLAND_USE_G2D*/

#if LV_WAYLAND_USE_DMABUF
    extern const lv_wayland_backend_ops_t wl_dmabuf_ops;
    extern const lv_wayland_backend_display_ops_t wl_dmabuf_display_ops;
#endif /*LV_WAYLAND_USE_DMABUF*/

#if LV_WAYLAND_USE_EGL
    extern const lv_wayland_backend_ops_t wl_egl_ops;
    extern const lv_wayland_backend_display_ops_t wl_egl_display_ops;
#endif /*LV_WAYLAND_USE_EGL*/

#if LV_WAYLAND_USE_SHM
    extern const lv_wayland_backend_ops_t wl_shm_ops;
    extern const lv_wayland_backend_display_ops_t wl_shm_display_ops;
#endif /*LV_WAYLAND_USE_SHM*/


#if !LV_WAYLAND_USE_EGL && !LV_WAYLAND_USE_G2D && !LV_WAYLAND_USE_SHM && !LV_WAYLAND_USE_DMABUF
    #error "At least one wayland backend must be selected."
#endif

/* The SHM, G2D and DMA-BUF backends hand a CPU buffer to LVGL, which the OpenGL
 * draw units can't work with*/
#if (LV_WAYLAND_USE_SHM || LV_WAYLAND_USE_G2D || LV_WAYLAND_USE_DMABUF) && (LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG)
    #error "LV_WAYLAND_USE_SHM, LV_WAYLAND_USE_G2D and LV_WAYLAND_USE_DMABUF are not compatible with LV_USE_DRAW_OPENGLES or LV_USE_DRAW_NANOVG. Use LV_WAYLAND_USE_EGL instead."
#endif

/* Ordered by preference: the first backend that can initialize a display wins.
 * On a 3D path only EGL can be enabled so this ordering only matters for 2D.
 */
static struct {
    const char * name;
    const lv_wayland_backend_ops_t * ops;
    const lv_wayland_backend_display_ops_t * display_ops;
    void * backend_data;
    bool available;
} backends[] = {
#if LV_WAYLAND_USE_G2D
    {"G2D", &wl_g2d_ops, &wl_g2d_display_ops, NULL, false},
#endif /*LV_WAYLAND_USE_G2D*/
#if LV_WAYLAND_USE_DMABUF
    {"DMA-BUF", &wl_dmabuf_ops, &wl_dmabuf_display_ops, NULL, false},
#endif /*LV_WAYLAND_USE_DMABUF*/
#if LV_WAYLAND_USE_EGL
    {"EGL", &wl_egl_ops, &wl_egl_display_ops, NULL, false},
#endif /*LV_WAYLAND_USE_EGL*/
#if LV_WAYLAND_USE_SHM
    {"SHM", &wl_shm_ops, &wl_shm_display_ops, NULL, false},
#endif /*LV_WAYLAND_USE_SHM*/
};

static const size_t backend_count = (sizeof(backends) / sizeof(backends[0]));

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_wayland_backend_init_all(void)
{
    for(size_t i = 0; i < backend_count; ++i) {
        LV_LOG_INFO("Initializing '%s' wayland backend", backends[i].name);

        backends[i].backend_data = NULL;
        backends[i].available = (backends[i].ops->init(&backends[i].backend_data) == LV_RESULT_OK);

        if(!backends[i].available) {
            LV_LOG_WARN("Failed to initialize the '%s' wayland backend, it will not be used", backends[i].name);
            backends[i].backend_data = NULL;
        }
    }
}

void lv_wayland_backend_deinit_all(void)
{
    for(size_t i = 0; i < backend_count; ++i) {
        if(!backends[i].available) {
            continue;
        }
        LV_LOG_INFO("Deinitializing '%s' wayland backend", backends[i].name);
        backends[i].ops->deinit(backends[i].backend_data);
        backends[i].backend_data = NULL;
        backends[i].available = false;
    }
}

void lv_wayland_backend_global_handler(struct wl_registry * registry, uint32_t name,
                                       const char * interface, uint32_t version)
{
    LV_ASSERT(registry != NULL);
    LV_ASSERT(interface != NULL);
    for(size_t i = 0; i < backend_count; ++i) {
        if(!backends[i].available) {
            continue;
        }
        backends[i].ops->global_handler(backends[i].backend_data, registry, name, interface, version);
    }
}

lv_result_t lv_wayland_backend_init_display(lv_wayland_backend_display_data_t * backend_ddata,
                                            lv_display_t * display,
                                            int32_t width,
                                            int32_t height)

{
    LV_ASSERT(backend_ddata != NULL);
    LV_ASSERT(display != NULL);

    for(size_t i = 0; i < backend_count; ++i) {
        if(!backends[i].available) {
            continue;
        }

        /* always memset it to zero in case the previous backend modified it*/
        lv_memzero(backend_ddata, sizeof(*backend_ddata));
        void * display_data = backends[i].display_ops->init_display(backends[i].backend_data, display, width, height);
        if(!display_data) {
            LV_LOG_WARN("Failed to initialize a display with the '%s' wayland backend", backends[i].name);
            continue;
        }
        backend_ddata->ops = backends[i].display_ops;
        backend_ddata->backend_data = backends[i].backend_data;
        backend_ddata->display_data = display_data;
        LV_LOG_INFO("Initialized display with '%s' wayland backend", backends[i].name);
        return LV_RESULT_OK;
    }
    return LV_RESULT_INVALID;
}
void lv_wayland_backend_deinit_display(lv_wayland_backend_display_data_t * backend_ddata,
                                       lv_display_t * display)

{
    LV_ASSERT(backend_ddata != NULL);
    LV_ASSERT(display != NULL);
    LV_ASSERT(backend_ddata->ops);

    backend_ddata->ops->deinit_display(backend_ddata->backend_data, display);
    lv_memzero(backend_ddata, sizeof(*backend_ddata));
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_WAYLAND*/
