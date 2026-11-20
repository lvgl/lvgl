#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

static lv_obj_t * tv;

void setUp(void)
{
    tv = lv_tileview_create(lv_screen_active());
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_tileview_forgets_a_deleted_active_tile(void)
{
    lv_obj_t * tile1 = lv_tileview_add_tile(tv, 0, 0, LV_DIR_RIGHT);
    lv_obj_t * tile2 = lv_tileview_add_tile(tv, 1, 0, LV_DIR_LEFT);

    lv_tileview_set_tile(tv, tile2, LV_ANIM_OFF);
    TEST_ASSERT_EQUAL_PTR(tile2, lv_tileview_get_tile_active(tv));

    /*Deleting another tile keeps the active one*/
    lv_obj_delete(tile1);
    TEST_ASSERT_EQUAL_PTR(tile2, lv_tileview_get_tile_active(tv));

    lv_obj_delete(tile2);
    TEST_ASSERT_NULL(lv_tileview_get_tile_active(tv));
}

void test_tileview_forgets_a_cleaned_or_moved_active_tile(void)
{
    lv_obj_t * tile = lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR);
    lv_tileview_set_tile(tv, tile, LV_ANIM_OFF);
    lv_obj_clean(tv);
    TEST_ASSERT_NULL(lv_tileview_get_tile_active(tv));

    tile = lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR);
    lv_tileview_set_tile(tv, tile, LV_ANIM_OFF);
    lv_obj_set_parent(tile, lv_screen_active());
    TEST_ASSERT_NULL(lv_tileview_get_tile_active(tv));
}

void test_tileview_set_tile_ignores_other_objects(void)
{
    lv_obj_t * tile = lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR);
    lv_tileview_set_tile(tv, tile, LV_ANIM_OFF);

    /*A plain child, and a tile of another tile view, are not tiles of this one*/
    lv_obj_t * plain = lv_obj_create(tv);
    lv_tileview_set_tile(tv, plain, LV_ANIM_OFF);
    TEST_ASSERT_EQUAL_PTR(tile, lv_tileview_get_tile_active(tv));

    lv_obj_t * other_tv = lv_tileview_create(lv_screen_active());
    lv_obj_t * other_tile = lv_tileview_add_tile(other_tv, 0, 0, LV_DIR_HOR);
    lv_tileview_set_tile(tv, other_tile, LV_ANIM_OFF);
    TEST_ASSERT_EQUAL_PTR(tile, lv_tileview_get_tile_active(tv));
}

void test_tileview_set_tile_by_index_skips_plain_children(void)
{
    /*A plain child at the tile's position comes first*/
    lv_obj_t * plain = lv_obj_create(tv);
    lv_obj_set_size(plain, LV_PCT(100), LV_PCT(100));
    lv_obj_t * tile = lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR);

    lv_tileview_set_tile_by_index(tv, 0, 0, LV_ANIM_OFF);
    TEST_ASSERT_EQUAL_PTR(tile, lv_tileview_get_tile_active(tv));
    TEST_ASSERT_TRUE(plain != lv_tileview_get_tile_active(tv));
}

#endif
