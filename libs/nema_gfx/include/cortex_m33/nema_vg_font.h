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
 * @brief Vector font rendering
 *
 * This file includes the necessary structs and functions that are used for rendering text (strings and single characters),
 * using vector fonts. The accompanying vector font converter utility, converts truetype fonts (ttf files) to instances
 * of the structs defined here. A use case of this module is included in the respective examples (examples/NemaVG/render_vg_font).
 */

#ifndef NEMA_VG_FONT_H_
#define NEMA_VG_FONT_H_

#include "nema_matrix3x3.h"
#include "nema_vg.h"
#include "nema_vg_context.h"
#include "nema_font.h" //vector to raster conversion

#ifdef __cplusplus
extern "C" {
#endif

#define NEMA_VG_ALIGNX_LEFT    (0x00U) /**< Align horizontally to the left */
#define NEMA_VG_ALIGNX_RIGHT   (0x01U) /**< Align horizontally to the right */
#define NEMA_VG_ALIGNX_CENTER  (0x02U) /**< Align horizontally centered */
#define NEMA_VG_ALIGNX_JUSTIFY (0x03U) /**< Justify horizontally */
#define NEMA_VG_ALIGNX_MASK    (0x03U) /**< Horizontal alignment mask */
#define NEMA_VG_ALIGNY_TOP     (0x00U) /**< Align vertically to the top */
#define NEMA_VG_ALIGNY_BOTTOM  (0x04U) /**< Align vertically to the bottom */
#define NEMA_VG_ALIGNY_CENTER  (0x08U) /**< Align vertically centered */
#define NEMA_VG_ALIGNY_JUSTIFY (0x0cU) /**< Justify vertically */
#define NEMA_VG_ALIGNY_MASK    (0x0cU) /**< Vertical alignment mask */
#define NEMA_VG_TEXT_WRAP      (0x10U) /**< Use text wrapping */

#define NEMA_VG_CHAR_LTR       (0x00U) /**< Character follows left to right orientation */
#define NEMA_VG_CHAR_RTL       (0x01U) /**< Character follows right to left orientation */
#define NEMA_VG_CHAR_TTB       (0x00U) /**< Character follows top to bottom orientation */
#define NEMA_VG_CHAR_BTT       (0x02U) /**< Character follows bottom to top orientation */


/** NemaVG Kerning pair information data struct */
typedef struct {
    const uint32_t   left;     /**< Neighbor character to the left of the current one (Unicode value) */
    const float      x_offset; /**< Kerning offset value (horizontally) */
} nema_vg_kern_pair_t;

/** NemaVG data struct of a glyph in vector format*/
typedef struct {
    const uint32_t  data_offset;    /**< Offset value for the data of the glyph in the respective data array */
    const size_t    data_length;    /**< Length of the data in the respective data array */
    const uint32_t  segment_offset; /**< Offset value for the segments of the glyph in the respective segment array */
    const size_t    segment_length; /**< Length of the segments in the respective segment array */
    const float     xAdvance;       /**< Advance width*/
    const uint32_t  kern_offset;    /**< Kerning offset  of the glyph in the respective kerning array */
    const uint8_t   kern_length;    /**< Length of the kerning information of the glyph */
    const int16_t   bbox_xmin;      /**< Minimum x of the glyph's bounding box */
    const int16_t   bbox_ymin;      /**< Minimum y of the glyph's bounding box */
    const int16_t   bbox_xmax;      /**< Maximum x of the glyph's bounding box */
    const int16_t   bbox_ymax;      /**< Maximum y of the glyph's bounding box */
} nema_vg_glyph_t;

/** NemaVG vector font range data struct */
typedef struct {
    const uint32_t first;          /**< Unicode value of the first value of the range */
    const uint32_t last;           /**< Unicode value of the last value of the range */
    const nema_vg_glyph_t *glyphs; /**< Pointer to the array of glyphs */
} nema_vg_font_range_t;

/** NemaVG vector font data struct*/
typedef struct {
    const uint32_t             version;          /**< Font version */
    const nema_vg_font_range_t *ranges;          /**< Pointer to the array of ranges */
    const nema_vg_float_t      *data;            /**< Pointer to the data of the vector font */
    const size_t                data_length;     /**< Length of the vector font data*/
    const uint8_t              *segment;         /**< Pointer to the segments of the vector font */
    const size_t                segment_length;  /**< Length of the vector font segments */
    const float                 size;            /**< Default font size (height) */
    const float                 xAdvance;        /**< Default advance width. If the space character is included in the ranges, then its advance width is set */
    const float                 ascender;        /**< Vertical distance from the baseline to the highest point of the font */
    const float                 descender;       /**< Vertical distance from the baseline to the lowest point of the font */
    const nema_vg_kern_pair_t  *kern_pairs;      /**< Pointer to the array of the font's kerning pairs */
    uint32_t                    flags;           /**< Bit field, reserved for future use */
} nema_vg_font_t;

/** \brief Bind the font to use in future nema_vg_print() calls. Sets error code if font is not supported.
 *
 * \param font Pointer to the vector font
 *
 */
void nema_vg_bind_font(nema_vg_font_t *font);

/** \brief Sets the size of the bound font. Future nema_vg_print() and nema_vg_print_char() calls will print using the last set size.
 *
 * \param font Pointer to the vector font
 *
 */
void nema_vg_set_font_size(float size);


/** \brief Print pre-formatted text
 *
 * \param paint Pointer to the current paint object (contains the text color)
 * \param str Pointer to string
 * \param x X coordinate of text-area's top-left corner
 * \param y Y coordinate of text-area's top-left corner
 * \param w Max allowed width
 * \param h Max allowed height
 * \param align Alignment and wrapping mode
 * \param m Transformation matrix
 *
 */
void nema_vg_print(NEMA_VG_PAINT_HANDLE paint, const char *str, float x, float y, float w, float h, uint32_t align, nema_matrix3x3_t m);


/** \brief Get the bounding box's width and height of a vector string. Prior to calling this function, "nema_vg_set_font_size" must be called first.
 *
 * \param str Pointer to string
 * \param w Pointer to variable where width should be written
 * \param h Pointer to variable where height should be written
 * \param max_w Max allowed width
 * \param size font size
 * \param wrap enable text wraping
 * \return Number of carriage returns
 *
 */
int  nema_vg_string_get_bbox(const char *str, float *w, float *h, float max_w, uint32_t wrap);


/** \brief Get the text ascender value in point units. Font size must be set pror to calling this function.
 *
 * \return Ascender pt
 *
 */
int nema_vg_get_ascender_pt(void);

/** \brief Print a single character
 *
 * \details The position of the character is determined by the 'orientation' argument.
 * x and y arguments define a point on the baseline. If the orientation is left to right (LTR),
 * the character will be placed to the right of the (x, y) point. Right to left (RTL) will place
 * the character to the left of the (x, y) point. Top to bottom (TTB) will have the same effect as
 * RTL and bottom to top (BTT) will place the character higher than the (x, y) point by an offset
 * equal to the font height.
 *
 * \param paint Pointer to the current paint object (contains the text color)
 * \param ch Character to be printed
 * \param x X coordinate of character's top-left or top-right corner (controlled by the 'orientation' parameter)
 * \param y Y coordinate of character's top-left or bottom-left corner (controlled by the 'orientation' parameter)
 * \param m Transformation matrix
 * \param orientation Character orientation (see NEMA_VG_CHAR_* defines)
 * \return Character width in pixels
 *
 */
float nema_vg_print_char(NEMA_VG_PAINT_HANDLE paint, char ch, float x, float y, nema_matrix3x3_t m, uint32_t orientation);

/** \brief Generates a raster font from a vector font
 *
 * \details Creates an 8-bpp raster version of the bound vector font. Performs dynamic memory allocation in
 * the graphics memory (for the font bitmaps) and in the heap (for the data structs accessed by the CPU).
 * When the font is no longer needed, function "nema_vg_destroy_raster_font()" can be used to free the allocated
 * memory. The font generation may fail when there is not enough memory to generate the font or when the font
 * size is greater than the height of the framebuffer.
 *
 * \param size The size of the font that will be generated
 * \param pool Memory pool to store the font bitmaps
 * \return Pointer to the data struct of generated raster font. If the font was not generated (due to insufficient memory) it returns NULL.
 *
 */
nema_font_t* nema_vg_generate_raster_font(int size, int pool);

/** \brief Frees the memory that was allocated for a font data struct
 *
 * \details This function frees memory that was allocated at runtime. Input must be
 *  a font data struct that was generated by the "nema_vg_generate_raster_font" function.
 *
 * \param font Pointer to the raster font data struct that will be erased from the memory
 *
 */
void nema_vg_destroy_raster_font(nema_font_t *font);

#ifdef __cplusplus
}
#endif
#endif // NEMA_VG_FONT_H_
