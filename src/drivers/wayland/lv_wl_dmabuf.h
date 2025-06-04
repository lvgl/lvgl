/**
 * @file lv_wl_dmabuf.c
 *
 */

#ifndef LV_WL_DMABUF_H
#define LV_WL_DMABUF_H

#ifdef __cplusplus
extern "C" {
#endif

#if LV_WAYLAND_USE_DMABUF

#include "lv_wayland.h"


#include "lv_wayland_private.h"
#include <wayland_linux_dmabuf.h>
#include <drm/drm_fourcc.h>
#include <stdlib.h>
#include <src/misc/lv_types.h>
#include <string.h>
#include <stdio.h>

#define DMABUF_DEBUG 0

#if DMABUF_DEBUG
#define DMABUF_LOG(fmt, ...) LV_LOG("[DMABUF_LOG] " fmt "\n", ##__VA_ARGS__)
#else
#define DMABUF_LOG(fmt, ...)
#endif
/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#define MAX_BUFFER_PLANES 4

struct buffer {
    int busy;

#if LV_WAYLAND_USE_DMABUF
    struct window * window;
    int plane_count;

    int dmabuf_fds[MAX_BUFFER_PLANES];
    uint32_t strides[MAX_BUFFER_PLANES];
    uint32_t offsets[MAX_BUFFER_PLANES];
    struct wl_buffer * buffer;
    uint32_t width;
    uint32_t height;
#endif

    void * buf_base[MAX_BUFFER_PLANES];
    lv_draw_buf_t * lv_draw_buf;
};

extern struct buffer * decorators_buf[8];

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_WAYLAND_USE_DMABUF*/
#endif /*LV_WL_DMABUF_H*/
