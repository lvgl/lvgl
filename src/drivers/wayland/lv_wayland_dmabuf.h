/**
 * @file lv_wayland_dmabuf.h
 *
 */

#ifndef LV_WAYLAND_DMABUF_H
#define LV_WAYLAND_DMABUF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_private.h"

#if LV_WAYLAND_USE_DMABUF_PROTOCOL

#include <wayland-client.h>
#include LV_STDBOOL_INCLUDE
#include LV_STDINT_INCLUDE

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief Shared DMA-BUF protocol context.
 *
 * Holds the bound zwp_linux_dmabuf_v1 object and the capabilities detected
 * during the feedback/format negotiation. One instance per backend is enough;
 * it is created in the backend's init() and used as its backend context.
 */
typedef struct {
    struct zwp_linux_dmabuf_v1 * handler;                     /**< Bound protocol object, NULL if unavailable */
    struct zwp_linux_dmabuf_feedback_v1 * feedback;           /**< Default feedback proxy, NULL if unused (protocol < 4) */
    bool supports_rgb565;                                     /**< XRGB8888 / ARGB8888 are always supported */
} lv_wayland_dmabuf_ctx_t;

/**
 * @brief Per-buffer DMA-BUF protocol state.
 *
 * A backend embeds this in its own buffer structure and adds the
 * allocator-specific fields (e.g. a gbm_bo + EGLImage, or an lv_draw_buf).
 * The release tracking (@ref busy) and the wl_buffer lifetime are managed
 * through this struct by the shared helper.
 */
typedef struct {
    struct wl_buffer * wl_buffer;   /**< Created asynchronously; NULL until ready or on failure */
    bool busy;                      /**< true while the compositor still owns the buffer */
} lv_wayland_dmabuf_buffer_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Reset a DMA-BUF context to a clean state.
 * @param ctx context to initialize
 */
void lv_wayland_dmabuf_ctx_init(lv_wayland_dmabuf_ctx_t * ctx);

/**
 * @brief Release the resources held by a DMA-BUF context.
 * @param ctx context to deinitialize
 */
void lv_wayland_dmabuf_ctx_deinit(lv_wayland_dmabuf_ctx_t * ctx);

/**
 * @brief Handle a wl_registry global announcement.
 *
 * Binds the zwp_linux_dmabuf_v1 interface and starts the format/feedback
 * negotiation when the matching global is advertised.
 *
 * @param ctx        DMA-BUF context
 * @param registry   the wl_registry
 * @param name       numeric name of the global
 * @param interface  interface string
 * @param version    advertised interface version
 * @return true if this global was the dmabuf interface and was handled
 */
bool lv_wayland_dmabuf_registry_handle_global(lv_wayland_dmabuf_ctx_t * ctx, struct wl_registry * registry,
                                              uint32_t name, const char * interface, uint32_t version);

/**
 * @brief Convert an LVGL color format to a DRM fourcc code.
 * @param cf LVGL color format
 * @return the matching DRM_FORMAT_* code (defaults to ARGB8888)
 */
uint32_t lv_wayland_dmabuf_cf_to_drm(lv_color_format_t cf);

/**
 * @brief Create a wl_buffer from a DMA-BUF file descriptor.
 *
 * Wraps the zwp_linux_buffer_params_v1 sequence. The resulting wl_buffer is
 * delivered asynchronously into @p buf->wl_buffer, so the caller must perform a
 * wl_display roundtrip and then verify that @p buf->wl_buffer is non-NULL. The
 * buffer's release listener clears @p buf->busy when the compositor is done.
 *
 * @param ctx        DMA-BUF context
 * @param buf        per-buffer protocol state (embedded in the backend buffer)
 * @param dmabuf_fd  the DMA-BUF file descriptor to share
 * @param width      buffer width in pixels
 * @param height     buffer height in pixels
 * @param stride     row stride in bytes
 * @param offset     plane offset in bytes
 * @param drm_cf     DRM fourcc of the buffer
 * @param modifier   DRM format modifier of the buffer. The caller must pass the
 *                   modifier the allocation actually has, not the one it asked
 *                   for, as the compositor interprets the memory layout with it.
 */
void lv_wayland_dmabuf_create_buffer(lv_wayland_dmabuf_ctx_t * ctx, lv_wayland_dmabuf_buffer_t * buf,
                                     int dmabuf_fd, uint32_t width, uint32_t height,
                                     uint32_t stride, uint32_t offset, uint32_t drm_cf,
                                     uint64_t modifier);

/**
 * @brief Flush pending buffer-creation requests and wait for the compositor
 *        to respond.
 *
 * Performs a wl_display_roundtrip() so that any zwp_linux_buffer_params_v1
 * requests queued by prior calls to lv_wayland_dmabuf_create_buffer() are
 * sent and their `created`/`failed` events are processed. Call this once
 * after creating one or more buffers, before touching buf->wl_buffer.
 *
 * After this function the caller is expected to call
 * @ref lv_wayland_dmabuf_buffer_is_valid to ensure the buffer was created
 * successfully.
 *
 * @param ctx  DMA-BUF context
 * @return     true if the roundtrip itself completed (e.g. no disconnect)
 */
bool lv_wayland_dmabuf_commit_creation(lv_wayland_dmabuf_ctx_t * ctx);

/**
 * @brief Check whether a buffer's asynchronous creation succeeded.
 *
 * Only meaningful after lv_wayland_dmabuf_commit_creation() has returned for the
 * roundtrip in which @p buf was created. Returns false if creation is still
 * pending (shouldn't happen post-commit) or if the compositor sent
 * zwp_linux_buffer_params_v1::failed.
 *
 * @param buf  per-buffer protocol state previously passed to
 *             lv_wayland_dmabuf_create_buffer()
 * @return     true if buffer is ready to use
 */
bool lv_wayland_dmabuf_buffer_is_valid(const lv_wayland_dmabuf_buffer_t * buf);

/**
 * @brief Check whether a buffer's is ready to be attached again.
 *
 * @param buf  per-buffer protocol state previously passed to
 *             lv_wayland_dmabuf_create_buffer()
 * @return     true if buffer is ready to be attached again
 */
bool lv_wayland_dmabuf_buffer_is_ready(const lv_wayland_dmabuf_buffer_t * buf);

/**
 * @brief Attach a buffer to a surface and mark it busy.
 *
 * Wraps wl_surface_attach() + wl_surface_damage() + wl_surface_commit(),
 * setting buf->busy = true beforehand so the caller can immediately treat the
 * buffer as in-flight. buf->busy is cleared later by the release listener
 * registered in lv_wayland_dmabuf_create_buffer(), when the compositor sends
 * wl_buffer::release.
 *
 * @param ctx      DMA-BUF context
 * @param surface  wl_surface to attach the buffer to
 * @param buf      per-buffer protocol state; caller must ensure
 *                 lv_wayland_dmabuf_buffer_is_valid(buf) is true
 * @param width    damage width in pixels
 * @param height   damage height in pixels
 */
void lv_wayland_dmabuf_buffer_attach(lv_wayland_dmabuf_ctx_t * ctx, struct wl_surface * surface,
                                     lv_wayland_dmabuf_buffer_t * buf,
                                     uint32_t width, uint32_t height);

#endif /*LV_WAYLAND_USE_DMABUF_PROTOCOL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WAYLAND_DMABUF_H*/
