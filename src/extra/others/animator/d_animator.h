#ifndef D_ANIMATOR_H
#define D_ANIMATOR_H

#include <glib-object.h>
#include <glib.h>

typedef struct {
    GHashTable* table;
} DAnimator;

typedef struct {
    const char* name;
    GType t;
} DAnimatorParamCfg;

typedef struct {
    DAnimatorParamCfg param;
    void (*anm_cb)(GHashTable* param_table);
} DAnimatorActionCfg;

DAnimator* d_animator_new();
void d_animator_register_action(DAnimator* anmt, DAnimatorActionCfg* cfg);
void d_animator_register_param(DAnimator* anmt, DAnimatorParamCfg* cfg);

void d_animator_anm(DAnimator* anmt, ...);
void d_animator_anm_valist(DAnimator* anmt, va_list var_args);

#endif
