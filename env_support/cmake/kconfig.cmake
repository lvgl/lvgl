set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(KCONFIG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Kconfig)
set(AUTOCONF_H ${CMAKE_CURRENT_BINARY_DIR}/autoconf.h)
set(OUTPUT_DOTCONFIG ${CMAKE_CURRENT_BINARY_DIR}/.config)
set(KCONFIG_LIST_OUT ${CMAKE_CURRENT_BINARY_DIR}/kconfig_list)
set(AUTO_CONF_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Normalizes INPUT_PATH (which is relative to CMAKE_SOURCE_DIR) to an absolute path 
macro(lv_normalize_config_path INPUT_PATH LABEL OUTPUT_VAR)
    message(STATUS "Using ${LABEL}: ${${INPUT_PATH}}")
    if(NOT IS_ABSOLUTE ${${INPUT_PATH}})
        file(REAL_PATH ${${INPUT_PATH}} ${OUTPUT_VAR} BASE_DIRECTORY ${CMAKE_SOURCE_DIR})
        message(STATUS "Converted to absolute path: ${${OUTPUT_VAR}}")
    else()
        set(${OUTPUT_VAR} ${${INPUT_PATH}})
    endif()
endmacro()

# Check if the user wants to use a defconfig, using the -DLV_BUILD_DEFCONFIG_PATH option
if(LV_BUILD_DEFCONFIG_PATH)
    lv_normalize_config_path(LV_BUILD_DEFCONFIG_PATH "defconfig" DOTCONFIG)
elseif(LV_BUILD_DOTCONFIG_PATH)
    lv_normalize_config_path(LV_BUILD_DOTCONFIG_PATH ".config" DOTCONFIG)
else()
    # No explicit config file set
    # Search, in order:
    #   1. the top-level project directory (matches where lv_conf.h is expected)
    #   2. the LVGL source directory (standard Kconfig in-tree location)
    #   3. the .config generated in the binary directory on a previous run
    if(EXISTS ${CMAKE_SOURCE_DIR}/.config)
        set(DOTCONFIG ${CMAKE_SOURCE_DIR}/.config)
    elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.config)
        set(DOTCONFIG ${CMAKE_CURRENT_SOURCE_DIR}/.config)
    else()
        set(DOTCONFIG ${OUTPUT_DOTCONFIG})
    endif()
endif()

if (NOT EXISTS ${DOTCONFIG})
    message(FATAL_ERROR "defconfig: ${DOTCONFIG} - does not exist")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E env LVGL_DIR=${LVGL_ROOT_DIR}
    ${Python_EXECUTABLE} 
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

# Re-run CMake configuration (which regenerates autoconf.h) when the input
# .config/defconfig changes, so that `cmake --build` picks up config edits.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${DOTCONFIG})

# Set the variable that can be used by the CMakeLists.txt including this file
set(KCONFIG_EXTERNAL_INCLUDE ${AUTOCONF_H})

# Ensure LV_BUILD_DEFCONFIG_PATH is not set in the path, to be able to call it without
# the -DLV_BUILD_DEFCONFIG_PATH after the first configuration, and to work with the .config
unset(LV_BUILD_DEFCONFIG_PATH CACHE)
