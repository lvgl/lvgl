set(SOURCE_FILES lv_theme.c)

# Create the main static library for this directory
add_library(lvgl_theme STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_theme_simple PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(SUBDIRS)


if(CONFIG_LV_USE_THEME_DEFAULT)
    list(APPEND SUBDIRS default)
endif()

if(CONFIG_LV_USE_THEME_MONO)
    list(APPEND SUBDIRS mono)
endif()

if(CONFIG_LV_USE_THEME_SIMPLE)
    list(APPEND SUBDIRS simple)
endif()

# Add each subdirectory and link its library
foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    target_link_libraries(lvgl_theme PRIVATE ${subdir})
endforeach()
