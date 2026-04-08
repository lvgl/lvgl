#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

#define TEST_DRAW_BUF_SAVE_PATH(path) "A:src/test_files/draw_buf_save_" path

/* Test setup and teardown */
void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static int compare_int(const void * ref_p, const void * pElement)
{
    int ref = *(const int *)ref_p;
    int element = *(const int *)pElement;
    return ref - element;
}

static int compare_string(const void * ref_p, const void * pElement)
{
    const char * ref = *(const char **)ref_p;
    const char * element = *(const char **)pElement;
    return lv_strcmp(ref, element);
}

/* Test empty array case */
void test_utils_bsearch_empty_array(void)
{
    int * array = NULL;
    int key = 5;
    int * result = (int *)lv_utils_bsearch(&key, array, 0, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result);
}

/* Test single element array */
void test_utils_bsearch_single_element(void)
{
    int array[] = {5};

    /* Test existing element */
    int key1 = 5;
    int * result1 = (int *)lv_utils_bsearch(&key1, array, 1, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result1);
    TEST_ASSERT_EQUAL_INT(5, *result1);

    /* Test non-existent element */
    int key2 = 1;
    int * result2 = (int *)lv_utils_bsearch(&key2, array, 1, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result2);
}

/* Test finding middle element */
void test_utils_bsearch_middle_element(void)
{
    int array[] = {1, 3, 5, 7, 9};
    int key = 5;
    int * result = (int *)lv_utils_bsearch(&key, array, 5, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(5, *result);
}

/* Test case when key is not found */
void test_utils_bsearch_not_found(void)
{
    int array[] = {1, 3, 5, 7, 9};
    int key = 2;
    int * result = (int *)lv_utils_bsearch(&key, array, 5, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result);
}

/* Test finding first element */
void test_utils_bsearch_first_element(void)
{
    int array[] = {1, 3, 5, 7, 9};
    int key = 1;
    int * result = (int *)lv_utils_bsearch(&key, array, 5, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(1, *result);
}

/* Test finding last element */
void test_utils_bsearch_last_element(void)
{
    int array[] = {1, 3, 5, 7, 9};
    int key = 9;
    int * result = (int *)lv_utils_bsearch(&key, array, 5, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(9, *result);
}

/* Test string array search */
void test_utils_bsearch_string_array(void)
{
    const char * array[] = {"apple", "banana", "cherry", "date", "elderberry"};
    const char * key = "cherry";
    const char ** result = (const char **)lv_utils_bsearch(&key, array, 5, sizeof(const char *), compare_string);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("cherry", *result);
}

/* Test string not found case */
void test_utils_bsearch_string_not_found(void)
{
    const char * array[] = {"apple", "banana", "cherry", "date", "elderberry"};
    const char * key = "fig";
    const char ** result = (const char **)lv_utils_bsearch(&key, array, 5, sizeof(const char *), compare_string);
    TEST_ASSERT_NULL(result);
}

/* Test searching with repeated keys */
void test_utils_bsearch_repeated_key(void)
{
    const char * array[] = {"apple", "banana", "apple", "date", "elderberry"};
    const char * key = "apple";
    const char ** result = (const char **)lv_utils_bsearch(&key, array, 5, sizeof(const char *), compare_string);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("apple", *result);
}

/* Test large array (1000 elements) */
void test_utils_bsearch_large_array(void)
{
#define LARGE_ARRAY_SIZE 1000
    int large_array[LARGE_ARRAY_SIZE];
    for(int i = 0; i < LARGE_ARRAY_SIZE; i++) {
        large_array[i] = i * 2;
    }

    /* Test finding existing element */
    int key = 500;
    int * result = (int *)lv_utils_bsearch(&key, large_array, LARGE_ARRAY_SIZE, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(500, *result);

    /* Test non-existent element */
    key = 501;
    result = (int *)lv_utils_bsearch(&key, large_array, LARGE_ARRAY_SIZE, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result);
}

/* Test n=2 boundary case - important for binary search algorithm */
void test_utils_bsearch_two_element_array(void)
{
    int array[] = {1, 3};

    /* Test finding first element */
    int key1 = 1;
    int * result1 = (int *)lv_utils_bsearch(&key1, array, 2, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result1);
    TEST_ASSERT_EQUAL_INT(1, *result1);

    /* Test finding second element */
    int key2 = 3;
    int * result2 = (int *)lv_utils_bsearch(&key2, array, 2, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result2);
    TEST_ASSERT_EQUAL_INT(3, *result2);

    /* Test element smaller than first */
    int key3 = 0;
    int * result3 = (int *)lv_utils_bsearch(&key3, array, 2, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result3);

    /* Test element between first and second */
    int key4 = 2;
    int * result4 = (int *)lv_utils_bsearch(&key4, array, 2, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result4);

    /* Test element larger than second */
    int key5 = 4;
    int * result5 = (int *)lv_utils_bsearch(&key5, array, 2, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result5);
}

/* Test n=3 boundary case - odd number of elements */
void test_utils_bsearch_three_element_array(void)
{
    int array[] = {1, 2, 3};

    /* Test finding first element */
    int key1 = 1;
    int * result1 = (int *)lv_utils_bsearch(&key1, array, 3, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result1);
    TEST_ASSERT_EQUAL_INT(1, *result1);

    /* Test finding second element */
    int key2 = 2;
    int * result2 = (int *)lv_utils_bsearch(&key2, array, 3, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result2);
    TEST_ASSERT_EQUAL_INT(2, *result2);

    /* Test finding third element */
    int key3 = 3;
    int * result3 = (int *)lv_utils_bsearch(&key3, array, 3, sizeof(int), compare_int);
    TEST_ASSERT_NOT_NULL(result3);
    TEST_ASSERT_EQUAL_INT(3, *result3);

    /* Test element not found */
    int key4 = 4;
    int * result4 = (int *)lv_utils_bsearch(&key4, array, 3, sizeof(int), compare_int);
    TEST_ASSERT_NULL(result4);
}

void test_draw_buf_save_to_file_open_fail(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Use invalid path to test open failure */
    lv_result_t res = lv_draw_buf_save_to_file(draw_buf, "Z:invalid/path/test.bin");
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);

    lv_draw_buf_destroy(draw_buf);
}

/* Test RGB565 format save */
void test_draw_buf_save_to_file_rgb565_format(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(100, 100, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    lv_result_t res = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("rgb565_output.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    lv_draw_buf_destroy(draw_buf);
}

/* Test minimal size buffer save */
void test_draw_buf_save_to_file_minimal_size(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(1, 1, LV_COLOR_FORMAT_A8, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    lv_result_t res = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("minimal_output.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    lv_draw_buf_destroy(draw_buf);
}

/* Test different file extensions */
void test_draw_buf_save_to_file_different_extensions(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);

    /* Test with different file extensions using default file system */
    lv_result_t res1 = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("test.bin"));
    lv_result_t res2 = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("test.raw"));
    lv_result_t res3 = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("test.dat"));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, res1);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res2);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res3);

    lv_draw_buf_destroy(draw_buf);
}

/* Test zero data size */
void test_draw_buf_save_to_file_zero_data_size(void)
{
    lv_draw_buf_t draw_buf;
    lv_result_t res1 = lv_draw_buf_init(&draw_buf, 0, 0, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO,
                                        NULL, 0);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res1);

    lv_result_t res = lv_draw_buf_save_to_file(&draw_buf, TEST_DRAW_BUF_SAVE_PATH("zero_size.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
}

/* Test indexed color format */
void test_draw_buf_save_to_file_indexed_format(void)
{
    lv_draw_buf_t draw_buf;
    lv_result_t res1 = lv_draw_buf_init(&draw_buf, 0, 0, LV_COLOR_FORMAT_RGB888, LV_STRIDE_AUTO,
                                        NULL, 0);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res1);

    lv_result_t res = lv_draw_buf_save_to_file(&draw_buf, TEST_DRAW_BUF_SAVE_PATH("indexed.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
}

void test_draw_buf_save_to_file_and_read_decode(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_XRGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    for(int32_t y = 0; y < 10; y++) {
        for(int32_t x = 0; x < 10; x++) {
            lv_color32_t * data = lv_draw_buf_goto_xy(draw_buf, x, y);
            data->blue = 0x00;
            data->red = 0xff;
            data->green = 0x00;
            data->alpha = 0xff;
        }
    }

    lv_result_t res = lv_draw_buf_save_to_file(draw_buf, TEST_DRAW_BUF_SAVE_PATH("decode_XRGB8888.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    lv_obj_t * image = lv_image_create(lv_screen_active());

    lv_image_set_src(image, TEST_DRAW_BUF_SAVE_PATH("decode_XRGB8888.bin"));

    lv_obj_center(image);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw_buf_save_to_file_decode_XRGB8888.png");

    lv_draw_buf_destroy(draw_buf);

}

#endif
