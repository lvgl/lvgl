#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include <unistd.h>
#include <sys/stat.h>

static lv_obj_t * active_screen = NULL;
static lv_obj_t * file_explorer_obj;
static lv_file_explorer_t * file_explorer;

void setUp(void)
{
    active_screen = lv_screen_active();
    file_explorer_obj = lv_file_explorer_create(active_screen);
    file_explorer = (lv_file_explorer_t *)file_explorer_obj;
}

void tearDown(void)
{
    /* Is there a way to destroy a chart without having to call remove_series for each of it series? */
    lv_obj_clean(active_screen);
}

void test_file_explorer_read_dir(void)
{
    mkdir("src/test_files/test_file_explorer_floder", 0777);
    mkdir("src/test_files/test_file_explorer_floder/dev", 0777);
    mkdir("src/test_files/test_file_explorer_floder/dev/shm", 0777);
    mkdir("src/test_files/test_file_explorer_floder/home", 0777);
    mkdir("src/test_files/test_file_explorer_floder/home/web_user", 0777);

    lv_file_explorer_open_dir(file_explorer_obj, "A:src/test_files/test_file_explorer_floder");
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_floder/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    lv_table_set_selected_cell(file_explorer->file_table, 2, 0);
    lv_table_set_selected_cell(file_explorer->file_table, 0, 0);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_floder/dev/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    lv_table_set_selected_cell(file_explorer->file_table, 2, 0);
    lv_table_set_selected_cell(file_explorer->file_table, 0, 0);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_floder/dev/shm/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    lv_table_set_selected_cell(file_explorer->file_table, 1, 0);
    lv_table_set_selected_cell(file_explorer->file_table, 0, 0);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_floder/dev/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    lv_table_set_selected_cell(file_explorer->file_table, 1, 0);
    lv_table_set_selected_cell(file_explorer->file_table, 0, 0);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_floder/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    lv_table_set_selected_cell(file_explorer->file_table, 3, 0);
    lv_table_set_selected_cell(file_explorer->file_table, 0, 0);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_floder/home/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    lv_table_set_selected_cell(file_explorer->file_table, 2, 0);
    lv_table_set_selected_cell(file_explorer->file_table, 0, 0);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_floder/home/web_user/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    rmdir("src/test_files/test_file_explorer_floder/dev/shm");
    rmdir("src/test_files/test_file_explorer_floder/dev");
    rmdir("src/test_files/test_file_explorer_floder/home/web_user");
    rmdir("src/test_files/test_file_explorer_floder/home");
    rmdir("src/test_files/test_file_explorer_floder");

}

#endif
