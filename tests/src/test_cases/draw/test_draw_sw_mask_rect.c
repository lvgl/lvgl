#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

#if LV_USE_DRAW_SW && LV_DRAW_SW_COMPLEX

#define BUF_SIZE 100

/*The layer buffer covers (100,100)..(199,199) in absolute coordinates.*/
#define BUF_X1 100
#define BUF_Y1 100

static lv_draw_buf_t * mask_rect_buf_create(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(BUF_SIZE, BUF_SIZE, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /*Fill with opaque white so any masking is visible in the alpha channel*/
    uint32_t y;
    for(y = 0; y < BUF_SIZE; y++) {
        lv_color32_t * row = lv_draw_buf_goto_xy(buf, 0, y);
        uint32_t x;
        for(x = 0; x < BUF_SIZE; x++) {
            row[x].red = 0xff;
            row[x].green = 0xff;
            row[x].blue = 0xff;
            row[x].alpha = 0xff;
        }
    }

    return buf;
}

static void mask_rect_run(lv_draw_buf_t * buf, const lv_area_t * clip_area, const lv_area_t * mask_area,
                          int32_t radius)
{
    lv_layer_t layer;
    lv_memzero(&layer, sizeof(layer));
    layer.draw_buf = buf;
    layer.color_format = LV_COLOR_FORMAT_ARGB8888;
    lv_area_set(&layer.buf_area, BUF_X1, BUF_Y1, BUF_X1 + BUF_SIZE - 1, BUF_Y1 + BUF_SIZE - 1);

    lv_draw_mask_rect_dsc_t dsc;
    lv_draw_mask_rect_dsc_init(&dsc);
    dsc.area = *mask_area;
    dsc.radius = radius;
    dsc.keep_outside = 1;

    lv_draw_task_t t;
    lv_memzero(&t, sizeof(t));
    t.target_layer = &layer;
    t.clip_area = *clip_area;

    lv_draw_sw_mask_rect(&t, &dsc);
}

static lv_opa_t mask_rect_alpha_at(lv_draw_buf_t * buf, uint32_t x, uint32_t y)
{
    lv_color32_t * row = lv_draw_buf_goto_xy(buf, 0, y);
    TEST_ASSERT_NOT_NULL(row);
    return row[x].alpha;
}

/*
 * The masked area is clipped to the target layer's buffer.
 *
 * `clip_area` may start above and to the left of `buf_area`, which used to make the
 * offsets handed to lv_draw_layer_go_to_xy() negative.  Those wrapped to huge unsigned
 * values, the row pointer came back NULL, and the row loop dereferenced it.
 */
void test_mask_rect_area_starting_before_the_buffer(void)
{
    lv_draw_buf_t * buf = mask_rect_buf_create();

    lv_area_t clip_area;
    lv_area_set(&clip_area, 0, 0, 299, 299);

    lv_area_t mask_area;
    lv_area_set(&mask_area, 50, 50, 190, 190);

    mask_rect_run(buf, &clip_area, &mask_area, 10);

    /*Well inside the mask — left untouched*/
    TEST_ASSERT_EQUAL_UINT8(0xff, mask_rect_alpha_at(buf, 0, 0));

    /*(190,190) absolute is the rounded corner of the mask — alpha reduced*/
    TEST_ASSERT_LESS_THAN_UINT8(0xff, mask_rect_alpha_at(buf, 90, 90));

    /*Past the mask with keep_outside set — left untouched*/
    TEST_ASSERT_EQUAL_UINT8(0xff, mask_rect_alpha_at(buf, 95, 95));

    lv_draw_buf_destroy(buf);
}

/*
 * The same clipping bounds the row width.
 *
 * A mask area extending past the right edge of the buffer used to leave `area_w` wider
 * than the buffer, so lv_memzero() and the alpha loop wrote past the end of each row.
 */
void test_mask_rect_area_extending_past_the_buffer(void)
{
    lv_draw_buf_t * buf = mask_rect_buf_create();

    lv_area_t clip_area;
    lv_area_set(&clip_area, 0, 0, 999, 999);

    lv_area_t mask_area;
    lv_area_set(&mask_area, 100, 100, 400, 400);

    mask_rect_run(buf, &clip_area, &mask_area, 10);

    /*The whole buffer is inside the mask, so nothing is cleared*/
    TEST_ASSERT_EQUAL_UINT8(0xff, mask_rect_alpha_at(buf, 99, 99));

    lv_draw_buf_destroy(buf);
}

/*
 * A mask area that misses the buffer entirely is a no-op.
 */
void test_mask_rect_area_outside_the_buffer(void)
{
    lv_draw_buf_t * buf = mask_rect_buf_create();

    lv_area_t clip_area;
    lv_area_set(&clip_area, 0, 0, 50, 50);

    lv_area_t mask_area;
    lv_area_set(&mask_area, 0, 0, 40, 40);

    mask_rect_run(buf, &clip_area, &mask_area, 10);

    TEST_ASSERT_EQUAL_UINT8(0xff, mask_rect_alpha_at(buf, 0, 0));
    TEST_ASSERT_EQUAL_UINT8(0xff, mask_rect_alpha_at(buf, 99, 99));

    lv_draw_buf_destroy(buf);
}

#else /*LV_USE_DRAW_SW && LV_DRAW_SW_COMPLEX*/

void test_mask_rect_area_starting_before_the_buffer(void)
{
    TEST_IGNORE_MESSAGE("Requires the software renderer with complex drawing");
}

void test_mask_rect_area_extending_past_the_buffer(void)
{
    TEST_IGNORE_MESSAGE("Requires the software renderer with complex drawing");
}

void test_mask_rect_area_outside_the_buffer(void)
{
    TEST_IGNORE_MESSAGE("Requires the software renderer with complex drawing");
}

#endif /*LV_USE_DRAW_SW && LV_DRAW_SW_COMPLEX*/

#endif
