include(FetchContent)

set(FETCHCONTENT_BASE_DIR
    "${CMAKE_SOURCE_DIR}/.deps"
    CACHE PATH "Directory for fetched dependencies" FORCE)

find_package(PkgConfig)

if(UNIX AND NOT PkgConfig_FOUND)
  message(
    WARNING
      "pkg-config not found - system libraries will only be resolved via find_package. "
      "Install `pkg-config` to improve dependency detection.")
endif()
