/**
 * @file lv_demo_flex_layout_flex_loader.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_flex_layout_main.h"

#if LV_USE_DEMO_FLEX_LAYOUT

/*********************
 *      DEFINES
 *********************/

#define FLEX_ALIGN_EVENT_DEF(item)                                              \
    static void flex_align_##item##_event_handler(lv_event_t* e)                \
    {                                                                           \
        view_t* ui = lv_event_get_user_data(e);                                 \
        lv_obj_t* ddlist = lv_event_get_target(e);                              \
        uint16_t selected = lv_dropdown_get_selected(ddlist);                   \
        if (ui->obj_cur) {                                                      \
            lv_obj_set_style_flex_##item##_place(ui->obj_cur,                   \
                                                 flex_align_map[selected], 0);  \
        }                                                                       \
    }

#define FLEX_ALIGN_DDLIST_UPDATE_DEF(item)                                            \
    do {                                                                              \
        lv_flex_align_t flex_align = lv_obj_get_style_flex_##item##_place(obj, 0);    \
        for (i = 0; i < ARRAY_SIZE(flex_align_map); i++) {                            \
            if (flex_align == flex_align_map[i]) {                                    \
                lv_dropdown_set_selected(ui->ctrl_pad.tab.align.ddlist_align_##item,  \
                                         i);                                          \
                break;                                                                \
            }                                                                         \
        }                                                                             \
    } while(0)

#define FLEX_ALIGN_EVENT_ATTACH(item)                                               \
    do {                                                                            \
        lv_obj_add_event_cb(ui->ctrl_pad.tab.align.ddlist_align_##item,             \
                            flex_align_##item##_event_handler,                      \
                            LV_EVENT_VALUE_CHANGED,                                 \
                            ui                                                      \
                           );                                                       \
    } while(0)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void flex_flow_event_handler(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_flex_flow_t flex_flow_map[] = {
    LV_FLEX_FLOW_ROW,
    LV_FLEX_FLOW_COLUMN,
    LV_FLEX_FLOW_ROW_WRAP,
    LV_FLEX_FLOW_ROW_REVERSE,
    LV_FLEX_FLOW_ROW_WRAP_REVERSE,
    LV_FLEX_FLOW_COLUMN_WRAP,
    LV_FLEX_FLOW_COLUMN_REVERSE,
    LV_FLEX_FLOW_COLUMN_WRAP_REVERSE
};

static const lv_flex_align_t flex_align_map[] = {
    LV_FLEX_ALIGN_START,
    LV_FLEX_ALIGN_END,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_SPACE_EVENLY,
    LV_FLEX_ALIGN_SPACE_AROUND,
    LV_FLEX_ALIGN_SPACE_BETWEEN,
};

/**********************
 *      MACROS
 **********************/

FLEX_ALIGN_EVENT_DEF(main)
FLEX_ALIGN_EVENT_DEF(cross)
FLEX_ALIGN_EVENT_DEF(track)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void flex_loader_attach(view_t * ui)
{
    lv_obj_add_event_cb(
        ui->ctrl_pad.tab.flex.ddlist_flow,
        flex_flow_event_handler,
        LV_EVENT_VALUE_CHANGED,
        ui
    );

    FLEX_ALIGN_EVENT_ATTACH(main);
    FLEX_ALIGN_EVENT_ATTACH(cross);
    FLEX_ALIGN_EVENT_ATTACH(track);
}

void flex_loader_obj_update(lv_obj_t * obj, view_t * ui)
{
    lv_flex_flow_t flex_flow = lv_obj_get_style_flex_flow(obj, 0);
    uint32_t i;
    for(i = 0; i < ARRAY_SIZE(flex_flow_map); i++) {
        if(flex_flow == flex_flow_map[i]) {
            lv_dropdown_set_selected(ui->ctrl_pad.tab.flex.ddlist_flow, i);
            break;
        }
    }

    FLEX_ALIGN_DDLIST_UPDATE_DEF(main);
    FLEX_ALIGN_DDLIST_UPDATE_DEF(cross);
    FLEX_ALIGN_DDLIST_UPDATE_DEF(track);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flex_flow_event_handler(lv_event_t * e)
{
    view_t * ui = lv_event_get_user_data(e);
    lv_obj_t * ddlist = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(ddlist);

    if(ui->obj_cur) {
        lv_obj_set_flex_flow(ui->obj_cur, flex_flow_map[selected]);
        lv_obj_scroll_to(ui->obj_cur, 0, 0, LV_ANIM_ON);
    }
}

#endif
