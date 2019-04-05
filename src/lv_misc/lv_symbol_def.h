#ifndef LV_SYMBOL_DEF_H
#define LV_SYMBOL_DEF_H
/* clang-format off */

#ifdef __cplusplus
extern "C" {
#endif
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif


#define LV_SYMBOL_GLYPH_FIRST      0xF800
#define LV_SYMBOL_AUDIO           "\xEF\xA0\x80"
#define LV_SYMBOL_VIDEO           "\xEF\xA0\x81"
#define LV_SYMBOL_LIST            "\xEF\xA0\x82"
#define LV_SYMBOL_OK              "\xEF\xA0\x83"
#define LV_SYMBOL_CLOSE           "\xEF\xA0\x84"
#define LV_SYMBOL_POWER           "\xEF\xA0\x85"
#define LV_SYMBOL_SETTINGS        "\xEF\xA0\x86"
#define LV_SYMBOL_TRASH           "\xEF\xA0\x87"
#define LV_SYMBOL_HOME            "\xEF\xA0\x88"
#define LV_SYMBOL_DOWNLOAD        "\xEF\xA0\x89"
#define LV_SYMBOL_DRIVE           "\xEF\xA0\x8A"
#define LV_SYMBOL_REFRESH         "\xEF\xA0\x8B"
#define LV_SYMBOL_MUTE            "\xEF\xA0\x8C"
#define LV_SYMBOL_VOLUME_MID      "\xEF\xA0\x8D"
#define LV_SYMBOL_VOLUME_MAX      "\xEF\xA0\x8E"
#define LV_SYMBOL_IMAGE           "\xEF\xA0\x8F"
#define LV_SYMBOL_EDIT            "\xEF\xA0\x90"
#define LV_SYMBOL_PREV            "\xEF\xA0\x91"
#define LV_SYMBOL_PLAY            "\xEF\xA0\x92"
#define LV_SYMBOL_PAUSE           "\xEF\xA0\x93"
#define LV_SYMBOL_STOP            "\xEF\xA0\x94"
#define LV_SYMBOL_NEXT            "\xEF\xA0\x95"
#define LV_SYMBOL_EJECT           "\xEF\xA0\x96"
#define LV_SYMBOL_LEFT            "\xEF\xA0\x97"
#define LV_SYMBOL_RIGHT           "\xEF\xA0\x98"
#define LV_SYMBOL_PLUS            "\xEF\xA0\x99"
#define LV_SYMBOL_MINUS           "\xEF\xA0\x9A"
#define LV_SYMBOL_WARNING         "\xEF\xA0\x9B"
#define LV_SYMBOL_SHUFFLE         "\xEF\xA0\x9C"
#define LV_SYMBOL_UP              "\xEF\xA0\x9D"
#define LV_SYMBOL_DOWN            "\xEF\xA0\x9E"
#define LV_SYMBOL_LOOP            "\xEF\xA0\x9F"
#define LV_SYMBOL_DIRECTORY       "\xEF\xA0\xA0"
#define LV_SYMBOL_UPLOAD          "\xEF\xA0\xA1"
#define LV_SYMBOL_CALL            "\xEF\xA0\xA2"
#define LV_SYMBOL_CUT             "\xEF\xA0\xA3"
#define LV_SYMBOL_COPY            "\xEF\xA0\xA4"
#define LV_SYMBOL_SAVE            "\xEF\xA0\xA5"
#define LV_SYMBOL_CHARGE          "\xEF\xA0\xA6"
#define LV_SYMBOL_BELL            "\xEF\xA0\xA7"
#define LV_SYMBOL_KEYBOARD        "\xEF\xA0\xA8"
#define LV_SYMBOL_GPS             "\xEF\xA0\xA9"
#define LV_SYMBOL_FILE            "\xEF\xA0\xAA"
#define LV_SYMBOL_WIFI            "\xEF\xA0\xAB"
#define LV_SYMBOL_BATTERY_FULL    "\xEF\xA0\xAC"
#define LV_SYMBOL_BATTERY_3       "\xEF\xA0\xAD"
#define LV_SYMBOL_BATTERY_2       "\xEF\xA0\xAE"
#define LV_SYMBOL_BATTERY_1       "\xEF\xA0\xAF"
#define LV_SYMBOL_BATTERY_EMPTY   "\xEF\xA0\xB0"
#define LV_SYMBOL_BLUETOOTH       "\xEF\xA0\xB1"
#define LV_SYMBOL_GLYPH_LAST      0xF831

/*Invalid symbol at (U+F831). If written before a string then `lv_img` will show it as a label*/
#define LV_SYMBOL_DUMMY           "\xEF\xA3\xBF"

/*
 * following list is generated using
 * cat lv_symbol_def.h | sed -E -n 's/^#define\s+(SYMBOL_\w+).*$/    _LV_STR_\1,/p'
 */
enum {
    _LV_STR_SYMBOL_AUDIO,
    _LV_STR_SYMBOL_VIDEO,
    _LV_STR_SYMBOL_LIST,
    _LV_STR_SYMBOL_OK,
    _LV_STR_SYMBOL_CLOSE,
    _LV_STR_SYMBOL_POWER,
    _LV_STR_SYMBOL_SETTINGS,
    _LV_STR_SYMBOL_TRASH,
    _LV_STR_SYMBOL_HOME,
    _LV_STR_SYMBOL_DOWNLOAD,
    _LV_STR_SYMBOL_DRIVE,
    _LV_STR_SYMBOL_REFRESH,
    _LV_STR_SYMBOL_MUTE,
    _LV_STR_SYMBOL_VOLUME_MID,
    _LV_STR_SYMBOL_VOLUME_MAX,
    _LV_STR_SYMBOL_IMAGE,
    _LV_STR_SYMBOL_EDIT,
    _LV_STR_SYMBOL_PREV,
    _LV_STR_SYMBOL_PLAY,
    _LV_STR_SYMBOL_PAUSE,
    _LV_STR_SYMBOL_STOP,
    _LV_STR_SYMBOL_NEXT,
    _LV_STR_SYMBOL_EJECT,
    _LV_STR_SYMBOL_LEFT,
    _LV_STR_SYMBOL_RIGHT,
    _LV_STR_SYMBOL_PLUS,
    _LV_STR_SYMBOL_MINUS,
    _LV_STR_SYMBOL_WARNING,
    _LV_STR_SYMBOL_SHUFFLE,
    _LV_STR_SYMBOL_UP,
    _LV_STR_SYMBOL_DOWN,
    _LV_STR_SYMBOL_LOOP,
    _LV_STR_SYMBOL_DIRECTORY,
    _LV_STR_SYMBOL_UPLOAD,
    _LV_STR_SYMBOL_CALL,
    _LV_STR_SYMBOL_CUT,
    _LV_STR_SYMBOL_COPY,
    _LV_STR_SYMBOL_SAVE,
    _LV_STR_SYMBOL_CHARGE,
    _LV_STR_SYMBOL_BELL,
    _LV_STR_SYMBOL_KEYBOARD,
    _LV_STR_SYMBOL_GPS,
    _LV_STR_SYMBOL_FILE,
    _LV_STR_SYMBOL_WIFI,
    _LV_STR_SYMBOL_BATTERY_FULL,
    _LV_STR_SYMBOL_BATTERY_3,
    _LV_STR_SYMBOL_BATTERY_2,
    _LV_STR_SYMBOL_BATTERY_1,
    _LV_STR_SYMBOL_BATTERY_EMPTY,
    _LV_STR_SYMBOL_BLUETOOTH,
    _LV_STR_SYMBOL_DUMMY,
};

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /*LV_SYMBOL_DEF_H*/





