if(ESP_PLATFORM)

    file(GLOB_RECURSE SOURCES src/*.c)

    idf_build_get_property(LV_MICROPYTHON LV_MICROPYTHON)

    if (LV_MICROPYTHON)
        idf_component_register(SRCS ${SOURCES}
                               INCLUDE_DIRS . src ../
                               REQUIRES main)
    else()
        idf_component_register(SRCS ${SOURCES}
                               INCLUDE_DIRS . src ../)
    endif()

    target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_CONF_INCLUDE_SIMPLE")

    if (CONFIG_LV_ATTRIBUTE_FAST_MEM_USE_IRAM)
        target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_ATTRIBUTE_FAST_MEM=IRAM_ATTR")
    endif()

elseif(ZEPHYR_BASE)

    if(CONFIG_LVGL)

        zephyr_include_directories(${ZEPHYR_BASE}/lib/gui/lvgl)

        target_include_directories(lvgl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

        zephyr_compile_definitions(LV_CONF_KCONFIG_EXTERNAL_INCLUDE=<autoconf.h>)

        zephyr_library()

        file(GLOB_RECURSE SOURCES src/*.c)
        zephyr_library_sources(${SOURCES})

    endif(CONFIG_LVGL)

else()

    file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.c)
    file(GLOB_RECURSE EXAMPLE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/examples/*.c)

    if(MICROPY_DIR)
        # with micropython, build lvgl as interface library
        # link chain is: lvgl_interface [lvgl] → usermod_lvgl_bindings [lv_bindings] → usermod [micropython] → firmware [micropython]
        add_library(lvgl_interface INTERFACE)
        # ${SOURCES} must NOT be given to add_library directly for some reason (won't be built)
        target_sources(lvgl_interface INTERFACE ${SOURCES})
        # Micropython builds with -Werror; we need to suppress some warnings, such as:
        #
        # /home/test/build/lv_micropython/ports/rp2/build-PICO/lv_mp.c:29316:16: error: 'lv_style_transition_dsc_t_path_xcb_callback' defined but not used [-Werror=unused-function]
        # 29316 | STATIC int32_t lv_style_transition_dsc_t_path_xcb_callback(const struct _lv_anim_t * arg0)
        #       |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        target_compile_options(lvgl_interface INTERFACE -Wno-unused-function)
    else(MICROPY_DIR)
        # without micropython, build lvgl and examples libs normally
        # default linux build uses this scope
        add_library(lvgl STATIC ${SOURCES})
        add_library(lvgl_examples STATIC ${EXAMPLE_SOURCES})

        include_directories(${CMAKE_SOURCE_DIR})

        # Lbrary and headers can be installed to system using make install
        file(GLOB LVGL_PUBLIC_HEADERS
          "${CMAKE_SOURCE_DIR}/lv_conf.h"
          "${CMAKE_SOURCE_DIR}/lvgl.h")

        if("${LIB_INSTALL_DIR}" STREQUAL "")
            set(LIB_INSTALL_DIR "lib")
        endif()
        if("${INC_INSTALL_DIR}" STREQUAL "")
            set(INC_INSTALL_DIR "include/lvgl")
        endif()

        install(DIRECTORY "${CMAKE_SOURCE_DIR}/src"
            DESTINATION "${CMAKE_INSTALL_PREFIX}/${INC_INSTALL_DIR}/"
            FILES_MATCHING
            PATTERN "*.h")

        set_target_properties(lvgl PROPERTIES
            OUTPUT_NAME lvgl
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            PUBLIC_HEADER "${LVGL_PUBLIC_HEADERS}")

        install(TARGETS lvgl
            ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
            PUBLIC_HEADER DESTINATION "${INC_INSTALL_DIR}")

    endif(MICROPY_DIR)

endif()


