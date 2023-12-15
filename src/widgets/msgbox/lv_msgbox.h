/**
 * @file lv_mbox.h
 *
 */

#ifndef LV_MSGBOX_H
#define LV_MSGBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_MSGBOX

/*Testing of dependencies*/
#if LV_USE_BUTTONMATRIX == 0
#error "lv_mbox: lv_btnm is required. Enable it in lv_conf.h (LV_USE_BUTTONMATRIX  1) "
#endif

#if LV_USE_LABEL == 0
#error "lv_mbox: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
    lv_obj_t * header;
    lv_obj_t * content;
    lv_obj_t * footer;
    lv_obj_t * title;
} lv_msgbox_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_header_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_content_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_footer_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_header_button_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_footer_button_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_msgbox_backdrop_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an empty message box
 * @param parent        the parent of the message box
 * @return              the created message box
 */
lv_obj_t * lv_msgbox_create(lv_obj_t * parent);

/**
 * Add title to the message box. It also creates a header for the title.
 * @param obj           pointer to a message box
 * @param title         the text of the tile
 * @return              the created title label
 */
lv_obj_t * lv_msgbox_add_title(lv_obj_t * obj, const char * title);

/**
 * Add a button to the header of to the message box. It also creates a header.
 * @param obj           pointer to a message box
 * @param icon          the icon of the button
 * @return              the created button
 */
lv_obj_t * lv_msgbox_add_header_button(lv_obj_t * obj, const void * icon);

/**
 * Add a text to the content area of message box. Multiply texts will be created below each other.
 * @param obj           pointer to a message box
 * @param icon          the icon of the button
 * @return              the created button
 */
lv_obj_t * lv_msgbox_add_text(lv_obj_t * obj, const char * text);

/**
 * Add a button to the footer of to the message box. It also creates a footer.
 * @param obj           pointer to a message box
 * @param text          the text of the button
 * @return              the created button
 */
lv_obj_t * lv_msgbox_add_footer_button(lv_obj_t * obj, const char * text);

/**
 * Add a close button to the message box. It also create a header.
 * @param obj           pointer to a message box
 * @return              the created close button
 */
lv_obj_t * lv_msgbox_add_close_button(lv_obj_t * obj);

/**
 * Get the header widget
 * @param obj           pointer to a message box
 * @return              the header, or NULL if not exists
 */
lv_obj_t * lv_msgbox_get_header(lv_obj_t * obj);

/**
 * Get the footer widget
 * @param obj           pointer to a message box
 * @return              the footer, or NULL if not exists
 */
lv_obj_t * lv_msgbox_get_footer(lv_obj_t * obj);

/**
 * Get the content widget
 * @param obj           pointer to a message box
 * @return              the content, or NULL if not exists
 */
lv_obj_t * lv_msgbox_get_content(lv_obj_t * obj);

/**
 * Get the title label
 * @param obj           pointer to a message box
 * @return              the title, or NULL if not exists
 */
lv_obj_t * lv_msgbox_get_title(lv_obj_t * obj);

/**
 * Close a message box
 * @param obj           pointer to a message box
 */
void lv_msgbox_close(lv_obj_t * mbox);

/**
 * Close a message box in the next call of the message box
 * @param obj           pointer to a message box
 */
void lv_msgbox_close_async(lv_obj_t * mbox);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MSGBOX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MSGBOX_H*/
