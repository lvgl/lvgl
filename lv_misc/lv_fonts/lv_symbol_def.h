#ifndef LV_SYMBOL_DEF_H
#define LV_SYMBOL_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../lv_conf.h"

/*
 * With no UTF-8 support (192-255)
 * - Basic symbols:         0xC0..0xCF
 * - Feedback symbols:      0xD0..0xDF
 * - File symbols:          0xE0..0xFF
 *
 * With UTF-8 support (in Supplemental Private Use Area-A)
 * - Basic symbols:     0xE000..0xE01F
 * - File symbols:      0xE020..0xE03F
 * - Feedback symbols:  0xE040..0xE05F
 * - Reserved:          0xE060..0xE07F
 */

#if LV_TXT_UTF8 == 0
#define SYMBOL_GLYPH_FIRST  0xC0
#define SYMBOL_GLYPH_LAST   0xFF

/*Basic symbols*/
#define SYMBOL_LIST      "\xC0"
#define SYMBOL_OK        "\xC1"
#define SYMBOL_CLOSE     "\xC2"
#define SYMBOL_POWER     "\xC3"
#define SYMBOL_SETTINGS  "\xC4"
#define SYMBOL_HOME      "\xC5"
#define SYMBOL_REFRESH   "\xC6"
#define SYMBOL_LEFT      "\xC7"
#define SYMBOL_RIGHT     "\xC8"
#define SYMBOL_PLUS      "\xC9"
#define SYMBOL_MINUS     "\xCA"
#define SYMBOL_UP        "\xCB"
#define SYMBOL_DOWN      "\xCC"
#define SYMBOL_KEYBOARD  "\xCD"

/*File symbols*/
#define SYMBOL_AUDIO        "\xE0"
#define SYMBOL_VIDEO        "\xE1"
#define SYMBOL_TRASH        "\xE2"
#define SYMBOL_DOWNLOAD     "\xE3"
#define SYMBOL_DRIVE        "\xE4"
#define SYMBOL_IMAGE        "\xE5"
#define SYMBOL_EDIT         "\xE6"
#define SYMBOL_PREV         "\xE7"
#define SYMBOL_PLAY         "\xE8"
#define SYMBOL_PAUSE        "\xE9"
#define SYMBOL_STOP         "\xEA"
#define SYMBOL_NEXT         "\xEB"
#define SYMBOL_EJECT        "\xEC"
#define SYMBOL_SHUFFLE      "\xED"
#define SYMBOL_LOOP         "\xEE"
#define SYMBOL_DIRECTORY    "\xEF"
#define SYMBOL_UPLOAD       "\xF0"
#define SYMBOL_CUT          "\xF1"
#define SYMBOL_COPY         "\xF2"
#define SYMBOL_SAVE         "\xF3"
#define SYMBOL_FILE         "\xF4"

/*Feedback symbols*/
#define SYMBOL_MUTE          "\xD0"
#define SYMBOL_VOLUME_MID    "\xD1"
#define SYMBOL_VOLUME_MAX    "\xD2"
#define SYMBOL_WARNING       "\xD3"
#define SYMBOL_CALL          "\xD4"
#define SYMBOL_CHARGE        "\xD5"
#define SYMBOL_BELL          "\xD6"
#define SYMBOL_GPS           "\xD7"
#define SYMBOL_WIFI          "\xD8"
#define SYMBOL_BATTERY_FULL  "\xD9"
#define SYMBOL_BATTERY_3     "\xDA"
#define SYMBOL_BATTERY_2     "\xDB"
#define SYMBOL_BATTERY_1     "\xDC"
#define SYMBOL_BATTERY_EMPTY "\xDD"
#define SYMBOL_BLUETOOTH     "\xDE"

#else

#define SYMBOL_GLYPH_FIRST  0xE000  /*Unicode*/
#define SYMBOL_GLYPH_LAST   0xE080  /*Unicode*/

/*Store the UTF8 code of the symbols*/

/*Basic symbols*/
#define SYMBOL_LIST      "\xEE\x80\x80"
#define SYMBOL_OK        "\xEE\x80\x81"
#define SYMBOL_CLOSE     "\xEE\x80\x82"
#define SYMBOL_POWER  "\xEE\x80\x83"
#define SYMBOL_SETTINGS  "\xEE\x80\x84"
#define SYMBOL_HOME      "\xEE\x80\x85"
#define SYMBOL_REFRESH   "\xEE\x80\x86"
#define SYMBOL_LEFT      "\xEE\x80\x87"
#define SYMBOL_RIGHT     "\xEE\x80\x88"
#define SYMBOL_PLUS      "\xEE\x80\x89"
#define SYMBOL_MINUS     "\xEE\x80\x8A"
#define SYMBOL_UP        "\xEE\x80\x8B"
#define SYMBOL_DOWN      "\xEE\x80\x8C"
#define SYMBOL_KEYBOARD  "\xEE\x80\x8D"

/*File symbols*/
#define SYMBOL_AUDIO        "\xEE\x80\xA0"
#define SYMBOL_VIDEO        "\xEE\x80\xA1"
#define SYMBOL_TRASH        "\xEE\x80\xA2"
#define SYMBOL_DOWNLOAD     "\xEE\x80\xA3"
#define SYMBOL_DRIVE        "\xEE\x80\xA4"
#define SYMBOL_IMAGE        "\xEE\x80\xA5"
#define SYMBOL_EDIT         "\xEE\x80\xA6"
#define SYMBOL_PREV         "\xEE\x80\xA7"
#define SYMBOL_PLAY         "\xEE\x80\xA8"
#define SYMBOL_PAUSE        "\xEE\x80\xA9"
#define SYMBOL_STOP         "\xEE\x80\xAA"
#define SYMBOL_NEXT         "\xEE\x80\xAB"
#define SYMBOL_EJECT        "\xEE\x80\xAC"
#define SYMBOL_SHUFFLE      "\xEE\x80\xAD"
#define SYMBOL_LOOP         "\xEE\x80\xAE"
#define SYMBOL_DIRECTORY    "\xEE\x80\xAF"
#define SYMBOL_UPLOAD       "\xEE\x80\xB0"
#define SYMBOL_CUT          "\xEE\x80\xB1"
#define SYMBOL_COPY         "\xEE\x80\xB2"
#define SYMBOL_SAVE         "\xEE\x80\xB3"
#define SYMBOL_FILE         "\xEE\x80\xB4"

/*Feedback symbols*/
#define SYMBOL_MUTE           "\xEE\x81\x80"
#define SYMBOL_VOLUME_MID     "\xEE\x81\x81"
#define SYMBOL_VOLUME_MAX     "\xEE\x81\x82"
#define SYMBOL_WARNING        "\xEE\x81\x83"
#define SYMBOL_CALL           "\xEE\x81\x84"
#define SYMBOL_CHARGE         "\xEE\x81\x85"
#define SYMBOL_BELL           "\xEE\x81\x86"
#define SYMBOL_GPS            "\xEE\x81\x87"
#define SYMBOL_WIFI           "\xEE\x81\x88"
#define SYMBOL_BATTERY_FULL   "\xEE\x81\x89"
#define SYMBOL_BATTERY_3      "\xEE\x81\x8A"
#define SYMBOL_BATTERY_2      "\xEE\x81\x8B"
#define SYMBOL_BATTERY_1      "\xEE\x81\x8C"
#define SYMBOL_BATTERY_EMPTY  "\xEE\x81\x8D"
#define SYMBOL_BLUETOOTH      "\xEE\x81\x8E"

#endif /*LV_TXT_UTF8*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /*LV_SYMBOL_DEF_H*/
