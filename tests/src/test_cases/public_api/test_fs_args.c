#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_fs_args_open_null_path(void)
{
    lv_fs_file_t f;
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_open(&f, NULL, LV_FS_MODE_RD));
}

void test_fs_args_path_get_size_null_path(void)
{
    uint32_t size;
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_path_get_size(NULL, &size));
}

void test_fs_args_dir_open_null_path(void)
{
    lv_fs_dir_t dir;
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_dir_open(&dir, NULL));
}

void test_fs_args_dir_read_invalid_handle(void)
{
    char filename[256];
    lv_fs_dir_t invalid_dir = {0};

    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_dir_read(&invalid_dir, filename, sizeof(filename)));
    TEST_ASSERT_EQUAL_CHAR('\0', filename[0]);
}

void test_fs_args_dir_read_zero_length_buffer(void)
{
    char filename[256];
    lv_fs_dir_t dir;

    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_dir_open(&dir, "A:src/test_files"));
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_dir_read(&dir, filename, 0));
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_dir_close(&dir));
}

#endif /*LV_BUILD_TEST*/
