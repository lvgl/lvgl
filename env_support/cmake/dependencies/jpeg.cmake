# ============================================================
# libjpeg Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "JPEG")
set(PKG_CONFIG_NAME "libjpeg")
set(PKG_LIB_PRIVATE "-ljpeg")

option(LV_USE_FIND_PACKAGE_JPEG "Resolve libjpeg via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_JPEG "Resolve libjpeg via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_JPEG "Fetch libjpeg-turbo from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_JPEG)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(JPEG_FOUND)
    message(STATUS "lvgl: libjpeg: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      JPEG::JPEG
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_JPEG AND PkgConfig_FOUND)
  pkg_check_modules(LIBJPEG IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(LIBJPEG_FOUND)
    message(STATUS "lvgl: libjpeg: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::LIBJPEG
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(NOT LV_FETCH_JPEG)
  message(
    FATAL_ERROR
      "lvgl: libjpeg not found. Enable LV_FETCH_JPEG or install it manually.")
endif()

message(STATUS "lvgl: libjpeg-turbo: fetching from source")
FetchContent_Declare(
  libjpeg-turbo
  GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo
  GIT_TAG 3.1.0)

set(ENABLE_SHARED
    OFF
    CACHE BOOL "" FORCE)
set(ENABLE_STATIC
    ON
    CACHE BOOL "" FORCE)
set(WITH_TURBOJPEG
    OFF
    CACHE BOOL "" FORCE)
set(WITH_JAVA
    OFF
    CACHE BOOL "" FORCE)
set(WITH_CRT_DLL
    OFF
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(libjpeg-turbo)
lvgl_link_fetched(TARGETS jpeg-static PKG_LIB_PRIVATE ${PKG_LIB_PRIVATE})
