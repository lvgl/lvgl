set(LVGL_LIBS_EXPAT_SOURCES 
    xmlparse.c
    xmlrole.c
    xmltok.c
    xmltok_impl.c
    xmltok_ns.c
)

# Create the main static library for this directory
add_library(lvgl_libs_expat STATIC ${LVGL_LIBS_EXPAT_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_expat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
