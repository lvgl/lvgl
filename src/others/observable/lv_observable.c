#include "lv_observable.h"

#include "../../../misc/lv_ll.h"
//#include "../../../misc/lv_style.h"

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
 *  GLOBAL VARIABLES
 **********************/

//lv_event_code_t LV_EVENT_OBSERVABLE_CHANGED;

/**********************
 *      MACROS
 **********************/

 /**********************
  *   GLOBAL FUNCTIONS
  **********************/



void lv_observable_init_bool(lv_observable_t *obs)
{
    obs->type = LV_TYPE_BOOL;
    obs->value.num = 0;
    _lv_ll_init(&(obs->subs_ll), sizeof(lv_observable_sub_dsc_t));
}

void lv_observable_init_int(lv_observable_t* obs)
{
    obs->type = LV_TYPE_INT;
    obs->value.num = 0;
    _lv_ll_init(&(obs->subs_ll), sizeof(lv_observable_sub_dsc_t));
}

void lv_observable_init_pointer(lv_observable_t* obs)
{
    obs->type = LV_TYPE_POINTER;
    obs->value.ptr = NULL;
    _lv_ll_init(&(obs->subs_ll), sizeof(lv_observable_sub_dsc_t));
}

void lv_observable_init_string(lv_observable_t* obs, char * buf, size_t size)
{
    obs->type = LV_TYPE_STRING + size;
    obs->value.ptr = buf;
    _lv_ll_init(&(obs->subs_ll), sizeof(lv_observable_sub_dsc_t));
}

void lv_observable_init_color(lv_observable_t* obs, lv_color_t color)
{
    obs->type = LV_TYPE_COLOR;
    obs->value.color = color;
    _lv_ll_init(&(obs->subs_ll), sizeof(lv_observable_sub_dsc_t));
}


void * lv_observable_subscribe_obj(lv_observable_t* obs, lv_observable_cb_t cb, lv_obj_t* obj, void* data1, void* data2)
{
    lv_observable_sub_dsc_t* s = _lv_ll_ins_tail(&(obs->subs_ll));
    LV_ASSERT_MALLOC(s);
    if (s == NULL) return NULL;

    lv_memset_00(s, sizeof(*s));

    s->obj = obj;
    s->callback = cb;
    s->data1 = data1;
    s->data2 = data2;
    return s;
}

void lv_observable_unsubscribe(lv_observable_t* obs, void* s)
{
    LV_ASSERT_NULL(s);
    _lv_ll_remove(&(obs->subs_ll), s);
    lv_mem_free(s);
}

/*
void _lv_observable_set(lv_observable_t* obs, lv_uintptr_t value)
{
    lv_observable_sub_dsc_t* s;
    obs->value = value;
    _LV_LL_READ(&(obs->subs_ll), s) {
        if (s->callback) {
            s->callback(obs, s);
        }
    }
}
*/

static void notify(lv_observable_t* obs)
{
    lv_observable_sub_dsc_t* s;
    _LV_LL_READ(&(obs->subs_ll), s) {
        if (s->callback) {
            s->callback(obs, s);
        }
    }
}

void lv_observable_set_num(lv_observable_t* obs, int32_t value)
{
    obs->value.num = value;
    notify(obs);
}

void lv_observable_set_ptr(lv_observable_t* obs, void *ptr)
{
    obs->value.ptr = ptr;
    notify(obs);
}

void lv_observable_set_color(lv_observable_t* obs, lv_color_t color)
{
    obs->value.color = color;
    notify(obs);
}

void lv_observable_set_string(lv_observable_t* obs, char* buf)
{
    size_t maxlen = obs->type - LV_TYPE_STRING;
    if (maxlen < 1)
        return;
    strncpy((char*)obs->value.ptr, buf, maxlen - 1);
    ((char*)obs->value.ptr)[maxlen - 1] = 0;    /* make sure it is terminated properly */
    notify(obs);
}

void lv_observable_set_obj_flag_cb(lv_observable_t* obs, void* s)
{
    lv_observable_sub_dsc_t* dsc = (lv_observable_sub_dsc_t*)s;
    if (obs->value.num) {
        lv_obj_add_flag(dsc->obj, (lv_obj_flag_t)dsc->data1);
    }
    else {
        lv_obj_clear_flag(dsc->obj, (lv_obj_flag_t)dsc->data1);
    }
}

void lv_bind_bool_to_obj_flag(lv_observable_t* obs, lv_obj_t* obj, lv_obj_flag_t flag)
{
    lv_observable_subscribe_obj(obs, lv_observable_set_obj_flag_cb, obj, (void*)flag, NULL);
}

void lv_observable_set_obj_local_style_prop_cb(lv_observable_t* obs, void* s)
{
    lv_observable_sub_dsc_t* dsc = (lv_observable_sub_dsc_t*)s;
    lv_obj_set_local_style_prop(dsc->obj, (lv_style_prop_t)dsc->data1, *(lv_style_value_t*)&obs->value, (lv_style_selector_t)dsc->data2);
}

void lv_bind_int_to_obj_local_style_prop(lv_observable_t* obs, lv_obj_t* obj, lv_style_prop_t prop, lv_style_selector_t selector)
{
    lv_observable_subscribe_obj(obs, lv_observable_set_obj_local_style_prop_cb, obj, (void*)prop, (void*)selector);
}

void lv_observable_set_label_text_cb(lv_observable_t* obs, void* s)
{
    lv_observable_sub_dsc_t* dsc = (lv_observable_sub_dsc_t*)s;
    lv_label_set_text(dsc->obj, obs->value.ptr);
}

void lv_bind_string_to_label_text(lv_observable_t* obs, lv_obj_t* obj, char * text)
{
    lv_observable_subscribe_obj(obs, lv_observable_set_label_text_cb, obj, text, NULL);
}

void lv_observable_set_int_label_text_cb(lv_observable_t* obs, void* s)
{
    lv_observable_sub_dsc_t* dsc = (lv_observable_sub_dsc_t*)s;
    lv_label_set_text_fmt(dsc->obj, dsc->data1, obs->value.num);
}

void lv_bind_int_to_label_text_fmt(lv_observable_t* obs, lv_obj_t* obj, const char* fmt)
{
    lv_observable_subscribe_obj(obs, lv_observable_set_int_label_text_cb, obj, fmt, NULL);
}

#if 0
/*****************************************
 lv_observable_t Variable binding example
*****************************************/	
	
#include "lvgl/src/extra/others/observable/lv_observable.h"


//#define OBS_FLAG_TEST
//#define OBS_COLOR_TEST
//#define OBS_CUSTOM_CB_TEST
//#define OBS_LABEL_TEST
//#define OBS_LABEL_PTR_TEST
#define OBS_LABEL_INT_TEST

static lv_obj_t* box1;
static lv_obj_t* box2;
static lv_obj_t* box3;
static lv_obj_t* label;

#ifdef OBS_FLAG_TEST
lv_observable_t box1Hide;
lv_observable_t box2Hide;
#endif

#ifdef OBS_COLOR_TEST
const lv_color_t colors[3] = { LV_COLOR_MAKE(255, 0, 0), LV_COLOR_MAKE(0, 255, 0), LV_COLOR_MAKE(0, 0, 255)};
lv_observable_t box3BgColor;
#endif

#ifdef OBS_CUSTOM_CB_TEST
lv_observable_t boxNum;
#endif

#ifdef OBS_LABEL_TEST
char labelTextBuf[64];
lv_observable_t labelText;
#endif

#ifdef OBS_LABEL_INT_TEST
lv_observable_t labelInt;
#endif


static void ui_blink_anim_cb(void* p, int32_t show)
{
	lv_obj_t* obj = (lv_obj_t*)p;
#ifdef OBS_FLAG_TEST
	lv_observable_set_num(&box1Hide, show);
	lv_observable_set_num(&box2Hide, !show);
//	lv_observable_set(&box3Hide, show);
#endif
#ifdef OBS_COLOR_TEST
	static int color = 0;
	if (!show) {
		lv_observable_set_color(&box3BgColor, colors[color]);
		if (++color == 3)
			color = 0;
	}
#endif
#ifdef OBS_CUSTOM_CB_TEST
	static int num = 0;
	lv_observable_set_num(&boxNum, num);
	if (++num == 4)
		num = 0;
#endif
#ifdef OBS_LABEL_TEST
	static int num = 0;
	char texts[3][64] = {
		"This",
		"is a",
		"test" };
#ifdef OBS_LABEL_PTR_TEST
	lv_observable_set_ptr(&labelText, texts[num]);
#else
	lv_observable_set_string(&labelText, texts[num]);
#endif
	if (++num == 3)
		num = 0;

#endif
#ifdef OBS_LABEL_INT_TEST
	static int num = 0;
	lv_observable_set_num(&labelInt, num);
	if (++num == 31)
		num = 0;
#endif
}

static void lv_set_obj_flag_to(lv_obj_t* obj, lv_obj_flag_t flag, bool value)
{
	if (value) {
		lv_obj_add_flag(obj, flag);
	}
	else {
		lv_obj_clear_flag(obj, flag);
	}
}

#ifdef OBS_CUSTOM_CB_TEST
static void lv_observable_custom_cb(lv_observable_t* obs, void* s)
{
	lv_set_obj_flag_to(box1, LV_OBJ_FLAG_HIDDEN, obs->value.num < 1);
	lv_set_obj_flag_to(box2, LV_OBJ_FLAG_HIDDEN, obs->value.num < 2);
	lv_set_obj_flag_to(box3, LV_OBJ_FLAG_HIDDEN, obs->value.num < 3);
}
#endif

/**
 * Called to blink an object
 * @param obj pointer to an object
 * @param blink_time blink time in ms (0 stops blinking)
 */
static void ui_blink_start(lv_obj_t* obj, uint32_t blink_time)
{
	if (blink_time == 0) {
		lv_anim_del(obj, ui_blink_anim_cb);
	}
	else {
		lv_anim_t a;
		lv_anim_init(&a);
		lv_anim_set_var(&a, obj);
		lv_anim_set_exec_cb(&a, ui_blink_anim_cb);
		lv_anim_set_time(&a, blink_time);
		lv_anim_set_playback_time(&a, blink_time);
		lv_anim_set_values(&a, 1, 0);
		lv_anim_set_path_cb(&a, lv_anim_path_step);
		lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
		lv_anim_start(&a);
	}
}

void lv_observable_example_init(void)
{
	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xffffff), 0);

	box1 = lv_obj_create(lv_scr_act());
	lv_obj_remove_style_all(box1);
	lv_obj_set_size(box1, 100, 100);
	lv_obj_set_align(box1, LV_ALIGN_LEFT_MID);
	lv_obj_set_x(box1, 50);
	lv_obj_set_style_bg_opa(box1, 255, 0);
	lv_obj_set_style_bg_color(box1, lv_color_hex(0xff0000), 0);

	box2 = lv_obj_create(lv_scr_act());
	lv_obj_remove_style_all(box2);
	lv_obj_set_size(box2, 100, 100);
	lv_obj_set_align(box2, LV_ALIGN_LEFT_MID);
	lv_obj_set_x(box2, 200);
	lv_obj_set_style_bg_opa(box2, 255, 0);
	lv_obj_set_style_bg_color(box2, lv_color_hex(0x00ff00), 0);

	box3 = lv_obj_create(lv_scr_act());
	lv_obj_remove_style_all(box3);
	lv_obj_set_size(box3, 100, 100);
	lv_obj_set_align(box3, LV_ALIGN_LEFT_MID);
	lv_obj_set_x(box3, 350);
	lv_obj_set_style_bg_opa(box3, 255, 0);
	lv_obj_set_style_bg_color(box3, lv_color_hex(0x0000ff), 0);

	label = lv_label_create(lv_scr_act());
	lv_obj_set_size(label, 100, 100);
	lv_obj_set_align(label, LV_ALIGN_CENTER);
	lv_obj_set_x(label, 350);

#ifdef OBS_FLAG_TEST
	lv_observable_init_bool(&box1Hide);
	lv_bind_bool_to_obj_flag(&box1Hide, box1, LV_OBJ_FLAG_HIDDEN);

	lv_observable_init_bool(&box2Hide);
	lv_bind_bool_to_obj_flag(&box2Hide, box2, LV_OBJ_FLAG_HIDDEN);

	lv_bind_bool_to_obj_flag(&box1Hide, box3, LV_OBJ_FLAG_HIDDEN);
#endif

#ifdef OBS_COLOR_TEST
	lv_observable_init_pointer(&box3BgColor);
	lv_bind_int_to_obj_local_style_prop(&box3BgColor, box3, LV_STYLE_BG_COLOR, 0);
#endif

#ifdef OBS_CUSTOM_CB_TEST
	lv_observable_init_int(&boxNum);
	lv_observable_subscribe_obj(&boxNum, lv_observable_custom_cb, NULL, NULL, NULL);
#endif

#ifdef OBS_LABEL_TEST
	lv_observable_init_string(&labelText, labelTextBuf, 64);
	lv_bind_string_to_label_text(&labelText, label, NULL);
#endif

#ifdef OBS_LABEL_INT_TEST
	lv_observable_init_int(&labelInt);
	lv_bind_int_to_label_text_fmt(&labelInt, label, "%d Psi");
#endif

	ui_blink_start(box3, 500);
}
	
#endif
