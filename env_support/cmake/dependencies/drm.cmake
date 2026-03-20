find_package(libdrm QUIET)

if(libdrm_FOUND)
  message(STATUS "lvgl: libdrm: found via find_package")
  target_link_libraries(lvgl PRIVATE libdrm::libdrm)
  return()
endif()

if(PkgConfig_FOUND)
  pkg_check_modules(LIBDRM IMPORTED_TARGET QUIET libdrm)
  if(LIBDRM_FOUND)
    message(STATUS "lvgl: libdrm: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::LIBDRM)
    return()
  endif()
endif()

message(FATAL_ERROR "lvgl: libdrm not found. Please install libdrm.")
