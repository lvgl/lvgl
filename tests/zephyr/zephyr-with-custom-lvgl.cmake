# Let Zephyr's LVGL module build against an arbitrary LVGL revision without
# patching Zephyr.
#
# Zephyr compiles LVGL from a hand-maintained source list that ships with
# Zephyr, and its glue code expects Kconfig symbols that LVGL may have renamed
# or dropped. Rather than carrying a patch per difference, this file reconciles
# them at configure time. Pass it in with:
#
#   west build -b <board> -- -DCMAKE_PROJECT_INCLUDE=<abs path to this file>
#   west twister ... -x CMAKE_PROJECT_INCLUDE=<abs path to this file>
#
# CMake includes this right after *every* project() call. A Zephyr build has
# several (Zephyr-Kernel and Picolibc during find_package(Zephyr), then the
# application's own), and only the last one has the module targets. The guard
# below is load-bearing: it makes this file act only on that pass.
#
# Modelled on lvgl/lv_zephyr's zephyr-with-custom-lvgl.cmake.
#
# What this replaces, and what Zephyr should do instead:
#
#   1. modules/lvgl/CMakeLists.txt hardcodes LVGL's source list, which goes
#      stale whenever LVGL moves a file. Reconciled below by file name.
#      Upstream: Zephyr should track LVGL's layout, ideally by using LVGL's own
#      CMake instead of a duplicated list.
#
#   2. modules/lvgl/Kconfig forward-declares LV_ATTRIBUTE_MEM_ALIGN_SIZE as an
#      int with no default, expecting LVGL's Kconfig to supply prompt and
#      default. LVGL removed that symbol, so CONFIG_LV_ATTRIBUTE_MEM_ALIGN_SIZE
#      never reaches the build and modules/lvgl/include/lv_conf.h fails to
#      compile. Defined below with LVGL's own former default of 1.
#      Upstream: Zephyr should stop depending on a symbol LVGL does not define
#      and use LVGL's LV_ATTRIBUTE_CUSTOM_INCLUDE mechanism.
#
#   3. modules/lvgl/include/lv_conf.h still uses LV_ASSERT_HANDLER_INCLUDE,
#      which LVGL deprecated. Its #warning is fatal under twister's -Werror.
#      Switched below to the supported LV_ASSERT_USE_CUSTOM_INCLUDE pair.
#      Upstream: Zephyr should migrate lv_conf.h.
#
# The one incompatibility that cannot be fixed from here is Zephyr's
# import_kconfig() bracket bug: it runs inside find_package(Zephyr), before any
# project(), so no CMake hook reaches it. That one is still a patch, see
# tests/zephyr/patches/.

if(NOT TARGET modules__lvgl)
  return()
endif()

set(lvgl_dir "${ZEPHYR_LVGL_MODULE_DIR}")
if(NOT lvgl_dir)
  message(FATAL_ERROR
    "[custom-lvgl] ZEPHYR_LVGL_MODULE_DIR is empty, so the LVGL checkout "
    "cannot be located. Is LVGL registered as a Zephyr module?")
endif()

# Sources that LVGL deleted outright, so there is nothing to remap them to.
# Every entry needs a reason. Anything else that disappears without a
# replacement is a real incompatibility and is reported as an error rather than
# quietly dropped -- finding those is the whole point of this check.
set(LVGL_DELETED_SOURCES
    # Deleted by LVGL a73e59f6d (#10292); no CBFS Kconfig symbol remains.
    "src/libs/fsdrv/lv_fs_cbfs.c"
)

# Index every C file under LVGL's src/ by file name, so a source that moved can
# be found at its new path.
file(GLOB_RECURSE lvgl_sources_rel RELATIVE "${lvgl_dir}" "${lvgl_dir}/src/*.c")
foreach(rel IN LISTS lvgl_sources_rel)
  get_filename_component(name "${rel}" NAME)
  # '.' is legal in a CMake variable name but awkward to read; normalise it.
  string(REPLACE "." "_" key "${name}")
  list(APPEND lvgl_byname_${key} "${rel}")
endforeach()

# Replace the target's stale LVGL paths with the ones this LVGL actually has.
# Reports every action taken through lvgl_remap_actions in the parent scope.
function(lvgl_reconcile_sources target)
  get_target_property(sources ${target} SOURCES)
  if(NOT sources)
    return()
  endif()

  set(kept "")
  set(actions "")
  foreach(src IN LISTS sources)
    # Only touch paths inside the LVGL checkout. Everything else is not ours:
    # Zephyr's own glue sources are listed relative to its module directory, and
    # generated files may not exist yet.
    if(NOT IS_ABSOLUTE "${src}")
      list(APPEND kept "${src}")
      continue()
    endif()
    file(RELATIVE_PATH rel "${lvgl_dir}" "${src}")
    if(IS_ABSOLUTE "${rel}" OR rel MATCHES "^\\.\\.")
      list(APPEND kept "${src}")
      continue()
    endif()
    if(EXISTS "${src}")
      list(APPEND kept "${src}")
      continue()
    endif()

    if(rel IN_LIST LVGL_DELETED_SOURCES)
      list(APPEND actions "deleted ${rel}")
      continue()
    endif()

    get_filename_component(name "${rel}" NAME)
    string(REPLACE "." "_" key "${name}")
    set(candidates "${lvgl_byname_${key}}")
    list(LENGTH candidates count)

    if(count EQUAL 1)
      list(APPEND kept "${lvgl_dir}/${candidates}")
      list(APPEND actions "moved ${rel} -> ${candidates}")
    elseif(count EQUAL 0)
      message(FATAL_ERROR
        "[custom-lvgl] ${target}: Zephyr wants to compile '${rel}', which this "
        "LVGL does not have and which no longer exists anywhere under src/.\n"
        "If LVGL removed it deliberately, add it to LVGL_DELETED_SOURCES in "
        "tests/zephyr/zephyr-with-custom-lvgl.cmake with a reason. Otherwise "
        "this is a real incompatibility that needs fixing in LVGL.")
    else()
      string(REPLACE ";" "\n  " pretty "${candidates}")
      message(FATAL_ERROR
        "[custom-lvgl] ${target}: '${rel}' moved, but '${name}' now matches "
        "more than one file so the new path is ambiguous:\n  ${pretty}\n"
        "Resolve it explicitly in tests/zephyr/zephyr-with-custom-lvgl.cmake.")
    endif()
  endforeach()

  set_target_properties(${target} PROPERTIES SOURCES "${kept}")
  set(lvgl_remap_actions "${lvgl_remap_actions};${actions}" PARENT_SCOPE)
endfunction()

set(lvgl_remap_actions "")
lvgl_reconcile_sources(modules__lvgl)
list(REMOVE_ITEM lvgl_remap_actions "")
list(LENGTH lvgl_remap_actions lvgl_remap_count)

if(lvgl_remap_count GREATER 0)
  message(STATUS
    "[custom-lvgl] reconciled ${lvgl_remap_count} stale LVGL source path(s) in "
    "Zephyr's module list")
  foreach(action IN LISTS lvgl_remap_actions)
    message(STATUS "[custom-lvgl]   ${action}")
  endforeach()
endif()

# Recorded so the CI can surface the count in the PR report: these remaps are
# exactly what Zephyr's own source list still owes LVGL's layout.
string(REPLACE ";" "\n" lvgl_remap_report "${lvgl_remap_actions}")
file(WRITE "${CMAKE_BINARY_DIR}/lvgl_source_remap.txt" "${lvgl_remap_report}\n")

# The demo sources are listed separately, by the demos sample's own
# CMakeLists.txt, and are added to `app` *after* this file runs -- so they
# cannot be reconciled here. Check them once the directory has been processed,
# so a future move under demos/ reports itself rather than surfacing as a bare
# "Cannot find source file".
function(lvgl_check_app_sources)
  get_target_property(sources app SOURCES)
  if(NOT sources)
    return()
  endif()
  set(missing "")
  foreach(src IN LISTS sources)
    if(NOT IS_ABSOLUTE "${src}")
      continue()
    endif()
    file(RELATIVE_PATH rel "${lvgl_dir}" "${src}")
    if(IS_ABSOLUTE "${rel}" OR rel MATCHES "^\\.\\.")
      continue()
    endif()
    if(NOT EXISTS "${src}")
      list(APPEND missing "${rel}")
    endif()
  endforeach()
  if(missing)
    string(REPLACE ";" "\n  " pretty "${missing}")
    message(FATAL_ERROR
      "[custom-lvgl] the sample compiles LVGL sources that do not exist:\n"
      "  ${pretty}\n"
      "These come from the sample's own CMakeLists.txt in Zephyr, not from "
      "Zephyr's lvgl module, so they need fixing upstream in Zephyr.")
  endif()
endfunction()

cmake_language(DEFER CALL lvgl_check_app_sources)

# --- Config values Zephyr's glue needs but LVGL no longer provides -----------
# LVGL resolves its options with `#ifndef LV_X`, so a -D define supplied here is
# seen first and wins over the Kconfig value.
foreach(target modules__lvgl app)
  if(TARGET ${target})
    target_compile_definitions(${target} PRIVATE
      # (2) Zephyr's lv_conf.h does __aligned(CONFIG_LV_ATTRIBUTE_MEM_ALIGN_SIZE)
      #     and nothing defines that symbol any more. 1 is LVGL's former default.
      CONFIG_LV_ATTRIBUTE_MEM_ALIGN_SIZE=1
      # (3) Take the supported path for the assert header instead of the
      #     deprecated LV_ASSERT_HANDLER_INCLUDE that lv_conf.h sets. The
      #     suppress flag is needed too, because in lv_conf_internal.h the
      #     `#include LV_ASSERT_HANDLER_INCLUDE` sits inside the same `#if` as
      #     the deprecation #warning -- silencing the warning alone would also
      #     drop the include and with it __ASSERT_NO_MSG.
      LV_ASSERT_USE_CUSTOM_INCLUDE=1
      LV_ASSERT_CUSTOM_INCLUDE="zephyr/sys/__assert.h"
      LV_DISABLE_ASSERT_HANDLER_INCLUDE_WARNING=1
    )
  endif()
endforeach()

# Note: this mutates targets by their internal names (modules__lvgl is derived
# from the glue path modules/lvgl). Re-run a clean build (west build -p) after
# changing which files LVGL compiles.
