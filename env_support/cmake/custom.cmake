include("${CMAKE_CURRENT_LIST_DIR}/version.cmake")

# Option to define LV_LVGL_H_INCLUDE_SIMPLE, default: ON
option(LV_LVGL_H_INCLUDE_SIMPLE
       "Use #include \"lvgl.h\" instead of #include \"../../lvgl.h\"" ON)

# Option to define LV_CONF_INCLUDE_SIMPLE, default: ON
option(LV_CONF_INCLUDE_SIMPLE
       "Use #include \"lv_conf.h\" instead of #include \"../../lv_conf.h\"" ON)

# Option LV_CONF_PATH, which should be the path for lv_conf.h
# If set parent path LV_CONF_DIR is added to includes
if( LV_CONF_PATH )
    get_filename_component(LV_CONF_DIR ${LV_CONF_PATH} DIRECTORY)
endif( LV_CONF_PATH )

# Option to build shared libraries (as opposed to static), default: OFF
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Set sources used for LVGL components
file(GLOB_RECURSE _LVGL_SOURCES ${LVGL_ROOT_DIR}/src/*.c ${LVGL_ROOT_DIR}/src/*.S)
file(GLOB_RECURSE _LVGL_HEADER ${LVGL_ROOT_DIR}/src/*.h )
file(GLOB_RECURSE EXAMPLE_SOURCES ${LVGL_ROOT_DIR}/examples/*.c)
file(GLOB_RECURSE DEMO_SOURCES ${LVGL_ROOT_DIR}/demos/*.c)
file(GLOB_RECURSE THORVG_SOURCES ${LVGL_ROOT_DIR}/src/libs/thorvg/*.cpp ${LVGL_ROOT_DIR}/src/others/vg_lite_tvg/*.cpp)

