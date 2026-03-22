find_package(glfw3 QUIET)

if(glfw3_FOUND)
  message(STATUS "lvgl: GLFW: found via find_package")
  target_link_libraries(lvgl PRIVATE glfw GLEW)
  return()
endif()

if(PkgConfig_FOUND)
  pkg_check_modules(GLFW3 IMPORTED_TARGET QUIET glfw3)
  pkg_check_modules(GLEW IMPORTED_TARGET QUIET glew)
  if(GLFW3_FOUND AND GLEW_FOUND)
    message(STATUS "lvgl: GLFW: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::GLFW3 PkgConfig::GLEW)
    return()
  endif()
endif()

message(
  FATAL_ERROR
    "lvgl: GLFW3 or GLEW not found. Please install libglfw3 and libglew.")
