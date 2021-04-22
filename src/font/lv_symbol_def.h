#ifndef LV_SYMBOL_DEF_H
#define LV_SYMBOL_DEF_H
/*clang-format off*/

#ifdef __cplusplus
extern "C" {
#endif

#include "../lv_conf_internal.h"

/*-------------------------------
 * Symbols from FontAwesome font
 *-----------------------------*/

/*In the font converter use this list as range:
      61441, 61448, 61451, 61452, 61453, 61457, 61459, 61461, 61465, 61468,
      61473, 61478, 61479, 61480, 61502, 61512, 61515, 61516, 61517, 61521,
      61522, 61523, 61524, 61543, 61544, 61550, 61552, 61553, 61556, 61559,
      61560, 61561, 61563, 61587, 61589, 61636, 61637, 61639, 61671, 61674,
      61683, 61724, 61732, 61787, 61931, 62016, 62017, 62018, 62019, 62020,
      62087, 62099, 62212, 62189, 62810, 63426, 63650
*/

#define LV_SYMBOL_AUDIO           "\xef\x80\x81" /*61441, 0xF001*/
#define LV_SYMBOL_VIDEO           "\xef\x80\x88" /*61448, 0xF008*/
#define LV_SYMBOL_LIST            "\xef\x80\x8b" /*61451, 0xF00B*/
#define LV_SYMBOL_OK              "\xef\x80\x8c" /*61452, 0xF00C*/
#define LV_SYMBOL_CLOSE           "\xef\x80\x8d" /*61453, 0xF00D*/
#define LV_SYMBOL_POWER           "\xef\x80\x91" /*61457, 0xF011*/
#define LV_SYMBOL_SETTINGS        "\xef\x80\x93" /*61459, 0xF013*/
#define LV_SYMBOL_HOME            "\xef\x80\x95" /*61461, 0xF015*/
#define LV_SYMBOL_DOWNLOAD        "\xef\x80\x99" /*61465, 0xF019*/
#define LV_SYMBOL_DRIVE           "\xef\x80\x9c" /*61468, 0xF01C*/
#define LV_SYMBOL_REFRESH         "\xef\x80\xa1" /*61473, 0xF021*/
#define LV_SYMBOL_MUTE            "\xef\x80\xa6" /*61478, 0xF026*/
#define LV_SYMBOL_VOLUME_MID      "\xef\x80\xa7" /*61479, 0xF027*/
#define LV_SYMBOL_VOLUME_MAX      "\xef\x80\xa8" /*61480, 0xF028*/
#define LV_SYMBOL_IMAGE           "\xef\x80\xbe" /*61502, 0xF03E*/
#define LV_SYMBOL_ENVELOPE        "\xef\x83\xa0" /*61664, 0xF0E0*/
#define LV_SYMBOL_EDIT            "\xef\x8C\x84" /*62212, 0xF304*/
#define LV_SYMBOL_TINT            "\xef\x81\x83" /*61507, 0xF043*/
#define LV_SYMBOL_PREV            "\xef\x81\x88" /*61512, 0xF048*/
#define LV_SYMBOL_PLAY            "\xef\x81\x8b" /*61515, 0xF04B*/
#define LV_SYMBOL_PAUSE           "\xef\x81\x8c" /*61516, 0xF04C*/
#define LV_SYMBOL_STOP            "\xef\x81\x8d" /*61517, 0xF04D*/
#define LV_SYMBOL_NEXT            "\xef\x81\x91" /*61521, 0xF051*/
#define LV_SYMBOL_EJECT           "\xef\x81\x92" /*61522, 0xF052*/
#define LV_SYMBOL_LEFT            "\xef\x81\x93" /*61523, 0xF053*/
#define LV_SYMBOL_RIGHT           "\xef\x81\x94" /*61524, 0xF054*/
#define LV_SYMBOL_PLUS            "\xef\x81\xa7" /*61543, 0xF067*/
#define LV_SYMBOL_MINUS           "\xef\x81\xa8" /*61544, 0xF068*/
#define LV_SYMBOL_EYE_OPEN        "\xef\x81\xae" /*61550, 0xF06E*/
#define LV_SYMBOL_EYE_CLOSE       "\xef\x81\xb0" /*61552, 0xF070*/
#define LV_SYMBOL_WARNING         "\xef\x81\xb1" /*61553, 0xF071*/
#define LV_SYMBOL_SHUFFLE         "\xef\x81\xb4" /*61556, 0xF074*/
#define LV_SYMBOL_UP              "\xef\x81\xb7" /*61559, 0xF077*/
#define LV_SYMBOL_DOWN            "\xef\x81\xb8" /*61560, 0xF078*/
#define LV_SYMBOL_LOOP            "\xef\x81\xb9" /*61561, 0xF079*/
#define LV_SYMBOL_DIRECTORY       "\xef\x81\xbb" /*61563, 0xF07B*/
#define LV_SYMBOL_UPLOAD          "\xef\x82\x93" /*61587, 0xF093*/
#define LV_SYMBOL_CALL            "\xef\x82\x95" /*61589, 0xF095*/
#define LV_SYMBOL_CUT             "\xef\x83\x84" /*61636, 0xF0C4*/
#define LV_SYMBOL_COPY            "\xef\x83\x85" /*61637, 0xF0C5*/
#define LV_SYMBOL_SAVE            "\xef\x83\x87" /*61639, 0xF0C7*/
#define LV_SYMBOL_BARS            "\xef\x83\x89" /*61641, 0xF0C9*/
#define LV_SYMBOL_CHARGE          "\xef\x83\xa7" /*61671, 0xF0E7*/
#define LV_SYMBOL_PASTE           "\xef\x83\xAA" /*61674, 0xF0EA*/
#define LV_SYMBOL_BELL            "\xef\x83\xb3" /*61683, 0xF0F3*/
#define LV_SYMBOL_KEYBOARD        "\xef\x84\x9c" /*61724, 0xF11C*/
#define LV_SYMBOL_GPS             "\xef\x84\xa4" /*61732, 0xF124*/
#define LV_SYMBOL_FILE            "\xef\x85\x9b" /*61787, 0xF158*/
#define LV_SYMBOL_WIFI            "\xef\x87\xab" /*61931, 0xF1EB*/
#define LV_SYMBOL_BATTERY_FULL    "\xef\x89\x80" /*62016, 0xF240*/
#define LV_SYMBOL_BATTERY_3       "\xef\x89\x81" /*62017, 0xF241*/
#define LV_SYMBOL_BATTERY_2       "\xef\x89\x82" /*62018, 0xF242*/
#define LV_SYMBOL_BATTERY_1       "\xef\x89\x83" /*62019, 0xF243*/
#define LV_SYMBOL_BATTERY_EMPTY   "\xef\x89\x84" /*62020, 0xF244*/
#define LV_SYMBOL_USB             "\xef\x8a\x87" /*62087, 0xF287*/
#define LV_SYMBOL_BLUETOOTH       "\xef\x8a\x93" /*62099, 0xF293*/
#define LV_SYMBOL_TRASH           "\xef\x8B\xad" /*62189, 0xF2ED*/
#define LV_SYMBOL_BACKSPACE       "\xef\x95\x9A" /*62810, 0xF55A*/
#define LV_SYMBOL_SD_CARD         "\xef\x9F\x82" /*63426, 0xF7C2*/
#define LV_SYMBOL_NEW_LINE        "\xef\xA2\xA2" /*63650, 0xF8A2*/

/** Invalid symbol at (U+F8FF). If written before a string then `lv_img` will show it as a label*/
#define LV_SYMBOL_DUMMY           "\xEF\xA3\xBF"

/*-------------------------------
 * Symbols from "normal" font
 *-----------------------------*/
#define LV_SYMBOL_BULLET          "\xE2\x80\xA2" /*20042, 0x2022*/

/*
 * The following list is generated using
 * cat src/font/lv_symbol_def.h | sed -E -n 's/^#define\s+LV_(SYMBOL_\w+).*".*$/    _LV_STR_\1,/p'
 */
enum {
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
    _LV_STR_SYMBOL_BACKSPACE,
    _LV_STR_SYMBOL_SD_CARD,
    _LV_STR_SYMBOL_NEW_LINE,
    _LV_STR_SYMBOL_DUMMY,
    _LV_STR_SYMBOL_BULLET,
};

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SYMBOL_DEF_H*/
