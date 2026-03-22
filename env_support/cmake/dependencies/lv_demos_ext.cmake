option(LV_FETCH_LV_DEMOS "Fetch external lv_demos automatically" ON)

if(NOT LV_FETCH_LV_DEMOS)
  return()
endif()

message(STATUS "lvgl: lv_demos_ext: fetching from source")

FetchContent_Declare(lv_demos_ext
                     GIT_REPOSITORY https://github.com/lvgl/lv_demos)
FetchContent_MakeAvailable(lv_demos_ext)
