#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_XML

void lv_example_xml_2(void)
{
    lv_xml_component_register_from_file("A:lvgl/examples/others/xml/view.xml");
    lv_xml_component_register_from_file("A:lvgl/examples/others/xml/my_button.xml");

    lv_obj_t * obj = (lv_obj_t *) lv_xml_create(NULL, "view", NULL);
    lv_screen_load(obj);
}
#endif
