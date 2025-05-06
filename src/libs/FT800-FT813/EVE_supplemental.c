#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_EVE
/*
@file    EVE_supplemental.h
@brief   supplemental functions
@version 5.0
@date    2023-12-23
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2023 Rudolph Riedel

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
- added EVE_polar_cartesian()

*/

#include "EVE_suppplemental.h"

/* define NULL if it not already is */
#ifndef NULL
#include <stdio.h>
#endif

#if defined (__AVR__)
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

/*
 * @brief widget function to draw a circle
 */
void EVE_widget_circle(int16_t xc0, int16_t yc0, uint16_t radius, uint16_t border, uint32_t bgcolor)
{
    EVE_cmd_dl_burst(DL_SAVE_CONTEXT);
    EVE_cmd_dl(DL_BEGIN | EVE_POINTS);
    EVE_cmd_dl(POINT_SIZE(radius));
    EVE_cmd_dl(VERTEX2F(xc0, yc0));
    EVE_color_rgb(bgcolor);
    EVE_cmd_dl(POINT_SIZE(radius - border));
    EVE_cmd_dl(VERTEX2F(xc0, yc0));
    EVE_cmd_dl(DL_END);
    EVE_cmd_dl_burst(DL_RESTORE_CONTEXT);
}

/*
 * @brief widget function to draw a rectangle
 */
void EVE_widget_rectangle(int16_t xc0, int16_t yc0, int16_t wid, int16_t hgt, int16_t border, uint16_t linewidth, uint32_t bgcolor)
{
    EVE_cmd_dl_burst(DL_SAVE_CONTEXT);
    EVE_cmd_dl(DL_BEGIN | EVE_RECTS);
    EVE_cmd_dl(LINE_WIDTH(linewidth));
    EVE_cmd_dl(VERTEX2F(xc0, yc0));
    EVE_cmd_dl(VERTEX2F(xc0 + wid, yc0 + hgt));
    EVE_color_rgb(bgcolor);
    EVE_cmd_dl(VERTEX2F(xc0 + border, yc0 + border));
    EVE_cmd_dl(VERTEX2F(xc0 + wid - border, yc0 + hgt - border));
    EVE_cmd_dl(DL_END);
    EVE_cmd_dl_burst(DL_RESTORE_CONTEXT);
}

static const int8_t sine_table[360] PROGMEM =
{
    0, 2, 4, 7, 9, 11, 13, 15, 18, 20, 22, 24, 26, 29, 31, 33, 35, 37, 39, 41,
    43, 46, 48, 50, 52, 54, 56, 58, 60, 62, 63, 65, 67, 69, 71, 73, 75, 76, 78,
    80, 82, 83, 85, 87, 88, 90, 91, 93, 94, 96, 97, 99, 100, 101, 103, 104, 105,
    107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 119, 120,
    121, 121, 122, 123, 123, 124, 124, 125, 125, 125, 126, 126, 126, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 126, 126, 126, 125, 125, 125,
    124, 124, 123, 123, 122, 121, 121, 120, 119, 119, 118, 117, 116, 115, 114,
    113, 112, 111, 110, 109, 108, 107, 105, 104, 103, 101, 100, 99, 97, 96, 94,
    93, 91, 90, 88, 87, 85, 83, 82, 80, 78, 76, 75, 73, 71, 69, 67, 65, 63, 62,
    60, 58, 56, 54, 52, 50, 48, 46, 43, 41, 39, 37, 35, 33, 31, 29, 26, 24, 22,
    20, 18, 15, 13, 11, 9, 7, 4, 2, 0, -2, -4, -7, -9, -11, -13, -15, -18, -20,
    -22, -24, -26, -29, -31, -33, -35, -37, -39, -41, -43, -46, -48, -50, -52,
    -54, -56, -58, -60, -62, -63, -65, -67, -69, -71, -73, -75, -76, -78, -80,
    -82, -83, -85, -87, -88, -90, -91, -93, -94, -96, -97, -99,-100,-101,-103,
    -104, -105, -107, -108, -109, -110, -111, -112, -113, -114, -115, -116,
    -117, -118, -119, -119, -120, -121, -121, -122, -123, -123, -124, -124,
    -125, -125, -125, -126, -126, -126, -127, -127, -127, -127, -127, -127,
    -127, -127, -127, -127, -127, -126, -126, -126, -125, -125, -125, -124,
    -124, -123, -123, -122, -121, -121, -120, -119, -119, -118, -117, -116,
    -115, -114, -113, -112, -111, -110, -109, -108, -107, -105, -104, -103,
    -101, -100, -99, -97, -96, -94, -93, -91, -90, -88, -87, -85, -83, -82,
    -80, -78, -76, -75, -73, -71, -69, -67, -65, -63, -62, -60, -58, -56, -54,
    -52, -50, -48, -46, -43, -41, -39, -37, -35, -33, -31, -29, -26, -24,
    -22, -20, -18, -15, -13, -11, -9, -7, -4, -2
};

/**
 * @brief Calculate coordinates from an angle and a length.
 * @param length distance from coordinate origin (0,0)
 * @param angle rotation in degrees
 * @return signed X/Y coordinates for use with VERTEX2F
 * @note - resolution for angle is 1° and rotation is clockwise
 * @note - angle should be limited to a (n*360)-1
 */
void EVE_polar_cartesian(uint16_t length, uint16_t angle, int16_t *p_xc0, int16_t *p_yc0)
{
    uint16_t anglev;
    anglev = angle % 360U;

    if (p_xc0 != NULL)
    {
        int32_t calc = (int16_t) length;
        calc = ((calc * (sine_table[anglev])) + 64) / 128;
        *p_xc0 = (int16_t) calc;
    }

    if (p_yc0 != NULL)
    {
        anglev = anglev + 270U;
        anglev = anglev % 360U;

        int32_t calc = (int16_t) length;
        calc = ((calc * (sine_table[anglev])) + 64) / 128;
        *p_yc0 = (int16_t) calc;
    }
}

#endif /*LV_USE_DRAW_EVE*/
