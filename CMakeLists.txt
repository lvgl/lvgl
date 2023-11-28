cmake_minimum_required(VERSION 3.12.4)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(NOT ESP_PLATFORM)
  project(lvgl LANGUAGES C CXX ASM HOMEPAGE_URL https://github.com/lvgl/lvgl)
endif()

set(LVGL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

if(ESP_PLATFORM)
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/esp.cmake)
elseif(ZEPHYR_BASE)
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/zephyr.cmake)
elseif(MICROPY_DIR)
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/micropython.cmake)
else()
  include(${CMAKE_CURRENT_LIST_DIR}/env_support/cmake/custom.cmake)
endif()

if (MSVC)
  if (BUILD_SHARED_LIBS)
    target_compile_definitions(lvgl PUBLIC LV_ATTRIBUTE_EXTERN_DATA=__declspec\(dllimport\))
  else()
    target_compile_definitions(lvgl PUBLIC LV_ATTRIBUTE_EXTERN_DATA=__declspec\(dllexport\))
  endif()
endif()