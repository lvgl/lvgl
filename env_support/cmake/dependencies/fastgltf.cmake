# ============================================================
# fastgltf Configuration
# ============================================================
set(CMAKE_PACKAGE_NAME "fastgltf")
set(PKG_LIB_PRIVATE "-lfastgltf")

option(LV_USE_FIND_PACKAGE_FASTGLTF "Resolve fastgltf via find_package"
       ${LV_USE_FIND_PACKAGE})
option(LV_FETCH_FASTGLTF "Fetch fastgltf from source" ${LV_FETCH_DEPENDENCIES})

if(LV_USE_FIND_PACKAGE_FASTGLTF)
  find_package(${CMAKE_PACKAGE_NAME} QUIET)
  if(fastgltf_FOUND)
    message(STATUS "lvgl: fastgltf: found via find_package")
    # Note: fastgltf doesn't have pkg-config support
    lvgl_link_packages(
      PRIVATE
      TARGETS
      fastgltf::fastgltf
      CMAKE_PACKAGE
      ${CMAKE_PACKAGE_NAME}
      PKG_LIB_PRIVATE
      ${PKG_LIB_PRIVATE})
    # Since fastgltf doesn't have pkg-config support we need to specify its
    # dependencies in order for pkg-config to work
    lvgl_add_pkg_libs_private("-lsimdjson")
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

lvgl_link_fetched(TARGETS fastgltf::fastgltf PKG_LIB_PRIVATE ${PKG_LIB_PRIVATE})
