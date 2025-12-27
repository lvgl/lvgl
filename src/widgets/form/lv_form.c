/**
 * @file lv_form.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_form_private.h"
#include "../../core/lv_obj_class_private.h"

#if LV_USE_FORM

#include "../../misc/lv_assert.h"
#include "../../core/lv_obj_private.h"
#include "../../stdlib/lv_string.h"
#include "../../stdlib/lv_sprintf.h"
#include "../../misc/lv_event.h"
#include "../../layouts/lv_layout.h"
#include "../../layouts/flex/lv_flex.h"
#include "../label/lv_label.h"
#include "../textarea/lv_textarea.h"
#include "../dropdown/lv_dropdown.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_form_class)
#define MY_CLASS_SECTION (&lv_form_section_class)
#define MY_CLASS_FIELD (&lv_form_field_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_form_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_form_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_form_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void lv_form_section_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_form_section_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_form_section_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void lv_form_field_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_form_field_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_form_field_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void update_form_layout(lv_obj_t * obj);
static void update_section_layout(lv_obj_t * obj);
static lv_form_validation_t validate_field(lv_obj_t * field);
static void form_submit_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_OBJ_PROPERTY
static const lv_property_ops_t lv_form_properties[] = {
    {
        .id = LV_PROPERTY_FORM_LAYOUT,
        .setter = (void (*)(lv_obj_t *, lv_property_t))lv_form_set_layout,
        .getter = (lv_property_t (*)(const lv_obj_t *))lv_form_get_layout,
    },
    {
        .id = LV_PROPERTY_FORM_SPACING,
        .setter = (void (*)(lv_obj_t *, lv_property_t))lv_form_set_spacing,
        .getter = (lv_property_t (*)(const lv_obj_t *))lv_form_get_spacing,
    },
};

static const lv_property_ops_t lv_form_section_properties[] = {
    {
        .id = LV_PROPERTY_FORM_SECTION_TITLE,
        .setter = (void (*)(lv_obj_t *, lv_property_t))lv_form_section_set_title,
        .getter = (lv_property_t (*)(const lv_obj_t *))lv_form_section_get_title,
    },
};

static const lv_property_ops_t lv_form_field_properties[] = {
    {
        .id = LV_PROPERTY_FORM_FIELD_REQUIRED,
        .setter = (void (*)(lv_obj_t *, lv_property_t))lv_form_field_set_required,
        .getter = (lv_property_t (*)(const lv_obj_t *))lv_form_field_is_required,
    },
    {
        .id = LV_PROPERTY_FORM_FIELD_VALUE,
        .setter = (void (*)(lv_obj_t *, lv_property_t))lv_form_field_set_value,
        .getter = (lv_property_t (*)(const lv_obj_t *))lv_form_field_get_value,
    },
};
#endif

const lv_obj_class_t lv_form_class = {
    .constructor_cb = lv_form_constructor,
    .destructor_cb = lv_form_destructor,
    .event_cb = lv_form_event,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_form_t),
    .base_class = &lv_obj_class,
    .name = "lv_form",
    LV_PROPERTY_CLASS_FIELDS(form, FORM)
};

const lv_obj_class_t lv_form_section_class = {
    .constructor_cb = lv_form_section_constructor,
    .destructor_cb = lv_form_section_destructor,
    .event_cb = lv_form_section_event,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_form_section_t),
    .base_class = &lv_obj_class,
    .name = "lv_form_section",
    LV_PROPERTY_CLASS_FIELDS(form_section, FORM_SECTION)
};

const lv_obj_class_t lv_form_field_class = {
    .constructor_cb = lv_form_field_constructor,
    .destructor_cb = lv_form_field_destructor,
    .event_cb = lv_form_field_event,
    .width_def = LV_PCT(100),
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_form_field_t),
    .base_class = &lv_obj_class,
    .name = "lv_form_field",
    LV_PROPERTY_CLASS_FIELDS(form_field, FORM_FIELD)
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_form_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_form_set_layout(lv_obj_t * obj, lv_form_layout_t layout)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_form_t * form = (lv_form_t *)obj;

    if(form->layout == layout) return;

    form->layout = layout;
    update_form_layout(obj);
    lv_obj_invalidate(obj);
}

lv_form_layout_t lv_form_get_layout(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_form_t * form = (lv_form_t *)obj;
    return form->layout;
}

void lv_form_set_spacing(lv_obj_t * obj, int32_t spacing)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_form_t * form = (lv_form_t *)obj;

    if(form->spacing == spacing) return;

    form->spacing = spacing;
    update_form_layout(obj);
    lv_obj_invalidate(obj);
}

int32_t lv_form_get_spacing(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_form_t * form = (lv_form_t *)obj;
    return form->spacing;
}

void lv_form_clean(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_clean(obj);
}

lv_obj_t * lv_form_add_section(lv_obj_t * form, const char * title)
{
    LV_ASSERT_OBJ(form, MY_CLASS);

    lv_obj_t * section = lv_obj_class_create_obj(MY_CLASS_SECTION, form);
    lv_obj_class_init_obj(section);

    if(title) {
        lv_form_section_set_title(section, title);
    }

    return section;
}

void lv_form_section_set_title(lv_obj_t * section, const char * title)
{
    LV_ASSERT_OBJ(section, MY_CLASS_SECTION);
    lv_form_section_t * sec = (lv_form_section_t *)section;

    /* Free old title if it was dynamically allocated */
    if(sec->title && !sec->static_title) {
        lv_free(sec->title);
    }

    if(title == NULL) {
        sec->title = NULL;
        sec->static_title = false;
    }
    else {
        size_t len = lv_strlen(title);
        sec->title = lv_malloc(len + 1);
        lv_strcpy(sec->title, title);
        sec->static_title = false;
    }

    /* Create or update label */
    lv_obj_t * label = lv_obj_get_child(section, 0);
    if(label && lv_obj_has_class(label, &lv_label_class)) {
        if(title) {
            lv_label_set_text(label, title);
            lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
        }
        else {
            lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else if(title) {
        label = lv_label_create(section);
        lv_label_set_text(label, title);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_decor(label, LV_TEXT_DECOR_UNDERLINE, 0);
    }

    update_section_layout(section);
}

const char * lv_form_section_get_title(const lv_obj_t * section)
{
    LV_ASSERT_OBJ(section, MY_CLASS_SECTION);
    lv_form_section_t * sec = (lv_form_section_t *)section;
    return sec->title;
}

void lv_form_section_set_field_spacing(lv_obj_t * section, int32_t spacing)
{
    LV_ASSERT_OBJ(section, MY_CLASS_SECTION);
    lv_form_section_t * sec = (lv_form_section_t *)section;

    if(sec->field_spacing == spacing) return;

    sec->field_spacing = spacing;
    update_section_layout(section);
    lv_obj_invalidate(section);
}

int32_t lv_form_section_get_field_spacing(const lv_obj_t * section)
{
    LV_ASSERT_OBJ(section, MY_CLASS_SECTION);
    lv_form_section_t * sec = (lv_form_section_t *)section;
    return sec->field_spacing;
}

lv_obj_t * lv_form_add_field(lv_obj_t * section, const char * label, lv_obj_t * input)
{
    LV_ASSERT_OBJ(section, MY_CLASS_SECTION);
    LV_ASSERT_NULL(input);

    lv_obj_t * field = lv_obj_class_create_obj(MY_CLASS_FIELD, section);
    lv_obj_class_init_obj(field);

    lv_form_field_t * fld = (lv_form_field_t *)field;

    /* Create label */
    if(label) {
        fld->label = lv_label_create(field);
        lv_label_set_text(fld->label, label);
        lv_obj_set_style_text_align(fld->label, LV_TEXT_ALIGN_RIGHT, 0);
    }

    /* Add input widget to field */
    if(input) {
        lv_obj_set_parent(input, field);
        fld->input = input;
    }

    /* Set up field layout */
    lv_obj_set_flex_flow(field, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(field, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_flex_grow(field, 1);

    /* Set spacing */
    lv_obj_set_style_pad_gap(field, 10, 0);

    return field;
}

void lv_form_field_set_required(lv_obj_t * field, bool required)
{
    LV_ASSERT_OBJ(field, MY_CLASS_FIELD);
    lv_form_field_t * fld = (lv_form_field_t *)field;

    fld->required = required;

    /* Update label style to indicate required */
    if(fld->label) {
        if(required) {
            lv_obj_set_style_text_color(fld->label, lv_palette_main(LV_PALETTE_RED), 0);
            /* Add asterisk */
            const char * text = lv_label_get_text(fld->label);
            if(text && text[0] != '*') {
                char * new_text = lv_malloc(lv_strlen(text) + 2);
                new_text[0] = '*';
                lv_strcpy(new_text + 1, text);
                lv_label_set_text(fld->label, new_text);
                lv_free(new_text);
            }
        }
        else {
            lv_obj_set_style_text_color(fld->label, lv_color_black(), 0);
        }
    }
}

bool lv_form_field_is_required(const lv_obj_t * field)
{
    LV_ASSERT_OBJ(field, MY_CLASS_FIELD);
    lv_form_field_t * fld = (lv_form_field_t *)field;
    return fld->required;
}

lv_form_validation_t lv_form_field_get_validation(const lv_obj_t * field)
{
    LV_ASSERT_OBJ(field, MY_CLASS_FIELD);
    lv_form_field_t * fld = (lv_form_field_t *)field;
    return fld->validation;
}

void lv_form_field_set_value(lv_obj_t * field, const char * value)
{
    LV_ASSERT_OBJ(field, MY_CLASS_FIELD);
    lv_form_field_t * fld = (lv_form_field_t *)field;

    /* Free old value */
    if(fld->value) {
        lv_free(fld->value);
    }

    if(value) {
        size_t len = lv_strlen(value);
        fld->value = lv_malloc(len + 1);
        lv_strcpy(fld->value, value);
    }
    else {
        fld->value = NULL;
    }

    /* Update input widget if it supports text */
    if(fld->input) {
        if(lv_obj_has_class(fld->input, &lv_textarea_class)) {
            lv_textarea_set_text(fld->input, value ? value : "");
        }
        else if(lv_obj_has_class(fld->input, &lv_label_class)) {
            lv_label_set_text(fld->input, value ? value : "");
        }
    }

    /* Validate */
    fld->validation = validate_field(field);
}

const char * lv_form_field_get_value(const lv_obj_t * field)
{
    LV_ASSERT_OBJ(field, MY_CLASS_FIELD);
    lv_form_field_t * fld = (lv_form_field_t *)field;

    /* Try to get value from input widget if no stored value */
    if(fld->value == NULL && fld->input) {
        if(lv_obj_has_class(fld->input, &lv_textarea_class)) {
            return lv_textarea_get_text(fld->input);
        }
    }

    return fld->value;
}

lv_obj_t * lv_form_field_get_label(const lv_obj_t * field)
{
    LV_ASSERT_OBJ(field, MY_CLASS_FIELD);
    lv_form_field_t * fld = (lv_form_field_t *)field;
    return fld->label;
}

lv_obj_t * lv_form_field_get_input(const lv_obj_t * field)
{
    LV_ASSERT_OBJ(field, MY_CLASS_FIELD);
    lv_form_field_t * fld = (lv_form_field_t *)field;
    return fld->input;
}

lv_result_t lv_form_get_data(lv_obj_t * form, char * buffer, size_t size)
{
    LV_ASSERT_OBJ(form, MY_CLASS);
    LV_ASSERT_NULL(buffer);

    if(size == 0) return LV_RESULT_INVALID;

    size_t pos = 0;
    buffer[0] = '\0';

    /* Iterate through sections */
    uint32_t child_cnt = lv_obj_get_child_count(form);
    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(form, i);
        if(!lv_obj_has_class(child, &lv_form_section_class)) continue;

        lv_form_section_t * section = (lv_form_section_t *)child;
        const char * section_title = section->title;

        /* Add section header */
        if(section_title) {
            int ret = lv_snprintf(buffer + pos, size - pos, "[%s]\n", section_title);
            if(ret > 0) pos += ret;
        }

        /* Iterate through fields */
        uint32_t field_cnt = lv_obj_get_child_count(child);
        for(uint32_t j = 0; j < field_cnt; j++) {
            lv_obj_t * field = lv_obj_get_child(child, j);
            if(!lv_obj_has_class(field, &lv_form_field_class)) continue;

            lv_form_field_t * fld = (lv_form_field_t *)field;
            const char * label = fld->label ? lv_label_get_text(fld->label) : "field";
            const char * value = lv_form_field_get_value(field);

            /* Skip asterisk in label */
            if(label && label[0] == '*') label++;

            int ret = lv_snprintf(buffer + pos, size - pos, "  %s: %s\n", label ? label : "", value ? value : "");
            if(ret > 0) pos += ret;

            if(pos >= size - 1) break;
        }

        if(pos >= size - 1) break;
    }

    return LV_RESULT_OK;
}

bool lv_form_validate(lv_obj_t * form)
{
    LV_ASSERT_OBJ(form, MY_CLASS);
    bool all_valid = true;

    /* Iterate through sections */
    uint32_t child_cnt = lv_obj_get_child_count(form);
    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(form, i);
        if(!lv_obj_has_class(child, &lv_form_section_class)) continue;

        /* Iterate through fields */
        uint32_t field_cnt = lv_obj_get_child_count(child);
        for(uint32_t j = 0; j < field_cnt; j++) {
            lv_obj_t * field = lv_obj_get_child(child, j);
            if(!lv_obj_has_class(field, &lv_form_field_class)) continue;

            lv_form_validation_t val = validate_field(field);
            if(val != LV_FORM_VALIDATION_OK) {
                all_valid = false;
            }
        }
    }

    return all_valid;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_form_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_form_t * form = (lv_form_t *)obj;

    /* Initialize widget data */
    form->layout = LV_FORM_LAYOUT_VERTICAL;
    form->spacing = 10;
    form->flags = 0;

    /* Set up flex layout for vertical arrangement */
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(obj, form->spacing, 0);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_form_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    /* No dynamic resources to free in form itself */
}

static void lv_form_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    if(code == LV_EVENT_STYLE_CHANGED) {
        update_form_layout(obj);
    }
}

static void lv_form_section_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_form_section_t * section = (lv_form_section_t *)obj;

    /* Initialize widget data */
    section->title = NULL;
    section->static_title = false;
    section->field_spacing = 10;

    /* Set up flex layout for vertical arrangement */
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(obj, section->field_spacing, 0);
    lv_obj_set_style_pad_all(obj, 5, 0);
    lv_obj_set_style_border_width(obj, 1, 0);
    lv_obj_set_style_border_color(obj, lv_palette_lighten(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_radius(obj, 5, 0);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_form_section_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_form_section_t * section = (lv_form_section_t *)obj;

    /* Free title if dynamically allocated */
    if(section->title && !section->static_title) {
        lv_free(section->title);
    }
}

static void lv_form_section_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;
    res = lv_obj_event_base(MY_CLASS_SECTION, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    if(code == LV_EVENT_STYLE_CHANGED) {
        update_section_layout(obj);
    }
}

static void lv_form_field_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_form_field_t * field = (lv_form_field_t *)obj;

    /* Initialize widget data */
    field->label = NULL;
    field->input = NULL;
    field->value = NULL;
    field->required = false;
    field->validation = LV_FORM_VALIDATION_OK;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_form_field_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_form_field_t * field = (lv_form_field_t *)obj;

    /* Free value if dynamically allocated */
    if(field->value) {
        lv_free(field->value);
    }
}

static void lv_form_field_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;
    res = lv_obj_event_base(MY_CLASS_FIELD, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_form_field_t * field = (lv_form_field_t *)obj;

    /* Handle value changes from input widgets */
    if(code == LV_EVENT_VALUE_CHANGED && field->input) {
        const char * value = NULL;

        if(lv_obj_has_class(field->input, &lv_textarea_class)) {
            value = lv_textarea_get_text(field->input);
        }
        else if(lv_obj_has_class(field->input, &lv_label_class)) {
            value = lv_label_get_text(field->input);
        }

        if(value) {
            /* Update stored value */
            if(field->value) lv_free(field->value);
            size_t len = lv_strlen(value);
            field->value = lv_malloc(len + 1);
            lv_strcpy(field->value, value);

            /* Validate */
            field->validation = validate_field(obj);

            /* Propagate event to form */
            lv_obj_t * form = lv_obj_get_parent(lv_obj_get_parent(obj)); /* field -> section -> form */
            if(form && lv_obj_has_class(form, &lv_form_class)) {
                lv_form_event_data_t event_data = {
                    .form = form,
                    .field = obj,
                    .value = value,
                    .is_valid = (field->validation == LV_FORM_VALIDATION_OK)
                };
                lv_event_send(form, LV_EVENT_VALUE_CHANGED, &event_data);
            }
        }
    }
}

static void update_form_layout(lv_obj_t * obj)
{
    lv_form_t * form = (lv_form_t *)obj;

    /* Update spacing */
    lv_obj_set_style_pad_gap(obj, form->spacing, 0);

    /* Update layout mode */
    switch(form->layout) {
        case LV_FORM_LAYOUT_VERTICAL:
            lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
            break;
        case LV_FORM_LAYOUT_HORIZONTAL:
            lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
            break;
        case LV_FORM_LAYOUT_GRID:
            /* Grid layout would need column/row descriptors */
            /* For now, use row wrap */
            lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW_WRAP);
            break;
    }
}

static void update_section_layout(lv_obj_t * obj)
{
    lv_form_section_t * section = (lv_form_section_t *)obj;

    /* Update spacing */
    lv_obj_set_style_pad_gap(obj, section->field_spacing, 0);
}

static lv_form_validation_t validate_field(lv_obj_t * field)
{
    lv_form_field_t * fld = (lv_form_field_t *)field;

    if(fld->required) {
        const char * value = lv_form_field_get_value(field);
        if(value == NULL || value[0] == '\0') {
            fld->validation = LV_FORM_VALIDATION_REQUIRED;
            return LV_FORM_VALIDATION_REQUIRED;
        }
    }

    /* Add more validation logic here if needed */
    /* For example: email format, number format, etc. */

    fld->validation = LV_FORM_VALIDATION_OK;
    return LV_FORM_VALIDATION_OK;
}

static void form_submit_event_cb(lv_event_t * e)
{
    lv_obj_t * form = lv_event_get_current_target(e);
    bool is_valid = lv_form_validate(form);

    lv_form_event_data_t event_data = {
        .form = form,
        .field = NULL,
        .value = NULL,
        .is_valid = is_valid
    };

    /* Send submit event */
    lv_event_send(form, LV_EVENT_SUBMIT, &event_data);
}

#endif /*LV_USE_FORM*/
