if(ESP_PLATFORM)

###################################
# Tests do not build for ESP-IDF. #
###################################

else()

cmake_minimum_required(VERSION 3.13)
project(lvgl_tests LANGUAGES C)

include(CTest)

set(LVGL_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(LVGL_TEST_COMMON_EXAMPLE_OPTIONS
    -DLV_BUILD_EXAMPLES=1
    -DLV_USE_DEMO_WIDGETS=1
    -DLV_USE_DEMO_STRESS=1
)

set(LVGL_TEST_OPTIONS_MINIMAL_MONOCHROME
    -DLV_COLOR_DEPTH=1
    -DLV_MEM_SIZE=65535
    -DLV_DPI_DEF=40
    -DLV_DRAW_COMPLEX=0
    -DLV_USE_METER=0
    -DLV_USE_LOG=1
    -DLV_USE_ASSERT_NULL=0
    -DLV_USE_ASSERT_MALLOC=0
    -DLV_USE_ASSERT_MEM_INTEGRITY=0
    -DLV_USE_ASSERT_OBJ=0
    -DLV_USE_ASSERT_STYLE=0
    -DLV_USE_USER_DATA=0
    -DLV_FONT_UNSCII_8=1
    -DLV_USE_BIDI=0
    -DLV_USE_ARABIC_PERSIAN_CHARS=0
    -DLV_BUILD_EXAMPLES=1
    -DLV_FONT_DEFAULT=&lv_font_montserrat_14
    -DLV_USE_PNG=1
    -DLV_USE_BMP=1
    -DLV_USE_GIF=1
    -DLV_USE_QRCODE=1
)

set(LVGL_TEST_OPTIONS_NORMAL_8BIT
    -DLV_COLOR_DEPTH=8
    -DLV_MEM_SIZE=65535
    -DLV_DPI_DEF=40
    -DLV_DRAW_COMPLEX=1
    -DLV_USE_LOG=1
    -DLV_USE_ASSERT_NULL=0
    -DLV_USE_ASSERT_MALLOC=0
    -DLV_USE_ASSERT_MEM_INTEGRITY=0
    -DLV_USE_ASSERT_OBJ=0
    -DLV_USE_ASSERT_STYLE=0
    -DLV_USE_USER_DATA=1
    -DLV_FONT_UNSCII_8=1
    -DLV_USE_FONT_SUBPX=1
    -DLV_USE_BIDI=0
    -DLV_USE_ARABIC_PERSIAN_CHARS=0
    ${LVGL_TEST_COMMON_EXAMPLE_OPTIONS}
    -DLV_FONT_DEFAULT=&lv_font_montserrat_14
    -DLV_USE_PNG=1
    -DLV_USE_BMP=1
    -DLV_USE_SJPG=1
    -DLV_USE_GIF=1
    -DLV_USE_QRCODE=1
)

set(LVGL_TEST_OPTIONS_16BIT
    -DLV_COLOR_DEPTH=16
    -DLV_COLOR_16_SWAP=0
    -DLV_MEM_SIZE=65536
    -DLV_DPI_DEF=40
    -DLV_DRAW_COMPLEX=1
    -DLV_DITHER_GRADIENT=1
    -DLV_USE_LOG=1
    -DLV_USE_ASSERT_NULL=0
    -DLV_USE_ASSERT_MALLOC=0
    -DLV_USE_ASSERT_MEM_INTEGRITY=0
    -DLV_USE_ASSERT_OBJ=0
    -DLV_USE_ASSERT_STYLE=0
    -DLV_USE_USER_DATA=1
    -DLV_FONT_UNSCII_8=1
    -DLV_USE_FONT_SUBPX=1
    -DLV_USE_BIDI=0
    -DLV_USE_ARABIC_PERSIAN_CHARS=0
    ${LVGL_TEST_COMMON_EXAMPLE_OPTIONS}
    -DLV_FONT_DEFAULT=&lv_font_montserrat_14
    -DLV_USE_PNG=1
    -DLV_USE_BMP=1
    -DLV_USE_SJPG=1
    -DLV_USE_GIF=1
    -DLV_USE_QRCODE=1
)

set(LVGL_TEST_OPTIONS_16BIT_SWAP
    -DLV_COLOR_DEPTH=16
    -DLV_COLOR_16_SWAP=1  
    -DLV_MEM_SIZE=65536
    -DLV_DPI_DEF=40
    -DLV_DRAW_COMPLEX=1
    -DLV_DITHER_GRADIENT=1
    -DLV_DITHER_ERROR_DIFFUSION=1
    -DLV_GRAD_CACHE_DEF_SIZE=8*1024
    -DLV_USE_LOG=1
    -DLV_USE_ASSERT_NULL=0
    -DLV_USE_ASSERT_MALLOC=0
    -DLV_USE_ASSERT_MEM_INTEGRITY=0
    -DLV_USE_ASSERT_OBJ=0
    -DLV_USE_ASSERT_STYLE=0
    -DLV_USE_USER_DATA=1
    -DLV_FONT_UNSCII_8=1
    -DLV_USE_FONT_SUBPX=1
    -DLV_USE_BIDI=0
    -DLV_USE_ARABIC_PERSIAN_CHARS=0
    ${LVGL_TEST_COMMON_EXAMPLE_OPTIONS}
    -DLV_FONT_DEFAULT=&lv_font_montserrat_14
    -DLV_USE_PNG=1
    -DLV_USE_BMP=1
    -DLV_USE_SJPG=1
    -DLV_USE_GIF=1
    -DLV_USE_QRCODE=1
)
  
set(LVGL_TEST_OPTIONS_FULL_32BIT
    -DLV_COLOR_DEPTH=32
    -DLV_MEM_SIZE=8388608
    -DLV_DPI_DEF=160
    -DLV_DRAW_COMPLEX=1
    -DLV_SHADOW_CACHE_SIZE=1
    -DLV_IMG_CACHE_DEF_SIZE=32
    -DLV_USE_LOG=1
    -DLV_LOG_LEVEL=LV_LOG_LEVEL_TRACE
    -DLV_LOG_PRINTF=1
    -DLV_USE_FONT_SUBPX=1
    -DLV_FONT_SUBPX_BGR=1
    -DLV_USE_PERF_MONITOR=1
    -DLV_USE_ASSERT_NULL=1
    -DLV_USE_ASSERT_MALLOC=1
    -DLV_USE_ASSERT_MEM_INTEGRITY=1
    -DLV_USE_ASSERT_OBJ=1
    -DLV_USE_ASSERT_STYLE=1
    -DLV_USE_USER_DATA=1
    -DLV_USE_LARGE_COORD=1
    -DLV_FONT_MONTSERRAT_8=1
    -DLV_FONT_MONTSERRAT_10=1
    -DLV_FONT_MONTSERRAT_12=1
    -DLV_FONT_MONTSERRAT_14=1
    -DLV_FONT_MONTSERRAT_16=1
    -DLV_FONT_MONTSERRAT_18=1
    -DLV_FONT_MONTSERRAT_20=1
    -DLV_FONT_MONTSERRAT_22=1
    -DLV_FONT_MONTSERRAT_24=1
    -DLV_FONT_MONTSERRAT_26=1
    -DLV_FONT_MONTSERRAT_28=1
    -DLV_FONT_MONTSERRAT_30=1
    -DLV_FONT_MONTSERRAT_32=1
    -DLV_FONT_MONTSERRAT_34=1
    -DLV_FONT_MONTSERRAT_36=1
    -DLV_FONT_MONTSERRAT_38=1
    -DLV_FONT_MONTSERRAT_40=1
    -DLV_FONT_MONTSERRAT_42=1
    -DLV_FONT_MONTSERRAT_44=1
    -DLV_FONT_MONTSERRAT_46=1
    -DLV_FONT_MONTSERRAT_48=1
    -DLV_FONT_MONTSERRAT_12_SUBPX=1
    -DLV_FONT_MONTSERRAT_28_COMPRESSED=1
    -DLV_FONT_DEJAVU_16_PERSIAN_HEBREW=1
    -DLV_FONT_SIMSUN_16_CJK=1
    -DLV_FONT_UNSCII_8=1
    -DLV_FONT_UNSCII_16=1
    -DLV_FONT_FMT_TXT_LARGE=1
    -DLV_USE_FONT_COMPRESSED=1
    -DLV_USE_BIDI=1
    -DLV_USE_ARABIC_PERSIAN_CHARS=1
    -DLV_USE_PERF_MONITOR=1
    -DLV_USE_MEM_MONITOR=1
    -DLV_LABEL_TEXT_SELECTION=1
    ${LVGL_TEST_COMMON_EXAMPLE_OPTIONS}
    -DLV_FONT_DEFAULT=&lv_font_montserrat_24
    -DLV_USE_FS_STDIO=1
    -DLV_FS_STDIO_LETTER='A' 
    -DLV_USE_FS_POSIX=1 
    -DLV_FS_POSIX_LETTER='B'
    -DLV_USE_PNG=1
    -DLV_USE_BMP=1
    -DLV_USE_SJPG=1
    -DLV_USE_GIF=1
    -DLV_USE_QRCODE=1
    -DLV_USE_FRAGMENT=1
)
  
set(LVGL_TEST_OPTIONS_TEST_COMMON
    --coverage
    -DLV_COLOR_DEPTH=32
    -DLV_MEM_SIZE=2097152
    -DLV_SHADOW_CACHE_SIZE=10240
    -DLV_IMG_CACHE_DEF_SIZE=32
    -DLV_DITHER_GRADIENT=1
    -DLV_DITHER_ERROR_DIFFUSION=1
    -DLV_GRAD_CACHE_DEF_SIZE=8*1024
    -DLV_USE_LOG=1
    -DLV_LOG_PRINTF=1
    -DLV_USE_FONT_SUBPX=1
    -DLV_FONT_SUBPX_BGR=1
    -DLV_USE_ASSERT_NULL=0
    -DLV_USE_ASSERT_MALLOC=0
    -DLV_USE_ASSERT_MEM_INTEGRITY=0
    -DLV_USE_ASSERT_OBJ=0
    -DLV_USE_ASSERT_STYLE=0
    -DLV_USE_USER_DATA=1
    -DLV_USE_LARGE_COORD=1
    -DLV_FONT_MONTSERRAT_14=1
    -DLV_FONT_MONTSERRAT_16=1
    -DLV_FONT_MONTSERRAT_18=1
    -DLV_FONT_MONTSERRAT_24=1
    -DLV_FONT_MONTSERRAT_48=1
    -DLV_FONT_MONTSERRAT_12_SUBPX=1
    -DLV_FONT_MONTSERRAT_28_COMPRESSED=1
    -DLV_FONT_DEJAVU_16_PERSIAN_HEBREW=1
    -DLV_FONT_SIMSUN_16_CJK=1
    -DLV_FONT_UNSCII_8=1
    -DLV_FONT_UNSCII_16=1
    -DLV_FONT_FMT_TXT_LARGE=1
    -DLV_USE_FONT_COMPRESSED=1
    -DLV_USE_BIDI=1
    -DLV_USE_ARABIC_PERSIAN_CHARS=1
    -DLV_LABEL_TEXT_SELECTION=1
    ${LVGL_TEST_COMMON_EXAMPLE_OPTIONS}
    -DLV_FONT_DEFAULT=&lv_font_montserrat_14
    -Wno-unused-but-set-variable # unused variables are common in the dual-heap arrangement
    -Wno-unused-variable
)

set(LVGL_TEST_OPTIONS_TEST_SYSHEAP
    ${LVGL_TEST_OPTIONS_TEST_COMMON}
    -DLVGL_CI_USING_SYS_HEAP
    -DLV_MEM_CUSTOM=1
    -fsanitize=address
)

set(LVGL_TEST_OPTIONS_TEST_DEFHEAP
    ${LVGL_TEST_OPTIONS_TEST_COMMON}
    -DLVGL_CI_USING_DEF_HEAP
    -DLV_MEM_SIZE=2097152
    -fsanitize=address
)

if (OPTIONS_MINIMAL_MONOCHROME)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_MINIMAL_MONOCHROME})
elseif (OPTIONS_NORMAL_8BIT)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_NORMAL_8BIT})
elseif (OPTIONS_16BIT)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_16BIT})
elseif (OPTIONS_16BIT_SWAP)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_16BIT_SWAP})
elseif (OPTIONS_FULL_32BIT)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_FULL_32BIT})
elseif (OPTIONS_TEST_SYSHEAP)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_TEST_SYSHEAP})
    set (TEST_LIBS --coverage -fsanitize=address)
elseif (OPTIONS_TEST_DEFHEAP)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_TEST_DEFHEAP})
    set (TEST_LIBS --coverage -fsanitize=address)
else()
    message(FATAL_ERROR "Must provide a known options value (check main.py?).")
endif()

# Options lvgl and examples are compiled with.
set(COMPILE_OPTIONS
    -DLV_CONF_PATH=${LVGL_TEST_DIR}/src/lv_test_conf.h
    -DLV_BUILD_TEST
    -pedantic-errors
    -Wall
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
    -Wno-discarded-qualifiers
    -Wpedantic
    -Wreturn-type
    -Wshadow
    -Wshift-negative-value
    -Wsizeof-pointer-memaccess
    -Wstack-usage=5000
    -Wtype-limits
    -Wundef
    -Wuninitialized
    -Wunreachable-code
    ${BUILD_OPTIONS}
)

# Options test cases are compiled with.
set(LVGL_TESTFILE_COMPILE_OPTIONS
    ${COMPILE_OPTIONS}
    -Wno-missing-prototypes
)

get_filename_component(LVGL_DIR ${LVGL_TEST_DIR} DIRECTORY)

# Include lvgl project file.
include(${LVGL_DIR}/CMakeLists.txt)
target_compile_options(lvgl PUBLIC ${COMPILE_OPTIONS})
target_compile_options(lvgl_examples PUBLIC ${COMPILE_OPTIONS})


set(TEST_INCLUDE_DIRS
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}/src>
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}/unity>
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}>
)

add_library(test_common
    STATIC
        src/lv_test_indev.c
        src/lv_test_init.c
        src/test_fonts/font_1.c
        src/test_fonts/font_2.c
        src/test_fonts/font_3.c
        unity/unity_support.c
        unity/unity.c
)
target_include_directories(test_common PUBLIC ${TEST_INCLUDE_DIRS})
target_compile_options(test_common PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})

# Some examples `#include "lvgl/lvgl.h"` - which is a path which is not
# in this source repository. If this repo is in a directory names 'lvgl'
# then we can add our parent directory to the include path.
# TODO: This is not good practice and should be fixed.
get_filename_component(LVGL_PARENT_DIR ${LVGL_DIR} DIRECTORY)
target_include_directories(lvgl_examples PUBLIC $<BUILD_INTERFACE:${LVGL_PARENT_DIR}>)

# Generate one test executable for each source file pair.
# The sources in src/test_runners is auto-generated, the
# sources in src/test_cases is the actual test case.
file( GLOB TEST_CASE_FILES src/test_cases/*.c )
foreach( test_case_fname ${TEST_CASE_FILES} )
    # If test file is foo/bar/baz.c then test_name is "baz".
    get_filename_component(test_name ${test_case_fname} NAME_WLE)
    if (${test_name} STREQUAL "_test_template")
        continue()
    endif()
    # Create path to auto-generated source file.
    set(test_runner_fname src/test_runners/${test_name}_Runner.c)
    add_executable( ${test_name}
        ${test_case_fname}
        ${test_runner_fname}
    )
    target_link_libraries(${test_name} test_common lvgl_examples lvgl_demos lvgl png ${TEST_LIBS})
    target_include_directories(${test_name} PUBLIC ${TEST_INCLUDE_DIRS})
    target_compile_options(${test_name} PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})

    add_test(
        NAME ${test_name} 
        WORKING_DIRECTORY ${LVGL_TEST_DIR}
        COMMAND ${test_name})
endforeach( test_case_fname ${TEST_CASE_FILES} )

endif()
