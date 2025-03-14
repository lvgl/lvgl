set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(KCONFIG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Kconfig)
set(AUTOCONF_H ${CMAKE_CURRENT_BINARY_DIR}/autoconf.h)
set(OUTPUT_DOTCONFIG ${CMAKE_CURRENT_BINARY_DIR}/.config)
set(KCONFIG_LIST_OUT ${CMAKE_CURRENT_BINARY_DIR}/kconfig_list)
set(AUTO_CONF_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Allow the user to pass -DDEFCONFIG=<path_to_a_defconfig> instead of
# using only .config
if (DEFCONFIG)
    set(DOTCONFIG ${DEFCONFIG})
else()
    set(DOTCONFIG ${CMAKE_CURRENT_SOURCE_DIR}/.config)
endif()

execute_process(
  COMMAND ${Python_EXECUTABLE}
  ${LVGL_ROOT_DIR}/scripts/kconfig.py
  ${LVGL_ROOT_DIR}/Kconfig
  ${OUTPUT_DOTCONFIG}
  ${AUTOCONF_H}
  ${KCONFIG_LIST_OUT}
  ${DOTCONFIG}
  WORKING_DIRECTORY ${LVGL_ROOT_DIR}
  # The working directory is set to the app dir such that the user
  # can use relative paths in CONF_FILE, e.g. CONF_FILE=nrf5.conf
  RESULT_VARIABLE ret
)
if(NOT "${ret}" STREQUAL "0")
  message(FATAL_ERROR "command failed with return code: ${ret}")
endif()

# Re-configure (Re-execute all CMakeLists.txt code) when autoconf.h changes
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${AUTOCONF_H})

# Set compile definitions for lvgl build
add_compile_definitions(LV_CONF_KCONFIG_EXTERNAL_INCLUDE="${AUTOCONF_H}")
# Set the variable that can be used by the CMakeLists.txt including this file
set(LV_CONF_KCONFIG_EXTERNAL_INCLUDE "${AUTOCONF_H}")