#ifndef LVGL_H
#define LVGL_H

#include "lv_version.h"
#include "3d/lv_3dmath.h"
#include "3d/lv_gltf_environment.h"
#include "3d/lv_gltf_model.h"
#include "3d/lv_gltf_model_loader.h"
#include "3d/lv_gltf_model_node.h"

/* Define LV_DISABLE_API_MAPPING using a compiler option
 * to make sure your application is not using deprecated names */
#ifndef LV_DISABLE_API_MAPPING
    #include "api_map/lv_api_map_v8.h"
    #include "api_map/lv_api_map_v9_0.h"
    #include "api_map/lv_api_map_v9_1.h"
    #include "api_map/lv_api_map_v9_2.h"
    #include "api_map/lv_api_map_v9_3.h"
    #include "api_map/lv_api_map_v9_4.h"
    #include "api_map/lv_api_map_v9_5.h"
#endif /*LV_DISABLE_API_MAPPING*/

#include "config/lv_conf_internal.h"
#include "config/lv_conf_kconfig.h"
#include "core/lv_anim.h"
#include "core/lv_anim_timeline.h"
#include "core/lv_area.h"
#include "core/lv_event.h"
#include "core/lv_group.h"
#include "core/lv_init.h"
#include "core/lv_matrix.h"
#include "core/lv_obj.h"
#include "core/lv_obj_class.h"
#include "core/lv_obj_draw.h"
#include "core/lv_obj_event.h"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_property.h"
#include "core/lv_obj_property_names.h"
#include "core/lv_obj_scroll.h"
#include "core/lv_obj_style.h"
#include "core/lv_obj_style_gen.h"
#include "core/lv_obj_tree.h"
#include "core/lv_observer.h"
#include "core/lv_refr.h"
#include "core/lv_style.h"
#include "core/lv_style_gen.h"
#include "core/lv_style_properties.h"
#include "core/lv_timer.h"
#include "core/lv_translation.h"
#include "debugging/lv_assert.h"
#include "debugging/lv_check_arg.h"
#include "debugging/lv_monkey.h"
#include "debugging/lv_profiler.h"
#include "debugging/lv_profiler_builtin.h"
#include "debugging/lv_sysmon.h"
#include "debugging/lv_test.h"
#include "debugging/lv_test_display.h"
#include "debugging/lv_test_fs.h"
#include "debugging/lv_test_helpers.h"
#include "debugging/lv_test_indev.h"
#include "debugging/lv_test_indev_gesture.h"
#include "debugging/lv_test_screenshot_compare.h"
#include "display/lv_display.h"
#include "draw/lv_color.h"
#include "draw/lv_color_op.h"
#include "draw/lv_draw.h"
#include "draw/lv_draw_3d.h"
#include "draw/lv_draw_arc.h"
#include "draw/lv_draw_blur.h"
#include "draw/lv_draw_buf.h"
#include "draw/lv_draw_image.h"
#include "draw/lv_draw_label.h"
#include "draw/lv_draw_line.h"
#include "draw/lv_draw_mask.h"
#include "draw/lv_draw_rect.h"
#include "draw/lv_draw_triangle.h"
#include "draw/lv_draw_vector.h"
#include "draw/lv_grad.h"
#include "draw/lv_image_dsc.h"
#include "draw/lv_palette.h"
#include "draw/lv_snapshot.h"
#include "draw/sw/lv_draw_sw_utils.h"
#include "drivers/display/lv_linux_drm.h"
#include "drivers/display/lv_draw_eve_display.h"
#include "drivers/display/lv_draw_eve_display_defines.h"
#include "drivers/display/lv_draw_eve_target.h"
#include "drivers/display/lv_linux_fbdev.h"
#include "drivers/display/lv_ft81x.h"
#include "drivers/display/lv_ili9341.h"
#include "drivers/display/lv_lcd_generic_mipi.h"
#include "drivers/display/lv_lovyan_gfx.h"
#include "drivers/display/lv_nv3007.h"
#include "drivers/display/lv_nxp_elcdif.h"
#include "drivers/display/lv_renesas_glcdc.h"
#include "drivers/display/lv_st7735.h"
#include "drivers/display/lv_st7789.h"
#include "drivers/display/lv_st7796.h"
#include "drivers/display/lv_st_ltdc.h"
#include "drivers/display/lv_tft_espi.h"
#include "drivers/ffmpeg/lv_ffmpeg.h"
#include "drivers/indev/lv_evdev.h"
#include "drivers/indev/lv_libinput.h"
#include "drivers/indev/lv_xkb.h"
#include "drivers/nuttx/lv_nuttx_entry.h"
#include "drivers/nuttx/lv_nuttx_fbdev.h"
#include "drivers/nuttx/lv_nuttx_lcd.h"
#include "drivers/nuttx/lv_nuttx_libuv.h"
#include "drivers/nuttx/lv_nuttx_touchscreen.h"
#include "drivers/opengles/lv_opengles_driver.h"
#include "drivers/opengles/lv_opengles_glfw.h"
#include "drivers/opengles/lv_opengles_texture.h"
#include "drivers/opengles/lv_opengles_window.h"
#include "drivers/qnx/lv_qnx.h"
#include "drivers/sdl/lv_sdl_keyboard.h"
#include "drivers/sdl/lv_sdl_mouse.h"
#include "drivers/sdl/lv_sdl_mousewheel.h"
#include "drivers/sdl/lv_sdl_window.h"
#include "drivers/uefi/lv_uefi.h"
#include "drivers/uefi/lv_uefi_context.h"
#include "drivers/uefi/lv_uefi_display.h"
#include "drivers/uefi/lv_uefi_edk2.h"
#include "drivers/uefi/lv_uefi_gnu_efi.h"
#include "drivers/uefi/lv_uefi_indev.h"
#include "drivers/wayland/lv_wayland.h"
#include "drivers/wayland/lv_wayland_keyboard.h"
#include "drivers/wayland/lv_wayland_pointer.h"
#include "drivers/wayland/lv_wayland_pointer_axis.h"
#include "drivers/wayland/lv_wayland_touch.h"
#include "drivers/wayland/lv_wayland_window.h"
#include "drivers/windows/lv_windows_display.h"
#include "drivers/windows/lv_windows_input.h"
#include "drivers/x11/lv_x11.h"
#include "font/lv_bidi.h"
#include "font/lv_binfont_loader.h"
#include "font/lv_font.h"
#include "font/lv_font_fmt_txt.h"
#include "font/lv_font_manager.h"
#include "font/lv_freetype.h"
#include "font/lv_imgfont.h"
#include "font/lv_symbol_def.h"
#include "font/lv_text.h"
#include "font/lv_tiny_ttf.h"
#include "fs/lv_fs.h"
#include "fs/lv_fsdrv.h"
#include "image/lv_bin_decoder.h"
#include "image/lv_bmp.h"
#include "image/lv_libjpeg_turbo.h"
#include "image/lv_libpng.h"
#include "image/lv_libwebp.h"
#include "image/lv_lodepng.h"
#include "image/lv_image_decoder.h"
#include "image/lv_svg.h"
#include "image/lv_tjpgd.h"
#include "indev/lv_gridnav.h"
#include "indev/lv_indev.h"
#include "indev/lv_indev_gesture.h"
#include "layouts/lv_flex.h"
#include "layouts/lv_grid.h"
#include "layouts/lv_layout.h"
#include "logging/lv_log.h"
#include "lv_types.h"
#include "misc/lv_async.h"
#include "misc/lv_math.h"
#include "misc/lv_tree.h"
#include "osal/lv_os.h"
#include "others/file_explorer/lv_file_explorer.h"
#include "others/fragment/lv_fragment.h"
#include "stdlib/lv_mem.h"
#include "stdlib/lv_sprintf.h"
#include "stdlib/lv_string.h"
#include "themes/default/lv_theme_default.h"
#include "themes/lv_theme.h"
#include "themes/mono/lv_theme_mono.h"
#include "themes/simple/lv_theme_simple.h"
#include "tick/lv_tick.h"
#include "widgets/lv_3dtexture.h"
#include "widgets/lv_animimage.h"
#include "widgets/lv_arc.h"
#include "widgets/lv_arclabel.h"
#include "widgets/lv_bar.h"
#include "widgets/lv_barcode.h"
#include "widgets/lv_button.h"
#include "widgets/lv_buttonmatrix.h"
#include "widgets/lv_calendar.h"
#include "widgets/lv_calendar_chinese.h"
#include "widgets/lv_calendar_header_arrow.h"
#include "widgets/lv_calendar_header_dropdown.h"
#include "widgets/lv_canvas.h"
#include "widgets/lv_chart.h"
#include "widgets/lv_checkbox.h"
#include "widgets/lv_dropdown.h"
#include "widgets/lv_gif.h"
#include "widgets/lv_gltf.h"
#include "widgets/lv_gstreamer.h"
#include "widgets/lv_image.h"
#include "widgets/lv_imagebutton.h"
#include "widgets/lv_ime_pinyin.h"
#include "widgets/lv_keyboard.h"
#include "widgets/lv_label.h"
#include "widgets/lv_led.h"
#include "widgets/lv_line.h"
#include "widgets/lv_list.h"
#include "widgets/lv_lottie.h"
#include "widgets/lv_menu.h"
#include "widgets/lv_msgbox.h"
#include "widgets/lv_qrcode.h"
#include "widgets/lv_rlottie.h"
#include "widgets/lv_roller.h"
#include "widgets/lv_scale.h"
#include "widgets/lv_slider.h"
#include "widgets/lv_span.h"
#include "widgets/lv_spinbox.h"
#include "widgets/lv_spinner.h"
#include "widgets/lv_switch.h"
#include "widgets/lv_table.h"
#include "widgets/lv_tabview.h"
#include "widgets/lv_textarea.h"
#include "widgets/lv_tileview.h"
#include "widgets/lv_win.h"

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
#endif /*LVGL_H*/
