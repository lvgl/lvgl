#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_XML && LV_USE_TRANSLATION

void lv_example_xml_2(void)
{
    lv_xml_component_register_from_file("A:my_button.xml");
    lv_xml_component_register_from_file("A:list_item.xml");

    lv_obj_t * obj = (lv_obj_t *) lv_xml_create(lv_screen_active(), "list_item", NULL);
    //    lv_obj_t * obj = (lv_obj_t *) lv_xml_create(lv_screen_active(), "view", NULL);
    //    lv_obj_set_pos(obj, 10, 10);

}
#endif
