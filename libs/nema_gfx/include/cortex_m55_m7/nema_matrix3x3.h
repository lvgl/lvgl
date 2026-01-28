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


#ifndef NEMA_MATRIX3X3_H__
#define NEMA_MATRIX3X3_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef float nema_matrix3x3_t[3][3];


/** \brief Load Identity Matrix
 *
 * \param m Matrix to be loaded
 *
 */
void nema_mat3x3_load_identity(nema_matrix3x3_t m);


/** \brief Copy matrix _m to matrix m
 *
 * \param m Destination matrix
 * \param m Source matrix
 *
 */
void
nema_mat3x3_copy(nema_matrix3x3_t m, nema_matrix3x3_t _m);

/** \brief Apply translate transformation
 *
 * \param m Matrix to apply transformation
 * \param tx X translation factor
 * \param ty Y translation factor
 *
 */
void nema_mat3x3_translate   (nema_matrix3x3_t m, float tx, float ty);

/** \brief Apply scale transformation
 *
 * \param m Matrix to apply transformation
 * \param sx X scaling factor
 * \param sy Y scaling factor
 *
 */
void nema_mat3x3_scale       (nema_matrix3x3_t m, float sx, float sy);

/** \brief Apply shear transformation
 *
 * \param m Matrix to apply transformation
 * \param shx X shearing factor
 * \param shy Y shearing factor
 *
 */
void nema_mat3x3_shear       (nema_matrix3x3_t m, float shx, float shy);

/** \brief Apply mirror transformation
 *
 * \param m Matrix to apply transformation
 * \param mx if non-zero, mirror horizontally
 * \param my if non-zero, mirror vertically
 *
 */
void nema_mat3x3_mirror      (nema_matrix3x3_t m, int mx, int my);

/** \brief Apply rotation transformation
 *
 * \param m Matrix to apply transformation
 * \param angle_degrees Angle to rotate in degrees
 *
 */
void nema_mat3x3_rotate      (nema_matrix3x3_t m, float angle_degrees);

/** \brief Apply rotation transformation
 *
 * \param m Matrix to apply transformation
 * \param cosa Cos of angle to rotate
 * \param sina Sin of angle to rotate
 *
 */
void
nema_mat3x3_rotate2(nema_matrix3x3_t m, float cosa, float sina);


/** \brief Multiply two 3x3 matrices ( m = m*_m)
 *
 * \param m left matrix, will be overwritten by the result
 * \param _m right matrix
 *
 */
void nema_mat3x3_mul(nema_matrix3x3_t m, nema_matrix3x3_t _m);

/** \brief Multiply vector with matrix
 *
 * \param m Matrix to multiply with
 * \param x Vector x coefficient
 * \param y Vector y coefficient
 *
 */
void nema_mat3x3_mul_vec(nema_matrix3x3_t m, float *x, float *y);

/** \brief Multiply vector with affine matrix
 *
 * \param m Matrix to multiply with
 * \param x Vector x coefficient
 * \param y Vector y coefficient
 *
 */
void nema_mat3x3_mul_vec_affine(nema_matrix3x3_t m, float *x, float *y);

/** \brief Calculate adjoint
 *
 * \param m Matrix
 *
 */
void nema_mat3x3_adj(nema_matrix3x3_t m);


/** \brief Divide matrix with scalar value
 *
 * \param m Matrix to divide
 * \param s scalar value
 *
 */
void nema_mat3x3_div_scalar(nema_matrix3x3_t m, float s);

/** \brief Invert matrix
 *
 * \param m Matrix to invert
 *
 */
int nema_mat3x3_invert(nema_matrix3x3_t m);

/** \private */
int nema_mat3x3_square_to_quad(float dx0, float dy0,
                             float dx1, float dy1,
                             float dx2, float dy2,
                             float dx3, float dy3,
                             nema_matrix3x3_t m);

/** \brief Map rectangle to quadrilateral
 *
 * \param width Rectangle width
 * \param height Rectangle height
 * \param sx0 x coordinate at the first vertex of the quadrilateral
 * \param sy0 y coordinate at the first vertex of the quadrilateral
 * \param sx1 x coordinate at the second vertex of the quadrilateral
 * \param sy1 y coordinate at the second vertex of the quadrilateral
 * \param sx2 x coordinate at the third vertex of the quadrilateral
 * \param sy2 y coordinate at the third vertex of the quadrilateral
 * \param sx3 x coordinate at the fourth vertex of the quadrilateral
 * \param sy3 y coordinate at the fourth vertex of the quadrilateral
 * \param m Mapping matrix
 *
 */
int nema_mat3x3_quad_to_rect(int width, int height,
                           float sx0, float sy0,
                           float sx1, float sy1,
                           float sx2, float sy2,
                           float sx3, float sy3,
                           nema_matrix3x3_t m);

/** \brief Apply rotation around a pivot point
 *
 * \param m Matrix to apply transformation
 * \param angle_degrees Angle to rotate in degrees
 * \param x X coordinate of the pivot point
 * \param y Y coordinate of the pivot point
 *
 */
void nema_mat3x3_rotate_pivot(nema_matrix3x3_t m, float angle_degrees,
                              float x, float y);

/** \brief Apply scale and then rotation around a pivot point
 *
 * \param m Matrix to apply transformation
* \param sx X scaling factor
 * \param sy Y scaling factor
 * \param angle_degrees Angle to rotate in degrees
 * \param x X coordinate of the pivot point
 * \param y Y coordinate of the pivot point
 *
 */
void nema_mat3x3_scale_rotate_pivot(nema_matrix3x3_t m,
                                    float sx, float sy,
                                    float angle_degrees, float x, float y);


/** \brief Copy matrix _m to matrix m
 *
 * \param m Destination matrix
 * \param m Source matrix
 *
 */

void
nema_mat3x3_copy(nema_matrix3x3_t m, nema_matrix3x3_t _m);


#ifdef __cplusplus
}
#endif

#endif
