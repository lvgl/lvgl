# Accumulates the compile definitions that are active while BUILDING lvgl.
set_property(GLOBAL PROPERTY LVGL_BUILD_DEFINES "")

# Accumulates the "-D..." fragments emitted into the pkg-config Cflags line.
# These mirror the install-interface definitions: both describe what a downstream
# consumer needs to compile against the installed lvgl, just via different channels
# (this one is the .pc file, the CMake exported target is the other).
set_property(GLOBAL PROPERTY LVGL_INSTALL_DEFINES "")

# Parse the [PUBLIC|PRIVATE|INTERFACE] scope keyword, defaulting to PUBLIC.
# Sets _scope and _defs in the calling function scope.
macro(_lvgl_parse_definition_scope)
  set(options PUBLIC PRIVATE INTERFACE)
  cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

  set(_scope PUBLIC)
  if(ARG_PRIVATE)
    set(_scope PRIVATE)
  elseif(ARG_INTERFACE)
    set(_scope INTERFACE)
  endif()

  set(_defs ${ARG_UNPARSED_ARGUMENTS})
endmacro()

# Adds compile definitions that apply when BUILDING lvgl.
# 1. Wraps each definition in $<BUILD_INTERFACE:...> on the lvgl target
# 2. Records the raw definitions so preprocess_lv_conf_internal.py can consume them
#
# Usage: lvgl_build_definitions([PUBLIC|PRIVATE|INTERFACE] <def>...)
function(lvgl_build_definitions)
  _lvgl_parse_definition_scope(${ARGN})

  foreach(_def IN LISTS _defs)
    target_compile_definitions(lvgl ${_scope} $<BUILD_INTERFACE:${_def}>)
  endforeach()

  get_property(_cur GLOBAL PROPERTY LVGL_BUILD_DEFINES)
  list(APPEND _cur ${_defs})
  set_property(GLOBAL PROPERTY LVGL_BUILD_DEFINES "${_cur}")
endfunction()

# Adds compile definitions that apply when the INSTALLED lvgl is consumed.
# 1. Wraps each definition in $<INSTALL_INTERFACE:...> on the lvgl target
#  (delivered to CMake consumers via the exported target)
# 2. Stores the defines so they can be consumed by pkg-config
#
# Usage: lvgl_install_definitions([PUBLIC|PRIVATE|INTERFACE] <def>...)
function(lvgl_install_definitions)
  _lvgl_parse_definition_scope(${ARGN})

  get_property(_cflags GLOBAL PROPERTY LVGL_INSTALL_DEFINES)
  foreach(_def IN LISTS _defs)
    target_compile_definitions(lvgl ${_scope} $<INSTALL_INTERFACE:${_def}>)
    list(APPEND _cflags "-D${_def}")
  endforeach()
  set_property(GLOBAL PROPERTY LVGL_INSTALL_DEFINES "${_cflags}")
endfunction()
