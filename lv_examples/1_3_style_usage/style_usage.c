/**
 * @file style_usage.c
 *
 */

/*
 * You can modify the appearance of the graphical objects with styles.
 * A style is simple 'lv_style_t' variable.
 * Objects save the address of this variable so it has to be 'static or 'global'.
 *
 * A style contains various attributes to describe rectangle, image or text like
 * objects at same time. To know which attribute is used by an object see:
 * http://www.gl.littlev.hu/object-types
 *
 * To set a new style for an object use: 'lv_obj_set_style(obj, &style);
 * If NULL is set as style then the object will inherit the parents style.
 * For example is you create a style for button the label appearance can be defined there as well.
 *
 * You can use built-in styles. 'lv_style_get(LV_STYLE_... , &copy)' will give you a pointer to built in style
 * and copy it to variable (second parameter) if it is not NULL.
 * By default the objects use the built-in styles.
 * The built-in styles can be modified in run time to give a new default skin to your GUI.
 *
 * Learn more here: http://www.gl.littlev.hu/objects#style
 * */

/*********************
 *      INCLUDES
 *********************/
#include "style_usage.h"

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
 * Create a simple 'Hello world!' label
 */
void style_usage_init(void)
{

    /************************************
     * BUTTON + LABEL WITH DEFAULT STYLE
     ************************************/

    lv_obj_t * btn1;
    btn1 = lv_btn_create(lv_scr_act(), NULL);           /*Create a simple button*/
    lv_obj_set_pos(btn1, 10, 10);
    lv_obj_t * label = lv_label_create(btn1, NULL);     /*Add a lebel tothe button*/
    lv_label_set_text(label, "Default");

    /************************
     * BUTTON WITH NEW STYLE
     ************************/

    /* Create a new style
     * Don't forget a style can describe any object type
     * like buttons and labels */
    static lv_style_t style_btn2;                       /*Styles can't be local variables*/
    lv_style_get(LV_STYLE_PRETTY_COLOR, &style_btn2);   /*Copy a built-in style as a starting point*/
    style_btn2.swidth = 10;                             /*10 px shadow*/
    style_btn2.bwidth = 5;                              /*5 px border width*/
    style_btn2.mcolor = COLOR_ORANGE;                   /*Orange main color*/
    style_btn2.gcolor = COLOR_RED;                      /*Red gradient color*/
    style_btn2.letter_space = 10;                       /*10 px letter space*/
    style_btn2.txt_align = LV_TXT_ALIGN_MID;            /*Text align: middle*/

    /*Create a button and apply the new style*/
    lv_obj_t * btn2;
    btn2 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_align(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style(btn2, &style_btn2);

    /* Add a label to the button.
     * Label by default inherits the parent's style */
    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "New\nstyle");

    /************************
     * LABEL WITH NEW STYLE
     ************************/

    /*Create new style for the label*/
    static lv_style_t style_label;
    lv_style_get(LV_STYLE_PRETTY_COLOR, &style_label);              /*Use a built-in style*/
    style_label.ccolor = color_mix(COLOR_BLUE, COLOR_WHITE, OPA_70);/*Light blue content color (text color) */
    style_label.letter_space = 4;                                   /*4 px letter space*/
    style_label.txt_align = LV_TXT_ALIGN_MID;                       /*Text align: middle*/

    /*Copy 'btn2'. It will use the same style as 'btn2'*/
    lv_obj_t * btn3;
    btn3 = lv_btn_create(lv_scr_act(), btn2);
    lv_obj_align(btn3, btn2, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    /*Create a label and apply the new style */
    label = lv_label_create(btn3, NULL);
    lv_label_set_text(label, "Label\nstyle");
    lv_obj_set_style(label, &style_label);


    /************************
     * CREATE A STYLED LED
     ***********************/

    /*Create a style for the LED*/
    static lv_style_t style_led;
    lv_style_get(LV_STYLE_PRETTY_COLOR, &style_led);
    style_led.swidth = 15;
    style_led.radius = LV_RADIUS_CIRCLE;
    style_led.bwidth = 5;
    style_led.bopa = OPA_30;
    style_led.mcolor = COLOR_MAKE(0xb5, 0x0f, 0x04);
    style_led.gcolor = COLOR_MAKE(0x50, 0x07, 0x02);
    style_led.bcolor = COLOR_MAKE(0xfa, 0x0f, 0x00);
    style_led.scolor = COLOR_MAKE(0xb5, 0x0f, 0x04);

    /*Create a LED and switch it ON*/
    lv_obj_t * led1  = lv_led_create(lv_scr_act(), NULL);
    lv_obj_set_style(led1, &style_led);
    lv_obj_align_us(led1, btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
    lv_led_on(led1);

    /*Copy the previous LED and set a brightness*/
    lv_obj_t * led2  = lv_led_create(lv_scr_act(), led1);
    lv_obj_align_us(led2, btn2, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
    lv_led_set_bright(led2, 190);

    /*Copy the previous LED and switch it OFF*/
    lv_obj_t * led3  = lv_led_create(lv_scr_act(), led1);
    lv_obj_align_us(led3, btn3, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
    lv_led_off(led3);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_EXAMPLE != 0*/
