include("${CMAKE_CURRENT_LIST_DIR}/version.cmake")

file(GLOB_RECURSE SOURCES ${LVGL_ROOT_DIR}/src/*.c ${LVGL_ROOT_DIR}/src/*.cpp)

idf_build_get_property(LV_MICROPYTHON LV_MICROPYTHON)

if(LV_MICROPYTHON)
  idf_component_register(
    SRCS
    ${SOURCES}
    INCLUDE_DIRS
    ${LVGL_ROOT_DIR}
    ${LVGL_ROOT_DIR}/src
    ${LVGL_ROOT_DIR}/../
    REQUIRES
    main)
else()
  if(CONFIG_LV_BUILD_EXAMPLES)
    file(GLOB_RECURSE EXAMPLE_SOURCES ${LVGL_ROOT_DIR}/examples/*.c)
    set_source_files_properties(${EXAMPLE_SOURCES} COMPILE_FLAGS "-Wno-unused-variable -Wno-format")
  endif()

  if(CONFIG_LV_USE_DEMO_WIDGETS)
    file(GLOB_RECURSE DEMO_WIDGETS_SOURCES ${LVGL_ROOT_DIR}/demos/widgets/*.c)
    list(APPEND DEMO_SOURCES ${DEMO_WIDGETS_SOURCES})
  endif()
  if(CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER)
    file(GLOB_RECURSE DEMO_KEYPAD_AND_ENCODER_SOURCES ${LVGL_ROOT_DIR}/demos/keypad_encoder/*.c)
    list(APPEND DEMO_SOURCES ${DEMO_KEYPAD_AND_ENCODER_SOURCES})
  endif()
  if(CONFIG_LV_USE_DEMO_BENCHMARK)
    file(GLOB_RECURSE DEMO_BENCHMARK_SOURCES ${LVGL_ROOT_DIR}/demos/benchmark/*.c)
    list(APPEND DEMO_SOURCES ${DEMO_BENCHMARK_SOURCES})
  endif()
  if(CONFIG_LV_USE_DEMO_STRESS)
    file(GLOB_RECURSE DEMO_STRESS_SOURCES ${LVGL_ROOT_DIR}/demos/stress/*.c)
    list(APPEND DEMO_SOURCES ${DEMO_STRESS_SOURCES})
  endif()
  if(CONFIG_LV_USE_DEMO_TRANSFORM)
    file(GLOB_RECURSE DEMO_TRANSFORM_SOURCES ${LVGL_ROOT_DIR}/demos/transform/*.c)
    list(APPEND DEMO_SOURCES ${DEMO_TRANSFORM_SOURCES})
  endif()
  if(CONFIG_LV_USE_DEMO_MULTILANG)
    file(GLOB_RECURSE DEMO_MULTILANG_SOURCES ${LVGL_ROOT_DIR}/demos/multilang/*.c)
    list(APPEND DEMO_SOURCES ${DEMO_MULTILANG_SOURCES})
  endif()
  if(CONFIG_LV_USE_DEMO_FLEX_LAYOUT)
    file(GLOB_RECURSE DEMO_FLEX_LAYOUT_SOURCES ${LVGL_ROOT_DIR}/demos/flex_layout/*.c)
    list(APPEND DEMO_SOURCES ${DEMO_FLEX_LAYOUT_SOURCES})
  endif()
  if(CONFIG_LV_USE_DEMO_SCROLL)
    file(GLOB_RECURSE DEMO_SCROLL_SOURCES ${LVGL_ROOT_DIR}/demos/scroll/*.c)
    list(APPEND DEMO_SOURCES ${DEMO_SCROLL_SOURCES})
  endif()
  if(CONFIG_LV_USE_DEMO_MUSIC)
    file(GLOB_RECURSE DEMO_MUSIC_SOURCES ${LVGL_ROOT_DIR}/demos/music/*.c)
    list(APPEND DEMO_SOURCES ${DEMO_MUSIC_SOURCES})
    set_source_files_properties(${DEMO_MUSIC_SOURCES} COMPILE_FLAGS "-Wno-format")
  endif()

  idf_component_register(SRCS ${SOURCES} ${EXAMPLE_SOURCES} ${DEMO_SOURCES}
      INCLUDE_DIRS ${LVGL_ROOT_DIR} ${LVGL_ROOT_DIR}/src ${LVGL_ROOT_DIR}/../
                   ${LVGL_ROOT_DIR}/examples ${LVGL_ROOT_DIR}/demos
      REQUIRES  esp_timer esp_driver_ppa esp_mm log)
endif()

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_CONF_INCLUDE_SIMPLE")

if(CONFIG_LV_ATTRIBUTE_FAST_MEM_USE_IRAM)
  target_compile_definitions(${COMPONENT_LIB}
                             PUBLIC "-DLV_ATTRIBUTE_FAST_MEM=IRAM_ATTR")
endif()

if(CONFIG_FREERTOS_SMP)
    target_include_directories(${COMPONENT_LIB} PRIVATE "${IDF_PATH}/components/freertos/FreeRTOS-Kernel-SMP/include/freertos/")
else()
    target_include_directories(${COMPONENT_LIB} PRIVATE "${IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos/")
endif()