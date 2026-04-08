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


#ifndef NEMA_GRAPHICS_H__
#define NEMA_GRAPHICS_H__

#include "nema_sys_defs.h"
#include "nema_hal.h"
#include "nema_matrix3x3.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _img_obj_ {
    nema_buffer_t bo;
    uint16_t w;
    uint16_t h;
    int      stride;
    uint32_t color;
    uint8_t  format;
    uint8_t  sampling_mode; // NEMA_FILTER*
} img_obj_t;

typedef img_obj_t nema_img_obj_t;

typedef enum {
    NEMA_NOTEX  = -1,    /**< No Texture */
    NEMA_TEX0   =  0,    /**< Texture 0 */
    NEMA_TEX1   =  1,    /**< Texture 1 */
    NEMA_TEX2   =  2,    /**< Texture 2 */
    NEMA_TEX3   =  3,    /**< Texture 3 */
    NEMA_TEXMAX =  4     /**< Enum max value */
} nema_tex_t;

#define NEMA_RGBX8888   0x00U  /**< RGBX8888 */
#define NEMA_RGBA8888   0x01U  /**< RGBA8888 */
#define NEMA_XRGB8888   0x02U  /**< XRGB8888 */
#define NEMA_ARGB8888   0x03U  /**< ARGB8888 */
#define NEMA_RGB565     0x04U  /**< RGBA5650 */
#define NEMA_RGBA5650   0x04U  /**< RGBA5650 */
#define NEMA_RGBA5551   0x05U  /**< RGBA5551 */
#define NEMA_RGBA4444   0x06U  /**< RGBA4444 */
#define NEMA_RGBA0800   0x07U  /**< RGBA0800 */
#define NEMA_A8         0x08U  /**< RGBA0008 */
#define NEMA_RGBA0008   0x08U  /**< RGBA0008 */
#define NEMA_L8         0x09U  /**< L8       */
#define NEMA_RGBA3320   0x38U  /**< RGBA3320 (source only) */
#define NEMA_RGB332     0x38U  /**< RGBA3320 (source only) */
#define NEMA_BW1        0x0CU  /**< A1       (source only) */
#define NEMA_A1         0x0CU  /**< A1       (source only) */
#define NEMA_L1         0x0BU  /**< L1       (source only) */
#define NEMA_UYVY       0x0DU  /**< UYVY     */
#define NEMA_ABGR8888   0x0EU  /**< ABGR8888 */
#define NEMA_XBGR8888   0x0FU  /**< XBGR8888 */
#define NEMA_BGRA8888   0x10U  /**< BGRA     */
#define NEMA_BGRX8888   0x11U  /**< BGRX     */
#define NEMA_TSC4       0x12U  /**< TSC4     */
#define NEMA_TSC6       0x16U  /**< TSC6     */
#define NEMA_TSC6A      0x17U  /**< TSC6A    */
#define NEMA_RV         0x18U  /**< RV       */
#define NEMA_GU         0x19U  /**< GU       */
#define NEMA_BY         0x1AU  /**< BY       */
#define NEMA_YUV        0x1BU  /**< YUV      */
#define NEMA_Z24_8      0x1cU  /**< Z24_8    */
#define NEMA_Z16        0x1dU  /**< Z16      */
#define NEMA_UV         0x1eU  /**< UV       */
#define NEMA_A1LE       0x27U  /**< A1LE     (source only) */
#define NEMA_A2LE       0x28U  /**< A2LE     (source only) */
#define NEMA_A4LE       0x29U  /**< A4LE     (source only) */
#define NEMA_L1LE       0x2AU  /**< L1LE     (source only) */
#define NEMA_L2LE       0x2BU  /**< L2LE     (source only) */
#define NEMA_L4LE       0x2CU  /**< L4LE     (source only) */
#define NEMA_A2         0x30U  /**< A2       (source only) */
#define NEMA_A4         0x34U  /**< A4       (source only) */
#define NEMA_L2         0x31U  /**< L2       (source only) */
#define NEMA_L4         0x35U  /**< L4       (source only) */
#define NEMA_BGR24      0x39U  /**< BGR24    */
#define NEMA_RGB24      0x3CU  /**< RGB24    */
#define NEMA_RV10       0x3DU  /**< RV-10bit */
#define NEMA_GU10       0x3EU  /**< GU-10bit */
#define NEMA_BY10       0x3FU  /**< BY-10bit */
#define NEMA_RGBA2222   0x40U  /**< RGBA2222 (Available if HW enabled - check HW manual) */
#define NEMA_ABGR2222   0x41U  /**< ABGR2222 (Available if HW enabled - check HW manual) */
#define NEMA_BGRA2222   0x42U  /**< BGRA2222 (Available if HW enabled - check HW manual) */
#define NEMA_ARGB2222   0x43U  /**< ARGB2222 (Available if HW enabled - check HW manual) */
#define NEMA_AL88       0x44U  /**< AL88     (Available if HW enabled - check HW manual) */
#define NEMA_AL44       0x45U  /**< AL44     (Available if HW enabled - check HW manual) */
#define NEMA_ARGB1555   0x46U  /**< ARGB1555 (Available if HW enabled - check HW manual) */
#define NEMA_ARGB4444   0x47U  /**< ARGB4444 (Available if HW enabled - check HW manual) */
#define NEMA_BGRA5650   0x13U  /**< BGRA5650 (Available if HW enabled - check HW manual) */
#define NEMA_BGR565     0x13U  /**< BGRA5650 (Available if HW enabled - check HW manual) */
#define NEMA_BGRA5551   0x48U  /**< BGRA5551 (Available if HW enabled - check HW manual) */
#define NEMA_ABGR1555   0x49U  /**< ABGR1555 (Available if HW enabled - check HW manual) */
#define NEMA_BGRA4444   0x4aU  /**< BGRA4444 (Available if HW enabled - check HW manual) */
#define NEMA_ABGR4444   0x4bU  /**< ABGR4444 (Available if HW enabled - check HW manual) */
#define NEMA_TSC12      0x4cU  /**< TSC12    (Available if HW enabled - check HW manual) */
#define NEMA_TSC12A     0x4dU  /**< TSC12A   (Available if HW enabled - check HW manual) */
#define NEMA_TSC6AP     0x4eU  /**< TSC6AP   (Available if HW enabled - check HW manual) */

#define NEMA_DITHER     0x80U  /**< Nema Dithering */
#define NEMA_FORMAT_MASK 0x7FU /**< Format Mask */

typedef uint32_t nema_tex_format_t;

//-----------------------------------------------------------------------------------------------------------------------

// Texture Unit Parameters
//-----------------------------------------------------------------------------------------
    // Filtering - 0:0
    //----------------------
#define NEMA_FILTER_PS  0x00U /**< Point Sampling. */
#define NEMA_FILTER_BL  0x01U /**< Bilinear filtering. */

    // Wrapping Mode 3:2
    //----------------------
#define NEMA_TEX_CLAMP  (0x00U)    /**< Clamp */
#define NEMA_TEX_REPEAT (0x01U<<2) /**< Repeat */
#define NEMA_TEX_BORDER (0x02U<<2) /**< Border */
#define NEMA_TEX_MIRROR (0x03U<<2) /**< Mirror */

    // Texture Coordinates Ordering 4:4
    //----------------------
#define NEMA_TEX_MORTON_ORDER (0x10U)

    // Texture Coordinates Format 6:5
    //----------------------
#define NEMA_TEX_RANGE_0_1   (0x1U<<5)  /**< Interpolated Coordinates range: 0-1 */
#define NEMA_TEX_LEFT_HANDED (0x1U<<6)  /**< (0,0) is bottom left corner */

typedef uint8_t nema_tex_mode_t;

// Triangle Culling
//-----------------------------------------------------------------------------------------
typedef enum {
    NEMA_CULL_NONE = 0      , /**< Disable Triangle/Quadrilateral Culling */
    NEMA_CULL_CW   = (1U<<28),   /**< Cull clockwise Triangles/Quadrilaterals */
    NEMA_CULL_CCW  = (1U<<29),  /**< Cull anti-clockwise Triangles/Quadrilaterals */
    NEMA_CULL_ALL  = NEMA_CULL_CW | NEMA_CULL_CCW   /**< Cull all */
} nema_tri_cull_t;

// Rotation Modes
//-----------------------------------------------------------------------------------------------------------------------

#define NEMA_ROT_000_CCW  (0x0U) /**< No rotation */
#define NEMA_ROT_090_CCW  (0x1U) /**< Rotate  90 degrees counter-clockwise */
#define NEMA_ROT_180_CCW  (0x2U) /**< Rotate 180 degrees counter-clockwise */
#define NEMA_ROT_270_CCW  (0x3U) /**< Rotate 270 degrees counter-clockwise */
#define NEMA_ROT_000_CW   (0x0U) /**< No rotation */
#define NEMA_ROT_270_CW   (0x1U) /**< Rotate 270 degrees clockwise */
#define NEMA_ROT_180_CW   (0x2U) /**< Rotate 180 degrees clockwise */
#define NEMA_ROT_090_CW   (0x3U) /**< Rotate  90 degrees clockwise */
#define NEMA_MIR_VERT     (0x4U) /**< Mirror Vertically */
#define NEMA_MIR_HOR      (0x8U)  /**< Mirror Horizontally */


/** \brief Check if a known GPU is present
 *
 * \return -1 if no known GPU is present
 *
 */
int nema_checkGPUPresence(void);

// ------------------------------ TEXTURES -------------------------------------

/** \brief Program a Texture Unit
 *
 * \param texid    Texture unit to be programmed
 * \param addr_gpu Texture's address as seen by the GPU
 * \param width    Texture's width
 * \param height   Texture's height
 * \param format   Texture's format
 * \param stride   Texture's stride. If stride < 0, it's left to be calculated
 * \param wrap_mode  Wrap/Repeat mode to be used. When using 'repeat' or 'mirror', texture dimensions must be a power of two. Otherwise the behavior is undefined.
 *
 */
void nema_bind_tex(nema_tex_t texid, uintptr_t addr_gpu,
                   uint32_t width, uint32_t height,
                   nema_tex_format_t format, int32_t stride, nema_tex_mode_t wrap_mode);

/** \brief Set Texture Mapping default color
 *
 * \param color default color in 32-bit RGBA format
 * \see nema_rgba()
 *
 */
void nema_set_tex_color(uint32_t color);

// ------------------------------ CONSTREGS ------------------------------------

/** \brief Write a value to a Constant Register of the GPU
 *
 * \param reg Constant Register to be written
 * \param value Value to be written
 *
 */
void nema_set_const_reg(int reg, uint32_t value);

// --------------------------------- CLIP --------------------------------------

/** \brief Sets the drawing area's Clipping Rectangle
 *
 * \param x Clip Window top-left x coordinate
 * \param y Clip Window minimum y
 * \param w Clip Window width
 * \param h Clip Window height
 *
 */
void nema_set_clip(int32_t x, int32_t y, uint32_t w, uint32_t h);

void nema_enable_gradient(int enable);

void nema_enable_depth(int enable);

/** \brief Enables MSAA per edge
 *
 * \param e0 Enable MSAA for edge 0 (vertices 0-1)
 * \param e1 Enable MSAA for edge 1 (vertices 1-2)
 * \param e2 Enable MSAA for edge 2 (vertices 2-3)
 * \param e3 Enable MSAA for edge 3 (vertices 3-0)
 * \return previous AA flags (may be ignored)
 *
 */
uint32_t nema_enable_aa(uint8_t e0, uint8_t e1, uint8_t e2, uint8_t e3);

// ------------------------------- DIRTY REGIONS ------------------------------------

/** \brief  Returns the bounding rectangle of all the pixels that have been modified since its previous call.
 * Available only on Nema|P and Nema|PVG GPUs.
 *
 * \param minx x coordinate of the upper left corner of the dirty region
 * \param miny y coordinate of the upper left corner of the dirty region
 * \param maxx x coordinate of the lower right corner of the dirty region
 * \param maxy y coordinate of the lower right corner of the dirty region
 *
 */
void nema_get_dirty_region(int *minx, int *miny, int *maxx, int *maxy);

/** \brief Clear dirty region information - runs via the bound command-list
 * Available only on Nema|P and Nema|PVG GPUs.
 *
 * \see nema_get_dirty_region()
 * \see nema_clear_dirty_region_imm()
 *
 */
void nema_clear_dirty_region(void);

/** \brief Clear dirty region information immediately, no command-list involved
 * Available only on Nema|P and Nema|PVG GPUs.
 *
 * \see nema_get_dirty_region()
 * \see nema_clear_dirty_region()
 *
 */
void nema_clear_dirty_region_imm(void);

// -------------------------------- UTILS --------------------------------------

/** \brief Set triangle/quadrilateral culling mode
 *
 * \param cull Culling mode
 *
 */
void nema_tri_cull(nema_tri_cull_t cull);

/** \brief Return pixel size in bytes
 *
 * \param format Color format
 * \return Pixel size in bytes
 *
 */
int nema_format_size (nema_tex_format_t format);

/** \brief Return stride in bytes
 *
 * \param format Color format
 * \param wrap_mode  Wrap/Repeat mode to be used. When using 'repeat' or 'mirror', texture dimensions must be a power of two. Otherwise the behavior is undefined.
 * \param width Texture color format
 * \return Stride in bytes
 *
 */
int nema_stride_size(nema_tex_format_t format, nema_tex_mode_t wrap_mode, int width);


/** \brief Return texture size in bytes
 *
 * \param format Texture color format
 * \param wrap_mode  Wrap/Repeat mode to be used. When using 'repeat' or 'mirror', texture dimensions must be a power of two. Otherwise the behavior is undefined.
 * \param width Texture width
 * \param height Texture height
 * \return Texture size in bytes
 *
 */
int nema_texture_size(nema_tex_format_t format, nema_tex_mode_t wrap_mode, int width, int height);

/** \brief Return Nema internal RGBA color
 *
 * \param R Red component
 * \param G Green component
 * \param B Blue component
 * \param A Alpha component
 * \return RGBA value
 *
 */
uint32_t nema_rgba(unsigned char R,
                   unsigned char G,
                   unsigned char B,
                   unsigned char A);

/** \brief Premultiply RGB channels with Alpha channel
 *
 * \param rgba RGBA color
 * \return Premultiplied RGBA color
 *
 */
uint32_t nema_premultiply_rgba(uint32_t rgba);

/** \brief Initialize NemaGFX library
 *
 * \return negative value on error
 *
 */
int nema_init(void);

/** \brief Reinitialize NemaGFX library
 *
 * \details This function reinitializes the NemaGFX library after a GPU poweroff
 * No memory allocation for ringbuffer etc is performed.
 *
 * \return negative value on error
 *
 */
int nema_reinit(void);

// ------------------------------- CONTEXT -------------------------------------

/** \brief Program Texture Unit with a foreground (source) texture (NEMA_TEX1)
 *
 * \param baseaddr_phys Address of the source texture, as seen by the GPU
 * \param width Texture width
 * \param height Texture hight
 * \param format Texture format
 * \param stride Texture stride. If negative, it's calculated internally.
 * \param wrap_mode  Wrap/Repeat mode to be used. When using 'repeat' or 'mirror', texture dimensions must be a power of two. Otherwise the behavior is undefined.
 *
 */
void nema_bind_src_tex(uintptr_t baseaddr_phys,
                       uint32_t width, uint32_t height,
                       nema_tex_format_t format, int32_t stride, nema_tex_mode_t mode);

/** \brief Program Texture Unit with a background texture ((NEMA_TEX2)
 *
 * \param baseaddr_phys Address of the source2 texture, as seen by the GPU
 * \param width Texture width
 * \param height Texture hight
 * \param format Texture format
 * \param stride Texture stride. If negative, it's calculated internally.
 * \param wrap_mode  Wrap/Repeat mode to be used. When using 'repeat' or 'mirror', texture dimensions must be a power of two. Otherwise the behavior is undefined.
 *
 */
void nema_bind_src2_tex(uintptr_t baseaddr_phys,
                       uint32_t width, uint32_t height,
                       nema_tex_format_t format, int32_t stride, nema_tex_mode_t mode);

/** \brief Program Texture Unit with a destination texture (NEMA_TEX0)
 *
 * \param baseaddr_phys Address of the destination texture, as seen by the GPU
 * \param width Texture width
 * \param height Texture hight
 * \param format Texture format
 * \param stride Texture stride. If negative, it's calculated internally.
 *
 */
void nema_bind_dst_tex(uintptr_t baseaddr_phys,
                        uint32_t width, uint32_t height,
                        nema_tex_format_t format, int32_t stride);

/** \brief Program Texture Unit with a lut/palette texture (NEMA_TEX2) and index texture (NEMA_TEX1_)
 *
 * \param baseaddr_phys Address of the index texture
 * \param width Index texture width
 * \param height Index texture hight
 * \param format Index texture format
 * \param stride Index texture stride. If negative, it's calculated internally.
 * \param mode Index texture sampling mode.  When using 'NEMA_TEX_REPEAT' or 'NEMA_TEX_MIRROR' wrapping mode,
 *             texture dimensions must be a power of two, otherwise the behavior is undefined. NEMA_FILTER_BL is not supported.
 * \param palette_baseaddr_phys Address of the lut/palette texture
 * \param palette_format lut/palette texture format
 *
 */
void nema_bind_lut_tex( uintptr_t baseaddr_phys,
                        uint32_t width, uint32_t height,
                        nema_tex_format_t format, int32_t stride, nema_tex_mode_t mode,
                        uintptr_t palette_baseaddr_phys,
                        nema_tex_format_t palette_format);

/** \brief Bind Depth Buffer
 *
 * \param baseaddr_phys Address of the depth buffer, as seen by the GPU
 * \param width Buffer width
 * \param height Buffer hight
 *
 */
void nema_bind_depth_buffer(uintptr_t baseaddr_phys,
                             uint32_t width, uint32_t height);

/** \private */
// -------------------------------- DEPTH --------------------------------------
void nema_set_depth(float start, float dx, float dy);

// ------------------------------ GRADIENT -------------------------------------
/** \private */
void nema_set_gradient(float r_init, float g_init, float b_init, float a_init,
                       float r_dx, float r_dy,
                       float g_dx, float g_dy,
                       float b_dx, float b_dy,
                       float a_dx, float a_dy);

// ------------------------------- DRAWING -------------------------------------
/** \brief Clear destination texture with color
 *
 * \param rgba8888 32-bit RGBA color
 * \see nema_rgba()
 *
 */
void nema_clear(uint32_t rgba8888);

/** \brief Clear depth buffer with specified value
 *
 * \param val Clear value
 *
 */
void nema_clear_depth(uint32_t val);

/** \brief Draw a colored line
 *
 * \param x0 x coordinate at the beginning of the line
 * \param y0 y coordinate at the beginning of the line
 * \param x1 x coordinate at the end of the line
 * \param y1 y coordinate at the end of the line
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_draw_line(int x0, int y0, int x1, int y1, uint32_t rgba8888);


/** \brief Draw a line with width. Apply AA if available
 *
 * \param x0 x coordinate at the beginning of the line
 * \param y0 y coordinate at the beginning of the line
 * \param x1 x coordinate at the end of the line
 * \param y1 y coordinate at the end of the line
 * \param w  line width
 * \param rgba8888 Color to be used
 * \see nema_draw_line()
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void
nema_draw_line_aa(float x0, float y0, float x1, float y1, float w,
                  uint32_t rgba8888);

/** \brief Draw a colored circle with 1 pixel width
 *
 * \param x x coordinate of the circle's center
 * \param y y coordinate of the circle's center
 * \param r circle's radius
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_draw_circle(int x, int y, int r, uint32_t rgba8888);

/** \brief Draw a colored circle with Anti-Aliasing (if available) and specified width
 *
 * \param x x coordinate of the circle's center
 * \param y y coordinate of the circle's center
 * \param r circle's radius
 * \param w pencil width
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_draw_circle_aa(float x, float y, float r, float w, uint32_t rgba8888);

/** \brief Draw a colored rectangle with rounded edges
 *
 * \param x0 x coordinate of the upper left vertex of the rectangle
 * \param y0 y coordinate at the upper left vertex of the rectangle
 * \param w width of the rectangle
 * \param h height of the rectangle
 * \param r corner radius
 * \param rgba8888
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_draw_rounded_rect(int x0, int y0, int w, int h, int r, uint32_t rgba8888);

/** \brief Draw a colored rectangle
 *
 * \param x x coordinate of the upper left vertex of the rectangle
 * \param y y coordinate at the upper left vertex of the rectangle
 * \param w width of the rectangle
 * \param h height of the rectangle
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_draw_rect(int x, int y, int w, int h, uint32_t rgba8888);

/** \brief Fill a circle with color
 *
 * \param x x coordinate of the circle's center
 * \param y y coordinate of the circle's center
 * \param r circle's radius
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_fill_circle(int x, int y, int r, uint32_t rgba8888);

/** \brief Fill a circle with color, use Anti-Aliasing if available
 *
 * \param x x coordinate of the circle's center
 * \param y y coordinate of the circle's center
 * \param r circle's radius
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_fill_circle_aa(float x, float y, float r, uint32_t rgba8888);

/** \brief Fill a triangle with color
 *
 * \param x0 x coordinate at the first vertex of the triangle
 * \param y0 y coordinate at the first vertex of the triangle
 * \param x1 x coordinate at the second vertex of the triangle
 * \param y1 y coordinate at the second vertex of the triangle
 * \param x2 x coordinate at the third vertex of the triangle
 * \param y2 y coordinate at the third vertex of the triangle
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 *
 */
void nema_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t rgba8888);

/** \brief Fill a rectangle with rounded edges with color
 *
 * \param x0 x coordinate of the upper left vertex of the rectangle
 * \param y0 y coordinate at the upper left vertex of the rectangle
 * \param w width of the rectangle
 * \param h height of the rectangle
 * \param r corner radius
 * \param rgba8888
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_fill_rounded_rect(int x0, int y0, int w, int h, int r, uint32_t rgba8888);

/** \brief Fill a rectangle with color
 *
 * \param x x coordinate of the upper left vertex of the rectangle
 * \param y y coordinate at the upper left vertex of the rectangle
 * \param w width of the rectangle
 * \param h height of the rectangle
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_fill_rect(int x, int y, int w, int h, uint32_t rgba8888);

/** \brief Fill a quadrilateral with color
 *
 * \param x0 x coordinate at the first vertex of the quadrilateral
 * \param y0 y coordinate at the first vertex of the quadrilateral
 * \param x1 x coordinate at the second vertex of the quadrilateral
 * \param y1 y coordinate at the second vertex of the quadrilateral
 * \param x2 x coordinate at the third vertex of the quadrilateral
 * \param y2 y coordinate at the third vertex of the quadrilateral
 * \param x3 x coordinate at the fourth vertex of the quadrilateral
 * \param y3 y coordinate at the fourth vertex of the quadrilateral
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_fill_quad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t rgba8888);


/** \brief Fill a rectangle with color (float coordinates)
 *
 * \param x x coordinate of the upper left vertex of the rectangle
 * \param y y coordinate at the upper left vertex of the rectangle
 * \param w width of the rectangle
 * \param h height of the rectangle
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_fill_rect_f(float x, float y, float w, float h, uint32_t rgba8888);


/** \brief Fill a quadrilateral with color (float coordinates)
 *
 * \param x0 x coordinate at the first vertex of the quadrilateral
 * \param y0 y coordinate at the first vertex of the quadrilateral
 * \param x1 x coordinate at the second vertex of the quadrilateral
 * \param y1 y coordinate at the second vertex of the quadrilateral
 * \param x2 x coordinate at the third vertex of the quadrilateral
 * \param y2 y coordinate at the third vertex of the quadrilateral
 * \param x3 x coordinate at the fourth vertex of the quadrilateral
 * \param y3 y coordinate at the fourth vertex of the quadrilateral
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 * \see nema_rgba()
 *
 */
void nema_fill_quad_f(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, uint32_t rgba8888);


/** \brief Fill a triangle with color (float coordinates)
 *
 * \param x0 x coordinate at the first vertex of the triangle
 * \param y0 y coordinate at the first vertex of the triangle
 * \param x1 x coordinate at the second vertex of the triangle
 * \param y1 y coordinate at the second vertex of the triangle
 * \param x2 x coordinate at the third vertex of the triangle
 * \param y2 y coordinate at the third vertex of the triangle
 * \param rgba8888 Color to be used
 * \see nema_set_blend_fill()
 *
 */
void nema_fill_triangle_f(float x0, float y0, float x1, float y1, float x2, float y2, uint32_t rgba8888);


// ------------------------------- BLITTING ------------------------------------

/** \brief Blit source texture to destination texture
 *
 * \param x destination x coordinate
 * \param y destination y coordinate
 * \see nema_set_blend_fill()
 *
 */
void nema_blit (int x, int y);


/** \brief Blit source texture to destination texture with rounded corners
 *
 * \param x destination x coordinate
 * \param y destination y coordinate
 * \param r destination corner radius
 * \see nema_set_blend_fill()
 *
 */
void nema_blit_rounded (int x, int y, int r);

/** \brief Blit source texture to destination's specified rectangle (crop or wrap when needed)
 *
 * \param x destination x coordinate
 * \param y destination y coordinate
 * \param w destination width
 * \param h destination height
 * \see nema_set_blend_blit()
 *
 */
void nema_blit_rect (int x, int y, int w, int h);

/** \brief Blit part of a source texture to destination's specified rectangle (crop or wrap when needed)
 *
 * \param x destination x coordinate
 * \param y destination y coordinate
 * \param w destination width
 * \param h destination height
 * \param x source x coordinate
 * \param y source y coordinate
 * \see nema_blit_subrect()
 * \see nema_set_blend_blit()
 *
 */
void nema_blit_subrect(int dst_x, int dst_y, int w, int h, int src_x, int src_y);

/** \brief Blit source texture to destination. Fit (scale) texture to specified rectangle.
 *
 * \param x destination x coordinate
 * \param y destination y coordinate
 * \param w destination width
 * \param h destination height
 * \see nema_set_blend_blit()
 *
 */
void nema_blit_rect_fit(int x, int y, int w, int h);


/** \brief Blit part of source texture to destination. Fit (scale) texture to specified rectangle.
 *
 * \param x destination x coordinate
 * \param y destination y coordinate
 * \param w destination width
 * \param h destination height
 * \param x source x coordinate
 * \param y source y coordinate
 * \param w source width
 * \param h source height
 * \see nema_blit_rect_fit()
 * \see nema_set_blend_blit()
 *
 */
void nema_blit_subrect_fit( int dst_x, int dst_y, int dst_w, int dst_h,
                            int src_x, int src_y, int src_w, int src_h);

/** \brief Rotate around pivot point and Blit source texture.
 *
 * \param cx destination rotation center x coordinate
 * \param cy destination rotation center y coordinate
 * \param px source pivot point x coordinate
 * \param py source pivot point y coordinate
 * \param degrees_cw degrees of clockwise rotation in range [0, 360]
 * \see nema_set_blend_blit()
 *
 */
void nema_blit_rotate_pivot( float cx, float cy,
                             float px, float py, float degrees_cw );

/** \brief Rotate and Blit source texture to destination.
 *
 * \param x destination x coordinate
 * \param y destination y coordinate
 * \param rotation Rotation to be done
 * \see nema_set_blend_blit()
 *
 */
void nema_blit_rotate(int x, int y, uint32_t rotation);

/** \brief Rotate and Blit partial source texture to destination.
 *
 * \param sx source upper left x coordinate
 * \param sy source upper left y coordinate
 * \param sw source width of partial region
 * \param sh source height of partial region
 * \param x destination x coordinate
 * \param y destination y coordinate
 * \param rotation Rotation to be done
 * \see nema_set_blend_blit()
 *
 */
void nema_blit_rotate_partial(int sx, int sy,
                              int sw, int sh,
                              int x,  int y,
                              uint32_t rotation);


/** \brief Blit source texture to destination. Fit texture to specified triangle.
 *
 * \param dx0 x coordinate at the first vertex of the triangle
 * \param dy0 y coordinate at the first vertex of the triangle
 * \param v0  in [0, 3] indicates the corner of the texture that fits to the first vertex of the triangle
 *                0 _ _ 1
 *                 |_ _|
 *                3     2
 * \param dx1 x coordinate at the second vertex of the triangle
 * \param dy1 y coordinate at the second vertex of the triangle
 * \param v1  in [0, 3] indicates the corner of the texture that fits to the second vertex of the triangle
 * \param dx2 x coordinate at the third vertex of the triangle
 * \param dy2 y coordinate at the third vertex of the triangle
 * \param v2  in [0, 3] indicates the corner of the texture that fits to the third vertex of the triangle
 * \see nema_set_blend_blit()
 *
 */
void nema_blit_tri_fit (float dx0, float dy0, int v0,
                        float dx1, float dy1, int v1,
                        float dx2, float dy2, int v2);

/** \brief Blit a triangular part of the source tecture to a triangular destination area
 *
 * \param dx0 x coordinate at the first vertex of the destination triangle
 * \param dy0 y coordinate at the first vertex of the destination triangle
 * \param dw0 w coordinate at the first vertex of the destination triangle
 * \param dx1 x coordinate at the second vertex of the destination triangle
 * \param dy1 y coordinate at the second vertex of the destination triangle
 * \param dw1 w coordinate at the second vertex of the destination triangle
 * \param dx2 x coordinate at the third vertex of the destination triangle
 * \param dy2 y coordinate at the third vertex of the destination triangle
 * \param dw2 w coordinate at the third vertex of the destination triangle
 * \param sx0 x coordinate at the first vertex of the source triangle
 * \param sy0 y coordinate at the first vertex of the source triangle
 * \param sx1 x coordinate at the second vertex of the source triangle
 * \param sy1 y coordinate at the second vertex of the source triangle
 * \param sx2 x coordinate at the third vertex of the source triangle
 * \param sy2 y coordinate at the third vertex of the source triangle
 * \see nema_set_blend_blit()
 *
 */
void nema_blit_tri_uv  (float dx0, float dy0, float dw0,
                        float dx1, float dy1, float dw1,
                        float dx2, float dy2, float dw2,
                        float sx0, float sy0,
                        float sx1, float sy1,
                        float sx2, float sy2
                        );

/** \brief Blit source texture to destination. Fit texture to specified quadrilateral.
 *
 * \param dx0 x coordinate at the first vertex of the quadrilateral
 * \param dy0 y coordinate at the first vertex of the quadrilateral
 * \param dx1 x coordinate at the second vertex of the quadrilateral
 * \param dy1 y coordinate at the second vertex of the quadrilateral
 * \param dx2 x coordinate at the third vertex of the quadrilateral
 * \param dy2 y coordinate at the third vertex of the quadrilateral
 * \param dx3 x coordinate at the fourth vertex of the quadrilateral
 * \param dy3 y coordinate at the fourth vertex of the quadrilateral
 * \see nema_set_blend_blit()
 * \see nema_blit_subrect_quad_fit()
 *
 */
void nema_blit_quad_fit (float dx0, float dy0,
                         float dx1, float dy1,
                         float dx2, float dy2,
                         float dx3, float dy3);

/** \brief Blit source texture to destination. Fit rectangulare area of texture to specified quadrilateral.
 *
 * \param dx0 x coordinate at the first vertex of the quadrilateral
 * \param dy0 y coordinate at the first vertex of the quadrilateral
 * \param dx1 x coordinate at the second vertex of the quadrilateral
 * \param dy1 y coordinate at the second vertex of the quadrilateral
 * \param dx2 x coordinate at the third vertex of the quadrilateral
 * \param dy2 y coordinate at the third vertex of the quadrilateral
 * \param dx3 x coordinate at the fourth vertex of the quadrilateral
 * \param dy3 y coordinate at the fourth vertex of the quadrilateral
 * \param sx x coordinate of the top left corner of the texture's rectangular area to be blitted
 * \param sy y coordinate of the top left corner of the texture's rectangular area to be blitted
 * \param sw width of the texture's rectangular area to be blitted
 * \param sh height of the texture's rectangular area to be blitted
 * \see nema_set_blend_blit()
 * \see nema_blit_quad_fit()
 *
 */
void nema_blit_subrect_quad_fit(float dx0, float dy0,
                                float dx1, float dy1,
                                float dx2, float dy2,
                                float dx3, float dy3,
                                int sx, int sy,
                                int sw, int sh);

/** \brief Blit source texture to destination. Use the matrix provided by the user.
 *
 * \param dx0 x coordinate at the first vertex of the quadrilateral
 * \param dy0 y coordinate at the first vertex of the quadrilateral
 * \param dx1 x coordinate at the second vertex of the quadrilateral
 * \param dy1 y coordinate at the second vertex of the quadrilateral
 * \param dx2 x coordinate at the third vertex of the quadrilateral
 * \param dy2 y coordinate at the third vertex of the quadrilateral
 * \param dx3 x coordinate at the fourth vertex of the quadrilateral
 * \param dy3 y coordinate at the fourth vertex of the quadrilateral
 * \param m 3x3 matrix (screen coordinates to texture coordinates)
 * \see nema_set_blend_blit()
 *
 */
void nema_blit_quad_m(float dx0, float dy0,
                      float dx1, float dy1,
                      float dx2, float dy2,
                      float dx3, float dy3, nema_matrix3x3_t m);




/** \brief Enable breakpoints
 *
 * \see nema_brk_disable()
 *
 */
void nema_brk_enable(void);

/** \brief Disable breakpoints
 *
 * \see nema_brk_enable()
 *
 */
void nema_brk_disable(void);


/** \brief Add a breakpoint to the current Command List
 *
 * \return Breakpoint ID
 *
 */
int  nema_brk_add(void);

/** \brief Add a breakpoint to the current Command List
 *
 * \param brk_id Breakpoint ID to wait for. If zero (0), wait until next Breakpoint
 * \return ID of reached Breakpoint
 *
 */
int  nema_brk_wait(int brk_id);

/** \brief Instruct the GPU to resume execution
 *
 *
 */
void nema_brk_continue(void);

/** \brief Enable external hold signals
 *
 * \param hold_id Hold signals to be enabled [0-3]
 * \see nema_ext_hold_disable()
 *
 */
void nema_ext_hold_enable(uint32_t hold_id);

/** \brief Disable external hold signals
 *
 * \param hold_id Hold signals to be disabled [0-3]
 * \see nema_ext_hold_enable()
 *
 */
void nema_ext_hold_disable(uint32_t hold_id);

/** \brief Enable Interrupt Request when GPU reaches hold point
 *
 * \param hold_id Hold signals' IRQ to be enabled [0-3]
 * \see nema_ext_hold_disable()
 *
 */
void nema_ext_hold_irq_enable(uint32_t hold_id);

/** \brief Disable external hold signals
 *
 * \param hold_id Hold signals' IRQ to be disabled [0-3]
 * \see nema_ext_hold_enable()
 *
 */
void nema_ext_hold_irq_disable(uint32_t hold_id);


/** \brief Assert hold signals internally via a Command List
 *
 * \param hold_id Hold signal to be asserted
 * \param stop If not zero, force Command List Processor to wait for FLAG to be deasserted
 * \see nema_ext_hold_deassert()
 *
 */
void nema_ext_hold_assert(uint32_t hold_id, int stop);

/** \brief Dessert hold signals internally via a Command List
 *
 * \param hold_id Hold signal to be deasserted
 * \see nema_ext_hold_assert()
 *
 */
void nema_ext_hold_deassert(uint32_t hold_id);

/** \brief Assert hold signals from the CPU (no Command List)
 *
 * \param hold_id Hold signal to be asserted
 * \see nema_ext_hold_deassert()
 *
 */
void nema_ext_hold_assert_imm(uint32_t hold_id);

/** \brief Dessert hold signals from the CPU (no Command List)
 *
 * \param hold_id Hold signal to be deasserted
 * \see nema_ext_hold_assert()
 *
 */
void nema_ext_hold_deassert_imm(uint32_t hold_id);

/** \brief Check for which architeture is the library compiled
 *
 * \return Returns string with the architecture name
 *
 */
const char* nema_get_sw_device_name(void);


#ifdef __cplusplus
}
#endif

#endif
