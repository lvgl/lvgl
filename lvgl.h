/**
 * @file lvgl.h
 * Include all LVGL related headers
 */

#ifndef LVGL_H
#define LVGL_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************
 * CURRENT VERSION OF LVGL
 ***************************/
#include "lv_version.h"

/*********************
 *      INCLUDES
 *********************/
#include "src/lv_init.h"

#include "src/stdlib/lv_mem.h"
#include "src/stdlib/lv_string.h"
#include "src/stdlib/lv_sprintf.h"

#include "src/misc/lv_log.h"
#include "src/misc/lv_timer.h"
#include "src/misc/lv_math.h"
#include "src/misc/lv_array.h"
#include "src/misc/lv_async.h"
#include "src/misc/lv_anim_timeline.h"
#include "src/misc/lv_profiler_builtin.h"
#include "src/misc/lv_rb.h"
#include "src/misc/lv_utils.h"
#include "src/misc/lv_iter.h"
#include "src/misc/lv_circle_buf.h"
#include "src/misc/lv_tree.h"
#include "src/misc/cache/lv_image_cache.h"

#include "src/tick/lv_tick.h"

#include "src/core/lv_obj.h"
#include "src/core/lv_group.h"
#include "src/indev/lv_indev.h"
#include "src/indev/lv_indev_gesture.h"
#include "src/core/lv_refr.h"
#include "src/display/lv_display.h"

#include "src/font/lv_font.h"
#include "src/font/lv_binfont_loader.h"
#include "src/font/lv_font_fmt_txt.h"

#include "src/widgets/animimage/lv_animimage.h"
#include "src/widgets/arc/lv_arc.h"
#include "src/widgets/bar/lv_bar.h"
#include "src/widgets/button/lv_button.h"
#include "src/widgets/buttonmatrix/lv_buttonmatrix.h"
#include "src/widgets/calendar/lv_calendar.h"
#include "src/widgets/canvas/lv_canvas.h"
#include "src/widgets/chart/lv_chart.h"
#include "src/widgets/checkbox/lv_checkbox.h"
#include "src/widgets/dropdown/lv_dropdown.h"
#include "src/widgets/image/lv_image.h"
#include "src/widgets/imagebutton/lv_imagebutton.h"
#include "src/widgets/keyboard/lv_keyboard.h"
#include "src/widgets/label/lv_label.h"
#include "src/widgets/led/lv_led.h"
#include "src/widgets/line/lv_line.h"
#include "src/widgets/list/lv_list.h"
#include "src/widgets/lottie/lv_lottie.h"
#include "src/widgets/menu/lv_menu.h"
#include "src/widgets/msgbox/lv_msgbox.h"
#include "src/widgets/roller/lv_roller.h"
#include "src/widgets/scale/lv_scale.h"
#include "src/widgets/slider/lv_slider.h"
#include "src/widgets/span/lv_span.h"
#include "src/widgets/spinbox/lv_spinbox.h"
#include "src/widgets/spinner/lv_spinner.h"
#include "src/widgets/switch/lv_switch.h"
#include "src/widgets/table/lv_table.h"
#include "src/widgets/tabview/lv_tabview.h"
#include "src/widgets/textarea/lv_textarea.h"
#include "src/widgets/tileview/lv_tileview.h"
#include "src/widgets/win/lv_win.h"

#include "src/others/snapshot/lv_snapshot.h"
#include "src/others/sysmon/lv_sysmon.h"
#include "src/others/monkey/lv_monkey.h"
#include "src/others/gridnav/lv_gridnav.h"
#include "src/others/fragment/lv_fragment.h"
#include "src/others/imgfont/lv_imgfont.h"
#include "src/others/observer/lv_observer.h"
#include "src/others/ime/lv_ime_pinyin.h"
#include "src/others/file_explorer/lv_file_explorer.h"
#include "src/others/font_manager/lv_font_manager.h"
#include "src/others/xml/lv_xml.h"
#include "src/others/xml/lv_xml_component.h"

#include "src/libs/barcode/lv_barcode.h"
#include "src/libs/bin_decoder/lv_bin_decoder.h"
#include "src/libs/bmp/lv_bmp.h"
#include "src/libs/rle/lv_rle.h"
#include "src/libs/fsdrv/lv_fsdrv.h"
#include "src/libs/lodepng/lv_lodepng.h"
#include "src/libs/libpng/lv_libpng.h"
#include "src/libs/gif/lv_gif.h"
#include "src/libs/qrcode/lv_qrcode.h"
#include "src/libs/tjpgd/lv_tjpgd.h"
#include "src/libs/libjpeg_turbo/lv_libjpeg_turbo.h"
#include "src/libs/freetype/lv_freetype.h"
#include "src/libs/rlottie/lv_rlottie.h"
#include "src/libs/ffmpeg/lv_ffmpeg.h"
#include "src/libs/tiny_ttf/lv_tiny_ttf.h"
#include "src/libs/svg/lv_svg.h"
#include "src/libs/svg/lv_svg_render.h"

#include "src/layouts/lv_layout.h"

#include "src/draw/lv_draw_buf.h"
#include "src/draw/lv_draw_vector.h"
#include "src/draw/sw/lv_draw_sw_utils.h"

#include "src/themes/lv_theme.h"

#include "src/drivers/lv_drivers.h"

#include "src/lv_api_map_v8.h"
#include "src/lv_api_map_v9_0.h"
#include "src/lv_api_map_v9_1.h"

#if LV_USE_PRIVATE_API
#include "src/lvgl_private.h"
#endif


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

/** Gives 1 if the x.y.z version is supported in the current version
 * Usage:
 *
 * - Require v6
 * #if LV_VERSION_CHECK(6,0,0)
 *   new_func_in_v6();
 * #endif
 *
 *
 * - Require at least v5.3
 * #if LV_VERSION_CHECK(5,3,0)
 *   new_feature_from_v5_3();
 * #endif
 *
 *
 * - Require v5.3.2 bugfixes
 * #if LV_VERSION_CHECK(5,3,2)
 *   bugfix_in_v5_3_2();
 * #endif
 *
 */
#define LV_VERSION_CHECK(x,y,z) (x == LVGL_VERSION_MAJOR && (y < LVGL_VERSION_MINOR || (y == LVGL_VERSION_MINOR && z <= LVGL_VERSION_PATCH)))

/**
 * Wrapper functions for VERSION macros
 */

static inline int lv_version_major(void)
{
    return LVGL_VERSION_MAJOR;
}

static inline int lv_version_minor(void)
{
    return LVGL_VERSION_MINOR;
}

static inline int lv_version_patch(void)
{
    return LVGL_VERSION_PATCH;
}

static inline const char * lv_version_info(void)
{
    return LVGL_VERSION_INFO;
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_H*/
