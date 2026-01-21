/**
 * @file lv_form.h
 *
 */

#ifndef LV_FORM_H
#define LV_FORM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_FORM != 0

#include "../../core/lv_obj.h"
#include "../../core/lv_obj_property.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Form layout modes
 */
typedef enum {
    LV_FORM_LAYOUT_VERTICAL,   /**< Elements are arranged vertically */
    LV_FORM_LAYOUT_HORIZONTAL, /**< Elements are arranged horizontally */
    LV_FORM_LAYOUT_GRID,       /**< Elements are arranged in a grid */
} lv_form_layout_t;

/**
 * Form field validation status
 */
typedef enum {
    LV_FORM_VALIDATION_OK,        /**< Field is valid */
    LV_FORM_VALIDATION_REQUIRED,  /**< Required field is empty */
    LV_FORM_VALIDATION_INVALID,   /**< Field has invalid format */
} lv_form_validation_t;

/* Forward declarations */
struct _lv_form_t;
struct _lv_form_section_t;
struct _lv_form_field_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_form_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_form_section_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_form_field_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_form_id_t {
    LV_PROPERTY_ID(FORM, LAYOUT, LV_PROPERTY_TYPE_INT, 0),
    LV_PROPERTY_ID(FORM, SPACING, LV_PROPERTY_TYPE_INT, 1),
    LV_PROPERTY_FORM_END,
};

enum _lv_property_form_section_id_t {
    LV_PROPERTY_ID(FORM_SECTION, TITLE, LV_PROPERTY_TYPE_STRING, 0),
    LV_PROPERTY_FORM_SECTION_END,
};

enum _lv_property_form_field_id_t {
    LV_PROPERTY_ID(FORM_FIELD, REQUIRED, LV_PROPERTY_TYPE_INT, 0),
    LV_PROPERTY_ID(FORM_FIELD, VALUE, LV_PROPERTY_TYPE_STRING, 1),
    LV_PROPERTY_FORM_FIELD_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*=======================
 * Form Main Object API
 *=======================*/

/**
 * Create a form object
 * @param parent    pointer to an object, it will be the parent of the new form
 * @return          pointer to the created form
 */
lv_obj_t * lv_form_create(lv_obj_t * parent);

/**
 * Set the layout of the form
 * @param obj       pointer to a form object
 * @param layout    the new layout mode
 */
void lv_form_set_layout(lv_obj_t * obj, lv_form_layout_t layout);

/**
 * Get the layout of the form
 * @param obj       pointer to a form object
 * @return          the current layout mode
 */
lv_form_layout_t lv_form_get_layout(const lv_obj_t * obj);

/**
 * Set the spacing between elements in the form
 * @param obj       pointer to a form object
 * @param spacing   the spacing value in pixels
 */
void lv_form_set_spacing(lv_obj_t * obj, int32_t spacing);

/**
 * Get the spacing between elements in the form
 * @param obj       pointer to a form object
 * @return          the spacing value in pixels
 */
int32_t lv_form_get_spacing(const lv_obj_t * obj);

/**
 * Clear all sections and fields from the form
 * @param obj       pointer to a form object
 */
void lv_form_clean(lv_obj_t * obj);

/*=======================
 * Form Section API
 *=======================*/

/**
 * Add a section to the form
 * @param form      pointer to a form object
 * @param title     pointer to the section title (NULL for no title)
 * @return          pointer to the created section
 */
lv_obj_t * lv_form_add_section(lv_obj_t * form, const char * title);

/**
 * Set the title of a section
 * @param section   pointer to a section object
 * @param title     pointer to the new title (NULL to remove title)
 */
void lv_form_section_set_title(lv_obj_t * section, const char * title);

/**
 * Get the title of a section
 * @param section   pointer to a section object
 * @return          pointer to the title string
 */
const char * lv_form_section_get_title(const lv_obj_t * section);

/**
 * Set the spacing between fields in a section
 * @param section   pointer to a section object
 * @param spacing   the spacing value in pixels
 */
void lv_form_section_set_field_spacing(lv_obj_t * section, int32_t spacing);

/**
 * Get the spacing between fields in a section
 * @param section   pointer to a section object
 * @return          the spacing value in pixels
 */
int32_t lv_form_section_get_field_spacing(const lv_obj_t * section);

/*=======================
 * Form Field API
 *=======================*/

/**
 * Add a field to a section with a label and input widget
 * @param section   pointer to a section object
 * @param label     pointer to the field label text
 * @param input     pointer to the input widget (e.g., textarea, dropdown, etc.)
 * @return          pointer to the created field object
 */
lv_obj_t * lv_form_add_field(lv_obj_t * section, const char * label, lv_obj_t * input);

/**
 * Set whether a field is required
 * @param field     pointer to a field object
 * @param required  true to mark as required
 */
void lv_form_field_set_required(lv_obj_t * field, bool required);

/**
 * Check if a field is required
 * @param field     pointer to a field object
 * @return          true if required
 */
bool lv_form_field_is_required(const lv_obj_t * field);

/**
 * Get the validation status of a field
 * @param field     pointer to a field object
 * @return          validation status
 */
lv_form_validation_t lv_form_field_get_validation(const lv_obj_t * field);

/**
 * Set the value of a field
 * @param field     pointer to a field object
 * @param value     pointer to the value string
 */
void lv_form_field_set_value(lv_obj_t * field, const char * value);

/**
 * Get the value of a field
 * @param field     pointer to a field object
 * @return          pointer to the value string
 */
const char * lv_form_field_get_value(const lv_obj_t * field);

/**
 * Get the label widget of a field
 * @param field     pointer to a field object
 * @return          pointer to the label widget
 */
lv_obj_t * lv_form_field_get_label(const lv_obj_t * field);

/**
 * Get the input widget of a field
 * @param field     pointer to a field object
 * @return          pointer to the input widget
 */
lv_obj_t * lv_form_field_get_input(const lv_obj_t * field);

/*=======================
 * Data Management API
 *=======================*/

/**
 * Get all form data as a simple string representation
 * @param form      pointer to a form object
 * @param buffer    buffer to store the result
 * @param size      size of the buffer
 * @return          LV_RESULT_OK on success
 */
lv_result_t lv_form_get_data(lv_obj_t * form, char * buffer, size_t size);

/**
 * Validate all fields in the form
 * @param form      pointer to a form object
 * @return          true if all fields are valid
 */
bool lv_form_validate(lv_obj_t * form);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FORM*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FORM_H*/
