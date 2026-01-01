/**
 * @file lv_form_private.h
 *
 */

#ifndef LV_FORM_PRIVATE_H
#define LV_FORM_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_form.h"
#include "../../core/lv_obj_private.h"

#if LV_USE_FORM != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Data of form object
 */
struct _lv_form_t {
    lv_obj_t obj;                    /* Base object */
    lv_form_layout_t layout;         /* Form layout mode */
    int32_t spacing;                 /* Spacing between elements */
    uint8_t flags;                   /* Form flags */
};

/**
 * Data of form section object
 */
struct _lv_form_section_t {
    lv_obj_t obj;                    /* Base object */
    char * title;                    /* Section title */
    bool static_title;               /* True if title is static (not allocated) */
    int32_t field_spacing;           /* Spacing between fields */
};

/**
 * Data of form field object
 */
struct _lv_form_field_t {
    lv_obj_t obj;                    /* Base object */
    lv_obj_t * label;                /* Label widget */
    lv_obj_t * input;                /* Input widget */
    char * value;                    /* Field value */
    bool required;                   /* Required flag */
    lv_form_validation_t validation; /* Validation status */
};

/**
 * Event data for form events
 */
typedef struct {
    lv_obj_t * form;                 /* Form object */
    lv_obj_t * field;                /* Field that triggered the event */
    const char * value;              /* Current value */
    bool is_valid;                   /* Validation result */
} lv_form_event_data_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_FORM != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FORM_PRIVATE_H*/
