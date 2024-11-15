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


#ifndef NEMA_TRANSITIONS_H__
#define NEMA_TRANSITIONS_H__

#include "nema_blender.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NEMA_TRANS_LINEAR_H,
    NEMA_TRANS_CUBE_H,
    NEMA_TRANS_INNERCUBE_H,
    NEMA_TRANS_STACK_H,
    NEMA_TRANS_LINEAR_V,
    NEMA_TRANS_CUBE_V,
    NEMA_TRANS_INNERCUBE_V,
    NEMA_TRANS_STACK_V,
    NEMA_TRANS_FADE,
    NEMA_TRANS_FADE_ZOOM,
    NEMA_TRANS_MAX,
    NEMA_TRANS_NONE,
} nema_transition_t;

/** \brief Transition from 'initial' texture to 'final' texture. The transition is complete when 'step' is 0 or 1
 *
 * \param effect Transition effect
 * \param initial Initial texture
 * \param final Final texture
 * \param blending_mode Blending mode
 * \param step Transition step within [0.f , 1.f] range
 * \param width Texture width
 * \param height Texture height
 *
 */
void nema_transition(nema_transition_t effect, nema_tex_t initial, nema_tex_t final,
                    uint32_t blending_mode, float step, int width, int height);


/** \brief Linear transition horizontally. When 'step' changes from zero to one, textures move from right to left,
otherwise textures move from left to right. The transition is complete when 'step' is 0 or 1.
 *
 * \param left Texture on the left side
 * \param right Texture on the right side
 * \param blending_mode Blending mode
 * \param step Current step within [0.f , 1.f] range
 * \param width Texture width
 *
 */
void nema_transition_linear_hor(nema_tex_t left, nema_tex_t right,
                                uint32_t blending_mode, float step, int width);

/** \brief Linear transition vertically. When 'step' changes from zero to one, textures move from top to bottom,
otherwise textures move from bottom to top. The transition is complete when 'step' is 0 or 1.
 *
 * \param up Texture on the top side
 * \param down Texture on the bottom side
 * \param blending_mode Blending mode
 * \param step Current step within [0.f , 1.f] range
 * \param height Texture height
 *
 */
void nema_transition_linear_ver(nema_tex_t up, nema_tex_t down,
                                uint32_t blending_mode, float step, int height);

/** \brief Cubic (textures are mapped on the external faces of a cube) transition horizontally. When 'step' changes from zero to one, textures move from left to right,
otherwise textures move from right to left. The transition is complete when 'step' is 0 or 1.
 *
 * \param left Texture on the left side
 * \param right Texture on the right side
 * \param blending_mode Blending mode
 * \param step Current step within [0.f , 1.f] range
 * \param width Texture width
 * \param height Texture height
 *
 */
void nema_transition_cube_hor(nema_tex_t left, nema_tex_t right,
                              uint32_t blending_mode, float step, int width, int height);

/** \brief Cube (textures are mapped on the external faces of a cube) transition vertically. When 'step' changes from zero to one, textures move from top to bottom,
otherwise textures move from bottom to top. The transition is complete when 'step' is 0 or 1.
 *
 * \param up Texture on the top side
 * \param down Texture on the bottom side
 * \param blending_mode Blending mode
 * \param step Current step within [0.f , 1.f] range
 * \param width Texture width
 * \param height Texture height
 *
 */
void nema_transition_cube_ver(nema_tex_t up, nema_tex_t down,
                              uint32_t blending_mode, float step, int width, int height);

/** \brief Inner Cube (textures are mapped on the internal faces of a cube) transition horizontally. When 'step' changes from zero to one, textures move from left to right,
otherwise textures move from right to left. The transition is complete when 'step' is 0 or 1.
 *
 * \param left Texture on the left side
 * \param right Texture on the right side
 * \param blending_mode Blending mode
 * \param step Current step within [0.f , 1.f] range
 * \param width Texture width
 * \param height Texture height
 *
 */
void nema_transition_innercube_hor(nema_tex_t left, nema_tex_t right,
                                   uint32_t blending_mode, float step, int width, int height);

/** \brief Inner Cube (textures are mapped on the internal faces of a cube) transition vertically. When 'step' changes from zero to one, textures move from top to bottom,
otherwise textures move from bottom to top. The transition The transition is complete when 'step' is 0 or 1.
 *
 * \param up Texture on the top side
 * \param down Texture on the bottom side
 * \param blending_mode Blending mode
 * \param step Current step within [0.f , 1.f] range
 * \param width Texture width
 * \param height Texture height
 *
 */
void nema_transition_innercube_ver(nema_tex_t up, nema_tex_t down,
                                   uint32_t blending_mode, float step, int width, int height);

/** \brief Stack transition horizontally. When 'step' changes from zero to one, textures move from left to right,
otherwise textures move from right to left. The transition is complete when 'step' is 0 or 1.
 *
 * \param up Texture on the top side
 * \param down Texture on the bottom side
 * \param blending_mode Blending mode
 * \param step Current step within [0.f , 1.f] range
 * \param width Texture width
 * \param height Texture height
 *
 */
void nema_transition_stack_hor(nema_tex_t left, nema_tex_t right, float step,
                                int width, int height);

/** \brief Stack transition vertically. When 'step' moves from zero to one, textures move from top to bottom,
otherwise textures move from bottom to top. The transition is complete when 'step' is 0 or 1.
 *
 * \param up Texture on the top side
 * \param down Texture on the bottom side
 * \param blending_mode Blending mode
 * \param step Current step within [0.f , 1.f] range
 * \param width Texture width
 * \param height Texture height
 *
 */
void nema_transition_stack_ver(nema_tex_t up, nema_tex_t down, float step,
                                int width, int height);

/** \brief Fade transition. Initial texture is being faded out, while final texture is being faded in.
The transition is complete when 'step' is 0 or 1.
 *
 * \param left Texture on the left side
 * \param right Texture on the right side
 * \param blending_mode Blending mode
 * \param step Current step within [0.f , 1.f] range
 * \param width Texture width
 * \param height Texture height
 *
 */
void nema_transition_fade(nema_tex_t initial, nema_tex_t final,
						  uint32_t blending_mode, float step, int width, int height);

/** \brief Fade-zoom transition. Initial texture is being zoomed and faded out, while final texture is being zoomed and faded in.
The transition is complete when 'step' is 0 or 1.
 *
 * \param initial Initial texture
 * \param final Final texture
 * \param blending_mode Blending mode
 * \param step Current step within [0.f , 1.f] range
 * \param width Texture width
 * \param height Texture height
 *
 */
void nema_transition_fade_zoom(nema_tex_t initial, nema_tex_t final,
                                uint32_t blending_mode, float step, int width, int height);

#ifdef __cplusplus
}
#endif

#endif
