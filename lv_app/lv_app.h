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

#if LV_APP_ENABLE != 0
#include "lvgl/lv_app/lv_app_util/lv_app_kb.h"
#include "lvgl/lv_app/lv_app_util/lv_app_fsel.h"
#include "lvgl/lv_app/lv_app_util/lv_app_notice.h"


/*********************
 *      DEFINES
 *********************/
/*Check dependencies*/
#if LV_OBJ_FREE_P == 0
#error "lv_app: Free pointer is required for application. Enable it lv_conf.h: LV_OBJ_FREE_P 1"
#endif

#if DM_CUSTOM == 0 && DM_MEM_SIZE < (2 * 1024)
#error "lv_app: not enough dynamic memory. Increase it in misc_conf.h: DM_MEM_SIZE"
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
	LV_APP_MODE_NONE 	 = 0x0000,
	LV_APP_MODE_NOT_LIST = 0x0001,		/*Do not list the application*/
	LV_APP_MODE_NO_SC_TITLE = 0x0002,	/*No short cut title*/
}lv_app_mode_t;

typedef enum
{
	LV_APP_COM_TYPE_CHAR,   /*Stream of characters. Not '\0' terminated*/
	LV_APP_COM_TYPE_INT,    /*Stream of 'int32_t' numbers*/
	LV_APP_COM_TYPE_LOG,    /*String about an event to log*/
    LV_APP_COM_TYPE_TRIG,   /*A trigger to do some specific action (data is ignored)*/
    LV_APP_COM_TYPE_INV,    /*Invalid type*/
    LV_APP_COM_TYPE_NUM,    /*Indicates the number of com. types*/
}lv_app_com_type_t;

struct __LV_APP_DSC_T;

typedef struct
{
	const struct __LV_APP_DSC_T * dsc;
	char * name;
	lv_obj_t * sc;
	lv_obj_t * sc_title;
	lv_obj_t * win;
    lv_obj_t * conf_win;
	void * app_data;
	void * sc_data;
	void * win_data;
}lv_app_inst_t;

typedef struct __LV_APP_DSC_T
{
	const char * name;
	lv_app_mode_t mode;
	void (*app_run)(lv_app_inst_t *, void *);
	void (*app_close) (lv_app_inst_t *);
	void (*com_rec) (lv_app_inst_t *, lv_app_inst_t *, lv_app_com_type_t, const void *, uint32_t);
	void (*sc_open) (lv_app_inst_t *, lv_obj_t *);
	void (*sc_close) (lv_app_inst_t *);
	void (*win_open) (lv_app_inst_t *, lv_obj_t *);
	void (*win_close) (lv_app_inst_t *);
    void (*conf_open) (lv_app_inst_t *, lv_obj_t * );
	uint16_t app_data_size;
	uint16_t sc_data_size;
	uint16_t win_data_size;
}lv_app_dsc_t;

typedef struct {
	lv_style_t menu;
    lv_style_t menu_btn_rel;
    lv_style_t menu_btn_pr;
    lv_style_t sc_rel;
    lv_style_t sc_pr;
    lv_style_t sc_send_rel;
    lv_style_t sc_send_pr;
    lv_style_t sc_rec_rel;
    lv_style_t sc_rec_pr;
    lv_style_t sc_title;
    lv_style_t win_header;
    lv_style_t win_scrl;
    lv_style_t win_cbtn_rel;
    lv_style_t win_cbtn_pr;
}lv_app_style_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the application system
 */
void lv_app_init(void);

/**
 * Run an application according to 'app_dsc'
 * @param app_dsc pointer to an application descriptor
 * @param conf pointer to an application specific configuration structure or NULL if unused
 * @return pointer to the opened application or NULL if any error occurred
 */
lv_app_inst_t * lv_app_run(const lv_app_dsc_t * app_dsc, void * conf);

/**
 * Close a running application. Close the Window and the Shortcut too if opened.
 * @param app pointer to an application
 */
void lv_app_close(lv_app_inst_t * app);

/**
 * Open a shortcut for an application
 * @param app pointer to an application
 * @return pointer to the shortcut
 */
lv_obj_t * lv_app_sc_open(lv_app_inst_t * app);

/**
 * Close the shortcut of an application
 * @param app pointer to an application
 */
void lv_app_sc_close(lv_app_inst_t * app);

/**
 * Open the application in a window
 * @param app pointer to an application
 * @return pointer to the shortcut
 */
lv_obj_t * lv_app_win_open(lv_app_inst_t * app);

/**
 * Close the window of an application
 * @param app pointer to an application
 */
void lv_app_win_close(lv_app_inst_t * app);

/**
 * Send data to other applications
 * @param app_send pointer to the application which is sending the message
 * @param type type of data from 'lv_app_com_type_t' enum
 * @param data pointer to the sent data
 * @param size length of 'data' in bytes
 * @return number application which were received the message
 */
uint16_t lv_app_com_send(lv_app_inst_t * app_send, lv_app_com_type_t type , const void * data, uint32_t size);

/**
 * Test an application communication connection
 * @param sender pointer to an application which sends data
 * @param receiver pointer to an application which receives data
 * @return false: no connection, true: there is connection
 */
bool lv_app_con_check(lv_app_inst_t * sender, lv_app_inst_t * receiver);

/**
 * Create a new connection between two applications
 * @param sender pointer to a data sender application
 * @param receiver pointer to a data receiver application
 */
void lv_app_con_set(lv_app_inst_t * sender, lv_app_inst_t * receiver);

/**
 * Delete a communication connection
 * @param sender pointer to a data sender application or NULL to be true for all sender
 * @param receiver pointer to a data receiver application  or NULL to be true for all receiver
 */
void lv_app_con_del(lv_app_inst_t * sender, lv_app_inst_t * receiver);

/**
 * Get the application descriptor from its name
 * @param name name of the app. dsc.
 * @return pointer to the app. dsc.
 */
const lv_app_dsc_t * lv_app_dsc_get(const char * name);

/**
 * Rename an application
 * @param app pointer to an application
 * @param name a string with the new name
 */
void lv_app_rename(lv_app_inst_t * app, const char * name);

/**
 * Get the window object from an object located on the window
 * @param obj pointer to an object on the window
 * @return pointer to the window of 'obj'
 */
lv_obj_t * lv_app_win_get_from_obj(lv_obj_t * obj);

/**
 * Read the list of the running applications. (Get he next element)
 * @param prev the previous application (at the first call give NULL to get the first application)
 * @param dsc pointer to an application descriptor to filer the applications (NULL to do not filter)
 * @return pointer to the next running application or NULL if no more
 */
lv_app_inst_t * lv_app_get_next(lv_app_inst_t * prev, lv_app_dsc_t * dsc);

/**
 * Read the list of applications descriptors. (Get he next element)
 * @param prev the previous application descriptors(at the first call give NULL to get the first)
 * @return pointer to the next application descriptors or NULL if no more
 */
lv_app_dsc_t ** lv_app_dsc_get_next(lv_app_dsc_t ** prev);


/**
 * Get a pointer to the application style structure. If modified then 'lv_app_refr_style' should be called
 * @return pointer to the application style structure
 */
lv_app_style_t * lv_app_style_get(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_ENABLE != 0*/

#endif /*LV_APP_H*/
