/**
 * @file lv_example_controller_2.c
 * @brief Navigation stack using obj controller
 */
#include "../../lv_examples.h"

#if LV_USE_OBJ_CONTROLLER && LV_USE_WIN && LV_BUILD_EXAMPLES

static void sample_controller_ctor(lv_obj_controller_t *self, void *args);

static lv_obj_t *sample_controller_create_obj(lv_obj_controller_t *self, lv_obj_t *parent);

static void sample_push_click(lv_event_t *e);

static void sample_pop_click(lv_event_t *e);

static void sample_controller_inc_click(lv_event_t *e);

typedef struct sample_controller_t {
    lv_obj_controller_t base;
    lv_obj_t *label;
    int counter;
} sample_controller_t;

static const lv_obj_controller_class_t sample_cls = {
        .constructor_cb = sample_controller_ctor,
        .create_obj_cb = sample_controller_create_obj,
        .instance_size = sizeof(sample_controller_t)
};

void lv_example_controller_2() {
    lv_obj_t *root = lv_obj_create(lv_scr_act());
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_layout(root, LV_LAYOUT_GRID);
    const static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    const static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(root, col_dsc, row_dsc);
    lv_obj_t *container = lv_obj_create(root);
    lv_obj_remove_style_all(container);
    lv_obj_set_grid_cell(container, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 0, 1);

    lv_obj_t *push_btn = lv_btn_create(root);
    lv_obj_t *push_label = lv_label_create(push_btn);
    lv_label_set_text(push_label, "Push");

    lv_obj_t *pop_btn = lv_btn_create(root);
    lv_obj_t *pop_label = lv_label_create(pop_btn);
    lv_label_set_text(pop_label, "Pop");
    lv_obj_set_grid_cell(push_btn, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(pop_btn, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_controller_manager_t *manager = lv_controller_manager_create(container, NULL);
    lv_controller_manager_replace(manager, &sample_cls, NULL);
    lv_obj_add_event_cb(push_btn, sample_push_click, LV_EVENT_CLICKED, manager);
    lv_obj_add_event_cb(pop_btn, sample_pop_click, LV_EVENT_CLICKED, manager);
}


static void sample_controller_ctor(lv_obj_controller_t *self, void *args) {
    ((sample_controller_t *) self)->counter = 0;
}

static lv_obj_t *sample_controller_create_obj(lv_obj_controller_t *self, lv_obj_t *parent) {
    sample_controller_t *controller = (sample_controller_t *) self;
    lv_obj_t *content = lv_obj_create(parent);
    lv_obj_remove_style_all(content);
    lv_obj_set_size(content, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
    lv_obj_t *label = lv_label_create(content);
    controller->label = label;
    lv_label_set_text_fmt(label, "The button has been pressed %d times", controller->counter);

    lv_obj_t *inc_btn = lv_btn_create(content);
    lv_obj_t *inc_label = lv_label_create(inc_btn);
    lv_label_set_text(inc_label, "+1");
    lv_obj_add_event_cb(inc_btn, sample_controller_inc_click, LV_EVENT_CLICKED, controller);

    return content;
}

static void sample_push_click(lv_event_t *e) {
    lv_controller_manager_t *manager = (lv_controller_manager_t *) lv_event_get_user_data(e);
    lv_controller_manager_push(manager, &sample_cls, NULL);
}

static void sample_pop_click(lv_event_t *e) {
    lv_controller_manager_t *manager = (lv_controller_manager_t *) lv_event_get_user_data(e);
    lv_controller_manager_pop(manager);
}

static void sample_controller_inc_click(lv_event_t *e) {
    sample_controller_t *controller = (sample_controller_t *) lv_event_get_user_data(e);
    controller->counter++;
    lv_label_set_text_fmt(controller->label, "The button has been pressed %d times", controller->counter);
}

#endif
