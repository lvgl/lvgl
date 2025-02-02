set(SOURCE_FILES)

if(CONFIG_LV_USE_FS_ARDUINO_ESP_LITTLEFS)
    list(APPEND SOURCE_FILES lv_fs_arduino_esp_littlefs.cpp)
endif()

if(CONFIG_LV_USE_FS_ARDUINO_SD)
    list(APPEND SOURCE_FILES lv_fs_arduino_sd.cpp)
endif()

if(CONFIG_LV_USE_FS_ARDUINO_SD)
    list(APPEND SOURCE_FILES lv_fs_arduino_sd.cpp)
endif()

if(CONFIG_LV_USE_FS_FATFTS)
    list(APPEND SOURCE_FILES lv_fs_fatfs.c)
endif()

if(CONFIG_LV_USE_FS_LITTLEFS)
    list(APPEND SOURCE_FILES lv_fs_littlefs.c)
endif()

if(CONFIG_LV_USE_FS_MEMFS)
    list(APPEND SOURCE_FILES lv_fs_memfs.c)
endif()

if(CONFIG_LV_USE_FS_POSIX) 
    list(APPEND SOURCE_FILES lv_fs_posix.c)
endif()

if(CONFIG_LV_USE_FS_STDIO) 
    list(APPEND SOURCE_FILES lv_fs_stdio.c)
endif()

if(CONFIG_LV_USE_FS_WIN32) 
    list(APPEND SOURCE_FILES lv_fs_win32.c)
endif()

if(NOT ${SOURCE_FILES} STREQUAL "")
    # Create the main static library for this directory
    add_library(lvgl_libs_fsdrv STATIC ${SOURCE_FILES})
    
    # Add the current directory as an include directory
    target_include_directories(lvgl_libs_fsdrv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
