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
  #[[
    unfortunately CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS does not work for global data. 
    for global data we still need decl specs.
    Check out the docs to lear more about the limitations of CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS
    https://cmake.org/cmake/help/latest/prop_tgt/WINDOWS_EXPORT_ALL_SYMBOLS.html#prop_tgt:WINDOWS_EXPORT_ALL_SYMBOLS

    For all files within the library (i.e. basically all lvgl files) we need to use dllimport.
    For all files from outsider the library (i.e. files which include lvgl headers) we need to use dllexport.
    We can do this by using CMakes PUBLIC and PRIVATE keyword.
  ]]
  target_compile_definitions(lvgl
    PRIVATE LV_ATTRIBUTE_EXTERN_DATA=__declspec\(dllimport\)
    PUBLIC LV_ATTRIBUTE_EXTERN_DATA=__declspec\(dllexport\)
  )
endif()
