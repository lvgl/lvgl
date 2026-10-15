#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

/*The draw descriptor API was renamed in v10*/
#if LVGL_VERSION_MAJOR < 10
    #define LV_DRAW_DSC_TYPE_FILL LV_DRAW_TASK_TYPE_FILL
#endif

static lv_draw_unit_t * saved_units;
static uint32_t saved_unit_count;
static lv_layer_t layer;
static const lv_area_t area = {0, 0, 15, 15};

void setUp(void)
{
    saved_units = LV_GLOBAL_DEFAULT()->draw_info.unit_head;
    saved_unit_count = LV_GLOBAL_DEFAULT()->draw_info.unit_cnt;
    LV_GLOBAL_DEFAULT()->draw_info.unit_head = NULL;
    LV_GLOBAL_DEFAULT()->draw_info.unit_cnt = 0;
    lv_layer_init(&layer);
}

void tearDown(void)
{
    lv_draw_layer_deinit(&layer);
    LV_GLOBAL_DEFAULT()->draw_info.unit_head = saved_units;
    LV_GLOBAL_DEFAULT()->draw_info.unit_cnt = saved_unit_count;
}

static lv_draw_task_t * append(void)
{
    return lv_draw_add_task(&layer, &area, LV_DRAW_DSC_TYPE_FILL);
}

void test_task_queue_append_after_out_of_order_completion(void)
{
    lv_draw_task_t * a = append();
    lv_draw_task_t * b = append();
    lv_draw_task_t * c = append();
    b->state = LV_DRAW_TASK_STATE_FINISHED;
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_EQUAL_PTR(a, layer.draw_task_head);
    TEST_ASSERT_EQUAL_PTR(c, a->next);
    TEST_ASSERT_EQUAL_PTR(c, layer.draw_task_tail);

    /*Complete the tail while earlier work remains, then append again.*/
    c->state = LV_DRAW_TASK_STATE_FINISHED;
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_EQUAL_PTR(a, layer.draw_task_tail);
    TEST_ASSERT_NULL(a->next);
    lv_draw_task_t * d = append();
    TEST_ASSERT_EQUAL_PTR(d, a->next);
    TEST_ASSERT_EQUAL_PTR(d, layer.draw_task_tail);

    a->state = LV_DRAW_TASK_STATE_FINISHED;
    d->state = LV_DRAW_TASK_STATE_FINISHED;
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_NULL(layer.draw_task_head);
    TEST_ASSERT_NULL(layer.draw_task_tail);
    lv_draw_task_t * e = append();
    TEST_ASSERT_EQUAL_PTR(e, layer.draw_task_head);
    TEST_ASSERT_EQUAL_PTR(e, layer.draw_task_tail);
    TEST_ASSERT_NULL(e->next);
}

void test_task_queue_deinit_clears_retained_tail(void)
{
    append();
    append();
    lv_draw_layer_deinit(&layer);
    TEST_ASSERT_NULL(layer.draw_task_head);
    TEST_ASSERT_NULL(layer.draw_task_tail);
    lv_draw_task_t * task = append();
    TEST_ASSERT_EQUAL_PTR(task, layer.draw_task_head);
    TEST_ASSERT_EQUAL_PTR(task, layer.draw_task_tail);
    TEST_ASSERT_NULL(task->next);
}

#endif
