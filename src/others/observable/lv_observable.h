#ifndef _LV_OBSERVABLE_H_
#define _LV_OBSERVABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

#define LV_TYPE_INT     0
#define LV_TYPE_FLOAT   1
#define LV_TYPE_COLOR   2
#define LV_TYPE_POINTER 3
#define LV_TYPE_STRING  4
#define LV_TYPE_NUM     5

/**
* A common type to handle all the observable types in the same way.
*/
typedef union {
    int32_t                 num;        /**< Integer number (opacity, enums, booleans or "normal" numbers)*/
    /*TODO: we may want to check for float support */
    float                   fnum;       /**< Floating point number */
    const void*             ptr;        /**< Constant pointer  (font, cone text, etc)*/
    lv_color_t              color;      /**< Color */
    int16_t                 num16[2];
} lv_observable_value_t;

/**
* Conditional function is evaluated before the object is notified
*/
typedef bool (*lv_cond_fn_t)(const lv_observable_value_t v1, const lv_observable_value_t v2);

/**
* Condition codes
*/
typedef enum {
    LV_COND_UC,
    LV_COND_EQ,
    LV_COND_NE,
    LV_COND_LT,
    LV_COND_LE,
    LV_COND_GT,
    LV_COND_GE,
    LV_COND_NUM
} lv_cond_t;

/*
* The observable value object
*/
typedef struct {
    size_t                  type;       /* for elementary types the id, for strings LV_TYPE_STRING + the length of the string */
    lv_observable_value_t   value;
    lv_ll_t                 subs_ll;    /* subscribers */
} lv_observable_t;

/*
* Callback called when the observed value changes
* Params:
*   s: the lv_observable_sub_dst_t object created when the object was subscribed to the value
*   cond: the result of evaluating the condition specified during subscription
*/
typedef void (*lv_observable_cb_t)(void* s, bool cond);

/*
* Descriptor for subscribing objects to observable values
*/
typedef struct {
    lv_observable_t*        obs;        /* the observable value */
    lv_cond_fn_t            cond;       /* condition to evaluate before notifying the object */
    lv_observable_value_t   cond_val;   /* compare this value to the value of the observable */
    lv_observable_cb_t      callback;   /* callback that should be called when the value changes */
    lv_obj_t*               obj;        /* object which has subscribed for the value */
    void*                   data1;      /* first user data supplied when registering an object */
    void*                   data2;      /* second user data supplied when registering an object */
} lv_observable_sub_dsc_t;


void lv_observable_init_bool(lv_observable_t* obs, bool value);
void lv_observable_init_int(lv_observable_t* obs, int value);
void lv_observable_init_pointer(lv_observable_t* obs, void *value);
void lv_observable_init_string(lv_observable_t* obs, char* buf, size_t size);
void lv_observable_init_color(lv_observable_t* obs, lv_color_t color);

void lv_observable_set_int(lv_observable_t* obs, int32_t value);
void lv_observable_set_ptr(lv_observable_t* obs, void* value);
void lv_observable_set_color(lv_observable_t* obs, lv_color_t color);
void lv_observable_set_string(lv_observable_t* obs, char* buf);

void* lv_observable_subscribe_obj(lv_observable_t* obs, lv_observable_cb_t cb, lv_obj_t* obj, void* data1, void* data2, lv_cond_fn_t cond, lv_observable_value_t cond_val);
void lv_observable_unsubscribe(lv_observable_t* obs, void* s);

void lv_bind_int_to_obj_flag_cond(lv_observable_t* obs, lv_obj_t* obj, lv_obj_flag_t flag, lv_cond_t cond, int32_t cond_val);
void lv_bind_int_to_obj_state_cond(lv_observable_t* obs, lv_obj_t* obj, lv_state_t flag, lv_cond_t cond, int32_t cond_val);
void lv_bind_int_to_callback_cond(lv_observable_t* obs, lv_observable_cb_t cb, lv_obj_t* obj, void* data1, void* data2, lv_cond_t cond, int32_t cond_val);


/* shorthands with no conditional */
#define lv_bind_int_to_obj_flag(obs, obj, flag) \
    lv_bind_int_to_obj_flag_cond(obs, obj, flag, LV_COND_NE, 0)


/*
void lv_bind_bool_to_obj_flag(lv_observable_t* obs, lv_obj_t* obj, lv_obj_flag_t flag);
void lv_bind_int_to_obj_local_style_prop(lv_observable_t* obs, lv_obj_t* obj, lv_style_prop_t prop, lv_style_selector_t selector);
void lv_bind_string_to_label_text(lv_observable_t* obs, lv_obj_t* obj, char* text);
void lv_bind_int_to_label_text_fmt(lv_observable_t* obs, lv_obj_t* obj, const char* fmt);
*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // _LV_OBSERVABLE_H_
