#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_FFMPEG

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void create_image_item(lv_obj_t * parent, const void * src, const char * text)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 300, 200);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * img = lv_image_create(cont);
    lv_image_set_src(img, src);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, text);
}

static void create_images(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_clean(screen);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    create_image_item(screen, "A:src/test_assets/test_img_lvgl_logo.png", "PNG File (32 bit)");
    create_image_item(screen, "A:src/test_assets/test_img_lvgl_logo_png_no_ext", "PNG File (32 bit) No Extension");
    create_image_item(screen, "A:src/test_assets/test_img_lvgl_logo_8bit_palette.png", "PNG File (8 bit palette)");
}

void test_ffmpeg_image_decoder_1(void)
{
    /* Temporarily remove other decoder */
#if LV_USE_LODEPNG
    lv_lodepng_deinit();
#endif

#if LV_USE_LIBPNG
    lv_libpng_deinit();
#endif

    create_images();

    /* Should decode images consistently with other PNG decoders  */
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/ffmpeg_1.png");

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        create_images();

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/ffmpeg_1.png");

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 32);

    /* Re-add other decoder */
#if LV_USE_LODEPNG
    lv_lodepng_init();
#endif

#if LV_USE_LIBPNG
    lv_libpng_init();
#endif

    lv_obj_clean(lv_screen_active());
}

void test_ffmpeg_player_1(void)
{
    lv_obj_t * player = lv_ffmpeg_player_create(lv_screen_active());
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_obj_center(player);

    lv_ffmpeg_player_set_src(player, "A:ERROR_FILE.mp4");
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/ffmpeg_player_error_file.png");

    /* Video: test_video_birds.mp4 Update frame rate 25FPS */
    lv_ffmpeg_player_set_src(player, "A:src/test_assets/test_video_birds.mp4");

    /* Not started, it should be in the black screen  */
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/ffmpeg_player_frame_0.png");

    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);

    lv_test_wait(400);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/ffmpeg_player_frame_1.png");

    lv_test_wait(400);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/ffmpeg_player_frame_2.png");

    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_PAUSE);

    lv_test_wait(400);

    /* Paused, it should be in the same frame  */
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/ffmpeg_player_frame_2.png");

    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_RESUME);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/ffmpeg_player_frame_3.png");

    lv_obj_delete(player);
}

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_ffmpeg_image_decoder_1(void)
{
}

void test_ffmpeg_player_1(void)
{
}

#endif /* LV_USE_FFMPEG */

#endif /* LV_BUILD_TEST */
