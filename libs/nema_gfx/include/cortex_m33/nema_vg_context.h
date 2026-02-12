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
 * @brief NemaVG Context interface.
 *
 * Contains NemaVG error codes, fill rules, rendering quality defines and functions for updating various rendering parameters.
 * The functions defined here can be used to access the context parameters. The Context is an internal (opaque) struct of NemaVG.
 */

#ifndef __NEMA_VG_CONTEXT_H__
#define __NEMA_VG_CONTEXT_H__

#include "nema_graphics.h"
#include "nema_matrix3x3.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifndef NEMA_VG_HANDLE
#define NEMA_VG_HANDLE void* /**< NemaVG handle object (void pointer)*/
#endif

#define NEMA_VG_PATH_HANDLE NEMA_VG_HANDLE  /**< NemaVG path handle (pointer to path object)*/
#define NEMA_VG_PAINT_HANDLE NEMA_VG_HANDLE /**< NemaVG paint handle (pointer to paint object)*/
#define NEMA_VG_GRAD_HANDLE NEMA_VG_HANDLE  /**< NemaVG gradient handle (pointer to gradient object)*/

typedef float nema_vg_float_t; /**< Floating point data type (default is 'float') */

#define NEMA_VG_ERR_NO_ERROR                (0x00000000U)  /**< No Error */
#define NEMA_VG_ERR_BAD_HANDLE              (0x00000001U)  /**< Bad handle */
#define NEMA_VG_ERR_BAD_BUFFER              (0x00000002U)  /**< Bad buffer */
#define NEMA_VG_ERR_INVALID_FILL_RULE       (0x00000004U)  /**< Invalid fill rule*/
#define NEMA_VG_ERR_INVALID_PAINT_TYPE      (0x00000008U)  /**< Invalid paint type*/
#define NEMA_VG_ERR_INVALID_VERTEX_DATA     (0x00000010U)  /**< Invalid vertex data*/
#define NEMA_VG_ERR_NO_RADIAL_ENABLED       (0x00000020U)  /**< Radial not present in HW*/
#define NEMA_VG_ERR_NO_BOUND_CL             (0x00000040U)  /**< No bound CL*/
#define NEMA_VG_ERR_INVALID_ARGUMENTS       (0x00000080U)  /**< Invalid arguments*/
#define NEMA_VG_ERR_INVALID_ARC_DATA        (0x00000100U)  /**< reserved */
#define NEMA_VG_ERR_CL_FULL                 (0x00000200U)  /**< reserved */
#define NEMA_VG_ERR_DRAW_OUT_OF_BOUNDS      (0x00000400U)  /**< Path is out of the drawing area */
#define NEMA_VG_ERR_INVALID_MASKING_OBJ     (0x00000800U)  /**< Masking object was not set */
#define NEMA_VG_ERR_INVALID_MASKING_FORMAT  (0x00001000U)  /**< Invalid Masking object Format */
#define NEMA_VG_ERR_INVALID_LUT_IDX_FORMAT  (0x00002000U)  /**< Invalid LUT indices object Format */
#define NEMA_VG_ERR_COORDS_OUT_OF_RANGE     (0x00004000U)  /**< Path coordinates out of supported range */
#define NEMA_VG_ERR_EMPTY_TSVG              (0x00008000U)  /**< Tsvg has no geometries */
#define NEMA_VG_ERR_NO_BOUND_FONT           (0x00010000U)  /**< There is no bound font */
#define NEMA_VG_ERR_UNSUPPORTED_FONT        (0x00020000U)  /**< The font is not supported (eg. older version) by NemaVG API */
#define NEMA_VG_ERR_NON_INVERTIBLE_MATRIX   (0x00040000U)  /**< A matrix that needs to be inverted, is not invertible */
#define NEMA_VG_ERR_INVALID_GRAD_STOPS      (0x00080000U)  /**< Gradient stops exceed maximum available stops */
#define NEMA_VG_ERR_NO_INIT                 (0x00100000U)  /**< VG uninitialized */
#define NEMA_VG_ERR_INVALID_STROKE_WIDTH    (0x00200000U)  /**< Invalid stroke width */
#define NEMA_VG_ERR_INVALID_OPACITY         (0x00400000U)  /**< Invalid opacity */
#define NEMA_VG_ERR_INVALID_CAP_STYLE       (0x00800000U)  /**< Invalid cap style */
#define NEMA_VG_ERR_INVALID_JOIN_STYLE      (0x01000000U)  /**< Invalid join style */
#define NEMA_VG_ERR_INVALID_STENCIL_SIZE    (0x02000000U)  /**< Invalid stencil buffer size */

#define NEMA_VG_FILL_DRAW                   (0x00U) /**< DEPRECATED Stroke fill rule */
#define NEMA_VG_STROKE                      (0x00U) /**< Stroke fill rule */
#define NEMA_VG_FILL_EVEN_ODD               (0x01U) /**< Evenodd fill rule */
#define NEMA_VG_FILL_NON_ZERO               (0x02U) /**< Non zero fill rule */

#define NEMA_VG_QUALITY_BETTER              (0x00U) /**< Better rendering quality (default option, balances rendering quality and performance)*/
#define NEMA_VG_QUALITY_FASTER              (0x01U) /**< Faster rendering quality (favors performance over rendering quality)*/
#define NEMA_VG_QUALITY_MAXIMUM             (0x02U) /**< Maximum rendering quality (favors rendering quality over performance)*/
#define NEMA_VG_QUALITY_NON_AA              (0x10U) /**< Rendering quality without AA*/

#define NEMA_VG_CAP_BUTT                    (0x00U) /**< Butt cap*/
#define NEMA_VG_CAP_ROUND                   (0x01U) /**< Round cap*/
#define NEMA_VG_CAP_SQUARE                  (0x02U) /**< Square cap*/
#define NEMA_VG_CAP_MAX                     (0x03U) /**< Max value for cap*/

#define NEMA_VG_JOIN_BEVEL                  (0x00U) /**< Bevel join*/
#define NEMA_VG_JOIN_MITER                  (0x01U) /**< Mitter join*/
#define NEMA_VG_JOIN_ROUND                  (0x02U) /**< Round join*/
#define NEMA_VG_JOIN_MAX                    (0x03U) /**< Max for join*/

#define NEMA_VG_TSVG_DISABLE_NONE          (0x00000000U) /**< Disable none*/
#define NEMA_VG_TSVG_DISABLE_CAPS          (0x00000001U) /**< Disable caps*/
#define NEMA_VG_TSVG_DISABLE_JOINS         (0x00000002U) /**< Disable joins*/

/** \brief Set the global transformation matrix. Global matrix will be applied in all NemaVG rendering operations that will follow.
 *
 * \param m transformation matrix
 *
 * \return Error code
 */
uint32_t nema_vg_set_global_matrix(nema_matrix3x3_t m);


/** \brief Disable the global transformation matrix.
 *
 */
void nema_vg_reset_global_matrix(void);

/** \brief Set the fill rule that will be applied when rendering a path.
 *
 * \param fill_rule fill rule (NEMA_VG_STROKE, NEMA_VG_FILL_EVEN_ODD, NEMA_VG_FILL_NON_ZERO)
 *
 */
void nema_vg_set_fill_rule(uint8_t fill_rule);

/** \brief Set the stroke width that will be applied when stroking a path.
 *
 * \param width Stroke width to be set
 *
 */
void nema_vg_stroke_set_width(float width);

 /** \brief Set stroke cap style
 *
 * \param cap_style Cap style (NEMA_VG_CAP_BUTT | NEMA_VG_CAP_SQUARE | NEMA_VG_CAP_ROUND)
 *
 */
void nema_vg_stroke_set_cap_style(uint8_t start_cap_style, uint8_t end_cap_style);

 /** \brief Set stroke join style
 *
 * \param join_style Join style (NEMA_VG_JOIN_BEVEL | NEMA_VG_JOIN_MITER | NEMA_VG_JOIN_ROUND)
 *
 */
void nema_vg_stroke_set_join_style(uint8_t join_style);

 /** \brief Set stroke miter limit
 * If miter join is chosen and miter length is bigger than the product
 * of miter limit and stroke width a bevel join will be added instead
 *
 * \param miter_limit miter join limit to be set
 *
 */
void nema_vg_stroke_set_miter_limit(float miter_limit);

/** \brief Enable/Disable Masking.
 *
 * \param masking 1 to enable, 0 to disable
 *
 */
void nema_vg_masking(uint8_t masking);

/** \brief Set the mask object (texture)
 *
 * \param mask_obj Texture to be used as mask. Its format must be NEMA_A1, NEMA_A2, NEMA_A4 or Nema_A8, otherwise it will return an error.
 * \return Error code. If no error occurs, NEMA_VG_ERR_NO_ERROR otherwise NEMA_VG_ERR_INVALID_MASKING_FORMAT.
 *
 */
uint32_t nema_vg_set_mask(nema_img_obj_t *mask_obj);

/** \brief Translate the mask object (texture) with respect to origin point (0, 0). Sets the position of the mask object.
 *
 * \param x Horizontal position to place the mask object
 * \param y Horizontal position to place the mask object
 *
 */
void nema_vg_set_mask_translation(float x, float y);

/** \brief Set the rendering quality
 *
 * \param quality level (NEMA_VG_QUALITY_BETTER, NEMA_VG_QUALITY_FASTER, NEMA_VG_QUALITY_MAXIMUM, NEMA_VG_QUALITY_NON_AA)
 *
 */
void nema_vg_set_quality(uint8_t quality);

/** \brief Set the blending mode for VG operations (see nema_blender.h documentation in NemaGFX API Manual)
 *  Additional Blending Operations: only NEMA_BLOP_SRC_PREMULT is supported
 *
 * \param blend Blending mode
 * \see nema_blending_mode()
 *
 */
void nema_vg_set_blend(uint32_t blend);

/** \brief Get the current error code. Clears the error afterwards.
 *
 * \return Error code. See NEMA_VG_ERR_* defines for all the possible error codes.
 */
uint32_t nema_vg_get_error(void);

/** \brief Enable/disable large coordinates handling when rendering a TSVG, a path or a predefined shape
 *
 * \param enable 0 to disable, 1 to enable
 * \param allow_internal_alloc 0 to not allow internal allocation, 1 to allow
 *
 */
void nema_vg_handle_large_coords(uint8_t enable, uint8_t allow_internal_alloc);

/** \brief Bind segment and data buffers to be used for handling large coordinates
 *
 * \param segs Pointer to segment buffer for large coordinates
 * \param segs_size_bytes Segment buffer size in bytes
 * \param data Pointer to data buffer for large coordinates
 * \param data_size_bytes Data buffer size in bytes
 *
 */
uint32_t nema_vg_bind_clip_coords_buf(void *segs, uint32_t segs_size_bytes, void *data, uint32_t data_size_bytes);

/** \brief Unbind segment and data buffers to be used for handling large coordinates
 *
 *
 */
void nema_vg_unbind_clip_coords_buf(void);

#ifdef __cplusplus
}
#endif

#endif //__NEMA_VG_CONTEXT_H__
