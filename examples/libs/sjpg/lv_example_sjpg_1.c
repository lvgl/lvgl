#include "../../lv_examples.h"
#if LV_USE_SJPG && LV_BUILD_EXAMPLES

/**
 * Load an SJPG image
 */
void lv_example_sjpg_1(void)
{
    lv_obj_t * wp;

    LV_IMAGE_DECLARE(codeblocks)

    wp = lv_image_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_image_set_src(wp, "A:codeblocks.jpg");
    //    lv_image_set_src(wp, &codeblocks);
    lv_obj_center(wp);
}

#endif
