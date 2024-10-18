find_package(PkgConfig)
pkg_check_modules(PKG_Libdrm pkg_check_modules libdrm)

set(Libdrm_DEFINITIONS ${PKG_Libdrm_CFLAGS_OTHER})
set(Libdrm_VERSION ${PKG_Libdrm_VERSION})

find_path(Libdrm_INCLUDE_DIR
    NAMES
        xf86drm.h
    HINTS
        ${PKG_Libdrm_INCLUDE_DIRS}
)
find_library(Libdrm_LIBRARY
    NAMES
        drm
    HINTS
        ${PKG_Libdrm_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libdrm
    FOUND_VAR
        Libdrm_FOUND
    REQUIRED_VARS
        Libdrm_LIBRARY
        Libdrm_INCLUDE_DIR
    VERSION_VAR
        Libdrm_VERSION
)

if(Libdrm_FOUND AND NOT TARGET Libdrm::Libdrm)
    add_library(Libdrm::Libdrm UNKNOWN IMPORTED)
    set_target_properties(Libdrm::Libdrm PROPERTIES
        IMPORTED_LOCATION "${Libdrm_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${Libdrm_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${Libdrm_INCLUDE_DIR}"
        INTERFACE_INCLUDE_DIRECTORIES "${Libdrm_INCLUDE_DIR}/libdrm"
    )
endif()

mark_as_advanced(Libdrm_LIBRARY Libdrm_INCLUDE_DIR)

set(Libdrm_LIBRARIES ${Libdrm_LIBRARY})
set(Libdrm_INCLUDE_DIRS ${Libdrm_INCLUDE_DIR} "${Libdrm_INCLUDE_DIR}/libdrm")
set(Libdrm_VERSION_STRING ${Libdrm_VERSION}    ${PKG_Libdrm_LIBRARY_DIRS})
