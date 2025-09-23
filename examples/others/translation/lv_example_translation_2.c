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

static void on_language_change(lv_event_t * e)
{
    lv_obj_t * label      = lv_event_get_target_obj(e);
    const char * tag      = (const char *) lv_event_get_user_data(e);
    /* You can get the new language with `lv_event_get_param`*/
    const char * language = (const char *) lv_event_get_param(e);
    LV_UNUSED(language);

    lv_label_set_text(label, lv_tr(tag));
}

static void language_change_cb(lv_event_t * e)
{
    static char selected_lang[20];

    lv_obj_t * dropdown = lv_event_get_target_obj(e);
    lv_dropdown_get_selected_str(dropdown, selected_lang, sizeof(selected_lang));
    lv_translation_set_language(selected_lang);
}

/**
 * Change label text when the translation language changes
 */
void lv_example_translation_2(void)
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

    /* Create a label for each tag */
    for(size_t i = 0; i < tag_count; ++i) {
        lv_obj_t * label = lv_label_create(lv_screen_active());

        /* Bind to the language change event so that we can change the label when the language changes */
        lv_obj_add_event_cb(label, on_language_change, LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, (void *)tags[i]);
    }

    lv_translation_set_language("English");
}

#endif /*LV_USE_TRANSLATION && LV_USE_DROPDOWN && LV_USE_LABEL && LV_BUILD_EXAMPLES*/
