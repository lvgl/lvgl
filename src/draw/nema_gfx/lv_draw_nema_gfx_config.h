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
 * @file touchgfx/hal/Config.hpp
 *
 * Declares various macros defining which section to use during linking.
 */
#ifndef TOUCHGFX_CONFIG_HPP
#define TOUCHGFX_CONFIG_HPP

/** A macro to generate the passed argument in double quotes */
#define STR(X) STR_I(X)
/** A macro to generate the passed argument in double quotes */
#define STR_I(X) #X

/**
 * Compiler specific macros.
 * LOCATION_PRAGMA is a macro for placing elements in the proper memory section
 * LOCATION_ATTRIBUTE is a macro for placing attributes in the proper memory section
 * FORCE_INLINE_FUNCTION is used to force inline of time critical functions.
 * TOUCHGFX_DEPRECATED is used to mark a function deprecated.
 */
#ifdef SIMULATOR

#define LOCATION_PRAGMA(name)
#define LOCATION_PRAGMA_NOLOAD(name)
#define LOCATION_ATTRIBUTE(name)
#define LOCATION_ATTRIBUTE_NOLOAD(name)
#define FORCE_INLINE_FUNCTION inline
#if defined(__GNUC__)
#define TOUCHGFX_DEPRECATED(message, decl) decl __attribute__((deprecated(message)))
#elif _MSC_VER >= 1900
// Visual Studio 2015 or newer
#define TOUCHGFX_DEPRECATED(message, decl) [[deprecated("Deprecated: " message)]] decl
#else
#define TOUCHGFX_DEPRECATED(message, decl) decl
#endif

#elif defined(__GNUC__) && !defined(__ARMCC_VERSION)

// xgcc
#define LOCATION_PRAGMA(name)
#define LOCATION_PRAGMA_NOLOAD(name)
#define LOCATION_ATTRIBUTE(name) __attribute__((section(STR(name)))) __attribute__((aligned(4)))
#define LOCATION_ATTRIBUTE_NOLOAD(name) __attribute__((section(STR(name)))) __attribute__((aligned(4)))
#define FORCE_INLINE_FUNCTION __attribute__((always_inline)) inline
#define TOUCHGFX_DEPRECATED(message, decl) [[deprecated(message)]] decl

#elif defined __ICCARM__

// IAR
#define LOCATION_PRAGMA(name) _Pragma(STR(location = name))
#define LOCATION_PRAGMA_NOLOAD(name) _Pragma(STR(location = name))
#define LOCATION_ATTRIBUTE(name)
#define LOCATION_ATTRIBUTE_NOLOAD(name)
#define FORCE_INLINE_FUNCTION _Pragma("inline=forced")
#if __IAR_SYSTEMS_ICC__ >= 9
#define TOUCHGFX_DEPRECATED(message, decl) [[deprecated(message)]] decl
#else
#define TOUCHGFX_DEPRECATED(message, decl) decl
#endif
#pragma diag_suppress = Pe236

#elif defined(__ARMCC_VERSION)

// Keil
#define LOCATION_PRAGMA(name)
#define LOCATION_PRAGMA_NOLOAD(name)
#define LOCATION_ATTRIBUTE(name) __attribute__((section(name))) __attribute__((aligned(4)))
#define LOCATION_ATTRIBUTE_NOLOAD(name) __attribute__((section(name), zero_init)) __attribute__((aligned(4)))
#define FORCE_INLINE_FUNCTION inline
#if __ARMCC_VERSION >= 6000000
// Only newer Keil support message to be given
#define TOUCHGFX_DEPRECATED(message, decl) decl __attribute__((deprecated(message)))
#else
#define TOUCHGFX_DEPRECATED(message, decl) decl __attribute__((deprecated))
#endif

#else

// Other/Unknown
#define LOCATION_PRAGMA(name)
#define LOCATION_PRAGMA_NOLOAD(name)
#define LOCATION_ATTRIBUTE(name)
#define LOCATION_ATTRIBUTE_NOLOAD(name)
#define FORCE_INLINE_FUNCTION
#define TOUCHGFX_DEPRECATED(message, decl) decl

#endif

/** A macro for placing Font Glyph in memory. */
#define FONT_GLYPH_LOCATION_FLASH_PRAGMA LOCATION_PRAGMA("FontFlashSection")
/** A macro for placing Font Glyph attribute in memory. */
#define FONT_GLYPH_LOCATION_FLASH_ATTRIBUTE LOCATION_ATTRIBUTE("FontFlashSection")

/** A macro for placing Font table in memory. */
#define FONT_TABLE_LOCATION_FLASH_PRAGMA LOCATION_PRAGMA("FontFlashSection")
/** A macro for placing Font table attribute in memory. */
#define FONT_TABLE_LOCATION_FLASH_ATTRIBUTE LOCATION_ATTRIBUTE("FontFlashSection")

/** A macro for placing Font lookup table in memory. */
#define FONT_SEARCHTABLE_LOCATION_FLASH_PRAGMA LOCATION_PRAGMA("FontSearchFlashSection")
/** A macro for placing Font table attribute in memory. */
#define FONT_SEARCHTABLE_LOCATION_FLASH_ATTRIBUTE LOCATION_ATTRIBUTE("FontSearchFlashSection")

/** A macro for placing Font kerning in memory. */
#define FONT_KERNING_LOCATION_FLASH_PRAGMA LOCATION_PRAGMA("FontSearchFlashSection")
/** A macro for placing Font kerning attribute in memory. */
#define FONT_KERNING_LOCATION_FLASH_ATTRIBUTE LOCATION_ATTRIBUTE("FontSearchFlashSection")

/** A macro for placing Text kerning in memory. */
#define TEXT_LOCATION_FLASH_PRAGMA LOCATION_PRAGMA("TextFlashSection")
/** A macro for placing Text attribute in memory. */
#define TEXT_LOCATION_FLASH_ATTRIBUTE LOCATION_ATTRIBUTE("TextFlashSection")

/** A generic macro for placing an element in memory. */
#define LOCATION_EXTFLASH_PRAGMA LOCATION_PRAGMA("ExtFlashSection")
/** A generic macro for placing an element attribute in memory. */
#define LOCATION_EXTFLASH_ATTRIBUTE LOCATION_ATTRIBUTE("ExtFlashSection")

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

#endif // TOUCHGFX_CONFIG_HPP
