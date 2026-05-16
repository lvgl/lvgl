/**
  ******************************************************************************
  * @file    nema_sys_defs.h
  * @author  MCD Application Team
  * @brief   Header file of NemaGFX System Definitions for STM32 Platforms.
  *          This file provides definition of types being used by the NemaGFX
  *          library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef NEMA_SYS_DEFS_H__
#define NEMA_SYS_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Temporarily added by ST */
#ifdef __ICCARM__
#define FORCE_INLINE _Pragma("inline = forced")
#else
#define FORCE_INLINE
#endif  
  
/* No Multi-Thread support */
#define TLS_VAR

#ifdef NEMA_VG_INVALIDATE_CACHE
void platform_disable_cache(void);  
void platform_invalidate_cache(void);

#define NEMA_VG_DISABLE_CACHE platform_disable_cache()
#define NEMA_VG_ENABLE_INVALIDATE_CACHE platform_invalidate_cache()
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NEMA_SYS_DEFS_H__ */
