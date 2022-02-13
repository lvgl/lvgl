/**
 * @file lv_example_fragment_1.c
 * @brief Basic usage of obj fragment
 */
#include "../../lv_examples.h"

#if LV_USE_FRAGMENT && LV_BUILD_EXAMPLES

static void sample_fragment_ctor(lv_fragment_t * self, void * args);

static lv_obj_t * sample_fragment_create_obj(lv_fragment_t * self, lv_obj_t * parent);

static lv_obj_t * root = NULL;

struct sample_fragment_t {
    lv_fragment_t base;
    const char * name;
};

static const lv_fragment_class_t sample_cls = {
    .constructor_cb = sample_fragment_ctor,
    .create_obj_cb = sample_fragment_create_obj,
    .instance_size = sizeof(struct sample_fragment_t)
};

void lv_example_fragment_1(void)
{
    root = lv_obj_create(lv_scr_act());
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_fragment_manager_t * manager = lv_fragment_manager_create(NULL);
    lv_fragment_t * fragment = lv_fragment_create(&sample_cls, "Fragment");
    lv_fragment_manager_replace(manager, fragment, &root);
}


static void sample_fragment_ctor(lv_fragment_t * self, void * args)
{
    ((struct sample_fragment_t *) self)->name = args;
}

static lv_obj_t * sample_fragment_create_obj(lv_fragment_t * self, lv_obj_t * parent)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);;
    lv_label_set_text_fmt(label, "Hello, %s!", ((struct sample_fragment_t *) self)->name);
    return label;
}

#endif
