/**
 * @file lv_hello_world.c
 *
 */

/*
 * The basic building blocks (components or widgets) in LittlevGL are the graphical objects.
 * For example:
 *  - Buttons
 *  - Labels
 *  - Charts
 *  - Sliders etc
 *
 * In this part you can learn the basics of the objects like creating, positioning, sizing etc.
 * You will also meet some different object types and their attributes.
 *
 * Regardless to the object type the 'lv_obj_t' variable type is used stores the objects
 * and you can refer to an object with an lv_obj_t pointer (lv_obj_t *)
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
 * Every object has exactly one parent object (except screens).
 * A parent can have unlimited number of children.
 * There is no limitation for the type of the parent.
 *
 * The children are visible only on their parent. The parts outside will be cropped (not displayed)
 *
 * If the parent is moved the children will be moved with it.
 *
 * The earlier created object (and its children) will drawn earlier.
 * Using this layers can be built.
 *
 * LEARN MORE
 * -------------
 * - General overview: http://www.gl.littlev.hu/objects
 * - Detailed description of types: http://www.gl.littlev.hu/object-types
 *
 * NOTES
 * -------------
 * - Be sure 'LV_OBJ_FREE_P' is enabled in 'lv_conf.h'
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_ex_objects.h"
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
static  lv_action_res_t btn_rel_action(lv_obj_t * btn, LV_INDEV_t * indev_proc);
static  lv_action_res_t ddlist_action(lv_obj_t * ddlist, LV_INDEV_t * indev_proc);

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
 * Initialize the Object usage example
 */
void lv_ex_objects(void)
{

    /********************
     * CREATE A SCREEN
     *******************/

    /* Create a new screen and load it
     * Screen can be created from any type object
     * Now a Page is used which is an objects with scrollable content*/
    lv_obj_t * scr = lv_page_create(NULL, NULL);
    lv_scr_load(scr);


    /****************
     * ADD A TITLE
     ****************/
    lv_obj_t * label = lv_label_create(scr, NULL);  /*First parameters (scr) is the parent*/
    lv_label_set_text(label, "Object usage demo");  /*Set the text*/
    lv_obj_set_x(label, 50);       /*Labels are inherited from Basic object so 'lv_obj_...' functions can be used*/


    /********************
     * CREATE TWO BUTTONS
     ********************/

    /*Create a button*/
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);    /*Create a button on the currently loaded screen*/
    lv_btn_set_rel_action(btn1, btn_rel_action);            /*Set function to call when the button is released*/
    lv_obj_align(btn1, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20); /*Align below the label*/
    label = lv_label_create(btn1, NULL);                    /*Create a label on the button (the 'label' variable can be reused)*/
    lv_label_set_text(label, "Button 1");

    /*Copy the previous button*/
    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), btn1);    /*Second parameter is an object to copy*/
    lv_obj_align(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, 50, 0);/*Align next to the prev. button.*/
    label = lv_label_create(btn2, NULL);                     /*Create a label on the button*/
    lv_label_set_text(label, "Button 2");


    /****************
     * ADD A SLIDER
     ****************/

    /*Add a slider (inheritance: lv_obj -> lv_bar -> lv_slider)*/
    lv_obj_t * slider = lv_slider_create(scr, NULL);                            /*Create a slider*/
    lv_obj_set_size(slider, lv_obj_get_width(lv_scr_act())  / 3, LV_DPI / 3);   /*Set the size*/
    lv_obj_align(slider, btn1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);                /*Align below the first button*/
    lv_bar_set_value(slider, 30);                                               /*Slider is a 'bar' so set its value like a 'bar'*/


    /***********************
     * ADD A DROP DOWN LIST
     ************************/

    lv_obj_t * ddlist = lv_ddlist_create(lv_scr_act(), NULL);
    lv_obj_align(ddlist, slider, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);        /*Align next to the slider*/
    lv_obj_set_free_p(ddlist, slider);                                  /*Save the pointer of the slider in the ddlist (used in 'ddlist_action()')*/
    lv_ddlist_set_options_str(ddlist, "None\nLittle\nHalf\nA lot\nAll");    /*Set the options*/
    lv_ddlist_set_action(ddlist, ddlist_action);                        /*Set function to call on new option choose*/
    lv_obj_set_top(ddlist, true);                                       /*Enable the drop down list always be on the top*/


    /****************
     * CREATE A CHART
     ****************/
    lv_obj_t * chart = lv_chart_create(lv_scr_act(), NULL);         /*Craete the chart*/
    lv_obj_set_size(chart, lv_obj_get_width(scr) / 2, lv_obj_get_width(scr) / 4);   /*Set the size*/
    lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);   /*Align below the slider*/
    lv_chart_set_dl_width(chart, 3 * LV_DOWNSCALE);                 /*Set the line width (LV_DOWNSCALE compensates anti-aliasing if enabled)*/

    /*Add a RED data line and set some points*/
    lv_chart_dl_t * dl1 = lv_chart_add_data_line(chart, COLOR_RED);
    lv_chart_set_next(chart, dl1, 10);
    lv_chart_set_next(chart, dl1, 25);
    lv_chart_set_next(chart, dl1, 45);
    lv_chart_set_next(chart, dl1, 80);

    /*Add a BLUE data line and set some points*/
    lv_chart_dl_t * dl2 = lv_chart_add_data_line(chart, COLOR_MAKE(0x40, 0x70, 0xC0));
    lv_chart_set_next(chart, dl2, 10);
    lv_chart_set_next(chart, dl2, 25);
    lv_chart_set_next(chart, dl2, 45);
    lv_chart_set_next(chart, dl2, 80);
    lv_chart_set_next(chart, dl2, 75);
    lv_chart_set_next(chart, dl2, 505);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called when a button is released
 * @param btn pointer to the released button
 * @param indev_proc pointer to caller display input (e.g. touchpad)
 * @return LV_ACTION_RES_OK because the object is not deleted in this function
 */
static  lv_action_res_t btn_rel_action(lv_obj_t * btn, LV_INDEV_t * indev_proc)
{
    /*Increase the button width*/
    cord_t width = lv_obj_get_width(btn);
    lv_obj_set_width(btn, width + 20);

    return LV_ACTION_RES_OK;
}

/**
 * Called when a new option is chosen in the drop down list
 * @param ddlist pointer to the drop down list
 * @param indev_proc pointer to caller display input (e.g. touchpad)
 * @return LV_ACTION_RES_OK because the object is not deleted in this function
 */
static  lv_action_res_t ddlist_action(lv_obj_t * ddlist, LV_INDEV_t * indev_proc)
{
    uint16_t opt = lv_ddlist_get_selected(ddlist);      /*Get the id of selected option*/

    lv_obj_t * slider = lv_obj_get_free_p(ddlist);      /*Get the saved slider*/
    lv_bar_set_value(slider, (opt * 100) / 4);          /*Modify the slider value according to the selection*/

    return LV_ACTION_RES_OK;
}

#endif /*USE_LV_EXAMPLE != 0*/
