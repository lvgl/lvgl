/**
 * @file lvgl_private.h
 *
 */

#ifndef LVGL_PRIVATE_H
#define LVGL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "core/lv_global.h"

#include "display/lv_display_private.h"
#include "indev/lv_indev_private.h"
#include "misc/lv_text_private.h"
#include "misc/cache/lv_cache_entry_private.h"
#include "misc/cache/lv_cache_private.h"
#include "layouts/lv_layout_private.h"
#include "stdlib/lv_mem_private.h"
#include "others/file_explorer/lv_file_explorer_private.h"
#include "others/sysmon/lv_sysmon_private.h"
#include "others/monkey/lv_monkey_private.h"
#include "others/ime/lv_ime_pinyin_private.h"
#include "others/fragment/lv_fragment_private.h"
#include "others/observer/lv_observer_private.h"
#include "libs/qrcode/lv_qrcode_private.h"
#include "libs/barcode/lv_barcode_private.h"
#include "libs/gif/lv_gif_private.h"
#include "draw/lv_draw_triangle_private.h"
#include "draw/lv_draw_private.h"
#include "draw/lv_draw_rect_private.h"
#include "draw/lv_draw_image_private.h"
#include "draw/lv_image_decoder_private.h"
#include "draw/lv_draw_label_private.h"
#include "draw/lv_draw_vector_private.h"
#include "draw/lv_draw_buf_private.h"
#include "draw/lv_draw_mask_private.h"
#include "draw/sw/lv_draw_sw_gradient_private.h"
#include "draw/sw/lv_draw_sw_private.h"
#include "draw/sw/lv_draw_sw_mask_private.h"
#include "draw/sw/blend/lv_draw_sw_blend_private.h"
#include "drivers/libinput/lv_xkb_private.h"
#include "drivers/libinput/lv_libinput_private.h"
#include "font/lv_font_fmt_txt_private.h"
#include "themes/lv_theme_private.h"
#include "core/lv_refr_private.h"
#include "core/lv_obj_style_private.h"
#include "core/lv_obj_private.h"
#include "core/lv_obj_scroll_private.h"
#include "core/lv_obj_draw_private.h"
#include "core/lv_obj_class_private.h"
#include "core/lv_group_private.h"
#include "core/lv_obj_event_private.h"
#include "misc/lv_timer_private.h"
#include "misc/lv_area_private.h"
#include "misc/lv_fs_private.h"
#include "misc/lv_profiler_builtin_private.h"
#include "misc/lv_event_private.h"
#include "misc/lv_bidi_private.h"
#include "misc/lv_rb_private.h"
#include "misc/lv_style_private.h"
#include "misc/lv_color_op_private.h"
#include "misc/lv_anim_private.h"
#include "widgets/msgbox/lv_msgbox_private.h"
#include "widgets/buttonmatrix/lv_buttonmatrix_private.h"
#include "widgets/slider/lv_slider_private.h"
#include "widgets/switch/lv_switch_private.h"
#include "widgets/calendar/lv_calendar_chinese_private.h"
#include "widgets/calendar/lv_calendar_private.h"
#include "widgets/imagebutton/lv_imagebutton_private.h"
#include "widgets/bar/lv_bar_private.h"
#include "widgets/image/lv_image_private.h"
#include "widgets/textarea/lv_textarea_private.h"
#include "widgets/table/lv_table_private.h"
#include "widgets/checkbox/lv_checkbox_private.h"
#include "widgets/roller/lv_roller_private.h"
#include "widgets/win/lv_win_private.h"
#include "widgets/keyboard/lv_keyboard_private.h"
#include "widgets/line/lv_line_private.h"
#include "widgets/animimage/lv_animimage_private.h"
#include "widgets/dropdown/lv_dropdown_private.h"
#include "widgets/menu/lv_menu_private.h"
#include "widgets/chart/lv_chart_private.h"
#include "widgets/button/lv_button_private.h"
#include "widgets/scale/lv_scale_private.h"
#include "widgets/led/lv_led_private.h"
#include "widgets/arc/lv_arc_private.h"
#include "widgets/tileview/lv_tileview_private.h"
#include "widgets/spinbox/lv_spinbox_private.h"
#include "widgets/span/lv_span_private.h"
#include "widgets/label/lv_label_private.h"
#include "widgets/canvas/lv_canvas_private.h"
#include "widgets/tabview/lv_tabview_private.h"
#include "tick/lv_tick_private.h"
#include "stdlib/builtin/lv_tlsf_private.h"
#include "libs/rlottie/lv_rlottie_private.h"
#include "libs/ffmpeg/lv_ffmpeg_private.h"
#include "widgets/lottie/lv_lottie_private.h"
#include "osal/lv_os_private.h"

/*********************
 *      DEFINES
 *********************/

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
} /*extern "C"*/
#endif

#endif /*LVGL_PRIVATE_H*/
