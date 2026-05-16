option(LV_USE_FIND_PACKAGE_FASTGLTF "Resolve fastgltf via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_USE_PKG_CONFIG_FASTGLTF "Resolve fastgltf via pkg-config"
       ${LV_USE_PKG_CONFIG})
option(LV_FETCH_FASTGLTF "Fetch fastgltf from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_FASTGLTF)
  find_package(fastgltf QUIET)
  if(fastgltf_FOUND)
    message(STATUS "lvgl: fastgltf: found via find_package")
    target_link_libraries(lvgl PRIVATE fastgltf::fastgltf)
    return()
  endif()
endif()

if(LV_USE_PKG_CONFIG_FASTGLTF AND PkgConfig_FOUND)
  pkg_check_modules(fastgltf IMPORTED_TARGET QUIET fastgltf)
  if(fastgltf_FOUND)
    message(STATUS "lvgl: fastgltf: found via pkg-config")
    target_link_libraries(lvgl PRIVATE PkgConfig::fastgltf)
    return()
  endif()
endif()

if(NOT LV_FETCH_FASTGLTF)
  message(
    FATAL_ERROR
      "lvgl: fastgltf not found. Enable LV_FETCH_FASTGLTF or install it manually."
  )
endif()

message(STATUS "lvgl: fastgltf: fetching from source")

FetchContent_Declare(
  fastgltf
  GIT_REPOSITORY https://github.com/spnda/fastgltf
  GIT_TAG f04052ebd8d157c7b6e8fc3dd9f1ed83df99f3e5)

set(FASTGLTF_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(FASTGLTF_BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
set(FASTGLTF_ENABLE_DEPRECATED_EXT
    ON
    CACHE BOOL "" FORCE)
set(FASTGLTF_DIFFUSE_TRANSMISSION_SUPPORT
    ON
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(fastgltf)

target_link_libraries(lvgl PRIVATE fastgltf::fastgltf)
