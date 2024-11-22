/**
 * @file lv_xml.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_xml.h"
#if LV_USE_XML

#include "lv_xml_base_types.h"
#include "lv_xml_parser.h"
#include "lv_xml_component.h"
#include "lv_xml_component_private.h"
#include "lv_xml_widget.h"
#include "lv_xml_style.h"
#include "lv_xml.h"
#include "lv_xml_utils.h"
#include "lv_xml_private.h"
#include "parsers/lv_xml_obj_parser.h"
#include "parsers/lv_xml_button_parser.h"
#include "parsers/lv_xml_label_parser.h"
#include "parsers/lv_xml_image_parser.h"
#include "parsers/lv_xml_slider_parser.h"
#include "parsers/lv_xml_tabview_parser.h"
#include "parsers/lv_xml_chart_parser.h"
#include "../../libs/expat/expat.h"
#include "../../draw/lv_draw_image.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void view_start_element_handler(void * user_data, const char * name, const char ** attrs);
static void view_end_element_handler(void * user_data, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_ll_t font_ll;
static lv_ll_t image_ll;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_xml_init(void)
{
    lv_ll_init(&font_ll, sizeof(lv_xml_font_t));
    lv_ll_init(&image_ll, sizeof(lv_xml_image_t));

#if LV_FONT_MONTSERRAT_14
    lv_xml_register_font("lv_montserrat_14", &lv_font_montserrat_14);
#endif

#if LV_FONT_MONTSERRAT_16
    lv_xml_register_font("lv_montserrat_16", &lv_font_montserrat_16);
#endif

#if LV_FONT_MONTSERRAT_18
    lv_xml_register_font("lv_montserrat_18", &lv_font_montserrat_18);
#endif

#if LV_FONT_MONTSERRAT_20
    lv_xml_register_font("lv_montserrat_20", &lv_font_montserrat_20);
#endif

    lv_xml_component_init();

    lv_xml_widget_register("lv_obj", lv_xml_obj_create, lv_xml_obj_apply);
    lv_xml_widget_register("lv_button", lv_xml_button_create, lv_xml_button_apply);
    lv_xml_widget_register("lv_label", lv_xml_label_create, lv_xml_label_apply);
    lv_xml_widget_register("lv_image", lv_xml_image_create, lv_xml_image_apply);
    lv_xml_widget_register("lv_slider", lv_xml_slider_create, lv_xml_slider_apply);
    lv_xml_widget_register("lv_tabview", lv_xml_tabview_create, lv_xml_tabview_apply);
    lv_xml_widget_register("lv_tabview-tab_bar", lv_xml_tabview_tab_bar_create, lv_xml_tabview_tab_bar_apply);
    lv_xml_widget_register("lv_tabview-tab", lv_xml_tabview_tab_create, lv_xml_tabview_tab_apply);
    lv_xml_widget_register("lv_chart", lv_xml_chart_create, lv_xml_chart_apply);
    lv_xml_widget_register("lv_chart-cursor", lv_xml_chart_cursor_create, lv_xml_chart_cursor_apply);
    lv_xml_widget_register("lv_chart-series", lv_xml_chart_series_create, lv_xml_chart_series_apply);
}

lv_obj_t * lv_xml_create_from_ctx(lv_obj_t * parent, lv_xml_component_ctx_t * parent_ctx, lv_xml_component_ctx_t * ctx,
                                  const char ** attrs)
{
    /* Initialize the parser state */
    lv_xml_parser_state_t state;
    lv_xml_parser_state_init(&state);
    state.ctx = *ctx;
    state.parent = parent;
    state.parent_attrs = attrs;
    state.parent_ctx = parent_ctx;

    lv_obj_t ** parent_node = lv_ll_ins_head(&state.parent_ll);
    *parent_node = parent;

    /* Create an XML parser and set handlers */
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser, &state);
    XML_SetElementHandler(parser, view_start_element_handler, view_end_element_handler);

    /* Parse the XML */
    if(XML_Parse(parser, ctx->view_def, lv_strlen(ctx->view_def), XML_TRUE) == XML_STATUS_ERROR) {
        LV_LOG_WARN("XML parsing error: %s on line %lu", XML_ErrorString(XML_GetErrorCode(parser)),
                    XML_GetCurrentLineNumber(parser));
        XML_ParserFree(parser);
        return NULL;
    }

    state.item = state.view;

    if(attrs) {
        ctx->root_widget->apply_cb(&state, attrs);
    }

    lv_ll_clear(&state.parent_ll);
    XML_ParserFree(parser);

    return state.view;
}

lv_obj_t * lv_xml_create(lv_obj_t * parent, const char * name, const char ** attrs)
{
    lv_obj_t * item = NULL;

    /* Select the widget specific parser type based on the name */
    lv_widget_processor_t * p = lv_xml_widget_get_processor(name);
    if(p) {
        lv_xml_parser_state_t state;
        lv_xml_parser_state_init(&state);
        state.parent = parent;
        state.item = p->create_cb(&state, attrs);
        if(attrs) {
            p->apply_cb(&state, attrs);
        }
        return state.item;
    }

    lv_xml_component_ctx_t * ctx = lv_xml_component_get_ctx(name);
    if(ctx) {
        item = lv_xml_create_from_ctx(parent, NULL, ctx, attrs);
        return item;
    }

    /* If it isn't a component either then it is unknown */
    LV_LOG_WARN("'%s' in not a known widget, element, or component", name);
    return NULL;
}


lv_result_t lv_xml_register_font(const char * name, const lv_font_t * font)
{
    lv_xml_font_t * f = lv_ll_ins_head(&font_ll);
    f->name = lv_strdup(name);
    f->font = font;

    return LV_RESULT_OK;
}

const lv_font_t * lv_xml_get_font(const char * name)
{
    lv_xml_font_t * f;
    LV_LL_READ(&font_ll, f) {
        if(lv_streq(f->name, name)) return f->font;
    }

    return NULL;
}

lv_result_t lv_xml_register_image(const char * name, const void * src)
{
    lv_xml_image_t * img = lv_ll_ins_head(&image_ll);
    img->name = lv_strdup(name);
    if(lv_image_src_get_type(src) == LV_IMAGE_SRC_FILE) {
        img->src = lv_strdup(src);
    }
    else {
        img->src = src;
    }

    return LV_RESULT_OK;
}

const void * lv_xml_get_image(const char * name)
{
    lv_xml_image_t * img;
    LV_LL_READ(&image_ll, img) {
        if(lv_streq(img->name, name)) return img->src;
    }

    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static const char * get_param_type(lv_xml_component_ctx_t * ctx, const char * name)
{
    lv_xml_param_t * p;
    LV_LL_READ(&ctx->param_ll, p) {
        if(lv_streq(p->name, name)) return p->type;
    }
    return NULL;
}

static const char * get_param_default(lv_xml_component_ctx_t * ctx, const char * name)
{
    lv_xml_param_t * p;
    LV_LL_READ(&ctx->param_ll, p) {
        if(lv_streq(p->name, name)) return p->def;
    }
    return NULL;
}

static void resolve_params(lv_xml_component_ctx_t * item_ctx, lv_xml_component_ctx_t * parent_ctx,
                           const char ** item_attrs, const char ** parent_attrs)
{
    uint32_t i;
    for(i = 0; item_attrs[i]; i += 2) {
        const char * name = item_attrs[i];
        const char * value = item_attrs[i + 1];
        if(lv_streq(name, "styles")) continue; /*Styles will handle it themselves*/
        if(value[0] == '$') {
            /*E.g. the ${my_color} value is the my_color attribute name on the parent*/
            const char * name_clean = &value[1]; /*skips `$`*/

            const char * type = get_param_type(item_ctx, name_clean);
            if(type == NULL) {
                LV_LOG_WARN("'%s' parameter is not defined on '%s'", name_clean, item_ctx->name);
            }
            const char * ext_value = lv_xml_get_value_of(parent_attrs, name_clean);
            if(ext_value) {
                /*If the value is not resolved earlier (e.g. it's a top level element created manually)
                 * use the default value*/
                if(ext_value[0] == '#' || ext_value[0] == '$') {
                    ext_value = get_param_default(item_ctx, name_clean);
                }
                else if(lv_streq(type, "style")) {
                    lv_xml_style_t * s = lv_xml_get_style_by_name(parent_ctx, ext_value);
                    ext_value = s->long_name;
                }
            }
            else {
                /*If the API attribute is not provide don't set it*/
                ext_value = get_param_default(item_ctx, name_clean);
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

static void resolve_consts(const char ** item_attrs, lv_xml_component_ctx_t * ctx)
{
    uint32_t i;
    for(i = 0; item_attrs[i]; i += 2) {
        const char * name = item_attrs[i];
        const char * value = item_attrs[i + 1];
        if(lv_streq(name, "styles")) continue; /*Styles will handle it themselves*/
        if(value[0] == '#') {
            const char * value_clean = &value[1];

            lv_xml_const_t * c;
            LV_LL_READ(&ctx->const_ll, c) {
                if(lv_streq(c->name, value_clean)) {
                    item_attrs[i + 1] = c->value;
                    break;
                }
            }

            /*If the const attribute is not provide don't set it*/
            if(c == NULL) {
                item_attrs[i] = "";
                item_attrs[i + 1] = "";
            }
        }
    }
}

static void view_start_element_handler(void * user_data, const char * name, const char ** attrs)
{
    lv_xml_parser_state_t * state = (lv_xml_parser_state_t *)user_data;
    bool is_view = false;

    if(lv_streq(name, "view")) {
        const char * extends = lv_xml_get_value_of(attrs, "extends");
        name = extends ? extends : "lv_obj";
        is_view = true;
    }

    lv_obj_t ** current_parent_p = lv_ll_get_tail(&state->parent_ll);
    if(current_parent_p == NULL) {
        if(state->parent == NULL) {
            LV_LOG_ERROR("There is no parent object available for %s. This also should never happen.", name);
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
    resolve_params(&state->ctx, state->parent_ctx, attrs, state->parent_attrs);

    resolve_consts(attrs, &state->ctx);

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
        if(state->ctx.is_widget) lv_obj_remove_style_all(item);

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
        LV_LOG_WARN("'%s' in not a known widget, element, or component", name);
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

#endif /* LV_USE_XML */
