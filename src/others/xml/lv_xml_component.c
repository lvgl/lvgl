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
#include "../../core/lv_global.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define xml_path_prefix LV_GLOBAL_DEFAULT()->xml_path_prefix

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    STYLE_PROP_TYPE_INT,
    STYLE_PROP_TYPE_UNKNOWN
} style_prop_anim_type_t;

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
static style_prop_anim_type_t style_prop_anim_get_type(lv_style_prop_t prop);
static int32_t anim_value_to_int(lv_style_prop_t prop_type, const char * value_str);
static void int_anim_exec_cb(lv_anim_t * a, int32_t v);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_ll_t component_scope_ll;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_xml_component_init(void)
{
    lv_ll_init(&component_scope_ll, sizeof(lv_xml_component_scope_t));

    lv_xml_component_scope_t * global_scope = lv_ll_ins_head(&component_scope_ll);
    lv_memzero(global_scope, sizeof(lv_xml_component_scope_t));

    lv_xml_component_scope_init(global_scope);
    global_scope->name = lv_strdup("globals");
}

void lv_xml_component_scope_init(lv_xml_component_scope_t * scope)
{
    lv_ll_init(&scope->style_ll, sizeof(lv_xml_style_t));
    lv_ll_init(&scope->const_ll, sizeof(lv_xml_const_t));
    lv_ll_init(&scope->param_ll, sizeof(lv_xml_param_t));
    lv_ll_init(&scope->gradient_ll, sizeof(lv_xml_grad_t));
    lv_ll_init(&scope->subjects_ll, sizeof(lv_xml_subject_t));
    lv_ll_init(&scope->event_ll, sizeof(lv_xml_event_cb_t));
    lv_ll_init(&scope->image_ll, sizeof(lv_xml_image_t));
    lv_ll_init(&scope->font_ll, sizeof(lv_xml_font_t));
    lv_ll_init(&scope->timeline_ll, sizeof(lv_xml_timeline_t));
}


lv_obj_t * lv_xml_component_process(lv_xml_parser_state_t * state, const char * name, const char ** attrs)
{
    lv_xml_component_scope_t * scope = lv_xml_component_get_scope(name);
    if(scope == NULL) return NULL;
    lv_obj_t * item = lv_xml_create_in_scope(state->parent, &state->scope, scope, attrs);
    if(item == NULL) {
        LV_LOG_WARN("Couldn't create component '%s'", name);
        return NULL;
    }

    /* Apply the properties of the component, e.g. <my_button x="20" styles="red"/> */
    state->item = item;
    lv_widget_processor_t * extended_proc = lv_xml_widget_get_extended_widget_processor(scope->extends);
    extended_proc->apply_cb(state, attrs);

#if LV_USE_OBJ_NAME
    /*Set a default indexed name*/
    if(state->item) {
        const char * value_of_name = lv_xml_get_value_of(attrs, "name");
        if(value_of_name) lv_obj_set_name(item, value_of_name);
        else {
            char name_buf[128];
            lv_snprintf(name_buf, sizeof(name_buf), "%s_#", scope->name);
            lv_obj_set_name(state->item, name_buf);
        }
    }
#endif
    return item;
}

lv_xml_component_scope_t * lv_xml_component_get_scope(const char * component_name)
{
    lv_xml_component_scope_t * scope;
    LV_LL_READ(&component_scope_ll, scope) {
        if(lv_streq(scope->name, component_name)) return scope;
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
        lv_xml_component_scope_t * global_scope = lv_xml_component_get_scope("globals");
        state.scope = *global_scope;
    }
    else {
        lv_xml_parser_state_init(&state);
        state.scope.name = name;
    }

    /* Parse the XML to extract metadata */
    XML_Memory_Handling_Suite mem_handlers;
    mem_handlers.malloc_fcn = lv_malloc;
    mem_handlers.realloc_fcn = lv_realloc;
    mem_handlers.free_fcn = lv_free;
    XML_Parser parser = XML_ParserCreate_MM(NULL, &mem_handlers, NULL);
    XML_SetUserData(parser, &state);
    XML_SetElementHandler(parser, start_metadata_handler, end_metadata_handler);

    if(XML_Parse(parser, xml_def, lv_strlen(xml_def), XML_TRUE) == XML_STATUS_ERROR) {
        LV_LOG_ERROR("XML parsing error; %s on line %lu",
                     XML_ErrorString(XML_GetErrorCode(parser)),
                     (unsigned long)XML_GetCurrentLineNumber(parser));
        XML_ParserFree(parser);
        lv_free((char *)state.scope.extends);
        return LV_RESULT_INVALID;
    }

    XML_ParserFree(parser);


    /* Copy extracted metadata to component processor */
    if(globals) {
        lv_xml_component_scope_t * global_scope = lv_xml_component_get_scope("globals");
        lv_memcpy(global_scope, &state.scope, sizeof(lv_xml_component_scope_t));
    }
    else {
        lv_xml_component_scope_t * scope = lv_ll_ins_head(&component_scope_ll);
        lv_memzero(scope, sizeof(lv_xml_component_scope_t));
        lv_memcpy(scope, &state.scope, sizeof(lv_xml_component_scope_t));

        /* Extract view content directly instead of using XML parser */
        scope->view_def = extract_view_content(xml_def);
        scope->name = lv_strdup(name);
        if(!scope->view_def) {
            LV_LOG_WARN("Failed to extract view content");
            /* Clean up and return error */
            lv_xml_component_unregister(name);
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
    lv_xml_component_scope_t * scope = lv_xml_component_get_scope(name);
    if(scope == NULL) return LV_RESULT_INVALID;

    lv_ll_remove(&component_scope_ll, scope);

    lv_free((char *)scope->name);
    lv_free((char *)scope->view_def);
    lv_free((char *)scope->extends);

    lv_xml_const_t * cnst;
    LV_LL_READ(&scope->const_ll, cnst) {
        lv_free((char *)cnst->name);
        lv_free((char *)cnst->value);
    }
    lv_ll_clear(&scope->const_ll);

    lv_xml_param_t * param;
    LV_LL_READ(&scope->param_ll, param) {
        lv_free((char *)param->name);
        lv_free((char *)param->def);
        lv_free((char *)param->type);
    }
    lv_ll_clear(&scope->param_ll);


    lv_xml_font_t * font;
    LV_LL_READ(&scope->font_ll, font) {
        lv_free((char *)font->name);
    }
    lv_ll_clear(&scope->font_ll);

    lv_xml_image_t * image;
    LV_LL_READ(&scope->image_ll, image) {
        lv_free((char *)image->name);
        lv_free((char *)image->src);
    }
    lv_ll_clear(&scope->image_ll);

    lv_xml_style_t * style;
    LV_LL_READ(&scope->style_ll, style) {
        lv_free((char *)style->name);
        lv_free((char *)style->long_name);
        lv_style_reset(&style->style);
    }
    lv_ll_clear(&scope->style_ll);


    lv_xml_grad_t * grad;
    LV_LL_READ(&scope->gradient_ll, grad) {
        lv_free((char *)grad->name);
    }
    lv_ll_clear(&scope->gradient_ll);

    lv_xml_subject_t * subject;
    LV_LL_READ(&scope->subjects_ll, subject) {
        lv_free((char *)subject->name);
        if(subject->subject->type == LV_SUBJECT_TYPE_STRING) {
            lv_free((char *)subject->subject->prev_value.pointer);
            lv_free((char *)subject->subject->value.pointer);
        }
        lv_free(subject->subject);
    }
    lv_ll_clear(&scope->subjects_ll);

    lv_xml_timeline_t * timeline;
    LV_LL_READ(&scope->timeline_ll, timeline) {
        lv_anim_t * a;
        LV_LL_READ(&timeline->anims_ll, a) {
            lv_free(a->var); /*It was the name of the target object*/
        }
        lv_ll_clear(&timeline->anims_ll);
        lv_free((char *)timeline->name);
    }
    lv_ll_clear(&scope->timeline_ll);

    lv_free(scope);

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

    lv_xml_register_const(&state->scope, name, value);
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

    char src_path_full[LV_XML_MAX_PATH_LENGTH];
    lv_snprintf(src_path_full, sizeof(src_path_full), "%s%s", xml_path_prefix, src_path);

    lv_xml_font_t * f;
    LV_LL_READ(&state->scope.font_ll, f) {
        if(lv_streq(f->name, name)) {
            LV_LOG_INFO("Font %s is already registered. Don't register it again.", name);
            return;
        }
    }

    /*E.g. <tiny_ttf name="inter_xl" src_path="fonts/Inter-SemiBold.ttf" size="22"/> */
    if(lv_streq(type, "tiny_ttf")) {
        const char * size = lv_xml_get_value_of(attrs, "size");
        if(size == NULL) {
            LV_LOG_WARN("'size' is missing from a `%s` tiny_ttf font", name);
            return;
        }
#if LV_TINY_TTF_FILE_SUPPORT
        lv_font_t * font = lv_tiny_ttf_create_file(src_path_full, lv_xml_atoi(size));
        if(font == NULL) {
            LV_LOG_WARN("Couldn't load  `%s` tiny_ttf font", name);
            return;
        }
        lv_result_t res = lv_xml_register_font(&state->scope, name, font);
        if(res == LV_RESULT_INVALID) {
            LV_LOG_WARN("Failed to register `%s` tiny_ttf font", name);
            lv_tiny_ttf_destroy(font);
            return;
        }

        /*Get the font which was just created and add a destroy_cb*/
        lv_xml_font_t * new_font;
        LV_LL_READ(&state->scope.font_ll, new_font) {
            if(lv_streq(new_font->name, name))  {
                new_font->font_destroy_cb = lv_tiny_ttf_destroy;
                break;
            }
        }

#else
        LV_LOG_WARN("LV_TINY_TTF_FILE_SUPPORT is not enabled for `%s` font", name);

#endif
    }
    else if(lv_streq(type, "bin")) {
        lv_font_t * font = lv_binfont_create(src_path_full);
        if(font == NULL) {
            LV_LOG_WARN("Couldn't load `%s` bin font", name);
            return;
        }

        lv_result_t res = lv_xml_register_font(&state->scope, name, font);
        if(res == LV_RESULT_INVALID) {
            LV_LOG_WARN("Failed to register `%s` bin font", name);
            lv_binfont_destroy(font);
            return;
        }

        lv_xml_font_t * new_font;
        LV_LL_READ(&state->scope.font_ll, new_font) {
            if(lv_streq(new_font->name, name))  {
                new_font->font_destroy_cb = lv_binfont_destroy;
                break;
            }
        }
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
        lv_xml_register_image(&state->scope, name, src_path);
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

    lv_subject_t * subject = lv_zalloc(sizeof(lv_subject_t));

    if(lv_streq(type, "int")) lv_subject_init_int(subject, lv_xml_atoi(value));
#if LV_USE_FLOAT
    else if(lv_streq(type, "float")) lv_subject_init_float(subject, lv_xml_atof(value));
#endif
    else if(lv_streq(type, "color")) lv_subject_init_color(subject, lv_xml_to_color(value));
    else if(lv_streq(type, "string")) {
        /*Simple solution for now. Will be improved later*/
        char * buf_prev = lv_malloc(256);
        char * buf_act = lv_malloc(256);
        lv_subject_init_string(subject, buf_act, buf_prev, 256, value);
    }

    lv_xml_register_subject(&state->scope, name, subject);
}

static void process_timeline_element(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * name = lv_xml_get_value_of(attrs, "name");

    if(name == NULL) {
        LV_LOG_WARN("'name' is missing from a timeline");
        return;
    }

    /*If already registered skip all. Don't set state->context
     *so animations won't be added later either*/
    lv_xml_timeline_t * at;
    LV_LL_READ(&state->scope.timeline_ll, at) {
        if(lv_streq(at->name, name)) {
            LV_LOG_INFO("Timeline %s is already registered. Don't register it again.", name);
            return;
        }
    }

    lv_xml_register_timeline(&state->scope, name);

    /*Save the created timeline so that animations can be added to it later*/
    state->context = lv_xml_get_timeline(&state->scope, name);
}

static void process_animation_element(lv_xml_parser_state_t * state, const char ** attrs)
{
    if(state->context == NULL) {
        LV_LOG_INFO("No parent timeline is set, skipping");
        return;
    }

    const char * target_str = lv_xml_get_value_of(attrs, "target");
    const char * prop_str = lv_xml_get_value_of(attrs, "prop");
    const char * start_str = lv_xml_get_value_of(attrs, "start");
    const char * end_str = lv_xml_get_value_of(attrs, "end");
    const char * duration_str = lv_xml_get_value_of(attrs, "duration");
    const char * delay_str = lv_xml_get_value_of(attrs, "delay");
    const char * early_apply_str = lv_xml_get_value_of(attrs, "early_apply");
    const char * selector_str = lv_xml_get_value_of(attrs, "selector");

    if(target_str == NULL) {
        LV_LOG_WARN("'target' is missing from a animation");
        return;
    }

    if(prop_str == NULL) {
        LV_LOG_WARN("'prop' is missing from a animation");
        return;
    }

    lv_style_prop_t prop = lv_xml_style_prop_to_enum(prop_str);
    if(prop == LV_STYLE_PROP_INV) {
        LV_LOG_WARN("Unknown style property; '%s'", prop_str);
        return;
    }

    style_prop_anim_type_t prop_type = style_prop_anim_get_type(prop);
    if(prop_type == STYLE_PROP_TYPE_UNKNOWN) {
        LV_LOG_WARN("Style property '%s' is not animateable", prop_str);
        return;
    }

    if(start_str == NULL) {
        LV_LOG_WARN("'start' is missing from a animation");
        return;
    }

    if(end_str == NULL) {
        LV_LOG_WARN("'end' is missing from a animation");
        return;
    }

    if(duration_str == NULL) duration_str = "1000";
    if(delay_str == NULL) delay_str = "0";
    if(early_apply_str == NULL) early_apply_str = "false";
    if(selector_str == NULL) selector_str = "";

    lv_style_selector_t selector = lv_xml_style_selector_text_to_enum(selector_str);

    int32_t start = anim_value_to_int(prop_type, start_str);
    int32_t end = anim_value_to_int(prop_type, end_str);

    lv_xml_timeline_t * at = state->context;
    if(at == NULL) {
        LV_LOG_WARN("There was no parent timeline for the animation");
        return;
    }

    if(target_str[0] == '#') target_str = lv_xml_get_const(&state->scope, &target_str[1]);
    if(prop_str[0] == '#') prop_str = lv_xml_get_const(&state->scope, &prop_str[1]);
    if(start_str[0] == '#') start_str = lv_xml_get_const(&state->scope, &start_str[1]);
    if(end_str[0] == '#') end_str = lv_xml_get_const(&state->scope, &end_str[1]);
    if(duration_str[0] == '#') duration_str = lv_xml_get_const(&state->scope, &duration_str[1]);
    if(delay_str[0] == '#') delay_str = lv_xml_get_const(&state->scope, &delay_str[1]);
    if(early_apply_str[0] == '#') early_apply_str = lv_xml_get_const(&state->scope, &early_apply_str[1]);

    if(!target_str || !prop_str || !start_str || !end_str || !duration_str || !delay_str || !early_apply_str) {
        LV_LOG_WARN("Couldn't resolve one or more constants. Skipping the animation.");
        return;
    }

    uint32_t selector_and_prop = ((prop & 0xff) << 24) | selector;

    lv_anim_t * a = lv_ll_ins_tail(&at->anims_ll);

    lv_anim_init(a);
    lv_anim_set_var(a, lv_strdup(target_str));
    lv_anim_set_values(a, start, end);
    lv_anim_set_custom_exec_cb(a, int_anim_exec_cb);
    lv_anim_set_duration(a, lv_xml_atoi(duration_str));
    lv_anim_set_delay(a, lv_xml_atoi(delay_str));
    lv_anim_set_early_apply(a, lv_xml_to_bool(early_apply_str));
    lv_anim_set_user_data(a, (void *)((uintptr_t)selector_and_prop));
}

static void process_grad_element(lv_xml_parser_state_t * state, const char * tag_name, const char ** attrs)
{
    lv_xml_grad_t * grad = lv_ll_ins_tail(&state->scope.gradient_ll);
    lv_memzero(grad, sizeof(lv_xml_grad_t));

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
        LV_LOG_WARN("Unknown gradient type; %s", tag_name);
    }
}


static void process_grad_stop_element(lv_xml_parser_state_t * state, const char ** attrs)
{
    /*Add the stop to the last gradient*/
    lv_xml_grad_t * grad = lv_ll_get_tail(&state->scope.gradient_ll);
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
    lv_xml_param_t * prop = lv_ll_ins_tail(&state->scope.param_ll);
    lv_memzero(prop, sizeof(lv_xml_param_t));

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

        state->scope.extends = lv_strdup(extends);
    }

    if(lv_streq(name, "widget")) state->scope.is_widget = 1;
    else if(lv_streq(name, "screen")) state->scope.is_screen = 1;

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
            lv_xml_style_register(&state->scope, attrs);
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
        case LV_XML_PARSER_SECTION_TIMELINE:
            process_timeline_element(state, attrs);
            break;
        case LV_XML_PARSER_SECTION_ANIMATION:
            process_animation_element(state, attrs);
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
    if(end) {
        end += 7; /* Include "</view>" in result */
    }
    else {
        /*If there is no "</view> maybe it's like <view ... />"*/
        end = strstr(start, "/>");
        if(!end) return NULL;
        end += 2; /* Include "/>" in result */
    }

    /* Calculate and allocate length */
    size_t len = end - start;
    char * view_content = lv_malloc(len + 1);
    if(!view_content) return NULL;

    /* Copy content and null terminate */
    lv_memcpy(view_content, start, len);
    view_content[len] = '\0';

    return view_content;
}


static style_prop_anim_type_t style_prop_anim_get_type(lv_style_prop_t prop)
{
    switch(prop) {
        case LV_STYLE_WIDTH:
        case LV_STYLE_MIN_WIDTH:
        case LV_STYLE_MAX_WIDTH:
        case LV_STYLE_HEIGHT:
        case LV_STYLE_MIN_HEIGHT:
        case LV_STYLE_MAX_HEIGHT:
        case LV_STYLE_LENGTH:
        case LV_STYLE_RADIUS:
        case LV_STYLE_PAD_LEFT:
        case LV_STYLE_PAD_RIGHT:
        case LV_STYLE_PAD_TOP:
        case LV_STYLE_PAD_BOTTOM:
        case LV_STYLE_PAD_ROW:
        case LV_STYLE_PAD_COLUMN:
        case LV_STYLE_PAD_RADIAL:
        case LV_STYLE_MARGIN_LEFT:
        case LV_STYLE_MARGIN_RIGHT:
        case LV_STYLE_MARGIN_TOP:
        case LV_STYLE_MARGIN_BOTTOM:
        case LV_STYLE_BG_OPA:
        case LV_STYLE_BG_MAIN_STOP:
        case LV_STYLE_BG_GRAD_STOP:
        case LV_STYLE_BG_IMAGE_RECOLOR_OPA:
        case LV_STYLE_BORDER_WIDTH:
        case LV_STYLE_BORDER_OPA:
        case LV_STYLE_OUTLINE_WIDTH:
        case LV_STYLE_OUTLINE_OPA:
        case LV_STYLE_OUTLINE_PAD:
        case LV_STYLE_SHADOW_WIDTH:
        case LV_STYLE_SHADOW_OFFSET_X:
        case LV_STYLE_SHADOW_OFFSET_Y:
        case LV_STYLE_SHADOW_SPREAD:
        case LV_STYLE_SHADOW_OPA:
        case LV_STYLE_TEXT_OPA:
        case LV_STYLE_TEXT_LETTER_SPACE:
        case LV_STYLE_TEXT_LINE_SPACE:
        case LV_STYLE_IMAGE_OPA:
        case LV_STYLE_IMAGE_RECOLOR_OPA:
        case LV_STYLE_LINE_OPA:
        case LV_STYLE_LINE_WIDTH:
        case LV_STYLE_LINE_DASH_WIDTH:
        case LV_STYLE_LINE_DASH_GAP:
        case LV_STYLE_ARC_OPA:
        case LV_STYLE_ARC_WIDTH:
        case LV_STYLE_OPA:
        case LV_STYLE_OPA_LAYERED:
        case LV_STYLE_COLOR_FILTER_OPA:
        case LV_STYLE_TRANSFORM_WIDTH:
        case LV_STYLE_TRANSFORM_HEIGHT:
        case LV_STYLE_TRANSLATE_X:
        case LV_STYLE_TRANSLATE_Y:
        case LV_STYLE_TRANSLATE_RADIAL:
        case LV_STYLE_TRANSFORM_SCALE_X:
        case LV_STYLE_TRANSFORM_SCALE_Y:
        case LV_STYLE_TRANSFORM_ROTATION:
        case LV_STYLE_TRANSFORM_PIVOT_X:
        case LV_STYLE_TRANSFORM_PIVOT_Y:
        case LV_STYLE_RECOLOR_OPA:
            return STYLE_PROP_TYPE_INT;

        default:
            return STYLE_PROP_TYPE_UNKNOWN;

    }
}

static int32_t anim_value_to_int(lv_style_prop_t prop_type, const char * value_str)
{
    if(prop_type == STYLE_PROP_TYPE_INT) {
        return lv_xml_atoi(value_str);
    }

    return 0;
}

static void int_anim_exec_cb(lv_anim_t * a, int32_t v)
{
    uint32_t data = (lv_uintptr_t)lv_anim_get_user_data(a);
    lv_style_prop_t prop = data >> 24;
    lv_style_selector_t selector = data & 0x00ffffff;

    lv_style_value_t style_value;
    style_value.num = v;
    lv_obj_set_local_style_prop(a->var, prop, style_value, selector);
}


#endif /* LV_USE_XML */
