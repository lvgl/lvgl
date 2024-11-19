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
 * @brief Paint operation related fuctions. Paint is an internal (opaque) struct of NemaVG.
 * The functions defined here can be used access its parameters.
 *
 */

#ifndef __NEMA_VG_PAINT_H__
#define __NEMA_VG_PAINT_H__

#include "nema_interpolators.h"
#include "nema_matrix3x3.h"
#include "nema_vg_context.h"
#include "nema_graphics.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  NEMA_VG_PAINT_COLOR        (0x00U) /**< Fill with color */
#define  NEMA_VG_PAINT_FILL         (0x00U) /**< Deprecated - Fill with color (same as NEMA_VG_PAINT_COLOR) */
#define  NEMA_VG_PAINT_GRAD_LINEAR  (0x01U) /**< Fill with linear gradient */
#define  NEMA_VG_PAINT_TEXTURE      (0x02U) /**< Fill with texture */
#define  NEMA_VG_PAINT_GRAD_RADIAL  (0x03U) /**< Fill with radial gradient */
#define  NEMA_VG_PAINT_GRAD_CONICAL (0x04U) /**< Fill with conical gradient */

#define  NEMA_VG_PAINT_MAX_GRAD_STOPS (32) /**< Maximum gradient stops*/

/** \brief Create a paint object.
 *
 * \return Handle to the created paint object
 *
 */
NEMA_VG_PAINT_HANDLE nema_vg_paint_create();

/** \brief Destroy a paint object.
 *
 * \param paint Handle to paint object that should be destroyed
 *
 */
void nema_vg_paint_destroy(NEMA_VG_PAINT_HANDLE paint);

/** \brief Clear the parameters of a paint object.
 *
 * \param paint Pointer (handle) to paint object
 *
 */
void nema_vg_paint_clear(NEMA_VG_PAINT_HANDLE paint);

/** \brief Set the paint type
 *
 * \param paint Pointer (handle) to paint
 * \param type Paint type (NEMA_VG_PAINT_COLOR, NEMA_VG_PAINT_GRAD_LINEAR, NEMA_VG_PAINT_TEXTURE, NEMA_VG_PAINT_GRAD_RADIAL, NEMA_VG_PAINT_GRAD_CONICAL)
 *
 */
void nema_vg_paint_set_type(NEMA_VG_PAINT_HANDLE paint, uint8_t type);


/** \brief Lock paint transformation to path. If locked, path and paint
 *  transformation will be in sync.
 *
 * \param paint Pointer to paint object
 * \param locked 1 if locked (default), 0 if not locked
 *
 */
void nema_vg_paint_lock_tran_to_path(NEMA_VG_PAINT_HANDLE paint, int locked);

/** \brief Set linear gradient to a paint object
 *
 * \param paint Pointer to paint object
 * \param grad Pointer to gradient object
 * \param x0  Linear gradient start point x coordinate
 * \param y0  Linear gradient start point y coordinate
 * \param x1  Linear gradient end point x coordinate
 * \param y1  Linear gradient end point y coordinate
 * \param sampling_mode Sampling mode. NEMA_TEX_BORDER defaults to NEMA_TEX_CLAMP
 *
 */
void nema_vg_paint_set_grad_linear(NEMA_VG_PAINT_HANDLE paint,
                                  NEMA_VG_GRAD_HANDLE grad,
                                  float x0, float y0,
                                  float x1, float y1,
                                  nema_tex_mode_t sampling_mode);

 /** \brief Set the paint color
 *
 * \param paint Pointer (handle) to paint object
 * \param rgba Color to be set, in rgba (hex 0xAABBGGRR) format
 *
 */
void nema_vg_paint_set_paint_color(NEMA_VG_PAINT_HANDLE paint, uint32_t rgba);

 /** \brief Set the paint opacity
 *
 * \param paint Pointer (pointer) to paint object
 * \param opacity Opacity to be set, 1 is fully opaque and 0 is fully transparent
 *
 */
void nema_vg_paint_set_opacity(NEMA_VG_PAINT_HANDLE paint, float opacity);

 /** \brief Set stroke width - DEPRECATED USE nema_vg_stroke_set_width
 *
 * \param paint Pointer (handle) to paint object
 * \param stroke_width Stroke width to be set
 *
 */
void nema_vg_paint_set_stroke_width(NEMA_VG_PAINT_HANDLE paint, float stroke_width);

 /** \brief Set transformation matrix for texture
 *
 * \param paint Pointer (handle) to paint object
 * \param m 3x3 transformation matrix
 *
 */
void nema_vg_paint_set_tex_matrix(NEMA_VG_PAINT_HANDLE paint, nema_matrix3x3_t m);

 /** \brief Set texture to paint object
 *
 * \param paint Pointer (handle) to paint
 * \param text Pointer to texture image object
 *
 */
void nema_vg_paint_set_tex(NEMA_VG_PAINT_HANDLE paint, nema_img_obj_t* tex);

 /** \brief Set Lut-based (look-up-table) texture to paint object. See Nema Pixpresso User Manual regarding Lut formats
 *
 * \param paint Pointer (handle) to paint object
 * \param lut_palette Pointer to the Palette of the Lut image object
 * \param lut_indices Pointer to the indices of the Lut image object
 *
 */
void nema_vg_paint_set_lut_tex(NEMA_VG_PAINT_HANDLE paint, nema_img_obj_t* lut_palette, nema_img_obj_t* lut_indices);

/** \brief Set Conical gradient to paint object
 *
 * \param paint Pointer (handle) to paint
 * \param grad Pointer (handle) to gradient
 * \param cx  Conical gradient center point x coordinate
 * \param cy  Conical gradient center point y coordinate
 * \param sampling_mode Sampling mode
 *
 */
void nema_vg_paint_set_grad_conical(NEMA_VG_PAINT_HANDLE paint,
                                    NEMA_VG_GRAD_HANDLE grad,
                                    float cx, float cy,
                                    nema_tex_mode_t sampling_mode);

/** \brief Set radial gradient to paint object
 *
 * \param paint Pointer (handle) to paint
 * \param grad Pointer (handle) to gradient
 * \param x0  Radial gradient center point x coordinate
 * \param y0  Radial gradient center point y coordinate
 * \param r  Radial gradient radius
 * \param sampling_mode Sampling mode
 *
 */
void
nema_vg_paint_set_grad_radial(NEMA_VG_PAINT_HANDLE paint,
                              NEMA_VG_GRAD_HANDLE grad,
                              float x0, float y0,
                              float r,
                              nema_tex_mode_t sampling_mode);


/** \brief Set radial gradient to paint object, with different horizontal and vertical radius
 *
 * \param paint Pointer (handle) to paint
 * \param grad Pointer (handle) to gradient
 * \param x0  Radial gradient center point x coordinate
 * \param y0  Radial gradient center point y coordinate
 * \param rx  Radial gradient radius on x axis
 * \param ry  Radial gradient radius on y axis
 * \param sampling_mode Sampling mode
 *
 */
void
nema_vg_paint_set_grad_radial2(NEMA_VG_PAINT_HANDLE paint,
                               NEMA_VG_GRAD_HANDLE grad,
                               float x0, float y0,
                               float rx, float ry,
                               nema_tex_mode_t sampling_mode);

/** \brief Create gradient object
 *
 * \return Handle (pointer) to the created gradient object
 */
NEMA_VG_GRAD_HANDLE
nema_vg_grad_create(void);

/** \brief Destroy gradient object
 *
 * \param grad Pointer to the gradient object
 *
 */
void
nema_vg_grad_destroy(NEMA_VG_GRAD_HANDLE grad);

/** \brief Set gradient parameters to a gradient object
 *
 * \param grad Pointer (handle) to gradient object
 * \param stops_count  Number of stop colors
 * \param stops Pointer to stop colors coordinates
 * \param colors Pointer to stop color values
 *
 */
void
nema_vg_grad_set(NEMA_VG_GRAD_HANDLE grad, int stops_count, float *stops, color_var_t* colors);

#ifdef __cplusplus
}
#endif

#endif //__NEMA_VG_PAINT_H__
