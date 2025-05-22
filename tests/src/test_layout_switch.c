#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#define NUM_OBJECTS 4

static lv_obj_t * active_screen = NULL;
static lv_obj_t * objects[NUM_OBJECTS] = {NULL};
static int32_t grid_col_dsc = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
static int32_t grid_row_dsc = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

static void set_layout_grid(void)
{
    lv_obj_set_grid_dsc_array(active_screen, grid_col_dsc, grid_row_dsc);
    lv_obj_set_grid_align(active_screen, LV_GRID_ALIGN_SPACE_BETWEEN, LV_GRID_ALIGN_SPACE_BETWEEN);

    for(int i = 0; i < NUM_OBJECTS; i++) {
        lv_obj_set_grid_cell(objects[i], LV_GRID_ALIGN_STRETCH, i % 2, 1, LV_GRID_ALIGN_STRETCH, i / 2, 1);
    }
    lv_obj_update_layout(active_screen);
}

static void set_layout_flex(lv_flex_flow_t flex_flow)
{
    lv_obj_set_layout(active_screen, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(active_screen, flex_flow);

    void (*obj_set_dimension)(lv_obj_t * obj, int32_t value) = NULL;

    switch(flex_flow) {
        case LV_FLEX_FLOW_COLUMN:
        case LV_FLEX_FLOW_COLUMN_WRAP:
        case LV_FLEX_FLOW_COLUMN_REVERSE:
        case LV_FLEX_FLOW_COLUMN_WRAP_REVERSE:
            obj_set_dimension = lv_obj_set_width;
            break;
        case LV_FLEX_FLOW_ROW:
        case LV_FLEX_FLOW_ROW_WRAP:
        case LV_FLEX_FLOW_ROW_REVERSE:
        case LV_FLEX_FLOW_ROW_WRAP_REVERSE:
            obj_set_dimension = lv_obj_set_height;
            break;
        default:
            LV_LOG_ERROR("Invalid flex flow");
            return;
    }

    for(int i = 0; i < NUM_OBJECTS; i++) {
        lv_obj_set_flex_grow(objects[i], 1);
        obj_set_dimension(objects[i], LV_PCT(100));
    }

    lv_obj_update_layout(active_screen);
}

void setUp(void)
{
    active_screen = lv_screen_active();
    for(int i = 0; i < NUM_OBJECTS; i++) {
        objects[i] = lv_obj_create(active_screen);
    }
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_layout_switch(void)
{
    set_layout_flex(LV_FLEX_FLOW_COLUMN);
    TEST_ASSERT_EQUAL(LV_LAYOUT_FLEX, lv_obj_get_style_layout(active_screen, LV_PART_MAIN));
    TEST_ASSERT_EQUAL(1, active_screen->h_layout);
    TEST_ASSERT_EQUAL(0, active_screen->w_layout);

    set_layout_grid();
    TEST_ASSERT_EQUAL(LV_LAYOUT_GRID, lv_obj_get_style_layout(active_screen, LV_PART_MAIN));
    TEST_ASSERT_EQUAL(1, active_screen->h_layout);
    TEST_ASSERT_EQUAL(1, active_screen->w_layout);

    set_layout_flex(LV_FLEX_FLOW_ROW);
    TEST_ASSERT_EQUAL(LV_LAYOUT_FLEX, lv_obj_get_style_layout(active_screen, LV_PART_MAIN));
    TEST_ASSERT_EQUAL(0, active_screen->h_layout);
    TEST_ASSERT_EQUAL(1, active_screen->w_layout);
}


#endif
