/**
 * @file lv_xml_component.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_component.h"
#if LV_USE_XML

#include "../../lvgl.h"
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
static void process_font_element(lv_xml_parser_state_t * state, const char * type, const char ** attrs);
static void process_image_element(lv_xml_parser_state_t * state, const char * type, const char ** attrs);
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

    lv_xml_component_ctx_t * global_ctx = lv_ll_ins_head(&component_ctx_ll);
    lv_memzero(global_ctx, sizeof(lv_xml_component_ctx_t));
    lv_xml_component_ctx_init(global_ctx);
    global_ctx->name = lv_strdup("globals");

}

void lv_xml_component_ctx_init(lv_xml_component_ctx_t * ctx)
{
    lv_ll_init(&ctx->style_ll, sizeof(lv_xml_style_t));
    lv_ll_init(&ctx->const_ll, sizeof(lv_xml_const_t));
    lv_ll_init(&ctx->param_ll, sizeof(lv_xml_param_t));
    lv_ll_init(&ctx->gradient_ll, sizeof(lv_xml_grad_t));
    lv_ll_init(&ctx->subjects_ll, sizeof(lv_xml_subject_t));
    lv_ll_init(&ctx->event_ll, sizeof(lv_xml_event_cb_t));
    lv_ll_init(&ctx->image_ll, sizeof(lv_xml_image_t));
    lv_ll_init(&ctx->font_ll, sizeof(lv_xml_font_t));
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
    bool globals = false;
    if(lv_streq(name, "globals")) globals = true;

    /* Create a temporary parser state to extract styles/params/consts */
    lv_xml_parser_state_t state;
    if(globals) {
        lv_xml_component_ctx_t * global_ctx = lv_xml_component_get_ctx("globals");
        state.ctx = *global_ctx;
    }
    else {
        lv_xml_parser_state_init(&state);
        state.ctx.name = name;
    }

    /* Parse the XML to extract metadata */
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser, &state);
    XML_SetElementHandler(parser, start_metadata_handler, end_metadata_handler);

    if(XML_Parse(parser, xml_def, lv_strlen(xml_def), XML_TRUE) == XML_STATUS_ERROR) {
        LV_LOG_ERROR("XML parsing error: %s on line %lu",
                     XML_ErrorString(XML_GetErrorCode(parser)),
                     (unsigned long)XML_GetCurrentLineNumber(parser));
        XML_ParserFree(parser);
        return LV_RESULT_INVALID;
    }

    XML_ParserFree(parser);


    /* Copy extracted metadata to component processor */
    if(globals) {
        lv_xml_component_ctx_t * global_ctx = lv_xml_component_get_ctx("globals");
        lv_memcpy(global_ctx, &state.ctx, sizeof(lv_xml_component_ctx_t));
    }
    else {
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

    lv_xml_const_t * cnst;
    LV_LL_READ(&ctx->const_ll, cnst) {
        lv_free((char *)cnst->name);
        lv_free((char *)cnst->value);
    }
    lv_ll_clear(&ctx->const_ll);

    lv_xml_param_t * param;
    LV_LL_READ(&ctx->param_ll, param) {
        lv_free((char *)param->name);
        lv_free((char *)param->def);
        lv_free((char *)param->type);
    }
    lv_ll_clear(&ctx->param_ll);


    lv_xml_font_t * font;
    LV_LL_READ(&ctx->font_ll, font) {
        lv_free((char *)font->name);
    }
    lv_ll_clear(&ctx->font_ll);

    lv_xml_image_t * image;
    LV_LL_READ(&ctx->image_ll, image) {
        lv_free((char *)image->name);
        lv_free((char *)image->src);
    }
    lv_ll_clear(&ctx->image_ll);

    lv_xml_style_t * style;
    LV_LL_READ(&ctx->style_ll, style) {
        lv_free((char *)style->name);
        lv_free((char *)style->long_name);
        lv_style_reset(&style->style);
    }
    lv_ll_clear(&ctx->style_ll);


    lv_xml_grad_t * grad;
    LV_LL_READ(&ctx->gradient_ll, grad) {
        lv_free((char *)grad->name);
    }
    lv_ll_clear(&ctx->gradient_ll);

    lv_xml_subject_t * subject;
    LV_LL_READ(&ctx->subjects_ll, subject) {
        lv_free((char *)subject->name);
        if(subject->subject->type == LV_SUBJECT_TYPE_STRING) {
            lv_free((char *)subject->subject->prev_value.pointer);
            lv_free((char *)subject->subject->value.pointer);
        }
        lv_free(subject->subject);
    }
    lv_ll_clear(&ctx->subjects_ll);

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

    lv_xml_register_const(&state->ctx, name, value);
}

static void process_font_element(lv_xml_parser_state_t * state, const char * type, const char ** attrs)
{
    const char * name = lv_xml_get_value_of(attrs, "name");
    if(name == NULL) {
        LV_LOG_WARN("'name' is missing from a font");
        return;
    }

    const char * src_path = lv_xml_get_value_of(attrs, "src_path");
    if(src_path == NULL) {
        LV_LOG_WARN("'src_path' is missing from a `%s` font", name);
        return;
    }

    const char * as_file = lv_xml_get_value_of(attrs, "as_file");
    if(as_file == NULL || lv_streq(as_file, "false")) {
        LV_LOG_INFO("Ignore non-file based font `%s`", name);
        return;
    }

    /*E.g. <tiny_ttf name="inter_xl" src_path="fonts/Inter-SemiBold.ttf" size="22"/> */
    if(lv_streq(type, "tiny_ttf")) {
        const char * size = lv_xml_get_value_of(attrs, "size");
        if(size == NULL) {
            LV_LOG_WARN("'size' is missing from a `%s` tiny_ttf font", name);
            return;
        }
#if LV_TINY_TTF_FILE_SUPPORT
        lv_font_t * font = lv_tiny_ttf_create_file(src_path, lv_xml_atoi(size));
        if(font == NULL) {
            LV_LOG_WARN("Couldn't load  `%s` tiny_ttf font", name);
            return;
        }
        lv_result_t res = lv_xml_register_font(&state->ctx, name, font);
        if(res == LV_RESULT_INVALID) {
            LV_LOG_WARN("Failed to register `%s` tiny_ttf font", name);
            lv_tiny_ttf_destroy(font);
            return;
        }

        /*Get the font which was just created and add a destroy_cb*/
        lv_xml_font_t * f = lv_ll_get_head(&state->ctx.font_ll);
        f->font_destroy_cb = lv_tiny_ttf_destroy;

#else
        LV_LOG_WARN("LV_TINY_TTF_FILE_SUPPORT is not enabled for `%s` font", name);

#endif
    }
    else if(lv_streq(type, "bin")) {
        lv_font_t * font = lv_binfont_create(src_path);
        if(font == NULL) {
            LV_LOG_WARN("Couldn't load `%s` bin font", name);
            return;
        }

        lv_result_t res = lv_xml_register_font(&state->ctx, name, font);
        if(res == LV_RESULT_INVALID) {
            LV_LOG_WARN("Failed to register `%s` bin font", name);
            lv_binfont_destroy(font);
            return;
        }

        /*Get the font which was just created and add a destroy_cb*/
        lv_xml_font_t * f = lv_ll_get_head(&state->ctx.font_ll);
        f->font_destroy_cb = lv_binfont_destroy;
    }
    else {
        LV_LOG_WARN("`%s` is a not supported font type", type);
    }
}

static void process_image_element(lv_xml_parser_state_t * state, const char * type, const char ** attrs)
{
    const char * name = lv_xml_get_value_of(attrs, "name");
    if(name == NULL) {
        LV_LOG_WARN("'name' is missing from a font");
        return;
    }

    const char * src_path = lv_xml_get_value_of(attrs, "src_path");
    if(src_path == NULL) {
        LV_LOG_WARN("'src_path' is missing from a `%s` font", name);
        return;
    }

    /* E.g. <file name="avatar" src_path="avatar1.png">*/
    if(lv_streq(type, "file")) {
        lv_xml_register_image(&state->ctx, name, src_path);
    }
    else {
        LV_LOG_INFO("Ignore non-file image `%s`", name);
    }
}

static void process_subject_element(lv_xml_parser_state_t * state, const char * type, const char ** attrs)
{
    const char * name = lv_xml_get_value_of(attrs, "name");
    const char * value = lv_xml_get_value_of(attrs, "value");

    if(name == NULL) {
        LV_LOG_WARN("'name' is missing from a subject");
        return;
    }
    if(value == NULL) {
        LV_LOG_WARN("'value' is missing from a subject");
        return;
    }

    lv_subject_t * subject = lv_malloc(sizeof(lv_subject_t));

    if(lv_streq(type, "int")) lv_subject_init_int(subject, lv_xml_atoi(value));
    else if(lv_streq(type, "color")) lv_subject_init_color(subject, lv_xml_to_color(value));
    else if(lv_streq(type, "string")) {
        /*Simple solution for now. Will be improved later*/
        char * buf_prev = lv_malloc(256);
        char * buf_act = lv_malloc(256);
        lv_subject_init_string(subject, buf_act, buf_prev, 256, value);
    }

    lv_xml_register_subject(&state->ctx, name, subject);
}

static void process_grad_element(lv_xml_parser_state_t * state, const char * tag_name, const char ** attrs)
{
    lv_xml_grad_t * grad = lv_ll_ins_tail(&state->ctx.gradient_ll);
    grad->name = lv_strdup(lv_xml_get_value_of(attrs, "name"));
    lv_grad_dsc_t * dsc = &grad->grad_dsc;
    lv_memzero(dsc, sizeof(lv_grad_dsc_t));
    dsc->extend = LV_GRAD_EXTEND_PAD;

    if(lv_streq(tag_name, "linear")) {
        dsc->dir = LV_GRAD_DIR_LINEAR;
        char buf[64];
        char * buf_p = buf;
        const char * start = lv_xml_get_value_of(attrs, "start");
        lv_strlcpy(buf, start, sizeof(buf));
        dsc->params.linear.start.x = lv_xml_to_size(lv_xml_split_str(&buf_p, ' '));
        dsc->params.linear.start.y = lv_xml_to_size(buf_p);

        buf_p = buf;
        const char * end = lv_xml_get_value_of(attrs, "end");
        lv_strlcpy(buf, end, sizeof(buf));
        dsc->params.linear.end.x = lv_xml_to_size(lv_xml_split_str(&buf_p, ' '));
        dsc->params.linear.end.y = lv_xml_to_size(buf_p);
    }
    else if(lv_streq(tag_name, "radial")) {
        dsc->dir = LV_GRAD_DIR_RADIAL;
        char buf[64];
        char * buf_p = buf;
        const char * center = lv_xml_get_value_of(attrs, "center");
        if(center) {
            lv_strlcpy(buf, center, sizeof(buf));
            dsc->params.radial.end.x = lv_xml_to_size(lv_xml_split_str(&buf_p, ' '));
            dsc->params.radial.end.y = lv_xml_to_size(buf_p);
        }
        else {
            dsc->params.radial.end.x = lv_pct(50);
            dsc->params.radial.end.y = lv_pct(50);
        }
        buf_p = buf;
        const char * center_edge = lv_xml_get_value_of(attrs, "edge");
        if(center_edge) {
            lv_strlcpy(buf, center_edge, sizeof(buf));
            dsc->params.radial.end_extent.x = lv_xml_to_size(lv_xml_split_str(&buf_p, ' '));
            dsc->params.radial.end_extent.y = lv_xml_to_size(buf_p);
        }
        else {
            dsc->params.radial.end_extent.x = lv_pct(100);
            dsc->params.radial.end_extent.y = lv_pct(100);
        }

        buf_p = buf;
        const char * center_radius = lv_xml_get_value_of(attrs, "radius");
        if(center_radius) {
            int32_t r = lv_xml_atoi(center_radius);
            lv_strlcpy(buf, center_edge, sizeof(buf));
            dsc->params.radial.end_extent.x = dsc->params.radial.end.x + r;
            dsc->params.radial.end_extent.y = dsc->params.radial.end.y;
        }

        buf_p = buf;
        const char * focal = lv_xml_get_value_of(attrs, "focal_center");
        if(focal) {
            lv_strlcpy(buf, focal, sizeof(buf));
            dsc->params.radial.focal.x = lv_xml_to_size(lv_xml_split_str(&buf_p, ' '));
            dsc->params.radial.focal.y = lv_xml_to_size(buf_p);
        }
        else {
            dsc->params.radial.focal.x = dsc->params.radial.end.x;
            dsc->params.radial.focal.y = dsc->params.radial.end.y;
        }

        buf_p = buf;
        const char * focal_edge = lv_xml_get_value_of(attrs, "focal_edge");
        if(focal_edge) {
            lv_strlcpy(buf, focal_edge, sizeof(buf));
            dsc->params.radial.focal_extent.x = lv_xml_to_size(lv_xml_split_str(&buf_p, ' '));
            dsc->params.radial.focal_extent.y = lv_xml_to_size(buf_p);
        }
        else {
            dsc->params.radial.focal_extent.x = dsc->params.radial.focal.x;
            dsc->params.radial.focal_extent.y = dsc->params.radial.focal.y;
        }

        buf_p = buf;
        const char * focal_radius = lv_xml_get_value_of(attrs, "focal_radius");
        if(focal_radius) {
            int32_t r = lv_xml_atoi(center_radius);
            lv_strlcpy(buf, center_edge, sizeof(buf));
            dsc->params.radial.focal_extent.x = dsc->params.radial.focal.x + r;
            dsc->params.radial.focal_extent.y = dsc->params.radial.focal.y;
        }

    }

    else if(lv_streq(tag_name, "conical")) {
        dsc->dir = LV_GRAD_DIR_CONICAL;
        char buf[64];
        char * buf_p = buf;
        const char * center = lv_xml_get_value_of(attrs, "center");
        if(center) {
            lv_strlcpy(buf, center, sizeof(buf));
            dsc->params.conical.center.x = lv_xml_to_size(lv_xml_split_str(&buf_p, ' '));
            dsc->params.conical.center.y = lv_xml_to_size(buf_p);
        }
        else {
            dsc->params.conical.center.x = lv_pct(50);
            dsc->params.conical.center.y = lv_pct(50);
        }
        buf_p = buf;
        const char * angle = lv_xml_get_value_of(attrs, "angle");
        if(angle) {
            lv_strlcpy(buf, angle, sizeof(buf));
            dsc->params.conical.start_angle = lv_xml_atoi(lv_xml_split_str(&buf_p, ' '));
            dsc->params.conical.end_angle = lv_xml_atoi(buf_p);
        }
        else {
            dsc->params.conical.start_angle = 0;
            dsc->params.conical.end_angle = 360;
        }
    }
    else if(lv_streq(tag_name, "horizontal")) {
        dsc->dir = LV_GRAD_DIR_HOR;
    }
    else if(lv_streq(tag_name, "vertical")) {
        dsc->dir = LV_GRAD_DIR_VER;
    }
    else {
        LV_LOG_WARN("Unknown gradient type: %s", tag_name);
    }
}


static void process_grad_stop_element(lv_xml_parser_state_t * state, const char ** attrs)
{
    /*Add the stop to the last gradient*/
    lv_xml_grad_t * grad = lv_ll_get_tail(&state->ctx.gradient_ll);
    lv_grad_dsc_t * dsc = &grad->grad_dsc;

    uint32_t idx = dsc->stops_count;
    if(idx == LV_GRADIENT_MAX_STOPS) {
        LV_LOG_WARN("Too many gradient stops. Incresase LV_GRADIENT_MAX_STOPS");
        return;
    }
    const char * color_value = lv_xml_get_value_of(attrs, "color");
    const char * opa_value = lv_xml_get_value_of(attrs, "opa");
    const char * offset_value = lv_xml_get_value_of(attrs, "offset");

    dsc->stops[idx].color = color_value ? lv_xml_to_color(color_value) : lv_color_black();
    dsc->stops[idx].opa = opa_value ? lv_xml_to_opa(opa_value) : LV_OPA_COVER;
    dsc->stops[idx].frac = offset_value ? lv_xml_to_opa(offset_value) : (uint8_t)((int32_t)idx * 255 /
                                                                                  (LV_GRADIENT_MAX_STOPS - 1));

    dsc->stops_count++;
}

static void process_prop_element(lv_xml_parser_state_t * state, const char ** attrs)
{
    lv_xml_param_t * prop = lv_ll_ins_tail(&state->ctx.param_ll);
    prop->name = lv_strdup(lv_xml_get_value_of(attrs, "name"));
    const char * def = lv_xml_get_value_of(attrs, "default");
    if(def) prop->def = lv_strdup(def);
    else prop->def = NULL;

    const char * type = lv_xml_get_value_of(attrs, "type");
    if(type == NULL) type = "compound"; /*If there in no type it means there are <param>s*/
    prop->type = lv_strdup(type);
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
        if(state->ctx.root_widget == NULL) {
            lv_xml_component_ctx_t * extended_component = lv_xml_component_get_ctx(extends);
            if(extended_component) {
                state->ctx.root_widget = extended_component->root_widget;
            }
            else {
                LV_LOG_WARN("The 'extend'ed widget is not found, using `lv_obj` as a fall back");
                state->ctx.root_widget = lv_xml_widget_get_processor("lv_obj");
            }
        }
    }

    if(lv_streq(name, "widget")) state->ctx.is_widget = 1;


    /* Process elements based on current context */
    switch(state->section) {
        case LV_XML_PARSER_SECTION_API:
            if(old_section != state->section) return;   /*Ignore the section opening, e.g. <api>*/
            process_prop_element(state, attrs);
            break;

        case LV_XML_PARSER_SECTION_CONSTS:
            if(old_section != state->section) return;   /*Ignore the section opening, e.g. <consts>*/
            process_const_element(state, attrs);
            break;
        case LV_XML_PARSER_SECTION_GRAD:
            if(old_section != state->section) return;   /*Ignore the section opening, e.g. <gradients>*/
            process_grad_element(state, name, attrs);
            break;
        case LV_XML_PARSER_SECTION_GRAD_STOP:
            process_grad_stop_element(state, attrs);
            break;

        case LV_XML_PARSER_SECTION_STYLES:
            if(old_section != state->section) return;   /*Ignore the section opening, e.g. <styles>*/
            lv_xml_style_register(&state->ctx, attrs);
            break;
        case LV_XML_PARSER_SECTION_FONTS:
            if(old_section != state->section) return;   /*Ignore the section opening, e.g. <styles>*/
            process_font_element(state, name, attrs);
            break;
        case LV_XML_PARSER_SECTION_IMAGES:
            if(old_section != state->section) return;   /*Ignore the section opening, e.g. <styles>*/
            process_image_element(state, name, attrs);
            break;

        case LV_XML_PARSER_SECTION_SUBJECTS:
            if(old_section != state->section) return;   /*Ignore the section opening, e.g. <subjects>*/
            process_subject_element(state, name, attrs);
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
