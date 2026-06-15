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


#ifndef _NEMA_INTERPOLATORS_H_
#define _NEMA_INTERPOLATORS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "nema_sys_defs.h"

typedef struct _color_var_t {
    float r; /**< Red */
    float g; /**< Green */
    float b; /**< Blue */
    float a; /**< Alpha */
} color_var_t;

/** \brief Interpolate color gradient for rectangle
 *
 * \param x0 x coordinate of the upper left vertex of the rectangle
 * \param y0 y coordinate at the upper left vertex of the rectangle
 * \param w width of the rectangle
 * \param h height of the rectangle
 * \param col0 color for the first vertex
 * \param col1 color for the second vertex
 * \param col1 color for the third vertex
 *
 */
void nema_interpolate_rect_colors(int x0, int y0, int w, int h, color_var_t* col0, color_var_t* col1, color_var_t* col2);


/** \brief Interpolate color gradient for triangle
 *
 * \details The upper left vertex of the triangle to be drawn
 *  must be in the vertex arguments as well. In addition, if
 * clipping is applied for rendering a triangle with gradient,
 * the upper left vertex must be within the clipping area.
 * \param x0 x coordinate at the first vertex of the triangle
 * \param y0 y coordinate at the first vertex of the triangle
 * \param x1 x coordinate at the second vertex of the triangle
 * \param y1 y coordinate at the second vertex of the triangle
 * \param x2 x coordinate at the third vertex of the triangle
 * \param y2 y coordinate at the third vertex of the triangle
 * \param col0 color for the first vertex
 * \param col1 color for the second vertex
 * \param col1 color for the third vertex
 *
 */
void nema_interpolate_tri_colors(float x0, float y0, float x1, float y1, float x2, float y2, color_var_t* col0, color_var_t* col1, color_var_t* col2);

/** \brief Interpolate depth buffer values for triangle
 *
 * \param x0 x coordinate at the first vertex of the triangle
 * \param y0 y coordinate at the first vertex of the triangle
 * \param z0 z coordinate at the first vertex of the triangle
 * \param x1 x coordinate at the second vertex of the triangle
 * \param y1 y coordinate at the second vertex of the triangle
 * \param z1 z coordinate at the second vertex of the triangle
 * \param x2 x coordinate at the third vertex of the triangle
 * \param y2 y coordinate at the third vertex of the triangle
 * \param z2 z coordinate at the third vertex of the triangle
 *
 */
void nema_interpolate_tri_depth(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2);

/** \brief Interpolate texture values for triangle
 *
 * \param x0 x coordinate at the first vertex of the triangle
 * \param y0 y coordinate at the first vertex of the triangle
 * \param w0 w coordinate at the first vertex of the triangle
 * \param tx0 x texture coordinate at the first vertex of the triangle
 * \param ty0 y texture coordinate at the first vertex of the triangle
 * \param x1 x coordinate at the second vertex of the triangle
 * \param y1 y coordinate at the second vertex of the triangle
 * \param w1 w coordinate at the second vertex of the triangle
 * \param tx1 x texture coordinate at the second vertex of the triangle
 * \param ty1 y texture coordinate at the second vertex of the triangle
 * \param x2 x coordinate at the third vertex of the triangle
 * \param y2 y coordinate at the third vertex of the triangle
 * \param w2 w coordinate at the third vertex of the triangle
 * \param tx2 x texture coordinate at the third vertex of the triangle
 * \param ty2 x texture coordinate at the third vertex of the triangle
 * \param tex_width texture width
 * \param tex_height texture height
 *
 */
void nema_interpolate_tx_ty(float x0, float y0, float w0, float tx0, float ty0,
                            float x1, float y1, float w1, float tx1, float ty1,
                            float x2, float y2, float w2, float tx2, float ty2,
                            int tex_width, int tex_height );
#ifdef __cplusplus
}
#endif

#endif // _NEMA_INTERPOLATORS_H_
