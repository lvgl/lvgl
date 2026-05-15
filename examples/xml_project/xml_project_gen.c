/**
 * @file xml_project_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "xml_project_gen.h"

#if LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/*----------------
 * Translations
 *----------------*/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/

/*----------------
 * Images
 *----------------*/

const void * img_cogwheel;

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

lv_subject_t subject_value;
lv_subject_t subject_value2;
lv_subject_t subject_opa;
lv_subject_t subject_index;
lv_subject_t subject_flag;
lv_subject_t subject_text;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void xml_project_init_gen(const char * asset_path)
{
    char buf[256];


    /*----------------
     * Fonts
     *----------------*/


    /*----------------
     * Images
     *----------------*/
    lv_snprintf(buf, 256, "%s%s", asset_path, "../assets/img_cogwheel_argb.png");
    img_cogwheel = lv_strdup(buf);

    /*----------------
     * Global styles
     *----------------*/

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&subject_value, 50);
    lv_subject_set_min_value_int(&subject_value, 0);
    lv_subject_set_max_value_int(&subject_value, 100);
    lv_subject_init_int(&subject_value2, 20);
    lv_subject_set_min_value_int(&subject_value2, 0);
    lv_subject_set_max_value_int(&subject_value2, 100);
    lv_subject_init_int(&subject_opa, 128);
    lv_subject_set_min_value_int(&subject_opa, 0);
    lv_subject_set_max_value_int(&subject_opa, 255);
    lv_subject_init_int(&subject_index, 0);
    lv_subject_init_int(&subject_flag, 0);
    static char subject_text_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_text_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_text,
                           subject_text_buf,
                           subject_text_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Hello"
                          );

    /*----------------
     * Translations
     *----------------*/

#if LV_USE_XML
    /* Register widgets */

    /* Register fonts */

    /* Register subjects */
    lv_xml_register_subject(NULL, "subject_value", &subject_value);
    lv_xml_register_subject(NULL, "subject_value2", &subject_value2);
    lv_xml_register_subject(NULL, "subject_opa", &subject_opa);
    lv_xml_register_subject(NULL, "subject_index", &subject_index);
    lv_xml_register_subject(NULL, "subject_flag", &subject_flag);
    lv_xml_register_subject(NULL, "subject_text", &subject_text);

    /* Register callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "img_cogwheel", img_cogwheel);
#endif

#if LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
#endif
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/