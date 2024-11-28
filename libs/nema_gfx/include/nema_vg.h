/* TSI 2023.xmo */
/*******************************************************************************
 * Copyright (c) 2023 Think Silicon Single Member PC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this header file and/or associated documentation files to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Materials, and to permit persons to whom the Materials are furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * NEMAGFX API. THE UNMODIFIED, NORMATIVE VERSIONS OF THINK-SILICON NEMAGFX
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT:
 *   https://think-silicon.com/products/software/nemagfx-api
 *
 *  The software is provided 'as is', without warranty of any kind, express or
 *  implied, including but not limited to the warranties of merchantability,
 *  fitness for a particular purpose and noninfringement. In no event shall
 *  Think Silicon Single Member PC be liable for any claim, damages or other
 *  liability, whether in an action of contract, tort or otherwise, arising
 *  from, out of or in connection with the software or the use or other dealings
 *  in the software.
 ******************************************************************************/

/**
 * @file
 * @brief Core NemaVG API drawing and initialization functions.
 *
 */

#ifndef __NEMA_VG_H__
#define __NEMA_VG_H__

#include "nema_core.h"
#include "nema_sys_defs.h"
#include "nema_vg_path.h"
#include "nema_vg_paint.h"
#include "nema_vg_context.h"

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------------------------------------------------------
//                           SETUP
// -------------------------------------------------------------------------------

/** \brief Initializes NemaVG library and allocates the stencil buffer to the default memory pool (NEMA_MEM_POOL_FB)
 * Call either this or nema_vg_init_stencil_pool to allocate the stencil buffer to a different memory pool
 * or nema_vg_init_stencil_prealloc to provide the stencil buffer
 * \param width Framebuffer width
 * \param height Framebuffer height
 */
void nema_vg_init(int width, int height);

/** \brief Initializes NemaVG library and allocate the stencil buffer in a specific memory pool.
 * Call either this or nema_vg_init to allocate the stencil buffer to the default memory pool (NEMA_MEM_POOL_FB)
 * or nema_vg_init_stencil_prealloc to provide the stencil buffer
 * \param width Stencil buffer width - Must be the first multiple of 4 of the framebuffer width
 * \param height Stencil buffer height - Must be the first multiple of 4 of the framebuffer height
 * \param pool Memory pool for allocating the stencil buffer (memory pools are platform specific and defined in nema_sys_defs.h file)
 */
void nema_vg_init_stencil_pool(int width, int height, int pool);

/** \brief Initializes NemaVG library without allocating the stencil buffer which is provided by the user.
 * Call either this or nema_vg_init to allocate the stencil buffer to the default memory pool (NEMA_MEM_POOL_FB)
 * or nema_vg_init_stencil_pool to allocate the stencil buffer to a different memory pool
 * \param width Stencil buffer width - Must be the first multiple of 4 of the framebuffer width
 * \param height Stencil buffer height - Must be the first multiple of 4 of the framebuffer height
 * \param stencil_bo stencil buffer
 */
void nema_vg_init_stencil_prealloc(int width, int height, nema_buffer_t stencil_bo);


/** \brief Reinitialize NemaVG library after a gpu powerofff
 *
 */
void nema_vg_reinit(void);

/** \brief Deinitialize NemaVG library. Free memory from implicitly allocated objects (stencil buffer
 *  if created inside the library, lut buffer and tsvgs' path, paint and gradient buffers)
 *
 *
 */
void nema_vg_deinit(void);

/** \brief Initialize NemaVG library for a new thread.
 * Must be called for every new thread that is used.
 *
 *
 */
void nema_vg_thread_init(void);


// -------------------------------------------------------------------------------
//                           PATH DRAW
// -------------------------------------------------------------------------------

/** \brief Draw a path using a specified paint object
 *
 * \param path Pointer (handle) to the path that will be drawn
 * \param paint Pointer (handle) to the paint object that wil be used for drawing

 * \return Error code. See NEMA_VG_ERR_* defines in "nema_vg_context.h" header file for the error codes.
 *
 */
uint32_t nema_vg_draw_path(NEMA_VG_PATH_HANDLE path, NEMA_VG_PAINT_HANDLE paint);

/** \brief Draw a line shape
 *
 *  \param x1 Upper left x coordinate
 *  \param y1 Upper left y coordinate
 *  \param x2 The width
 *  \param y2 The height
 *  \param m 3x3 affine transformation matrix
 *  \param paint The paint to draw

 * \return Error code. See NEMA_VG_ERR_* defines in "nema_vg_context.h" header file for the error codes.
 *
 */
uint32_t nema_vg_draw_line(float x1, float y1, float x2, float y2,
                            nema_matrix3x3_t m,
                            NEMA_VG_PAINT_HANDLE paint);

/** \brief Draw a rectangle shape
 *
 *  \param x Upper left x coordinate
 *  \param y Upper left y coordinate
 *  \param width The width
 *  \param height The height
 *  \param m 3x3 affine transformation matrix
 *  \param paint The paint to draw

 * \return Error code
 *
 */
uint32_t nema_vg_draw_rect(float x, float y, float width, float height,
                            nema_matrix3x3_t m,
                            NEMA_VG_PAINT_HANDLE paint);

/** \brief Draw a rounded rectangle shape
 *
 *  \param x Upper left x coordinate
 *  \param y Upper left y coordinate
 *  \param width The width
 *  \param height The height
 *  \param rx Horizontal cornel radius
 *  \param ry Vertical cornel radius
 *  \param m 3x3 affine transformation matrix
 *  \param paint The paint to draw

 * \return Error code. See NEMA_VG_ERR_* defines in "nema_vg_context.h" header file for the error codes.
 *
 */
uint32_t nema_vg_draw_rounded_rect(float x, float y, float width, float height,
                                float rx, float ry,
                                nema_matrix3x3_t m,
                                NEMA_VG_PAINT_HANDLE paint);

/** \brief Draw a ellipse shape
 *
 *  \param cx The x position of the ellipse
 *  \param cy The y position of the ellipse
 *  \param rx Radius on the x axis
 *  \param ry Radius on the y axis
 *  \param m 3x3 affine transformation matrix
 *  \param paint The paint to draw

 * \return Error code. See NEMA_VG_ERR_* defines in "nema_vg_context.h" header file for the error codes.
 *
 */
uint32_t nema_vg_draw_ellipse(float cx, float cy, float rx, float ry,
                            nema_matrix3x3_t m,
                            NEMA_VG_PAINT_HANDLE paint);

/** \brief Draw a circle shape
 *
 *  \param cx The x center of the circle
 *  \param cy The y center of the circle
 *  \param r Radius of the circle
 *  \param m 3x3 affine transformation matrix
 *  \param paint The paint to draw

 * \return Error code. See NEMA_VG_ERR_* defines in "nema_vg_context.h" header file for the error codes.
 *
 */
uint32_t nema_vg_draw_circle(float cx, float cy, float r,
                            nema_matrix3x3_t m,
                            NEMA_VG_PAINT_HANDLE paint);


/** \brief Draw a filled ring with rounded caps shape. In case of a conical gradient paint type,
 * the conical gradient center should be at the center of the ring(cx, cy). In other case, where the two centers do not match,
 * the ring should be drawn with NEMA_VG_QUALITY_MAXIMUM. The ring width can be set with the paint's stroke_width.
 *
 *  \param cx The center x coordinate of the ring
 *  \param cy The center y coordinate of the ring
 *  \param ring_radius The radius of the ring
 *  \param angle_start The angle in degrees of the ring
 *  \param angle_end The angle in degrees that ends this ring
 *  \param paint The paint to draw

 * \return Error code. See NEMA_VG_ERR_* defines in "nema_vg_context.h" header file for the error codes.
 *
 */
uint32_t nema_vg_draw_ring(float cx, float cy, float ring_radius, float angle_start, float angle_end,
                                            NEMA_VG_PAINT_HANDLE paint);

/** \private
 * \brief Draw a filled ring with flat ending or rounded ending caps. In case of a conical gradient paint type,
 * the conical gradient center should be at the center of the ring(cx, cy). In other case, where the two centers do not match,
 * the ring should be drawn with NEMA_VG_QUALITY_MAXIMUM. The ring width can be set with the paint's stroke_width.
 *
 *  \param cx The center x coordinate of the ring
 *  \param cy The center y coordinate of the ring
 *  \param ring_radius The radius of the ring
 *  \param angle_start The angle in degrees of the ring
 *  \param angle_end The angle in degrees that ends this ring
 *  \param paint The paint to draw
 *  \param has_caps 1 For caps 0 for flat ending

 * \return Error code. See NEMA_VG_ERR_* defines in "nema_vg_context.h" header file for the error codes.
 *
 */
uint32_t nema_vg_draw_ring_generic(float cx, float cy, float ring_radius, float angle_start, float angle_end,
                                            NEMA_VG_PAINT_HANDLE paint, uint8_t has_caps);


/** \brief Returns the minimum and maximum values for the coordinates that
 * can be handled by the underlying hardware
 *
 *  \param min_coord Minimum coordinate (x or y) value (pointer)
 *  \param max_coord Maximum coordinate (x or y) value (pointer)
 *
 */
void nema_vg_get_coord_limits(float *min_coord, float *max_coord);


/** \brief Disables tsvg features from rendering. Should be set before
 * nema_vg_draw_tsvg()
 *
 *  \param feature feature to be disabled
 *
 */
void nema_vg_tsvg_disable_feature(uint32_t feature);

#ifdef __cplusplus
}
#endif

#endif //__NEMA_VG_H__
