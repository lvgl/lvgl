/**
 * @file lv_ex_styles.h
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
#include "lv_ex_styles.h"

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
void lv_ex_styles(void)
{

    /****************************************
     * BASE OBJECT + LABEL WITH DEFAULT STYLE
     ****************************************/

    lv_obj_t * obj1;
    obj1 = lv_obj_create(lv_scr_act(), NULL);           /*Create a simple objects*/
    lv_obj_set_pos(obj1, 10, 10);
    lv_obj_t * label = lv_label_create(obj1, NULL);

    /*Add a label to the object*/
    lv_label_set_text(label, "Default");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    /****************************************
     * BASE OBJECT WITH PRETTY COLOR STYLE
     ****************************************/

    lv_obj_t * obj2;
    obj2 = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_align(obj2, obj1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);            /*Align next to the previous object*/
    lv_obj_set_style(obj2, lv_style_get(LV_STYLE_PRETTY_COLOR, NULL));  /*Set built in style*/
    label = lv_label_create(obj2, NULL);

    /* Add a label to the object.
     * Labels by default inherit the parent's style */
    lv_label_set_text(label, "Pretty\ncolor");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    /*****************************
     * BASE OBJECT WITH NEW STYLE
     *****************************/

    /* Create a new style */
    static lv_style_t style_new;                       /*Styles can't be local variables*/
    lv_style_get(LV_STYLE_PRETTY_COLOR, &style_new);   /*Copy a built-in style as a starting point*/
    style_new.radius = LV_RADIUS_CIRCLE;               /*Fully round corners*/
    style_new.swidth = 8;                              /*8 px shadow*/
    style_new.bwidth = 2;                              /*2 px border width*/
    style_new.mcolor = COLOR_WHITE;                    /*White main color*/
    style_new.gcolor = color_mix(COLOR_BLUE, COLOR_WHITE, OPA_40);     /*light blue gradient color*/
    style_new.scolor = COLOR_MAKE(0xa0, 0xa0, 0xa0);    /*Light gray shadow color*/
    style_new.ccolor = color_mix(COLOR_BLUE, COLOR_WHITE, OPA_90);  /*Blue content color (text color)*/
    style_new.letter_space = 10;                       /*10 px letter space*/
    style_new.txt_align = LV_TXT_ALIGN_MID;            /*Middel text align*/

    /*Create a base object and apply the new style*/
    lv_obj_t * obj3;
    obj3 = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_align(obj3, obj2, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style(obj3, &style_new);

    /* Add a label to the object.
     * Labels by default inherit the parent's style */
    label = lv_label_create(obj3, NULL);
    lv_label_set_text(label, "New\nstyle");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);


    /************************
     * CREATE A STYLED LED
     ***********************/

    /*Create a style for the LED*/
    static lv_style_t style_led;
    lv_style_get(LV_STYLE_PRETTY_COLOR, &style_led);
    style_led.swidth = 15;
    style_led.radius = LV_RADIUS_CIRCLE;
    style_led.bwidth = 3;
    style_led.bopa = OPA_30;
    style_led.mcolor = COLOR_MAKE(0xb5, 0x0f, 0x04);
    style_led.gcolor = COLOR_MAKE(0x50, 0x07, 0x02);
    style_led.bcolor = COLOR_MAKE(0xfa, 0x0f, 0x00);
    style_led.scolor = COLOR_MAKE(0xb5, 0x0f, 0x04);

    /*Create a LED and switch it ON*/
    lv_obj_t * led1  = lv_led_create(lv_scr_act(), NULL);
    lv_obj_set_style(led1, &style_led);
    lv_obj_align_us(led1, obj1, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
    lv_led_on(led1);

    /*Copy the previous LED and set a brightness*/
    lv_obj_t * led2  = lv_led_create(lv_scr_act(), led1);
    lv_obj_align_us(led2, obj2, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
    lv_led_set_bright(led2, 190);

    /*Copy the previous LED and switch it OFF*/
    lv_obj_t * led3  = lv_led_create(lv_scr_act(), led1);
    lv_obj_align_us(led3, obj3, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
    lv_led_off(led3);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_EXAMPLE != 0*/
