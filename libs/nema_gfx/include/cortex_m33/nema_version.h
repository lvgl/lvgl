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
 * @brief Contains version numbers for NemaGFX API and the currently supported font version.
 *
 */

#ifndef NEMA_VERSION_H__
#define NEMA_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#define NEMA_MAJOR_VERSION    0x01U       /**< NemaGFX API version, major number */
#define NEMA_MINOR_VERSION    0x04U       /**< NemaGFX API version, minor number */
#define NEMA_REVISION_VERSION 0x0BU      /**< NemaGFX API version, revision number */
#define NEMA_IMP_VERSION      0x00240600U /**< NemaGFX API version, implementation in format 0x00YYMM00 (Y: year, M: month) */

#define NEMA_API_VERSION ((NEMA_MAJOR_VERSION << 16) + (NEMA_MINOR_VERSION << 8) + (NEMA_REVISION_VERSION)) /**< NemaGFX API version in format 0x00MMmmrr (M:major, m:minor, r:revision if any) */

#define NEMA_FONT_VERSION     0x01U       /**< Current font version */


#ifdef __cplusplus
}
#endif

#endif //NEMA_VERSION_H__
