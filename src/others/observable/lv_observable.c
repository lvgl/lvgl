#include "lv_observable.h"

#include "../../../misc/lv_ll.h"
//#include "../../../misc/lv_style.h"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

//lv_event_code_t LV_EVENT_OBSERVABLE_CHANGED;

/**********************
 *      MACROS
 **********************/

 /**********************
  *   GLOBAL FUNCTIONS
  **********************/


void lv_observable_init_int(lv_observable_t* obs, int32_t value)
{
    obs->type = LV_TYPE_INT;
    _lv_ll_init(&(obs->subs_ll), sizeof(lv_observable_sub_dsc_t));
    lv_observable_set_int(obs, value);
}

bool lv_cond_uc(lv_observable_value_t v1, lv_observable_value_t v2)
{
    return true;
}

bool lv_cond_int_eq(lv_observable_value_t v1, lv_observable_value_t v2)
{
    return v1.num == v2.num;
}

bool lv_cond_int_gt(lv_observable_value_t v1, lv_observable_value_t v2)
{
    return v1.num > v2.num;
}

bool lv_cond_int_ge(lv_observable_value_t v1, lv_observable_value_t v2)
{
    return v1.num >= v2.num;
}

bool lv_cond_int_ne(lv_observable_value_t v1, lv_observable_value_t v2)
{
    return v1.num != v2.num;
}

bool lv_cond_int_lt(lv_observable_value_t v1, lv_observable_value_t v2)
{
    return v1.num < v2.num;
}

bool lv_cond_int_le(lv_observable_value_t v1, lv_observable_value_t v2)
{
    return v1.num <= v2.num;
}

const lv_cond_fn_t lv_cond_int_fn[LV_COND_NUM] = {
    NULL,
    lv_cond_int_eq,
    lv_cond_int_ne,
    lv_cond_int_lt,
    lv_cond_int_le,
    lv_cond_int_gt,
    lv_cond_int_ge,
};

static void notify(lv_observable_t* obs)
{
    lv_observable_sub_dsc_t* s;
    _LV_LL_READ(&(obs->subs_ll), s) {
        if (s->callback) {
            s->callback(s, s->cond ? s->cond(obs->value, s->cond_val) : true);
        }
    }
}

void* lv_observable_subscribe_obj(lv_observable_t* obs, lv_observable_cb_t cb, lv_obj_t* obj, void* data1, void* data2, lv_cond_fn_t cond, lv_observable_value_t cond_val)
{
    lv_observable_sub_dsc_t* s = _lv_ll_ins_tail(&(obs->subs_ll));
    LV_ASSERT_MALLOC(s);
    if (s == NULL) return NULL;

    lv_memset_00(s, sizeof(*s));

    s->cond = cond;
    s->cond_val = cond_val;
    s->obj = obj;
    s->callback = cb;
    s->data1 = data1;
    s->data2 = data2;

    notify(obs);        /**/
    return s;
}


void lv_observable_set_int(lv_observable_t* obs, int32_t value)
{
    obs->value.num = value;
    notify(obs);
}

void lv_observable_set_obj_flag_cb(void* s, bool cond)
{
    lv_observable_sub_dsc_t* dsc = (lv_observable_sub_dsc_t*)s;
    if (cond) {
        lv_obj_add_flag(dsc->obj, (lv_obj_flag_t)dsc->data1);
    }
    else {
        lv_obj_clear_flag(dsc->obj, (lv_obj_flag_t)dsc->data1);
    }
}

void lv_bind_int_to_obj_flag_cond(lv_observable_t* obs, lv_obj_t* obj, lv_obj_flag_t flag, lv_cond_t cond, int32_t cond_val)
{
    lv_observable_value_t v;

    v.num = cond_val;
    lv_observable_subscribe_obj(obs, lv_observable_set_obj_flag_cb, obj, (void*)flag, NULL, lv_cond_int_fn[cond], v);
}

void lv_bind_int_to_obj_flag(lv_observable_t* obs, lv_obj_t* obj, lv_obj_flag_t flag)
{
    lv_bind_int_to_obj_flag_cond(obs, obj, flag, LV_COND_NE, 0);
}

#if 0

/*
void testIntToHiddenFlagBinding(lv_observable_t *numBoxes, lv_obj_t *box1, lv_obj_t *box2) {
    lv_observable_init_int(numBoxes, 0);

    lv_bind_int_to_obj_flag_cond(numBoxes, box1, LV_OBJ_FLAG_HIDDEN, LV_COND_LT, 1);
    lv_bind_int_to_obj_flag_cond(numBoxes, box2, LV_OBJ_FLAG_HIDDEN, LV_COND_LT, 2);
}
*/

void ui_extra_init(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xffffff), 0);

    box1 = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(box1);
    lv_obj_set_size(box1, 100, 100);
    lv_obj_set_align(box1, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(box1, 50);
    lv_obj_set_style_bg_opa(box1, 255, 0);
    lv_obj_set_style_bg_color(box1, lv_color_hex(0xff0000), 0);

    box2 = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(box2);
    lv_obj_set_size(box2, 100, 100);
    lv_obj_set_align(box2, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(box2, 200);
    lv_obj_set_style_bg_opa(box2, 255, 0);
    lv_obj_set_style_bg_color(box2, lv_color_hex(0x00ff00), 0);

    box3 = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(box3);
    lv_obj_set_size(box3, 100, 100);
    lv_obj_set_align(box3, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(box3, 350);
    lv_obj_set_style_bg_opa(box3, 255, 0);
    lv_obj_set_style_bg_color(box3, lv_color_hex(0x0000ff), 0);

    label = lv_label_create(lv_scr_act());
    //	lv_obj_remove_style_all(label);
    lv_obj_set_size(label, 100, 100);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_obj_set_x(label, 350);

    lv_observable_init_int(&numBoxes, 0);

    lv_bind_int_to_obj_flag_cond(&numBoxes, box1, LV_OBJ_FLAG_HIDDEN, LV_COND_LT, 1);
    lv_bind_int_to_obj_flag_cond(&numBoxes, box2, LV_OBJ_FLAG_HIDDEN, LV_COND_LT, 2);
    lv_bind_int_to_obj_flag_cond(&numBoxes, box3, LV_OBJ_FLAG_HIDDEN, LV_COND_LT, 3);

    ui_blink_start(box3, 500);
}

#endif
