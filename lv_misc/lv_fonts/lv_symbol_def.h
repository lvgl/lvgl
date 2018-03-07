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
#else
#define LV_SYMBOL_GLYPH_FIRST  0xF000

#define SYMBOL_AUDIO           "\xEF\x80\x80"
#define SYMBOL_VIDEO           "\xEF\x80\x81"
#define SYMBOL_LIST            "\xEF\x80\x82"
#define SYMBOL_OK              "\xEF\x80\x83"
#define SYMBOL_CLOSE           "\xEF\x80\x84"
#define SYMBOL_POWER           "\xEF\x80\x85"
#define SYMBOL_SETTINGS        "\xEF\x80\x86"
#define SYMBOL_TRASH           "\xEF\x80\x87"
#define SYMBOL_HOME            "\xEF\x80\x88"
#define SYMBOL_DOWNLOAD        "\xEF\x80\x89"
#define SYMBOL_DRIVE           "\xEF\x80\x8A"
#define SYMBOL_REFRESH         "\xEF\x80\x8B"
#define SYMBOL_MUTE            "\xEF\x80\x8C"
#define SYMBOL_VOLUME_MID      "\xEF\x80\x8D"
#define SYMBOL_VOLUME_MAX      "\xEF\x80\x8E"
#define SYMBOL_IMAGE           "\xEF\x80\x8F"
#define SYMBOL_EDIT            "\xEF\x80\x90"
#define SYMBOL_PREV            "\xEF\x80\x91"
#define SYMBOL_PLAY            "\xEF\x80\x92"
#define SYMBOL_PAUSE           "\xEF\x80\x93"
#define SYMBOL_STOP            "\xEF\x80\x94"
#define SYMBOL_NEXT            "\xEF\x80\x95"
#define SYMBOL_EJECT           "\xEF\x80\x96"
#define SYMBOL_LEFT            "\xEF\x80\x97"
#define SYMBOL_RIGHT           "\xEF\x80\x98"
#define SYMBOL_PLUS            "\xEF\x80\x99"
#define SYMBOL_MINUS           "\xEF\x80\x9A"
#define SYMBOL_WARNING         "\xEF\x80\x9B"
#define SYMBOL_SHUFFLE         "\xEF\x80\x9C"
#define SYMBOL_UP              "\xEF\x80\x9D"
#define SYMBOL_DOWN            "\xEF\x80\x9E"
#define SYMBOL_LOOP            "\xEF\x80\x9F"
#define SYMBOL_DIRECTORY       "\xEF\x80\xA0"
#define SYMBOL_UPLOAD          "\xEF\x80\xA1"
#define SYMBOL_CALL            "\xEF\x80\xA2"
#define SYMBOL_CUT             "\xEF\x80\xA3"
#define SYMBOL_COPY            "\xEF\x80\xA4"
#define SYMBOL_SAVE            "\xEF\x80\xA5"
#define SYMBOL_CHARGE          "\xEF\x80\xA6"
#define SYMBOL_BELL            "\xEF\x80\xA7"
#define SYMBOL_KEYBOARD        "\xEF\x80\xA8"
#define SYMBOL_GPS             "\xEF\x80\xA9"
#define SYMBOL_FILE            "\xEF\x80\xAA"
#define SYMBOL_WIFI            "\xEF\x80\xAB"
#define SYMBOL_BATTERY_FULL    "\xEF\x80\xAC"
#define SYMBOL_BATTERY_3       "\xEF\x80\xAD"
#define SYMBOL_BATTERY_2       "\xEF\x80\xAE"
#define SYMBOL_BATTERY_1       "\xEF\x80\xAF"
#define SYMBOL_BATTERY_EMPTY   "\xEF\x80\xB0"
#define SYMBOL_BLUETOOTH       "\xEF\x80\xB1"
#endif






#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /*LV_SYMBOL_DEF_H*/
