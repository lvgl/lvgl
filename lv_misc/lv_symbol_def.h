#ifndef LV_SYMBOL_DEF_H
#define LV_SYMBOL_DEF_H

#ifdef __cplusplus
extern "C" {
#endif
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

/*
 * With no UTF-8 support (192- 255) (192..241 is used)
 *
 * With UTF-8 support (in Supplemental Private Use Area-A): 0xF800 .. 0xF831
 * - Basic symbols:     0xE000..0xE01F
 * - File symbols:      0xE020..0xE03F
 * - Feedback symbols:  0xE040..0xE05F
 * - Reserved:          0xE060..0xE07F
 */

#if LV_TXT_UTF8 == 0
#define LV_SYMBOL_GLYPH_FIRST  0xC0
#define SYMBOL_AUDIO           "\xC0"
#define SYMBOL_VIDEO           "\xC1"
#define SYMBOL_LIST            "\xC2"
#define SYMBOL_OK              "\xC3"
#define SYMBOL_CLOSE           "\xC4"
#define SYMBOL_POWER           "\xC5"
#define SYMBOL_SETTINGS        "\xC6"
#define SYMBOL_TRASH           "\xC7"
#define SYMBOL_HOME            "\xC8"
#define SYMBOL_DOWNLOAD        "\xC9"
#define SYMBOL_DRIVE           "\xCA"
#define SYMBOL_REFRESH         "\xCB"
#define SYMBOL_MUTE            "\xCC"
#define SYMBOL_VOLUME_MID      "\xCD"
#define SYMBOL_VOLUME_MAX      "\xCE"
#define SYMBOL_IMAGE           "\xCF"
#define SYMBOL_EDIT            "\xD0"
#define SYMBOL_PREV            "\xD1"
#define SYMBOL_PLAY            "\xD2"
#define SYMBOL_PAUSE           "\xD3"
#define SYMBOL_STOP            "\xD4"
#define SYMBOL_NEXT            "\xD5"
#define SYMBOL_EJECT           "\xD6"
#define SYMBOL_LEFT            "\xD7"
#define SYMBOL_RIGHT           "\xD8"
#define SYMBOL_PLUS            "\xD9"
#define SYMBOL_MINUS           "\xDA"
#define SYMBOL_WARNING         "\xDB"
#define SYMBOL_SHUFFLE         "\xDC"
#define SYMBOL_UP              "\xDD"
#define SYMBOL_DOWN            "\xDE"
#define SYMBOL_LOOP            "\xDF"
#define SYMBOL_DIRECTORY       "\xE0"
#define SYMBOL_UPLOAD          "\xE1"
#define SYMBOL_CALL            "\xE2"
#define SYMBOL_CUT             "\xE3"
#define SYMBOL_COPY            "\xE4"
#define SYMBOL_SAVE            "\xE5"
#define SYMBOL_CHARGE          "\xE6"
#define SYMBOL_BELL            "\xE7"
#define SYMBOL_KEYBOARD        "\xE8"
#define SYMBOL_GPS             "\xE9"
#define SYMBOL_FILE            "\xEA"
#define SYMBOL_WIFI            "\xEB"
#define SYMBOL_BATTERY_FULL    "\xEC"
#define SYMBOL_BATTERY_3       "\xED"
#define SYMBOL_BATTERY_2       "\xEE"
#define SYMBOL_BATTERY_1       "\xEF"
#define SYMBOL_BATTERY_EMPTY   "\xF0"
#define SYMBOL_BLUETOOTH       "\xF1"
#define LV_SYMBOL_GLYPH_LAST   0xF1
#define SYMBOL_DUMMY           "\xFF"       /*Invalid symbol. If written before a string then `lv_img` will show it as a label*/

#else
#define LV_SYMBOL_GLYPH_FIRST  0xF800
#define SYMBOL_AUDIO           "\xEF\xA0\x80"
#define SYMBOL_VIDEO           "\xEF\xA0\x81"
#define SYMBOL_LIST            "\xEF\xA0\x82"
#define SYMBOL_OK              "\xEF\xA0\x83"
#define SYMBOL_CLOSE           "\xEF\xA0\x84"
#define SYMBOL_POWER           "\xEF\xA0\x85"
#define SYMBOL_SETTINGS        "\xEF\xA0\x86"
#define SYMBOL_TRASH           "\xEF\xA0\x87"
#define SYMBOL_HOME            "\xEF\xA0\x88"
#define SYMBOL_DOWNLOAD        "\xEF\xA0\x89"
#define SYMBOL_DRIVE           "\xEF\xA0\x8A"
#define SYMBOL_REFRESH         "\xEF\xA0\x8B"
#define SYMBOL_MUTE            "\xEF\xA0\x8C"
#define SYMBOL_VOLUME_MID      "\xEF\xA0\x8D"
#define SYMBOL_VOLUME_MAX      "\xEF\xA0\x8E"
#define SYMBOL_IMAGE           "\xEF\xA0\x8F"
#define SYMBOL_EDIT            "\xEF\xA0\x90"
#define SYMBOL_PREV            "\xEF\xA0\x91"
#define SYMBOL_PLAY            "\xEF\xA0\x92"
#define SYMBOL_PAUSE           "\xEF\xA0\x93"
#define SYMBOL_STOP            "\xEF\xA0\x94"
#define SYMBOL_NEXT            "\xEF\xA0\x95"
#define SYMBOL_EJECT           "\xEF\xA0\x96"
#define SYMBOL_LEFT            "\xEF\xA0\x97"
#define SYMBOL_RIGHT           "\xEF\xA0\x98"
#define SYMBOL_PLUS            "\xEF\xA0\x99"
#define SYMBOL_MINUS           "\xEF\xA0\x9A"
#define SYMBOL_WARNING         "\xEF\xA0\x9B"
#define SYMBOL_SHUFFLE         "\xEF\xA0\x9C"
#define SYMBOL_UP              "\xEF\xA0\x9D"
#define SYMBOL_DOWN            "\xEF\xA0\x9E"
#define SYMBOL_LOOP            "\xEF\xA0\x9F"
#define SYMBOL_DIRECTORY       "\xEF\xA0\xA0"
#define SYMBOL_UPLOAD          "\xEF\xA0\xA1"
#define SYMBOL_CALL            "\xEF\xA0\xA2"
#define SYMBOL_CUT             "\xEF\xA0\xA3"
#define SYMBOL_COPY            "\xEF\xA0\xA4"
#define SYMBOL_SAVE            "\xEF\xA0\xA5"
#define SYMBOL_CHARGE          "\xEF\xA0\xA6"
#define SYMBOL_BELL            "\xEF\xA0\xA7"
#define SYMBOL_KEYBOARD        "\xEF\xA0\xA8"
#define SYMBOL_GPS             "\xEF\xA0\xA9"
#define SYMBOL_FILE            "\xEF\xA0\xAA"
#define SYMBOL_WIFI            "\xEF\xA0\xAB"
#define SYMBOL_BATTERY_FULL    "\xEF\xA0\xAC"
#define SYMBOL_BATTERY_3       "\xEF\xA0\xAD"
#define SYMBOL_BATTERY_2       "\xEF\xA0\xAE"
#define SYMBOL_BATTERY_1       "\xEF\xA0\xAF"
#define SYMBOL_BATTERY_EMPTY   "\xEF\xA0\xB0"
#define SYMBOL_BLUETOOTH       "\xEF\xA0\xB1"
#define LV_SYMBOL_GLYPH_LAST   0xF831
#define SYMBOL_DUMMY           "\xEF\xA3\xBF"       /*Invalid symbol at (U+F831). If written before a string then `lv_img` will show it as a label*/
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /*LV_SYMBOL_DEF_H*/
