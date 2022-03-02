file(GLOB_RECURSE SOURCES ${LVGL_ROOT_DIR}/src/*.c)
file(GLOB_RECURSE EXAMPLE_SOURCES ${LVGL_ROOT_DIR}/examples/*.c)

# With micropython, build lvgl as interface library, link chain is:
# lvgl_interface [lvgl] → usermod_lvgl_bindings [lv_bindings] → usermod
# [micropython] → firmware [micropython]
add_library(lvgl_interface INTERFACE)
# ${SOURCES} must NOT be given to add_library directly for some reason (won't be
# built)
target_sources(lvgl_interface INTERFACE ${SOURCES})
# Micropython builds with -Werror; we need to suppress some warnings, such as:
#
# /home/test/build/lv_micropython/ports/rp2/build-PICO/lv_mp.c:29316:16: error:
# 'lv_style_transition_dsc_t_path_xcb_callback' defined but not used
# [-Werror=unused-function] 29316 | STATIC int32_t
# lv_style_transition_dsc_t_path_xcb_callback(const struct _lv_anim_t * arg0) |
# ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
target_compile_options(lvgl_interface INTERFACE -Wno-unused-function)
