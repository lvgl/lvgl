#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
}

static void map_cleanup(void);

void tearDown(void)
{
    map_cleanup();
}

void test_fs_args_open_null_path(void)
{
    lv_fs_file_t f;
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_open(&f, NULL, LV_FS_MODE_RD));
}

/*Truncating needs write access*/
void test_fs_args_open_trunc_without_wr(void)
{
    lv_fs_file_t f;
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_open(&f, "A:src/test_files/trunc.txt", LV_FS_MODE_TRUNC));
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_open(&f, "A:src/test_files/trunc.txt", LV_FS_MODE_RD | LV_FS_MODE_TRUNC));
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

/*The file the map tests open, closed in tearDown so a failing assert doesn't leak it*/
static const uint8_t map_data[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
static lv_fs_file_t map_file;
static bool map_file_open;

static void map_cleanup(void)
{
    if(map_file_open) lv_fs_close(&map_file);
    map_file_open = false;
}

static lv_fs_file_t * map_open(void)
{
    lv_fs_path_ex_t path;
    lv_fs_make_path_from_buffer(&path, LV_FS_MEMFS_LETTER, map_data, sizeof(map_data), NULL);
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_open(&map_file, (const char *)&path, LV_FS_MODE_RD));
    map_file_open = true;
    return &map_file;
}

void test_fs_args_map_null(void)
{
    lv_fs_file_t * f = map_open();

    const void * ptr = NULL;
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_map(NULL, 0, 1, &ptr));
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_map(f, 0, 1, NULL));
}

/*An empty range is rejected, also at the end of the file, and the pointer is left alone*/
void test_fs_args_map_zero_length(void)
{
    lv_fs_file_t * f = map_open();

    const void * sentinel = f;
    const void * ptr = sentinel;
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_map(f, 0, 0, &ptr));
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_map(f, 10, 0, &ptr));
    TEST_ASSERT_EQUAL_PTR(sentinel, ptr);
}

void test_fs_args_unmap_null_file(void)
{
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_unmap(NULL, map_data, 1));
}

/*A real mapping with an empty length is rejected*/
void test_fs_args_unmap_zero_length(void)
{
    lv_fs_file_t * f = map_open();

    const void * ptr = NULL;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_map(f, 0, 4, &ptr));
    TEST_ASSERT_EQUAL(LV_FS_RES_INV_PARAM, lv_fs_unmap(f, ptr, 0));
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_unmap(f, ptr, 4));
}

#endif /*LV_BUILD_TEST*/
