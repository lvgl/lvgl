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


#ifndef NEMA_PROVISIONAL_H__
#define NEMA_PROVISIONAL_H__

#include "nema_sys_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Fill a triangle strip with color (float coordinates)
 *
 * \param vertices pointer to vertices coordinated (first x coordinate of vertex,
 *                 then y coordinate of vertex)
 * \param num_vertices number of vertices
 * \param stride Distance between two vertices
 * \param rgba8888 Color to be used
 *
 */
void nema_fill_triangle_strip_f(float* vertices, int num_vertices, int stride, uint32_t rgba8888);

/** \brief Fill a traingle fan with color (float coordinates)
 *
 * \param vertices pointer to vertices coordinated (first x coordinate of vertex,
 *                 then y coordinate of vertex)
 * \param num_vertices number of vertices
 * \param stride Distance between two vertices
 * \param rgba8888 Color to be used
 *
 */
void nema_fill_triangle_fan_f(float* vertices, int num_vertices, int stride, uint32_t rgba8888);

/** \brief Draws a triangle with specific border width. Apply AA if available.
 * Degenerated triangles have undefined behavior.
 *
 * \param x0 x coordinate at the first vertex of the triangle
 * \param y0 y coordinate at the first vertex of the triangle
 * \param x1 x coordinate at the second vertex of the triangle
 * \param y1 y coordinate at the second vertex of the triangle
 * \param x2 x coordinate at the third vertex of the triangle
 * \param y2 y coordinate at the third vertex of the triangle
 * \param border_width triangle's border width
 * \param color color of the triangle
 *
 */
void nema_draw_triangle_aa(float x0, float y0, float x1, float y1, float x2, float y2,
                           float border_width, uint32_t color);


/** \brief Draw a colored rectangle with rounded edges and specific border width. Apply AA if available.
 *
 * \param x x coordinate of the upper left vertex of the rectangle
 * \param y y coordinate at the upper left vertex of the rectangle
 * \param w width of the rectangle
 * \param h height of the rectangle
 * \param r corner radius
 * \param border_width border width
 * \param rgba8888 rgba color of the rounded rectangle
 *
 */
void nema_draw_rounded_rect_aa(float x, float y, float w, float h, float r, float border_width, uint32_t rgba8888);


/** \brief Draw a filled colored rectangle with rounded edges and specific border width. Apply AA if available.
 *
 * \param x x coordinate of the upper left vertex of the rectangle
 * \param y y coordinate at the upper left vertex of the rectangle
 * \param w width of the rectangle
 * \param h height of the rectangle
 * \param r corner radius
 * \param rgba8888 rgba color of the rounded rectangle
 *
 */
void nema_fill_rounded_rect_aa(float x, float y, float w, float h, float r, uint32_t rgba8888);

/** \brief Draws a quadrilateral with specific border width. Apply AA if available.
 * Only Convex quadrilaterals are supported.
 *
 * \param x0 x coordinate at the first vertex of the quadrilateral
 * \param y0 y coordinate at the first vertex of the quadrilateral
 * \param x1 x coordinate at the second vertex of the quadrilateral
 * \param y1 y coordinate at the second vertex of the quadrilateral
 * \param x2 x coordinate at the third vertex of the quadrilateral
 * \param y2 y coordinate at the third vertex of the quadrilateral
 * \param x3 x coordinate at the fourth vertex of the quadrilateral
 * \param y3 y coordinate at the fourth vertex of the quadrilateral
 * \param border_width trianquadrilateralgle's border width
 * \param color color of the quadrilateral
 *
 */
void nema_draw_quad_aa(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3,
                       float border_width, uint32_t color);
#ifdef __cplusplus
}
#endif

#endif // NEMA_PROVISIONAL_H__
