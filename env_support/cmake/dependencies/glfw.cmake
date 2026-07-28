# ============================================================
# GLFW Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "glfw3")
set(PKG_CONFIG_NAME "glfw3")
set(PKG_LIB_PRIVATE "-lglfw")

option(LV_USE_FIND_PACKAGE_GLFW "Resolve GLFW via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_GLFW "Resolve GLFW via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_GLFW "Fetch GLFW from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_GLFW)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(glfw3_FOUND)
    message(STATUS "lvgl: GLFW: found via find_package")
    lvgl_link_packages(
      PRIVATE
      TARGETS
      glfw
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_GLFW AND PkgConfig_FOUND)
  pkg_check_modules(GLFW3 IMPORTED_TARGET QUIET ${PKG_CONFIG_NAME})
  if(GLFW3_FOUND)
    message(STATUS "lvgl: GLFW: found via pkg-config")
    lvgl_link_pkg_config(
      PRIVATE
      TARGETS
      PkgConfig::GLFW3
      PKG_CONFIG
      ${PKG_CONFIG_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    return()
  endif()
endif()

if(NOT LV_FETCH_GLFW)
  message(
    FATAL_ERROR
      "lvgl: GLFW not found. Enable LV_FETCH_GLFW or install libglfw3 manually."
  )
endif()

message(STATUS "lvgl: GLFW: fetching from source")
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
  GIT_TAG 3.4)

set(GLFW_BUILD_DOCS
    OFF
    CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)
lvgl_link_fetched(TARGETS glfw PKG_LIB_PRIVATE ${PKG_LIB_PRIVATE})
