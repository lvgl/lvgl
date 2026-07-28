set_property(GLOBAL PROPERTY LVGL_PKG_REQUIRES "")
set_property(GLOBAL PROPERTY LVGL_PKG_REQUIRES_PRIVATE "")
set_property(GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE "")
set_property(GLOBAL PROPERTY LVGL_CMAKE_PUBLIC_DEPS "")
set_property(GLOBAL PROPERTY LVGL_CMAKE_PRIVATE_DEPS "")
set_property(GLOBAL PROPERTY LVGL_CMAKE_RAW_LIBS "")

macro(lvgl_add_pkg_requires)
  get_property(_cur GLOBAL PROPERTY LVGL_PKG_REQUIRES)
  list(APPEND _cur ${ARGN})
  set_property(GLOBAL PROPERTY LVGL_PKG_REQUIRES "${_cur}")
endmacro()

macro(lvgl_add_pkg_requires_private)
  get_property(_cur GLOBAL PROPERTY LVGL_PKG_REQUIRES_PRIVATE)
  list(APPEND _cur ${ARGN})
  set_property(GLOBAL PROPERTY LVGL_PKG_REQUIRES_PRIVATE "${_cur}")
endmacro()

macro(lvgl_add_pkg_libs_private)
  get_property(_cur GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE)
  list(APPEND _cur ${ARGN})
  set_property(GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE "${_cur}")
endmacro()

macro(lvgl_add_cmake_public_deps)
  get_property(_cur GLOBAL PROPERTY LVGL_CMAKE_PUBLIC_DEPS)
  list(APPEND _cur ${ARGN})
  set_property(GLOBAL PROPERTY LVGL_CMAKE_PUBLIC_DEPS "${_cur}")
endmacro()

macro(lvgl_add_cmake_private_deps)
  get_property(_cur GLOBAL PROPERTY LVGL_CMAKE_PRIVATE_DEPS)
  list(APPEND _cur ${ARGN})
  set_property(GLOBAL PROPERTY LVGL_CMAKE_PRIVATE_DEPS "${_cur}")
endmacro()

macro(lvgl_add_cmake_raw_libs)
  get_property(_cur GLOBAL PROPERTY LVGL_CMAKE_RAW_LIBS)
  list(APPEND _cur ${ARGN})
  set_property(GLOBAL PROPERTY LVGL_CMAKE_RAW_LIBS "${_cur}")
endmacro()

# ============================================================
# Link Functions
# ============================================================

# Links packages found via `find_package` 
# 1. Links targets normally (will be exported)
# 2. Registers CMAKE_PACKAGE for find_dependency in lvglConfig.cmake
# 3. Registers PKG_CONFIG for .pc Requires/Requires.private 
# 4. Registers PKG_LIB_PRIVATE for .pc Libs.private
function(lvgl_link_packages)
  set(options PUBLIC PRIVATE)
  set(multiValueArgs TARGETS CMAKE_PACKAGE PKG_CONFIG PKG_LIB_PRIVATE)
  cmake_parse_arguments(ARG "${options}" "" "${multiValueArgs}" ${ARGN})

  set(SCOPE PRIVATE)
  if(ARG_PUBLIC OR CONFIG_LV_USE_PRIVATE_API)
    set(SCOPE PUBLIC)
  endif()

  if(ARG_TARGETS)
    target_link_libraries(lvgl ${SCOPE} ${ARG_TARGETS})
  endif()

  if(ARG_CMAKE_PACKAGE)
    if(ARG_PUBLIC)
      lvgl_add_cmake_public_deps(${ARG_CMAKE_PACKAGE})
    else()
      lvgl_add_cmake_private_deps(${ARG_CMAKE_PACKAGE})
    endif()
  endif()

  if(ARG_PKG_CONFIG)
    if(ARG_PUBLIC)
      lvgl_add_pkg_requires(${ARG_PKG_CONFIG})
    else()
      lvgl_add_pkg_requires_private(${ARG_PKG_CONFIG})
    endif()
  endif()

  if(ARG_PKG_LIB_PRIVATE)
    lvgl_add_pkg_libs_private(${ARG_PKG_LIB_PRIVATE})
  endif()
endfunction()

# Links a package via pkg-config
# 1. Wraps targets in BUILD_INTERFACE (not exported) 
# 2. Bakes include dirs into lvgl's interface 
# 3. Registers raw libs for INTERFACE_LINK_LIBRARIES on exported target 
# 4. Registers PKG_CONFIG for .pc Requires/Requires.private
# 5. Registers PKG_LIB_PRIVATE for .pc Libs.private
function(lvgl_link_pkg_config)
  set(options PUBLIC PRIVATE)
  set(multiValueArgs TARGETS PKG_CONFIG PKG_LIB_PRIVATE)
  cmake_parse_arguments(ARG "${options}" "" "${multiValueArgs}" ${ARGN})

  set(SCOPE PRIVATE)
  if(ARG_PUBLIC OR CONFIG_LV_USE_PRIVATE_API)
    set(SCOPE PUBLIC)
  endif()

  foreach(_target IN LISTS ARG_TARGETS)
    target_link_libraries(lvgl ${SCOPE} $<BUILD_INTERFACE:${_target}>)
    get_target_property(_inc_dirs ${_target} INTERFACE_INCLUDE_DIRECTORIES)
    foreach(_dir IN LISTS _inc_dirs)
      target_include_directories(
        lvgl ${SCOPE} $<BUILD_INTERFACE:${_dir}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    endforeach()
  endforeach()

  if(ARG_PKG_LIB_PRIVATE)
    lvgl_add_pkg_libs_private(${ARG_PKG_LIB_PRIVATE})
    lvgl_add_cmake_raw_libs(${ARG_PKG_LIB_PRIVATE})
  endif()

  if(ARG_PKG_CONFIG)
    if(ARG_PUBLIC)
      lvgl_add_pkg_requires(${ARG_PKG_CONFIG})
    else()
      lvgl_add_pkg_requires_private(${ARG_PKG_CONFIG})
    endif()
  endif()
endfunction()

# Links targets fetched from source (FetchContent). No find_package nor
# pkgconfig support. Only registers PKG_LIB_PRIVATE for .pc Libs.private.
#
# In the cmake path, we only set these for lvgl's BUILD_INTERFACE to
# exclude them from lvgl's exported link interface.
function(lvgl_link_fetched)
  set(multiValueArgs TARGETS PKG_LIB_PRIVATE)
  cmake_parse_arguments(ARG "" "" "${multiValueArgs}" ${ARGN})

  set(SCOPE PRIVATE)
  if(CONFIG_LV_USE_PRIVATE_API)
    set(SCOPE PUBLIC)
  endif()

  if(ARG_TARGETS)
    set(_targets)
    foreach(_target IN LISTS ARG_TARGETS)
      list(APPEND _targets $<BUILD_INTERFACE:${_target}>)
    endforeach()
    target_link_libraries(lvgl ${SCOPE} ${_targets})
  endif()

  if(ARG_PKG_LIB_PRIVATE)
    lvgl_add_pkg_libs_private(${ARG_PKG_LIB_PRIVATE})
  endif()
endfunction()

# Links a system library that is neither found via find_package/pkg-config nor
# fetched from source (e.g. m). Unlike lvgl_link_fetched, these must stay
# in lvgl's exported link interface so consumers of an installed lvgl link
# against them too. Only registers PKG_LIB_PRIVATE for .pc Libs.private.
function(lvgl_link_system_lib)
  set(multiValueArgs TARGETS PKG_LIB_PRIVATE)
  cmake_parse_arguments(ARG "" "" "${multiValueArgs}" ${ARGN})

  set(SCOPE PRIVATE)
  if(CONFIG_LV_USE_PRIVATE_API)
    set(SCOPE PUBLIC)
  endif()

  if(ARG_TARGETS)
    target_link_libraries(lvgl ${SCOPE} ${ARG_TARGETS})
  endif()

  if(ARG_PKG_LIB_PRIVATE)
    lvgl_add_pkg_libs_private(${ARG_PKG_LIB_PRIVATE})
  endif()
endfunction()
