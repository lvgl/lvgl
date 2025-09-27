#include "../../lv_examples.h"

#if LV_USE_TRANSLATION && LV_USE_DROPDOWN && LV_USE_LABEL && LV_BUILD_EXAMPLES

static const char * tags[] = {"tiger", "lion", "rabbit", "elephant", NULL};
static const char * languages[] = {"English", "Deutsch", "Español", NULL};

static void add_static_translations(void)
{
    static const char * translations[] = {
        "The Tiger",    "Der Tiger",     "El Tigre",
        "The Lion",     "Der Löwe",      "El León",
        "The Rabbit",   "Das Kaninchen", "El Conejo",
        "The Elephant", "Der Elefant",   "El Elefante",
    };

    lv_translation_add_static(languages, tags, translations);
}

static void language_change_cb(lv_event_t * e)
{
    static char selected_lang[20];

    lv_obj_t * dropdown = lv_event_get_target_obj(e);
    lv_dropdown_get_selected_str(dropdown, selected_lang, sizeof(selected_lang));
    lv_translation_set_language(selected_lang);
}

/**
 * Use a translation tag in labels
 */
void lv_example_label_7(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    add_static_translations();
    const size_t tag_count = sizeof(tags) / sizeof(tags[0]) - 1;
    const size_t lang_count = sizeof(languages) / sizeof(languages[0]) - 1;

    /* Create a dropdown to be able to select the language */
    lv_obj_t * language_dropdown = lv_dropdown_create(lv_screen_active());
    lv_dropdown_clear_options(language_dropdown);

    for(size_t i = 0; i < lang_count; ++i) {
        lv_dropdown_add_option(language_dropdown, languages[i], i);
    }

    lv_obj_add_event_cb(language_dropdown, language_change_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_translation_set_language("English");

    /* Create a label for each tag */
    for(size_t i = 0; i < tag_count; ++i) {
        lv_obj_t * label = lv_label_create(lv_screen_active());
        lv_label_set_translation_tag(label, tags[i]);
    }
}

#endif /*LV_USE_TRANSLATION && LV_USE_DROPDOWN && LV_USE_LABEL && LV_BUILD_EXAMPLES*/
