if(ESP_PLATFORM)

###################################
# Tests do not build for ESP-IDF. #
###################################

else()

cmake_minimum_required(VERSION 3.16)

if(WIN32)
    set(VCPKG_TARGET_TRIPLET "x64-windows")
    include($ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake)
endif()

project(lvgl_tests LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

set(FLAG_CHECK_WHITELIST --coverage -fsanitize=address -Werror)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
function(filter_compiler_options lang options_out)
    set(options ${ARGN})
    foreach(option ${options})
        if (option IN_LIST FLAG_CHECK_WHITELIST)
            list(APPEND ${options_out} ${option})
            continue()
        endif ()

        string(TOUPPER FLAG_SUPPORTED_FOR_${lang}_${option} option_var_name)
        string(REPLACE "-" "_" option_var_name ${option_var_name})
        string(REPLACE "+" "_" option_var_name ${option_var_name})

        if (${lang} STREQUAL C)
            check_c_compiler_flag(${option} ${option_var_name})
        elseif (${lang} STREQUAL CXX)
            check_cxx_compiler_flag(${option} ${option_var_name})
        else()
            message(FATAL_ERROR "Unknown language ${lang}")
        endif ()
        if(${option_var_name})
            list(APPEND ${options_out} ${option})
        endif()
    endforeach()
    set(${options_out} ${${options_out}} PARENT_SCOPE)
endfunction()

find_program(VALGRIND_EXECUTABLE valgrind)
if (VALGRIND_EXECUTABLE)
    set(MEMORYCHECK_COMMAND ${VALGRIND_EXECUTABLE})
    set(MEMORYCHECK_COMMAND_OPTIONS --error-exitcode=1)
endif()

include(CTest)

set(LVGL_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(SANITIZE_AND_COVERAGE_OPTIONS
    -fsanitize=address
    -fsanitize=leak
    -fsanitize=undefined
    --coverage
)

set(LVGL_TEST_OPTIONS_VG_LITE
    -DLV_TEST_OPTION=6
    -Wno-dangling-pointer # workaround for thorvg dangling-pointer warning
)

set(LVGL_TEST_OPTIONS_SDL
    -DLV_TEST_OPTION=7
)

set(LVGL_TEST_OPTIONS_MINIMAL_MONOCHROME
    -DLV_TEST_OPTION=1
)

set(LVGL_TEST_OPTIONS_NORMAL_8BIT
    -DLV_TEST_OPTION=2
)

set(LVGL_TEST_OPTIONS_16BIT
    -DLV_TEST_OPTION=3
)

set(LVGL_TEST_OPTIONS_FULL_24BIT
    -DLV_TEST_OPTION=4
)

set(LVGL_TEST_OPTIONS_FULL_32BIT
    -DLV_TEST_OPTION=5
)

set(LVGL_TEST_OPTIONS_TEST_SYSHEAP
    -DLV_TEST_OPTION=5
    -DLVGL_CI_USING_SYS_HEAP
    -Wno-unused-but-set-variable # unused variables are common in the dual-heap arrangement
    ${SANITIZE_AND_COVERAGE_OPTIONS}
)

set(LVGL_TEST_OPTIONS_TEST_DEFHEAP
    -DLV_TEST_OPTION=5
    -DLV_USE_OBJ_PROPERTY=1      # add obj property test and disable pedantic
    -DLV_USE_OBJ_PROPERTY_NAME=1
    -DLVGL_CI_USING_DEF_HEAP
    ${SANITIZE_AND_COVERAGE_OPTIONS}
)

if (OPTIONS_VG_LITE)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_VG_LITE})
elseif (OPTIONS_SDL)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_SDL})
elseif (OPTIONS_NORMAL_8BIT)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_NORMAL_8BIT})
elseif (OPTIONS_16BIT)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_16BIT})
elseif (OPTIONS_24BIT)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_24BIT})
elseif (OPTIONS_FULL_32BIT)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_FULL_32BIT})
elseif (OPTIONS_TEST_SYSHEAP)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_TEST_SYSHEAP})
    filter_compiler_options (C TEST_LIBS ${SANITIZE_AND_COVERAGE_OPTIONS})
    set (CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "disable examples" FORCE)
    set (ENABLE_TESTS ON)
    add_definitions(-DREF_IMGS_PATH="ref_imgs/")
elseif (OPTIONS_TEST_DEFHEAP)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_TEST_DEFHEAP})
    filter_compiler_options (C TEST_LIBS ${SANITIZE_AND_COVERAGE_OPTIONS})
    set (CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "disable examples" FORCE)
    set (ENABLE_TESTS ON)
    add_definitions(-DREF_IMGS_PATH="ref_imgs/")
elseif (OPTIONS_TEST_MEMORYCHECK)
    # sanitizer is disabled because valgrind uses LD_PRELOAD and the
    # sanitizer lib needs to load first
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_TEST_SYSHEAP})
    set (CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "disable examples" FORCE)
    set (ENABLE_TESTS ON)
    add_definitions(-DREF_IMGS_PATH="ref_imgs/")
elseif (OPTIONS_TEST_VG_LITE)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_VG_LITE} -DLVGL_CI_USING_SYS_HEAP ${SANITIZE_AND_COVERAGE_OPTIONS})
    filter_compiler_options (C TEST_LIBS ${SANITIZE_AND_COVERAGE_OPTIONS})
    set (CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "disable examples" FORCE)
    set (ENABLE_TESTS ON)
    add_definitions(-DREF_IMGS_PATH="ref_imgs_vg_lite/")

    # In 32-bit systems, the output of ThorVG's anti-aliasing algorithm has a slight deviation.
    if ($ENV{NON_AMD64_BUILD})
        # Set a tolerance value for the VG-Lite tests.
        add_definitions(-DREF_IMG_TOLERANCE=9)
    endif()
else()
    message(FATAL_ERROR "Must provide a known options value (check main.py?).")
endif()

if ($ENV{NON_AMD64_BUILD})
    set(BUILD_TARGET_DEF -DNON_AMD64_BUILD)
    message("Non AMD64 target is specified")

    # Ensure 32-bit build
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
endif()

if ($ENV{NON_AMD64_BUILD})
    set(CMAKE_LIBRARY_PATH "/usr/lib/i386-linux-gnu" CACHE PATH "search 32bit lib path firstly")
endif()

# Options lvgl and examples are compiled with.
set(COMPILE_OPTIONS
    ${BUILD_OPTIONS}
    ${BUILD_TARGET_DEF}
)

if(NOT (CMAKE_C_COMPILER_ID STREQUAL "MSVC"))
    list(APPEND COMPILE_OPTIONS
        -pedantic-errors
        -Wall
        -Wformat-security
        -Wclobbered
        -Wdeprecated
        -Wdouble-promotion
        -Wempty-body
        -Werror
        -Wextra
        -Wformat-security
        -Wmaybe-uninitialized
        -Wmissing-prototypes
        -Wpointer-arith
        -Wmultichar
        -Wpedantic
        -Wreturn-type
        -Wshadow
        -Wshift-negative-value
        -Wsizeof-pointer-memaccess
        #-Wstack-usage=6000
        -Wtype-limits
        -Wundef
        -Wuninitialized
        -Wunreachable-code
        -Werror=float-conversion
        -Werror=strict-aliasing
        -Wno-double-promotion
        -Wno-unreachable-code
        -Wno-gnu-zero-variadic-macro-arguments
        -Wunused-function
	-Wno-overlength-strings
    )
else()
    list(APPEND COMPILE_OPTIONS
        /W3         # enable all "production quality" warnings
        /we4013     # treat function undefined as error
    )
endif()

list(APPEND COMPILE_OPTIONS
    ${BUILD_OPTIONS}
    ${BUILD_TARGET_DEF}
)

# LV_USE_OBJ_PROPERTY is enabled in option OPTIONS_TEST_DEFHEAP
# It relies on C11 anonymous struct/union support, thus disable warnings.
if(OPTIONS_TEST_DEFHEAP)
    list(REMOVE_ITEM COMPILE_OPTIONS -pedantic-errors -Wpedantic)
endif()

filter_compiler_options(C LVGL_C_COMPILE_OPTIONS ${COMPILE_OPTIONS})

if(NOT (CMAKE_C_COMPILER_ID STREQUAL "MSVC"))
    # Options test cases are compiled with.
    filter_compiler_options(C LVGL_TESTFILE_COMPILE_OPTIONS ${LVGL_C_COMPILE_OPTIONS} -Wno-missing-prototypes)

    filter_compiler_options(CXX LVGL_CXX_COMPILE_OPTIONS
        ${COMPILE_OPTIONS}
        -Wno-shadow
        -Wno-unused-parameter
        -Wno-c++11-extensions
        -Wno-missing-prototypes
        -Wno-deprecated-copy-with-user-provided-dtor
        -Wno-float-conversion
        -Wno-pedantic
    )
else()
    # Options test cases are compiled with.
    filter_compiler_options(C LVGL_TESTFILE_COMPILE_OPTIONS ${LVGL_C_COMPILE_OPTIONS})

    filter_compiler_options(CXX LVGL_CXX_COMPILE_OPTIONS
        ${COMPILE_OPTIONS}
    )
endif()

get_filename_component(LVGL_DIR ${LVGL_TEST_DIR} DIRECTORY)

# Include lvgl project file.
# Set LV_BUILD_CONF_PATH so os_desktop.cmake knows about it before compiling lvgl
# Used to preprocess lv_conf_internal.h with the test config
set(LV_BUILD_CONF_PATH "${LVGL_TEST_DIR}/src/lv_test_conf.h" CACHE STRING "set test configuration")

include(${LVGL_DIR}/CMakeLists.txt)
target_compile_options(lvgl PUBLIC $<$<COMPILE_LANGUAGE:C>: ${LVGL_C_COMPILE_OPTIONS}>)
target_compile_options(lvgl PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${LVGL_CXX_COMPILE_OPTIONS}>)
target_compile_definitions(lvgl PUBLIC LV_BUILD_TEST)
if(NOT (CMAKE_C_COMPILER_ID STREQUAL "MSVC"))
    target_compile_options(lvgl PUBLIC $<$<COMPILE_LANGUAGE:ASM>:${LVGL_C_COMPILE_OPTIONS}>)
endif()

if (TARGET lvgl_thorvg)
    target_compile_options(lvgl_thorvg PUBLIC $<$<COMPILE_LANGUAGE:C>:${LVGL_C_COMPILE_OPTIONS}>)
    target_compile_options(lvgl_thorvg PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${LVGL_CXX_COMPILE_OPTIONS}>)
endif()

if (TARGET lvgl_examples)
    target_compile_options(lvgl_examples PUBLIC $<$<COMPILE_LANGUAGE:C>:${LVGL_C_COMPILE_OPTIONS}>)
    target_compile_options(lvgl_examples PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${LVGL_CXX_COMPILE_OPTIONS}>)
endif()

if (TARGET lvgl_demos)
    target_compile_options(lvgl_demos PUBLIC $<$<COMPILE_LANGUAGE:C>:${LVGL_C_COMPILE_OPTIONS}>)
    target_compile_options(lvgl_demos PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${LVGL_CXX_COMPILE_OPTIONS}>)
endif()


# LVGL_CONF_DIR and LVGL_CONF_PATH are normalized and set by os_desktop.cmake

set(TEST_INCLUDE_DIRS
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}/src>
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}/unity>
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}>
    $<BUILD_INTERFACE:${LVGL_CONF_DIR}>
)

file(GLOB_RECURSE TEST_IMAGES_SRC ${LVGL_TEST_DIR}/test_images/*.c)

add_library(test_common
    STATIC
        src/lv_test_init.c
        src/test_assets/test_animimg001.c
        src/test_assets/test_animimg002.c
        src/test_assets/test_animimg003.c
        src/test_assets/test_img_cogwheel_i4.c
        src/test_assets/test_img_cogwheel_a8.c
        src/test_assets/test_img_cogwheel_rgb565.c
        src/test_assets/test_img_cogwheel_rgb565a8.c
        src/test_assets/test_img_cogwheel_xrgb8888.c
        src/test_assets/test_img_cogwheel_argb8888.c
        src/test_assets/test_img_svg.c
        src/test_assets/test_font_1.c
        src/test_assets/test_font_2.c
        src/test_assets/test_font_3.c
        src/test_assets/test_font_montserrat_ascii_1bpp.c
        src/test_assets/test_font_montserrat_ascii_2bpp.c
        src/test_assets/test_font_montserrat_ascii_4bpp.c
        src/test_assets/test_font_montserrat_ascii_8bpp.c
        src/test_assets/test_font_montserrat_ascii_3bpp_compressed.c
        src/test_assets/test_font_1_bin.c
        src/test_assets/test_font_2_bin.c
        src/test_assets/test_font_3_bin.c
        src/test_assets/test_img_caret_down.c
        src/test_assets/test_arc_bg.c
        src/test_assets/test_img_lvgl_logo_png.c
        src/test_assets/test_img_lvgl_logo_jpg.c
        src/test_assets/test_img_emoji_F617.c
        src/test_assets/test_ubuntu_font.c
        src/test_assets/test_kern_one_otf.c
        src/test_assets/test_imagebutton_left.c
        src/test_assets/test_imagebutton_mid.c
        src/test_assets/test_imagebutton_right.c
        src/test_assets/test_music_button_play.c
        src/test_assets/test_lottie_approve.c
        unity/unity.c
        ${TEST_IMAGES_SRC}
)
target_include_directories(test_common PUBLIC ${TEST_INCLUDE_DIRS})
target_compile_options(test_common PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})
target_compile_definitions(test_common PRIVATE LV_BUILD_TEST ${LVGL_COMPILER_DEFINES})

# Generate one test executable for each source file pair.
# The sources in ${CMAKE_CURRENT_BINARY_DIR} is auto-generated, the
# sources in src/test_cases is the actual test case.
find_package(Ruby REQUIRED)
set(generate_test_runner_rb
    ${CMAKE_CURRENT_SOURCE_DIR}/unity/generate_test_runner.rb)
set(generate_test_runner_config ${CMAKE_CURRENT_SOURCE_DIR}/config.yml)

if(NOT WIN32)
    # libjpeg is required for the jpeg test case
    find_package(JPEG REQUIRED)
    include_directories(${JPEG_INCLUDE_DIR})

    # Wayland
    find_package(PkgConfig)
    pkg_check_modules(wayland_client wayland-client)

    if (wayland_client_FOUND)
        pkg_check_modules(wayland-cursor REQUIRED wayland-cursor)
        pkg_check_modules(xkbcommon REQUIRED xkbcommon)

        link_libraries(wayland-client wayland-cursor xkbcommon)

        # Add auto generated source required for XDG shell
        include_directories("${LVGL_TEST_DIR}/wayland_protocols")
        target_sources(test_common PUBLIC "wayland_protocols/wayland_xdg_shell.c")
    else()
        add_definitions(-DLV_USE_WAYLAND=0)
    endif()
endif()

# libpng is required for the png test case
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

if(NOT WIN32)
    # FFmpeg is optional for the image and video test cases
    find_package(PkgConfig)
    pkg_check_modules(FFMPEG libavformat libavcodec libswscale libavutil)
    if(FFMPEG_FOUND)
        include_directories(${FFMPEG_INCLUDE_DIRS})
    else()
        message("FFmpeg not found, defaulting to 0")
        add_definitions(-DLV_USE_FFMPEG=0)
    endif()
else()
    message("Disable FFmpeg for Windows")
    add_definitions(-DLV_USE_FFMPEG=0)
endif()

# libfreetype is required for the font test case
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

if(OPTIONS_SDL)
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
endif()

# libinput is required for the libinput device driver test case
find_package(Libinput OPTIONAL_COMPONENTS)
if ($ENV{NON_AMD64_BUILD})
    include_directories(${LIBINPUT_INCLUDE_DIRS})
endif()

# OpenGL ES is required for its driver
if ($ENV{NON_AMD64_BUILD})
    message("Disable OpenGL, GLEW or glfw3 for non-amd64 build")
    add_definitions(-DLV_USE_OPENGLES=0)
else()
    find_package(OpenGL)
    find_package(GLEW)
    find_package(glfw3)

    if(OpenGL_FOUND AND GLEW_FOUND AND glfw3_FOUND)
        # Include directories
        include_directories(${OPENGL_INCLUDE_DIR})
        include_directories(${GLEW_INCLUDE_DIRS})
        include_directories(${GLFW_INCLUDE_DIRS})
        message("Enable LV_USE_OPENGLES")
    else()
        message("OpenGL, GLEW or glfw3 not found, defaulting to 0")
        add_definitions(-DLV_USE_OPENGLES=0)
    endif()
endif()

if (NOT LIBINPUT_FOUND)
    message("libinput not found, defaulting to 0")
    add_definitions(-DLV_USE_LIBINPUT=0)
endif()

find_package(PkgConfig)
pkg_check_modules(xkbcommon pkg_check_modules xkbcommon)

if (NOT xkbcommon_FOUND)
    message("xkbcommon not found, defaulting to 0")
    add_definitions(-DLV_LIBINPUT_XKB=0)
endif()

# libdrm is required for the DRM display driver test case
include(${CMAKE_CURRENT_LIST_DIR}/FindLibDRM.cmake)
if(Libdrm_FOUND)
    include_directories(${Libdrm_INCLUDE_DIRS})
else()
    message("libdrm not found, defaulting to 0")
    add_definitions(-DLV_USE_LINUX_DRM=0)
endif()

# If we are running on mac, set LV_USE_LINUX_FBDEV to 0
if(APPLE)
    add_definitions(-DLV_USE_LINUX_FBDEV=0)
endif()

if(WIN32)
    add_definitions(-DLV_USE_LINUX_FBDEV=0)
    add_definitions(-DLV_USE_WINDOWS=1)
    add_definitions(-DLV_USE_WAYLAND=0)
    add_definitions(-DLV_USE_OS=LV_OS_WINDOWS)
endif()

# disable test targets for build only tests
if (ENABLE_TESTS)
    file(GLOB_RECURSE TEST_CASE_FILES src/test_cases/*.c)
    file(GLOB_RECURSE TEST_LIBS_FILES src/test_libs/*.c)
else()
    set(TEST_CASE_FILES)
    set(TEST_LIBS_FILES)
endif()

# build a test libs target
if (TEST_LIBS_FILES)
    add_library(test_libs STATIC ${TEST_LIBS_FILES})
    target_include_directories(test_libs PUBLIC ${TEST_INCLUDE_DIRS} "src/test_libs")
    target_compile_definitions(test_libs PRIVATE LV_BUILD_TEST ${LVGL_COMPILER_DEFINES})
    target_compile_options(test_libs PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})
    list(APPEND TEST_LIBS test_libs)
endif()

foreach( test_case_fname ${TEST_CASE_FILES} )
    # If test file is foo/bar/baz.c then test_name is "baz".
    get_filename_component(test_name ${test_case_fname} NAME_WLE)
    if (${test_name} STREQUAL "_test_template")
        continue()
    endif()

    # gather all test cases
    list(APPEND TEST_CASES ${test_name})

    # Create path to auto-generated source file.
    set(test_runner_fname ${CMAKE_CURRENT_BINARY_DIR}/${test_name}_Runner.c)
    # Run ruby to generate source in build directory
    add_custom_command(
        OUTPUT ${test_runner_fname}
        COMMAND ${RUBY_EXECUTABLE} ${generate_test_runner_rb}
                ${test_case_fname} ${test_runner_fname}
                ${generate_test_runner_config}
        DEPENDS ${generate_test_runner_rb} ${test_case_fname}
                ${generate_test_runner_config}
    )
    add_executable( ${test_name}
        ${test_case_fname}
        ${test_runner_fname}
    )
    target_link_libraries(${test_name} PRIVATE
            test_common
            lvgl_demos
            lvgl
            lvgl_thorvg
            ${PNG_LIBRARIES}
            ${FREETYPE_LIBRARIES}
            ${LIBDRM_LIBRARIES}
            ${LIBINPUT_LIBRARIES}
            ${JPEG_LIBRARIES}
            ${FFMPEG_LIBRARIES}
            m
            pthread
            ${TEST_LIBS})

    if(OPTIONS_SDL)
        target_link_libraries(${test_name} PRIVATE ${SDL_LIBRARY})
    endif()

	if (NOT $ENV{NON_AMD64_BUILD})
	    target_link_libraries(${test_name} PRIVATE
    	        ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw)
	endif()

    target_include_directories(${test_name} PUBLIC ${TEST_INCLUDE_DIRS})
    target_compile_options(${test_name} PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})

    add_test(
        NAME ${test_name}
        WORKING_DIRECTORY ${LVGL_TEST_DIR}
        COMMAND ${test_name})
endforeach( test_case_fname ${TEST_CASE_FILES} )

add_custom_target(run
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --timeout 300
    WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
    DEPENDS ${TEST_CASES}
    USES_TERMINAL
)

endif()
