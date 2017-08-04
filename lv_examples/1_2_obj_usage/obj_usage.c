/**
 * @file lv_hello_world.c
 *
 */

/*
 * The basic building blocks in LittlevGL are the graphical objects.
 * For example:
 *  - Buttons
 *  - Labels
 *  - Charts
 *  - Sliders etc
 *
 * Regardless to the object type the 'lv_obj_t' is used stores objects
 * and you can refer to an object with an lv_obj_t pointer (lv_obj_t *)
 *
 *
 * INHERITANCE
 * -------------
 * Similarly to object oriented languages some kind of inheritance is used
 * among the object types.
 *
 * Every object is derived from the 'Basic object'. (lv_obj)
 *
 * The types are backward compatible which means a type can use all the ancestor
 * attributes/functions too.
 *
 * For example a 'Button' is derived from 'Container'  which is derived from 'Basic objects'.
 * Therefore a button can use container attributes like automatically fit size to the content.
 *
 * PARENT-CHILD
 * -------------
 * A parent can be considered as the container of its children.
 * Every object has exactly one parent object (except screens) but
 * a parent can have unlimited number of children.
 * There is no limitation for the type of the parent.
 *
 * The children are visible only on their parent. The parts outside will be cropped (not displayed)
 *
 * If the parent is moved the children move with it.
 *
 * The earlier created object (and its children) will drawn earlier.
 * Using this layers can be built.
 *
 * LEARN MORE
 * -------------
 * - General overview: http://www.gl.littlev.hu/objects
 * - Detailed description of types: http://www.gl.littlev.hu/object-types
 */

/*********************
 *      INCLUDES
 *********************/
#include "obj_usage.h"
#if USE_LV_EXAMPLE != 0

#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static  lv_action_res_t btn_rel_action(lv_obj_t * btn, lv_dispi_t * dispi);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 *
 */
void lv_obj_usage_init(void)
{

    /* Create a new screen and load it
     * Screen can be created from any type object
     * Now a Page is used which is an objects with scrollable content*/
    lv_obj_t * scr = lv_page_create(NULL, NULL);
    lv_scr_load(scr);

    /*Add a title*/
    lv_obj_t * label = lv_label_create(scr, NULL);  /*First parameters (scr) is the parent*/
    lv_label_set_text(label, "Object usage demo");  /*Set the text*/
    lv_obj_set_x(label, 50);       /*Labels are inherited from Basic object so 'lv_obj_...' functions can be used*/

    /*Create a button*/
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);    /*Create a button the currently loaded screen*/
    lv_btn_set_rel_action(btn1, btn_rel_action);            /*Set function to call when the button is released*/
    lv_obj_align(btn1, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20); /*Align below the label*/
    label = lv_label_create(btn1, NULL);            /*Create a label on the button (the 'label' variable can be reused)*/
    lv_label_set_text(label, "Button 1");

    /*Copy the previous button*/
    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), btn1);    /*Second parameter is an object to copy*/
    lv_obj_align(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, 50, 0);/*Align next to the prev. button.*/
    label = lv_label_create(btn2, NULL);                     /*Create a label on the button*/
    lv_label_set_text(label, "Button 2");

    /*Add a slider (inheritance: lv_obj -> lv_bar -> lv_slider)*/
    lv_obj_t * slider = lv_slider_create(scr, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static  lv_action_res_t btn_rel_action(lv_obj_t * btn, lv_dispi_t * dispi)
{
    cord_t width = lv_obj_get_width(btn);
    lv_obj_set_width(btn, width + 20);

    return LV_ACTION_RES_OK;
}

#endif /*USE_LV_EXAMPLE != 0*/
