# Copyright (c) 2019 jan.van_winkel@dxplore.eu
# Copyright (c) 2020 Teslabs Engineering S.L.
#
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_LVGL)

set(ZEPHYR_CURRENT_LIBRARY lvgl)
include_directories(${ZEPHYR_BASE}/lib/gui/lvgl)

target_include_directories(lvgl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

zephyr_compile_definitions(LV_CONF_INCLUDE_SIMPLE=1)

zephyr_library()

zephyr_library_sources(
    src/lv_core/lv_disp.c
    src/lv_core/lv_group.c
    src/lv_core/lv_indev.c
    src/lv_core/lv_obj.c
    src/lv_core/lv_refr.c
    src/lv_core/lv_style.c

    src/lv_draw/lv_draw_arc.c
    src/lv_draw/lv_draw_blend.c
    src/lv_draw/lv_draw_img.c
    src/lv_draw/lv_draw_label.c
    src/lv_draw/lv_draw_line.c
    src/lv_draw/lv_draw_mask.c
    src/lv_draw/lv_draw_rect.c
    src/lv_draw/lv_draw_triangle.c
    src/lv_draw/lv_img_buf.c
    src/lv_draw/lv_img_cache.c
    src/lv_draw/lv_img_decoder.c

    src/lv_hal/lv_hal_disp.c
    src/lv_hal/lv_hal_indev.c
    src/lv_hal/lv_hal_tick.c

    src/lv_misc/lv_anim.c
    src/lv_misc/lv_area.c
    src/lv_misc/lv_async.c
    src/lv_misc/lv_bidi.c
    src/lv_misc/lv_color.c
    src/lv_misc/lv_debug.c
    src/lv_misc/lv_fs.c
    src/lv_misc/lv_gc.c
    src/lv_misc/lv_ll.c
    src/lv_misc/lv_log.c
    src/lv_misc/lv_math.c
    src/lv_misc/lv_mem.c
    src/lv_misc/lv_printf.c
    src/lv_misc/lv_task.c
    src/lv_misc/lv_templ.c
    src/lv_misc/lv_txt.c
    src/lv_misc/lv_txt_ap.c
    src/lv_misc/lv_utils.c

    src/lv_font/lv_font.c
    src/lv_font/lv_font_dejavu_16_persian_hebrew
    src/lv_font/lv_font_fmt_txt.c
    src/lv_font/lv_font_montserrat_12.c
    src/lv_font/lv_font_montserrat_12_subpx.c
    src/lv_font/lv_font_montserrat_14.c
    src/lv_font/lv_font_montserrat_16.c
    src/lv_font/lv_font_montserrat_18.c
    src/lv_font/lv_font_montserrat_20.c
    src/lv_font/lv_font_montserrat_22.c
    src/lv_font/lv_font_montserrat_24.c
    src/lv_font/lv_font_montserrat_26.c
    src/lv_font/lv_font_montserrat_28.c
    src/lv_font/lv_font_montserrat_28_compressed.c
    src/lv_font/lv_font_montserrat_30.c
    src/lv_font/lv_font_montserrat_32.c
    src/lv_font/lv_font_montserrat_34.c
    src/lv_font/lv_font_montserrat_36.c
    src/lv_font/lv_font_montserrat_38.c
    src/lv_font/lv_font_montserrat_40.c
    src/lv_font/lv_font_montserrat_42.c
    src/lv_font/lv_font_montserrat_44.c
    src/lv_font/lv_font_montserrat_46.c
    src/lv_font/lv_font_montserrat_48.c
    src/lv_font/lv_font_simsun_16_cjk.c
    src/lv_font/lv_font_unscii_8.c

    src/lv_themes/lv_theme.c
    src/lv_themes/lv_theme_empty.c
    src/lv_themes/lv_theme_material.c
    src/lv_themes/lv_theme_mono.c

    src/lv_widgets/lv_arc.c
    src/lv_widgets/lv_bar.c
    src/lv_widgets/lv_btn.c
    src/lv_widgets/lv_btnmatrix.c
    src/lv_widgets/lv_calendar.c
    src/lv_widgets/lv_canvas.c
    src/lv_widgets/lv_chart.c
    src/lv_widgets/lv_checkbox.c
    src/lv_widgets/lv_cont.c
    src/lv_widgets/lv_cpicker.c
    src/lv_widgets/lv_dropdown.c
    src/lv_widgets/lv_gauge.c
    src/lv_widgets/lv_img.c
    src/lv_widgets/lv_imgbtn.c
    src/lv_widgets/lv_keyboard.c
    src/lv_widgets/lv_label.c
    src/lv_widgets/lv_led.c
    src/lv_widgets/lv_line.c
    src/lv_widgets/lv_linemeter.c
    src/lv_widgets/lv_list.c
    src/lv_widgets/lv_msgbox.c
    src/lv_widgets/lv_objmask.c
    src/lv_widgets/lv_page.c
    src/lv_widgets/lv_roller.c
    src/lv_widgets/lv_slider.c
    src/lv_widgets/lv_spinbox.c
    src/lv_widgets/lv_spinner.c
    src/lv_widgets/lv_switch.c
    src/lv_widgets/lv_table.c
    src/lv_widgets/lv_tabview.c
    src/lv_widgets/lv_textarea.c
    src/lv_widgets/lv_tileview.c
    src/lv_widgets/lv_win.c
)

endif()
