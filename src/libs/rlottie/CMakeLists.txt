set(LVGL_LIBS_RLOTTIE_SOURCES 
    lv_rlottie.c
)

# Create the object library for this directory
add_library(lvgl_libs_rlottie OBJECT ${LVGL_LIBS_RLOTTIE_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_rlottie PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
