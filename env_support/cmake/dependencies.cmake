include(FetchContent)

set(FETCHCONTENT_BASE_DIR
    "${CMAKE_SOURCE_DIR}/.deps"
    CACHE PATH "Directory for fetched dependencies")

find_package(PkgConfig)

if(UNIX AND NOT PkgConfig_FOUND)
  message(
    WARNING
      "pkg-config not found - system libraries will only be resolved via find_package. "
      "Install `pkg-config` to improve dependency detection.")
endif()

if(CONFIG_LV_USE_GLTF)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/fastgltf.cmake)
endif()

if(CONFIG_LV_USE_GLTF OR LV_USE_LIBWEBP)
  include(${CMAKE_CURRENT_LIST_DIR}/dependencies/webp.cmake)
endif()
