if(CONFIG_LVGL)

  zephyr_include_directories(${ZEPHYR_BASE}/lib/gui/lvgl)

  target_include_directories(lvgl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

  zephyr_compile_definitions(LV_CONF_KCONFIG_EXTERNAL_INCLUDE=<autoconf.h>)

  zephyr_library()

  file(GLOB_RECURSE SOURCES src/*.c)
  zephyr_library_sources(${SOURCES})

endif(CONFIG_LVGL)
