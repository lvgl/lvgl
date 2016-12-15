/**
 * @file lv_app.h
 *
 */

#ifndef LV_APP_H
#define LV_APP_H

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
	LV_APP_MODE_NONE 	 = 0x0000,
	LV_APP_MODE_RUN_ONCE = 0x0001,
	LV_APP_MODE_NO_SC    = 0x0002,		/*No short cut*/
	LV_APP_MODE_NO_WIN  = 0x0004,		/*No window mode*/
	LV_APP_MODE_NO_CON   = 0x0008,		/*No connection to other apps*/
	LV_APP_MODE_NO_CLOSE = 0x0010,		/*No close control button*/
	LV_APP_MODE_NO_FIX   = 0x0020,		/*No fix control button*/
}lv_app_mode_t;

typedef enum
{
	LV_APP_EVENT_OPEN,
	LV_APP_EVENT_CLOSE,
	LV_APP_EVENT_SC_OPENED,
	LV_APP_EVENT_SC_CLOSED,
	LV_APP_EVENT_WIN_OPENED,
	LV_APP_EVENT_WIN_CLOSED,
}lv_app_event_t;

struct __LV_APP_DSC_T;

typedef struct
{
	const struct __LV_APP_DSC_T * dsc;
	const char * name_mod;
	lv_obj_t * sc;
	lv_obj_t * win;
	void * app_data;
	void * sc_data;
	void * win_data;
}lv_app_inst_t;

typedef struct __LV_APP_DSC_T
{
	const char * name;
	lv_app_mode_t mode;
	void (*app_run)(lv_app_inst_t *, const char *);
	void (*app_close) (lv_app_inst_t *);
	void (*event_read) (lv_app_inst_t *, lv_app_event_t);
	void (*sc_open) (lv_app_inst_t *, lv_obj_t *);
	void (*sc_close) (lv_app_inst_t *);
	void (*win_open) (lv_app_inst_t *, lv_obj_t *);
	void (*win_close) (lv_app_inst_t *);
	uint16_t app_data_size;
	uint16_t sc_data_size;
	uint16_t win_data_size;
}lv_app_dsc_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_app_init(void);
lv_app_inst_t * lv_app_run(const lv_app_dsc_t * app_dsc, const char * cstr);
void lv_app_close(lv_app_inst_t * app);
void lv_app_event_send(lv_app_inst_t * app, lv_app_event_t event);
lv_obj_t * lv_app_sc_open(lv_app_inst_t * app);
void lv_app_sc_close(lv_app_inst_t * app);
lv_obj_t * lv_app_win_open(lv_app_inst_t * app);
void lv_app_win_close(lv_app_inst_t * app);
const lv_app_dsc_t * lv_app_dsc_get(const char * name);

const lv_app_dsc_t * lv_app_example_init(void);

/**********************
 *      MACROS
 **********************/

#endif
