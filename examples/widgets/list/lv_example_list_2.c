#include "../../lv_examples.h"
#if LV_USE_LIST && LV_BUILD_EXAMPLES

static lv_obj_t* list2;
static lv_obj_t* buttonColumn;

static lv_obj_t* currentButton = NULL;

static void event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        LV_LOG_USER("Clicked: %s", lv_list_get_btn_text(list2, obj));

        if (currentButton == obj)
        {
            currentButton = NULL;
        }
        else
        {
            currentButton = obj;
        }
        lv_obj_t* parent = lv_obj_get_parent(obj);
        uint32_t i;
        for (i = 0; i < lv_obj_get_child_cnt(parent); i++)
        {
            lv_obj_t* child = lv_obj_get_child(parent, i);
            if (child == currentButton)
            {
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else
            {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
    }
}

static void event_handler_mu(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t* obj = lv_event_get_target(e);
    if ((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT))
    {
        if (currentButton == NULL) return;
        lv_obj_t* parent = lv_obj_get_parent(currentButton);
        uint_fast32_t i = lv_obj_get_child_id(currentButton);
        if (i > 0)
        {
            lv_obj_swap(parent->spec_attr->children[i], parent->spec_attr->children[i - 1]);
        }
    }
}

static void event_handler_dn(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t* obj = lv_event_get_target(e);
    if ((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT))
    {
        if (currentButton == NULL) return;
        lv_obj_t* parent = lv_obj_get_parent(currentButton);
        uint_fast32_t i = lv_obj_get_child_id(currentButton);
        if (i < lv_obj_get_child_cnt(parent) - 1)
        {
            lv_obj_swap(parent->spec_attr->children[i], parent->spec_attr->children[i + 1]);
        }
    }
}


void lv_example_list_2(void)
{
    /*Create a list*/
    list2 = lv_list_create(lv_scr_act());
    lv_obj_set_size(list2, lv_obj_get_width(lv_scr_act()) - 90, lv_obj_get_height(lv_scr_act()) - 10);
    lv_obj_set_align(list2, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(list2, 5, 5);
    lv_obj_set_flex_flow(list2, LV_FLEX_FLOW_COLUMN);

    /*Add buttons to the list*/
    lv_obj_t* btn;

    for (int i = 0; i < 10; i++)
    {
        char szBuf[100];
        sprintf(szBuf, " Item %d ", i);
        btn = lv_btn_create(list2);
        lv_obj_t* lab = lv_label_create(btn);
        lv_label_set_text(lab, szBuf);
        lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    }

    buttonColumn = lv_obj_create(lv_scr_act());
    lv_obj_set_size(buttonColumn, 80, lv_obj_get_height(list2));
    lv_obj_set_align(buttonColumn, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos(buttonColumn, -5, 5);
    lv_obj_set_flex_flow(buttonColumn, LV_FLEX_FLOW_COLUMN);

    btn = lv_list_add_btn(buttonColumn, LV_SYMBOL_UP, NULL);
    lv_obj_add_event_cb(btn, event_handler_mu, LV_EVENT_ALL, NULL);
    btn = lv_list_add_btn(buttonColumn, LV_SYMBOL_DOWN, NULL);
    lv_obj_add_event_cb(btn, event_handler_dn, LV_EVENT_ALL, NULL);
}

#endif