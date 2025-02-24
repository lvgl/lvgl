/**
 * @file lv_xml_widget.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_widget.h"
#include "lv_xml_parser.h"
#include "../../stdlib/lv_string.h"
#include "../../stdlib/lv_mem.h"

#if LV_USE_XML

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
static lv_widget_processor_t * widget_processor_head;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_xml_widget_register(const char * name, lv_xml_widget_create_cb_t create_cb,
                                   lv_xml_widget_apply_cb_t apply_cb)
{
    lv_widget_processor_t * p = lv_malloc(sizeof(lv_widget_processor_t));
    lv_memzero(p, sizeof(lv_widget_processor_t));

    p->name = lv_strdup(name);
    p->create_cb = create_cb;
    p->apply_cb = apply_cb;

    if(widget_processor_head == NULL) widget_processor_head = p;
    else {
        p->next = widget_processor_head;
        widget_processor_head = p;
    }
    return LV_RESULT_OK;
}

lv_widget_processor_t * lv_xml_widget_get_processor(const char * name)
{
    /* Select the widget specific parser type based on the name */
    lv_widget_processor_t * p = widget_processor_head;
    while(p) {
        if(lv_streq(p->name, name)) {
            return p;
        }

        p = p->next;
    }
    return NULL;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
