option(LV_FETCH_LV_DEMOS "Fetch external lv_demos automatically"
       ${LV_FETCH_DEPENDENCIES})

if(NOT LV_FETCH_LV_DEMOS)
  return()
endif()

message(STATUS "lvgl: lv_demos_ext: fetching from source")

FetchContent_Declare(
  lv_demos_ext
  GIT_REPOSITORY https://github.com/lvgl/lv_demos
  GIT_TAG 3d75ca15bae4d4b58b32ab11e20f8c78e61dc5f9)

FetchContent_MakeAvailable(lv_demos_ext)
