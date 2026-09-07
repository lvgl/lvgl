# Unit tests for LVGL. Not a project of its own: this is pulled in by
# env_support/cmake/main.cmake when LV_BUILD_TESTS is set, after the lvgl,
# lvgl_thorvg, lvgl_examples and lvgl_demos targets have been created.
#
# tests/main.py drives the build. It picks a configuration by passing the list
# of defconfig fragments from tests/configs in LV_BUILD_DEFCONFIG_PATH, and
# everything below is derived from the resulting CONFIG_LV_* variables, so
# adding a test configuration only takes a new recipe in main.py.

option(LVGL_TEST_ENABLE "Build and register the test executables" OFF)

set(LVGL_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT MSVC)
    set(IS_GNU_LIKE TRUE)
endif()

# Set by the CI job that builds and runs the tests as 32-bit binaries. The
# -m32 flags themselves come from main.py, they have to be in place before the
# compiler and library checks run.
set(NON_AMD64_BUILD "$ENV{NON_AMD64_BUILD}")

########################### COMPILER OPTIONS ###########################

# Options that check_c_compiler_flag() can not detect, either because they only
# fail when linking or because they are accepted but warn.
set(FLAG_CHECK_WHITELIST --coverage -fsanitize=address -Werror)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

# Copy the subset of ARGN that this compiler supports into ${options_out}.
function(filter_compiler_options lang options_out)
    set(options ${ARGN})
    foreach(option ${options})
        if(option IN_LIST FLAG_CHECK_WHITELIST)
            list(APPEND ${options_out} ${option})
            continue()
        endif()

        string(TOUPPER FLAG_SUPPORTED_FOR_${lang}_${option} option_var_name)
        string(REPLACE "-" "_" option_var_name ${option_var_name})
        string(REPLACE "+" "_" option_var_name ${option_var_name})

        if(${lang} STREQUAL C)
            check_c_compiler_flag(${option} ${option_var_name})
        elseif(${lang} STREQUAL CXX)
            check_cxx_compiler_flag(${option} ${option_var_name})
        else()
            message(FATAL_ERROR "Unknown language ${lang}")
        endif()
        if(${option_var_name})
            list(APPEND ${options_out} ${option})
        endif()
    endforeach()
    set(${options_out} ${${options_out}} PARENT_SCOPE)
endfunction()

if(IS_GNU_LIKE)
    set(COMPILE_OPTIONS
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
        -Wmultichar
        -Wpedantic
        -Wpointer-arith
        -Wreturn-type
        -Wshadow
        -Wshift-negative-value
        -Wsizeof-pointer-memaccess
        -Wtype-limits
        -Wundef
        -Wuninitialized
        -Wunreachable-code
        -Wunused-function
        -Werror=float-conversion
        -Werror=strict-aliasing
        -Wno-double-promotion
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-overlength-strings
        -Wno-unreachable-code
    )
else()
    set(COMPILE_OPTIONS
        /W3         # enable all "production quality" warnings
        /we4013     # treat function undefined as error
    )
endif()

# The configurations whose tests are executed also run under the sanitizers and
# collect coverage.
if(LVGL_TEST_ENABLE AND IS_GNU_LIKE)
    set(SANITIZE_AND_COVERAGE_OPTIONS
        -fsanitize=address
        -fsanitize=leak
        -fsanitize=undefined
        --coverage
    )
    list(APPEND COMPILE_OPTIONS ${SANITIZE_AND_COVERAGE_OPTIONS})
endif()

# Configuration dependent compiler options. Deriving these from the resulting
# configuration rather than from the name of the test configuration keeps them
# in step with the defconfigs.
if(IS_GNU_LIKE)
    if(CONFIG_LV_USE_VG_LITE_THORVG)
        # Workaround for a ThorVG dangling-pointer warning
        list(APPEND COMPILE_OPTIONS -Wno-dangling-pointer)
    endif()

    if(NOT CONFIG_LV_USE_BUILTIN_MALLOC)
        # Unused variables are common in the dual-heap arrangement
        list(APPEND COMPILE_OPTIONS -Wno-unused-but-set-variable)
    endif()

    if(CONFIG_LV_USE_OBJ_PROPERTY)
        # The property API relies on C11 anonymous structs and unions
        list(REMOVE_ITEM COMPILE_OPTIONS -pedantic-errors -Wpedantic)
    endif()
endif()

filter_compiler_options(C LVGL_C_COMPILE_OPTIONS ${COMPILE_OPTIONS})

if(IS_GNU_LIKE)
    # The test cases define their functions without a prototype
    filter_compiler_options(C LVGL_TESTFILE_COMPILE_OPTIONS
        ${LVGL_C_COMPILE_OPTIONS}
        -Wno-missing-prototypes
    )
    filter_compiler_options(CXX LVGL_CXX_COMPILE_OPTIONS
        ${COMPILE_OPTIONS}
        -Wno-c++11-extensions
        -Wno-deprecated-copy-with-user-provided-dtor
        -Wno-float-conversion
        -Wno-missing-prototypes
        -Wno-pedantic
        -Wno-shadow
        -Wno-unused-parameter
    )
else()
    filter_compiler_options(C LVGL_TESTFILE_COMPILE_OPTIONS ${LVGL_C_COMPILE_OPTIONS})
    filter_compiler_options(CXX LVGL_CXX_COMPILE_OPTIONS ${COMPILE_OPTIONS})
endif()

# LV_DISABLE_API_MAPPING is not an lv_conf.h option, it can only be set from the
# build system. The tests must use the current API names.
set(COMPILE_DEFINITIONS LV_BUILD_TEST LV_DISABLE_API_MAPPING=1)

if(NON_AMD64_BUILD)
    list(APPEND COMPILE_DEFINITIONS NON_AMD64_BUILD)
endif()

######################## ADJUST THE LVGL TARGETS ########################

foreach(target lvgl lvgl_thorvg lvgl_examples lvgl_demos)
    if(TARGET ${target})
        target_compile_options(${target} PUBLIC
            $<$<COMPILE_LANGUAGE:C>:${LVGL_C_COMPILE_OPTIONS}>
            $<$<COMPILE_LANGUAGE:CXX>:${LVGL_CXX_COMPILE_OPTIONS}>
        )
        if(IS_GNU_LIKE)
            target_compile_options(${target} PUBLIC
                $<$<COMPILE_LANGUAGE:ASM>:${LVGL_C_COMPILE_OPTIONS}>)
        endif()
    endif()
endforeach()

if(SANITIZE_AND_COVERAGE_OPTIONS)
    # The sanitizer and coverage runtimes have to be on the link line as well
    target_link_options(lvgl PUBLIC ${SANITIZE_AND_COVERAGE_OPTIONS})
endif()

# Reference images are renderer specific, each renderer has its own set
if(CONFIG_LV_USE_DRAW_VG_LITE)
    set(REF_IMGS_PATH "ref_imgs_vg_lite/")
elseif(CONFIG_LV_USE_DRAW_NANOVG)
    set(REF_IMGS_PATH "ref_imgs_nanovg/")
else()
    set(REF_IMGS_PATH "ref_imgs/")
endif()

# lv_test_screenshot_compare() is part of LVGL, so the reference image location
# has to be given to the library, not to the test cases.
target_compile_definitions(lvgl PUBLIC
    ${COMPILE_DEFINITIONS}
    REF_IMGS_PATH="${REF_IMGS_PATH}")

if(CONFIG_LV_USE_DRAW_VG_LITE AND NON_AMD64_BUILD)
    # On 32-bit systems the output of ThorVG's anti-aliasing algorithm deviates
    # slightly
    target_compile_definitions(lvgl PUBLIC REF_IMG_TOLERANCE=9)
endif()

############################# BUILD TESTS #############################

set(TEST_INCLUDE_DIRS
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}/src>
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}/unity>
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}>
)

# Only the configurations that execute their tests need the test cases, the test
# harness and the test assets. For the others this listfile exists purely to
# build LVGL itself with the strict warning set.
if(NOT LVGL_TEST_ENABLE)
    return()
endif()

file(GLOB TEST_ASSETS_SRC ${LVGL_TEST_DIR}/src/test_assets/*.c)
file(GLOB_RECURSE TEST_IMAGES_SRC
    ${LVGL_TEST_DIR}/test_images/*.c
    ${LVGL_ROOT_DIR}/examples/assets/*.c)

add_library(test_common STATIC
    src/lv_test_init.c
    unity/unity.c
    ${TEST_ASSETS_SRC}
    ${TEST_IMAGES_SRC}
)
target_include_directories(test_common PUBLIC ${TEST_INCLUDE_DIRS})
target_compile_options(test_common PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})
# Linking LVGL also brings in its include directories and the definitions that
# select the configuration
target_link_libraries(test_common PUBLIC lvgl)
target_compile_definitions(test_common PRIVATE
    # A few test cases use an image from examples/assets, whose sources are
    # guarded by LV_BUILD_EXAMPLES. Compile them in even when the examples
    # library itself is not built.
    LV_BUILD_EXAMPLES=1
)

file(GLOB_RECURSE TEST_CASE_FILES src/test_cases/*.c)
file(GLOB_RECURSE TEST_HELPERS_FILES src/test_helpers/*.c)

if(TEST_HELPERS_FILES)
    add_library(test_helpers STATIC ${TEST_HELPERS_FILES})
    target_include_directories(test_helpers PUBLIC ${TEST_INCLUDE_DIRS} src/test_helpers)
    target_compile_options(test_helpers PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})
    target_link_libraries(test_helpers PUBLIC lvgl)
endif()

# Generate one test executable per test case source file. The runner source is
# produced by Unity's ruby script into the build directory.
find_package(Ruby)
if(NOT RUBY_EXECUTABLE)
    message(FATAL_ERROR "Ruby interpreter not found")
endif()

set(generate_test_runner_rb ${LVGL_TEST_DIR}/unity/generate_test_runner.rb)
set(generate_test_runner_config ${LVGL_TEST_DIR}/config.yml)

foreach(test_case_fname ${TEST_CASE_FILES})
    # If the test file is foo/bar/baz.c then test_name is "baz"
    get_filename_component(test_name ${test_case_fname} NAME_WLE)
    if(${test_name} STREQUAL "_test_template")
        continue()
    endif()

    list(APPEND TEST_CASES ${test_name})

    set(test_runner_fname ${CMAKE_CURRENT_BINARY_DIR}/${test_name}_Runner.c)
    add_custom_command(
        OUTPUT ${test_runner_fname}
        COMMAND ${RUBY_EXECUTABLE} ${generate_test_runner_rb}
                ${test_case_fname} ${test_runner_fname}
                ${generate_test_runner_config}
        DEPENDS ${generate_test_runner_rb} ${test_case_fname}
                ${generate_test_runner_config}
    )

    add_executable(${test_name} ${test_case_fname} ${test_runner_fname})
    target_include_directories(${test_name} PUBLIC ${TEST_INCLUDE_DIRS})
    target_compile_options(${test_name} PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})
    target_link_libraries(${test_name} PRIVATE
        test_common
        lvgl
        $<TARGET_NAME_IF_EXISTS:lvgl_demos>
        $<TARGET_NAME_IF_EXISTS:lvgl_thorvg>
        $<TARGET_NAME_IF_EXISTS:test_helpers>
    )

    add_test(
        NAME ${test_name}
        WORKING_DIRECTORY ${LVGL_TEST_DIR}
        COMMAND ${test_name})

    if(CONFIG_LV_USE_NANOVG_TEST_HEADLESS)
        # Force the Mesa driver so that rendering is reproducible
        set(nanovg_test_env
            "LSAN_OPTIONS=fast_unwind_on_malloc=0"
            "LIBGL_ALWAYS_SOFTWARE=1"
            "GALLIUM_DRIVER=llvmpipe"
            "__EGL_VENDOR_LIBRARY_FILENAMES=${LVGL_TEST_DIR}/egl_vendor_mesa_only.json")
        set_tests_properties(${test_name} PROPERTIES ENVIRONMENT "${nanovg_test_env}")
    endif()
endforeach()

add_custom_target(run
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --timeout 300
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${TEST_CASES}
    USES_TERMINAL
)
