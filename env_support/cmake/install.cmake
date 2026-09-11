# Install public headers
install(
    DIRECTORY "${LVGL_ROOT_DIR}/include/lvgl"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# Install private headers only if required
if(CONFIG_LV_USE_PRIVATE_API)
    install(
        DIRECTORY "${LVGL_ROOT_DIR}/src/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lvgl_private"
        FILES_MATCHING PATTERN "*.h"
    )

    install(CODE "
        file(READ \"${CMAKE_INSTALL_FULL_INCLUDEDIR}/lvgl_private/lvgl_public.h\" content)
        string(REPLACE \"../include/lvgl/lvgl.h\" \"../lvgl/lvgl.h\" content \"\${content}\")
        file(WRITE \"${CMAKE_INSTALL_FULL_INCLUDEDIR}/lvgl_private/lvgl_public.h\" \"\${content}\")
    ")
endif()

# Install lv_conf.h inside lvgl/config so its next to lv_conf_internal.h
# and define LV_CONF_INCLUDE_SIMPLE so that it can be found
lvgl_install_definitions(LV_CONF_INCLUDE_SIMPLE)
if(LV_BUILD_USE_KCONFIG)
    # Kconfig: install the expanded configuration header
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/lv_conf_expanded.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lvgl/config"
        RENAME lv_conf.h)
elseif(NOT LV_CONF_SKIP)
    # Non-kconfig: install the actual lv_conf.h used during the build
    install(
        FILES "${CONF_PATH}"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lvgl/config"
        RENAME lv_conf.h)
endif()

# Collect accumulated dependencies
get_property(PKG_REQUIRES GLOBAL PROPERTY LVGL_PKG_REQUIRES)
get_property(PKG_REQUIRES_PRIVATE GLOBAL PROPERTY LVGL_PKG_REQUIRES_PRIVATE)
get_property(PKG_LIBS_PRIVATE GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE)
get_property(CMAKE_PUBLIC_DEPS GLOBAL PROPERTY LVGL_CMAKE_PUBLIC_DEPS)
get_property(CMAKE_PRIVATE_DEPS GLOBAL PROPERTY LVGL_CMAKE_PRIVATE_DEPS)
get_property(CMAKE_RAW_LIBS GLOBAL PROPERTY LVGL_CMAKE_RAW_LIBS)
get_property(PKG_CFLAGS GLOBAL PROPERTY LVGL_INSTALL_DEFINES)

list(JOIN PKG_CFLAGS " " LVGL_PKG_CFLAGS)
list(JOIN PKG_LIBS_PRIVATE " " LVGL_PKG_LIBS_PRIVATE)
list(JOIN PKG_REQUIRES " " LVGL_PKG_REQUIRES)
list(JOIN PKG_REQUIRES_PRIVATE " " LVGL_PKG_REQUIRES_PRIVATE)
list(JOIN CMAKE_PUBLIC_DEPS ";" LVGL_CMAKE_PUBLIC_DEPS)
list(JOIN CMAKE_PRIVATE_DEPS ";" LVGL_CMAKE_PRIVATE_DEPS)
list(JOIN CMAKE_RAW_LIBS ";" LVGL_CMAKE_RAW_LIBS)

if(LVGL_CMAKE_RAW_LIBS)
	set_property(TARGET lvgl APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${LVGL_CMAKE_RAW_LIBS}")
endif()

# Generate and install pkg-config file
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/lvgl.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lvgl.pc"
    @ONLY
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/lvgl.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)

# Generate and install CMake config files
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/lvglConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lvglConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lvgl"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/lvglConfigVersion.cmake"
    VERSION ${LVGL_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/lvglConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/lvglConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lvgl"
)

install(TARGETS lvgl
    EXPORT lvglTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT lvglTargets
    NAMESPACE lvgl::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lvgl"
)

# Install library thorvg
if(LV_BUILD_THORVG_INTERNAL)

    set_target_properties(
        lvgl_thorvg
        PROPERTIES OUTPUT_NAME lvgl_thorvg
        VERSION ${LVGL_VERSION}
        SOVERSION ${LVGL_SOVERSION}
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

    install(TARGETS lvgl_thorvg
        EXPORT lvglTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

endif()

if(CONFIG_LV_BUILD_DEMOS)

    # Install demo headers (only if user enables demos)
    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demos"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lvgl"
        FILES_MATCHING PATTERN "*.h"
    )

    # Demo library target (if you build one)
    install(TARGETS lvgl_demos
	EXPORT lvglDemosTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(EXPORT lvglDemosTargets
        NAMESPACE lvgl::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lvgl"
    )

endif()

if(CONFIG_LV_BUILD_EXAMPLES)

    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lvgl"
        FILES_MATCHING PATTERN "*.h"
    )

    install(TARGETS lvgl_examples
	EXPORT lvglExamplesTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(EXPORT lvglExamplesTargets
        NAMESPACE lvgl::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lvgl"
    )

endif()
