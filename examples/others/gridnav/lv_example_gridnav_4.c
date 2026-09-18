#include "../../lv_examples.h"
#if LV_USE_GRIDNAV && LV_USE_FLEX && LV_USE_BUTTON && LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * Create a list: a flex container that stacks its children in a column.
 */
static lv_obj_t * list_create(lv_obj_t * parent)
{
    lv_obj_t * list = lv_obj_create(parent);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    return list;
}

/**
 * Add a full-width list button holding an optional icon and a text label.
 */
static lv_obj_t * list_add_button(lv_obj_t * list, const void * icon, const char * txt)
{
    lv_obj_t * btn = lv_button_create(list);
    lv_obj_set_size(btn, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);

#if LV_USE_IMAGE == 1
    if(icon) {
        lv_obj_t * img = lv_image_create(btn);
        lv_image_set_src(img, icon);
    }
#endif

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, txt);
    lv_obj_set_flex_grow(label, 1);

    return btn;
}

/**
 * Add a non-focusable, full-width text row that separates sections.
 */
static lv_obj_t * list_add_text(lv_obj_t * list, const char * txt)
{
    lv_obj_t * label = lv_label_create(list);
    lv_obj_set_width(label, lv_pct(100));
    lv_label_set_text(label, txt);
    return label;
}

static void event_handler(lv_event_t * e)
{
#if LV_USE_LOG
    lv_obj_t * obj = lv_event_get_target_obj(e);
    /*The button's text lives in its label child*/
    LV_LOG_USER("Clicked: %s", lv_label_get_text(lv_obj_get_child(obj, -1)));
#else
    LV_UNUSED(e);
#endif
}

/**
 * @title List with section separators
 * @brief Twenty list buttons grouped by full-width text separators under gridnav rollover.
 *
 * A list, a `LV_FLEX_FLOW_COLUMN` container built by the static `list_*` helpers,
 * sits on the left and is registered with `LV_GRIDNAV_CTRL_ROLLOVER`. Every fifth
 * iteration adds a non-focusable `lv_label` separator, and each focusable entry is
 * an `lv_button` with an `LV_SYMBOL_FILE` icon. Items are removed from the default
 * group so gridnav handles movement, and each item's `LV_EVENT_CLICKED` callback
 * logs the text of its label child. A separate `Button` sits at the right edge.
 */
void lv_example_gridnav_4(void)
{
    /*The example requires that a default group is set and
     *there is a keyboard indev*/
    lv_group_t * group = lv_group_get_default();
    if(!group) {
        LV_LOG_WARN("Gridnav example requires a default group");
        return;
    }

    lv_obj_t * list = list_create(lv_screen_active());
    lv_gridnav_add(list, LV_GRIDNAV_CTRL_ROLLOVER);
    lv_obj_align(list, LV_ALIGN_LEFT_MID, 10, 0);
    lv_group_add_obj(group, list);

    uint32_t i;
    for(i = 0; i < 20; i++) {
        char buf[32];

        /*Add some separators too, they are not focusable by gridnav*/
        if((i % 5) == 0) {
            lv_snprintf(buf, sizeof(buf), "Section %d", i / 5 + 1);
            list_add_text(list, buf);
        }

        lv_snprintf(buf, sizeof(buf), "File %d", i + 1);
        lv_obj_t * item = list_add_button(list, LV_SYMBOL_FILE, buf);
        lv_obj_add_event_cb(item, event_handler, LV_EVENT_CLICKED, NULL);
        lv_group_remove_obj(item);  /*The default group adds it automatically*/
    }

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_align(btn, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Button");
}

#endif
