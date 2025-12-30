/**
 * @file lv_wl_backend_private.h
 *
 */

#ifndef LV_WL_BACKEND_PRIVATE_H
#define LV_WL_BACKEND_PRIVATE_H

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

/**
 * @typedef lv_wayland_backend_init_t
 * @brief Initialize the backend context
 *
 * This function is called once when the Wayland driver is initialized to create
 * the global backend context. The returned pointer will be passed as backend_ctx
 * to all other backend operations.
 *
 * @return Pointer to backend-specific context data, or NULL on failure
 *
 * @note This is called before any displays are created
 * @see lv_wayland_backend_deinit_t
 */
typedef void * (*lv_wayland_backend_init_t)(void);

/**
 * @typedef lv_wayland_backend_deinit_t
 * @brief Deinitialize the backend context
 *
 * This function is called when the Wayland driver is deinitialized. It must
 * clean up all resources allocated in the init function and free the backend
 * context.
 *
 * @param[in] backend_ctx Pointer to the backend context returned by init
 *
 * @note This is called after all displays have been destroyed
 * @see lv_wayland_backend_init_t
 */
typedef void (*lv_wayland_backend_deinit_t)(void * backend_ctx);

/**
 * @typedef lv_wayland_backend_init_display_t
 * @brief Initialize a new display
 *
 * This function is called when creating a new LVGL display on Wayland. It should
 * allocate and initialize per-display resources needed for rendering.
 *
 * @param[in] backend_ctx Pointer to the backend context
 * @param[in] display Pointer to the LVGL display object
 * @param[in] width Initial width of the display in pixels
 * @param[in] height Initial height of the display in pixels
 * @return Pointer to display-specific data, or NULL on failure
 *
 * @note The returned pointer can be retrieved later using
 *       lv_wayland_get_backend_display_data()
 * @note It is expected that each display gets its own data structure in order for a backend
 *       to support multiple displays
 */
typedef void * (*lv_wayland_backend_init_display_t)(void * backend_ctx, lv_display_t * display, int32_t width,
                                                    int32_t height);

/**
 * @typedef lv_wayland_backend_resize_display_t
 * @brief Resize or reconfigure a display
 *
 * This function is called when a display needs to be resized or when its rotation
 * is modified. The backend should update its rendering resources accordingly.
 *
 * @param[in] backend_ctx Pointer to the backend context
 * @param[in] display Pointer to the LVGL display object being resized
 * @return Pointer to updated display-specific data, or NULL on failure
 *
 * @note This may be called multiple times during a display's lifetime
 * @note The returned pointer will replace the previous display data. It can be
 *       retrieved using lv_wayland_get_backend_display_data()
 * @warning The display data is overwritten with the return value of this function
 */
typedef void * (*lv_wayland_backend_resize_display_t)(void * backend_ctx, lv_display_t * display);

/**
 * @typedef lv_wayland_backend_destroy_display_t
 * @brief Destroy a display
 *
 * This function is called when an LVGL display is being destroyed. It must clean up
 * all per-display resources and free the display data that was allocated in
 * init_display.
 *
 * @param[in] backend_ctx Pointer to the backend context
 * @param[in] display Pointer to the LVGL display object being destroyed
 *
 * @note The display data associated with this display must be freed
 */
typedef void (*lv_wayland_backend_destroy_display_t)(void * backend_ctx, lv_display_t * display);

/**
 * @typedef lv_wayland_backend_global_handler_t
 * @brief Handle Wayland global objects
 *
 * This function is called for every global object advertised by the Wayland
 * compositor. The backend can use this to bind to Wayland protocols it requires
 * (e.g., wl_shm, EGL extensions, DMA-BUF protocols, etc.).
 *
 * @param[in] backend_ctx Pointer to the backend context
 * @param[in] registry Wayland registry object
 * @param[in] name Numeric name of the global object
 * @param[in] interface String name of the interface (e.g., "wl_shm")
 * @param[in] version Version number of the interface
 *
 * @note This is called during Wayland connection setup
 * @note The backend should use wl_registry_bind() to bind to needed protocols
 */
typedef void (*lv_wayland_backend_global_handler_t)(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                                                    const char * interface, uint32_t version);

/**
 * @struct lv_wayland_backend_ops_t
 * @brief Wayland backend operations structure
 *
 * This structure defines the complete set of operations that a Wayland backend
 * must implement. All function pointers must be non-NULL.
 *
 * @par Lifecycle Order:
 * 1. init() - Initialize backend context
 * 2. global_handler() - Called for each Wayland global (may be called multiple times)
 * 3. init_display() - Create display (may be called multiple times for multiple displays)
 * 4. resize_display() - Resize display (called as needed)
 * 5. deinit_display() - Destroy display (called once per display)
 * 6. deinit() - Clean up backend context
 */
typedef struct {
    lv_wayland_backend_init_t init;                         /**< Initialize backend context */
    lv_wayland_backend_global_handler_t global_handler;     /**< Handle Wayland global objects */
    lv_wayland_backend_init_display_t init_display;         /**< Initialize a new display */
    lv_wayland_backend_resize_display_t resize_display;     /**< Resize or reconfigure display */
    lv_wayland_backend_destroy_display_t deinit_display;    /**< Destroy a display */
    lv_wayland_backend_deinit_t deinit;                     /**< Deinitialize backend context */
} lv_wayland_backend_ops_t;

extern const lv_wayland_backend_ops_t wl_backend_ops;

/** @brief Get the backend-specific display data
 *
 * Retrieves the per-display data pointer that was returned by the backend's
 * init_display/resize_display functions. This allows the backend to access its own
 * display-specific state and resources.
 *
 * @param[in] display Pointer to the LVGL display object
 * @return Pointer to backend-specific display data
 *
 * @note This returns the value that was returned by lv_wayland_backend_init_display_t
 *       or lv_wayland_backend_resize_display_t
 * @see lv_wayland_backend_init_display_t
 */
void * lv_wayland_get_backend_display_data(lv_display_t * display);

/**
 * @brief Get the Wayland surface for rendering
 *
 * Retrieves the wl_surface object associated with the display window. This is the
 * surface that the backend must use for all rendering operations (attaching buffers,
 * committing frames, etc.).
 *
 * @param[in] display Pointer to the LVGL display object
 * @return Pointer to the Wayland surface for rendering, or NULL if not available
 *
 * @note This surface is managed by the Wayland driver and must not be destroyed
 *       by the backend
 * @note All rendering output should be attached to this surface
 */
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

#endif /*LV_WL_BACKEND_PRIVATE_H*/
