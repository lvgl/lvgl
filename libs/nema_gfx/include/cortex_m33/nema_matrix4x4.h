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


#ifndef NEMA_MATRIX4X4_H__
#define NEMA_MATRIX4X4_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef float nema_matrix4x4_t[4][4];


/** \brief Load a 4x4 Identity Matrix
 *
 * \param m Matrix to be loaded
 *
 */
void nema_mat4x4_load_identity(nema_matrix4x4_t m);

/** \brief Multiply two 4x4 matrices
 *
 * \param m Result Matrix
 * \param m_l Left operand
 * \param m_r Right operand
 *
 */
void nema_mat4x4_mul(nema_matrix4x4_t  m,
                     nema_matrix4x4_t  m_l,
                     nema_matrix4x4_t  m_r);


void nema_mat4x4_copy(nema_matrix4x4_t m_l,
                 nema_matrix4x4_t m_r);

/** \brief Multiply a 4x1 vector with a 4x4 matrix
 *
 * \param m Matrix to be multiplied
 * \param x Vector first element
 * \param y Vector second element
 * \param z Vector third element
 * \param w Vector forth element
 *
 */
void nema_mat4x4_mul_vec(nema_matrix4x4_t m, float *x, float *y, float *z, float *w);

// ------------------------------------------------------------------------------------
// Object Transformation - ModelView Matrix
// Object Coordinates to Eye Coordinates
// ------------------------------------------------------------------------------------

/** \brief Apply translate transformation
 *
 * \param m Matrix to apply transformation
 * \param tx X translation factor
 * \param ty Y translation factor
 * \param tz Z translation factor
 *
 */
void nema_mat4x4_translate(nema_matrix4x4_t m, float tx, float ty, float tz);

/** \brief Apply scale transformation
 *
 * \param m Matrix to apply transformation
 * \param sx X scaling factor
 * \param sy Y scaling factor
 * \param sz Z scaling factor
 *
 */
void nema_mat4x4_scale(nema_matrix4x4_t m, float sx, float sy, float sz);

/** \brief Apply rotate transformation around X axis
 *
 * \param m Matrix to apply transformation
 * \param angle_degrees Angle to rotate in degrees
 *
 */
void nema_mat4x4_rotate_X    (nema_matrix4x4_t m, float angle_degrees);

/** \brief Apply rotate transformation around Y axis
 *
 * \param m Matrix to apply transformation
 * \param angle_degrees Angle to rotate in degrees
 *
 */
void nema_mat4x4_rotate_Y    (nema_matrix4x4_t m, float angle_degrees);

/** \brief Apply rotate transformation around Z axis
 *
 * \param m Matrix to apply transformation
 * \param angle_degrees Angle to rotate in degrees
 *
 */
void nema_mat4x4_rotate_Z    (nema_matrix4x4_t m, float angle_degrees);

// ------------------------------------------------------------------------------------
// Scene Transformation/Frustum - Projection Matrix
// Eye Coordinates to Clip Coordinates
// ------------------------------------------------------------------------------------

/** \brief Set up a perspective projection matrix
 *
 * \param m A 4x4 Matrix
 * \param fovy_degrees Field of View in degrees
 * \param aspect Aspect ratio that determines the field of view in the x direction.
 * \param nearVal Distance from the viewer to the near clipping plane (always positive)
 * \param farVal Distance from the viewer to the far clipping plane (always positive)
 *
 */
void nema_mat4x4_load_perspective(nema_matrix4x4_t m, float fovy_degrees, float aspect,
                                  float nearVal, float farVal);


/** \brief Set up a Right Hand perspective projection matrix
 *
 * \param m A 4x4 Matrix
 * \param fovy_degrees Field of View in degrees
 * \param aspect Aspect ratio that determines the field of view in the x direction.
 * \param nearVal Distance from the viewer to the near clipping plane (always positive)
 * \param farVal Distance from the viewer to the far clipping plane (always positive)
 *
 */
void nema_mat4x4_load_perspective_rh(nema_matrix4x4_t m, float fovy_degrees, float aspect,
                                  float nearVal, float farVal);

/** \brief Set up an orthographic projection matrix
 *
 * \param m A 4x4 Matrix
 * \param left   Left vertical clipping plane
 * \param right  Right vertical clipping plane
 * \param bottom bottom horizontal clipping plane
 * \param top    Top horizontal clipping plane
 * \param nearVal Distance from the viewer to the near clipping plane (always positive)
 * \param farVal Distance from the viewer to the far clipping plane (always positive)
 *
 */
void nema_mat4x4_load_ortho(nema_matrix4x4_t m,
                            float left,    float right,
                            float bottom,  float top,
                            float nearVal, float farVal);

/** \brief Set up a 2D orthographic projection matrix
 *
 * \param m A 4x4 Matrix
 * \param left   Left vertical clipping plane
 * \param right  Right vertical clipping plane
 * \param bottom bottom horizontal clipping plane
 * \param top    Top horizontal clipping plane
 *
 */
void nema_mat4x4_load_ortho_2d(nema_matrix4x4_t m,
                               float left,   float right,
                               float bottom, float top);

/** \brief Set up a Right Hand view matrix.
 *
 * \param m A 4x4 Matrix
 * \param eye_x   Eye position x.
 * \param eye_y   Eye position y.
 * \param eye_z   Eye position z.
 * \param center_x   Center x to look at
 * \param center_y   Center y to look at
 * \param center_z   Center z to look at
 * \param up_x   Up vector x. (Usually 0)
 * \param up_y   Up vector y. (Usually 1)
 * \param up_z   Up vector z. (Usually 0)
 *
 */
void nema_mat4x4_look_at_rh(nema_matrix4x4_t m,
                        float eye_x, float eye_y, float eye_z,
                        float center_x, float center_y, float center_z,
                        float up_x, float up_y, float up_z);

// ------------------------------------------------------------------------------------
// Clip Coordinates to Window Coordinates
// ------------------------------------------------------------------------------------

/** \brief Convenience Function to calculate window coordinates from object coordinates
 *
 * \param mvp Model, View and Projection Matrix
 * \param x_orig Window top left X coordinate
 * \param y_orig Window top left Y coordinate
 * \param width Window width
 * \param height Window height
 * \param nearVal Distance from the viewer to the near clipping plane (always positive)
 * \param farVal Distance from the viewer to the far clipping plane (always positive)
 * \param x X object coordinate
 * \param y Y object coordinate
 * \param z Z object coordinate
 * \param w W object coordinate
 * \return 1 if vertex is outside frustum (should be clipped)
 *
 */
int nema_mat4x4_obj_to_win_coords(nema_matrix4x4_t mvp,
                                   float x_orig,  float y_orig,
                                   int width, int height,
                                   float nearVal, float farVal,
                                   float *x,
                                   float *y,
                                   float *z,
                                   float *w);

#ifdef __cplusplus
}
#endif

#endif
