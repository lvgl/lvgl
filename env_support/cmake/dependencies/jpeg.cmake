find_package(JPEG QUIET)
if(JPEG_FOUND)
  message(STATUS "lvgl: libjpeg: found via find_package")
  target_link_libraries(lvgl PRIVATE JPEG::JPEG)
  return()
endif()

if(PkgConfig_FOUND)
  pkg_check_modules(LIBJPEG IMPORTED_TARGET QUIET libjpeg)
  if(LIBJPEG_FOUND)
    message(STATUS "lvgl: libjpeg: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::LIBJPEG)
    return()
  endif()
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
target_link_libraries(lvgl PRIVATE jpeg-static)
