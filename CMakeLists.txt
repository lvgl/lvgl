if(ESP_PLATFORM)

file(GLOB_RECURSE SOURCES src/*.c)

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS . src
                       REQUIRES main)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_CONF_INCLUDE_SIMPLE")

if (CONFIG_LV_MEM_CUSTOM)
    if (CONFIG_LV_MEM_CUSTOM_ALLOC)
        target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_MEM_CUSTOM_ALLOC=${CONFIG_LV_MEM_CUSTOM_ALLOC}")
    endif()

    if (CONFIG_LV_MEM_CUSTOM_FREE)
        target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_MEM_CUSTOM_FREE=${CONFIG_LV_MEM_CUSTOM_FREE}")
    endif()
endif()

if (CONFIG_LV_TICK_CUSTOM)
    if (CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR)
        target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_TICK_CUSTOM_SYS_TIME_EXPR=${CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR}")
    endif()
endif()

if (CONFIG_LV_USER_DATA_FREE)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_USER_DATA_FREE=${CONFIG_LV_USER_DATA_FREE}")
endif()

if (CONFIG_LV_ATTRIBUTE_FAST_MEM_USE_IRAM)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_ATTRIBUTE_FAST_MEM=IRAM_ATTR")
endif()

elseif(ZEPHYR_BASE)

if(CONFIG_LVGL)

zephyr_include_directories(${ZEPHYR_BASE}/lib/gui/lvgl)

target_include_directories(lvgl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

zephyr_compile_definitions(LV_CONF_KCONFIG_EXTERNAL_INCLUDE=<autoconf.h>)

zephyr_compile_definitions_ifdef(CONFIG_LV_MEM_CUSTOM
    LV_MEM_CUSTOM_ALLOC=${CONFIG_LV_MEM_CUSTOM_ALLOC}
    )
zephyr_compile_definitions_ifdef(CONFIG_LV_MEM_CUSTOM
    LV_MEM_CUSTOM_FREE=${CONFIG_LV_MEM_CUSTOM_FREE}
    )
zephyr_compile_definitions_ifdef(CONFIG_LV_TICK_CUSTOM
    LV_TICK_CUSTOM_SYS_TIME_EXPR=${CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR}
    )

zephyr_library()

file(GLOB_RECURSE SOURCES src/*.c)
zephyr_library_sources(${SOURCES})

endif() # CONFIG_LVGL

else()
message(FATAL_ERROR "Unknown platform.")
endif()
