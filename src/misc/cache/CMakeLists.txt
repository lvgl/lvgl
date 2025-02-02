set(SOURCE_FILES 
    lv_cache.c
    lv_cache_entry.c
    lv_cache_lru_rb.c
    lv_image_cache.c
    lv_image_header_cache.c
)

# Create the main static library for this directory
add_library(lvgl_cache STATIC ${SOURCE_FILES})

# Add the current directory as an include directory
target_include_directories(lvgl_cache PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
