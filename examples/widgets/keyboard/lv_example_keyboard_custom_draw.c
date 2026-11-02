#include "../../lv_examples.h"
#if LV_USE_KEYBOARD && LV_BUILD_EXAMPLES

static void event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_draw_dsc_t * base_dsc = lv_event_get_draw_dsc(e);
    bool pressed = false;
    if(lv_keyboard_get_selected_button(obj) == base_dsc->id1 && lv_obj_has_state(obj, LV_STATE_PRESSED)) {
        pressed = true;
    }

    /*When the keyboard draws the buttons...*/
    if(base_dsc->part == LV_PART_ITEMS) {
        /*Get a color based on the button's index*/
        lv_palette_t palette = (lv_palette_t)(base_dsc->id1 % LV_PALETTE_LAST);
        lv_draw_dsc_type_t type = lv_draw_dsc_get_type(base_dsc);
        if(type == LV_DRAW_DSC_TYPE_FILL) {
            lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_dsc_get_fill_dsc(base_dsc);
            fill_draw_dsc->color = pressed ? lv_palette_darken(palette, 3) : lv_palette_main(palette);
        }
        if(type == LV_DRAW_DSC_TYPE_LABEL) {
            lv_draw_label_dsc_t * label_draw_dsc = lv_draw_dsc_get_label_dsc(base_dsc);
            /*For the OK symbol, draw a star instead*/
            if(lv_strcmp(label_draw_dsc->text, LV_SYMBOL_OK) == 0) {
                label_draw_dsc->opa = 0;    /*Hide the label*/

                LV_IMAGE_DECLARE(img_star);
                lv_image_header_t header;
                lv_result_t res = lv_image_decoder_get_info(&img_star, &header);
                if(res != LV_RESULT_OK) return;

                lv_area_t a;
                lv_area_set(&a, 0, 0, header.w - 1, header.h - 1);
                lv_area_t label_area = label_draw_dsc->coords;
                lv_area_align(&label_area, &a, LV_ALIGN_CENTER, 0, 0);

                lv_draw_image_dsc_t img_draw_dsc;
                lv_draw_image_dsc_init(&img_draw_dsc);
                img_draw_dsc.src = &img_star;
                lv_draw_image(base_dsc->layer, &img_draw_dsc, &a);
            }
            else {
                /*For the other labels just pick an lighter color*/
                label_draw_dsc->color = lv_palette_lighten(palette, 4);
            }
        }

    }
}

/**
 * @title Per-button colors via draw events
 * @brief Recolor each keyboard key and swap the OK symbol for a star image.
 *
 * A centered `lv_keyboard` has `LV_OBJ_FLAG_SEND_DRAW_DSC_EVENTS` set and a
 * `LV_EVENT_DRAW_DSC_BEGIN` callback attached. For every `LV_PART_ITEMS` draw
 * descriptor the handler picks a palette entry from `base_dsc->id1`, darkens it
 * while the button is in `LV_STATE_PRESSED`, and lightens the label text. When
 * the label is `LV_SYMBOL_OK` it is hidden and `img_star` is drawn in its place.
 */
void lv_example_keyboard_custom_draw(void)
{
    lv_obj_t * kb = lv_keyboard_create(lv_screen_active());
    lv_obj_add_event_cb(kb, event_cb, LV_EVENT_DRAW_DSC_BEGIN, NULL);
    lv_obj_set_send_draw_dsc_events(kb, true);
    lv_obj_center(kb);
}

#endif
