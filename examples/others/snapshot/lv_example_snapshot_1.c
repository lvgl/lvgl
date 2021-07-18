#include "../../lv_examples.h"
#if LV_USE_SNAPSHOT && LV_BUILD_EXAMPLES

static void event_cb(lv_event_t* e)
{
    lv_obj_t * snapshot_obj = lv_event_get_user_data(e);
    lv_obj_t * img = lv_event_get_target(e);

    if(snapshot_obj) {
        lv_img_dsc_t* snapshot = (void*)lv_img_get_src(snapshot_obj);
        if(snapshot){
            lv_snapshot_free(snapshot);
        }

        /*Update the snapshot, we know parent of object is the container.*/
        snapshot = lv_snapshot_take(img->parent, LV_IMG_CF_TRUE_COLOR_ALPHA);
        if(snapshot == NULL)
            return;
        lv_img_set_src(snapshot_obj, snapshot);
    }
}

void lv_example_snapshot_1(void)
{
    LV_IMG_DECLARE(img_star);
    lv_obj_t * root = lv_scr_act();
    lv_obj_set_style_bg_color(root, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);

    /*Create an image object to show snapshot*/
    lv_obj_t * snapshot_obj = lv_img_create(root);
    lv_obj_set_style_bg_color(snapshot_obj, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_set_style_bg_opa(snapshot_obj, LV_OPA_100, 0);
    lv_img_set_zoom(snapshot_obj, 128);

    /*Create the container and its children*/
    lv_obj_t * container = lv_obj_create(root);

    lv_obj_align(container, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(container, 180, 180);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_radius(container, 50, 0);
    lv_obj_t * img;
    int i;
    for(i = 0; i < 4; i++) {
        img = lv_img_create(container);
        lv_img_set_src(img, &img_star);
        lv_obj_set_style_bg_color(img, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(img, LV_OPA_COVER, 0);
        lv_obj_set_style_transform_zoom(img, 400, LV_STATE_PRESSED);
        lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(img, event_cb, LV_EVENT_PRESSED, snapshot_obj);
        lv_obj_add_event_cb(img, event_cb, LV_EVENT_RELEASED, snapshot_obj);
    }
}

#endif
