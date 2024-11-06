#include "d_animator.h"
#include "glib.h"
#include <gobject/gvaluecollector.h>
#include <stdarg.h>

#define D_ANMT_ACTION_FLAG (1 << 0)

struct DAnimatorParamDsc {
    guint flag;
    GType t;
};

struct DAnimatorActionDsc {
    struct DAnimatorParamDsc param_dsc;
    void (*anm_cb)(GHashTable* param_table);
};

DAnimator* d_animator_new()
{
    DAnimator* anmt = g_new(DAnimator, 1);
    anmt->table = g_hash_table_new(g_str_hash, g_str_equal);
    return anmt;
}

void d_animator_register_action(DAnimator* anmt, DAnimatorActionCfg* cfg)
{
    g_return_if_fail(anmt != NULL);
    g_return_if_fail(cfg != NULL);
    g_return_if_fail(g_hash_table_lookup(anmt->table, cfg->param.name) == NULL);
    struct DAnimatorActionDsc* dsc = g_new(struct DAnimatorActionDsc, 1);
    dsc->anm_cb = cfg->anm_cb;
    dsc->param_dsc.t = cfg->param.t;
    dsc->param_dsc.flag = D_ANMT_ACTION_FLAG;
    g_hash_table_insert(anmt->table, cfg->param.name, dsc);
}

void d_animator_register_param(DAnimator* anmt, DAnimatorParamCfg* cfg)
{
    g_return_if_fail(anmt != NULL);
    g_return_if_fail(cfg != NULL);
    g_return_if_fail(g_hash_table_lookup(anmt->table, cfg->name) == NULL);
    struct DAnimatorParamDsc* dsc = g_new(struct DAnimatorParamDsc, 1);
    dsc->t = cfg->t;
    g_hash_table_insert(anmt->table, cfg->name, dsc);
}

static void acts_cb(gpointer dat, gpointer ud)
{
    struct DAnimatorActionDsc* dsc = dat;
    dsc->anm_cb(ud);
}
static void param_cb(gpointer key, gpointer value, gpointer ud)
{
    GValue* v = value;
    g_value_unset(v);
}
void d_animator_anm(DAnimator* anmt, ...)
{
    g_return_if_fail(anmt != NULL);
    va_list ls;
    va_start(ls, anmt);
    d_animator_anm_valist(anmt, ls);
    va_end(ls);
}

void d_animator_anm_valist(DAnimator* anmt, va_list ls)
{
    GPtrArray* acts = g_ptr_array_new();
    GHashTable* param_table = g_hash_table_new(g_str_hash, g_str_equal);
    for (const char* name = va_arg(ls, const char*); name; name = va_arg(ls, const char*)) {
        struct DAnimatorParamDsc* dsc = g_hash_table_lookup(anmt->table, name);
        g_return_if_fail(dsc != NULL);
        GValue* v = g_new0(GValue, 1);
        const char* e = NULL;
        G_VALUE_COLLECT_INIT(v, dsc->t, ls, 0, &e);
        g_return_if_fail(e == NULL);
        g_hash_table_insert(param_table, name, v);
        if (dsc->flag & D_ANMT_ACTION_FLAG) {
            g_ptr_array_add(acts, dsc);
        }
    }
    g_ptr_array_foreach(acts, acts_cb, param_table);
    g_ptr_array_free(acts, TRUE);
    g_hash_table_foreach(param_table, param_cb, NULL);
    g_hash_table_destroy(param_table);
}
