/*
@file    EVE_config.h
@brief   configuration information for some TFTs
@version 5.0
@date    2024-01-24
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
- re-added the EVE_CSPREAD parameters to all profiles
- added a profile for the Gameduino3 shield
- cleanup: removed FT80x profiles
- replaced FT81x_enable and BT81x_enable with "EVE_GEN n"
- added a couple of test profiles for BT817
- moved the profile for the RiTFT50 over to the one for the RiTFT70
- added a profile for the VM816C50A-D from Bridgetek
- added a profile for PH800480T024-IFC03 and PH800480T013-IFC05 from PowerTip
- added profiles for EVE4_40G and EVE4_101G from Matrix Orbital
- added profiles for the EVE4 series modules from Riverdi
- tuned the profile for the RVT101H a little
- sorted the display-defines by chip
- added a profile for the EVE4x-70G IPS from Matrix Orbital
- added a profile for RiTFT35 from Riverdi
- changed the clock-polarity for EVE_EVE4_70G
- added a profile for CFAF800480Ex-050SC-A2 modules from Crystalfontz
- removed the define for the selected configuration to allow setting the define in the build-environment instead
- added an error message if no valid define was setup and therefore no set of parameters is configured
- converted all TABs to SPACEs
- removed EVE_TOUCH_RZTHRESH as it only applies to resistive touch screens and as EVE_init() still writes it if the
define exists - it can be configured thru project options
- added EVE_Display_Parameters_t to be used with an additional init function, still not sure how to procede exactly
- split the settings for EVE_RiTFT70 and EVE_RiTFT50 after a report for the EVE_RiTFT70 not working properly and
confirmation that the provided alternative parameters do work, the EVE_RiTFT50 however are confirmed to be working with
the IOT5
- commented out EVE_Display_Parameters_t for now
- removed the 4.0 history
- added EVE_TYPE_BAR, EVE_HSIZE_VISIBLE and EVE_VSIZE_VISIBLE defines to the bar-type display configurations
- changed the timings for EVE3x-39A and EVE3x-39G to what Matrix Orbital is using in their library
- changed the timings for EVE2_38A and EVE2_38G to what Matrix Orbital is using in their library
- added profiles for EVE_EVE3x_38A and EVE_EVE3x_38G
- fixed some MISRA-C issues
- basic maintenance: checked for violations of white space and indent rules
- added an EVE_CFAF800480E1_050SC_A2 profile without EVE_HAS_GT911 as there is a newer version of this
  model around that mimics a FT5316 while still using a GT911.
- added a profile for the CFAF1024600B0-070SC-A1 from Crystalfontz
- added a profile for the Gameduino GD3X 7"
- slightly adjusted the Sunflower config and removed the "untested" tag
- added profiles for new displays from Panasys
- switched from using CMD_PCLKFREQ to writing to REG_PCLK_FREQ directly
- added define EVE_SET_REG_PCLK_2X to set REG_PCLK_2X to 1 when necessary
- split the EVE_NHD_43_800480 in a separate config to add a new optional parameter: EVE_BACKLIGHT_FREQ
- added a configuration for Crystalfonts CFA800480E3-050Sx
- added a configuration for Crystalfonts CFA240400E1-030Tx
- added a configuration for Crystalfonts CFA240320Ex-024Sx
- added EVE_BACKLIGHT_FREQ to all Riverdi modules with a value of 4kHz as recommended by Riverdi
- EVE_PCLK_FREQ was incorrectly using "UL"
- added EVE_BACKLIGHT_FREQ to all Newhaven modules with a value of 800Hz

*/

#ifndef EVE_CONFIG_H
#define EVE_CONFIG_H

/* define one of these in your build-environment to select the settings for the TFT attached */
#if 0

/* BT817 / BT818 */
#define EVE_RVT35H
#define EVE_RVT43H
#define EVE_RVT50H
#define EVE_RVT70H
#define EVE_RVT101H
#define EVE_EVE4_40G
#define EVE_EVE4_70G
#define EVE_EVE4_101G
#define EVE_CFAF800480Ex_050SC_A2
#define EVE_CFAF800480E1_050SC_A2
#define EVE_CFAF1024600B0_070SC_A1
#define EVE_CFA800480E3_050SX
#define EVE_PS817_043WQ_C_IPS

/* BT815 / BT816 */
#define EVE_VM816C50AD
#define EVE_PAF90
#define EVE_RiTFT35
#define EVE_RiTFT43
#define EVE_RiTFT50
#define EVE_RiTFT70
#define EVE_GD3X
#define EVE_EVE3_29
#define EVE_EVE3_35
#define EVE_EVE3_35G
#define EVE_EVE3_43
#define EVE_EVE3_43G
#define EVE_EVE3_50
#define EVE_EVE3_50G
#define EVE_EVE3_70
#define EVE_EVE3_70G
#define EVE_EVE3x_38
#define EVE_EVE3x_38G
#define EVE_EVE3x_39
#define EVE_EVE3x_39G
#define EVE_PS816_043WQ_R_IPS
#define EVE_PS815_043W_C_IPS
#define EVE_PS815_050W_C_IPS
#define EVE_PS815_070W_C_IPS
#define EVE_PS815_090W_C_IPS

/* FT812 / F813 */
#define EVE_ME812A
#define EVE_ME813A
#define EVE_ET07
#define EVE_RVT50
#define EVE_RVT70
#define EVE_EVE2_29
#define EVE_EVE2_35
#define EVE_EVE2_35G
#define EVE_EVE2_38
#define EVE_EVE2_38G
#define EVE_EVE2_43
#define EVE_EVE2_43G
#define EVE_EVE2_50
#define EVE_EVE2_50G
#define EVE_EVE2_70
#define EVE_EVE2_70G
#define EVE_NHD_35
#define EVE_NHD_43
#define EVE_NHD_43_800480
#define EVE_NHD_50
#define EVE_NHD_70
#define EVE_ADAM101
#define EVE_CFAF240400C1_030SC
#define EVE_CFAF320240F_035T
#define EVE_CFAF480128A0_039TC
#define EVE_CFAF800480E0_050SC
#define EVE_GEN4_FT81X_43
#define EVE_GEN4_FT812_50
#define EVE_GEN4_FT812_70
#define EVE_GEN4_FT813_50
#define EVE_GEN4_FT813_70
#define EVE_SUNFLOWER
#define EVE_PH800480

/* F810 / F811 */
#define EVE_VM810C
#define EVE_FT810CB_HY50HD
#define EVE_FT811CB_HY50HD
#define EVE_CFA240400E1_030TX
#define EVE_CFA240320EX_024SX
#define EVE_GAMEDUINO3

#endif

#if 0
typedef struct
{
    uint16_t hsize; /* valid range: 12 bits / 0-4095, Thd, length of the visible part of a line (in PCLKs) - active display width */
    uint16_t vsize; /* valid range: 12 bits / 0-4095, Tvd, number of visible lines (in lines) - active display height */
    uint16_t hsync0; /* valid range: 12 bits / 0-4095, Thf, Horizontal Front Porch */
    uint16_t hsync1;  /* valid range: 12 bits / 0-4095, Tvf + Tvp, Vertical Front Porch plus Vsync Pulse width */
    uint16_t hoffset; /* valid range: 12 bits / 0-4095, Thf + Thp + Thb, length of non-visible part of line (in PCLK cycles) */
    uint16_t hcycle; /* valid range: 12 bits / 0-4095, Th, total length of line (visible and non-visible) (in PCLKs) */
    uint16_t vsync0; /* valid range: 12 bits / 0-4095, Tvf, Vertical Front Porch */
    uint16_t vsync1; /* valid range: 12 bits / 0-4095, Tvf + Tvp, Vertical Front Porch plus Vsync Pulse width */
    uint16_t voffset;  /* valid range: 12 bits / 0-4095, Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
    uint16_t vcycle; /* valid range: 12 bits / 0-4095, Tv, total number of lines (visible and non-visible) (in lines) */
    uint8_t swizzle; /* 4 bits, controls the arrangement of the output colour pins */
    uint8_t pclkpol; /* 1 bit, 0 = rising edge, 1 = falling edge */
    uint8_t cspread; /* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
    uint8_t pclk; /* pixel-clock divider, 0 = no PCLK output, 1 = use second PLL for pixel-clock in BT817 / BT818 */
    uint32_t pclk_freq; /* frequency in Hz for BT817 / BT818 to be used with EVE_cmd_pclkfreq() in order to write REG_PCLK_FREQ */
    uint8_t pwm_duty; /* valid range: 0-128, backlight PWM level, 0 = off, 128 = max */
    bool has_crystal;
    bool has_gt911;
} EVE_Display_Parameters_t;
#endif

/* display timing parameters below */

/* ########## 320 x 240 ########## */

/* EVE2-35A 320x240 3.5" Matrix Orbital, resistive, or non-touch, FT812 */
#if defined (EVE_EVE2_35)
#define Resolution_320x240

#define EVE_PCLK (9L)
#define EVE_PCLKPOL (0L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 2
#endif

/* EVE2-35G 320x240 3.5" Matrix Orbital, capacitive touch, FT813 */
#if defined (EVE_EVE2_35G)
#define Resolution_320x240

#define EVE_PCLK (9L)
#define EVE_PCLKPOL (0L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 2
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* untested */
/* EVE3-35A 320x240 3.5" Matrix Orbital, resistive, or non-touch, BT816 */
#if defined (EVE_EVE3_35)
#define Resolution_320x240

#define EVE_PCLK (11L)
#define EVE_PCLKPOL (0L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* EVE3-35G 320x240 3.5" Matrix Orbital, capacitive-touch, BT815 */
#if defined (EVE_EVE3_35G)
#define Resolution_320x240

#define EVE_PCLK (11L)
#define EVE_PCLKPOL (0L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* Crystalfonts CFAF320240F-035T 320x240 3.5" , FT810 resistive touch */
#if defined (EVE_CFAF320240F_035T)
#define EVE_HSIZE (320L)
#define EVE_VSIZE (240L)

#define EVE_VSYNC0 (1L)
#define EVE_VSYNC1 (4L)
#define EVE_VOFFSET (4L)
#define EVE_VCYCLE (245L)
#define EVE_HSYNC0 (10L)
#define EVE_HSYNC1 (20L)
#define EVE_HOFFSET (40L)
#define EVE_HCYCLE (510L)
#define EVE_PCLK (8L)
#define EVE_PCLKPOL (0L)
#define EVE_SWIZZLE (2L)
#define EVE_CSPREAD (0L)
#define EVE_GEN 2
#endif

/* Sunflower Arduino Shield, 320x240 3.5" from Cowfish, FT813 */
// https://github.com/Cowfish-Studios/Cowfish_Sunflower_Shield_PCB
/* note: CS is on pin D6 and PD is on pin D5 */
#if defined (EVE_SUNFLOWER)
#define Resolution_320x240

#define EVE_PCLK (9L)
#define EVE_PCLKPOL (0L)
#define EVE_SWIZZLE (2L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 2
#endif

/* NHD-3.5-320240FT-CxXx-xxx 320x240 3.5" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_35)
#define Resolution_320x240

#define EVE_PCLK (9L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (2L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2

/* there are at least two series of these, the older one is using a backlight controller that */
/* works up to 1kHz and the newer one is using a backlight controller that works from 800Hz to 100kHz */
// https://github.com/RudolphRiedel/FT800-FT813/discussions/90#discussioncomment-6201237
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (800U) /* if not overwritten in the project options, set 800Hz as a compromise */
#endif
#endif

/* untested */
/* 320x240 3.5" Riverdi, various options, BT815/BT816 */
#if defined (EVE_RiTFT35)
#define EVE_HSIZE (320L)
#define EVE_VSIZE (240L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (2L)
#define EVE_VOFFSET (13L)
#define EVE_VCYCLE (263L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (10L)
#define EVE_HOFFSET (70L)
#define EVE_HCYCLE (408L)
#define EVE_PCLK (11L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (2L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (4000U) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested */
/* RVT35HHBxxxxx 320x240 3.5" Riverdi, various options, BT817 */
#if defined (EVE_RVT35H)
#define EVE_HSIZE (320L)
#define EVE_VSIZE (240L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (4L)
#define EVE_VOFFSET (12L)
#define EVE_VCYCLE (260L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (4L)
#define EVE_HOFFSET (43L)
#define EVE_HCYCLE (371L)
#define EVE_PCLK (12L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (4000U) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* ########## 480 x 272 ########## */

/* untested */
/* EVE2-43A 480x272 4.3" Matrix Orbital, resistive or no touch, FT812 */
#if defined (EVE_EVE2_43)
#define Resolution_480x272

#define EVE_PCLK (6L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 2
#endif

/* EVE2-43G 480x272 4.3" Matrix Orbital, capacitive touch, FT813 */
#if defined (EVE_EVE2_43G)
#define Resolution_480x272

#define EVE_PCLK (6L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 2
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* untested */
/* EVE3-43A 480x272 4.3" Matrix Orbital, resistive, or non-touch, BT816 */
#if defined (EVE_EVE3_43)
#define Resolution_480x272

#define EVE_PCLK (7L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* EVE3-43G 480x272 4.3" Matrix Orbital, capacitive-touch, BT815 */
#if defined (EVE_EVE3_43G)
#define Resolution_480x272

#define EVE_PCLK (7L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#define EVE_HAS_GT911
#endif

/* 480x272 4.3" Riverdi, various options, BT815/BT816 */
#if defined (EVE_RiTFT43)
#define Resolution_480x272

#define EVE_PCLK (7L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (4000U) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested */
/* RVT43HLBxxxxx 480x272 4.3" Riverdi, various options, BT817 */
#if defined (EVE_RVT43H)
#define EVE_HSIZE (480L)
#define EVE_VSIZE (272L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (4L)
#define EVE_VOFFSET (12L)
#define EVE_VCYCLE (292L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (4L)
#define EVE_HOFFSET (43L)
#define EVE_HCYCLE (531L)
#define EVE_PCLK (7L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (4000U) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested */
/* PS817-043WQ-C-IPS 480x272 4.3" Panasys, BT817 */
#if defined (EVE_PS817_043WQ_C_IPS)
#define EVE_HSIZE (480L)
#define EVE_VSIZE (272L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (4L)
#define EVE_VOFFSET (12L)
#define EVE_VCYCLE (292L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (4L)
#define EVE_HOFFSET (43L)
#define EVE_HCYCLE (531L)
#define EVE_PCLK (7L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#endif

/* untested */
/* PS816-043WQ-R-IPS 480x272 4.3" Panasys, resistive touch, BT816 */
#if defined (EVE_PS816_043WQ_R_IPS)
#define EVE_HSIZE (480L)
#define EVE_VSIZE (272L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (4L)
#define EVE_VOFFSET (12L)
#define EVE_VCYCLE (292L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (4L)
#define EVE_HOFFSET (43L)
#define EVE_HCYCLE (531L)
#define EVE_PCLK (7L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* untested */
/* NHD-4.3-480272FT-CxXx-xxx 480x272 4.3" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_43)
#define Resolution_480x272

#define EVE_PCLK (6L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2

/* there are at least two series of these, the older one is using a backlight controller that */
/* works up to 1kHz and the newer one is using a backlight controller that works from 800Hz to 100kHz */
// https://github.com/RudolphRiedel/FT800-FT813/discussions/90#discussioncomment-6201237
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (800U) /* if not overwritten in the project options, set 800Hz as a compromise */
#endif
#endif

/* untested */
/* 4D-Systems GEN4-FT81x-43xx 480x272 4.3",resistive or capacitive, FT812 / FT813 */
#if defined (EVE_GEN4_FT81X_43)
#define Resolution_480x272

#define EVE_PCLK (6L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2
#endif

/* untested */
/* Gameduino 3, 480x272 4.3",resistive touch, FT810 */
#if defined (EVE_GAMEDUINO3)
#define Resolution_480x272

#define EVE_PCLK (6L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (3L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 2
#endif

/* ########## 800 x 480 ########## */

/* untested */
/* NHD-4.3-800480FT-CSXP-CTP 800x480 4.3" Newhaven, capacitive touch, FT813 */
#if defined (EVE_NHD_43_800480)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2

/* there are at least two series of these, the older one is using a backlight controller that */
/* works up to 1kHz and the newer one is using a backlight controller that works from 800Hz to 100kHz */
// https://github.com/RudolphRiedel/FT800-FT813/discussions/90#discussioncomment-6201237
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (800U) /* if not overwritten in the project options, set 800Hz as a compromise */
#endif
#endif

/* untested */
/* FTDI/BRT EVE2 modules VM810C50A-D, ME812A-WH50R and ME813A-WH50C, 800x480 5.0" */
/* 4D-Systems GEN4 FT812/FT813 5.0/7.0 */
#if defined (EVE_VM810C) || defined (EVE_ME812A) || defined (EVE_ME813A) || defined (EVE_GEN4_FT812_50) || \
    defined (EVE_GEN4_FT813_50) || defined (EVE_GEN4_FT812_70) || defined (EVE_GEN4_FT813_70)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2
#endif

/* untested */
/* PH800480T024-IFC03 800x480 7.0" FT813 from PowerTip */
/* PH800480T013-IFC05 800x480 7.0" FT813 from PowerTip */
#if defined (EVE_PH800480)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2
#endif

/* untested */
/* NHD-7.0-800480FT-CxXx-xxx 800x480 7.0" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_70)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2

/* there are at least two series of these, the older one is using a backlight controller that */
/* works up to 1kHz and the newer one is using a backlight controller that works from 800Hz to 100kHz */
// https://github.com/RudolphRiedel/FT800-FT813/discussions/90#discussioncomment-6201237
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (800U) /* if not overwritten in the project options, set 800Hz as a compromise */
#endif
#endif

/* untested */
/* Matrix Orbital EVE2 modules EVE2-50A, EVE2-70A : 800x480 5.0" and 7.0" resistive, or no touch, FT812 */
#if defined (EVE_EVE2_50) || defined (EVE_EVE2_70)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_GEN 2
#endif

/* Matrix Orbital EVE2 modules EVE2-50G, EVE2-70G : 800x480 5.0" and 7.0" capacitive touch, FT813 */
/* Crystalfonts CFAF800480E0-050SC 800x480 5.0" , FT813 capacitive touch */
#if defined (EVE_EVE2_50G) || defined (EVE_EVE2_70G) || defined (EVE_CFAF800480E0_050SC)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_GEN 2
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* untested */
/* Crystalfonts CFAF800480E1-050SC-A2 800x480 5.0" , BT817 capacitive touch */
/* Crystalfonts CFAF800480E2-050SC-A2 800x480 5.0" , BT817 capacitive touch with overhanging glass bezel */
/* this should also work with CFAF800480E2-050SN-A2 (no touch) and CFAF800480E2-050SR-A2 (resistive touch) */
#if defined (EVE_CFAF800480Ex_050SC_A2)
#define Resolution_800x480

#define EVE_PCLK_FREQ (0x0451U) /* value to be put into REG_PCLK_FREQ -> 30MHz, REG_PCLK is set to 1 */
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#endif

/* untested */
/* Crystalfonts CFAF800480E1-050SC-A2 800x480 5.0" , BT817 capacitive touch */
/* note: there is a new variant of the CFAF800480E1-050SC-A2 which mimics a FT5316 as touch controller */
/* this should also work with CFAF800480E2-050SN-A2 (no touch) and CFAF800480E2-050SR-A2 (resistive touch) */
#if defined (EVE_CFAF800480E1_050SC_A2)
#define Resolution_800x480

#define EVE_PCLK_FREQ (0x0451U) /* value to be put into REG_PCLK_FREQ -> 30MHz, REG_PCLK is set to 1 */
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#endif

/* untested */
/* Crystalfonts CFA800480E3-050Sx Family, 800x480, 5.0", IPS, sunlight readable, BT817 */
/* CFA800480E3-050SN - no touch */
/* CFA800480E3-050SR - resistive touch */
/* CFA800480E3-050SC - capacitive touch */
/* CFA800480E3-050SW - capactive with wide glass bezel */
#if defined (EVE_CFA800480E3_050SX)
#define Resolution_800x480

#define EVE_PCLK_FREQ (0x0451U) /* value to be put into REG_PCLK_FREQ -> 30MHz, REG_PCLK is set to 1 */
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#endif

/* untested */
/* Matrix Orbital EVE3 modules EVE3-50A, EVE3-70A : 800x480 5.0" and 7.0" resistive, or no touch, BT816 */
/* PAF90B5WFNWC01 800x480 9.0" Panasys, BT815 */
#if defined (EVE_EVE3_50) || defined (EVE_EVE3_70) || defined (EVE_PAF90)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* Matrix Orbital EVE3 modules EVE3-50G, EVE3-70G : 800x480 5.0" and 7.0" capacitive touch, BT815 */
#if defined (EVE_EVE3_50G) || defined (EVE_EVE3_70G)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#define EVE_HAS_GT911
#endif

/* untested */
/* Bridgtek 800x480 5.0" BT816 */
#if defined (EVE_VM816C50AD)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* RVT50xQFxxxxx 800x480 5.0" Riverdi, various options, FT812/FT813 */
/* RVT70xQFxxxxx 800x480 7.0" Riverdi, various options, FT812/FT813, tested with RVT70UQFNWC0x */
#if defined (EVE_RVT70) || defined (EVE_RVT50)
#define EVE_HSIZE (800L) /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE (480L) /* Tvd Number of visible lines (in lines) - display height */

#define EVE_VSYNC0 (0L)    /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 (10L)   /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET (23L)  /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE (525L)  /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 (0L)    /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 (10L)   /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET (46L)  /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE (1056L) /* Th Total length of line (visible and non-visible) (in PCLKs) */
#define EVE_PCLK (2L)      /* 60MHz / REG_PCLK = PCLK frequency 30 MHz */
#define EVE_PCLKPOL (1L)   /* PCLK polarity (0 = rising edge, 1 = falling edge) */
#define EVE_SWIZZLE (0L)   /* Defines the arrangement of the RGB pins of the FT800 */
#define EVE_CSPREAD (1L)
#define EVE_GEN 2
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (4000U) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested but confirmed to be working */
/* RVT50xQBxxxxx 800x480 5.0" Riverdi, various options, BT815/BT816 */
/* not working properly? try the EVE_RiTFT70 profile */
#if defined (EVE_RiTFT50)
#define EVE_HSIZE (800L)
#define EVE_VSIZE (480L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (10L)
#define EVE_VOFFSET (23L)
#define EVE_VCYCLE (525L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (10L)
#define EVE_HOFFSET (46L)
#define EVE_HCYCLE (1056L)
#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (4000U) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested but confirmed to be working */
/* RVT70xQBxxxxx 800x480 7.0" Riverdi, various options, BT815/BT816 */
/* not working properly? try the EVE_RiTFT50 profile */
#if defined (EVE_RiTFT70)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (4000U) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested */
/* PS815-043W-C-IPS 800x480 4.3" Panasys, BT815 */
#if defined (EVE_PS815_043W_C_IPS)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* untested */
/* PS815-050W-C-IPS 800x480 5.0" Panasys, BT815 */
#if defined (EVE_PS815_050W_C_IPS)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* untested */
/* PS815-070W-C-IPS 800x480 7.0" Panasys, BT815 */
#if defined (EVE_PS815_070W_C_IPS)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* untested */
/* PS815-090W-C-IPS 800x480 9.0" Panasys, BT815 */
#if defined (EVE_PS815_090W_C_IPS)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#endif

/* Gameduino GD3X 7" shield with BT816 */
/* note: CS is on pin D8 and there is no PD pin */
#if defined (EVE_GD3X)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (3L)
#define EVE_CSPREAD (0L)
#define EVE_GEN 3
#endif

/* untested */
/* RVT50HQBxxxxx 800x480 5.0" Riverdi, various options, BT817 */
#if defined (EVE_RVT50H)
#define EVE_HSIZE (800L)
#define EVE_VSIZE (480L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (4L)
#define EVE_VOFFSET (8L)
#define EVE_VCYCLE (496L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (4L)
#define EVE_HOFFSET (8L)
#define EVE_HCYCLE (816L)
#define EVE_PCLK (3L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (4000U) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested */
/* NHD-5.0-800480FT-CxXx-xxx 800x480 5.0" Newhaven, resistive or capacitive, FT81x */
#if defined (EVE_NHD_50)
#define Resolution_800x480

#define EVE_PCLK (2L)
#define EVE_PCLKPOL (0L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2

/* there are at least two series of these, the older one is using a backlight controller that */
/* works up to 1kHz and the newer one is using a backlight controller that works from 800Hz to 100kHz */
// https://github.com/RudolphRiedel/FT800-FT813/discussions/90#discussioncomment-6201237
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (800U) /* if not overwritten in the project options, set 800Hz as a compromise */
#endif
#endif

/* FT810CB-HY50HD: FT810 800x480 5.0" HAOYU */
#if defined (EVE_FT810CB_HY50HD)
#define EVE_HSIZE (800L)
#define EVE_VSIZE (480L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (2L)
#define EVE_VOFFSET (13L)
#define EVE_VCYCLE (525L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (20L)
#define EVE_HOFFSET (64L)
#define EVE_HCYCLE (952L)
#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2
#endif

/* FT811CB-HY50HD: FT811 800x480 5.0" HAOYU */
#if defined (EVE_FT811CB_HY50HD)
#define EVE_HSIZE (800L)
#define EVE_VSIZE (480L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (2L)
#define EVE_VOFFSET (13L)
#define EVE_VCYCLE (525L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (20L)
#define EVE_HOFFSET (64L)
#define EVE_HCYCLE (952L)
#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2
#endif

/* untested */
/* G-ET0700G0DM6 800x480 7.0" Glyn */
#if defined (EVE_ET07)
#define EVE_HSIZE (800L)
#define EVE_VSIZE (480L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (2L)
#define EVE_VOFFSET (35L)
#define EVE_VCYCLE (525L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (128L)
#define EVE_HOFFSET (203L)
#define EVE_HCYCLE (1056L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK (2L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 2
#endif

/* ########## 1024 x 600 ########## */

/* ADAM101-LCP-SWVGA-NEW 1024x600 10.1" Glyn, capacitive, FT813 */
#if defined (EVE_ADAM101)
#define EVE_HSIZE (1024L)
#define EVE_VSIZE (600L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (1L)
#define EVE_VOFFSET (1L)
#define EVE_VCYCLE (720L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (1L)
#define EVE_HOFFSET (1L)
#define EVE_HCYCLE (1100L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK (2L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 2
#endif

/* tested with RVT70HSBNWC00-B */
/* RVT70HSBxxxxx 1024x600 7.0" Riverdi, various options, BT817 */
#if defined (EVE_RVT70H)
#define EVE_HSIZE (1024L)
#define EVE_VSIZE (600L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (10L)
#define EVE_VOFFSET (23L)
#define EVE_VCYCLE (635L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (70L)
#define EVE_HOFFSET (160L)
#define EVE_HCYCLE (1344L)
#define EVE_PCLK_FREQ (0x0D12U) /* value to be put into REG_PCLK_FREQ -> 51MHz, REG_PCLK is set to 1 */
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (4000U) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested but confirmed to be working */
/* EVE4x-70G IPS 1024x600 7" Matrix Orbital, capacitive touch, BT817 */
#if defined (EVE_EVE4_70G)
#define EVE_HSIZE (1024L)
#define EVE_VSIZE (600L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (10L)
#define EVE_VOFFSET (23L)
#define EVE_VCYCLE (632L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (70L)
#define EVE_HOFFSET (160L)
#define EVE_HCYCLE (1344L)
#define EVE_PCLK_FREQ (0x0D12U) /* value to be put into REG_PCLK_FREQ -> 51MHz, REG_PCLK is set to 1 */
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#define EVE_HAS_GT911
#endif

/* CFAF1024600B0-070SC-A1 1024x600 7.0" Crystalfontz, BT817 */
#if defined (EVE_CFAF1024600B0_070SC_A1)
#define EVE_HSIZE   (1024L)
#define EVE_VSIZE   (600L)

#define EVE_VSYNC0  (1L)
#define EVE_VSYNC1  (2L)
#define EVE_VOFFSET (25L)
#define EVE_VCYCLE  (626L)
#define EVE_HSYNC0  (16L)
#define EVE_HSYNC1  (17L)
#define EVE_HOFFSET (177L)
#define EVE_HCYCLE  (1344L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (3L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#define EVE_PCLK_FREQ (0x0D12U) /* value to be put into REG_PCLK_FREQ -> 51MHz, REG_PCLK is set to 1 */
#endif

/* ########## 1280 x 800 ########## */

/* untested */
/* EVE4-101G 1280x800 10.1" Matrix Orbital, capacitive touch, BT817 */
#if defined (EVE_EVE4_101G)
#define EVE_HSIZE (1280L)         /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE (800L)          /* Tvd Number of visible lines (in lines) - display height */
#define EVE_VSYNC0 (11L)          /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 (12L)          /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET (22L)         /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE (823L)         /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 (78L)          /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 (80L)          /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET (158L)        /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE (1440L)        /* Th Total length of line (visible and non-visible) (in PCLKs) */
#define EVE_PCLK_FREQ (0x08C1U) /* value to be put into REG_PCLK_FREQ -> 72MHz, REG_PCLK is set to 1 */
#define EVE_SET_REG_PCLK_2X
#define EVE_PCLKPOL (0L)          /* PCLK polarity (0 = rising edge, 1 = falling edge) */
#define EVE_SWIZZLE (0L)          /* Defines the arrangement of the RGB pins */
#define EVE_CSPREAD (0L) /* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#define EVE_HAS_GT911
#endif

/* tested with RVT101HVBNWC00-B */
/* RVT101HVBxxxxx 1280x800 7.0" Riverdi, various options, BT817 */
#if defined (EVE_RVT101H)
#define EVE_HSIZE (1280L)
#define EVE_VSIZE (800L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (10L)
#define EVE_VOFFSET (23L)
#define EVE_VCYCLE (838L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (20L)
#define EVE_HOFFSET (88L)
#define EVE_HCYCLE (1440L)
#define EVE_PCLK_FREQ (0x08C1U) /* value to be put into REG_PCLK_FREQ -> 72MHz, REG_PCLK is set to 1 */
#define EVE_SET_REG_PCLK_2X
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#if !defined (EVE_BACKLIGHT_FREQ)
#define EVE_BACKLIGHT_FREQ (4000U) /* if not overwritten in the project options, set 4kHz as recommended by Riverdi */
#endif
#endif

/* untested */
#if defined (EVE_WXGA_TEST1)
#define EVE_HSIZE (1280L)        /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE (800L)         /* Tvd Number of visible lines (in lines) - display height */
#define EVE_VSYNC0 (0L)          /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 (15L)         /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET (38L)        /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE (838L)        /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 (0L)          /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 (72L)         /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET (160L)       /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE (1440L)       /* Th Total length of line (visible and non-visible) (in PCLKs) */
#define EVE_PCLK_FREQ (0x08C1U) /* value to be put into REG_PCLK_FREQ -> 72MHz, REG_PCLK is set to 1 */
#define EVE_SET_REG_PCLK_2X
#define EVE_PCLKPOL (0L)         /* PCLK polarity (0 = rising edge, 1 = falling edge) */
#define EVE_SWIZZLE (0L)         /* Defines the arrangement of the RGB pins */
#define EVE_CSPREAD (0L)         /* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#endif

/* untested */
#if defined (EVE_WXGA_TEST2)
#define EVE_HSIZE (1280L)        /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE (800L)         /* Tvd Number of visible lines (in lines) - display height */
#define EVE_VSYNC0 (0L)          /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 (15L)         /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET (38L)        /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE (838L)        /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 (0L)          /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 (72L)         /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET (160L)       /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE (1440L)       /* Th Total length of line (visible and non-visible) (in PCLKs) */
#define EVE_PCLK_FREQ (0x08C1U) /* value to be put into REG_PCLK_FREQ -> 72MHz, REG_PCLK is set to 1 */
#define EVE_SET_REG_PCLK_2X
#define EVE_PCLKPOL (1L)         /* PCLK polarity (0 = rising edge, 1 = falling edge) */
#define EVE_SWIZZLE (0L)         /* Defines the arrangement of the RGB pins */
#define EVE_CSPREAD (0L)         /* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#endif

/* ########## non-standard ########## */

/* untested */
/* Crystalfonts CFA240320Ex-024Sx 240x320 2.4" , FT811 */
/* CFA240320E0-024SN - no touch */
/* CFA240320E0-024SC - capacitve touch */
#if defined (EVE_CFA240320EX_024SX)
#define EVE_HSIZE (240L)
#define EVE_VSIZE (320L)

#define EVE_VSYNC0 (8L)
#define EVE_VSYNC1 (12L)
#define EVE_VOFFSET (16L)
#define EVE_VCYCLE (337L)
#define EVE_HSYNC0 (38L)
#define EVE_HSYNC1 (48L)
#define EVE_HOFFSET (68L)
#define EVE_HCYCLE (458L)
#define EVE_PCLK (6L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (3L)
#define EVE_CSPREAD (0L)
#define EVE_GEN 2
#endif

/* untested */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
/* EVE2-29A 320x102 2.9" 1U Matrix Orbital, non-touch, FT812 */
#if defined (EVE_EVE2_29)
#define EVE_HSIZE (320L)
#define EVE_VSIZE (102L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (2L)
#define EVE_VOFFSET (156L)
#define EVE_VCYCLE (262L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (10L)
#define EVE_HOFFSET (70L)
#define EVE_HCYCLE (408L)
#define EVE_PCLKPOL (0L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK (9L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 2
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE (320L)
#define EVE_VSIZE_VISIBLE (102L)
#endif

/* untested */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
/* EVE3-29A 320x102 2.9" 1U Matrix Orbital, non-touch, BT816 */
#if defined (EVE_EVE3_29)
#define EVE_HSIZE (320L)
#define EVE_VSIZE (102L)

#define EVE_VSYNC0 (0L)
#define EVE_VSYNC1 (2L)
#define EVE_VOFFSET (156L)
#define EVE_VCYCLE (262L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (10L)
#define EVE_HOFFSET (70L)
#define EVE_HCYCLE (408L)
#define EVE_PCLKPOL (0L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK (11L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 3
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE (320L)
#define EVE_VSIZE_VISIBLE (102L)
#endif

/* Crystalfonts CFAF240400C1-030SC 240x400 3.0" , FT811 capacitive touch */
#if defined (EVE_CFAF240400C1_030SC)
#define EVE_HSIZE (240L)
#define EVE_VSIZE (400L)

#define EVE_VSYNC0 (4L)
#define EVE_VSYNC1 (6L)
#define EVE_VOFFSET (8L)
#define EVE_VCYCLE (409L)
#define EVE_HSYNC0 (10L)
#define EVE_HSYNC1 (20L)
#define EVE_HOFFSET (40L)
#define EVE_HCYCLE (489L)
#define EVE_PCLKPOL (0L)
#define EVE_SWIZZLE (2L)
#define EVE_PCLK (5L)
#define EVE_CSPREAD (0L)
#define EVE_GEN 2
#endif

/* untested */
/* Crystalfonts CFA240400E1-030Tx 240x400 3.0" , FT811 */
/* CFA240400E1-030TN - no touch */
/* CFA240400E1-030TC - capacitve touch */
#if defined (EVE_CFA240400E1_030TX)
#define EVE_HSIZE (240L)
#define EVE_VSIZE (400L)

#define EVE_VSYNC0 (41L)
#define EVE_VSYNC1 (43L)
#define EVE_VOFFSET (45L)
#define EVE_VCYCLE (444L)
#define EVE_HSYNC0 (2L)
#define EVE_HSYNC1 (4L)
#define EVE_HOFFSET (8L)
#define EVE_HCYCLE (370L)
#define EVE_PCLKPOL (0L)
#define EVE_SWIZZLE (2L)
#define EVE_PCLK (6L)
#define EVE_CSPREAD (0L)
#define EVE_GEN 2
#endif

/* EVE2-38A 480x116 3.8" 1U Matrix Orbital, resistive touch, FT812 */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
#if defined (EVE_EVE2_38)
#define EVE_HSIZE (480L)
#define EVE_VSIZE (272L)

#define EVE_VSYNC0 (152L)
#define EVE_VSYNC1 (10L)
#define EVE_VOFFSET (12L)
#define EVE_VCYCLE (292L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (41L)
#define EVE_HOFFSET (43L)
#define EVE_HCYCLE (524L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK (6L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 2
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE (480L)
#define EVE_VSIZE_VISIBLE (116L)
#endif

/* EVE2-38G 480x116 3.8" 1U Matrix Orbital, capacitive touch, FT813 */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
#if defined (EVE_EVE2_38G)
#define EVE_HSIZE (480L)
#define EVE_VSIZE (272L)

#define EVE_VSYNC0 (152L)
#define EVE_VSYNC1 (10L)
#define EVE_VOFFSET (12L)
#define EVE_VCYCLE (292L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (41L)
#define EVE_HOFFSET (43L)
#define EVE_HCYCLE (524L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK (6L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 2
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE (480L)
#define EVE_VSIZE_VISIBLE (116L)
#endif

/* untested */
/* EVE3x-38A 480x116 3.8" 1U Matrix Orbital, resistive touch, BT816 */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
#if defined (EVE_EVE3x_38)
#define EVE_HSIZE (480L)
#define EVE_VSIZE (272L)

#define EVE_VSYNC0 (152L)
#define EVE_VSYNC1 (10L)
#define EVE_VOFFSET (12L)
#define EVE_VCYCLE (292L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (41L)
#define EVE_HOFFSET (43L)
#define EVE_HCYCLE (524L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK (7L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 3
#define EVE_HSIZE_VISIBLE (480L)
#define EVE_VSIZE_VISIBLE (116L)
#endif

/* untested */
/* EVE3-38G 480x116 3.8" 1U Matrix Orbital, capacitive touch, BT815 */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
#if defined (EVE_EVE3x_38G)
#define EVE_HSIZE (480L)
#define EVE_VSIZE (272L)

#define EVE_VSYNC0 (152L)
#define EVE_VSYNC1 (10L)
#define EVE_VOFFSET (12L)
#define EVE_VCYCLE (292L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (41L)
#define EVE_HOFFSET (43L)
#define EVE_HCYCLE (524L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK (6L)
#define EVE_CSPREAD (1L)
#define EVE_GEN 3
#define EVE_HAS_GT911 /* special treatment required for out-of-spec touch-controller */
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE (480L)
#define EVE_VSIZE_VISIBLE (116L)
#endif

/* untested */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
/* EVE3x-39A 480x128 3.9" 1U Matrix Orbital, resistive touch, BT816 */
#if defined (EVE_EVE3x_39)
#define EVE_HSIZE (480L)
#define EVE_VSIZE (272L)

#define EVE_VSYNC0 (7L)
#define EVE_VSYNC1 (8L)
#define EVE_VOFFSET (12L)
#define EVE_VCYCLE (288L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (44L)
#define EVE_HOFFSET (16L)
#define EVE_HCYCLE (524L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK (7L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE (480L)
#define EVE_VSIZE_VISIBLE (128L)
#endif

/* untested */
// timings are from here: https://github.com/MatrixOrbital/EVE2-Library/blob/master/Eve2_81x.c
/* EVE3x-39G 480x128 3.9" 1U Matrix Orbital, capacitive touch, BT815 */
#if defined (EVE_EVE3x_39G)
#define EVE_HSIZE (480L)
#define EVE_VSIZE (272L)

#define EVE_VSYNC0 (7L)
#define EVE_VSYNC1 (8L)
#define EVE_VOFFSET (12L)
#define EVE_VCYCLE (288L)
#define EVE_HSYNC0 (0L)
#define EVE_HSYNC1 (44L)
#define EVE_HOFFSET (16L)
#define EVE_HCYCLE (524L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK (7L)
#define EVE_CSPREAD (1L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 3
#define EVE_HAS_GT911
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE (480L)
#define EVE_VSIZE_VISIBLE (128L)
#endif

/* Crystalfonts CFAF480128A0-039TC 480x128 3.9" , FT811 capacitive touch */
#if defined (EVE_CFAF480128A0_039TC)
#define EVE_HSIZE (480L)
#define EVE_VSIZE (128L)

#define EVE_VSYNC0 (4L)
#define EVE_VSYNC1 (5L)
#define EVE_VOFFSET (8L)
#define EVE_VCYCLE (137L)
#define EVE_HSYNC0 (24L)
#define EVE_HSYNC1 (35L)
#define EVE_HOFFSET (41L)
#define EVE_HCYCLE (1042L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_PCLK (7L)
#define EVE_CSPREAD (0L)
#define EVE_GEN 2
#define EVE_TYPE_BAR
#define EVE_HSIZE_VISIBLE (480L)
#define EVE_VSIZE_VISIBLE (128L)
#endif

/* untested */
/* note: timing parameters from Matrix Orbital, does not use the second pll, 58,64 FPS */
/* EVE4-40G 720x720 4.0" Matrix Orbital, capacitive touch, BT817 */
#if defined (EVE_EVE4_40G)
#define EVE_HSIZE (720L)
#define EVE_VSIZE (720L)

#define EVE_VSYNC0 (16)
#define EVE_VSYNC1 (18L)
#define EVE_VOFFSET (35L)
#define EVE_VCYCLE (756L)
#define EVE_HSYNC0 (46L)
#define EVE_HSYNC1 (48L)
#define EVE_HOFFSET (91)
#define EVE_HCYCLE (812L)
#define EVE_PCLK (2L)
#define EVE_PCLKPOL (1L)
#define EVE_SWIZZLE (0L)
#define EVE_CSPREAD (0L)
#define EVE_HAS_CRYSTAL
#define EVE_GEN 4
#define EVE_HAS_GT911
#endif

/* ########## Common Timings ########## */

#if defined (Resolution_320x240)
#define EVE_HSIZE (320L) /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE (240L) /* Tvd Number of visible lines (in lines) - display height */

#define EVE_VSYNC0 (0L)   /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 (2L)   /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET (18L) /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE (262L) /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 (0L)   /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 (10L)  /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET (70L) /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE (408L) /* Th Total length of line (visible and non-visible) (in PCLKs) */
#endif

#if defined (Resolution_480x272)
#define EVE_HSIZE (480L) /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE (272L) /* Tvd Number of visible lines (in lines) - display height */

#define EVE_VSYNC0 (0L)   /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 (10L)  /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET (12L) /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE (292L) /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 (0L)   /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 (41L)  /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET (43L) /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE (548L) /* Th Total length of line (visible and non-visible) (in PCLKs) */
#endif

#if defined (Resolution_800x480)
#define EVE_HSIZE (800L) /* Thd Length of visible part of line (in PCLKs) - display width */
#define EVE_VSIZE (480L) /* Tvd Number of visible lines (in lines) - display height */

#define EVE_VSYNC0 (0L)   /* Tvf Vertical Front Porch */
#define EVE_VSYNC1 (3L)   /* Tvf + Tvp Vertical Front Porch plus Vsync Pulse width */
#define EVE_VOFFSET (32L) /* Tvf + Tvp + Tvb Number of non-visible lines (in lines) */
#define EVE_VCYCLE (525L) /* Tv Total number of lines (visible and non-visible) (in lines) */
#define EVE_HSYNC0 (0L)   /* Thf Horizontal Front Porch */
#define EVE_HSYNC1 (48L)  /* Thf + Thp Horizontal Front Porch plus Hsync Pulse width */
#define EVE_HOFFSET (88L) /* Thf + Thp + Thb Length of non-visible part of line (in PCLK cycles) */
#define EVE_HCYCLE (928L) /* Th Total length of line (visible and non-visible) (in PCLKs) */
#endif

#if !defined (EVE_HSIZE)
#error "Please add a define for the desired display to your build-environment, e.g. -DEVE_EVE3_50G"
#endif

#endif /* EVE_CONFIG_H */
