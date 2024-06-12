cmake_minimum_required(VERSION 2.8.12)
project(lv_wayland)

find_package(PkgConfig)
pkg_check_modules(wayland-client REQUIRED wayland-client)
pkg_check_modules(wayland-cursor REQUIRED wayland-cursor)
pkg_check_modules(xkbcommon REQUIRED xkbcommon)

# Wayland protocols
find_program(WAYLAND_SCANNER_EXECUTABLE NAMES wayland-scanner)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols>=1.25)
pkg_get_variable(WAYLAND_PROTOCOLS_BASE wayland-protocols pkgdatadir)

macro(wayland_generate protocol_xml_file output_dir target)
    get_filename_component(output_file_base ${protocol_xml_file} NAME_WE)
    set(output_file_noext "${output_dir}/wayland-${output_file_base}-client-protocol")
    add_custom_command(OUTPUT "${output_file_noext}.h"
        COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" client-header "${protocol_xml_file}" "${output_file_noext}.h"
        DEPENDS "${protocol_xml_file}"
        VERBATIM)

    add_custom_command(OUTPUT "${output_file_noext}.c"
        COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" private-code "${protocol_xml_file}" "${output_file_noext}.c"
        DEPENDS "${protocol_xml_file}"
        VERBATIM)

    if(NOT EXISTS ${protocol_xml_file})
        message("Protocol XML file not found: " ${protocol_xml_file})
    else()
        set_property(TARGET ${target} APPEND PROPERTY SOURCES  "${output_file_noext}.h" "${output_file_noext}.c")
    endif()
endmacro()

set(WAYLAND_PROTOCOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/protocols")
file(MAKE_DIRECTORY ${WAYLAND_PROTOCOLS_DIR})

add_custom_target(generate_protocols ALL)

wayland_generate("${WAYLAND_PROTOCOLS_BASE}/stable/xdg-shell/xdg-shell.xml" ${WAYLAND_PROTOCOLS_DIR} generate_protocols)
