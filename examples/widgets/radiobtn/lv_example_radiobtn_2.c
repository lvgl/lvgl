#include "../../lv_examples.h"
#if LV_USE_RADIOBTN && LV_BUILD_EXAMPLES

static lv_obj_t * label_gender_result;
static lv_obj_t * label_age_result;
static lv_obj_t * label_city_result;
static char buf[32];


static void event_handler_gender(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_radiobtn_t * rb = (lv_radiobtn_t *)obj;
    const char * gender = rb->checked_txt;
    lv_snprintf(buf, sizeof(buf), "Your gender: \n%s", gender);
    lv_label_set_text(label_gender_result, buf);
    LV_LOG_USER("Selected gender: %s", gender);
}

static void event_handler_age(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_radiobtn_t * rb = (lv_radiobtn_t *)obj;
    const char * age = rb->checked_txt;
    lv_snprintf(buf, sizeof(buf), "Your age: \n%s", age);
    lv_label_set_text(label_age_result, buf);
    LV_LOG_USER("Selected age group: %s", age);
}

static void event_handler_city(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_radiobtn_t * rb = (lv_radiobtn_t *)obj;
    const char * city = rb->checked_txt;
    lv_snprintf(buf, sizeof(buf), "You live in: \n%s", city);
    lv_label_set_text(label_city_result, buf);
    LV_LOG_USER("Selected city: %s", city);
}

void lv_example_radiobtn_2(void)
{
    /*Create label_title*/
    lv_obj_t * label_tilte = lv_label_create(lv_scr_act());
    lv_label_set_text(label_tilte, "QUESTIONS");
    lv_obj_set_x(label_tilte, lv_pct(3));

    /*Create a question container*/
    lv_obj_t * cont_question;
    cont_question = lv_obj_create(lv_scr_act());
    lv_obj_set_flex_flow(cont_question, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(cont_question, lv_pct(45), lv_pct(90));
    lv_obj_set_pos(cont_question, lv_pct(3), 20);

    /*Create label_gender*/
    lv_obj_t * label_gender = lv_label_create(cont_question);
    lv_label_set_text(label_gender, "Gender:");
    lv_obj_set_width(label_gender, lv_pct(100));

    /*Create radiobtn_gender*/
    lv_obj_t * radiobtn_gender = lv_radiobtn_create(cont_question);
    lv_obj_set_size(radiobtn_gender, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_add_event_cb(radiobtn_gender, event_handler_gender, LV_EVENT_CLICKED, NULL);

    /*Add buttons to the radiobtn_gender*/
    lv_radiobtn_add_item(radiobtn_gender, "Male");
    lv_radiobtn_add_item(radiobtn_gender, "Female");
    lv_radiobtn_add_item(radiobtn_gender, "Others");

    /*Create label_age*/
    lv_obj_t * label_age = lv_label_create(cont_question);
    lv_label_set_text(label_age, "Age group:");
    lv_obj_set_width(label_age, lv_pct(100));

    /*Create radiobtn_gender*/
    lv_obj_t * radiobtn_age = lv_radiobtn_create(cont_question);
    lv_obj_set_size(radiobtn_age, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_add_event_cb(radiobtn_age, event_handler_age, LV_EVENT_CLICKED, NULL);

    /*Add buttons to the radiobtn*/
    lv_radiobtn_add_item(radiobtn_age, "0 - 18");
    lv_radiobtn_add_item(radiobtn_age, "19 - 30");
    lv_radiobtn_add_item(radiobtn_age, "31 - 50");
    lv_radiobtn_add_item(radiobtn_age, "50 - ");

    /*Create label_city*/
    lv_obj_t * label_city = lv_label_create(cont_question);
    lv_label_set_text(label_city, "Live in:");
    lv_obj_set_width(label_city, lv_pct(100));

    /*Create radiobtn_city*/
    lv_obj_t * radiobtn_city = lv_radiobtn_create(cont_question);
    lv_obj_set_size(radiobtn_city, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_add_event_cb(radiobtn_city, event_handler_city, LV_EVENT_CLICKED, NULL);

    /*Add buttons to the radiobtn*/
    lv_radiobtn_add_item(radiobtn_city, "Beijing");
    lv_radiobtn_add_item(radiobtn_city, "Tokyo");
    lv_radiobtn_add_item(radiobtn_city, "Paris");
    lv_radiobtn_add_item(radiobtn_city, "London");
    lv_radiobtn_add_item(radiobtn_city, "New York");
    lv_radiobtn_add_item(radiobtn_city, "Others");

    /*Create label_tilte_result*/
    lv_obj_t * label_tilte_result = lv_label_create(lv_scr_act());
    lv_label_set_text(label_tilte_result, "ANSWERS");
    lv_obj_set_x(label_tilte_result, lv_pct(52));

    /*Create a answer container*/
    lv_obj_t * cont_answer;
    cont_answer = lv_obj_create(lv_scr_act());
    lv_obj_set_flex_flow(cont_answer, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(cont_answer, lv_pct(45), lv_pct(60));
    lv_obj_set_pos(cont_answer, lv_pct(52), 20);

    /*Create label_gender_result*/
    label_gender_result = lv_label_create(cont_answer);
    lv_label_set_text(label_gender_result, "Please select your gender");
    lv_obj_set_width(label_gender_result, lv_pct(100));

    /*Create label_age_result*/
    label_age_result = lv_label_create(cont_answer);
    lv_label_set_text(label_age_result, "Please select your age group");
    lv_obj_set_width(label_age_result, lv_pct(100));

    /*Create label_city_result*/
    label_city_result = lv_label_create(cont_answer);
    lv_label_set_text(label_city_result, "Please select your city");
    lv_obj_set_width(label_city_result, lv_pct(100));
}

#endif
