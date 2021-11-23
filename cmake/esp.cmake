file(GLOB_RECURSE SOURCES ${LVGL_ROOT_DIR}/src/*.c)

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

  target_compile_definitions(${COMPONENT_LIB}
                             INTERFACE "-DLV_CONF_INCLUDE_SIMPLE")

  if(CONFIG_LV_ATTRIBUTE_FAST_MEM_USE_IRAM)
    target_compile_definitions(${COMPONENT_LIB}
                               INTERFACE "-DLV_ATTRIBUTE_FAST_MEM=IRAM_ATTR")
  endif()
else()
  idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS ${LVGL_ROOT_DIR}
                         ${LVGL_ROOT_DIR}/src ${LVGL_ROOT_DIR}/../)

  target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_CONF_INCLUDE_SIMPLE")

  if(CONFIG_LV_ATTRIBUTE_FAST_MEM_USE_IRAM)
    target_compile_definitions(${COMPONENT_LIB}
                               PUBLIC "-DLV_ATTRIBUTE_FAST_MEM=IRAM_ATTR")
  endif()
endif()
