#ifndef LV_SYMBOL_DEF_H
#define LV_SYMBOL_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../lv_conf_internal.h"

/*-------------------------------
 * Symbols from "normal" font
 *-----------------------------*/
#define LV_SYMBOL_BULLET          "\xE2\x80\xA2" /*20042, 0x2022*/

/*-------------------------------
 * Symbols from FontAwesome font
 *-----------------------------*/

/*In the font converter use this list as range:
      61441, 61448, 61451, 61452, 61453, 61457, 61459, 61461, 61465, 61468,
      61473, 61478, 61479, 61480, 61502, 61507, 61512, 61515, 61516, 61517,
      61521, 61522, 61523, 61524, 61543, 61544, 61550, 61552, 61553, 61556,
      61559, 61560, 61561, 61563, 61587, 61589, 61636, 61637, 61639, 61641,
      61664, 61671, 61674, 61683, 61724, 61732, 61787, 61931, 62016, 62017,
      62018, 62019, 62020, 62087, 62099, 62189, 62212, 62810, 63426, 63650
*/

#define LV_SYMBOL_AUDIO           "\xEF\x80\x81" /*61441, 0xF001*/
#define LV_SYMBOL_VIDEO           "\xEF\x80\x88" /*61448, 0xF008*/
#define LV_SYMBOL_LIST            "\xEF\x80\x8B" /*61451, 0xF00B*/
#define LV_SYMBOL_OK              "\xEF\x80\x8C" /*61452, 0xF00C*/
#define LV_SYMBOL_CLOSE           "\xEF\x80\x8D" /*61453, 0xF00D*/
#define LV_SYMBOL_POWER           "\xEF\x80\x91" /*61457, 0xF011*/
#define LV_SYMBOL_SETTINGS        "\xEF\x80\x93" /*61459, 0xF013*/
#define LV_SYMBOL_HOME            "\xEF\x80\x95" /*61461, 0xF015*/
#define LV_SYMBOL_DOWNLOAD        "\xEF\x80\x99" /*61465, 0xF019*/
#define LV_SYMBOL_DRIVE           "\xEF\x80\x9C" /*61468, 0xF01C*/
#define LV_SYMBOL_REFRESH         "\xEF\x80\xA1" /*61473, 0xF021*/
#define LV_SYMBOL_MUTE            "\xEF\x80\xA6" /*61478, 0xF026*/
#define LV_SYMBOL_VOLUME_MID      "\xEF\x80\xA7" /*61479, 0xF027*/
#define LV_SYMBOL_VOLUME_MAX      "\xEF\x80\xA8" /*61480, 0xF028*/
#define LV_SYMBOL_IMAGE           "\xEF\x80\xBE" /*61502, 0xF03E*/
#define LV_SYMBOL_TINT            "\xEF\x81\x83" /*61507, 0xF043*/
#define LV_SYMBOL_PREV            "\xEF\x81\x88" /*61512, 0xF048*/
#define LV_SYMBOL_PLAY            "\xEF\x81\x8B" /*61515, 0xF04B*/
#define LV_SYMBOL_PAUSE           "\xEF\x81\x8C" /*61516, 0xF04C*/
#define LV_SYMBOL_STOP            "\xEF\x81\x8D" /*61517, 0xF04D*/
#define LV_SYMBOL_NEXT            "\xEF\x81\x91" /*61521, 0xF051*/
#define LV_SYMBOL_EJECT           "\xEF\x81\x92" /*61522, 0xF052*/
#define LV_SYMBOL_LEFT            "\xEF\x81\x93" /*61523, 0xF053*/
#define LV_SYMBOL_RIGHT           "\xEF\x81\x94" /*61524, 0xF054*/
#define LV_SYMBOL_PLUS            "\xEF\x81\xA7" /*61543, 0xF067*/
#define LV_SYMBOL_MINUS           "\xEF\x81\xA8" /*61544, 0xF068*/
#define LV_SYMBOL_EYE_OPEN        "\xEF\x81\xAE" /*61550, 0xF06E*/
#define LV_SYMBOL_EYE_CLOSE       "\xEF\x81\xB0" /*61552, 0xF070*/
#define LV_SYMBOL_WARNING         "\xEF\x81\xB1" /*61553, 0xF071*/
#define LV_SYMBOL_SHUFFLE         "\xEF\x81\xB4" /*61556, 0xF074*/
#define LV_SYMBOL_UP              "\xEF\x81\xB7" /*61559, 0xF077*/
#define LV_SYMBOL_DOWN            "\xEF\x81\xB8" /*61560, 0xF078*/
#define LV_SYMBOL_LOOP            "\xEF\x81\xB9" /*61561, 0xF079*/
#define LV_SYMBOL_DIRECTORY       "\xEF\x81\xBB" /*61563, 0xF07B*/
#define LV_SYMBOL_UPLOAD          "\xEF\x82\x93" /*61587, 0xF093*/
#define LV_SYMBOL_CALL            "\xEF\x82\x95" /*61589, 0xF095*/
#define LV_SYMBOL_CUT             "\xEF\x83\x84" /*61636, 0xF0C4*/
#define LV_SYMBOL_COPY            "\xEF\x83\x85" /*61637, 0xF0C5*/
#define LV_SYMBOL_SAVE            "\xEF\x83\x87" /*61639, 0xF0C7*/
#define LV_SYMBOL_BARS            "\xEF\x83\x89" /*61641, 0xF0C9*/
#define LV_SYMBOL_ENVELOPE        "\xEF\x83\xA0" /*61664, 0xF0E0*/
#define LV_SYMBOL_CHARGE          "\xEF\x83\xA7" /*61671, 0xF0E7*/
#define LV_SYMBOL_PASTE           "\xEF\x83\xAA" /*61674, 0xF0EA*/
#define LV_SYMBOL_BELL            "\xEF\x83\xB3" /*61683, 0xF0F3*/
#define LV_SYMBOL_KEYBOARD        "\xEF\x84\x9C" /*61724, 0xF11C*/
#define LV_SYMBOL_GPS             "\xEF\x84\xA4" /*61732, 0xF124*/
#define LV_SYMBOL_FILE            "\xEF\x85\x9B" /*61787, 0xF158*/
#define LV_SYMBOL_WIFI            "\xEF\x87\xAB" /*61931, 0xF1EB*/
#define LV_SYMBOL_BATTERY_FULL    "\xEF\x89\x80" /*62016, 0xF240*/
#define LV_SYMBOL_BATTERY_3       "\xEF\x89\x81" /*62017, 0xF241*/
#define LV_SYMBOL_BATTERY_2       "\xEF\x89\x82" /*62018, 0xF242*/
#define LV_SYMBOL_BATTERY_1       "\xEF\x89\x83" /*62019, 0xF243*/
#define LV_SYMBOL_BATTERY_EMPTY   "\xEF\x89\x84" /*62020, 0xF244*/
#define LV_SYMBOL_USB             "\xEF\x8a\x87" /*62087, 0xF287*/
#define LV_SYMBOL_BLUETOOTH       "\xEF\x8a\x93" /*62099, 0xF293*/
#define LV_SYMBOL_TRASH           "\xEF\x8B\xAD" /*62189, 0xF2ED*/
#define LV_SYMBOL_EDIT            "\xEF\x8C\x84" /*62212, 0xF304*/
#define LV_SYMBOL_BACKSPACE       "\xEF\x95\x9A" /*62810, 0xF55A*/
#define LV_SYMBOL_SD_CARD         "\xEF\x9F\x82" /*63426, 0xF7C2*/
#define LV_SYMBOL_NEW_LINE        "\xEF\xA2\xA2" /*63650, 0xF8A2*/

/** Invalid symbol at (U+F8FF). If written before a string then `lv_img` will show it as a label*/
#define LV_SYMBOL_DUMMY           "\xEF\xA3\xBF"

/*
 * The following list is generated using
 * cat src/font/lv_symbol_def.h | sed -E -n 's/^#define\s+LV_(SYMBOL_\w+).*".*$/    _LV_STR_\1,/p'
 */
enum {
    _LV_STR_SYMBOL_BULLET,
    _LV_STR_SYMBOL_AUDIO,
    _LV_STR_SYMBOL_VIDEO,
    _LV_STR_SYMBOL_LIST,
    _LV_STR_SYMBOL_OK,
    _LV_STR_SYMBOL_CLOSE,
    _LV_STR_SYMBOL_POWER,
    _LV_STR_SYMBOL_SETTINGS,
    _LV_STR_SYMBOL_HOME,
    _LV_STR_SYMBOL_DOWNLOAD,
    _LV_STR_SYMBOL_DRIVE,
    _LV_STR_SYMBOL_REFRESH,
    _LV_STR_SYMBOL_MUTE,
    _LV_STR_SYMBOL_VOLUME_MID,
    _LV_STR_SYMBOL_VOLUME_MAX,
    _LV_STR_SYMBOL_IMAGE,
    _LV_STR_SYMBOL_TINT,
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
    _LV_STR_SYMBOL_EYE_OPEN,
    _LV_STR_SYMBOL_EYE_CLOSE,
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
    _LV_STR_SYMBOL_BARS,
    _LV_STR_SYMBOL_ENVELOPE,
    _LV_STR_SYMBOL_CHARGE,
    _LV_STR_SYMBOL_PASTE,
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
    _LV_STR_SYMBOL_USB,
    _LV_STR_SYMBOL_BLUETOOTH,
    _LV_STR_SYMBOL_TRASH,
    _LV_STR_SYMBOL_EDIT,
    _LV_STR_SYMBOL_BACKSPACE,
    _LV_STR_SYMBOL_SD_CARD,
    _LV_STR_SYMBOL_NEW_LINE,
    _LV_STR_SYMBOL_DUMMY,
};

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SYMBOL_DEF_H*/
