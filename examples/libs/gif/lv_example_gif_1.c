#include "../../lv_examples.h"
#if LV_USE_GIF && LV_BUILD_EXAMPLES

/**
 * Open a gif from a file and array too
 */
void lv_example_gif_1(void)
{
    extern uint8_t blub_gif[];
    lv_gif_create_from_data(lv_scr_act(), blub_gif);


}

#endif
