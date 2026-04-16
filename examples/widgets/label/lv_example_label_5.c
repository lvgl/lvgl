#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * @title Customized circular scroll timing
 * @brief Set delays and infinite repeats on a label's circular scroll animation.
 *
 * An `lv_anim_t` template is configured with a 1000 ms initial delay, a 3000 ms
 * repeat delay, and `LV_ANIM_REPEAT_INFINITE`. A style stores the template via
 * `lv_style_set_anim` and is attached to a 150 px wide label running in
 * `LV_LABEL_LONG_MODE_SCROLL_CIRCULAR`, which applies the timing whenever the
 * scroll animation fires.
 */
void lv_example_label_5(void)
{
    static lv_anim_t animation_template;
    static lv_style_t label_style;

    lv_anim_init(&animation_template);
    lv_anim_set_delay(&animation_template, 1000);           /*Wait 1 second to start the first scroll*/
    lv_anim_set_repeat_delay(&animation_template,
                             3000);    /*Repeat the scroll 3 seconds after the label scrolls back to the initial position*/
    lv_anim_set_repeat_count(&animation_template, LV_ANIM_REPEAT_INFINITE);

    /*Initialize the label style with the animation template*/
    lv_style_init(&label_style);
    lv_style_set_anim(&label_style, &animation_template);

    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);      /*Circular scroll*/
    lv_obj_set_width(label1, 150);
    lv_label_set_text(label1, "It is a circularly scrolling text. ");
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_style(label1, &label_style, LV_STATE_DEFAULT);           /*Add the style to the label*/
}

#endif
