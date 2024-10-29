/******************************************************************************
* Copyright (c) 2018(-2024) STMicroelectronics.
* All rights reserved.
*
* This file is part of the TouchGFX 4.23.2 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
*******************************************************************************/

/**
 * @file lv_draw_nema_gfx_config.h
 *
 * Declares various macros defining which section to use during linking.
 */
#ifndef LV_DRAW_NEMA_GFX_CONFIG_H
#define LV_DRAW_NEMA_GFX_CONFIG_H

/** A macro to generate the passed argument in double quotes */
#define STR(X) STR_I(X)
/** A macro to generate the passed argument in double quotes */
#define STR_I(X) #X

/**
 * Compiler specific macros.
 * LOCATION_PRAGMA is a macro for placing elements in the proper memory section
 * LOCATION_ATTRIBUTE is a macro for placing attributes in the proper memory section
 * FORCE_INLINE_FUNCTION is used to force inline of time critical functions.
 */
#ifdef SIMULATOR

    #define LOCATION_PRAGMA(name)
    #define LOCATION_PRAGMA_NOLOAD(name)
    #define LOCATION_ATTRIBUTE(name)
    #define LOCATION_ATTRIBUTE_NOLOAD(name)
    #define FORCE_INLINE_FUNCTION inline

#elif defined(__GNUC__) && !defined(__ARMCC_VERSION)

    // xgcc
    #define LOCATION_PRAGMA(name)
    #define LOCATION_PRAGMA_NOLOAD(name)
    #define LOCATION_ATTRIBUTE(name) __attribute__((section(STR(name)))) __attribute__((aligned(4)))
    #define LOCATION_ATTRIBUTE_NOLOAD(name) __attribute__((section(STR(name)))) __attribute__((aligned(4)))
    #define FORCE_INLINE_FUNCTION __attribute__((always_inline)) inline

#elif defined __ICCARM__

    // IAR
    #define LOCATION_PRAGMA(name) _Pragma(STR(location = name))
    #define LOCATION_PRAGMA_NOLOAD(name) _Pragma(STR(location = name))
    #define LOCATION_ATTRIBUTE(name)
    #define LOCATION_ATTRIBUTE_NOLOAD(name)
    #define FORCE_INLINE_FUNCTION _Pragma("inline=forced")
    #pragma diag_suppress = Pe236

#elif defined(__ARMCC_VERSION)

    // Keil
    #define LOCATION_PRAGMA(name)
    #define LOCATION_PRAGMA_NOLOAD(name)
    #define LOCATION_ATTRIBUTE(name) __attribute__((section(name))) __attribute__((aligned(4)))
    #define LOCATION_ATTRIBUTE_NOLOAD(name) __attribute__((section(name), zero_init)) __attribute__((aligned(4)))
    #define FORCE_INLINE_FUNCTION inline

#else

    // Other/Unknown
    #define LOCATION_PRAGMA(name)
    #define LOCATION_PRAGMA_NOLOAD(name)
    #define LOCATION_ATTRIBUTE(name)
    #define LOCATION_ATTRIBUTE_NOLOAD(name)
    #define FORCE_INLINE_FUNCTION

#endif

/**
 * To be able to use __restrict__ on the supported platform. The IAR compiler does not support
 * this.
 */
#ifdef __GNUC__
    #define RESTRICT __restrict__
#else
    #define RESTRICT
#endif // __GNUC__

/** Use KEEP to make sure the compiler does not remove this. */
#ifdef __ICCARM__
    #define KEEP __root
#else
    #define KEEP
#endif

#endif /* LV_DRAW_NEMA_GFX_CONFIG_H */
