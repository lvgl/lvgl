option(LV_USE_FIND_PACKAGE_GLFW "Resolve GLFW via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_GLFW "Resolve GLFW via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_GLFW "Fetch GLFW from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_GLFW)
  find_package(glfw3 QUIET)
  if(glfw3_FOUND)
    message(STATUS "lvgl: GLFW: found via find_package")
    target_link_libraries(lvgl PRIVATE glfw)
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_GLFW AND PkgConfig_FOUND)
  pkg_check_modules(GLFW3 IMPORTED_TARGET QUIET glfw3)
  if(GLFW3_FOUND)
    message(STATUS "lvgl: GLFW: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::GLFW3)
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
target_link_libraries(lvgl PRIVATE glfw)
