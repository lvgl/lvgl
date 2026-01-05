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

#ifndef NEMA_ERROR_H__
#define NEMA_ERROR_H__

#include "nema_sys_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

// Error Handling

#define  NEMA_ERR_NO_ERROR                     (0x00000000U) /**< No error has occured */
#define  NEMA_ERR_SYS_INIT_FAILURE             (0x00000001U) /**< System initialization failure */
#define  NEMA_ERR_GPU_ABSENT                   (0x00000002U) /**< Nema GPU is absent */
#define  NEMA_ERR_RB_INIT_FAILURE              (0x00000004U) /**< Ring buffer initialization failure */
#define  NEMA_ERR_NON_EXPANDABLE_CL_FULL       (0x00000008U) /**< Non expandable command list is full*/
#define  NEMA_ERR_CL_EXPANSION                 (0x00000010U) /**< Command list expansion error */
#define  NEMA_ERR_OUT_OF_GFX_MEMORY            (0x00000020U) /**< Graphics memory is full */
#define  NEMA_ERR_OUT_OF_HOST_MEMORY           (0x00000040U) /**< Host memory is full */
#define  NEMA_ERR_NO_BOUND_CL                  (0x00000080U) /**< There is no bound command list */
#define  NEMA_ERR_NO_BOUND_FONT                (0x00000100U) /**< There is no bound font */
#define  NEMA_ERR_GFX_MEMORY_INIT              (0x00000200U) /**< Graphics memory initialization failure */
#define  NEMA_ERR_DRIVER_FAILURE               (0x00000400U) /**< Nema GPU Kernel Driver failure*/
#define  NEMA_ERR_MUTEX_INIT                   (0x00000800U) /**< Mutex initialization failure*/
#define  NEMA_ERR_INVALID_BO                   (0x00001000U) /**< Invalid buffer provided*/
#define  NEMA_ERR_INVALID_CL                   (0x00002000U) /**< Invalid CL provided*/
#define  NEMA_ERR_INVALID_CL_ALIGMENT          (0x00004000U) /**< Invalid CL buffer alignment*/
#define  NEMA_ERR_NO_INIT                      (0x00008000U) /**< GFX uninitialised*/

/** \brief Return Error Id
 *
 * \return 0 if no error exists
 *
 */
uint32_t nema_get_error(void);


#ifdef __cplusplus
}
#endif

#endif // NEMA_ERROR_H__
