#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include <unistd.h>
#include <sys/stat.h>

static lv_obj_t * active_screen = NULL;
static lv_obj_t * file_explorer_obj;
static lv_file_explorer_t * file_explorer;
static lv_table_t * file_table;

void setUp(void)
{
    active_screen = lv_screen_active();
    file_explorer_obj = lv_file_explorer_create(active_screen);
    file_explorer = (lv_file_explorer_t *)file_explorer_obj;
    file_table = (lv_table_t *)file_explorer->file_table;
}

void tearDown(void)
{
    /* Is there a way to destroy a chart without having to call remove_series for each of it series? */
    lv_obj_clean(active_screen);
}

void test_file_explorer_read_dir(void)
{
    uint8_t back_row = 0, dev_row = 0, shm_row = 0, home_row = 0, user_row = 0;

    mkdir("src/test_files/test_file_explorer_folder", 0777);
    mkdir("src/test_files/test_file_explorer_folder/dev", 0777);
    mkdir("src/test_files/test_file_explorer_folder/dev/shm", 0777);
    mkdir("src/test_files/test_file_explorer_folder/home", 0777);
    mkdir("src/test_files/test_file_explorer_folder/home/web_user", 0777);

    lv_file_explorer_open_dir(file_explorer_obj, "A:src/test_files/test_file_explorer_folder");
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_folder/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    for(uint8_t i = 0; i < file_table->row_cnt; i++) {
        if(lv_strcmp(lv_table_get_cell_value(file_explorer->file_table, i, 0), LV_SYMBOL_LEFT "  Back") == 0) {
            back_row = i;
        }
        if(lv_strcmp(lv_table_get_cell_value(file_explorer->file_table, i, 0), LV_SYMBOL_DIRECTORY "  dev") == 0) {
            dev_row = i;
        }
        if(lv_strcmp(lv_table_get_cell_value(file_explorer->file_table, i, 0), LV_SYMBOL_DIRECTORY "  home") == 0) {
            home_row = i;
        }
    }

    /* Since the default table->col_act = LV_TABLE_CELL_NONE, it is necessary to specify file_table->col_act = 0 */
    file_table->col_act = 0;
    file_table->row_act = dev_row;
    lv_obj_send_event(file_explorer->file_table, LV_EVENT_CLICKED, NULL);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_folder/dev/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    for(uint8_t i = 0; i < file_table->row_cnt; i++) {
        if(lv_strcmp(lv_table_get_cell_value(file_explorer->file_table, i, 0), LV_SYMBOL_DIRECTORY "  shm") == 0) {
            shm_row = i;
        }
    }

    file_table->row_act = shm_row;
    lv_obj_send_event(file_explorer->file_table, LV_EVENT_CLICKED, NULL);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_folder/dev/shm/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    file_table->row_act = back_row;
    lv_obj_send_event(file_explorer->file_table, LV_EVENT_CLICKED, NULL);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_folder/dev/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    file_table->row_act = back_row;
    lv_obj_send_event(file_explorer->file_table, LV_EVENT_CLICKED, NULL);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_folder/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    file_table->row_act = home_row;
    lv_obj_send_event(file_explorer->file_table, LV_EVENT_CLICKED, NULL);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_folder/home/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    for(uint8_t i = 0; i < file_table->row_cnt; i++) {
        if(lv_strcmp(lv_table_get_cell_value(file_explorer->file_table, i, 0), LV_SYMBOL_DIRECTORY "  web_user") == 0) {
            user_row = i;
        }
    }

    file_table->row_act = user_row;
    lv_obj_send_event(file_explorer->file_table, LV_EVENT_CLICKED, NULL);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_folder/home/web_user/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    rmdir("src/test_files/test_file_explorer_folder/dev/shm");
    rmdir("src/test_files/test_file_explorer_folder/dev");
    rmdir("src/test_files/test_file_explorer_folder/home/web_user");
    rmdir("src/test_files/test_file_explorer_folder/home");
    rmdir("src/test_files/test_file_explorer_folder");

}

#endif
