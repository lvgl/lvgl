#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

/*The draw descriptor API was renamed in v10*/
#if LVGL_VERSION_MAJOR < 10
    #define LV_DRAW_DSC_TYPE_FILL LV_DRAW_TASK_TYPE_FILL
    #define LV_DRAW_DSC_TYPE_LAYER LV_DRAW_TASK_TYPE_LAYER
    #define LV_EVENT_DRAW_DSC_BEGIN LV_EVENT_DRAW_TASK_ADDED
    #define LV_EVENT_DRAW_DSC_END LV_EVENT_DRAW_TASK_ADDED
    #define lv_obj_set_send_draw_dsc_events lv_obj_set_send_draw_task_events
    typedef lv_draw_dsc_base_t lv_draw_dsc_t;
#endif

/*Draw unit ids and preference scores set by the mock evaluate callbacks below*/
#define BATCH_UNIT_ID 90
#define BATCH_SCORE 20
#define IMMEDIATE_UNIT_ID 91
#define IMMEDIATE_SCORE 10

static lv_draw_unit_t * saved_units;
static uint32_t saved_unit_count;
static lv_layer_t layer;

/*The mock evaluate callbacks tell the tasks apart by the x1 of their area:
 *`area`: claimed by the batch unit
 *`immediate_area`: claimed by the batch unit, then taken over by the immediate unit
 *`unclaimed_area`: claimed by no unit, so the task fails*/
static const lv_area_t area = {0, 0, 15, 15};
static const lv_area_t immediate_area = {1, 0, 15, 15};
static const lv_area_t unclaimed_area = {2, 0, 15, 15};

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

static unsigned dispatch_calls;
static lv_draw_unit_t batch_unit, immediate_unit;

/*The batch unit claims every task except the unclaimed ones. It has `requires_all_tasks`
 *set, so it waits for the complete layer before `dispatch_batch` finishes its tasks*/
static int32_t evaluate_batch(lv_draw_unit_t * unit, lv_draw_task_t * task)
{
    LV_UNUSED(unit);
    if(task->area.x1 == unclaimed_area.x1) return 0;
    task->preferred_draw_unit_id = BATCH_UNIT_ID;
    task->preference_score = BATCH_SCORE;
    return 1;
}

/*The immediate unit is evaluated after the batch unit and takes the immediate tasks over,
 *so a layer with such a task is dispatched as usual. The tests finish these tasks by hand,
 *as if the unit completed them asynchronously*/
static int32_t evaluate_immediate(lv_draw_unit_t * unit, lv_draw_task_t * task)
{
    LV_UNUSED(unit);
    if(task->area.x1 == immediate_area.x1) {
        task->preferred_draw_unit_id = IMMEDIATE_UNIT_ID;
        task->preference_score = IMMEDIATE_SCORE;
    }
    return 1;
}

/*Dispatch callback of both units: counts the calls, and finishes the batch unit's tasks
 *once the layer is complete*/
static int32_t dispatch_batch(lv_draw_unit_t * unit, lv_layer_t * target)
{
    LV_UNUSED(unit);
    dispatch_calls++;
    if(target->all_tasks_added) {
        for(lv_draw_task_t * task = target->draw_task_head; task; task = task->next) {
            if(task->state == LV_DRAW_TASK_STATE_WAITING && task->preferred_draw_unit_id == BATCH_UNIT_ID)
                task->state = LV_DRAW_TASK_STATE_FINISHED;
        }
    }
    return LV_DRAW_UNIT_IDLE;
}

static void install_batch_units(void)
{
    lv_memzero(&batch_unit, sizeof(batch_unit));
    batch_unit.name = "test_batch";
    batch_unit.requires_all_tasks = true;
    batch_unit.evaluate_cb = evaluate_batch;
    batch_unit.dispatch_cb = dispatch_batch;
    lv_memzero(&immediate_unit, sizeof(immediate_unit));
    immediate_unit.name = "test_immediate";
    immediate_unit.evaluate_cb = evaluate_immediate;
    immediate_unit.dispatch_cb = dispatch_batch;
    batch_unit.next = &immediate_unit;
    LV_GLOBAL_DEFAULT()->draw_info.unit_head = &batch_unit;
    LV_GLOBAL_DEFAULT()->draw_info.unit_cnt = 2;
    dispatch_calls = 0;
}

void test_batch_dispatch_waits_for_complete_layer(void)
{
    install_batch_units();
    for(unsigned i = 0; i < 64; i++) {
        lv_draw_task_t * task = append();
        lv_draw_finalize_task_creation(&layer, task);
        lv_draw_dispatch_layer(NULL, &layer);
    }
    TEST_ASSERT_EQUAL_UINT(0, dispatch_calls);
    TEST_ASSERT_EQUAL_UINT(0, layer.unfinalized_tasks);
    /*The pending request must allow canvas/snapshot drain loops to proceed.*/
    lv_draw_dispatch_wait_for_request();
    layer.all_tasks_added = true;
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_GREATER_THAN_UINT(0, dispatch_calls);
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_NULL(layer.draw_task_head);
}

void test_batch_dispatch_later_unit_wins_and_async_cleanup_continues(void)
{
    install_batch_units();
    lv_draw_task_t * first = append();
    lv_draw_finalize_task_creation(&layer, first);
    lv_draw_task_t * async = lv_draw_add_task(&layer, &immediate_area, LV_DRAW_DSC_TYPE_FILL);
    lv_draw_finalize_task_creation(&layer, async);
    TEST_ASSERT_EQUAL_UINT(IMMEDIATE_UNIT_ID, async->preferred_draw_unit_id);
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_GREATER_THAN_UINT(0, dispatch_calls);
    async->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    lv_draw_task_t * last = append();
    lv_draw_finalize_task_creation(&layer, last);
    async->state = LV_DRAW_TASK_STATE_FINISHED;
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_EQUAL_PTR(last, first->next);
    /*Do not re-enable deferral after the mixed task disappears.*/
    first->state = LV_DRAW_TASK_STATE_FINISHED;
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_EQUAL_PTR(last, layer.draw_task_head);
}

void test_batch_dispatch_unfinalized_task_keeps_normal_dispatch(void)
{
    install_batch_units();
    lv_draw_task_t * outer = append();
    lv_draw_task_t * nested = append();
    lv_draw_finalize_task_creation(&layer, nested);
    TEST_ASSERT_EQUAL_UINT(1, layer.unfinalized_tasks);
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_GREATER_THAN_UINT(0, dispatch_calls);
    lv_draw_finalize_task_creation(&layer, outer);
    TEST_ASSERT_EQUAL_UINT(0, layer.unfinalized_tasks);
    /*Abandoned task creation must also balance the pending count.*/
    append();
    lv_draw_layer_deinit(&layer);
    TEST_ASSERT_EQUAL_UINT(0, layer.unfinalized_tasks);
}

void test_batch_dispatch_child_completes_while_parent_accumulates(void)
{
    install_batch_units();
    layer.display = lv_display_get_default();
    lv_layer_t * child = lv_draw_layer_create(&layer, LV_COLOR_FORMAT_ARGB8888, &area);
    TEST_ASSERT_NOT_NULL(child);
    lv_draw_task_t * blend = lv_draw_add_task(&layer, &area, LV_DRAW_DSC_TYPE_LAYER);
    ((lv_draw_image_dsc_t *)blend->draw_dsc)->src = child;
    blend->state = LV_DRAW_TASK_STATE_BLOCKED;
    lv_draw_finalize_task_creation(&layer, blend);
    lv_draw_task_t * fill = lv_draw_add_task(child, &area, LV_DRAW_DSC_TYPE_FILL);
    lv_draw_finalize_task_creation(child, fill);
    child->all_tasks_added = true;
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_EQUAL_UINT(0, dispatch_calls);
    lv_draw_dispatch_layer(NULL, child);
    lv_draw_dispatch_layer(NULL, child);
    TEST_ASSERT_NULL(child->draw_task_head);
    TEST_ASSERT_EQUAL(LV_DRAW_TASK_STATE_WAITING, blend->state);
    /*Parent still waits for its own remaining tasks.*/
    unsigned previous_calls = dispatch_calls;
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_EQUAL_UINT(previous_calls, dispatch_calls);
    layer.all_tasks_added = true;
    lv_draw_dispatch_layer(NULL, &layer);
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_NULL(layer.draw_task_head);
}

static unsigned nested_events;
static void nested_task_event(lv_event_t * event)
{
    nested_events++;
    lv_draw_task_t * nested = append();
    ((lv_draw_dsc_t *)nested->draw_dsc)->obj = lv_event_get_target_obj(event);
    lv_draw_finalize_task_creation(&layer, nested);
    TEST_ASSERT_EQUAL_UINT(1, layer.unfinalized_tasks);
}

void test_batch_dispatch_nested_task_event_finishes_before_deferral(void)
{
    install_batch_units();
    nested_events = 0;
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_send_draw_dsc_events(obj, true);
    lv_obj_add_event_cb(obj, nested_task_event, LV_EVENT_DRAW_DSC_BEGIN, NULL);
    lv_draw_task_t * outer = append();
    ((lv_draw_dsc_t *)outer->draw_dsc)->obj = obj;
    lv_draw_finalize_task_creation(&layer, outer);
    TEST_ASSERT_EQUAL_UINT(1, nested_events);
    TEST_ASSERT_EQUAL_UINT(0, layer.unfinalized_tasks);
    /*Both tasks are in the list, in the order the event handling of the version defines*/
    TEST_ASSERT_NOT_NULL(layer.draw_task_head);
    TEST_ASSERT_NOT_NULL(layer.draw_task_head->next);
    TEST_ASSERT_NULL(layer.draw_task_head->next->next);
    TEST_ASSERT_EQUAL_PTR(layer.draw_task_head->next, layer.draw_task_tail);
    TEST_ASSERT_TRUE(layer.draw_task_head == outer || layer.draw_task_head->next == outer);
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_EQUAL_UINT(0, dispatch_calls);
    lv_obj_delete(obj);
}

/*Appends a task from a draw event of the task being finalized*/
static void append_in_event(lv_event_t * event)
{
    lv_draw_task_t * added = append();
    ((lv_draw_dsc_t *)added->draw_dsc)->obj = lv_event_get_target_obj(event);
    lv_draw_finalize_task_creation(&layer, added);
}

void test_task_queue_event_keeps_the_tasks_after_the_finalized_one(void)
{
    install_batch_units();
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_send_draw_dsc_events(obj, true);
    lv_obj_add_event_cb(obj, append_in_event, LV_EVENT_DRAW_DSC_END, NULL);
    lv_draw_task_t * first = append();
    ((lv_draw_dsc_t *)first->draw_dsc)->obj = obj;
    lv_draw_task_t * second = append();
    lv_draw_finalize_task_creation(&layer, second);
    /*Finalizing a task that is not the last one keeps the later tasks behind it,
     *and the task added by the event goes after them*/
    lv_draw_finalize_task_creation(&layer, first);
    TEST_ASSERT_EQUAL_PTR(first, layer.draw_task_head);
    TEST_ASSERT_EQUAL_PTR(second, first->next);
    TEST_ASSERT_NOT_NULL(second->next);
    TEST_ASSERT_NULL(second->next->next);
    TEST_ASSERT_EQUAL_PTR(second->next, layer.draw_task_tail);
    TEST_ASSERT_EQUAL_UINT(0, layer.unfinalized_tasks);
    lv_obj_delete(obj);
}

void test_batch_dispatch_unclaimed_task_is_released(void)
{
    install_batch_units();
    lv_draw_task_t * first = append();
    lv_draw_finalize_task_creation(&layer, first);
    lv_draw_task_t * unsupported = lv_draw_add_task(&layer, &unclaimed_area, LV_DRAW_DSC_TYPE_FILL);
    lv_draw_finalize_task_creation(&layer, unsupported);
    TEST_ASSERT_EQUAL(LV_DRAW_TASK_STATE_FAILED, unsupported->state);
    lv_draw_dispatch_layer(NULL, &layer);
    TEST_ASSERT_EQUAL_PTR(first, layer.draw_task_head);
    TEST_ASSERT_NULL(first->next);
    TEST_ASSERT_EQUAL_UINT(0, layer.unfinalized_tasks);
}
#endif
