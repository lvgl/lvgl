if(CONFIG_LVGL)

  zephyr_include_directories(${ZEPHYR_BASE}/lib/gui/lvgl)

  target_include_directories(lvgl INTERFACE ${LVGL_ROOT_DIR})

  zephyr_compile_definitions(LV_CONF_KCONFIG_EXTERNAL_INCLUDE=<autoconf.h>)

  zephyr_library()

  file(GLOB_RECURSE SOURCES ${LVGL_ROOT_DIR}/src/*.c)
  zephyr_library_sources(${SOURCES})

endif(CONFIG_LVGL)
