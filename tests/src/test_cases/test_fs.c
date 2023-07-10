#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"


const char * read_exp =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam sed maximus orci. Morbi massa nisi, varius eu convallis ac, venenatis at metus. In in nibh id urna pretium feugiat vitae eu libero. Ut eget fringilla eros. Nunc ullamcorper lectus mauris, vel rhoncus velit volutpat et. Phasellus sed molestie massa. Maecenas quis dui sollicitudin, vulputate nunc ut, dictum quam. Nam a congue lorem. Nulla non facilisis sapien. Ut luctus nulla nibh, sed finibus urna porta non. Duis aliquet augue id urna euismod auctor. Integer pellentesque vulputate enim non mattis. Donec finibus mattis dolor, et feugiat nisi pharetra porta. Mauris ullamcorper cursus magna. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus.";

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}
#include <stdio.h>
void test_read(void)
{
    lv_fs_res_t res;

    /*'A' has cache*/
    lv_fs_file_t fa;
    res = lv_fs_open(&fa, "A:src/test_files/readtest.txt", LV_FS_MODE_RD);
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, res);

    /*'B' has no cache*/
    lv_fs_file_t fb;
    res = lv_fs_open(&fb, "B:src/test_files/readtest.txt", LV_FS_MODE_RD);
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, res);

    /*Use an odd size to make sure it's not aligned with the drivier's'cache size*/
    uint8_t buf[79];
    uint32_t cnt = 0;
    uint32_t br = 1;
    while(br) {
        res = lv_fs_read(&fa, buf, sizeof(buf), &br);
        TEST_ASSERT_EQUAL(LV_FS_RES_OK, res);
        TEST_ASSERT_TRUE(memcmp(buf, read_exp + cnt, br) == 0);

        res = lv_fs_read(&fb, buf, sizeof(buf), &br);
        TEST_ASSERT_EQUAL(LV_FS_RES_OK, res);
        TEST_ASSERT_TRUE(memcmp(buf, read_exp + cnt, br) == 0);
        cnt += br;
    }

    lv_fs_close(&fa);
    lv_fs_close(&fb);
}

#endif
