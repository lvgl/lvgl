/**
 * @file hal_indev_keycode.h
 *
 * @description Input Device keycodes
 * 
 */

#ifndef HAL_INDEV_KEYCODE_H
#define HAL_INDEV_KEYCODE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**
 * Common keycodes taken from Linux Source
 * include/uapi/linux/input-event-codes.h
 */
#define KEY_RESERVED    0
#define KEY_ESC         1
#define KEY_1           2
#define KEY_2           3
#define KEY_3           4
#define KEY_4           5
#define KEY_5           6
#define KEY_6           7
#define KEY_7           8
#define KEY_8           9
#define KEY_9           10
#define KEY_0           11
#define KEY_MINUS       12
#define KEY_EQUAL       13
#define KEY_BACKSPACE   14
#define KEY_TAB         15
#define KEY_Q           16
#define KEY_W           17
#define KEY_E           18
#define KEY_R           19
#define KEY_T           20
#define KEY_Y           21
#define KEY_U           22
#define KEY_I           23
#define KEY_O           24
#define KEY_P           25
#define KEY_LEFTBRACE   26
#define KEY_RIGHTBRACE  27
#define KEY_ENTER       28
#define KEY_LEFTCTRL    29
#define KEY_A           30
#define KEY_S           31
#define KEY_D           32
#define KEY_F           33
#define KEY_G           34
#define KEY_H           35
#define KEY_J           36
#define KEY_K           37
#define KEY_L           38
#define KEY_SEMICOLON   39
#define KEY_APOSTROPHE  40
#define KEY_GRAVE       41
#define KEY_LEFTSHIFT   42
#define KEY_BACKSLASH   43
#define KEY_Z           44
#define KEY_X           45
#define KEY_C           46
#define KEY_V           47
#define KEY_B           48
#define KEY_N           49
#define KEY_M           50
#define KEY_COMMA       51
#define KEY_DOT         52
#define KEY_SLASH       53
#define KEY_RIGHTSHIFT  54
#define KEY_KPASTERISK  55
#define KEY_LEFTALT     56
#define KEY_SPACE       57
#define KEY_CAPSLOCK    58
#define KEY_F1          59
#define KEY_F2          60
#define KEY_F3          61
#define KEY_F4          62
#define KEY_F5          63
#define KEY_F6          64
#define KEY_F7          65
#define KEY_F8          66
#define KEY_F9          67
#define KEY_F10         68
#define KEY_NUMLOCK     69
#define KEY_SCROLLLOCK  70
#define KEY_KP7         71
#define KEY_KP8         72
#define KEY_KP9         73
#define KEY_KPMINUS     74
#define KEY_KP4         75
#define KEY_KP5         76
#define KEY_KP6         77
#define KEY_KPPLUS      78
#define KEY_KP1         79
#define KEY_KP2         80
#define KEY_KP3         81
#define KEY_KP0         82
#define KEY_KPDOT       83

#define KEY_ZENKAKUHANKAKU      85
#define KEY_102ND               86
#define KEY_F11                 87
#define KEY_F12                 88
#define KEY_RO                  89
#define KEY_KATAKANA            90
#define KEY_HIRAGANA            91
#define KEY_HENKAN              92
#define KEY_KATAKANAHIRAGANA    93
#define KEY_MUHENKAN            94
#define KEY_KPJPCOMMA           95
#define KEY_KPENTER             96
#define KEY_RIGHTCTRL           97
#define KEY_KPSLASH             98
#define KEY_SYSRQ               99
#define KEY_RIGHTALT            100
#define KEY_LINEFEED            101
#define KEY_HOME                102
#define KEY_UP                  103
#define KEY_PAGEUP              104
#define KEY_LEFT                105
#define KEY_RIGHT               106
#define KEY_END                 107
#define KEY_DOWN                108
#define KEY_PAGEDOWN            109
#define KEY_INSERT              110
#define KEY_DELETE              111
#define KEY_MACRO               112
#define KEY_MUTE                113
#define KEY_VOLUMEDOWN          114
#define KEY_VOLUMEUP            115
#define KEY_POWER               116 /* SC System Power Down */
#define KEY_KPEQUAL             117
#define KEY_KPPLUSMINUS         118
#define KEY_PAUSE               119
#define KEY_SCALE               120 /* AL Compiz Scale (Expose) */

#define KEY_KPCOMMA             121
#define KEY_HANGEUL             122
#define KEY_HANGUEL             KEY_HANGEUL
#define KEY_HANJA               123
#define KEY_YEN                 124
#define KEY_LEFTMETA            125
#define KEY_RIGHTMETA           126
#define KEY_COMPOSE             127

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
