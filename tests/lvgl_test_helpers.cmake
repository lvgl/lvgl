# Helpers for registering the unit test sources.

# Register every test case source file directly inside `folder`, a path
# relative to tests/src. Subfolders are not included
#
#    lvgl_add_test_folder(test_cases/widgets)
function(lvgl_add_test_folder folder)
    set(dir ${LVGL_TEST_DIR}/src/${folder})
    if(NOT IS_DIRECTORY ${dir})
        message(FATAL_ERROR "lvgl_add_test_folder: no such folder: ${dir}")
    endif()

    file(GLOB sources CONFIGURE_DEPENDS ${dir}/*.c)
    if(NOT sources)
        message(FATAL_ERROR "lvgl_add_test_folder: no test case in ${dir}")
    endif()

    set_property(GLOBAL APPEND PROPERTY LVGL_TEST_CASE_FILES ${sources})
endfunction()

# Same as lvgl_add_test_folder(), but dependent on a condition.
# The folder is the last argument and everything before it is passed to if()
#
#     lvgl_add_test_folder_if(CONFIG_LV_USE_GLTF test_cases/3d)
#     lvgl_add_test_folder_if(NOT CONFIG_LV_CHECK_ARG_ASSERT_ON_FAIL test_cases/public_api)
function(lvgl_add_test_folder_if)
    if(ARGC LESS 2)
        message(FATAL_ERROR "lvgl_add_test_folder_if: expected <condition...> <folder>, got: ${ARGV}")
    endif()
    set(condition ${ARGV})
    list(POP_BACK condition folder)
    if(${condition})
        lvgl_add_test_folder(${folder})
    endif()
endfunction()

# Get the test files registered so far, as a list of absolute paths.
function(lvgl_get_test_files out_var)
    get_property(files GLOBAL PROPERTY LVGL_TEST_CASE_FILES)
    list(SORT files)
    set(${out_var} ${files} PARENT_SCOPE)
endfunction()
