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

#[[
    unfortunately CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS does not work for global data. 
    for global data we still need decl specs.
    Check out the docs to learn more about the limitations of CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS
    https://cmake.org/cmake/help/latest/prop_tgt/WINDOWS_EXPORT_ALL_SYMBOLS.html#prop_tgt:WINDOWS_EXPORT_ALL_SYMBOLS

    For all compiled sources within the library (i.e. basically all lvgl files) we need to use dllexport.
    For all compiled sources from outside the library (i.e. files which include lvgl headers) we need to use dllimport.
    We can do this by using CMakes INTERFACE and PRIVATE keyword.
  ]]
if (MSVC)
  target_compile_definitions(lvgl
    INTERFACE LV_ATTRIBUTE_EXTERN_DATA=__declspec\(dllimport\)
    PRIVATE LV_ATTRIBUTE_EXTERN_DATA=__declspec\(dllexport\)
  )

  #[[
    GCC and Clang have added some (non std compliant) extensions to their preprocessor.
    ##VA_ARGS__ is one of them. It is used in lvgl to support variadic macros.
    if no argument is passed to the macro, the MSVC preprocessor will not remove the trailing comma.
    This will result in a syntax error.
    To fix this we need to enable the /Zc:preprocessor flag. This will make the MSVC preprocessor behave like the GCC/Clang preprocessor.
    In C++20 and C23 __VA_OPT__ is introduced. Making this behaviour standard.
    But for now we will stick with this preprocessor flag.
  ]]
  target_compile_options(lvgl PUBLIC /Zc:preprocessor)
endif()
