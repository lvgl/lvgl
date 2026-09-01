include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/version.cmake")

############################## OPTIONS ######################################

set(LV_BUILD_CONF_PATH "" CACHE PATH
    "Use this to specify the location of and/or filename of lv_conf.h")

set(LV_BUILD_CONF_DIR "" CACHE PATH
    "Can be used to specify the include dir containing lv_conf.h, to be used in conjunction with LV_CONF_INCLUDE_SIMPLE")

option(LV_BUILD_USE_KCONFIG "Use Kconfig" OFF)

option(LV_CONF_SKIP 
       "Build without lv_conf.h, using lv_conf_internal.h defaults. Options can still be set via compiler definitions"
       OFF)

set(LV_BUILD_DEFCONFIG_PATH "" CACHE STRING
    "Supply the default Kconfig configuration - used with Kconfig. Accepts a \
\";\"-separated list of defconfigs, merged in order so later ones override earlier ones")

option(LV_BUILD_DEFCONFIG_STRICT
    "Fail if a defconfig assigns an unknown or promptless symbol, or a value \
that ends up having no effect. Off by default so that existing configurations \
keep working" OFF)

set(LV_BUILD_DOTCONFIG_PATH "" CACHE PATH
    "Path to .config path - used with Kconfig")

set(LV_BUILD_KCONFIG_ROOT ${LVGL_ROOT_DIR}/Kconfig CACHE PATH "Path to root Kconfig file")

option(LV_BUILD_SET_CONFIG_OPTS
    "Create variables from the definitions in lv_conf_internal.h"  OFF)

option(LV_BUILD_LVGL_H_SIMPLE_INCLUDE
    "Disable if the lvgl directory is located at the top-level of your project" ON)

option(LV_BUILD_LVGL_H_SYSTEM_INCLUDE
    "Enable if LVGL will be installed on the system or the build system of your application uses a sysroot" OFF)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# These control how LVGL resolves its optional dependencies.
# All three strategies are tried in order: find_package -> pkg-config -> FetchContent.
# Individual dependencies can be further controlled with LV_USE_FIND_PACKAGE_<DEP>,
# LV_USE_PKG_CONFIG_<DEP>, and LV_FETCH_<DEP> options.

# Dependency management require CONFIG_LV_* variables to be set (e.g. CONFIG_LV_USE_WAYLAND).
# CONFIG_LV_* variables can be set manually using the -D argument or cmake-gui and are kept in cache  
# or generated automatically by enabling LV_BUILD_SET_CONFIG_OPTS, which parses lv_conf_internal.h to derive them.
option(LV_USE_FIND_PACKAGE "Resolve dependencies via find_package" ON)
option(LV_USE_PKG_CONFIG "Resolve dependencies via pkg-config (requires pkg-config to be installed)" ON)
option(LV_FETCH_DEPENDENCIES "Fetch dependencies from source if not found on the system" ON)

option(LV_BUILD_TESTS "Build the unit tests in the tests directory" OFF)

option(CONFIG_LV_BUILD_DEMOS "Build demos" ON)
option(CONFIG_LV_BUILD_EXAMPLES "Build examples" ON)
option(CONFIG_LV_USE_THORVG "Enable ThorVG" ON)
option(CONFIG_LV_USE_THORVG_INTERNAL "Use the internal version of ThorVG" ON)
option(CONFIG_LV_USE_PRIVATE_API "If set - install the private headers" OFF)

############################## BUILD ######################################

if (LV_BUILD_CONF_PATH AND LV_BUILD_CONF_DIR)
    message(FATAL_ERROR "can not use LV_BUILD_CONF_DIR and LV_BUILD_CONF_PATH at the same time")
endif()

if (LV_CONF_SKIP AND (LV_BUILD_USE_KCONFIG OR LV_BUILD_SET_CONFIG_OPTS OR LV_BUILD_TESTS))
    message(FATAL_ERROR "can not use kconfig (LV_BUILD_USE_KCONFIG) nor enable LV_BUILD_SET_CONFIG_OPTS with LV_CONF_SKIP enabled")
endif()

if (LV_BUILD_TESTS)
    if (NOT EXISTS ${LVGL_ROOT_DIR}/tests/CMakeLists.txt)
        message(FATAL_ERROR "LV_BUILD_TESTS is set but the tests directory is missing")
    endif()

    # Every test configuration is a set of Kconfig defconfigs, see
    # tests/configs and the recipes in tests/main.py
    set(LV_BUILD_USE_KCONFIG ON)
    set(LV_BUILD_DEFCONFIG_STRICT ON)

    set(CMAKE_C_STANDARD 99)
    set(CMAKE_CXX_STANDARD 14)

    # Lets `ctest -T memcheck` work. include(CTest) reads this, so it has to be
    # found here rather than in tests/CMakeLists.txt.
    find_program(VALGRIND_EXECUTABLE valgrind)
    if (VALGRIND_EXECUTABLE)
        set(MEMORYCHECK_COMMAND ${VALGRIND_EXECUTABLE})
        set(MEMORYCHECK_COMMAND_OPTIONS --error-exitcode=1)
    endif()

    enable_testing()
    include(CTest)
endif()

if (LV_BUILD_USE_KCONFIG)
    # If Kconfig is used then the configuration has to be expanded
    set(LV_BUILD_SET_CONFIG_OPTS ON)
endif()

if (LV_BUILD_SET_CONFIG_OPTS)
    # Python is required when LV_BUILD_SET_CONFIG_OPTS is enabled
    # lv_conf_internal.h is pre-processed to avoid setting CONFIG_LV_USE
    # or CONFIG_LV_BUILD options manually
    find_package(Python REQUIRED)
endif()

# Determine if LVGL is built from a top-level directory
get_directory_property(HAS_PARENT_SCOPE PARENT_DIRECTORY)

set(LVGL_INCLUDE_DIR ${LVGL_ROOT_DIR}/include)

# Set sources used for LVGL components
file(GLOB_RECURSE SOURCES ${LVGL_ROOT_DIR}/src/*.c
                          ${LVGL_ROOT_DIR}/src/*.cpp
                          ${LVGL_ROOT_DIR}/src/*.S)
file(GLOB_RECURSE EXAMPLE_SOURCES ${LVGL_ROOT_DIR}/examples/*.c)
file(GLOB_RECURSE DEMO_SOURCES ${LVGL_ROOT_DIR}/demos/*.c)
file(GLOB_RECURSE THORVG_SOURCES ${LVGL_ROOT_DIR}/src/libs/thorvg/*.cpp
                                 ${LVGL_ROOT_DIR}/src/others/vg_lite_tvg/*.cpp)

# Remove the ThorVG sources from the main sources
list(REMOVE_ITEM SOURCES ${THORVG_SOURCES})

# Check for the case where LVGL is distributed with only its source code
if(NOT EXAMPLE_SOURCES AND CONFIG_LV_BUILD_EXAMPLES)
    message(STATUS "No Examples found. Disabling Examples build")
    set(CONFIG_LV_BUILD_EXAMPLES OFF)
endif()

if(NOT DEMO_SOURCES AND CONFIG_LV_BUILD_DEMOS)
    message(STATUS "No Demos found. Disabling Demos build")
    set(CONFIG_LV_BUILD_DEMOS OFF)
endif()

# Build LVGL library
add_library(lvgl ${SOURCES})
add_library(lvgl::lvgl ALIAS lvgl)

set(CONF_PATH)
include(${CMAKE_CURRENT_LIST_DIR}/lvgl_target_definitions.cmake)

if(LV_CONF_SKIP)
    # no config file
    lvgl_build_definitions(LV_CONF_SKIP)
    lvgl_install_definitions(LV_CONF_SKIP)
elseif(LV_BUILD_USE_KCONFIG)
    # Use a .config, a defconfig or multiple .config fragments
    # files by generating a header file from them

    include(${CMAKE_CURRENT_LIST_DIR}/kconfig.cmake)
    # If using Kconfig, we need to define additional build definitions
    lvgl_build_definitions(
      LV_CONF_SKIP
      "LV_CONF_KCONFIG_EXTERNAL_INCLUDE=\"${KCONFIG_EXTERNAL_INCLUDE}\"")
else()
    # lv_conf.h
    lvgl_build_definitions(LV_KCONFIG_IGNORE)
    lvgl_install_definitions(LV_KCONFIG_IGNORE)

    message(STATUS ${LV_BUILD_CONF_PATH})

    if (LV_BUILD_CONF_PATH)

        set(CONF_PATH ${LV_BUILD_CONF_PATH})
        message(STATUS "Using configuration: ${CONF_PATH}")

        if (NOT IS_ABSOLUTE ${CONF_PATH})
            file(REAL_PATH ${CONF_PATH} CONF_PATH BASE_DIRECTORY ${CMAKE_SOURCE_DIR})
            message(STATUS "Converted to absolute path: ${CONF_PATH}")
        endif()

        get_filename_component(CONF_INC_DIR ${CONF_PATH} DIRECTORY)

        lvgl_build_definitions("LV_CONF_PATH=\"${CONF_PATH}\"")

    elseif(LV_BUILD_CONF_DIR)

        message(STATUS "Using lv_conf.h from the user-supplied configuration directory: ${LV_BUILD_CONF_DIR}")

        set(CONF_INC_DIR ${LV_BUILD_CONF_DIR})

        # Avoid compilation errors due to the relative path of the include directory
        if (NOT IS_ABSOLUTE ${CONF_INC_DIR})
            file(REAL_PATH ${CONF_INC_DIR} CONF_INC_DIR BASE_DIRECTORY ${CMAKE_SOURCE_DIR})
            message(STATUS "Converted to absolute path: ${CONF_INC_DIR}")
        endif()

        set(CONF_PATH ${CONF_INC_DIR}/lv_conf.h)

        lvgl_build_definitions(LV_CONF_INCLUDE_SIMPLE)

    else()

        message(STATUS "Using lv_conf.h from the top-level project directory")

        set(CONF_INC_DIR ${CMAKE_SOURCE_DIR})
        set(CONF_PATH ${CONF_INC_DIR}/lv_conf.h)

        lvgl_build_definitions(LV_CONF_INCLUDE_SIMPLE)

    endif()

    # Check if lv_conf.h exists at the configured location
    if (NOT EXISTS ${CONF_PATH})
        message(FATAL_ERROR "Configuration file: ${CONF_PATH} - not found")
    endif()
endif()


if (LV_BUILD_LVGL_H_SYSTEM_INCLUDE)
    lvgl_build_definitions(LV_LVGL_H_INCLUDE_SYSTEM)
elseif(LV_BUILD_LVGL_H_SIMPLE_INCLUDE)
    lvgl_build_definitions(LV_LVGL_H_INCLUDE_SIMPLE)
endif()


if (LV_BUILD_SET_CONFIG_OPTS)
    # Use the C compiler of the build to preprocess lv_conf_internal.h

    # Use the definitions active during the build (recorded by
    # lvgl_build_definitions). We can't use get_target_property(...
    # COMPILE_DEFINITIONS) here because it returns raw generator-expression
    # syntax (e.g. "$<BUILD_INTERFACE:LV_CONF_SKIP>") that the preprocessor
    # can't parse.
    get_property(CONF_DEFINES GLOBAL PROPERTY LVGL_BUILD_DEFINES)

    execute_process(
        COMMAND ${Python_EXECUTABLE} ${LVGL_ROOT_DIR}/scripts/build-tools/preprocess_lv_conf_internal.py
        --input ${LVGL_INCLUDE_DIR}/lvgl/config/lv_conf_internal.h
        --tmp_file ${CMAKE_CURRENT_BINARY_DIR}/tmp.h
        --output ${CMAKE_CURRENT_BINARY_DIR}/lv_conf_expanded.h
        --cc ${CMAKE_C_COMPILER}
        --defs ${CONF_DEFINES}
        --include ${LVGL_ROOT_DIR} ${CMAKE_SOURCE_DIR} ${LVGL_ROOT_DIR}/src ${CONF_INC_DIR}
        RESULT_VARIABLE ret
    )
    if(NOT "${ret}" STREQUAL "0")
        message(FATAL_ERROR "preprocess_lv_conf_internal.py failed with return code: ${ret}")
    endif()

    if(HAS_PARENT_SCOPE)
        # Also set the CONFIG_LV_USE_* and CONFIG_LV_BUILD_* configuration
        # on the parent scope
        set(PARENT_SCOPE_ARG "--parentscope")
    else()
        set(PARENT_SCOPE_ARG "")
    endif()

    set(GEN_VARS_ARGS "")
    if(LV_BUILD_USE_KCONFIG)
        # Tell generate_cmake_variables.py to expect a Kconfig based config
        list(APPEND GEN_VARS_ARGS --kconfig)
    endif()

    # Convert the expanded lv_conf_expanded.h to cmake variables
    execute_process(
        COMMAND ${Python_EXECUTABLE}
        ${LVGL_ROOT_DIR}/scripts/generate_cmake_variables.py
        --input ${CMAKE_CURRENT_BINARY_DIR}/lv_conf_expanded.h
        --output ${CMAKE_CURRENT_BINARY_DIR}/lv_conf.cmake
        ${PARENT_SCOPE_ARG}
        ${GEN_VARS_ARGS}
        RESULT_VARIABLE ret
    )

    if(NOT "${ret}" STREQUAL "0")
        message(FATAL_ERROR "generate_cmake_variables.py command failed with return code: ${ret}")
    endif()

    # This will set all CONFIG_LV_USE_* or CONFIG_LV_BUILD_* variables in cmake
    include(${CMAKE_CURRENT_BINARY_DIR}/lv_conf.cmake)

    # Regen lv_conf.cmake when lv_conf.h changes
    if(CONF_PATH)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CONF_PATH})
    endif()

    if(NOT CONFIG_LV_USE_PRIVATE_API AND (CONFIG_LV_BUILD_DEMOS OR CONFIG_LV_BUILD_EXAMPLES))
        message(STATUS "CONFIG_LV_USE_PRIVATE_API forcefully enabled because demos or examples are being built")
        set(CONFIG_LV_USE_PRIVATE_API ON CACHE BOOL "" FORCE)
    endif()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/lvgl_link_libraries.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dependencies.cmake)

# Set the configuration inc dir for all targets created in this CMakeLists.txt
# CMAKE_CURRENT_SOURCE_DIR is necessary because the assets include lvgl/lvgl.h ...
include_directories(${CONF_INC_DIR} ${LVGL_INCLUDE_DIR} ${LVGL_INCLUDE_DIR}/lvgl)

target_include_directories(lvgl SYSTEM PUBLIC
    $<BUILD_INTERFACE:${LVGL_ROOT_DIR}>
    $<BUILD_INTERFACE:${LVGL_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CONF_INC_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Propagate the compiler definitions set on LVGL to the rest of the targets
# mentioned in this file
get_target_property(COMP_DEF lvgl COMPILE_DEFINITIONS)

set(LV_BUILD_THORVG_INTERNAL OFF)
if(CONFIG_LV_USE_THORVG AND CONFIG_LV_USE_THORVG_INTERNAL)
    set(LV_BUILD_THORVG_INTERNAL ON)
endif()

if(LV_BUILD_THORVG_INTERNAL)
    message(STATUS "Enabling the building of ThorVG internal")

    add_library(lvgl_thorvg ${THORVG_SOURCES})
    add_library(lvgl::thorvg ALIAS lvgl_thorvg)
    target_include_directories(lvgl_thorvg PRIVATE ${LVGL_ROOT_DIR}/src/libs/thorvg)
    set_target_properties(lvgl_thorvg PROPERTIES COMPILE_DEFINITIONS "${COMP_DEF}")

    # This tells cmake to link lvgl with lvgl_thorvg
    # The linker will resolve all dependencies when dynamic linking 
    target_link_libraries(lvgl PRIVATE lvgl_thorvg)

    # export lvgl_thorvg as a private library to pkg-config
    get_property(current_libs GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE)
    list(APPEND current_libs "-llvgl_thorvg")
    set_property(GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE "${current_libs}")

    # During static linking, we need to create a cyclic dependency as thorvg also needs lvgl
    if (NOT BUILD_SHARED_LIBS)
        target_link_libraries(lvgl_thorvg PRIVATE lvgl)
    endif()
endif()

if(NOT (CMAKE_C_COMPILER_ID STREQUAL "MSVC"))
    set_source_files_properties(${LVGL_ROOT_DIR}/src/others/vg_lite_tvg/vg_lite_tvg.cpp
        PROPERTIES COMPILE_FLAGS -Wunused-parameter)
endif()

# Build LVGL example library
if(CONFIG_LV_BUILD_EXAMPLES)

    message(STATUS "Enabling the building of examples")

    add_library(lvgl_examples ${EXAMPLE_SOURCES})
    add_library(lvgl::examples ALIAS lvgl_examples)
    target_include_directories(lvgl_examples SYSTEM PUBLIC
        $<BUILD_INTERFACE:${LVGL_ROOT_DIR}/examples>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    set_target_properties(lvgl_examples PROPERTIES COMPILE_DEFINITIONS "${COMP_DEF}")
    target_link_libraries(lvgl_examples PRIVATE lvgl)

endif()

# Build LVGL demos library
if(CONFIG_LV_BUILD_DEMOS)

    message(STATUS "Enabling the building of demos")

    add_library(lvgl_demos ${DEMO_SOURCES})
    add_library(lvgl::demos ALIAS lvgl_demos)
    target_include_directories(lvgl_demos SYSTEM PUBLIC
        $<BUILD_INTERFACE:${LVGL_ROOT_DIR}/demos>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    set_target_properties(lvgl_demos PROPERTIES COMPILE_DEFINITIONS "${COMP_DEF}")
    target_link_libraries(lvgl_demos PRIVATE lvgl)

endif()

############################## INSTALLATION ######################################


# Install public headers
install(
    DIRECTORY "${LVGL_ROOT_DIR}/include/lvgl"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# Install private headers only if required
if(CONFIG_LV_USE_PRIVATE_API)
    install(
        DIRECTORY "${LVGL_ROOT_DIR}/src/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lvgl_private"
        FILES_MATCHING PATTERN "*.h"
    )

    install(CODE "
        file(READ \"${CMAKE_INSTALL_FULL_INCLUDEDIR}/lvgl_private/lvgl_public.h\" content)
        string(REPLACE \"../include/lvgl/lvgl.h\" \"../lvgl/lvgl.h\" content \"\${content}\")
        file(WRITE \"${CMAKE_INSTALL_FULL_INCLUDEDIR}/lvgl_private/lvgl_public.h\" \"\${content}\")
    ")
endif()

# Install lv_conf.h inside lvgl/config so its next to lv_conf_internal.h
# and define LV_CONF_INCLUDE_SIMPLE so that it can be found
lvgl_install_definitions(LV_CONF_INCLUDE_SIMPLE)
if(LV_BUILD_USE_KCONFIG)
    # Kconfig: install the expanded configuration header
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/lv_conf_expanded.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lvgl/config"
        RENAME lv_conf.h)
elseif(NOT LV_CONF_SKIP)
    # Non-kconfig: install the actual lv_conf.h used during the build
    install(
        FILES "${CONF_PATH}"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lvgl/config"
        RENAME lv_conf.h)
endif()

# Collect accumulated dependencies
get_property(PKG_REQUIRES GLOBAL PROPERTY LVGL_PKG_REQUIRES)
get_property(PKG_REQUIRES_PRIVATE GLOBAL PROPERTY LVGL_PKG_REQUIRES_PRIVATE)
get_property(PKG_LIBS_PRIVATE GLOBAL PROPERTY LVGL_PKG_LIBS_PRIVATE)
get_property(CMAKE_PUBLIC_DEPS GLOBAL PROPERTY LVGL_CMAKE_PUBLIC_DEPS)
get_property(CMAKE_PRIVATE_DEPS GLOBAL PROPERTY LVGL_CMAKE_PRIVATE_DEPS)
get_property(CMAKE_RAW_LIBS GLOBAL PROPERTY LVGL_CMAKE_RAW_LIBS)
get_property(PKG_CFLAGS GLOBAL PROPERTY LVGL_INSTALL_DEFINES)

list(JOIN PKG_CFLAGS " " LVGL_PKG_CFLAGS)
list(JOIN PKG_LIBS_PRIVATE " " LVGL_PKG_LIBS_PRIVATE)
list(JOIN PKG_REQUIRES " " LVGL_PKG_REQUIRES)
list(JOIN PKG_REQUIRES_PRIVATE " " LVGL_PKG_REQUIRES_PRIVATE)
list(JOIN CMAKE_PUBLIC_DEPS ";" LVGL_CMAKE_PUBLIC_DEPS)
list(JOIN CMAKE_PRIVATE_DEPS ";" LVGL_CMAKE_PRIVATE_DEPS)
list(JOIN CMAKE_RAW_LIBS ";" LVGL_CMAKE_RAW_LIBS)

if(LVGL_CMAKE_RAW_LIBS)
	set_property(TARGET lvgl APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${LVGL_CMAKE_RAW_LIBS}")
endif()

# Generate and install pkg-config file
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/lvgl.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lvgl.pc"
    @ONLY
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/lvgl.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)

# Generate and install CMake config files
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/lvglConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lvglConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lvgl"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/lvglConfigVersion.cmake"
    VERSION ${LVGL_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/lvglConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/lvglConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lvgl"
)

install(TARGETS lvgl
    EXPORT lvglTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT lvglTargets
    NAMESPACE lvgl::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lvgl"
)

# Install library thorvg
if(LV_BUILD_THORVG_INTERNAL)

    set_target_properties(
        lvgl_thorvg
        PROPERTIES OUTPUT_NAME lvgl_thorvg
        VERSION ${LVGL_VERSION}
        SOVERSION ${LVGL_SOVERSION}
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

    install(TARGETS lvgl_thorvg
        EXPORT lvglTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

endif()

if(CONFIG_LV_BUILD_DEMOS)

    # Install demo headers (only if user enables demos)
    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demos"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lvgl"
        FILES_MATCHING PATTERN "*.h"
    )

    # Demo library target (if you build one)
    install(TARGETS lvgl_demos
	EXPORT lvglDemosTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(EXPORT lvglDemosTargets
        NAMESPACE lvgl::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lvgl"
    )

endif()

if(CONFIG_LV_BUILD_EXAMPLES)

    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lvgl"
        FILES_MATCHING PATTERN "*.h"
    )

    install(TARGETS lvgl_examples
	EXPORT lvglExamplesTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(EXPORT lvglExamplesTargets
        NAMESPACE lvgl::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/lvgl"
    )

endif()

############################## SET GLOBAL VARIABLES ######################################

if (HAS_PARENT_SCOPE)
    # Set the configuration include directory on the top-level project
    # Useful so that it doesn't need to be determined a second time in the project itself
    set(LVGL_CONF_INC_DIR ${CONF_INC_DIR} PARENT_SCOPE)
    set(LVGL_CONF_PATH ${CONF_PATH} PARENT_SCOPE)
    set(LVGL_COMPILER_DEFINES ${COMP_DEF} PARENT_SCOPE)

else()
    # Building LVGL as the top-level project, set variable containing the
    # normalized path of the include dir containing the configuration file
    set(LVGL_CONF_INC_DIR ${CONF_INC_DIR})
    set(LVGL_CONF_PATH ${CONF_PATH})
    set(LVGL_COMPILER_DEFINES ${COMP_DEF})
endif()

############################## TESTS ######################################

# Added last so that the tests can adjust the targets created above, e.g. to
# compile LVGL itself with the sanitizers and the strict warning set.
if (LV_BUILD_TESTS)
    add_subdirectory(${LVGL_ROOT_DIR}/tests tests)
endif()
