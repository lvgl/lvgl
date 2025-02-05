set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
set(KCONFIG_ROOT ${CMAKE_SOURCE_DIR}/Kconfig)
set(AUTOCONF_H ${CMAKE_CURRENT_BINARY_DIR}/autoconf.h)
set(OUTPUT_DOTCONFIG ${CMAKE_CURRENT_BINARY_DIR}/.config)
set(KCONFIG_LIST_OUT ${CMAKE_CURRENT_BINARY_DIR}/kconfig_list)
set(AUTO_CONF_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(DOTCONFIG ${CMAKE_SOURCE_DIR}/.config)

# Re-configure (Re-execute all CMakeLists.txt code) when autoconf.h changes
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${AUTOCONF_H})

include(cmake/extensions.cmake)
include(cmake/kconfig.cmake)
add_compile_definitions(LV_CONF_KCONFIG_EXTERNAL_INCLUDE="${AUTOCONF_H}")
