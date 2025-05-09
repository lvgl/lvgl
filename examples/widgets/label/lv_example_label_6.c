#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_BUILD_EXAMPLES && LV_FONT_MONTSERRAT_20

static bool fix_w_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc, uint32_t letter,
                                uint32_t letter_next)
{
    bool ret = lv_font_get_glyph_dsc_fmt_txt(font, dsc, letter, letter_next);
    if(!ret) return false;

    /* Set a fixed width */
    dsc->adv_w = 20;
    dsc->ofs_x = (dsc->adv_w - dsc->box_w) / 2;
    return true;
}

void lv_example_label_6(void)
{
    /* Clone the original font and override its behavior */
    static lv_font_t mono_font;
    mono_font = lv_font_montserrat_20;
    mono_font.get_glyph_dsc = fix_w_get_glyph_dsc;

    /* Create a label with normal font */
    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_font(label1, &lv_font_montserrat_20, 0);
    lv_label_set_text(label1, "0123.Wabc");

    /* Create a label with fixed-width glyph descriptor override */
    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_obj_set_y(label2, 30);
    lv_obj_set_style_text_font(label2, &mono_font, 0);
    lv_label_set_text(label2, "0123.Wabc");
}

#endif
