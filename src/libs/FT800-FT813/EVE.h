/*
@file    EVE.h
@brief   Contains FT80x/FT81x/BT81x API definitions
@version 5.0
@date    2024-01-28
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2024 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

5.0
- started to add BT817 / BT818 defines
- cleanup: removed FT80x defines
- replaced BT81X_ENABLE with "EVE_GEN > 2"
- removed FT81X_ENABLE as FT81x already is the lowest supported chip revision now
- added more BT817 / BT818 defines
- removed undocumented registers and commands
- merged FT80x and FT81x definitions as FT81x is baseline now
- removed the history from before 4.0
- fixed typo: REG_AH_CYCLE_MAX -> REG_AH_HCYCLE_MAX
- re-arranged the host commands, removed EVE_CLKINT for BT817/BT818,
    removed FT80x EVE_CLK36M and EVE_CLK48M
- added EVE_OPT_OVERLAY
- removed the 4.0 history
- fixed some MISRA-C issues
- removed macro BEGIN(prim) - use (DL_BEGIN | EVE_BITMAPS) for example
- removed macro END() - use define DL_END
- removed macro RESTORE_CONTEXT() - use define DL_RESTORE_CONTEXT
- removed macro RETURN() - use define DL_RETURN
- removed macro SAVE_CONTEXT() - use define DL_SAVE_CONTEXT
- basic maintenance: checked for violations of white space and indent rules
- more linter fixes
- changed EVE_COMPRESSED_RGBA_ASTC_nxn_KHR to EVE_ASTC_nXn to fix linter
    warnings and used the opportunity to make these shorter
- added DL_COLOR_A as alternative to the COLOR_A macro
- added defines for all DL_ display list commands
- cleaned up the macros
- fix: changed DL_CLEAR_RGB to DL_CLEAR_COLOR_RGB
    as this is what the programming guide uses
- fix: renamed EVE_ROM_FONT_ADDR to EVE_ROM_FONTROOT
- added #ifdef __cplusplus / extern "C" to allow
    adding EVE_ functions to C++ code
- fix: typo REG_COPRO_PATCH_DTR -> REG_COPRO_PATCH_PTR
- started to convert the function-like macros to static inline functions to be
    a little friendlier towards C++ in regards of type-safety
- added type-casts to all simple macros
- converted some more function-like macros to static inline functions
- converted the rest of the function-like macros to static inline functions
- fix: forgot to comment out the EVE2 BITMAP_TRANSFORM_E when converting it to an inline function

*/

#ifndef EVE_H
#define EVE_H

#include "EVE_target.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "EVE_config.h"
#include "EVE_commands.h"

/* Memory */
#define EVE_RAM_G         ((uint32_t) 0x00000000UL)
#define EVE_ROM_CHIPID    ((uint32_t) 0x000C0000UL)
#define EVE_ROM_FONT      ((uint32_t) 0x001E0000UL)
#define EVE_ROM_FONTROOT  ((uint32_t) 0x002FFFFCUL)
#define EVE_RAM_DL        ((uint32_t) 0x00300000UL)
#define EVE_RAM_REG       ((uint32_t) 0x00302000UL)
#define EVE_RAM_CMD       ((uint32_t) 0x00308000UL)

/* Memory buffer sizes */
#define EVE_RAM_G_SIZE   ((uint32_t) 1024U*1024UL)
#define EVE_CMDFIFO_SIZE ((uint32_t) 4U*1024UL)
#define EVE_RAM_DL_SIZE  ((uint32_t) 8U*1024UL)

/* diplay list list commands, most need OR's arguments */
#define DL_DISPLAY       ((uint32_t) 0x00000000UL)
#define DL_BITMAP_SOURCE ((uint32_t) 0x01000000UL)
#define DL_CLEAR_COLOR_RGB ((uint32_t) 0x02000000UL)
#define DL_TAG           ((uint32_t) 0x03000000UL)
#define DL_COLOR_RGB     ((uint32_t) 0x04000000UL)
#define DL_BITMAP_HANDLE ((uint32_t) 0x05000000UL)
#define DL_CELL          ((uint32_t) 0x06000000UL)
#define DL_BITMAP_LAYOUT ((uint32_t) 0x07000000UL)
#define DL_BITMAP_SIZE   ((uint32_t) 0x08000000UL)
#define DL_ALPHA_FUNC    ((uint32_t) 0x09000000UL)
#define DL_STENCIL_FUNC  ((uint32_t) 0x0A000000UL)
#define DL_BLEND_FUNC    ((uint32_t) 0x0B000000UL)
#define DL_STENCIL_OP    ((uint32_t) 0x0C000000UL)
#define DL_POINT_SIZE    ((uint32_t) 0x0D000000UL)
#define DL_LINE_WIDTH    ((uint32_t) 0x0E000000UL)
#define DL_CLEAR_COLOR_A ((uint32_t) 0x0F000000UL)
#define DL_COLOR_A       ((uint32_t) 0x10000000UL)
#define DL_CLEAR_STENCIL ((uint32_t) 0x11000000UL)
#define DL_CLEAR_TAG     ((uint32_t) 0x12000000UL)
#define DL_STENCIL_MASK  ((uint32_t) 0x13000000UL)
#define DL_TAG_MASK      ((uint32_t) 0x14000000UL)
#define DL_BITMAP_TRANSFORM_A ((uint32_t) 0x15000000UL)
#define DL_BITMAP_TRANSFORM_B ((uint32_t) 0x16000000UL)
#define DL_BITMAP_TRANSFORM_C ((uint32_t) 0x17000000UL)
#define DL_BITMAP_TRANSFORM_D ((uint32_t) 0x18000000UL)
#define DL_BITMAP_TRANSFORM_E ((uint32_t) 0x19000000UL)
#define DL_BITMAP_TRANSFORM_F ((uint32_t) 0x1A000000UL)
#define DL_SCISSOR_XY    ((uint32_t) 0x1B000000UL)
#define DL_SCISSOR_SIZE  ((uint32_t) 0x1C000000UL)
#define DL_CALL          ((uint32_t) 0x1D000000UL)
#define DL_JUMP          ((uint32_t) 0x1E000000UL)
#define DL_BEGIN         ((uint32_t) 0x1F000000UL)
#define DL_COLOR_MASK    ((uint32_t) 0x20000000UL)
#define DL_END           ((uint32_t) 0x21000000UL)
#define DL_SAVE_CONTEXT  ((uint32_t) 0x22000000UL)
#define DL_RESTORE_CONTEXT ((uint32_t) 0x23000000UL)
#define DL_RETURN        ((uint32_t) 0x24000000UL)
#define DL_MACRO         ((uint32_t) 0x25000000UL)
#define DL_CLEAR         ((uint32_t) 0x26000000UL)
#define DL_VERTEX_FORMAT ((uint32_t) 0x27000000UL)
#define DL_BITMAP_LAYOUT_H ((uint32_t) 0x28000000UL)
#define DL_BITMAP_SIZE_H ((uint32_t) 0x29000000UL)
#define DL_PALETTE_SOURCE ((uint32_t) 0x2A000000UL)
#define DL_VERTEX_TRANSLATE_X ((uint32_t) 0x2B000000UL)
#define DL_VERTEX_TRANSLATE_Y ((uint32_t) 0x2C000000UL)
#define DL_NOP            ((uint32_t) 0x2D000000UL)

#define DL_VERTEX2F     ((uint32_t) 0x40000000UL)
#define DL_VERTEX2II    ((uint32_t) 0x80000000UL)

#define CLR_COL     ((uint8_t) 0x4U)
#define CLR_STN     ((uint8_t) 0x2U)
#define CLR_TAG     ((uint8_t) 0x1U)

/* Host commands */
#define EVE_ACTIVE       ((uint8_t) 0x00U) /* place EVE in active state */
#define EVE_STANDBY      ((uint8_t) 0x41U) /* place EVE in Standby (clk running) */
#define EVE_SLEEP        ((uint8_t) 0x42U) /* place EVE in Sleep (clk off) */
#define EVE_CLKEXT       ((uint8_t) 0x44U) /* select external clock source */
#define EVE_CLKINT       ((uint8_t) 0x48U) /* select internal clock source, not a valid option for BT817 / BT818 */
#define EVE_PWRDOWN      ((uint8_t) 0x50U) /* place EVE in Power Down (core off) */
#define EVE_CLKSEL       ((uint8_t) 0x61U) /* configure system clock */
#define EVE_RST_PULSE    ((uint8_t) 0x68U) /* reset core - all registers default and processors reset */
#define EVE_CORERST      ((uint8_t) 0x68U) /* reset core - all registers default and processors reset */
#define EVE_PINDRIVE     ((uint8_t) 0x70U) /* setup drive strength for various pins */
#define EVE_PIN_PD_STATE ((uint8_t) 0x71U) /* setup how pins behave during power down */

/* Graphic command defines */
#define EVE_NEVER      ((uint8_t) 0UL)
#define EVE_LESS       ((uint8_t) 1UL)
#define EVE_LEQUAL     ((uint8_t) 2UL)
#define EVE_GREATER    ((uint8_t) 3UL)
#define EVE_GEQUAL     ((uint8_t) 4UL)
#define EVE_EQUAL      ((uint8_t) 5UL)
#define EVE_NOTEQUAL   ((uint8_t) 6UL)
#define EVE_ALWAYS     ((uint8_t) 7UL)

/* Bitmap formats */
#define EVE_ARGB1555   ((uint8_t) 0UL)
#define EVE_L1         ((uint8_t) 1UL)
#define EVE_L4         ((uint8_t) 2UL)
#define EVE_L8         ((uint8_t) 3UL)
#define EVE_RGB332     ((uint8_t) 4UL)
#define EVE_ARGB2      ((uint8_t) 5UL)
#define EVE_ARGB4      ((uint8_t) 6UL)
#define EVE_RGB565     ((uint8_t) 7UL)
#define EVE_PALETTED   ((uint8_t) 8UL)
#define EVE_TEXT8X8    ((uint8_t) 9UL)
#define EVE_TEXTVGA    ((uint8_t) 10UL)
#define EVE_BARGRAPH   ((uint8_t) 11UL)

/* Bitmap filter types */
#define EVE_NEAREST    ((uint8_t) 0UL)
#define EVE_BILINEAR   ((uint8_t) 1UL)

/* Bitmap wrap types */
#define EVE_BORDER     ((uint8_t) 0UL)
#define EVE_REPEAT     ((uint8_t) 1UL)

/* Stencil defines */
#define EVE_KEEP       ((uint8_t) 1UL)
#define EVE_REPLACE    ((uint8_t) 2UL)
#define EVE_INCR       ((uint8_t) 3UL)
#define EVE_DECR       ((uint8_t) 4UL)
#define EVE_INVERT     ((uint8_t) 5UL)

/* Graphics display list swap defines */
#define EVE_DLSWAP_DONE   ((uint8_t) 0UL)
#define EVE_DLSWAP_LINE   ((uint8_t) 1UL)
#define EVE_DLSWAP_FRAME  ((uint8_t) 2UL)

/* Interrupt bits */
#define EVE_INT_SWAP          ((uint8_t) 0x01)
#define EVE_INT_TOUCH         ((uint8_t) 0x02)
#define EVE_INT_TAG           ((uint8_t) 0x04)
#define EVE_INT_SOUND         ((uint8_t) 0x08)
#define EVE_INT_PLAYBACK      ((uint8_t) 0x10)
#define EVE_INT_CMDEMPTY      ((uint8_t) 0x20)
#define EVE_INT_CMDFLAG       ((uint8_t) 0x40)
#define EVE_INT_CONVCOMPLETE  ((uint8_t) 0x80)

/* Touch mode */
#define EVE_TMODE_OFF        ((uint8_t) 0U)
#define EVE_TMODE_ONESHOT    ((uint8_t) 1U)
#define EVE_TMODE_FRAME      ((uint8_t) 2U)
#define EVE_TMODE_CONTINUOUS ((uint8_t) 3U)

/* Alpha blending */
#define EVE_ZERO                 ((uint32_t) 0UL)
#define EVE_ONE                  ((uint32_t) 1UL)
#define EVE_SRC_ALPHA            ((uint32_t) 2UL)
#define EVE_DST_ALPHA            ((uint32_t) 3UL)
#define EVE_ONE_MINUS_SRC_ALPHA  ((uint32_t) 4UL)
#define EVE_ONE_MINUS_DST_ALPHA  ((uint32_t) 5UL)

/* Graphics primitives */
#define EVE_BITMAPS              ((uint32_t) 1UL)
#define EVE_POINTS               ((uint32_t) 2UL)
#define EVE_LINES                ((uint32_t) 3UL)
#define EVE_LINE_STRIP           ((uint32_t) 4UL)
#define EVE_EDGE_STRIP_R         ((uint32_t) 5UL)
#define EVE_EDGE_STRIP_L         ((uint32_t) 6UL)
#define EVE_EDGE_STRIP_A         ((uint32_t) 7UL)
#define EVE_EDGE_STRIP_B         ((uint32_t) 8UL)
#define EVE_RECTS                ((uint32_t) 9UL)
#define EVE_INT_G8               ((uint32_t) 18UL)
#define EVE_INT_L8C              ((uint32_t) 12UL)
#define EVE_INT_VGA              ((uint32_t) 13UL)
#define EVE_PALETTED565          ((uint32_t) 14UL)
#define EVE_PALETTED4444         ((uint32_t) 15UL)
#define EVE_PALETTED8            ((uint32_t) 16UL)
#define EVE_L2                   ((uint32_t) 17UL)

/* Widget command options */
#define EVE_OPT_MONO             ((uint16_t) 1U)
#define EVE_OPT_NODL             ((uint16_t) 2U)
#define EVE_OPT_FLAT             ((uint16_t) 256U)
#define EVE_OPT_CENTERX          ((uint16_t) 512U)
#define EVE_OPT_CENTERY          ((uint16_t) 1024U)
#define EVE_OPT_CENTER           (EVE_OPT_CENTERX | EVE_OPT_CENTERY)
#define EVE_OPT_NOBACK           ((uint16_t) 4096U)
#define EVE_OPT_NOTICKS          ((uint16_t) 8192U)
#define EVE_OPT_NOHM             ((uint16_t) 16384U)
#define EVE_OPT_NOPOINTER        ((uint16_t) 16384U)
#define EVE_OPT_NOSECS           ((uint16_t) 32768U)
#define EVE_OPT_NOHANDS          ((uint16_t) 49152U)
#define EVE_OPT_RIGHTX           ((uint16_t) 2048U)
#define EVE_OPT_SIGNED           ((uint16_t) 256U)

#define EVE_OPT_MEDIAFIFO        ((uint16_t) 16U)
#define EVE_OPT_FULLSCREEN       ((uint16_t) 8U)
#define EVE_OPT_NOTEAR           ((uint16_t) 4U)
#define EVE_OPT_SOUND            ((uint16_t) 32U)

/* ADC */
#define EVE_ADC_DIFFERENTIAL     ((uint32_t) 1UL)
#define EVE_ADC_SINGLE_ENDED     ((uint32_t) 0UL)

/* Fonts */
#define EVE_NUMCHAR_PERFONT     ((uint32_t) 128UL)  /* number of font characters per bitmap handle */
#define EVE_FONT_TABLE_SIZE     ((uint32_t) 148UL)  /* size of the font table - utilized for loopup by the graphics engine */
#define EVE_FONT_TABLE_POINTER  ((uint32_t) 0xFFFFCUL) /* pointer to the inbuilt font tables starting from bitmap handle 16 */

/* Audio sample type defines */
#define EVE_LINEAR_SAMPLES       ((uint32_t) 0UL) /* 8bit signed samples */
#define EVE_ULAW_SAMPLES         ((uint32_t) 1UL) /* 8bit ulaw samples */
#define EVE_ADPCM_SAMPLES        ((uint32_t) 2UL) /* 4bit ima adpcm samples */

/* Synthesized sound */
#define EVE_SILENCE      ((uint8_t) 0x00U)
#define EVE_SQUAREWAVE   ((uint8_t) 0x01U)
#define EVE_SINEWAVE     ((uint8_t) 0x02U)
#define EVE_SAWTOOTH     ((uint8_t) 0x03U)
#define EVE_TRIANGLE     ((uint8_t) 0x04U)
#define EVE_BEEPING      ((uint8_t) 0x05U)
#define EVE_ALARM        ((uint8_t) 0x06U)
#define EVE_WARBLE       ((uint8_t) 0x07U)
#define EVE_CAROUSEL     ((uint8_t) 0x08U)
#define EVE_PIPS(n)      ((uint8_t) (0x0FU + (n)))
#define EVE_HARP         ((uint8_t) 0x40U)
#define EVE_XYLOPHONE    ((uint8_t) 0x41U)
#define EVE_TUBA         ((uint8_t) 0x42U)
#define EVE_GLOCKENSPIEL ((uint8_t) 0x43U)
#define EVE_ORGAN        ((uint8_t) 0x44U)
#define EVE_TRUMPET      ((uint8_t) 0x45U)
#define EVE_PIANO        ((uint8_t) 0x46U)
#define EVE_CHIMES       ((uint8_t) 0x47U)
#define EVE_MUSICBOX     ((uint8_t) 0x48U)
#define EVE_BELL         ((uint8_t) 0x49U)
#define EVE_CLICK        ((uint8_t) 0x50U)
#define EVE_SWITCH       ((uint8_t) 0x51U)
#define EVE_COWBELL      ((uint8_t) 0x52U)
#define EVE_NOTCH        ((uint8_t) 0x53U)
#define EVE_HIHAT        ((uint8_t) 0x54U)
#define EVE_KICKDRUM     ((uint8_t) 0x55U)
#define EVE_POP          ((uint8_t) 0x56U)
#define EVE_CLACK        ((uint8_t) 0x57U)
#define EVE_CHACK        ((uint8_t) 0x58U)
#define EVE_MUTE         ((uint8_t) 0x60U)
#define EVE_UNMUTE       ((uint8_t) 0x61U)

/* Synthesized sound frequencies, midi note */
#define EVE_MIDI_A0   ((uint8_t) 21U)
#define EVE_MIDI_A_0  ((uint8_t) 22U)
#define EVE_MIDI_B0   ((uint8_t) 23U)
#define EVE_MIDI_C1   ((uint8_t) 24U)
#define EVE_MIDI_C_1  ((uint8_t) 25U)
#define EVE_MIDI_D1   ((uint8_t) 26U)
#define EVE_MIDI_D_1  ((uint8_t) 27U)
#define EVE_MIDI_E1   ((uint8_t) 28U)
#define EVE_MIDI_F1   ((uint8_t) 29U)
#define EVE_MIDI_F_1  ((uint8_t) 30U)
#define EVE_MIDI_G1   ((uint8_t) 31U)
#define EVE_MIDI_G_1  ((uint8_t) 32U)
#define EVE_MIDI_A1   ((uint8_t) 33U)
#define EVE_MIDI_A_1  ((uint8_t) 34U)
#define EVE_MIDI_B1   ((uint8_t) 35U)
#define EVE_MIDI_C2   ((uint8_t) 36U)
#define EVE_MIDI_C_2  ((uint8_t) 37U)
#define EVE_MIDI_D2   ((uint8_t) 38U)
#define EVE_MIDI_D_2  ((uint8_t) 39U)
#define EVE_MIDI_E2   ((uint8_t) 40U)
#define EVE_MIDI_F2   ((uint8_t) 41U)
#define EVE_MIDI_F_2  ((uint8_t) 42U)
#define EVE_MIDI_G2   ((uint8_t) 43U)
#define EVE_MIDI_G_2  ((uint8_t) 44U)
#define EVE_MIDI_A2   ((uint8_t) 45U)
#define EVE_MIDI_A_2  ((uint8_t) 46U)
#define EVE_MIDI_B2   ((uint8_t) 47U)
#define EVE_MIDI_C3   ((uint8_t) 48U)
#define EVE_MIDI_C_3  ((uint8_t) 49U)
#define EVE_MIDI_D3   ((uint8_t) 50U)
#define EVE_MIDI_D_3  ((uint8_t) 51U)
#define EVE_MIDI_E3   ((uint8_t) 52U)
#define EVE_MIDI_F3   ((uint8_t) 53U)
#define EVE_MIDI_F_3  ((uint8_t) 54U)
#define EVE_MIDI_G3   ((uint8_t) 55U)
#define EVE_MIDI_G_3  ((uint8_t) 56U)
#define EVE_MIDI_A3   ((uint8_t) 57U)
#define EVE_MIDI_A_3  ((uint8_t) 58U)
#define EVE_MIDI_B3   ((uint8_t) 59U)
#define EVE_MIDI_C4   ((uint8_t) 60U)
#define EVE_MIDI_C_4  ((uint8_t) 61U)
#define EVE_MIDI_D4   ((uint8_t) 62U)
#define EVE_MIDI_D_4  ((uint8_t) 63U)
#define EVE_MIDI_E4   ((uint8_t) 64U)
#define EVE_MIDI_F4   ((uint8_t) 65U)
#define EVE_MIDI_F_4  ((uint8_t) 66U)
#define EVE_MIDI_G4   ((uint8_t) 67U)
#define EVE_MIDI_G_4  ((uint8_t) 68U)
#define EVE_MIDI_A4   ((uint8_t) 69U)
#define EVE_MIDI_A_4  ((uint8_t) 70U)
#define EVE_MIDI_B4   ((uint8_t) 71U)
#define EVE_MIDI_C5   ((uint8_t) 72U)
#define EVE_MIDI_C_5  ((uint8_t) 73U)
#define EVE_MIDI_D5   ((uint8_t) 74U)
#define EVE_MIDI_D_5  ((uint8_t) 75U)
#define EVE_MIDI_E5   ((uint8_t) 76U)
#define EVE_MIDI_F5   ((uint8_t) 77U)
#define EVE_MIDI_F_5  ((uint8_t) 78U)
#define EVE_MIDI_G5   ((uint8_t) 79U)
#define EVE_MIDI_G_5  ((uint8_t) 80U)
#define EVE_MIDI_A5   ((uint8_t) 81U)
#define EVE_MIDI_A_5  ((uint8_t) 82U)
#define EVE_MIDI_B5   ((uint8_t) 83U)
#define EVE_MIDI_C6   ((uint8_t) 84U)
#define EVE_MIDI_C_6  ((uint8_t) 85U)
#define EVE_MIDI_D6   ((uint8_t) 86U)
#define EVE_MIDI_D_6  ((uint8_t) 87U)
#define EVE_MIDI_E6   ((uint8_t) 88U)
#define EVE_MIDI_F6   ((uint8_t) 89U)
#define EVE_MIDI_F_6  ((uint8_t) 90U)
#define EVE_MIDI_G6   ((uint8_t) 91U)
#define EVE_MIDI_G_6  ((uint8_t) 92U)
#define EVE_MIDI_A6   ((uint8_t) 93U)
#define EVE_MIDI_A_6  ((uint8_t) 94U)
#define EVE_MIDI_B6   ((uint8_t) 95U)
#define EVE_MIDI_C7   ((uint8_t) 96U)
#define EVE_MIDI_C_7  ((uint8_t) 97U)
#define EVE_MIDI_D7   ((uint8_t) 98U)
#define EVE_MIDI_D_7  ((uint8_t) 99U)
#define EVE_MIDI_E7   ((uint8_t) 100U)
#define EVE_MIDI_F7   ((uint8_t) 101U)
#define EVE_MIDI_F_7  ((uint8_t) 102U)
#define EVE_MIDI_G7   ((uint8_t) 103U)
#define EVE_MIDI_G_7  ((uint8_t) 104U)
#define EVE_MIDI_A7   ((uint8_t) 105U)
#define EVE_MIDI_A_7  ((uint8_t) 106U)
#define EVE_MIDI_B7   ((uint8_t) 107U)
#define EVE_MIDI_C8   ((uint8_t) 108U)

/* GPIO bits */
#define EVE_GPIO0  ((uint8_t) 0U)
#define EVE_GPIO1  ((uint8_t) 1U) /* default gpio pin for audio shutdown, 1 - enable, 0 - disable */
#define EVE_GPIO7  ((uint8_t) 7U) /* default gpio pin for display enable, 1 - enable, 0 - disable */

/* Display rotation */
#define EVE_DISPLAY_0   ((uint8_t) 0U) /* 0 degrees rotation */
#define EVE_DISPLAY_180 ((uint8_t) 1U) /* 180 degrees rotation */

/* Commands */
#define CMD_APPEND       ((uint32_t) 0xFFFFFF1EUL)
#define CMD_BGCOLOR      ((uint32_t) 0xFFFFFF09UL)
#define CMD_BUTTON       ((uint32_t) 0xFFFFFF0DUL)
#define CMD_CALIBRATE    ((uint32_t) 0xFFFFFF15UL)
#define CMD_CLOCK        ((uint32_t) 0xFFFFFF14UL)
#define CMD_COLDSTART    ((uint32_t) 0xFFFFFF32UL)
#define CMD_DIAL         ((uint32_t) 0xFFFFFF2DUL)
#define CMD_DLSTART      ((uint32_t) 0xFFFFFF00UL)
#define CMD_FGCOLOR      ((uint32_t) 0xFFFFFF0AUL)
#define CMD_GAUGE        ((uint32_t) 0xFFFFFF13UL)
#define CMD_GETMATRIX    ((uint32_t) 0xFFFFFF33UL)
#define CMD_GETPROPS     ((uint32_t) 0xFFFFFF25UL)
#define CMD_GETPTR       ((uint32_t) 0xFFFFFF23UL)
#define CMD_GRADCOLOR    ((uint32_t) 0xFFFFFF34UL)
#define CMD_GRADIENT     ((uint32_t) 0xFFFFFF0BUL)
#define CMD_INFLATE      ((uint32_t) 0xFFFFFF22UL)
#define CMD_INTERRUPT    ((uint32_t) 0xFFFFFF02UL)
#define CMD_KEYS         ((uint32_t) 0xFFFFFF0EUL)
#define CMD_LOADIDENTITY ((uint32_t) 0xFFFFFF26UL)
#define CMD_LOADIMAGE    ((uint32_t) 0xFFFFFF24UL)
#define CMD_LOGO         ((uint32_t) 0xFFFFFF31UL)
#define CMD_MEDIAFIFO    ((uint32_t) 0xFFFFFF39UL)
#define CMD_MEMCPY       ((uint32_t) 0xFFFFFF1DUL)
#define CMD_MEMCRC       ((uint32_t) 0xFFFFFF18UL)
#define CMD_MEMSET       ((uint32_t) 0xFFFFFF1BUL)
#define CMD_MEMWRITE     ((uint32_t) 0xFFFFFF1AUL)
#define CMD_MEMZERO      ((uint32_t) 0xFFFFFF1CUL)
#define CMD_NUMBER       ((uint32_t) 0xFFFFFF2EUL)
#define CMD_PLAYVIDEO    ((uint32_t) 0xFFFFFF3AUL)
#define CMD_PROGRESS     ((uint32_t) 0xFFFFFF0FUL)
#define CMD_REGREAD      ((uint32_t) 0xFFFFFF19UL)
#define CMD_ROMFONT      ((uint32_t) 0xFFFFFF3FUL)
#define CMD_ROTATE       ((uint32_t) 0xFFFFFF29UL)
#define CMD_SCALE        ((uint32_t) 0xFFFFFF28UL)
#define CMD_SCREENSAVER  ((uint32_t) 0xFFFFFF2FUL)
#define CMD_SCROLLBAR    ((uint32_t) 0xFFFFFF11UL)
#define CMD_SETBASE      ((uint32_t) 0xFFFFFF38UL)
#define CMD_SETBITMAP    ((uint32_t) 0xFFFFFF43UL)
#define CMD_SETFONT      ((uint32_t) 0xFFFFFF2BUL)
#define CMD_SETFONT2     ((uint32_t) 0xFFFFFF3BUL)
#define CMD_SETMATRIX    ((uint32_t) 0xFFFFFF2AUL)
#define CMD_SETROTATE    ((uint32_t) 0xFFFFFF36UL)
#define CMD_SETSCRATCH   ((uint32_t) 0xFFFFFF3CUL)
#define CMD_SKETCH       ((uint32_t) 0xFFFFFF30UL)
#define CMD_SLIDER       ((uint32_t) 0xFFFFFF10UL)
#define CMD_SNAPSHOT     ((uint32_t) 0xFFFFFF1FUL)
#define CMD_SNAPSHOT2    ((uint32_t) 0xFFFFFF37UL)
#define CMD_SPINNER      ((uint32_t) 0xFFFFFF16UL)
#define CMD_STOP         ((uint32_t) 0xFFFFFF17UL)
#define CMD_SWAP         ((uint32_t) 0xFFFFFF01UL)
#define CMD_TEXT         ((uint32_t) 0xFFFFFF0CUL)
#define CMD_TOGGLE       ((uint32_t) 0xFFFFFF12UL)
#define CMD_TRACK        ((uint32_t) 0xFFFFFF2CUL)
#define CMD_TRANSLATE    ((uint32_t) 0xFFFFFF27UL)
#define CMD_VIDEOFRAME   ((uint32_t) 0xFFFFFF41UL)
#define CMD_VIDEOSTART   ((uint32_t) 0xFFFFFF40UL)

/* Registers */
#define REG_ANA_COMP         ((uint32_t) 0x00302184UL) /* only listed in datasheet */
#define REG_BIST_EN          ((uint32_t) 0x00302174UL) /* only listed in datasheet */
#define REG_CLOCK            ((uint32_t) 0x00302008UL)
#define REG_CMDB_SPACE       ((uint32_t) 0x00302574UL)
#define REG_CMDB_WRITE       ((uint32_t) 0x00302578UL)
#define REG_CMD_DL           ((uint32_t) 0x00302100UL)
#define REG_CMD_READ         ((uint32_t) 0x003020f8UL)
#define REG_CMD_WRITE        ((uint32_t) 0x003020fcUL)
#define REG_CPURESET         ((uint32_t) 0x00302020UL)
#define REG_CSPREAD          ((uint32_t) 0x00302068UL)
#define REG_CTOUCH_EXTENDED  ((uint32_t) 0x00302108UL)
#define REG_CTOUCH_TOUCH0_XY ((uint32_t) 0x00302124UL) /* only listed in datasheet */
#define REG_CTOUCH_TOUCH4_X  ((uint32_t) 0x0030216cUL)
#define REG_CTOUCH_TOUCH4_Y  ((uint32_t) 0x00302120UL)
#define REG_CTOUCH_TOUCH1_XY ((uint32_t) 0x0030211cUL)
#define REG_CTOUCH_TOUCH2_XY ((uint32_t) 0x0030218cUL)
#define REG_CTOUCH_TOUCH3_XY ((uint32_t) 0x00302190UL)
#define REG_TOUCH_CONFIG     ((uint32_t) 0x00302168UL)
#define REG_DATESTAMP        ((uint32_t) 0x00302564UL) /* only listed in datasheet */
#define REG_DITHER           ((uint32_t) 0x00302060UL)
#define REG_DLSWAP           ((uint32_t) 0x00302054UL)
#define REG_FRAMES           ((uint32_t) 0x00302004UL)
#define REG_FREQUENCY        ((uint32_t) 0x0030200cUL)
#define REG_GPIO             ((uint32_t) 0x00302094UL)
#define REG_GPIOX            ((uint32_t) 0x0030209cUL)
#define REG_GPIOX_DIR        ((uint32_t) 0x00302098UL)
#define REG_GPIO_DIR         ((uint32_t) 0x00302090UL)
#define REG_HCYCLE           ((uint32_t) 0x0030202cUL)
#define REG_HOFFSET          ((uint32_t) 0x00302030UL)
#define REG_HSIZE            ((uint32_t) 0x00302034UL)
#define REG_HSYNC0           ((uint32_t) 0x00302038UL)
#define REG_HSYNC1           ((uint32_t) 0x0030203cUL)
#define REG_ID               ((uint32_t) 0x00302000UL)
#define REG_INT_EN           ((uint32_t) 0x003020acUL)
#define REG_INT_FLAGS        ((uint32_t) 0x003020a8UL)
#define REG_INT_MASK         ((uint32_t) 0x003020b0UL)
#define REG_MACRO_0          ((uint32_t) 0x003020d8UL)
#define REG_MACRO_1          ((uint32_t) 0x003020dcUL)
#define REG_MEDIAFIFO_READ   ((uint32_t) 0x00309014UL) /* only listed in programmers guide */
#define REG_MEDIAFIFO_WRITE  ((uint32_t) 0x00309018UL) /* only listed in programmers guide */
#define REG_OUTBITS          ((uint32_t) 0x0030205cUL)
#define REG_PCLK             ((uint32_t) 0x00302070UL)
#define REG_PCLK_POL         ((uint32_t) 0x0030206cUL)
#define REG_PLAY             ((uint32_t) 0x0030208cUL)
#define REG_PLAYBACK_FORMAT  ((uint32_t) 0x003020c4UL)
#define REG_PLAYBACK_FREQ    ((uint32_t) 0x003020c0UL)
#define REG_PLAYBACK_LENGTH  ((uint32_t) 0x003020b8UL)
#define REG_PLAYBACK_LOOP    ((uint32_t) 0x003020c8UL)
#define REG_PLAYBACK_PLAY    ((uint32_t) 0x003020ccUL)
#define REG_PLAYBACK_READPTR ((uint32_t) 0x003020bcUL)
#define REG_PLAYBACK_START   ((uint32_t) 0x003020b4UL)
#define REG_PWM_DUTY         ((uint32_t) 0x003020d4UL)
#define REG_PWM_HZ           ((uint32_t) 0x003020d0UL)
#define REG_RENDERMODE       ((uint32_t) 0x00302010UL) /* only listed in datasheet */
#define REG_ROTATE           ((uint32_t) 0x00302058UL)
#define REG_SNAPFORMAT       ((uint32_t) 0x0030201cUL) /* only listed in datasheet */
#define REG_SNAPSHOT         ((uint32_t) 0x00302018UL) /* only listed in datasheet */
#define REG_SNAPY            ((uint32_t) 0x00302014UL) /* only listed in datasheet */
#define REG_SOUND            ((uint32_t) 0x00302088UL)
#define REG_SPI_WIDTH        ((uint32_t) 0x00302188UL) /* listed with false offset in programmers guide V1.1 */
#define REG_SWIZZLE          ((uint32_t) 0x00302064UL)
#define REG_TAG              ((uint32_t) 0x0030207cUL)
#define REG_TAG_X            ((uint32_t) 0x00302074UL)
#define REG_TAG_Y            ((uint32_t) 0x00302078UL)
#define REG_TAP_CRC          ((uint32_t) 0x00302024UL) /* only listed in datasheet */
#define REG_TAP_MASK         ((uint32_t) 0x00302028UL) /* only listed in datasheet */
#define REG_TOUCH_ADC_MODE   ((uint32_t) 0x00302108UL)
#define REG_TOUCH_CHARGE     ((uint32_t) 0x0030210cUL)
#define REG_TOUCH_DIRECT_XY  ((uint32_t) 0x0030218cUL)
#define REG_TOUCH_DIRECT_Z1Z2 ((uint32_t) 0x00302190UL)
#define REG_TOUCH_MODE       ((uint32_t) 0x00302104UL)
#define REG_TOUCH_OVERSAMPLE ((uint32_t) 0x00302114UL)
#define REG_TOUCH_RAW_XY     ((uint32_t) 0x0030211cUL)
#define REG_TOUCH_RZ         ((uint32_t) 0x00302120UL)
#define REG_TOUCH_RZTHRESH   ((uint32_t) 0x00302118UL)
#define REG_TOUCH_SCREEN_XY  ((uint32_t) 0x00302124UL)
#define REG_TOUCH_SETTLE     ((uint32_t) 0x00302110UL)
#define REG_TOUCH_TAG        ((uint32_t) 0x0030212cUL)
#define REG_TOUCH_TAG1       ((uint32_t) 0x00302134UL) /* only listed in datasheet */
#define REG_TOUCH_TAG1_XY    ((uint32_t) 0x00302130UL) /* only listed in datasheet */
#define REG_TOUCH_TAG2       ((uint32_t) 0x0030213cUL) /* only listed in datasheet */
#define REG_TOUCH_TAG2_XY    ((uint32_t) 0x00302138UL) /* only listed in datasheet */
#define REG_TOUCH_TAG3       ((uint32_t) 0x00302144UL) /* only listed in datasheet */
#define REG_TOUCH_TAG3_XY    ((uint32_t) 0x00302140UL) /* only listed in datasheet */
#define REG_TOUCH_TAG4       ((uint32_t) 0x0030214cUL)/* only listed in datasheet */
#define REG_TOUCH_TAG4_XY    ((uint32_t) 0x00302148UL) /* only listed in datasheet */
#define REG_TOUCH_TAG_XY     ((uint32_t) 0x00302128UL)
#define REG_TOUCH_TRANSFORM_A ((uint32_t) 0x00302150UL)
#define REG_TOUCH_TRANSFORM_B ((uint32_t) 0x00302154UL)
#define REG_TOUCH_TRANSFORM_C ((uint32_t) 0x00302158UL)
#define REG_TOUCH_TRANSFORM_D ((uint32_t) 0x0030215cUL)
#define REG_TOUCH_TRANSFORM_E ((uint32_t) 0x00302160UL)
#define REG_TOUCH_TRANSFORM_F ((uint32_t) 0x00302164UL)
#define REG_TRACKER          ((uint32_t) 0x00309000UL) /* only listed in programmers guide */
#define REG_TRACKER_1        ((uint32_t) 0x00309004UL) /* only listed in programmers guide */
#define REG_TRACKER_2        ((uint32_t) 0x00309008UL) /* only listed in programmers guide */
#define REG_TRACKER_3        ((uint32_t) 0x0030900cUL) /* only listed in programmers guide */
#define REG_TRACKER_4        ((uint32_t) 0x00309010UL) /* only listed in programmers guide */
#define REG_TRIM             ((uint32_t) 0x00302180UL)
#define REG_VCYCLE           ((uint32_t) 0x00302040UL)
#define REG_VOFFSET          ((uint32_t) 0x00302044UL)
#define REG_VOL_PB           ((uint32_t) 0x00302080UL)
#define REG_VOL_SOUND        ((uint32_t) 0x00302084UL)
#define REG_VSIZE            ((uint32_t) 0x00302048UL)
#define REG_VSYNC0           ((uint32_t) 0x0030204cUL)
#define REG_VSYNC1           ((uint32_t) 0x00302050UL)


/* Macros for static display list generation */

//#define ALPHA_FUNC(func,ref) ((DL_ALPHA_FUNC) | (((func) & 7UL) << 8U) | ((ref) & 0xFFUL))
/**
 * @brief Set the alpha test function.
 *
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t ALPHA_FUNC(uint8_t func, uint8_t ref)
{
    uint32_t const funcv = ((uint32_t) func & 7U) << 8U;
    return (DL_ALPHA_FUNC | funcv | ref);
}

//#define BITMAP_HANDLE(handle) ((DL_BITMAP_HANDLE) | ((handle) & 0x1FUL))
/**
 * @brief Set the bitmap handle.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_HANDLE(uint8_t handle)
{
    return (DL_BITMAP_HANDLE | ((handle) & 0x1FUL));
}

//#define BITMAP_LAYOUT(format,linestride,height) ((DL_BITMAP_LAYOUT) | (((format) & 0x1FUL) << 19U) | (((linestride) & 0x3FFUL) << 9U) | ((height) & 0x1FFUL))
/**
 * @brief Set the source bitmap memory format and layout for the current handle.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_LAYOUT(uint8_t format, uint16_t linestride, uint16_t height)
{
    uint32_t const formatv = ((uint32_t) format & 0x1FUL) << 19U;
    uint32_t const linestridev = ((uint32_t) linestride & 0x3FFUL) << 9U;
    uint32_t const heightv = height & 0x1FFUL;
    return (DL_BITMAP_LAYOUT | formatv | linestridev | heightv);
}

//#define BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((DL_BITMAP_SIZE) | (((filter) & 1UL) << 20U) | (((wrapx) & 1UL) << 19U) | (((wrapy) & 1UL) << 18U) | (((width) & 0x1FFUL) << 9U) | ((height) & 0x1FFUL))
/**
 * @brief Set the source bitmap memory format and layout for the current handle.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_SIZE(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height)
{
    uint32_t const filterv = (filter & 0x1UL) << 20U;
    uint32_t const wrapxv = (wrapx & 0x1UL) << 19U;
    uint32_t const wrapyv = (wrapy & 0x1UL) << 18U;
    uint32_t const widthv = (width & 0x1FFUL) << 9U;
    uint32_t const heightv = height & 0x1FFUL;
    return (DL_BITMAP_SIZE | filterv | wrapxv | wrapyv | widthv | heightv);
}

//#define BITMAP_LAYOUT_H(linestride,height) ((DL_BITMAP_LAYOUT_H) | (((((linestride) & 0xC00U) >> 10U)&3UL) << 2U) | ((((height) & 0x600U) >> 9U) & 3UL))
/**
 * @brief Set the 2 most significant bits of the source bitmap memory format and layout for the current handle.
 * @param linestride 12-bit value specified to BITMAP_LAYOUT
 * @param height 11-bit value specified to BITMAP_LAYOUT
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_LAYOUT_H(uint16_t linestride, uint16_t height)
{
    uint32_t const linestridev = (uint32_t) ((((linestride & 0xC00U) >> 10U) &3UL) << 2U);
    uint32_t const heightv = (uint32_t) (((height & 0x600U) >> 9U) & 3UL);
    return (DL_BITMAP_LAYOUT_H | linestridev | heightv);
}

//#define BITMAP_SIZE_H(width,height) ((DL_BITMAP_SIZE_H) | (((((width) & 0x600U) >> 9U) & 3UL) << 2U) | ((((height) & 0x600U) >> 9U) & 3UL))
/**
 * @brief Set the 2 most significant bits of bitmaps dimension for the current handle.
 * @param linestride 11-bit value of bitmap width, the 2 most significant bits are used
 * @param height 11-bit value of bitmap width, the 2 most significant bits are used
 * @note this is different to FTDIs implementation as this takes the original values as parameters and not only the upper bits
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_SIZE_H(uint16_t width, uint16_t height)
{
    uint32_t const widthv = (uint32_t) ((((width & 0x600U) >> 9U) & 3UL) << 2U);
    uint32_t const heightv = (uint32_t) (((height & 0x600U) >> 9U) & 3UL);
    return ((DL_BITMAP_SIZE_H) | widthv | heightv);
}

//#define BITMAP_SOURCE(addr) ((DL_BITMAP_SOURCE) | ((addr) & 0x3FFFFFUL))
/**
 * @brief Set the source address of bitmap data in RAM_G or flash memory.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_SOURCE(uint32_t addr)
{
    return (DL_BITMAP_SOURCE | (addr & 0x3FFFFFUL));
}

#if EVE_GEN < 3 /* only define these for FT81x */
//#define BITMAP_TRANSFORM_A(a) ((DL_BITMAP_TRANSFORM_A) | ((a) & 0x1FFFFUL))
/**
 * @brief Set the A coefficient of the bitmap transform matrix.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_TRANSFORM_A(uint32_t val)
{
    return (DL_BITMAP_TRANSFORM_A | (val & 0x1FFFFUL));
}

//#define BITMAP_TRANSFORM_B(b) ((DL_BITMAP_TRANSFORM_B) | ((b) & 0x1FFFFUL))
/**
 * @brief Set the B coefficient of the bitmap transform matrix.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_TRANSFORM_B(uint32_t val)
{
    return (DL_BITMAP_TRANSFORM_B | (val & 0x1FFFFUL));
}

//#define BITMAP_TRANSFORM_D(d) ((DL_BITMAP_TRANSFORM_D) | ((d) & 0x1FFFFUL))
/**
 * @brief Set the D coefficient of the bitmap transform matrix.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_TRANSFORM_D(uint32_t val)
{
    return (DL_BITMAP_TRANSFORM_D | (val & 0x1FFFFUL));
}

//#define BITMAP_TRANSFORM_E(e) ((DL_BITMAP_TRANSFORM_E) | ((e) & 0x1FFFFUL))
/**
 * @brief Set he E coefficient of the bitmap transform matrix.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_TRANSFORM_E(uint32_t val)
{
    return (DL_BITMAP_TRANSFORM_E | (val & 0x1FFFFUL));
}

#endif

//#define BITMAP_TRANSFORM_C(c) ((DL_BITMAP_TRANSFORM_C) | ((c) & 0x1FFFFUL))
/**
 * @brief Set the C coefficient of the bitmap transform matrix.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_TRANSFORM_C(uint32_t val)
{
    return (DL_BITMAP_TRANSFORM_C | (val & 0x1FFFFUL));
}

//#define BITMAP_TRANSFORM_F(f) ((DL_BITMAP_TRANSFORM_F) | ((f) & 0x1FFFFUL))
/**
 * @brief Set the F coefficient of the bitmap transform matrix.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_TRANSFORM_F(uint32_t val)
{
    return (DL_BITMAP_TRANSFORM_F | (val & 0x1FFFFUL));
}

//#define BLEND_FUNC(src,dst) ((DL_BLEND_FUNC) | (((src) & 7UL) << 3U) | ((dst) & 7UL))
/**
 * @brief Execute a sequence of commands at another location in the display list.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BLEND_FUNC(uint8_t src, uint8_t dst)
{
    uint32_t const srcv = (uint32_t) ((src & 7UL) << 3U);
    uint32_t const dstv = (uint32_t) (dst & 7UL);
    return (DL_BLEND_FUNC | srcv | dstv);
}

//#define CALL(dest) ((DL_CALL) | ((dest) & 0xFFFFUL))
/**
 * @brief Execute a sequence of commands at another location in the display list.
 * @note valid range for dest is from zero to 2047
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t CALL(uint16_t dest)
{
    return (DL_CALL | (dest & 0x7FFUL));
}

//#define JUMP(dest) ((DL_JUMP) | ((dest) & 0xFFFFUL))
/**
 * @brief Execute commands at another location in the display list.
 * @note valid range for dest is from zero to 2047
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t JUMP(uint16_t dest)
{
    return (DL_JUMP | (dest & 0x7FFUL));
}

//#define CELL(cell) ((DL_CELL) | ((cell) & 0x7FUL))
/**
 * @brief Set the bitmap cell number for the VERTEX2F command.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t CELL(uint8_t cell)
{
    return (DL_CELL | (cell & 0x7FUL));
}

//#define CLEAR(c,s,t) ((DL_CLEAR) | (((c) & 1UL) << 2U) | (((s) & 1UL) << 1U) | ((t) & 1UL))
/**
 * @brief Clear buffers to preset values.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t CLEAR(uint8_t color, uint8_t stencil, uint8_t tag)
{
    uint32_t const colorv = (color & 1UL) << 2U;
    uint32_t const stencilv = (stencil & 1UL) << 1U;
    uint32_t const tagv = (tag & 1UL);
    return (DL_CLEAR | colorv | stencilv | tagv);
}

//#define CLEAR_COLOR_A(alpha) ((DL_CLEAR_COLOR_A) | ((alpha) & 0xFFUL))
/**
 * @brief Set clear value for the alpha channel.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t CLEAR_COLOR_A(uint8_t alpha)
{
    return (DL_CLEAR_COLOR_A | alpha);
}

//#define CLEAR_COLOR_RGB(red,green,blue) ((DL_CLEAR_COLOR_RGB) | (((red) & 0xFFUL) << 16U) | (((green) & 0xFFUL) << 8U) | ((blue) & 0xFFUL))
/**
 * @brief Set clear values for red, green and blue channels.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t CLEAR_COLOR_RGB(uint8_t red, uint8_t green, uint8_t blue)
{
    uint32_t const redv = ((red & 0xFFUL) << 16U);
    uint32_t const greenv = ((green & 0xFFUL) << 8U);
    uint32_t const bluev = (blue & 0xFFUL);
    return (DL_CLEAR_COLOR_RGB | redv | greenv | bluev);
}

//#define CLEAR_STENCIL(s) ((DL_CLEAR_STENCIL) | ((s) & 0xFFUL))
/**
 * @brief Set clear value for the stencil buffer.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t CLEAR_STENCIL(uint8_t val)
{
    return (DL_CLEAR_STENCIL | val);
}

//#define CLEAR_TAG(s) ((DL_CLEAR_TAG) | ((s) & 0xFFUL))
/**
 * @brief Set clear value for the tag buffer.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t CLEAR_TAG(uint8_t val)
{
    return (DL_CLEAR_TAG | val);
}

//#define COLOR_A(alpha) ((DL_COLOR_A) | ((alpha) & 0xFFUL))
/**
 * @brief Set the current color alpha.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t COLOR_A(uint8_t alpha)
{
    return (DL_COLOR_A | alpha);
}

//#define COLOR_MASK(r,g,b,a) ((DL_COLOR_MASK) | (((r) & 1UL) << 3U) | (((g) & 1UL) << 2U) | (((b) & 1UL) << 1U) | ((a) & 1UL))
/**
 * @brief Enable or disable writing of color components.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t COLOR_MASK(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    uint32_t const redv = ((red & 1UL) << 3U);
    uint32_t const greenv = ((green & 1UL) << 2U);
    uint32_t const bluev = ((blue & 1UL) << 1U);
    uint32_t const alphav = (alpha & 1UL);
    return (DL_COLOR_MASK | redv | greenv | bluev | alphav);
}

//#define COLOR_RGB(red,green,blue) ((DL_COLOR_RGB) | (((red) & 0xFFUL) << 16U) | (((green) & 0xFFUL) << 8U) | ((blue) & 0xFFUL))
/**
 * @brief Set the current color red, green and blue.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t COLOR_RGB(uint8_t red, uint8_t green, uint8_t blue)
{
    uint32_t const redv = ((red & 0xFFUL) << 16U);
    uint32_t const greenv = ((green & 0xFFUL) << 8U);
    uint32_t const bluev = (blue & 0xFFUL);
    return (DL_COLOR_RGB | redv | greenv | bluev);
}

//#define LINE_WIDTH(width) ((DL_LINE_WIDTH) | (((uint32_t) (width)) & 0xFFFUL))
/**
 * @brief Set the width of lines to be drawn with primitive LINES in 1/16 pixel precision.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t LINE_WIDTH(uint16_t width)
{
    return (DL_LINE_WIDTH | (width & 0xFFFUL));
}

//#define MACRO(m) ((DL_MACRO) | ((m) & 1UL))
/**
 * @brief Execute a single command from a macro register.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t MACRO(uint8_t macro)
{
    return (DL_MACRO | (macro & 0x1UL));
}

//#define PALETTE_SOURCE(addr) ((DL_PALETTE_SOURCE) | ((addr) & 0x3FFFFF3UL))
/**
 * @brief Set the base address of the palette.
 * @note 2-byte alignment is required if pixel format is PALETTE4444 or PALETTE565.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t PALETTE_SOURCE(uint32_t addr)
{
    return (DL_PALETTE_SOURCE | (addr & 0x3FFFFFUL));
}

//#define POINT_SIZE(size) ((DL_POINT_SIZE) | ((size) & 0x1FFFUL))
/**
 * @brief Set the radius of points to be drawn with primitive POINTS in 1/16 pixel precision.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t POINT_SIZE(uint16_t size)
{
    return (DL_POINT_SIZE | (size & 0x1FFFUL));
}

//#define SCISSOR_SIZE(width,height) ((DL_SCISSOR_SIZE) | (((width) & 0xFFFUL) << 12U) | ((height) & 0xFFFUL))
/**
 * @brief Set the size of the scissor clip rectangle.
 * @note valid range for width and height is from zero to 2048
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t SCISSOR_SIZE(uint16_t width, uint16_t height)
{
    uint32_t const widthv = (uint32_t) ((width & 0xFFFUL) << 12U);
    uint32_t const heightv = (uint32_t) (height & 0xFFFUL);
    return (DL_SCISSOR_SIZE | widthv | heightv);
}

//#define SCISSOR_XY(x,y) ((DL_SCISSOR_XY) | (((x) & 0x7FFUL) << 11U) | ((y) & 0x7FFUL))
/**
 * @brief Set the top left corner of the scissor clip rectangle.
 * @note valid range for width and height is from zero to 2047
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t SCISSOR_XY(uint16_t xc0, uint16_t yc0)
{
    uint32_t const xc0v = (uint32_t) ((xc0 & 0x7FFUL) << 11U);
    uint32_t const yc0v = (uint32_t) (yc0 & 0x7FFUL);
    return (DL_SCISSOR_XY | xc0v | yc0v);
}

//#define STENCIL_FUNC(func,ref,mask) ((DL_STENCIL_FUNC) | (((func) & 7UL) << 16U) | (((ref) & 0xFFUL) << 8U)|((mask) & 0xFFUL))
/**
 * @brief Set function and reference value for stencil testing.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t STENCIL_FUNC(uint8_t func, uint8_t ref, uint8_t mask)
{
    uint32_t const funcv = (uint32_t) ((func & 7UL) << 16U);
    uint32_t const refv = (uint32_t) ((ref & 0xFFUL) << 8U);
    uint32_t const maskv = (uint32_t) (mask & 0xFFUL);
    return (DL_STENCIL_FUNC | funcv | refv | maskv);
}

//#define STENCIL_MASK(mask) ((DL_STENCIL_MASK) | ((mask) & 0xFFUL))
/**
 * @brief Control the writing of individual bits in the stencil planes.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t STENCIL_MASK(uint8_t mask)
{
    return (DL_STENCIL_MASK | mask);
}

//#define STENCIL_OP(sfail,spass) ((DL_STENCIL_OP) | (((sfail) & 7UL) << 3U) | ((spass) & 7UL))
/**
 * @brief Set stencil test actions.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t STENCIL_OP(uint8_t sfail, uint8_t spass)
{
    uint32_t const sfailv = (uint32_t) ((sfail & 0x07UL) << 3U);
    uint32_t const spassv = (uint32_t) (spass & 0x07UL);
    return (DL_STENCIL_OP | sfailv | spassv);
}

//#define TAG(s) ((DL_TAG) | ((s) & 0xFFUL))
/**
 * @brief Attach the tag value for the following graphics objects drawn on the screen.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t TAG(uint8_t tagval)
{
    return (DL_TAG | tagval);
}

//#define TAG_MASK(mask) ((DL_TAG_MASK) | ((mask) & 1UL))
/**
 * @brief Control the writing of the tag buffer.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t TAG_MASK(uint8_t mask)
{
    return (DL_TAG_MASK | ((mask) & 1UL));
}

//#define VERTEX2F(x,y) ((DL_VERTEX2F) | ((((uint32_t) (x)) & 0x7FFFUL) << 15U) | (((uint32_t) (y)) & 0x7FFFUL))
/**
 * @brief Set coordinates for graphics primitves.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t VERTEX2F(int16_t xc0, int16_t yc0)
{
    uint32_t const xc0v = ((((uint32_t) ((uint16_t) xc0)) & 0x7FFFUL) << 15U);
    uint32_t const yc0v = (((uint32_t) ((uint16_t) yc0)) & 0x7FFFUL);
    return (DL_VERTEX2F | xc0v | yc0v);
}

//#define VERTEX2II(x,y,handle,cell) ((DL_VERTEX2II) | (((x) & 0x1FFUL) << 21U) | (((y) & 0x1FFUL) << 12U) | (((handle) & 0x1FUL) << 7U) | ((cell) & 0x7FUL))
/**
 * @brief Set coordinates, bitmap-handle and cell-number for graphics primitves.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t VERTEX2II(uint16_t xc0, uint16_t yc0, uint8_t handle, uint8_t cell)
{
    uint32_t const xc0v = ((((uint32_t) xc0) & 0x1FFUL) << 21U);
    uint32_t const yc0v = ((((uint32_t) yc0) & 0x1FFUL) << 12U);
    uint32_t const handlev = ((((uint32_t) handle) & 0x1FUL) << 7U);
    uint32_t const cellv = (((uint32_t) cell) & 0x7FUL);
    return (DL_VERTEX2II | xc0v | yc0v | handlev | cellv);
}

//#define VERTEX_FORMAT(frac) ((DL_VERTEX_FORMAT) | ((frac) & 7UL))
/**
 * @brief Set the precision of VERTEX2F coordinates.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t VERTEX_FORMAT(uint8_t frac)
{
    return (DL_VERTEX_FORMAT | ((frac) & 7UL));
}

//#define VERTEX_TRANSLATE_X(x) ((DL_VERTEX_TRANSLATE_X) | ((x) & 0x1FFFFUL))
/**
 * @brief Set the vertex transformations X translation component.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t VERTEX_TRANSLATE_X(int32_t xco)
{
    return (DL_VERTEX_TRANSLATE_X | (((uint32_t) xco) & 0x1FFFFUL));
}

//#define VERTEX_TRANSLATE_Y(y) ((DL_VERTEX_TRANSLATE_Y) | ((y) & 0x1FFFFUL))
/**
 * @brief Set the vertex transformations Y translation component.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t VERTEX_TRANSLATE_Y(int32_t yco)
{
    return (DL_VERTEX_TRANSLATE_Y | (((uint32_t) yco) & 0x1FFFFUL));
}

/* #define BEGIN(prim) ((DL_BEGIN) | ((prim) & 15UL)) */ /* use define DL_BEGIN */
/* #define DISPLAY() ((DL_DISPLAY)) */ /* use define DL_DISPLAY */
/* #define END() ((DL_END)) */ /* use define DL_END */
/* #define RESTORE_CONTEXT() ((DL_RESTORE_CONTEXT)) */ /* use define DL_RESTORE_CONTEXT */
/* #define RETURN() ((DL_RETURN)) */ /* use define DL_RETURN */
/* #define SAVE_CONTEXT() ((DL_SAVE_CONTEXT)) */ /* use define DL_SAVE_CONTEXT */
/* #define NOP() ((DL_NOP)) */

/* ########## EVE Generation 3: BT815 / BT816 definitions ########## */

#if EVE_GEN > 2

#define EVE_GLFORMAT  ((uint32_t) 31UL) /* used with BITMAP_LAYOUT to indicate bitmap-format is specified by BITMAP_EXT_FORMAT */

#define DL_BITMAP_EXT_FORMAT ((uint32_t) 0x2E000000UL) /* requires OR'd arguments */
#define DL_BITMAP_SWIZZLE    ((uint32_t) 0x2F000000UL)
/* #define DL_INT_FRR           ((uint32_t) 0x30000000UL) */ /* ESE displays "Internal: flash read result" - undocumented display list command */

/* Extended Bitmap formats */
#define EVE_ASTC_4X4   ((uint32_t) 37808UL)
#define EVE_ASTC_5X4   ((uint32_t) 37809UL)
#define EVE_ASTC_5X5   ((uint32_t) 37810UL)
#define EVE_ASTC_6X5   ((uint32_t) 37811UL)
#define EVE_ASTC_6X6   ((uint32_t) 37812UL)
#define EVE_ASTC_8X5   ((uint32_t) 37813UL)
#define EVE_ASTC_8X6   ((uint32_t) 37814UL)
#define EVE_ASTC_8X8   ((uint32_t) 37815UL)
#define EVE_ASTC_10X5  ((uint32_t) 37816UL)
#define EVE_ASTC_10X6  ((uint32_t) 37817UL)
#define EVE_ASTC_10X8  ((uint32_t) 37818UL)
#define EVE_ASTC_10X10 ((uint32_t) 37819UL)
#define EVE_ASTC_12X10 ((uint32_t) 37820UL)
#define EVE_ASTC_12X12 ((uint32_t) 37821UL)

#define EVE_RAM_ERR_REPORT      ((uint32_t) 0x309800UL) /* max 128 bytes null terminated string */
#define EVE_RAM_FLASH           ((uint32_t) 0x800000UL)
#define EVE_RAM_FLASH_POSTBLOB  ((uint32_t) 0x801000UL)

#define EVE_OPT_FLASH  ((uint16_t) 64U)
#define EVE_OPT_OVERLAY ((uint16_t) 128U)
#define EVE_OPT_FORMAT ((uint16_t) 4096U)
#define EVE_OPT_FILL   ((uint16_t) 8192U)

/* Commands for BT815 / BT816 */
#define CMD_BITMAP_TRANSFORM ((uint32_t) 0xFFFFFF21UL)
#define CMD_SYNC             ((uint32_t) 0xFFFFFF42UL) /* does not need a dedicated function, just use EVE_cmd_dl(CMD_SYNC) */
#define CMD_FLASHERASE       ((uint32_t) 0xFFFFFF44UL) /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHERASE) */
#define CMD_FLASHWRITE       ((uint32_t) 0xFFFFFF45UL)
#define CMD_FLASHREAD        ((uint32_t) 0xFFFFFF46UL)
#define CMD_FLASHUPDATE      ((uint32_t) 0xFFFFFF47UL)
#define CMD_FLASHDETACH      ((uint32_t) 0xFFFFFF48UL) /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHDETACH) */
#define CMD_FLASHATTACH      ((uint32_t) 0xFFFFFF49UL) /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHATTACH) */
#define CMD_FLASHFAST        ((uint32_t) 0xFFFFFF4AUL)
#define CMD_FLASHSPIDESEL    ((uint32_t) 0xFFFFFF4BUL) /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHSPIDESEL) */
#define CMD_FLASHSPITX       ((uint32_t) 0xFFFFFF4CUL)
#define CMD_FLASHSPIRX       ((uint32_t) 0xFFFFFF4DUL)
#define CMD_FLASHSOURCE      ((uint32_t) 0xFFFFFF4EUL)
#define CMD_CLEARCACHE       ((uint32_t) 0xFFFFFF4FUL) /* does not need a dedicated function, just use EVE_cmd_dl(CMD_CLEARCACHE) */
#define CMD_INFLATE2         ((uint32_t) 0xFFFFFF50UL)
#define CMD_ROTATEAROUND     ((uint32_t) 0xFFFFFF51UL)
#define CMD_RESETFONTS       ((uint32_t) 0xFFFFFF52UL) /* does not need a dedicated function, just use EVE_cmd_dl(CMD_RESETFONTS) */
#define CMD_ANIMSTART        ((uint32_t) 0xFFFFFF53UL)
#define CMD_ANIMSTOP         ((uint32_t) 0xFFFFFF54UL)
#define CMD_ANIMXY           ((uint32_t) 0xFFFFFF55UL)
#define CMD_ANIMDRAW         ((uint32_t) 0xFFFFFF56UL)
#define CMD_GRADIENTA        ((uint32_t) 0xFFFFFF57UL)
#define CMD_FILLWIDTH        ((uint32_t) 0xFFFFFF58UL)
#define CMD_APPENDF          ((uint32_t) 0xFFFFFF59UL)
#define CMD_ANIMFRAME        ((uint32_t) 0xFFFFFF5AUL)
#define CMD_VIDEOSTARTF      ((uint32_t) 0xFFFFFF5FUL) /* does not need a dedicated function, just use EVE_cmd_dl(CMD_VIDEOSTARTF) */

/* Registers for BT815 / BT816 */
#define REG_ADAPTIVE_FRAMERATE ((uint32_t) 0x0030257cUL)
#define REG_PLAYBACK_PAUSE     ((uint32_t) 0x003025ecUL)
#define REG_FLASH_STATUS       ((uint32_t) 0x003025f0UL)
#define REG_FLASH_SIZE         ((uint32_t) 0x00309024UL)
#define REG_PLAY_CONTROL       ((uint32_t) 0x0030914eUL)
#define REG_COPRO_PATCH_PTR    ((uint32_t) 0x00309162UL)

/* Macros for BT815 / BT816 */

//#define BITMAP_EXT_FORMAT(format) ((DL_BITMAP_EXT_FORMAT) | ((format) & 0xFFFFUL))
/**
 * @brief Set the extended format of the bitmap.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_EXT_FORMAT(uint16_t format)
{
    return (DL_BITMAP_EXT_FORMAT | format);
}

//#define BITMAP_SWIZZLE(r,g,b,a) ((DL_BITMAP_SWIZZLE) | (((r) & 7UL) << 9U) | (((g) & 7UL) << 6U) | (((b) & 7UL) << 3U) | ((a) & 7UL))
/**
 * @brief Set the source for the red, green, blue and alpha channels of a bitmap.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_SWIZZLE(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    uint32_t const redv = ((red & 7UL) << 9U);
    uint32_t const greenv = ((green & 7UL) << 6U);
    uint32_t const bluev = ((blue & 7UL) << 3U);
    uint32_t const alphav = (alpha & 7UL);
    return (DL_BITMAP_SWIZZLE | redv | greenv | bluev | alphav);
}

//#define BITMAP_TRANSFORM_A_EXT(p,v) ((DL_BITMAP_TRANSFORM_A) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))
/**
 * @brief Set the A coefficient of the bitmap transform matrix.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_TRANSFORM_A(uint8_t prc, uint32_t val)
{
    uint32_t const prcv = ((prc & 1UL) << 17U);
    uint32_t const valv = (val & 0x1FFFFUL);
    return (DL_BITMAP_TRANSFORM_A | prcv | valv);
}

//#define BITMAP_TRANSFORM_B_EXT(p,v) ((DL_BITMAP_TRANSFORM_B) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))
/**
 * @brief Set the B coefficient of the bitmap transform matrix.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_TRANSFORM_B(uint8_t prc, uint32_t val)
{
    uint32_t const prcv = ((prc & 1UL) << 17U);
    uint32_t const valv = (val & 0x1FFFFUL);
    return (DL_BITMAP_TRANSFORM_B | prcv | valv);
}

//#define BITMAP_TRANSFORM_D_EXT(p,v) ((DL_BITMAP_TRANSFORM_D) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))
/**
 * @brief Set the D coefficient of the bitmap transform matrix.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_TRANSFORM_D(uint8_t prc, uint32_t val)
{
    uint32_t const prcv = ((prc & 1UL) << 17U);
    uint32_t const valv = (val & 0x1FFFFUL);
    return (DL_BITMAP_TRANSFORM_D | prcv | valv);
}

//#define BITMAP_TRANSFORM_E_EXT(p,v) ((DL_BITMAP_TRANSFORM_E) | (((p) & 1UL) << 17U) | ((v) & 0x1FFFFUL))
/**
 * @brief Set the E coefficient of the bitmap transform matrix.
 * @return a 32 bit word for use with EVE_cmd_dl()
 */
static inline uint32_t BITMAP_TRANSFORM_E(uint8_t prc, uint32_t val)
{
    uint32_t const prcv = ((prc & 1UL) << 17U);
    uint32_t const valv = (val & 0x1FFFFUL);
    return (DL_BITMAP_TRANSFORM_E | prcv | valv);
}

//#define BITMAP_TRANSFORM_A(a) BITMAP_TRANSFORM_A_EXT(0UL,(a))
//#define BITMAP_TRANSFORM_B(b) BITMAP_TRANSFORM_B_EXT(0UL,(b))
//#define BITMAP_TRANSFORM_D(d) BITMAP_TRANSFORM_D_EXT(0UL,(d))
//#define BITMAP_TRANSFORM_E(e) BITMAP_TRANSFORM_E_EXT(0UL,(e))

#endif  /* EVE_GEN > 2 */

/* ########## EVE Generation 4: BT817 / BT818 definitions ########## */

#if EVE_GEN > 3

/* Commands for BT817 / BT818 */
#define CMD_ANIMFRAMERAM   ((uint32_t) 0xFFFFFF6DUL)
#define CMD_ANIMSTARTRAM   ((uint32_t) 0xFFFFFF6EUL)
#define CMD_APILEVEL       ((uint32_t) 0xFFFFFF63UL)
#define CMD_CALIBRATESUB   ((uint32_t) 0xFFFFFF60UL)
#define CMD_CALLLIST       ((uint32_t) 0xFFFFFF67UL)
#define CMD_ENDLIST        ((uint32_t) 0xFFFFFF69UL) /* does not need a dedicated function, just use EVE_cmd_dl(CMD_ENDLIST) */
#define CMD_FLASHPROGRAM   ((uint32_t) 0xFFFFFF70UL)
#define CMD_FONTCACHE      ((uint32_t) 0xFFFFFF6BUL)
#define CMD_FONTCACHEQUERY ((uint32_t) 0xFFFFFF6CUL)
#define CMD_GETIMAGE       ((uint32_t) 0xFFFFFF64UL)
#define CMD_HSF            ((uint32_t) 0xFFFFFF62UL)
#define CMD_LINETIME       ((uint32_t) 0xFFFFFF5EUL)
#define CMD_NEWLIST        ((uint32_t) 0xFFFFFF68UL)
#define CMD_PCLKFREQ       ((uint32_t) 0xFFFFFF6AUL)
#define CMD_RETURN         ((uint32_t) 0xFFFFFF66UL) /* does not need a dedicated function, just use EVE_cmd_dl(CMD_RETURN) */
#define CMD_RUNANIM        ((uint32_t) 0xFFFFFF6FUL)
#define CMD_TESTCARD       ((uint32_t) 0xFFFFFF61UL) /* does not need a dedicated function, just use EVE_cmd_dl(CMD_TESTCARD) */
#define CMD_WAIT           ((uint32_t) 0xFFFFFF65UL)

/* Registers for BT817 / BT818 */
#define REG_UNDERRUN      ((uint32_t) 0x0030260cUL)
#define REG_AH_HCYCLE_MAX ((uint32_t) 0x00302610UL)
#define REG_PCLK_FREQ     ((uint32_t) 0x00302614UL)
#define REG_PCLK_2X       ((uint32_t) 0x00302618UL)
#define REG_ANIM_ACTIVE   ((uint32_t) 0x0030902CUL)

#endif /*  EVE_GEN > 3 */

#ifdef __cplusplus
}
#endif

#endif /* EVE_H */
