function(lvgl_link_libraries)
  set(options PUBLIC PRIVATE FETCHED)
  set(multiValueArgs TARGETS PKG_CONFIG CMAKE_PACKAGE PKG_LIB_PRIVATE
                     RAW_LIB_PRIVATE)
  cmake_parse_arguments(ARG "${options}" "" "${multiValueArgs}" ${ARGN})

  set(SCOPE "PRIVATE")
  if(ARG_PUBLIC)
    set(SCOPE "PUBLIC")
  endif()

  foreach(target IN LISTS ARG_TARGETS)
    if(target MATCHES "^PkgConfig::")
      target_link_libraries(lvgl ${SCOPE} $<BUILD_INTERFACE:${target}>)
    else()
      target_link_libraries(lvgl ${SCOPE} ${target})
    endif()
  endforeach()

  # Handle Raw Linker Flags (e.g., -lm, -lpthread)
  if(ARG_PKG_LIB_PRIVATE)
    get_property(current_libs GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE)
    list(APPEND current_libs ${ARG_PKG_LIB_PRIVATE})
    set_property(GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE "${current_libs}")
  endif()

  # If we have a raw lib but no cmake package, it needs to be baked into the
  # exported target
  if(ARG_PKG_LIB_PRIVATE AND NOT ARG_CMAKE_PACKAGE)
    get_property(current_raw GLOBAL PROPERTY LVGL_CMAKE_RAW_LIBS)
    list(APPEND current_raw ${ARG_PKG_LIB_PRIVATE})
    set_property(GLOBAL PROPERTY LVGL_CMAKE_RAW_LIBS "${current_raw}")
  endif()

  # Skip Metadata Registration for Fetched Dependencies If FETCHED is set, the
  # code is baked in or handled internally. We do NOT want downstream users to
  # try and find these on their system.
  if(ARG_FETCHED)
    return()
  endif()

  # Registration for pkg-config (.pc)
  if(ARG_PKG_CONFIG)
    if(ARG_PUBLIC)
      set(pc_prop "LVGL_PKG_REQUIRES")
    else()
      set(pc_prop "LVGL_PKG_REQUIRES_PRIVATE")
    endif()

    get_property(current_pc GLOBAL PROPERTY ${pc_prop})
    list(APPEND current_pc ${ARG_PKG_CONFIG})
    set_property(GLOBAL PROPERTY ${pc_prop} "${current_pc}")
  endif()

  # Registration for CMake (lvglConfig.cmake)
  if(ARG_CMAKE_PACKAGE)
    if(ARG_PUBLIC)
      set(cmake_prop "LVGL_CMAKE_PUBLIC_DEPS")
    else()
      set(cmake_prop "LVGL_CMAKE_PRIVATE_DEPS")
    endif()

    get_property(current_cmake GLOBAL PROPERTY ${cmake_prop})
    list(APPEND current_cmake ${ARG_CMAKE_PACKAGE})
    set_property(GLOBAL PROPERTY ${cmake_prop} "${current_cmake}")
  endif()
endfunction()

function(lvgl_link_system_library LIB_NAME PC_FLAG)
  target_link_libraries(lvgl PRIVATE ${LIB_NAME})
  get_property(current_libs GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE)
  list(APPEND current_libs ${PC_FLAG})
  set_property(GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE "${current_libs}")
endfunction()

set_property(GLOBAL PROPERTY LVGL_PKG_REQUIRES "")
set_property(GLOBAL PROPERTY LVGL_PKG_REQUIRES_PRIVATE "")
set_property(GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE "")
set_property(GLOBAL PROPERTY LVGL_CMAKE_PUBLIC_DEPS "")
set_property(GLOBAL PROPERTY LVGL_CMAKE_PRIVATE_DEPS "")
set_property(GLOBAL PROPERTY LVGL_CMAKE_RAW_LIBS "")
