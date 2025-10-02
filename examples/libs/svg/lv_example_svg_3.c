#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_SVG && LV_USE_VECTOR_GRAPHIC

/**
 * Draw SVG data in a draw event
 */
static void event_cb(lv_event_t * e)
{
    static const char svg_data[] = "<svg width=\"12cm\" height=\"4cm\" viewBox=\"0 0 1200 400\">"
                                   "<circle cx=\"600\" cy=\"200\" r=\"100\" fill=\"red\" stroke=\"blue\" stroke-width=\"10\"/></svg>";

    lv_layer_t * layer = lv_event_get_layer(e);
    lv_svg_node_t * svg = lv_svg_load_data(svg_data, sizeof(svg_data) / sizeof(char));
    lv_draw_svg(layer, svg);
    lv_svg_node_delete(svg);
}

void lv_example_svg_3(void)
{
    lv_obj_add_event_cb(lv_screen_active(), event_cb, LV_EVENT_DRAW_MAIN, NULL);
}
#else

void lv_example_svg_3(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "SVG is not enabled");
    lv_obj_center(label);
}

#endif
#endif
