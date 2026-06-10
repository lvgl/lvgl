# Install directory defaults
if("${LIB_INSTALL_DIR}" STREQUAL "")
  set(LIB_INSTALL_DIR "lib")
endif()
if("${RUNTIME_INSTALL_DIR}" STREQUAL "")
  set(RUNTIME_INSTALL_DIR "bin")
endif()
if("${INC_INSTALL_DIR}" STREQUAL "")
  set(INC_INSTALL_DIR "include")
endif()

# Install public headers
install(DIRECTORY "${LVGL_ROOT_DIR}/include/lvgl"
        DESTINATION "${INC_INSTALL_DIR}")

# Install private headers only if required
if(CONFIG_LV_USE_PRIVATE_API)
  install(
    DIRECTORY "${LVGL_ROOT_DIR}/src/"
    DESTINATION "${INC_INSTALL_DIR}/lvgl_private"
    FILES_MATCHING
    PATTERN "*.h")

  # In the source tree, `lvgl_public.h` includes the public API via
  # "../include/lvgl/lvgl.h". When installed, `lvgl/` and `lvgl_private/` are
  # siblings under `include/`, so the path is patched to "../lvgl/lvgl.h" to
  # match the installed layout. No other changes are required to other files
  # because the `lvgl_private` folder structure is the same as the one in `src`
  # meaning that all other includes work, as long as all private header files
  # always go through `lvgl_public.h` to include public API symbols
  install(
    CODE "
	file(READ \"\${CMAKE_INSTALL_PREFIX}/${INC_INSTALL_DIR}/lvgl_private/lvgl_public.h\" content)
        string(REPLACE \"../include/lvgl/lvgl.h\" \"../lvgl/lvgl.h\" content \"\${content}\")
        file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${INC_INSTALL_DIR}/lvgl_private/lvgl_public.h\" \"\${content}\")
    ")
endif()

# When KConfig is used, copy the expanded conf header and rename it to lv_conf.h
if(LV_BUILD_USE_KCONFIG)
  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/lv_conf_expanded.h"
    DESTINATION "${INC_INSTALL_DIR}"
    RENAME lv_conf.h)
endif()

# Configure and install generated files
configure_file("${LVGL_ROOT_DIR}/lvgl.pc.in"
               "${CMAKE_CURRENT_BINARY_DIR}/lvgl.pc" @ONLY)
configure_file("${LVGL_ROOT_DIR}/lv_version.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/lv_version.h" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lvgl.pc"
        DESTINATION "share/pkgconfig/")

# Install library
set_target_properties(
  lvgl
  PROPERTIES OUTPUT_NAME lvgl
             VERSION ${LVGL_VERSION}
             SOVERSION ${LVGL_SOVERSION}
             ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

install(
  TARGETS lvgl
  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
  LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
  RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}")

# Install library thorvg
if(CONFIG_LV_USE_THORVG_INTERNAL)

  set_target_properties(
    lvgl_thorvg
    PROPERTIES OUTPUT_NAME lvgl_thorvg
               VERSION ${LVGL_VERSION}
               SOVERSION ${LVGL_SOVERSION}
               ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
               PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

  install(
    TARGETS lvgl_thorvg
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")

endif()

if(CONFIG_LV_BUILD_DEMOS)

  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demos"
    DESTINATION "${INC_INSTALL_DIR}/lvgl"
    FILES_MATCHING
    PATTERN "*.h")

  set_target_properties(
    lvgl_demos
    PROPERTIES OUTPUT_NAME lvgl_demos
               VERSION ${LVGL_VERSION}
               SOVERSION ${LVGL_SOVERSION}
               ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
               PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

  install(
    TARGETS lvgl_demos
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")

endif()

if(CONFIG_LV_BUILD_EXAMPLES)

  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
    DESTINATION "${INC_INSTALL_DIR}/lvgl"
    FILES_MATCHING
    PATTERN "*.h")

  set_target_properties(
    lvgl_examples
    PROPERTIES OUTPUT_NAME lvgl_examples
               VERSION ${LVGL_VERSION}
               SOVERSION ${LVGL_SOVERSION}
               ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
               PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

  install(
    TARGETS lvgl_examples
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    RUNTIME DESTINATION "${RUNTIME_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")

endif()
