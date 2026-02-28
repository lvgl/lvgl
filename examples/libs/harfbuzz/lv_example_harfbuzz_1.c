#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_FREETYPE && LV_USE_HARFBUZZ

#if LV_FREETYPE_USE_LVGL_PORT
    #define PATH_PREFIX "A:"
#else
    #define PATH_PREFIX "./"
#endif

/**
 * Render Hindi (Devanagari) text with HarfBuzz shaping.
 *
 * Devanagari requires text shaping because consonants combine into conjuncts,
 * vowel signs (matras) reorder around base characters, and half-forms are
 * used for consonant clusters.  Without shaping, the characters appear
 * disconnected and incorrectly ordered.
 *
 * Requires: NotoSansDevanagari-Regular.ttf in the working directory
 * (download from https://fonts.google.com/noto/specimen/Noto+Sans+Devanagari)
 */
void lv_example_harfbuzz_1(void)
{
    /*Create a Devanagari font*/
    lv_font_t * font_hindi = lv_freetype_font_create(
                                 PATH_PREFIX "NotoSansDevanagari-Regular.ttf",
                                 LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 32,
                                 LV_FREETYPE_FONT_STYLE_NORMAL);

    if(!font_hindi) {
        LV_LOG_ERROR("failed to create Devanagari font");
        return;
    }

    /*Set a Latin fallback so digits and punctuation also render*/
    font_hindi->fallback = &lv_font_montserrat_14;

    /*Container with column layout*/
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, lv_pct(90), LV_SIZE_CONTENT);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(cont, 8, 0);

    /*Simple greeting*/
    lv_obj_t * lbl1 = lv_label_create(cont);
    lv_obj_set_style_text_font(lbl1, font_hindi, 0);
    lv_label_set_text(lbl1, "नमस्ते दुनिया");               /* Hello World */

    /*Conjuncts: these consonant clusters fuse into single glyphs*/
    lv_obj_t * lbl2 = lv_label_create(cont);
    lv_obj_set_style_text_font(lbl2, font_hindi, 0);
    lv_label_set_text(lbl2, "क्ष त्र ज्ञ श्र");             /* ksha, tra, gya, shra */

    /*Matra (vowel sign) combinations*/
    lv_obj_t * lbl3 = lv_label_create(cont);
    lv_obj_set_style_text_font(lbl3, font_hindi, 0);
    lv_label_set_text(lbl3, "कि की कु कू के कै को कौ");     /* ka + various vowel signs */

    /*Complex words with half-forms and Reph*/
    lv_obj_t * lbl4 = lv_label_create(cont);
    lv_obj_set_style_text_font(lbl4, font_hindi, 0);
    lv_label_set_text(lbl4, "स्त्री विद्या राष्ट्र");       /* stri, vidya, rashtra */
}

#else

void lv_example_harfbuzz_1(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "FreeType and HarfBuzz are not enabled");
    lv_obj_center(label);
}

#endif
#endif
