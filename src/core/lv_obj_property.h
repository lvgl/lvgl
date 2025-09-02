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
#define LV_PROPERTY_TYPE_POINT          4   /*lv_point_t */
#define LV_PROPERTY_TYPE_POINTER        5   /*void * pointer*/
#define LV_PROPERTY_TYPE_IMGSRC         6   /*Special pointer for image*/
#define LV_PROPERTY_TYPE_TEXT           7   /*Special pointer of char* */
#define LV_PROPERTY_TYPE_OBJ            8   /*Special pointer of lv_obj_t* */
#define LV_PROPERTY_TYPE_DISPLAY        9   /*Special pointer of lv_display_t* */
#define LV_PROPERTY_TYPE_FONT           10  /*Special pointer of lv_font_t* */
#define LV_PROPERTY_TYPE_BOOL           11  /*int32_t type*/

#define LV_PROPERTY_TYPE_SHIFT          28
#define LV_PROPERTY_TYPE2_SHIFT         24

/* Example:
 * LV_PROPERTY_ID(OBJ, FLAG_CLICKABLE, LV_PROPERTY_TYPE_INT, 1),
 * produces
 * LV_PROPERTY_OBJ_FLAG_CLICKABLE = (LV_PROPERTY_OBJ_START + (1)) | ((LV_PROPERTY_TYPE_INT) << LV_PROPERTY_TYPE_SHIFT)
 */
#define LV_PROPERTY_ID(clz, name, type, index)          LV_PROPERTY_## clz ##_##name = (LV_PROPERTY_## clz ##_START + ((int)index)) | ((type) << LV_PROPERTY_TYPE_SHIFT)

/* Example:
 * LV_PROPERTY_ID2(SLIDER, VALUE, LV_PROPERTY_TYPE_INT, LV_PROPERTY_TYPE_BOOL, 0)
 * produces
 * LV_PROPERTY_SLIDER_VALUE = (LV_PROPERTY_SLIDER_START + (0)) | ((LV_PROPERTY_TYPE_INT) << LV_PROPERTY_TYPE_SHIFT) | ((LV_PROPERTY_TYPE_BOOL) << LV_PROPERTY_TYPE2_SHIFT)
 */
#define LV_PROPERTY_ID2(clz, name, type, type2, index)  LV_PROPERTY_ID(clz, name, type, index) | ((type2) << LV_PROPERTY_TYPE2_SHIFT)

#define LV_PROPERTY_ID_TYPE(id) ((id) >> LV_PROPERTY_TYPE_SHIFT)
#define LV_PROPERTY_ID_TYPE2(id) ((id) >> LV_PROPERTY_TYPE_SHIFT)
#define LV_PROPERTY_ID_INDEX(id) ((id) & 0xfffffff)

/*Set properties from an array of lv_property_t*/
#define LV_OBJ_SET_PROPERTY_ARRAY(obj, array) lv_obj_set_properties(obj, array, sizeof(array)/sizeof(array[0]))


/**********************
 *      TYPEDEFS
 **********************/

/**
 * Group of predefined widget ID start value.
 */
enum _lv_prop_id_range_boundary_t {
    LV_PROPERTY_ID_INVALID      = 0,

    /*ID 0x01 to 0xff are style ID, check lv_style_prop_t*/
    LV_PROPERTY_STYLE_START     = 0x00,

    LV_PROPERTY_ID_START        = 0x0100, /*ID smaller than 0xff is style ID*/
    /*Define the property ID for every widget here. */
    LV_PROPERTY_OBJ_START       = 0x0100, /* lv_obj.c */
    LV_PROPERTY_IMAGE_START     = 0x0200, /* lv_image.c */
    LV_PROPERTY_LABEL_START     = 0x0300, /* lv_label.c */
    LV_PROPERTY_KEYBOARD_START  = 0x0400, /* lv_keyboard.c */
    LV_PROPERTY_TEXTAREA_START  = 0x0500, /* lv_textarea.c */
    LV_PROPERTY_ROLLER_START    = 0x0600, /* lv_roller.c */
    LV_PROPERTY_DROPDOWN_START  = 0x0700, /* lv_dropdown.c */
    LV_PROPERTY_SLIDER_START    = 0x0800, /* lv_slider.c */
    LV_PROPERTY_ANIMIMAGE_START = 0x0900, /* lv_animimage.c */

    /*Special ID, use it to extend ID and make sure it's unique and compile time determinant*/
    LV_PROPERTY_ID_BUILTIN_LAST = 0xffff, /*ID of 0x10000 ~ 0xfffffff is reserved for user*/

    LV_PROPERTY_ID_ANY          = 0x7ffffffe, /*Special ID used by lvgl to intercept all setter/getter call.*/
};

struct _lv_property_name_t {
    const char * name;
    lv_prop_id_t id;
};

typedef struct {
    lv_prop_id_t id;
    union {
        int32_t num;                /**< Signed integer number (enums or "normal" numbers)*/
        uint32_t num_u;             /**< Unsigned integer number (opacity, Booleans)  */
        bool enable;                /**< Booleans */
        const void * ptr;           /**< Constant pointers  (font, cone text, etc.) */
        lv_color_t color;           /**< Colors */
        lv_value_precise_t precise; /**< float or int for precise value */
        lv_point_t point;           /**< Point, contains two int32_t */

        struct {
            /**
             * Note that place struct member `style` at first place is intended.
             * `style` shares same memory with `num`, `ptr`, `color`.
             * So we set the style value directly without using `prop.style.num`.
             *
             * E.g.
             *
             * static const lv_property_t obj_pos_x = {
             *      .id = LV_PROPERTY_STYLE_X,
             *      .num = 123,
             *      .selector = LV_STATE_PRESSED,
             * }
             *
             * instead of:
             * static const lv_property_t obj_pos_x = {
             *      .id = LV_PROPERTY_STYLE_X,
             *      .style.num = 123, // note this line.
             *      .selector = LV_STATE_PRESSED,
             * }
             */
            lv_style_value_t style; /**< Make sure it's the first element in struct. */
            uint32_t selector;      /**< Style selector, lv_part_t | lv_state_t */
        };

        /**
         * For some properties like slider range, it contains two simple (4-byte) values
         * so we can use `arg1.num` and `arg2.num` to set the argument.
         */
        struct {
            union {
                int32_t num;
                uint32_t num_u;
                bool enable;
                const void * ptr;
                lv_color_t color;
                lv_value_precise_t precise;
            } arg1, arg2;
        };
    };
} lv_property_t;

typedef struct {
    lv_prop_id_t id;

    void * setter;      /**< Callback used to set property. */
    void * getter;      /**< Callback used to get property. */
} lv_property_ops_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set Widget property.
 * @param obj       pointer to Widget
 * @param value     property value to set
 * @return          return LV_RESULT_OK if call succeeded
 */
lv_result_t lv_obj_set_property(lv_obj_t * obj, const lv_property_t * value);

/**
 * Set multiple Widget properties. Helper `LV_OBJ_SET_PROPERTY_ARRAY` can be used for constant property array.
 * @param obj       pointer to Widget
 * @param value     property value array to set
 * @param count     number of array elements
 * @return          return LV_RESULT_OK if call succeeded
 */
lv_result_t lv_obj_set_properties(lv_obj_t * obj, const lv_property_t * value, uint32_t count);

/*=====================
 * Getter functions
 *====================*/

/**
 * Read property value from Widget.
 * If id is a style property.  Style selector is 0 by default.
 * @param obj       pointer to Widget
 * @param id        ID of property to read
 * @return          return property value read. The returned property ID is set to `LV_PROPERTY_ID_INVALID` if read failed.
 */
lv_property_t lv_obj_get_property(lv_obj_t * obj, lv_prop_id_t id);

/**
 * Read style property value from Widget
 * @param obj       pointer to Widget
 * @param id        ID of style property
 * @param selector  selector for style property
 * @return          return property value read. The returned property ID is set to `LV_PROPERTY_ID_INVALID` if read failed.
 */
lv_property_t lv_obj_get_style_property(lv_obj_t * obj, lv_prop_id_t id, uint32_t selector);

/**
 * Get property ID by recursively searching for name in Widget's class hierarchy, and
 * if still not found, then search style properties.
 * Requires to enabling `LV_USE_OBJ_PROPERTY_NAME`.
 * @param obj       pointer to Widget whose class and base-class hierarchy are to be searched.
 * @param name      property name
 * @return          property ID found or `LV_PROPERTY_ID_INVALID` if not found.
 */
lv_prop_id_t lv_obj_property_get_id(const lv_obj_t * obj, const char * name);

/**
 * Get property ID by doing a non-recursive search for name directly in Widget class properties.
 * Requires enabling `LV_USE_OBJ_PROPERTY_NAME`.
 * @param clz       pointer to Widget class that has specified property.
 * @param name      property name
 * @return          property ID found or `LV_PROPERTY_ID_INVALID` if not found.
 */
lv_prop_id_t lv_obj_class_property_get_id(const lv_obj_class_t * clz, const char * name);

/**
 * Get style property ID by name. Requires enabling `LV_USE_OBJ_PROPERTY_NAME`.
 * @param name      property name
 * @return          property ID found or `LV_PROPERTY_ID_INVALID` if not found.
 */
lv_prop_id_t lv_style_property_get_id(const char * name);

/**********************
 *      MACROS
 **********************/

#include "../widgets/property/lv_obj_property_names.h"
#include "../widgets/property/lv_style_properties.h"

#endif /*LV_USE_OBJ_PROPERTY*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_PROPERTY_H*/
