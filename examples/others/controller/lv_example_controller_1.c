/**
 * @file lv_example_controller_1.c
 * @brief Basic usage of obj controller
 */
#include "../../lv_examples.h"

#if LV_USE_OBJ_CONTROLLER && LV_BUILD_EXAMPLES

static void sample_controller_ctor(lv_obj_controller_t *self, void *args);

static lv_obj_t *sample_controller_create_obj(lv_obj_controller_t *self, lv_obj_t *parent);

struct sample_controller_t {
    lv_obj_controller_t base;
    const char *name;
};

static const lv_obj_controller_class_t sample_cls = {
        .constructor_cb = sample_controller_ctor,
        .create_obj_cb = sample_controller_create_obj,
        .instance_size = sizeof(struct sample_controller_t)
};

void lv_example_controller_1() {
    lv_obj_t *root = lv_obj_create(lv_scr_act());
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_controller_manager_t *manager = lv_controller_manager_create(root, NULL);
    lv_controller_manager_replace(manager, &sample_cls, "Controller");
}


static void sample_controller_ctor(lv_obj_controller_t *self, void *args) {
    ((struct sample_controller_t *) self)->name = args;
}

static lv_obj_t *sample_controller_create_obj(lv_obj_controller_t *self, lv_obj_t *parent) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text_fmt(label, "Hello, %s!", ((struct sample_controller_t *) self)->name);
    return label;
}

#endif
