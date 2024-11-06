#ifndef LV_ANMT_H
#define LV_ANMT_H

#include "d_animator.h"

void lv_anmt_init();

extern DAnimator* __lv_anmt;
#define lv_obj_anim(obj, ...) d_animator_anm(__lv_anmt, "obj", obj, __VA_ARGS__)

#endif
