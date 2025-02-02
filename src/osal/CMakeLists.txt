set(SOURCE_FILES
    lv_os.c
)

if(CONFIG_LV_OS_CMSIS_RTOS2)
    list(APPEND SOURCE_FILES lv_cmsis_rtos2.c)
endif()

if(CONFIG_LV_OS_FREERTOS)
    list(APPEND SOURCE_FILES lv_freertos.c)
endif()

if(CONFIG_LV_OS_MQX)
    list(APPEND SOURCE_FILES lv_mqx.c)
endif()

if(CONFIG_LV_OS_PTHREAD)
    list(APPEND SOURCE_FILES lv_pthread.c)
endif()

if(CONFIG_LV_OS_RTTHREAD)
    list(APPEND SOURCE_FILES lv_rtthread.c)
endif()

if(CONFIG_LV_OS_SDL2)
    list(APPEND SOURCE_FILES lv_sdl2.c)
endif()

if(CONFIG_LV_OS_WINDOWS)
    list(APPEND SOURCE_FILES lv_windows.c)
endif()


# Create the main static library for this directory
add_library(lvgl_osal STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_osal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
