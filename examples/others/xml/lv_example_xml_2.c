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
    lv_xml_component_register_from_file("A:lvgl/examples/others/xml/view.xml");
    lv_xml_test_register_from_file("A:lvgl/examples/others/xml/test1.xml", "A:");

    lv_xml_register_font(NULL, "lv_montserrat_18", &lv_font_montserrat_18);

    lv_xml_test_run(1);

    uint32_t step_cnt = lv_xml_test_get_step_count();
    uint32_t i;
    for(i = 0; i < step_cnt; i++) {
        if(lv_xml_test_get_step_type(i) == LV_XML_TEST_STEP_TYPE_SCREENSHOT_COMPARE) {
            if(lv_xml_test_get_status(i)) printf("Step %d passed\n", i);
            else printf("Step %d failed\n", i);
        }
        else {
            printf("Step %d is not a screenshot compare\n", i);
        }
    }

}
#endif
