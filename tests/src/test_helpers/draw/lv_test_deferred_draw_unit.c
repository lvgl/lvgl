/**
* @file lv_test_deferred_draw_unit.c
*
*/
#if LV_BUILD_TEST

/*********************
 *      INCLUDES
 *********************/

#include "lv_test_deferred_draw_unit.h"

#include "unity/unity.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void deferred_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                               const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
static int32_t deferred_wait_for_finish(lv_draw_unit_t * draw_unit);

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_draw_unit_draw_cbs_t deferred_draw_cbs = {
    .draw_fill_cb = deferred_draw_fill,
};

static lv_draw_unit_t * deferred_unit;
static bool pending;
static uint32_t fill_cnt;
static uint32_t wait_cnt;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void deferred_draw_unit_create(void)
{
    TEST_ASSERT_NULL(deferred_unit);

    pending = false;
    fill_cnt = 0;
    wait_cnt = 0;

    /*The unit created last is the main unit*/
    deferred_unit = lv_draw_create_unit(sizeof(lv_draw_unit_t));
    deferred_unit->name = "DEFERRED";
    deferred_unit->draw_cbs = &deferred_draw_cbs;
    deferred_unit->wait_for_finish_cb = deferred_wait_for_finish;
    deferred_unit->fallback = lv_draw_sw_get_draw_unit();
}

void deferred_draw_unit_delete(void)
{
    if(deferred_unit == NULL) return;

    lv_draw_global_info_t * info = &LV_GLOBAL_DEFAULT()->draw_info;
    lv_draw_unit_t ** u = &info->unit_head;
    while(*u && *u != deferred_unit) u = &(*u)->next;
    TEST_ASSERT_NOT_NULL(*u);
    *u = deferred_unit->next;
    info->unit_cnt--;
    lv_free(deferred_unit);
    deferred_unit = NULL;
}

bool deferred_draw_unit_has_pending(void)
{
    return pending;
}

uint32_t deferred_draw_unit_get_fill_cnt(void)
{
    return fill_cnt;
}

uint32_t deferred_draw_unit_get_wait_cnt(void)
{
    return wait_cnt;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void deferred_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                               const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
    LV_UNUSED(dsc);
    LV_UNUSED(coords);

    pending = true;
    fill_cnt++;
}

static int32_t deferred_wait_for_finish(lv_draw_unit_t * draw_unit)
{
    LV_UNUSED(draw_unit);

    pending = false;
    wait_cnt++;
    return 0;
}

#endif /*LV_BUILD_TEST*/
