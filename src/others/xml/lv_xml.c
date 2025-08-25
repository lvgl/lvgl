/**
 * @file lv_xml.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_xml.h"
#if LV_USE_XML

#include "lv_xml.h"
#include "lv_xml_base_types.h"
#include "lv_xml_parser.h"
#include "lv_xml_component.h"
#include "lv_xml_component_private.h"
#include "lv_xml_widget.h"
#include "lv_xml_style.h"
#include "lv_xml_translation.h"
#include "lv_xml_utils.h"
#include "lv_xml_private.h"
#include "parsers/lv_xml_obj_parser.h"
#include "parsers/lv_xml_button_parser.h"
#include "parsers/lv_xml_label_parser.h"
#include "parsers/lv_xml_image_parser.h"
#include "parsers/lv_xml_bar_parser.h"
#include "parsers/lv_xml_slider_parser.h"
#include "parsers/lv_xml_tabview_parser.h"
#include "parsers/lv_xml_chart_parser.h"
#include "parsers/lv_xml_table_parser.h"
#include "parsers/lv_xml_dropdown_parser.h"
#include "parsers/lv_xml_roller_parser.h"
#include "parsers/lv_xml_scale_parser.h"
#include "parsers/lv_xml_buttonmatrix_parser.h"
#include "parsers/lv_xml_spangroup_parser.h"
#include "parsers/lv_xml_textarea_parser.h"
#include "parsers/lv_xml_keyboard_parser.h"
#include "parsers/lv_xml_arc_parser.h"
#include "parsers/lv_xml_switch_parser.h"
#include "parsers/lv_xml_checkbox_parser.h"
#include "parsers/lv_xml_canvas_parser.h"
#include "parsers/lv_xml_calendar_parser.h"
#include "../../libs/expat/expat.h"
#include "../../draw/lv_draw_image.h"
#include "../../core/lv_global.h"
#include "../../misc/lv_anim_timeline_private.h"

/*********************
 *      DEFINES
 *********************/
#define xml_path_prefix LV_GLOBAL_DEFAULT()->xml_path_prefix
#define lv_event_xml_store_timeline LV_GLOBAL_DEFAULT()->lv_event_xml_store_timeline

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void view_start_element_handler(void * user_data, const char * name, const char ** attrs);
static void view_end_element_handler(void * user_data, const char * name);
static void get_timeline_from_event_cb(lv_event_t * e);
static void free_timelines_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_xml_init(void)
{
    xml_path_prefix = lv_strdup("");

    /*It will be sued to store animation time lines in user_data*/
    lv_event_xml_store_timeline = lv_event_register_id();

    lv_xml_component_init();

    lv_xml_register_font(NULL, "lv_font_default", lv_font_get_default());

    lv_xml_widget_register("lv_obj", lv_xml_obj_create, lv_xml_obj_apply);
    lv_xml_widget_register("lv_button", lv_xml_button_create, lv_xml_button_apply);
    lv_xml_widget_register("lv_label", lv_xml_label_create, lv_xml_label_apply);
    lv_xml_widget_register("lv_image", lv_xml_image_create, lv_xml_image_apply);
    lv_xml_widget_register("lv_bar", lv_xml_bar_create, lv_xml_bar_apply);
    lv_xml_widget_register("lv_slider", lv_xml_slider_create, lv_xml_slider_apply);
    lv_xml_widget_register("lv_tabview", lv_xml_tabview_create, lv_xml_tabview_apply);
    lv_xml_widget_register("lv_tabview-tab_bar", lv_xml_tabview_tab_bar_create, lv_xml_tabview_tab_bar_apply);
    lv_xml_widget_register("lv_tabview-tab", lv_xml_tabview_tab_create, lv_xml_tabview_tab_apply);
    lv_xml_widget_register("lv_tabview-tab_button", lv_xml_tabview_tab_button_create, lv_xml_tabview_tab_button_apply);
    lv_xml_widget_register("lv_chart", lv_xml_chart_create, lv_xml_chart_apply);
    lv_xml_widget_register("lv_chart-cursor", lv_xml_chart_cursor_create, lv_xml_chart_cursor_apply);
    lv_xml_widget_register("lv_chart-series", lv_xml_chart_series_create, lv_xml_chart_series_apply);
    lv_xml_widget_register("lv_chart-axis", lv_xml_chart_axis_create, lv_xml_chart_axis_apply);
    lv_xml_widget_register("lv_table", lv_xml_table_create, lv_xml_table_apply);
    lv_xml_widget_register("lv_table-column", lv_xml_table_column_create, lv_xml_table_column_apply);
    lv_xml_widget_register("lv_table-cell", lv_xml_table_cell_create, lv_xml_table_cell_apply);
    lv_xml_widget_register("lv_dropdown", lv_xml_dropdown_create, lv_xml_dropdown_apply);
    lv_xml_widget_register("lv_dropdown-list", lv_xml_dropdown_list_create, lv_xml_dropdown_list_apply);
    lv_xml_widget_register("lv_roller", lv_xml_roller_create, lv_xml_roller_apply);
    lv_xml_widget_register("lv_scale", lv_xml_scale_create, lv_xml_scale_apply);
    lv_xml_widget_register("lv_scale-section", lv_xml_scale_section_create, lv_xml_scale_section_apply);
    lv_xml_widget_register("lv_spangroup", lv_xml_spangroup_create, lv_xml_spangroup_apply);
    lv_xml_widget_register("lv_spangroup-span", lv_xml_spangroup_span_create, lv_xml_spangroup_span_apply);
    lv_xml_widget_register("lv_buttonmatrix", lv_xml_buttonmatrix_create, lv_xml_buttonmatrix_apply);
    lv_xml_widget_register("lv_textarea", lv_xml_textarea_create, lv_xml_textarea_apply);
    lv_xml_widget_register("lv_keyboard", lv_xml_keyboard_create, lv_xml_keyboard_apply);
    lv_xml_widget_register("lv_arc", lv_xml_arc_create, lv_xml_arc_apply);
    lv_xml_widget_register("lv_switch", lv_xml_switch_create, lv_xml_switch_apply);
    lv_xml_widget_register("lv_checkbox", lv_xml_checkbox_create, lv_xml_checkbox_apply);
    lv_xml_widget_register("lv_canvas", lv_xml_canvas_create, lv_xml_canvas_apply);
    lv_xml_widget_register("lv_calendar", lv_xml_calendar_create, lv_xml_calendar_apply);
    lv_xml_widget_register("lv_calendar-header_arrow", lv_xml_calendar_header_arrow_create,
                           lv_xml_calendar_header_arrow_apply);
    lv_xml_widget_register("lv_calendar-header_dropdown", lv_xml_calendar_header_dropdown_create,
                           lv_xml_calendar_header_dropdown_apply);

    lv_xml_widget_register("lv_obj-style", lv_obj_xml_style_create, lv_obj_xml_style_apply);
    lv_xml_widget_register("lv_obj-remove_style", lv_obj_xml_remove_style_create, lv_obj_xml_remove_style_apply);
    lv_xml_widget_register("lv_obj-remove_style_all", lv_obj_xml_remove_style_all_create,
                           lv_obj_xml_remove_style_all_apply);

    lv_xml_widget_register("lv_obj-event_cb", lv_obj_xml_event_cb_create, lv_obj_xml_event_cb_apply);
    lv_xml_widget_register("lv_obj-subject_set_int_event", lv_obj_xml_subject_set_create, lv_obj_xml_subject_set_apply);
    lv_xml_widget_register("lv_obj-subject_set_float_event", lv_obj_xml_subject_set_create, lv_obj_xml_subject_set_apply);
    lv_xml_widget_register("lv_obj-subject_set_string_event", lv_obj_xml_subject_set_create, lv_obj_xml_subject_set_apply);
    lv_xml_widget_register("lv_obj-subject_increment_event", lv_obj_xml_subject_increment_create,
                           lv_obj_xml_subject_increment_apply);
    lv_xml_widget_register("lv_obj-screen_load_event", lv_obj_xml_screen_load_event_create,
                           lv_obj_xml_screen_load_event_apply);
    lv_xml_widget_register("lv_obj-screen_create_event", lv_obj_xml_screen_create_event_create,
                           lv_obj_xml_screen_create_event_apply);
    lv_xml_widget_register("lv_obj-play_timeline_event", lv_obj_xml_play_timeline_event_create,
                           lv_obj_xml_play_timeline_event_apply);

    lv_xml_widget_register("lv_obj-bind_style", lv_obj_xml_bind_style_create, lv_obj_xml_bind_style_apply);
    lv_xml_widget_register("lv_obj-bind_flag_if_eq", lv_obj_xml_bind_flag_create, lv_obj_xml_bind_flag_apply);
    lv_xml_widget_register("lv_obj-bind_flag_if_not_eq", lv_obj_xml_bind_flag_create, lv_obj_xml_bind_flag_apply);
    lv_xml_widget_register("lv_obj-bind_flag_if_gt", lv_obj_xml_bind_flag_create, lv_obj_xml_bind_flag_apply);
    lv_xml_widget_register("lv_obj-bind_flag_if_lt", lv_obj_xml_bind_flag_create, lv_obj_xml_bind_flag_apply);
    lv_xml_widget_register("lv_obj-bind_flag_if_ge", lv_obj_xml_bind_flag_create, lv_obj_xml_bind_flag_apply);
    lv_xml_widget_register("lv_obj-bind_flag_if_le", lv_obj_xml_bind_flag_create, lv_obj_xml_bind_flag_apply);

    lv_xml_widget_register("lv_obj-bind_state_if_eq", lv_obj_xml_bind_state_create, lv_obj_xml_bind_state_apply);
    lv_xml_widget_register("lv_obj-bind_state_if_not_eq", lv_obj_xml_bind_state_create, lv_obj_xml_bind_state_apply);
    lv_xml_widget_register("lv_obj-bind_state_if_gt", lv_obj_xml_bind_state_create, lv_obj_xml_bind_state_apply);
    lv_xml_widget_register("lv_obj-bind_state_if_lt", lv_obj_xml_bind_state_create, lv_obj_xml_bind_state_apply);
    lv_xml_widget_register("lv_obj-bind_state_if_ge", lv_obj_xml_bind_state_create, lv_obj_xml_bind_state_apply);
    lv_xml_widget_register("lv_obj-bind_state_if_le", lv_obj_xml_bind_state_create, lv_obj_xml_bind_state_apply);
}

void lv_xml_deinit(void)
{
#if LV_USE_TEST
    lv_xml_test_unregister();
#endif

    lv_free((void *)xml_path_prefix);
}

void * lv_xml_create_in_scope(lv_obj_t * parent, lv_xml_component_scope_t * parent_scope,
                              lv_xml_component_scope_t * scope,
                              const char ** attrs)
{
    /* Initialize the parser state */
    lv_xml_parser_state_t state;
    lv_xml_parser_state_init(&state);
    state.scope = *scope; /*Scope won't be modified here, so it's safe to copy it by value*/
    state.parent = parent;
    state.parent_attrs = attrs;
    state.parent_scope = parent_scope;

    lv_obj_t ** parent_node = lv_ll_ins_head(&state.parent_ll);
    *parent_node = parent;

    /* Create an XML parser and set handlers */
    XML_Memory_Handling_Suite mem_handlers;
    mem_handlers.malloc_fcn = lv_malloc;
    mem_handlers.realloc_fcn = lv_realloc;
    mem_handlers.free_fcn = lv_free;
    XML_Parser parser = XML_ParserCreate_MM(NULL, &mem_handlers, NULL);
    XML_SetUserData(parser, &state);
    XML_SetElementHandler(parser, view_start_element_handler, view_end_element_handler);

    /* Parse the XML */
    if(XML_Parse(parser, scope->view_def, lv_strlen(scope->view_def), XML_TRUE) == XML_STATUS_ERROR) {
        LV_LOG_WARN("XML parsing error: %s on line %lu", XML_ErrorString(XML_GetErrorCode(parser)),
                    XML_GetCurrentLineNumber(parser));
        XML_ParserFree(parser);
        return NULL;
    }

    state.item = state.view;

#if LV_USE_OBJ_NAME
    /*Set a default indexed name*/
    if(state.item) {
        if(state.scope.is_screen) {
            lv_obj_set_name(state.item, scope->name);
        }
        else if(lv_obj_get_name(state.item) == NULL) {
            char name_buf[128];
            lv_snprintf(name_buf, sizeof(name_buf), "%s_#", scope->name);
        }
    }
#endif

    /*Create the timelines as well*/
    if(!lv_ll_is_empty(&scope->timeline_ll)) {
        lv_xml_timeline_t * at_xml;
        lv_anim_timeline_t ** timeline_array;
        timeline_array = lv_malloc((lv_ll_get_len(&scope->timeline_ll) + 1) * sizeof(lv_anim_timeline_t *));
        uint32_t i = 0;
        LV_LL_READ(&scope->timeline_ll, at_xml) {
            lv_anim_timeline_t * at = lv_anim_timeline_create();
            at->user_data = lv_strdup(at_xml->name);

            lv_anim_t * a_stored;
            LV_LL_READ(&at_xml->anims_ll, a_stored) {
                int32_t delay = -a_stored->act_time;
                lv_anim_timeline_add(at, delay, a_stored);
            }

            at->base_obj = state.view;
            timeline_array[i] = at;
            i++;
        }


        timeline_array[i] = NULL; /*Closing to avoid storing the length*/


        lv_obj_add_event_cb(state.view, get_timeline_from_event_cb, lv_event_xml_store_timeline, timeline_array);
        lv_obj_add_event_cb(state.view, free_timelines_event_cb, LV_EVENT_DELETE, timeline_array);
    }

    lv_ll_clear(&state.parent_ll);
    XML_ParserFree(parser);

    return state.view;
}

void * lv_xml_create(lv_obj_t * parent, const char * name, const char ** attrs)
{
    lv_obj_t * item = NULL;

    /* Select the widget specific parser type based on the name */
    lv_widget_processor_t * p = lv_xml_widget_get_processor(name);
    if(p) {
        lv_xml_parser_state_t state;
        lv_xml_parser_state_init(&state);
        state.parent = parent;

        /* When a component is just created there is no scope where
         * its styles, constants, etc are stored.
         * So leave state.scope = NULL which means the global context.*/

        state.item = p->create_cb(&state, attrs);
        if(state.item == NULL) {
            LV_LOG_WARN("Couldn't create widget.");
            return NULL;
        }
        if(attrs) {
            p->apply_cb(&state, attrs);
        }
        return state.item;
    }

    lv_xml_component_scope_t * scope = lv_xml_component_get_scope(name);
    if(scope) {
        item = lv_xml_create_in_scope(parent, NULL, scope, attrs);
        if(item == NULL) {
            LV_LOG_WARN("Couldn't create component.");
            return NULL;
        }
        const char * value_of_name = NULL;
        if(attrs) {
            lv_xml_parser_state_t state;
            lv_xml_parser_state_init(&state);
            state.parent = parent;
            state.item = item;

            /* When a component is just created there is no scope where
             * its styles, constants, etc are stored.
             * So leave state.scope = NULL which means the global context.*/

            p = lv_xml_widget_get_extended_widget_processor(scope->extends);
            p->apply_cb(&state, attrs);
#if LV_USE_OBJ_NAME
            value_of_name = lv_xml_get_value_of(attrs, "name");
            if(value_of_name) lv_obj_set_name(item, value_of_name);
#endif
        }

        /*Set a default indexed name for non screens*/
#if LV_USE_OBJ_NAME
        if(lv_obj_get_parent(item) && value_of_name == NULL) {
            char name_buf[128];
            lv_snprintf(name_buf, sizeof(name_buf), "%s_#", scope->name);
            lv_obj_set_name(item, name_buf);
        }
#endif

        return item;
    }

    /* If it isn't a component either then it is unknown */
    LV_LOG_WARN("'%s' is not a known widget, element, or component", name);
    return NULL;
}


void lv_xml_set_default_asset_path(const char * path_prefix)
{
    lv_free((void *)xml_path_prefix);
    if(path_prefix == NULL) path_prefix = "";
    xml_path_prefix = lv_strdup(path_prefix);
}


lv_result_t lv_xml_register_font(lv_xml_component_scope_t * scope, const char * name, const lv_font_t * font)
{

    if(scope == NULL) scope = lv_xml_component_get_scope("globals");
    if(scope == NULL) {
        LV_LOG_WARN("No component found to register font `%s`", name);
        return LV_RESULT_INVALID;
    }

    lv_xml_font_t * f;
    LV_LL_READ(&scope->font_ll, f) {
        if(lv_streq(f->name, name)) {
            LV_LOG_INFO("Font `%s` is already registered. Don't register it again.", name);
            return LV_RESULT_OK;
        }
    }

    f = lv_ll_ins_head(&scope->font_ll);
    lv_memzero(f, sizeof(*f));
    f->name = lv_strdup(name);
    f->font = font;

    return LV_RESULT_OK;
}

const lv_font_t * lv_xml_get_font(lv_xml_component_scope_t * scope, const char * name)
{
    lv_xml_font_t * f;
    if(scope) {
        LV_LL_READ(&scope->font_ll, f) {
            if(lv_streq(f->name, name)) return f->font;
        }
    }

    /*If not found in the component check the global space*/
    if((scope == NULL || scope->name == NULL) || !lv_streq(scope->name, "globals")) {
        scope = lv_xml_component_get_scope("globals");
        if(scope) {
            LV_LL_READ(&scope->font_ll, f) {
                if(lv_streq(f->name, name)) return f->font;
            }
        }
    }

    LV_LOG_WARN("No font was found with name \"%s\". Using LV_FONT_DEFAULT instead.", name);
    return lv_font_get_default();
}

lv_result_t lv_xml_register_subject(lv_xml_component_scope_t * scope, const char * name, lv_subject_t * subject)
{
    if(scope == NULL) scope = lv_xml_component_get_scope("globals");
    if(scope == NULL) {
        LV_LOG_WARN("No component found to register subject `%s`", name);
        return LV_RESULT_INVALID;
    }

    lv_xml_subject_t * s;
    LV_LL_READ(&scope->subjects_ll, s) {
        if(lv_streq(s->name, name)) {
            LV_LOG_INFO("Subject `%s` is already registered. Don't register it again.", name);
            return LV_RESULT_OK;
        }
    }

    s = lv_ll_ins_head(&scope->subjects_ll);
    lv_memzero(s, sizeof(*s));
    s->name = lv_strdup(name);
    s->subject = subject;

    return LV_RESULT_OK;
}

lv_subject_t * lv_xml_get_subject(lv_xml_component_scope_t * scope, const char * name)
{
    lv_xml_subject_t * s;
    if(scope) {
        LV_LL_READ(&scope->subjects_ll, s) {
            if(lv_streq(s->name, name)) return s->subject;
        }
    }

    /*If not found in the component check the global space*/
    if((scope == NULL || scope->name == NULL) || !lv_streq(scope->name, "globals")) {
        scope = lv_xml_component_get_scope("globals");
        if(scope) {
            LV_LL_READ(&scope->subjects_ll, s) {
                if(lv_streq(s->name, name)) return s->subject;
            }
        }
    }

    LV_LOG_WARN("No subject was found with name \"%s\".", name);
    return NULL;
}


lv_result_t lv_xml_register_timeline(lv_xml_component_scope_t * scope, const char * name)
{
    if(scope == NULL) scope = lv_xml_component_get_scope("globals");
    if(scope == NULL) {
        LV_LOG_WARN("No component found to register subject `%s`", name);
        return LV_RESULT_INVALID;
    }

    lv_xml_timeline_t * at;
    LV_LL_READ(&scope->timeline_ll, at) {
        if(lv_streq(at->name, name)) {
            LV_LOG_INFO("Animation timeline `%s` is already registered. Don't register it again.", name);
            return LV_RESULT_OK;
        }
    }

    at = lv_ll_ins_head(&scope->timeline_ll);
    at->name = lv_strdup(name);
    lv_ll_init(&at->anims_ll, sizeof(lv_anim_t));

    return LV_RESULT_OK;
}

void * lv_xml_get_timeline(lv_xml_component_scope_t * scope, const char * name)
{
    lv_xml_timeline_t * at;
    if(scope) {
        LV_LL_READ(&scope->timeline_ll, at) {
            if(lv_streq(at->name, name)) return at;
        }
    }

    /*If not found in the component check the global space*/
    if((scope == NULL || scope->name == NULL) || !lv_streq(scope->name, "globals")) {
        scope = lv_xml_component_get_scope("globals");
        if(scope) {
            LV_LL_READ(&scope->timeline_ll, at) {
                if(lv_streq(at->name, name)) return at;
            }
        }
    }

    LV_LOG_WARN("No timeline was found with name \"%s\".", name);
    return NULL;
}


lv_result_t lv_xml_register_const(lv_xml_component_scope_t * scope, const char * name, const char * value)
{
    if(scope == NULL) scope = lv_xml_component_get_scope("globals");
    if(scope == NULL) {
        LV_LOG_WARN("No component found to register constant `%s`", name);
        return LV_RESULT_INVALID;
    }

    lv_xml_const_t * cnst;
    LV_LL_READ(&scope->const_ll, cnst) {
        if(lv_streq(cnst->name, name)) {
            LV_LOG_INFO("Const `%s` is already registered. Don't register it again.", name);
            return LV_RESULT_OK;
        }
    }

    cnst = lv_ll_ins_head(&scope->const_ll);
    lv_memzero(cnst, sizeof(*cnst));

    cnst->name = lv_strdup(name);
    cnst->value = lv_strdup(value);

    return LV_RESULT_OK;
}

const char * lv_xml_get_const(lv_xml_component_scope_t * scope, const char * name)
{

    if(scope == NULL) scope = lv_xml_component_get_scope("globals");
    if(scope == NULL) return NULL;

    lv_xml_const_t * cnst;
    if(scope) {
        LV_LL_READ(&scope->const_ll, cnst) {
            if(lv_streq(cnst->name, name)) return cnst->value;
        }
    }

    /*If not found in the component check the global space*/
    if((scope == NULL || scope->name == NULL) || !lv_streq(scope->name, "globals")) {
        scope = lv_xml_component_get_scope("globals");
        if(scope) {
            LV_LL_READ(&scope->const_ll, cnst) {
                if(lv_streq(cnst->name, name)) return cnst->value;
            }
        }
    }

    LV_LOG_WARN("No constant was found with name \"%s\".", name);
    return NULL;
}


lv_result_t lv_xml_register_image(lv_xml_component_scope_t * scope, const char * name, const void * src)
{
    if(scope == NULL) scope = lv_xml_component_get_scope("globals");
    if(scope == NULL) {
        LV_LOG_WARN("No component found to register image `%s`", name);
        return LV_RESULT_INVALID;
    }

    lv_xml_image_t * img;
    LV_LL_READ(&scope->image_ll, img) {
        if(lv_streq(img->name, name)) {
            LV_LOG_INFO("Image `%s` is already registered. Don't register it again.", name);
            return LV_RESULT_OK;
        }
    }

    img = lv_ll_ins_head(&scope->image_ll);
    lv_memzero(img, sizeof(*img));
    img->name = lv_strdup(name);
    if(lv_image_src_get_type(src) == LV_IMAGE_SRC_FILE) {
        char buf[LV_XML_MAX_PATH_LENGTH];
        lv_snprintf(buf, sizeof(buf), "%s%s", xml_path_prefix, src);
        img->src = lv_strdup(buf);
    }
    else {
        img->src = src;
    }

    return LV_RESULT_OK;
}

const void * lv_xml_get_image(lv_xml_component_scope_t * scope, const char * name)
{
    if(scope == NULL) scope = lv_xml_component_get_scope("globals");
    if(scope == NULL) return NULL;

    lv_xml_image_t * img;
    if(scope) {
        LV_LL_READ(&scope->image_ll, img) {
            if(lv_streq(img->name, name)) return img->src;
        }
    }

    /*If not found in the component check the global space*/
    if((scope == NULL || scope->name == NULL) || !lv_streq(scope->name, "globals")) {
        scope = lv_xml_component_get_scope("globals");
        if(scope) {
            LV_LL_READ(&scope->image_ll, img) {
                if(lv_streq(img->name, name)) return img->src;
            }
        }
    }

    LV_LOG_WARN("No image was found with name \"%s\"", name);
    return NULL;
}

lv_result_t lv_xml_register_event_cb(lv_xml_component_scope_t * scope, const char * name, lv_event_cb_t cb)
{
    if(scope == NULL) scope = lv_xml_component_get_scope("globals");
    if(scope == NULL) {
        LV_LOG_WARN("No component found to register event `%s`", name);
        return LV_RESULT_INVALID;
    }

    lv_xml_event_cb_t * e;
    LV_LL_READ(&scope->event_ll, e) {
        if(lv_streq(e->name, name)) {
            LV_LOG_INFO("Event_cb `%s` is already registered. Don't register it again.", name);
            return LV_RESULT_OK;
        }
    }

    e = lv_ll_ins_head(&scope->event_ll);
    lv_memzero(e, sizeof(*e));
    e->name = lv_strdup(name);
    e->cb = cb;

    return LV_RESULT_OK;
}


lv_event_cb_t lv_xml_get_event_cb(lv_xml_component_scope_t * scope, const char * name)
{
    if(scope == NULL) scope = lv_xml_component_get_scope("globals");
    if(scope == NULL) return NULL;

    lv_xml_event_cb_t * e;
    if(scope) {
        LV_LL_READ(&scope->event_ll, e) {
            if(lv_streq(e->name, name)) return e->cb;
        }
    }

    /*If not found in the component check the global space*/
    if((scope == NULL || scope->name == NULL) || !lv_streq(scope->name, "globals")) {
        scope = lv_xml_component_get_scope("globals");
        if(scope) {
            LV_LL_READ(&scope->event_ll, e) {
                if(lv_streq(e->name, name)) return e->cb;
            }
        }
    }

    LV_LOG_WARN("No event was found with name \"%s\"", name);
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static const char * get_param_type(lv_xml_component_scope_t * scope, const char * name)
{
    lv_xml_param_t * p;
    LV_LL_READ(&scope->param_ll, p) {
        if(lv_streq(p->name, name)) return p->type;
    }
    return NULL;
}

static const char * get_param_default(lv_xml_component_scope_t * scope, const char * name)
{
    lv_xml_param_t * p;
    LV_LL_READ(&scope->param_ll, p) {
        if(lv_streq(p->name, name)) return p->def;
    }
    return NULL;
}

static void resolve_params(lv_xml_component_scope_t * item_scope, lv_xml_component_scope_t * parent_scope,
                           const char ** item_attrs, const char ** parent_attrs)
{
    uint32_t i;
    for(i = 0; item_attrs[i]; i += 2) {
        const char * value = item_attrs[i + 1];

        if(value[0] == '$') {
            /*E.g. the ${my_color} value is the my_color attribute name on the parent*/
            const char * name_clean = &value[1]; /*skips `$`*/

            const char * type = get_param_type(item_scope, name_clean);
            if(type == NULL) {
                LV_LOG_WARN("'%s' parameter is not defined on '%s'", name_clean, item_scope->name);
            }
            const char * ext_value = lv_xml_get_value_of(parent_attrs, name_clean);
            if(ext_value) {
                /*If the value is not resolved earlier (e.g. it's a top level element created manually)
                 * use the default value*/
                if(ext_value[0] == '#' || ext_value[0] == '$') {
                    ext_value = get_param_default(item_scope, name_clean);
                }
                else if(lv_streq(type, "style")) {
                    lv_xml_style_t * s = lv_xml_get_style_by_name(parent_scope, ext_value);
                    ext_value = s->long_name;
                }
            }
            else {
                /*If the API attribute is not provide don't set it*/
                ext_value = get_param_default(item_scope, name_clean);
            }
            if(ext_value) {
                item_attrs[i + 1] = ext_value;
            }
            else {
                /*Not set and no default value either
                 *Don't set this property*/
                item_attrs[i] = "";
                item_attrs[i + 1] = "";
            }
        }
    }
}

static void resolve_consts(const char ** item_attrs, lv_xml_component_scope_t * scope)
{
    uint32_t i;
    for(i = 0; item_attrs[i]; i += 2) {
        const char * name = item_attrs[i];
        const char * value = item_attrs[i + 1];
        if(lv_streq(name, "styles")) continue; /*Styles will handle it themselves*/
        if(value[0] == '#') {
            const char * value_clean = &value[1];

            const char * const_value = lv_xml_get_const(scope, value_clean);
            if(const_value) {
                item_attrs[i + 1] = const_value;
            }
            /*If the const attribute is not provide don't set it*/
            else {
                item_attrs[i] = "";
                item_attrs[i + 1] = "";
            }
        }
    }
}

static void view_start_element_handler(void * user_data, const char * name, const char ** attrs)
{
    lv_xml_parser_state_t * state = (lv_xml_parser_state_t *)user_data;
    state->tag_name = name;

    bool is_view = false;
    if(lv_streq(name, "view")) {
        const char * extends = lv_xml_get_value_of(attrs, "extends");
        name = extends ? extends : "lv_obj";
        is_view = true;
    }

    lv_obj_t ** current_parent_p = lv_ll_get_tail(&state->parent_ll);
    if(current_parent_p == NULL) {
        if(state->parent == NULL) {
            LV_LOG_ERROR("There is no parent object available for %s. This should never happen.", name);
            return;
        }
        else {
            current_parent_p = &state->parent;
        }
    }
    else {
        state->parent = *current_parent_p;
    }

    /*In `state->attrs` we have parameters of the component creation
     *E.g. <my_button x="10" title="Hello"/>
     *In `attrs` we have the attributes of child of the view.
     *E.g. in `my_button` `<lv_label x="5" text="${title}".
     *This function changes the pointers in the child attributes if the start with '$'
     *with the corresponding parameter. E.g. "text", "${title}" -> "text", "Hello" */
    resolve_params(&state->scope, state->parent_scope, attrs, state->parent_attrs);

    resolve_consts(attrs, &state->scope);

    void * item = NULL;
    /* Select the widget specific parser type based on the name */
    lv_widget_processor_t * p = lv_xml_widget_get_processor(name);
    if(p) {
        item = p->create_cb(state, attrs);
        state->item = item;


        /*If it's a widget remove all styles. E.g. if it extends an `lv_button`
         *now it has the button theme styles. However if it were a real widget
         *it had e.g. `my_widget_class` so the button's theme wouldn't apply on it.
         *Removing the style will ensure a better preview*/
        if(state->scope.is_widget && is_view) lv_obj_remove_style_all(item);

        /*Apply the attributes from e.g. `<lv_slider value="30" x="20">`*/
        if(item) {
            p->apply_cb(state, attrs);
        }
    }

    /* If not a widget, check if it is a component */
    if(item == NULL) {
        item = lv_xml_component_process(state, name, attrs);
        state->item = item;
    }

    /* If it isn't a component either then it is unknown */
    if(item == NULL) {
        LV_LOG_WARN("'%s' is not a known widget, element, or component", name);
        return;
    }

    void ** new_parent = lv_ll_ins_tail(&state->parent_ll);
    *new_parent = item;

    if(is_view) {
        state->view = item;
    }
}

static void view_end_element_handler(void * user_data, const char * name)
{
    LV_UNUSED(name);

    lv_xml_parser_state_t * state = (lv_xml_parser_state_t *)user_data;

    lv_obj_t ** current_parent = lv_ll_get_tail(&state->parent_ll);
    if(current_parent) {
        lv_ll_remove(&state->parent_ll, current_parent);
        lv_free(current_parent);
    }
}

static void get_timeline_from_event_cb(lv_event_t * e)
{
    void ** out = lv_event_get_param(e);
    *out = lv_event_get_user_data(e);
}

static void free_timelines_event_cb(lv_event_t * e)
{
    lv_anim_timeline_t ** at_array = lv_event_get_user_data(e);
    uint32_t i;
    for(i = 0; at_array[i]; i++) {
        lv_free(lv_anim_timeline_get_user_data(at_array[i]));
        lv_anim_timeline_delete(at_array[i]);
    }
    lv_free(at_array);
}

#endif /* LV_USE_XML */
