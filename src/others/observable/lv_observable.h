#ifndef _LV_OBSERVABLE_H_
#define _LV_OBSERVABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

#define LV_TYPE_BOOL    0
#define LV_TYPE_INT     1
#define LV_TYPE_COLOR   2
#define LV_TYPE_POINTER 3
#define LV_TYPE_STRING  4

typedef struct {
    size_t type;        /* for elementary types the id, for strings LV_TYPE_STRING + the length of the string */
//    lv_uintptr_t value; /* value for bool, int and pointer, pointer to a statically allocated char array for strings */
    lv_style_value_t value;
    lv_ll_t subs_ll;    /* subscribers */
} lv_observable_t;

typedef void (*lv_observable_cb_t)(lv_observable_t* obs, void* s);

typedef struct {
    lv_observable_cb_t  callback;   /* callback that should be called when the value changes */
    lv_obj_t* obj;                  /* object which subscribed for the value change */
    void* data1;                    /* user data supplied when registering an object */
    void* data2;                    /* user data supplied when registering an object */
} lv_observable_sub_dsc_t;


void lv_observable_init_bool(lv_observable_t* obs);
void lv_observable_init_int(lv_observable_t* obs);
void lv_observable_init_pointer(lv_observable_t* obs);
void lv_observable_init_string(lv_observable_t* obs, char* buf, size_t size);
void lv_observable_init_color(lv_observable_t* obs, lv_color_t color);

void lv_observable_set_num(lv_observable_t* obs, int32_t value);
void lv_observable_set_ptr(lv_observable_t* obs, void* ptr);
void lv_observable_set_color(lv_observable_t* obs, lv_color_t color);
void lv_observable_set_string(lv_observable_t* obs, char* buf);

void* lv_observable_subscribe_obj(lv_observable_t* obs, lv_observable_cb_t cb, lv_obj_t* obj, void* data1, void* data2);
void lv_observable_unsubscribe(lv_observable_t* obs, void* s);
void lv_bind_bool_to_obj_flag(lv_observable_t* obs, lv_obj_t* obj, lv_obj_flag_t flag);
void lv_bind_int_to_obj_local_style_prop(lv_observable_t* obs, lv_obj_t* obj, lv_style_prop_t prop, lv_style_selector_t selector);
void lv_bind_string_to_label_text(lv_observable_t* obs, lv_obj_t* obj, char* text);
void lv_bind_int_to_label_text_fmt(lv_observable_t* obs, lv_obj_t* obj, const char* fmt);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // _LV_OBSERVABLE_H_
