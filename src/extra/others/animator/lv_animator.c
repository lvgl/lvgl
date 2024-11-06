#include "lv_animator.h"

#include "lvgl/lvgl.h"

static void w(GHashTable* param_table)
{
    GValue* obj_v = g_hash_table_lookup(param_table, "obj");
    g_return_if_fail(obj_v != NULL);
    GValue* w_v = g_hash_table_lookup(param_table, "w");
    g_return_if_fail(w_v != NULL);
    lv_obj_t* obj = g_value_get_pointer(obj_v);
    gint64 target = g_value_get_int64(w_v);

    lv_anim_del(obj, lv_obj_set_width);
    lv_coord_t cur = lv_obj_get_width(obj);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_width);
    lv_anim_set_values(&a, cur, target);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_time(&a, 200);

    GValue* del_cb_v = g_hash_table_lookup(param_table, "deleted_cb");
    if (del_cb_v) {
        lv_anim_deleted_cb_t cb = g_value_get_pointer(del_cb_v);
        lv_anim_set_deleted_cb(&a, cb);
    }

    lv_anim_start(&a);
}

static void y(GHashTable* param_table)
{
    GValue* obj_v = g_hash_table_lookup(param_table, "obj");
    g_return_if_fail(obj_v != NULL);
    GValue* y_v = g_hash_table_lookup(param_table, "y");
    g_return_if_fail(y_v != NULL);
    lv_obj_t* obj = g_value_get_pointer(obj_v);
    gint64 target = g_value_get_int64(y_v);

    lv_anim_del(obj, lv_obj_set_y);
    lv_coord_t cur = lv_obj_get_y_aligned(obj);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, cur, target);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_time(&a, 200);
    lv_anim_start(&a);
}

static void x(GHashTable* param_table)
{
    GValue* obj_v = g_hash_table_lookup(param_table, "obj");
    g_return_if_fail(obj_v != NULL);
    GValue* x_v = g_hash_table_lookup(param_table, "x");
    g_return_if_fail(x_v != NULL);
    lv_obj_t* obj = g_value_get_pointer(obj_v);
    gint64 target = g_value_get_int64(x_v);

    lv_anim_del(obj, lv_obj_set_x);
    lv_coord_t cur = lv_obj_get_x_aligned(obj);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&a, cur, target);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_time(&a, 200);
    lv_anim_start(&a);
}

static void opa_cb(lv_obj_t* obj, int32_t v)
{
    lv_obj_set_style_opa(obj, v, LV_PART_MAIN | LV_STATE_DEFAULT);
}
static void opa(GHashTable* param_table)
{
    GValue* obj_v = g_hash_table_lookup(param_table, "obj");
    g_return_if_fail(obj_v != NULL);
    GValue* opa_v = g_hash_table_lookup(param_table, "opa");
    g_return_if_fail(opa_v != NULL);
    lv_obj_t* obj = g_value_get_pointer(obj_v);
    gint64 opa = g_value_get_int64(opa_v);

    lv_anim_del(obj, opa_cb);
    lv_coord_t cur = lv_obj_get_style_opa(obj, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)opa_cb);
    lv_anim_set_values(&a, cur, opa);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_time(&a, 200);
    lv_anim_start(&a);
}

DAnimator* lv_animator_new()
{
    DAnimator* anmt = d_animator_new();

    DAnimatorParamCfg param_cfg[] = {
        { "obj", G_TYPE_POINTER },
        { "deleted_cb", G_TYPE_POINTER },
    };

    DAnimatorActionCfg act_cfg[] = {
        { { "w", G_TYPE_INT64 }, w },
        { { "x", G_TYPE_INT64 }, x },
        { { "y", G_TYPE_INT64 }, y },
        { { "opa", G_TYPE_INT64 }, opa },
    };

    for (int i = 0; i < G_N_ELEMENTS(param_cfg); i++) {
        d_animator_register_param(anmt, &param_cfg[i]);
    }

    for (int i = 0; i < G_N_ELEMENTS(act_cfg); i++) {
        d_animator_register_action(anmt, &act_cfg[i]);
    }

    return anmt;
}
