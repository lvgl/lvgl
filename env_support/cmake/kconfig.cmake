set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

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

# Returns in OUTPUT_VAR a hash of the given defconfig fragments: their
# paths and their contents.
function(lv_defconfig_hash FRAGMENTS OUTPUT_VAR)
    set(defconfig_list "strict=${LV_BUILD_DEFCONFIG_STRICT}")
    foreach(fragment IN LISTS FRAGMENTS)
        file(SHA256 ${fragment} fragment_hash)
        string(APPEND defconfig_list ";${fragment}=${fragment_hash}")
    endforeach()
    string(SHA256 defconfig_hash "${defconfig_list}")
    set(${OUTPUT_VAR} ${defconfig_hash} PARENT_SCOPE)
endfunction()

set(KCONFIG_INPUT_FLAGS)

if(LV_BUILD_DEFCONFIG_PATH AND LV_BUILD_DOTCONFIG_PATH)
    message(WARNING "Both LV_BUILD_DEFCONFIG_PATH and LV_BUILD_DOTCONFIG_PATH are set, \
ignoring the latter. Pass -DLV_BUILD_DEFCONFIG_PATH= to drop the defconfigs.")
endif()

# Check if the user wants to use a defconfig, using the -DLV_BUILD_DEFCONFIG_PATH option
if(LV_BUILD_DEFCONFIG_PATH)
    # Several defconfigs can be given as a ";"-separated list. They are merged in
    # order, so a later fragment overrides the value set by an earlier one. This
    # lets related configurations share a common base instead of duplicating it.
    set(DEFCONFIGS)
    foreach(defconfig IN LISTS LV_BUILD_DEFCONFIG_PATH)
        lv_normalize_config_path(defconfig "defconfig" defconfig_abs)
        if(NOT EXISTS ${defconfig_abs})
            message(FATAL_ERROR "lvgl: ${defconfig_abs} does not exist")
        endif()
        list(APPEND DEFCONFIGS ${defconfig_abs})
    endforeach()

    # The defconfig fragments are merged to create ${OUTPUT_DOTCONFIG}
    # ${OUTPUT_DOTCONFIG} can be edited by hand or with menuconfig and the next
    # build picks these updates instead of using the defconfig fragments we started with
    # Editing a fragment, or changing the list of fragments, creates ${OUTPUT_DOTCONFIG} again.
    lv_defconfig_hash("${DEFCONFIGS}" DEFCONFIG_HASH)

    if(EXISTS ${OUTPUT_DOTCONFIG} AND "${DEFCONFIG_HASH}" STREQUAL "${LV_BUILD_DEFCONFIG_HASH}")
        # No changes to defconfig fragments and ${OUTPUT_DOTCONFIG}
        # already exist, just use it
        set(DOTCONFIG ${OUTPUT_DOTCONFIG})
    else()
        if(EXISTS ${OUTPUT_DOTCONFIG})
            message(STATUS "lvgl: defconfigs changed, regenerating ${OUTPUT_DOTCONFIG} from them")
        endif()

        set(DOTCONFIG ${DEFCONFIGS})

        list(LENGTH DEFCONFIGS defconfig_count)
        if(LV_BUILD_DEFCONFIG_STRICT)
            # Apply the stricter checks: assignments to unknown or promptless
            # symbols, and values that end up not taking effect, become errors.
            # Overriding a symbol set by an earlier fragment is allowed.
            set(KCONFIG_INPUT_FLAGS --handwritten-input-configs)
        elseif(defconfig_count GREATER 1)
            # Merging without the strict checks needs this, otherwise overriding a
            # symbol set by an earlier fragment is reported as an error.
            set(KCONFIG_INPUT_FLAGS --forced-input-configs)
        endif()
    endif()

    # Both defconfig fragment and ${OUTPUT_DOTCONFIG} changes
    # must trigger a cmake configuration
    set(CONFIGURE_DEPENDS ${DEFCONFIGS} ${OUTPUT_DOTCONFIG})
elseif(LV_BUILD_DOTCONFIG_PATH)
    lv_normalize_config_path(LV_BUILD_DOTCONFIG_PATH ".config" DOTCONFIG)
    set(CONFIGURE_DEPENDS ${DOTCONFIG})
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
    set(CONFIGURE_DEPENDS ${DOTCONFIG})
endif()

foreach(config IN LISTS DOTCONFIG)
    if (NOT EXISTS ${config})
        message(FATAL_ERROR "defconfig: ${config} - does not exist")
    endif()
endforeach()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E env LVGL_DIR=${LVGL_ROOT_DIR}
    ${Python_EXECUTABLE}
    ${LVGL_ROOT_DIR}/scripts/build-tools/kconfig.py
    ${KCONFIG_INPUT_FLAGS}
    ${LV_BUILD_KCONFIG_ROOT}
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

if(DEFINED DEFCONFIG_HASH)
    # store the defconfig hash in cache so we can use it on the next run
    set(LV_BUILD_DEFCONFIG_HASH ${DEFCONFIG_HASH} CACHE INTERNAL
    "Hash of the defconfigs ${OUTPUT_DOTCONFIG} was seeded from")
else()
    # No defconfig in use any more, so the recorded hash would only make
    # a later one look already applied.
    unset(LV_BUILD_DEFCONFIG_HASH CACHE)
endif()

# Re-run CMake configuration (which regenerates autoconf.h) when the input
# .config/defconfig changes, so that `cmake --build` picks up config edits.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CONFIGURE_DEPENDS})

# Set the variable that can be used by the CMakeLists.txt including this file
set(KCONFIG_EXTERNAL_INCLUDE ${AUTOCONF_H})
