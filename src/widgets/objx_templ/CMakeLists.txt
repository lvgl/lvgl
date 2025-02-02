set(SOURCE_FILES lv_objx_templ.c)
# Create the main static library for this directory
add_library(lvgl_widget_objx_templ STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_widget_objx_templ PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})