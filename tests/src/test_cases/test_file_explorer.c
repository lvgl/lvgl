#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

static lv_obj_t * active_screen = NULL;
static lv_obj_t * file_explorer_obj;
static lv_file_explorer_t * file_explorer;
static lv_table_t * file_table;

static void read_dir(void);

void setUp(void)
{
    active_screen = lv_screen_active();
    file_explorer_obj = lv_file_explorer_create(active_screen);
    file_explorer = (lv_file_explorer_t *)file_explorer_obj;
    file_table = (lv_table_t *)file_explorer->file_table;

    mkdir("src/test_files/test_file_explorer_folder", 0777);
    mkdir("src/test_files/test_file_explorer_folder/dev", 0777);
    int fd = open("src/test_files/test_file_explorer_folder/video.mp4", O_CREAT | O_RDWR, 0777);
    close(fd);
    fd = open("src/test_files/test_file_explorer_folder/audio.mp3", O_CREAT | O_RDWR, 0777);
    close(fd);
    fd = open("src/test_files/test_file_explorer_folder/image.jpg", O_CREAT | O_RDWR, 0777);
    close(fd);
    mkdir("src/test_files/test_file_explorer_folder/dev/shm", 0777);
    mkdir("src/test_files/test_file_explorer_folder/home", 0777);
    mkdir("src/test_files/test_file_explorer_folder/home/web_user", 0777);
}

void tearDown(void)
{
    /* Is there a way to destroy a chart without having to call remove_series for each of it series? */
    lv_obj_clean(active_screen);

    remove("src/test_files/test_file_explorer_folder/dev/shm");
    remove("src/test_files/test_file_explorer_folder/dev");
    remove("src/test_files/test_file_explorer_folder/home/web_user");
    remove("src/test_files/test_file_explorer_folder/home");
    remove("src/test_files/test_file_explorer_folder/video.mp4");
    remove("src/test_files/test_file_explorer_folder/audio.mp3");
    remove("src/test_files/test_file_explorer_folder/image.jpg");
    remove("src/test_files/test_file_explorer_folder");
}

void test_file_explorer_read_dir(void)
{
    lv_file_explorer_open_dir(file_explorer_obj, "A:src/test_files/test_file_explorer_folder");
    read_dir();
}

void test_file_explorer_read_dir_sorted(void)
{
    lv_file_explorer_open_dir(file_explorer_obj, "A:src/test_files/test_file_explorer_folder");
    lv_file_explorer_set_sort(file_explorer_obj, LV_EXPLORER_SORT_KIND);
    read_dir();
}

static void read_dir(void)
{
    uint32_t back_row = 0, dev_row = 0, shm_row = 0, home_row = 0, user_row = 0;
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_folder/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

    for(uint32_t i = 0; i < file_table->row_cnt; i++) {
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

    for(uint32_t i = 0; i < file_table->row_cnt; i++) {
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

    for(uint32_t i = 0; i < file_table->row_cnt; i++) {
        if(lv_strcmp(lv_table_get_cell_value(file_explorer->file_table, i, 0), LV_SYMBOL_DIRECTORY "  web_user") == 0) {
            user_row = i;
        }
    }

    file_table->row_act = user_row;
    lv_obj_send_event(file_explorer->file_table, LV_EVENT_CLICKED, NULL);
    TEST_ASSERT_EQUAL_STRING("A:src/test_files/test_file_explorer_folder/home/web_user/",
                             lv_file_explorer_get_current_path(file_explorer_obj));

}
#endif
