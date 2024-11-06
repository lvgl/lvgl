#include "lv_anmt.h"
#include "lv_animator.h"

extern DAnimator* __lv_anmt;
void lv_anmt_init()
{
    __lv_anmt = lv_animator_new();
}
