set(SOURCE_FILES 
    lv_anim.c
    lv_anim_timeline.c
    lv_area.c
    lv_array.c
    lv_async.c
    lv_circle_buf.c
    lv_color.c
    lv_color_op.c
    lv_event.c
    lv_fs.c
    lv_iter.c
    lv_ll.c
    lv_log.c
    lv_lru.c
    lv_math.c
    lv_palette.c
    lv_rb.c
    lv_style.c
    lv_style_gen.c
    lv_templ.c
    lv_text.c
    lv_timer.c
    lv_tree.c
    lv_utils.c
)


if(CONFIG_LV_USE_BIDI)
    list(APPEND SOURCE_FILES lv_bidi.c)
endif()

if(CONFIG_LV_USE_MATRIX) 
    list(APPEND SOURCE_FILES lv_matrix.c)
endif()

if(CONFIG_LV_USE_PROFILER AND CONFIG_LV_USE_PROFILER_BUILTIN) 
    list(APPEND SOURCE_FILES lv_profiler_builtin.c)
endif()


if(CONFIG_LV_USE_ARABIC_PERSIAN_CHARS)
    list(APPEND SOURCE_FILES lv_text_ap.c)
endif()


# Create the main static library for this directory
add_library(lvgl_misc STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_misc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
