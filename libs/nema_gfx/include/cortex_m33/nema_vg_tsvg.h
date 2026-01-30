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
 * @brief API for rendering .tsvg images
 *
 */

#ifndef NEMA_VG_TSVG_H_
#define NEMA_VG_TSVG_H_

#include "nema_vg_context.h"

/** \brief Draws a TSVG buffer
 *
 * \param buffer Pointer to the TSVG buffer that will be drawn
 *
 */
void
nema_vg_draw_tsvg(const void* buffer);

/** \brief Get the width and height of tsvg
 *
 * \param buffer Tsvg buffer
 * \param width return Tsvg width
 * \param height return Tsvg height
 *
 */
void nema_vg_get_tsvg_resolution(const void *buffer, uint32_t *width, uint32_t *height);

#endif // NEMA_VG_TSVG_H_
