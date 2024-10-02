target_compile_definitions(
  lvgl PUBLIC $<$<BOOL:${LV_LVGL_H_INCLUDE_SIMPLE}>:LV_LVGL_H_INCLUDE_SIMPLE>
              $<$<BOOL:${LV_CONF_INCLUDE_SIMPLE}>:LV_CONF_INCLUDE_SIMPLE>)

# Add definition of LV_CONF_PATH only if needed
if(LV_CONF_PATH)
  target_compile_definitions(lvgl PUBLIC LV_CONF_PATH=${LV_CONF_PATH})
endif()

# Add definition of LV_CONF_SKIP only if needed
if(LV_CONF_SKIP)
  target_compile_definitions(lvgl PUBLIC LV_CONF_SKIP=1)
endif()




