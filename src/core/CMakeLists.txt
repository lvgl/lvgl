set(LVGL_CORE_SOURCES
    lv_group.c
    lv_obj.c
    lv_obj_class.c
    lv_obj_draw.c
    lv_obj_event.c
    lv_obj_pos.c
    lv_obj_scroll.c
    lv_obj_style.c
    lv_obj_style_gen.c
    lv_obj_tree.c
    lv_refr.c
)

if(CONFIG_LV_USE_OBJ_ID AND CONFIG_LV_USE_OBJ_ID_BUILTIN)
    list(APPEND LVGL_CORE_SOURCES lv_obj_id_builtin.c)
endif()

if(CONFIG_LV_USE_OBJ_PROPERTY)
    list(APPEND LVGL_CORE_SOURCES lv_obj_property.c)
endif()

# Create the main static library for this directory
add_library(lvgl_core STATIC ${LVGL_CORE_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
