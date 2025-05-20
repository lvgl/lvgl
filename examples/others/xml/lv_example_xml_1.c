#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_XML

void lv_example_xml_1(void)
{
    /*A red button created from builti-in LVGL widgets
     *It has an API parameter too to change its text.*/
    const char * red_button_xml =
        "<component>"
        "  <api>"
        "    <prop name=\"button_text\" type=\"string\" default=\"None\"/>"
        "  </api>"
        "  <view extends=\"lv_button\" radius=\"0\" style_bg_color=\"0xa91500\">"
        "    <lv_label text=\"$button_text\" align=\"center\"/>"
        "  </view>"
        "</component>";

    /*The card is just an lv_obj where a label and two red buttons are used.
     * Its API allow setting a title (label test) and the action (the text of a button)*/
    const char * card_xml =
        "<component>"
        "  <api>"
        "    <prop name=\"title\" type=\"string\" default=\"Hello world\"/>"
        "    <prop name=\"action\" type=\"string\"/>"
        "  </api>"
        "  <view width=\"200\" height=\"content\">"
        "    <lv_label text=\"$title\" align=\"top_mid\"/>"
        "    <red_button y=\"20\" align=\"top_left\" button_text=\"Cancel\"/>"
        "    <red_button y=\"20\" align=\"top_right\" button_text=\"$action\"/>"
        "  </view>"
        "</component>";

    /* Motor card is a special case of a card where the title and action are already set*/
    const char * motor_card_xml =
        "<component>"
        "  <view extends=\"card\" title=\"Motor start?\" action=\"Start\">"
        "  </view>"
        "</component>";

    /*Register all the custom components*/
    lv_xml_component_register_from_data("red_button", red_button_xml);
    lv_xml_component_register_from_data("card", card_xml);
    lv_xml_component_register_from_data("motor_card", motor_card_xml);

    lv_obj_t * card;
    /*Create a card with the default values*/
    card = (lv_obj_t *) lv_xml_create(lv_screen_active(), "card", NULL);

    /*Create a motor card too. The returned value can be adjusted freely*/
    card = (lv_obj_t *) lv_xml_create(lv_screen_active(), "motor_card", NULL);
    lv_obj_set_y(card, 90);

    /*Pass properties to a card*/
    const char * attrs[] = {
        "y", "180",
        "action", "Apply",
        "title", "New title",
        NULL, NULL,
    };
    card = (lv_obj_t *) lv_xml_create(lv_screen_active(), "card", attrs);

}
#endif
