#include "../../lv_examples.h"
#if LV_USE_ANIMBTN == 1 && LV_USE_RLOTTIE == 1 && LV_BUILD_EXAMPLES

void lv_example_animbtn_1(void)
{
    /*Create a lottie animation to use*/
    lv_obj_t * lottie = lv_rlottie_create_from_file(lv_scr_act(), 64, 64, "test.json");
    /*Create a animation button*/
    lv_obj_t * anim = lv_animbtn_create(lv_scr_act(), lottie);

    /*Prepare descriptors for some states*/
    lv_animbtn_state_desc_t desc;
    desc.first_frame = 0;
    desc.last_frame = 1;
    desc.control = 0;
    lv_animbtn_set_state_desc(anim, LV_ANIMBTN_STATE_RELEASED, desc);

    desc.first_frame = 1;
    desc.last_frame = ((lv_rlottie_t*)lottie)->total_frames;
    desc.control = LV_ANIMBTN_CTRL_LOOP | LV_ANIMBTN_CTRL_FORWARD;
    lv_animbtn_set_state_desc(anim, LV_ANIMBTN_STATE_CHECKED_RELEASED, desc);
    lv_obj_center(anim);
}

#endif
