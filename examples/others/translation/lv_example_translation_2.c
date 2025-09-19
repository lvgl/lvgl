#include "../../lv_examples.h"

#if LV_USE_TRANSLATION && LV_BUILD_EXAMPLES

static const char * tags[] = {"tiger", "lion", "rabbit", "elephant", NULL};

static void add_static(void)
{
    static const char * languages[]    = {"en", "de", "es", NULL};
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

static void language_change_timer(lv_timer_t * timer)
{
    LV_UNUSED(timer);
    lv_obj_t * language_label = (lv_obj_t *)lv_timer_get_user_data(timer);

    if(lv_streq(lv_translation_get_language(), "en")) {
        lv_translation_set_language("de");
        lv_label_set_text_static(language_label, "Deutsch");
    }
    else if(lv_streq(lv_translation_get_language(), "de")) {
        lv_translation_set_language("es");
        lv_label_set_text_static(language_label, "Español");
    }
    else {
        lv_translation_set_language("en");
        lv_label_set_text_static(language_label, "English");
    }
}

/**
 * Change label text when the translation language changes
 */
void lv_example_translation_2(void)
{
    add_static();
    const size_t tag_count = sizeof(tags) / sizeof(tags[0]) - 1;

    lv_obj_t * container   = lv_obj_create(lv_screen_active());
    lv_obj_center(container);
    lv_obj_set_size(container, LV_PCT(50), LV_PCT(50));

    lv_obj_t * language_label = lv_label_create(lv_screen_active());
    lv_obj_align_to(language_label, container, LV_ALIGN_OUT_TOP_MID, 0, -20);

    /* We create a label for each tag */
    for(size_t i = 0; i < tag_count; ++i) {
        lv_obj_t * label = lv_label_create(container);

        /* Bind to the language change event so that we can change the label when the language changes */
        lv_obj_add_event_cb(label, on_language_change, LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, (void *)tags[i]);
        lv_obj_set_pos(label, (i / 2) * 200, (i % 2) * 50);
    }

    lv_translation_set_language("en");
    lv_label_set_text_static(language_label, "English");

    lv_timer_create(language_change_timer, 1000, language_label);
}

#endif /*LV_USE_TRANSLATION && LV_BUILD_EXAMPLES*/
