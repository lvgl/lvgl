/**
 * @file lv_obj_property.h
 *
 */

#ifndef LV_OBJ_PROPERTY_H
#define LV_OBJ_PROPERTY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../misc/lv_style.h"

#if LV_USE_OBJ_PROPERTY

/*********************
 *      DEFINES
 *********************/

/*All possible property value types*/
#define LV_PROPERTY_TYPE_INVALID        0   /*Use default 0 as invalid to detect program outliers*/
#define LV_PROPERTY_TYPE_INT            1   /*int32_t type*/
#define LV_PROPERTY_TYPE_PRECISE        2   /*lv_value_precise_t, int32_t or float depending on LV_USE_FLOAT*/
#define LV_PROPERTY_TYPE_COLOR          3   /*ARGB8888 type*/
#define LV_PROPERTY_TYPE_POINTER        4   /*void * pointer*/
#define LV_PROPERTY_TYPE_IMGSRC         5   /*Special pointer for image*/

/**********************
 *      TYPEDEFS
 **********************/

#define LV_PROPERTY_ID(clz, name, type, index)    LV_PROPERTY_## clz ##_##name = (LV_PROPERTY_## clz ##_START + (index)) | ((type) << 28)

#define LV_PROPERTY_ID_TYPE(id) ((id) >> 28)
#define LV_PROPERTY_ID_INDEX(id) ((id) & 0xfffffff)

/*Set properties from an array of lv_property_t*/
#define LV_OBJ_SET_PROPERTY_ARRAY(obj, array) lv_obj_set_properties(obj, array, sizeof(array)/sizeof(array[0]))

/**
 * Group of predefined widget ID start value.
 */
enum {
    LV_PROPERTY_ID_INVALID = 0,

    /*ID 0 to 0xff are style ID, check lv_style_prop_t*/
    LV_PROPERTY_ID_START = 0x100, /*ID little than 0xff is style ID*/

    /* lv_obj.c */
    LV_PROPERTY_OBJ_START = 1000,

    /* lv_image.c */
    LV_PROPERTY_IMAGE_START = 1100,

    /*Special ID*/
    LV_PROPERTY_ID_BUILTIN_LAST, /*Use it to extend ID and make sure it's unique and compile time determinant*/

    LV_PROPERTY_ID_ANY = 0x7ffffffe, /*Special ID used by lvgl to intercept all setter/getter call.*/
};

typedef uint32_t lv_prop_id_t;

typedef struct {
    lv_prop_id_t id;
    union {
        int32_t num;                /**< Number integer number (opacity, enums, booleans or "normal" numbers)*/
        const void * ptr;           /**< Constant pointers  (font, cone text, etc)*/
        lv_color_t color;           /**< Colors*/
        lv_value_precise_t precise; /**< float or int for precise value*/
        lv_style_value_t _style;    /**< A place holder for style value which is same as property value.*/
    };
} lv_property_t;

typedef struct {
    lv_prop_id_t id;

    void * setter;
    void * getter;
} lv_property_ops_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set widget property value.
 * @param obj       pointer to an object
 * @param id        ID of which property
 * @param value     The property value to set
 * @return          return LV_RESULT_OK if success
 */
lv_result_t lv_obj_set_property(lv_obj_t * obj, const lv_property_t * value);

lv_result_t lv_obj_set_properties(lv_obj_t * obj, const lv_property_t * value, uint32_t count);

/*=====================
 * Getter functions
 *====================*/

/**
 * Read property value from object
 * @param obj       pointer to an object
 * @param id        ID of which property
 * @param value     pointer to a buffer to store the value
 * @return ? to be discussed, LV_RESULT_OK or LV_RESULT_INVALID
 */
lv_property_t lv_obj_get_property(lv_obj_t * obj, lv_prop_id_t id);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OBJ_PROPERTY*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_PROPERTY_H*/
