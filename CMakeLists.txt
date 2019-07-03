# Copyright (c) 2019 jan.van_winkel@dxplore.eu
#
# SPDX-License-Identifier: Apache-2.0

if (CONFIG_LVGL)

set(ZEPHYR_CURRENT_LIBRARY lvgl)
include_directories(${ZEPHYR_BASE}/lib/gui/lvgl)

target_include_directories(lvgl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

zephyr_compile_definitions(LV_CONF_INCLUDE_SIMPLE=1)

zephyr_library()

zephyr_library_sources(

    lv_core/lv_group.c
    lv_core/lv_indev.c
    lv_core/lv_lang.c
    lv_core/lv_obj.c
    lv_core/lv_refr.c
    lv_core/lv_style.c
    lv_core/lv_vdb.c

    lv_draw/lv_draw.c
    lv_draw/lv_draw_arc.c
    lv_draw/lv_draw_img.c
    lv_draw/lv_draw_label.c
    lv_draw/lv_draw_line.c
    lv_draw/lv_draw_rbasic.c
    lv_draw/lv_draw_rect.c
    lv_draw/lv_draw_triangle.c
    lv_draw/lv_draw_vbasic.c

    lv_hal/lv_hal_disp.c
    lv_hal/lv_hal_indev.c
    lv_hal/lv_hal_tick.c

    lv_misc/lv_anim.c
    lv_misc/lv_area.c
    lv_misc/lv_circ.c
    lv_misc/lv_color.c
    lv_misc/lv_font.c
    lv_misc/lv_fs.c
    lv_misc/lv_gc.c
    lv_misc/lv_ll.c
    lv_misc/lv_log.c
    lv_misc/lv_math.c
    lv_misc/lv_mem.c
    lv_misc/lv_task.c
    lv_misc/lv_templ.c
    lv_misc/lv_txt.c

    lv_objx/lv_arc.c
    lv_objx/lv_bar.c
    lv_objx/lv_btn.c
    lv_objx/lv_btnm.c
    lv_objx/lv_calendar.c
    lv_objx/lv_canvas.c
    lv_objx/lv_cb.c
    lv_objx/lv_chart.c
    lv_objx/lv_cont.c
    lv_objx/lv_ddlist.c
    lv_objx/lv_gauge.c
    lv_objx/lv_img.c
    lv_objx/lv_imgbtn.c
    lv_objx/lv_kb.c
    lv_objx/lv_label.c
    lv_objx/lv_led.c
    lv_objx/lv_line.c
    lv_objx/lv_list.c
    lv_objx/lv_lmeter.c
    lv_objx/lv_mbox.c
    lv_objx/lv_objx_templ.c
    lv_objx/lv_page.c
    lv_objx/lv_preload.c
    lv_objx/lv_roller.c
    lv_objx/lv_slider.c
    lv_objx/lv_spinbox.c
    lv_objx/lv_sw.c
    lv_objx/lv_ta.c
    lv_objx/lv_table.c
    lv_objx/lv_tabview.c
    lv_objx/lv_tileview.c
    lv_objx/lv_win.c

    lv_themes/lv_theme.c
    lv_themes/lv_theme_alien.c
    lv_themes/lv_theme_default.c
    lv_themes/lv_theme_material.c
    lv_themes/lv_theme_mono.c
    lv_themes/lv_theme_nemo.c
    lv_themes/lv_theme_night.c
    lv_themes/lv_theme_templ.c
    lv_themes/lv_theme_zen.c

    lv_fonts/lv_font_builtin.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_BUILD_IN_FONT_10
    lv_fonts/lv_font_dejavu_10.c
    lv_fonts/lv_font_dejavu_10_cyrillic.c
    lv_fonts/lv_font_dejavu_10_latin_sup.c
    lv_fonts/lv_font_symbol_10.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_BUILD_IN_FONT_20
    lv_fonts/lv_font_dejavu_20.c
    lv_fonts/lv_font_dejavu_20_cyrillic.c
    lv_fonts/lv_font_dejavu_20_latin_sup.c
    lv_fonts/lv_font_symbol_20.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_BUILD_IN_FONT_30
    lv_fonts/lv_font_dejavu_30.c
    lv_fonts/lv_font_dejavu_30_cyrillic.c
    lv_fonts/lv_font_dejavu_30_latin_sup.c
    lv_fonts/lv_font_symbol_30.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_BUILD_IN_FONT_40
    lv_fonts/lv_font_dejavu_40.c
    lv_fonts/lv_font_dejavu_40_cyrillic.c
    lv_fonts/lv_font_dejavu_40_latin_sup.c
    lv_fonts/lv_font_symbol_40.c
)

zephyr_library_sources_ifdef( CONFIG_LVGL_BUILD_IN_FONT_MONOSPACE
    lv_fonts/lv_font_monospace_8.c
)

endif()
