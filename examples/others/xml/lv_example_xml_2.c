#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_XML

void lv_example_xml_2(void)
{
    lv_result_t res;
    res = lv_xml_component_register_from_file("A:lvgl/examples/others/xml/my_h3.xml");
    if(res != LV_RESULT_OK) {
        lv_obj_t * label = lv_label_create(lv_screen_active());
        lv_label_set_text(label, "Couldn't open the XML files.");
        lv_obj_center(label);
        return;
    }
    lv_xml_component_register_from_file("A:lvgl/examples/others/xml/my_card.xml");
    lv_xml_component_register_from_file("A:lvgl/examples/others/xml/my_button.xml");

    lv_xml_register_font(NULL, "lv_montserrat_18", &lv_font_montserrat_18);

    lv_subject_t s1;
    lv_subject_t s2;
    static char buf[200];
    lv_subject_init_string(&s1, buf, NULL, 200, "Waaaa");
    lv_subject_init_int(&s2, 25);

    lv_xml_register_subject(NULL, "s1", &s2);

    lv_xml_test_register_from_file("A:lvgl/examples/others/xml/view.xml", "A:");

    lv_xml_test_run_all(1);

}
#endif
