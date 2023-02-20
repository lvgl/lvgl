if(ESP_PLATFORM)

###################################
# Tests do not build for ESP-IDF. #
###################################

else()

cmake_minimum_required(VERSION 3.13)
project(lvgl_tests LANGUAGES C)

find_program(VALGRIND_EXECUTABLE valgrind)
if (VALGRIND_EXECUTABLE)
    set(MEMORYCHECK_COMMAND ${VALGRIND_EXECUTABLE})
    set(MEMORYCHECK_COMMAND_OPTIONS --error-exitcode=1)
endif()
include(CTest)

set(LVGL_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})


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
)

set(LVGL_TEST_OPTIONS_TEST_DEFHEAP
    -DLV_TEST_OPTION=5
    -DLVGL_CI_USING_DEF_HEAP
    -fsanitize=address
    --coverage
)

if (OPTIONS_NORMAL_8BIT)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_NORMAL_8BIT})
elseif (OPTIONS_16BIT)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_16BIT})
elseif (OPTIONS_24BIT)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_24BIT})
elseif (OPTIONS_FULL_32BIT)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_FULL_32BIT})
elseif (OPTIONS_TEST_SYSHEAP)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_TEST_SYSHEAP} -fsanitize=address --coverage)
    set (TEST_LIBS --coverage -fsanitize=address)
    set (LV_CONF_BUILD_DISABLE_EXAMPLES ON)
    set (ENABLE_TESTS ON)
elseif (OPTIONS_TEST_DEFHEAP)
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_TEST_DEFHEAP})
    set (TEST_LIBS --coverage -fsanitize=address)
    set (LV_CONF_BUILD_DISABLE_EXAMPLES ON)
    set (ENABLE_TESTS ON)
elseif (OPTIONS_TEST_MEMORYCHECK)
    # sanitizer is disabled because valgrind uses LD_PRELOAD and the
    # sanitizer lib needs to load first
    set (BUILD_OPTIONS ${LVGL_TEST_OPTIONS_TEST_SYSHEAP})
    set (LV_CONF_BUILD_DISABLE_EXAMPLES ON)
    set (ENABLE_TESTS ON)
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
if (TARGET lvgl_examples)
  target_compile_options(lvgl_examples PUBLIC ${COMPILE_OPTIONS})
endif()


set(TEST_INCLUDE_DIRS
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}/src>
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}/unity>
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}>
)

add_library(test_common
    STATIC
        src/lv_test_indev.c
        src/lv_test_init.c
        src/test_assets/test_animimg001.c
        src/test_assets/test_animimg002.c
        src/test_assets/test_animimg003.c
        src/test_assets/font_1.c
        src/test_assets/font_2.c
        src/test_assets/font_3.c
        src/test_assets/test_img_caret_down.c
        src/test_assets/ubuntu_font.c
        unity/unity_support.c
        unity/unity.c
)
target_include_directories(test_common PUBLIC ${TEST_INCLUDE_DIRS})
target_compile_options(test_common PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})

# Generate one test executable for each source file pair.
# The sources in ${CMAKE_CURRENT_BINARY_DIR} is auto-generated, the
# sources in src/test_cases is the actual test case.
find_package(Ruby REQUIRED)
set(generate_test_runner_rb
    ${CMAKE_CURRENT_SOURCE_DIR}/unity/generate_test_runner.rb)
set(generate_test_runner_config ${CMAKE_CURRENT_SOURCE_DIR}/config.yml)

# disable test targets for build only tests
if (ENABLE_TESTS)
    file( GLOB TEST_CASE_FILES src/test_cases/*.c )
else()
    set(TEST_CASE_FILES)
endif()

foreach( test_case_fname ${TEST_CASE_FILES} )
    # If test file is foo/bar/baz.c then test_name is "baz".
    get_filename_component(test_name ${test_case_fname} NAME_WLE)
    if (${test_name} STREQUAL "_test_template")
        continue()
    endif()
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
    target_link_libraries(${test_name} test_common lvgl_demos lvgl png m ${TEST_LIBS})
    target_include_directories(${test_name} PUBLIC ${TEST_INCLUDE_DIRS})
    target_compile_options(${test_name} PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})

    add_test(
        NAME ${test_name}
        WORKING_DIRECTORY ${LVGL_TEST_DIR}
        COMMAND ${test_name})
endforeach( test_case_fname ${TEST_CASE_FILES} )

endif()
