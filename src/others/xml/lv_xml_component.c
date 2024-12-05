/**
 * @file lv_xml_component.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_component.h"
#if LV_USE_XML

#include "lv_xml_component_private.h"
#include "lv_xml_private.h"
#include "lv_xml_parser.h"
#include "lv_xml_style.h"
#include "lv_xml_base_types.h"
#include "lv_xml_widget.h"
#include "parsers/lv_xml_obj_parser.h"
#include "../../libs/expat/expat.h"
#include "../../misc/lv_fs.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void start_metadata_handler(void * user_data, const char * name, const char ** attrs);
static void end_metadata_handler(void * user_data, const char * name);
static void process_const_element(lv_xml_parser_state_t * state, const char ** attrs);
static void process_prop_element(lv_xml_parser_state_t * state, const char ** attrs);
static char * extract_view_content(const char * xml_definition);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_ll_t component_ctx_ll;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_xml_component_init(void)
{
    lv_ll_init(&component_ctx_ll, sizeof(lv_xml_component_ctx_t));
}


lv_obj_t * lv_xml_component_process(lv_xml_parser_state_t * state, const char * name, const char ** attrs)
{
    lv_xml_component_ctx_t * ctx = lv_xml_component_get_ctx(name);
    if(ctx == NULL) return NULL;
    lv_obj_t * item = lv_xml_create_from_ctx(state->parent, &state->ctx, ctx, attrs);
    if(item == NULL) {
        LV_LOG_WARN("Couldn't create component '%s'", name);
        return NULL;
    }

    /* Apply the properties of the component, e.g. <my_button x="20" styles="red"/> */
    state->item = item;
    ctx->root_widget->apply_cb(state, attrs);

    return item;
}

lv_xml_component_ctx_t * lv_xml_component_get_ctx(const char * component_name)
{
    lv_xml_component_ctx_t * ctx;
    LV_LL_READ(&component_ctx_ll, ctx) {
        if(lv_streq(ctx->name, component_name)) return ctx;
    }

    return NULL;
}

lv_result_t lv_xml_component_register_from_data(const char * name, const char * xml_def)
{
    /* Create a temporary parser state to extract styles/params/consts */
    lv_xml_parser_state_t state;
    lv_xml_parser_state_init(&state);
    state.ctx.name = name;

    /* Parse the XML to extract metadata */
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser, &state);
    XML_SetElementHandler(parser, start_metadata_handler, end_metadata_handler);

    if(XML_Parse(parser, xml_def, lv_strlen(xml_def), XML_TRUE) == XML_STATUS_ERROR) {
        LV_LOG_WARN("XML parsing error: %s on line %lu",
                    XML_ErrorString(XML_GetErrorCode(parser)),
                    (unsigned long)XML_GetCurrentLineNumber(parser));
        XML_ParserFree(parser);
        return LV_RESULT_INVALID;
    }

    XML_ParserFree(parser);

    /* Copy extracted metadata to component processor */
    lv_xml_component_ctx_t * ctx = lv_ll_ins_head(&component_ctx_ll);
    lv_memzero(ctx, sizeof(lv_xml_component_ctx_t));
    lv_memcpy(ctx, &state.ctx, sizeof(lv_xml_component_ctx_t));

    /* Extract view content directly instead of using XML parser */
    ctx->view_def = extract_view_content(xml_def);
    ctx->name = lv_strdup(name);
    if(!ctx->view_def) {
        LV_LOG_WARN("Failed to extract view content");
        /* Clean up and return error */
        lv_free(ctx);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}


lv_result_t lv_xml_component_register_from_file(const char * path)
{
    /* Extract component name from path */
    /* Create a copy of the filename to modify */
    char * filename = lv_strdup(lv_fs_get_last(path));
    const char * ext = lv_fs_get_ext(filename);
    filename[lv_strlen(filename) - lv_strlen(ext) - 1] = '\0'; /*Trim the extension*/

    lv_fs_res_t fs_res;
    lv_fs_file_t f;
    fs_res = lv_fs_open(&f, path, LV_FS_MODE_RD);
    if(fs_res != LV_FS_RES_OK) {
        LV_LOG_WARN("Couldn't open %s", path);
        lv_free(filename);
        return LV_RESULT_INVALID;
    }

    /* Determine file size */
    lv_fs_seek(&f, 0, LV_FS_SEEK_END);
    uint32_t file_size = 0;
    lv_fs_tell(&f, &file_size);
    lv_fs_seek(&f, 0, LV_FS_SEEK_SET);

    /* Create the buffer */
    char * xml_buf = lv_malloc(file_size + 1);
    if(xml_buf == NULL) {
        LV_LOG_WARN("Memory allocation failed for file %s (%d bytes)", path, file_size + 1);
        lv_free(filename);
        lv_fs_close(&f);
        return LV_RESULT_INVALID;
    }

    /* Read the file content  */
    uint32_t rn;
    lv_fs_read(&f, xml_buf, file_size, &rn);
    if(rn != file_size) {
        LV_LOG_WARN("Couldn't read %s fully", path);
        lv_free(filename);
        lv_free(xml_buf);
        lv_fs_close(&f);
        return LV_RESULT_INVALID;
    }

    /* Null-terminate the buffer */
    xml_buf[rn] = '\0';

    /* Register the component */
    lv_result_t res = lv_xml_component_register_from_data(filename, xml_buf);

    /* Housekeeping */
    lv_free(filename);
    lv_free(xml_buf);
    lv_fs_close(&f);

    return res;
}

lv_result_t lv_xml_component_unregister(const char * name)
{
    lv_xml_component_ctx_t * ctx = lv_xml_component_get_ctx(name);
    if(ctx == NULL) return LV_RESULT_INVALID;

    lv_ll_remove(&component_ctx_ll, ctx);

    lv_free((char *)ctx->name);
    lv_free((char *)ctx->view_def);
    lv_ll_clear(&ctx->param_ll);
    lv_ll_clear(&ctx->style_ll);
    lv_free(ctx);

    return LV_RESULT_OK;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void process_const_element(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * name = lv_xml_get_value_of(attrs, "name");
    const char * value = lv_xml_get_value_of(attrs, "value");

    if(name == NULL) {
        LV_LOG_WARN("'name' is missing from a constant");
        return;
    }
    if(value == NULL) {
        LV_LOG_WARN("'value' is missing from a constant");
        return;
    }

    lv_xml_const_t * cnst = lv_ll_ins_tail(&state->ctx.const_ll);
    cnst->name = lv_strdup(name);
    cnst->value = lv_strdup(value);
}

static void process_prop_element(lv_xml_parser_state_t * state, const char ** attrs)
{
    lv_xml_param_t * cnst = lv_ll_ins_tail(&state->ctx.param_ll);
    cnst->name = lv_strdup(lv_xml_get_value_of(attrs, "name"));
    const char * def = lv_xml_get_value_of(attrs, "default");
    if(def) cnst->def = lv_strdup(def);
    else cnst->def = NULL;

    const char * type = lv_xml_get_value_of(attrs, "type");
    if(type == NULL) type = "compound"; /*If there in no type it means there are <param>s*/
    cnst->type = lv_strdup(lv_xml_get_value_of(attrs, "type"));
}

static void start_metadata_handler(void * user_data, const char * name, const char ** attrs)
{
    lv_xml_parser_state_t * state = (lv_xml_parser_state_t *)user_data;

    lv_xml_parser_section_t old_section = state->section;
    lv_xml_parser_start_section(state, name);
    if(lv_streq(name, "view")) {
        const char * extends = lv_xml_get_value_of(attrs, "extends");
        if(extends == NULL) extends = "lv_obj";
        state->ctx.root_widget = lv_xml_widget_get_processor(extends);
    }

    if(lv_streq(name, "widget")) state->ctx.is_widget = 1;

    if(old_section != state->section) return;   /*Ignore the section opening, e.g. <styles>*/

    /* Process elements based on current context */
    switch(state->section) {
        case LV_XML_PARSER_SECTION_API:
            process_prop_element(state, attrs);
            break;

        case LV_XML_PARSER_SECTION_CONSTS:
            process_const_element(state, attrs);
            break;

        case LV_XML_PARSER_SECTION_STYLES:
            if(lv_streq(name, "style")) {
                lv_xml_style_register(&state->ctx, attrs);
            }
            break;

        default:
            break;
    }
}

static void end_metadata_handler(void * user_data, const char * name)
{
    lv_xml_parser_state_t * state = (lv_xml_parser_state_t *)user_data;
    lv_xml_parser_end_section(state, name);
}

static char * extract_view_content(const char * xml_definition)
{
    if(!xml_definition) return NULL;

    /* Find start of view tag */
    const char * start = strstr(xml_definition, "<view");
    if(!start) return NULL;

    /* Find end of view tag */
    const char * end = strstr(xml_definition, "</view>");
    if(!end) return NULL;
    end += 7; /* Include "</view>" in result */

    /* Calculate and allocate length */
    size_t len = end - start;
    char * view_content = lv_malloc(len + 1);
    if(!view_content) return NULL;

    /* Copy content and null terminate */
    lv_memcpy(view_content, start, len);
    view_content[len] = '\0';

    return view_content;
}

#endif /* LV_USE_XML */
