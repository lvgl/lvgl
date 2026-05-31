#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_USE_TRANSLATION && LV_BUILD_EXAMPLES

/**
 * @title Label with translation tag
 * @brief Bind a label to a translation tag so its text follows the active language.
 *
 * `lv_label_set_translation_tag` ties the label to a key in the
 * translation table. After `lv_translation_set_language` is called with
 * a new language code, every label bound to a tag re-renders with the
 * translated string. Translations themselves are registered in a CSV
 * (or XML) through `lv_translation_add_static`/`lv_translation_load`.
 */
void lv_example_label_translation(void)
{
    /* Register a small inline translation table so the example is
     * self-contained. In a real project this typically lives in a
     * generated file or an asset. */
    static const char * const langs[]   = {"en", "de", NULL};
    static const char * const tags[]    = {"greeting", "farewell", "thanks", NULL};

    /*                                    en             de */
    static const char * const trans[] = {"Hey!",        "Hallo",            /*greetings*/
                                         "Goodbye",     "Auf Wiedersehen",  /*farewell*/
                                         "Thank you",   "Danke"
                                        };           /*thanks*/

    lv_translation_add_static(langs, tags, trans);

    lv_translation_set_language("en");

    /* Lay out one label per tag so the language switch updates all three at once. */
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 8, 0);

    lv_obj_t * l_greet = lv_label_create(scr);
    lv_label_set_translation_tag(l_greet, "greeting");

    lv_obj_t * l_bye = lv_label_create(scr);
    lv_label_set_translation_tag(l_bye, "farewell");

    lv_obj_t * l_thanks = lv_label_create(scr);
    lv_label_set_translation_tag(l_thanks, "thanks");


    /* Switching the language causes every bound label to refresh automatically. */
    lv_translation_set_language("de");
}

#endif
