/**
 * @file lv_app.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app.h"

#if LV_APP_ENABLE != 0
#include "lv_app_util/lv_app_kb.h"
#include "lv_app_util/lv_app_notice.h"
#include "lv_app_util/lv_app_fsel.h"

#include "lvgl/lv_misc/anim.h"

#include "../lv_appx/lv_app_example.h"
#include "../lv_appx/lv_app_sysmon.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_app_inst_t * sender;
    lv_app_inst_t * receiver;
}lv_app_con_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_app_init_desktop(void);

/*Actions*/
static lv_action_res_t lv_app_menu_rel_action(lv_obj_t * app_btn, lv_dispi_t * dispi);
static lv_action_res_t lv_app_menu_elem_rel_action(lv_obj_t * app_elem_btn, lv_dispi_t * dispi);
static lv_action_res_t lv_app_sc_page_rel_action(lv_obj_t * sc, lv_dispi_t * dispi);
static lv_action_res_t lv_app_sc_rel_action(lv_obj_t * sc, lv_dispi_t * dispi);
static lv_action_res_t lv_app_sc_lpr_action(lv_obj_t * sc, lv_dispi_t * dispi);
static lv_action_res_t lv_app_win_close_action(lv_obj_t * close_btn, lv_dispi_t * dispi);
static lv_action_res_t lv_app_win_minim_action(lv_obj_t * close_minim, lv_dispi_t * dispi);

static lv_action_res_t lv_app_win_open_anim_create(lv_app_inst_t * app);
static lv_action_res_t lv_app_win_minim_anim_create(lv_app_inst_t * app);
#if LV_APP_EFFECT_ANIM != 0 && LV_APP_ANIM_WIN != 0
static void lv_app_win_close_anim_cb(lv_obj_t * app_win);
#endif

static void lv_app_init_icons(void);
static void lv_app_init_style(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static ll_dsc_t app_dsc_ll;  /*Store a pointer to the app. descriptors*/
static ll_dsc_t app_inst_ll; /*Store the running apps*/
static ll_dsc_t app_con_ll;  /*Store the communication connection between the apps*/
static lv_obj_t * app_scr;   /*Screen of the applications*/
static lv_obj_t * menuh; 	 /*Holder of timg_bubbleshe menu on the top*/
static lv_obj_t * app_btn;   /*The "Apps" button on the menu*/
static lv_obj_t * sys_apph;  /*Holder of the system app. buttons*/
static lv_obj_t * app_list;  /*A list which is opened on 'app_btn' release*/
static lv_obj_t * sc_page;   /*A page for the shortcuts */
static lv_app_inst_t * con_send; /*The sender application in connection mode. Not NLL means connection mode is active*/
static lv_app_style_t app_style; /*Styles for application related things*/


/*Declare icons*/
LV_IMG_DECLARE(img_add);
LV_IMG_DECLARE(img_bubble);
LV_IMG_DECLARE(img_calendar);
LV_IMG_DECLARE(img_clock);
LV_IMG_DECLARE(img_close);
LV_IMG_DECLARE(img_down);
LV_IMG_DECLARE(img_driver);
LV_IMG_DECLARE(img_eject);
LV_IMG_DECLARE(img_file);
LV_IMG_DECLARE(img_folder);
LV_IMG_DECLARE(img_left);
LV_IMG_DECLARE(img_ok);
LV_IMG_DECLARE(img_play);
LV_IMG_DECLARE(img_right);
LV_IMG_DECLARE(img_settings);
LV_IMG_DECLARE(img_shut_down);
LV_IMG_DECLARE(img_star);
LV_IMG_DECLARE(img_up);
LV_IMG_DECLARE(img_user);
LV_IMG_DECLARE(img_volume);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the application system
 */
void lv_app_init(void)
{
    /*Init linked lists*/
	ll_init(&app_dsc_ll, sizeof(lv_app_dsc_t *));
	ll_init(&app_inst_ll, sizeof(lv_app_inst_t));
    ll_init(&app_con_ll, sizeof(lv_app_con_t));

	app_scr = lv_scr_act();
	lv_app_init_icons();
	lv_app_init_style();

	/*Create the desktop elements*/
	lv_app_init_desktop();

	/*Init. the utilities*/
	lv_app_kb_init();
	lv_app_notice_init();
	lv_app_fsel_init();

	/*Initialize all application descriptors*/
	/*ADD NEW APPLICATION INITS HERE!!!*/
	const lv_app_dsc_t ** dsc;
#if USE_LV_APP_EXAMPLE != 0
	dsc = ll_ins_head(&app_dsc_ll);
	*dsc = lv_app_example_init();
#endif

#if USE_LV_APP_SYSMON != 0
    dsc = ll_ins_head(&app_dsc_ll);
    *dsc = lv_app_sysmon_init();
#endif
}

/**
 * Run an application according to 'app_dsc'
 * @param app_dsc pointer to an application descriptor
 * @param cstr a Create STRing which can give initial parameters to the application (NULL or "" if unused)
 * @param conf pointer to an application specific configuration structure or NULL if unused
 * @return pointer to the opened application or NULL if any error occurred
 */
lv_app_inst_t * lv_app_run(const lv_app_dsc_t * app_dsc, const char * cstr, void * conf)
{
	/*Add a new application and initialize it*/
	lv_app_inst_t * app;
	app = ll_ins_head(&app_inst_ll);
	app->dsc = app_dsc;
	app->app_data = dm_alloc(app_dsc->app_data_size);
	app->name = NULL;
	lv_app_rename(app, app_dsc->name); /*Set a default name*/

	/*Call the application specific run function*/
	app_dsc->app_run(app, cstr, conf);

	return app;
}

/**
 * Close a running application. Close the Window and the Shortcut too if opened.
 * @param app pointer to an application
 */
void lv_app_close(lv_app_inst_t * app)
{
	lv_app_win_close(app);
	lv_app_sc_close(app);

	app->dsc->app_close(app);

	dm_free(app->app_data);
	dm_free(app->name);
}

/**
 * Open a shortcut for an application
 * @param app pointer to an application
 * @return pointer to the shortcut
 */
lv_obj_t * lv_app_sc_open(lv_app_inst_t * app)
{
	/*Save the current position of the scrollable part of the page*/
	cord_t scrl_y = lv_obj_get_y(lv_page_get_scrable(sc_page));

	/*Create a basic  shortcut*/
	app->sc = lv_btn_create(sc_page, NULL);
	lv_obj_set_free_p(app->sc, app);
	lv_obj_set_style(app->sc, &app_style.sc_style);
	lv_obj_set_opa(app->sc, app_style.sc_opa);
	lv_obj_set_size(app->sc, LV_APP_SC_WIDTH, LV_APP_SC_HEIGHT);
	lv_rect_set_layout(app->sc, LV_RECT_LAYOUT_OFF);
	lv_btn_set_rel_action(app->sc, lv_app_sc_rel_action);
    lv_btn_set_lpr_action(app->sc, lv_app_sc_lpr_action);
	lv_page_glue_obj(app->sc, true);
	if((app->dsc->mode & LV_APP_MODE_NO_SC_TITLE) == 0) {
        /*Create a title on top of the shortcut*/
        app->sc_title = lv_label_create(app->sc, NULL);
        lv_obj_set_style(app->sc_title, &app_style.sc_title_style);
    #if LV_APP_EFFECT_ANIM != 0
        lv_label_set_long_mode(app->sc_title, LV_LABEL_LONG_SCROLL);
    #else
        lv_obj_set_size(app->sc_title, LV_APP_SC_WIDTH, font_get_height(font_get(app_style.sc_title_style.font)));
        lv_label_set_long_mode(app->sc_title, LV_LABEL_LONG_DOTS);
    #endif
        lv_label_set_text(app->sc_title, app->name);
        lv_obj_align_us(app->sc_title, NULL, LV_ALIGN_IN_TOP_MID, 0, app_style.sc_title_margin);
	} else {
	    app->sc_title = NULL;
    }
	/*Allocate data and call the app specific sc_open function*/
	app->sc_data = dm_alloc(app->dsc->sc_data_size);
	app->dsc->sc_open(app, app->sc);

	/* Restore position of the scrollable part of the page because
	 * it moved when the shortcut is created*/
	lv_obj_set_y(lv_page_get_scrable(sc_page), scrl_y);
#if LV_APP_EFFECT_ANIM == 0
    lv_page_focus(sc_page, app->sc, false);
#else
    lv_page_focus(sc_page, app->sc, true);
#endif

	return app->sc;
}

/**
 * Close the shortcut of an application
 * @param app pointer to an application
 */
void lv_app_sc_close(lv_app_inst_t * app)
{
	if(app->sc == NULL) return;
	lv_obj_del(app->sc);
	app->sc = NULL;
	app->sc_title = NULL;
	dm_free(app->sc_data);
	app->sc_data = NULL;
}

/**
 * Open the application in a window
 * @param app pointer to an application
 * @return pointer to the shortcut
 */
lv_obj_t * lv_app_win_open(lv_app_inst_t * app)
{

	/*Close the app list if opened*/
	if(app_list != NULL) {
		lv_obj_del(app_list);
		app_list = NULL;
	}

	app->win = lv_win_create(lv_scr_act(), NULL);
	lv_obj_set_free_p(app->win, app);
	lv_obj_set_style(app->win, &app_style.win_style);
	lv_win_set_title(app->win, app->dsc->name);
	lv_obj_t * win_content = lv_page_get_scrable(lv_win_get_content(app->win));
	lv_rect_set_fit(win_content, false, true);
	lv_obj_set_width(win_content, LV_HOR_RES - 2 * app_style.win_style.content.bg_rects.hpad);

	lv_win_add_ctrl_btn(app->win, "U:/icon_down", lv_app_win_minim_action);
	lv_win_add_ctrl_btn(app->win, "U:/icon_close", lv_app_win_close_action);

    app->win_data = dm_alloc(app->dsc->win_data_size);
    app->dsc->win_open(app, app->win);

	return app->win;
}

/**
 * Close the window of an application
 * @param app pointer to an application
 */
void lv_app_win_close(lv_app_inst_t * app)
{
	if(app->win == NULL) return;

	lv_app_kb_close(false);

	lv_obj_del(app->win);
	app->win = NULL;
	dm_free(app->win_data);
	app->win_data = NULL;
}
/**
 * Send data to other applications
 * @param app_send pointer to the application which is sending the message
 * @param type type of data from 'lv_app_com_type_t' enum
 * @param data pointer to the sent data
 * @param len length of 'data' in bytes
 * @return number application which were received the message
 */
uint16_t lv_app_com_send(lv_app_inst_t * app_send, lv_app_com_type_t type , const void * data, uint32_t len)
{
    lv_app_con_t * con;
    uint16_t rec_cnt = 0;

    /*Add the notifications to the notice utility*/
    if(type == LV_APP_COM_TYPE_NOTICE) {
        lv_app_notice_add(data);
    }

    LL_READ(app_con_ll, con) {
        if(con->sender == app_send) {
            if(con->receiver->dsc->com_rec != NULL)
            con->receiver->dsc->com_rec(app_send, con->receiver, type, data, len);
            rec_cnt ++;
        }
    }

    return rec_cnt;
}

/**
 * Test an application communication connection
 * @param sender pointer to an application which sends data
 * @param receiver pointer to an application which receives data
 * @return false: no connection, true: there is connection
 */
bool lv_app_con_check(lv_app_inst_t * sender, lv_app_inst_t * receiver)
{
    lv_app_con_t * con;

    LL_READ(app_con_ll, con) {
        if(con->sender == sender && con->receiver == receiver) {
            return true;
        }
    }

    return false;
}

/**
 * Create a new connection between two applications
 * @param sender pointer to a data sender application
 * @param receiver pointer to a data receiver application
 */
void lv_app_con_set(lv_app_inst_t * sender, lv_app_inst_t * receiver)
{
    if(lv_app_con_check(sender, receiver) == false) {
        lv_app_con_t * con;
        con = ll_ins_head(&app_con_ll);
        con->sender = sender;
        con->receiver = receiver;
    }
}

/**
 * Delete a communication connection
 * @param sender pointer to a data sender application
 * @param receiver pointer to a data receiver application
 */
void lv_app_con_del(lv_app_inst_t * sender, lv_app_inst_t * receiver)
{
    lv_app_con_t * con;

    LL_READ(app_con_ll, con) {
        if(con->sender == sender && con->receiver == receiver) {
            ll_rem(&app_con_ll, con);
            dm_free(con);
        }
    }
}

/**
 * Get the application descriptor from its name
 * @param name name of the app. dsc.
 * @return pointer to the app. dsc.
 */
const lv_app_dsc_t * lv_app_dsc_get(const char * name)
{
    const lv_app_dsc_t ** dsc;
    LL_READ(app_dsc_ll, dsc) {
        if(strcmp((*dsc)->name, name) == 0) {
            return *dsc;
        }
    }

    return NULL;
}

/**
 * Rename an application
 * @param app pointer to an application
 * @param name a string with the new name
 */
void lv_app_rename(lv_app_inst_t * app, const char * name)
{
	dm_free(app->name);
	app->name = dm_alloc(strlen(name) + 1);
	strcpy(app->name, name);

	if(app->sc_title != NULL) {
		lv_label_set_text(app->sc_title, app->name);
	}
}

/**
 * Get the window object from an object located on the window
 * @param obj pointer to an object on the window
 * @return pointer to the window of 'obj'
 */
lv_obj_t * lv_app_win_get_from_obj(lv_obj_t * obj)
{
    lv_obj_t * par = obj;
    lv_obj_t * win;

    do {
        win = par;
        par = lv_obj_get_parent(win);
    }
    while(par != app_scr);

    return win;
}

/**
 * Read the list of the running applications. (Get he next element)
 * @param prev the previous application (at the first call give NULL to get the first application)
 * @param dsc pointer to an application descriptor to filer the applications (NULL to do not filter)
 * @return pointer to the next running application or NULL if no more
 */
lv_app_inst_t * lv_app_get_next(lv_app_inst_t * prev, lv_app_dsc_t * dsc)
{
    lv_app_inst_t * next;

    while(1) {
        if(prev == NULL) next = ll_get_head(&app_inst_ll);
        else next = ll_get_next(&app_inst_ll, prev);
        if(next == NULL) break;

        if(next->dsc == dsc || dsc == NULL) return next;

        prev = next;

    };

    return NULL;
}

/**
 * Refresh the style of the applications
 * */
void lv_app_refr_style(void)
{
    lv_style_refr_all(NULL);

    lv_obj_set_opa(menuh, app_style.menu_opa);
    lv_obj_set_opa(app_btn, app_style.menu_btn_opa);

    lv_obj_set_width(lv_page_get_scrable(sc_page),
                LV_HOR_RES - 2 * (app_style.sc_page_style.bg_rects.hpad));
}


/**
 * Get a pointer to the application style structure. If modified then 'lv_app_refr_style' should be called
 * @return pointer to the application style structure
 */
lv_app_style_t * lv_app_style_get(void)
{
    return &app_style;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Create the object on the desktop
 */
static void lv_app_init_desktop(void)
{
    /*Shortcut area*/
    sc_page = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_style(sc_page, &app_style.sc_page_style);
    lv_obj_set_size(sc_page, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(sc_page, 0, 0);
    lv_rect_set_fit(lv_page_get_scrable(sc_page), false, true);
    lv_rect_set_layout(lv_page_get_scrable(sc_page), LV_RECT_LAYOUT_GRID);
    lv_page_set_rel_action(sc_page, lv_app_sc_page_rel_action);

    /*Menu on the top*/
    menuh = lv_rect_create(lv_scr_act(), NULL);
    lv_obj_set_size(menuh, LV_HOR_RES, app_style.menu_h);
    lv_obj_set_pos(menuh, 0, 0);
    lv_obj_set_style(menuh, &app_style.menu_style);

    app_btn = lv_btn_create(menuh, NULL);
    lv_obj_set_style(app_btn, &app_style.menu_btn_style);
    lv_obj_set_height(app_btn, app_style.menu_h);
    lv_rect_set_fit(app_btn, true, false);
    lv_btn_set_rel_action(app_btn, lv_app_menu_rel_action);
    lv_obj_t * app_label = lv_label_create(app_btn, NULL);
    lv_obj_set_style(app_label, &app_style.menu_btn_label_style);
    lv_label_set_text(app_label, "Apps");
    lv_obj_set_pos(app_btn, 0, 0);

    sys_apph = lv_rect_create(menuh, NULL);
    lv_rect_set_layout(sys_apph, LV_RECT_LAYOUT_ROW_M);
    lv_rect_set_fit(sys_apph, true, false);
    lv_obj_set_height(sys_apph, app_style.menu_h);
    lv_obj_set_style(sys_apph, lv_rects_get(LV_RECTS_TRANSP, NULL));
   /* clock = lv_label_create(sys_apph, NULL);
    lv_obj_set_style(clock, &app_style.menu_btn_label_style);
    lv_label_set_text(clock, "20:17");
*/
    lv_obj_align(sys_apph, NULL, LV_ALIGN_IN_RIGHT_MID, 0, 0);

    lv_app_refr_style();
}

/*-----------------------
    APP. MENU ACTIONS
 ------------------------*/

/**
 * CAlled when the "Apps" button is released to open or close the app. list
 * @param app_btn pointer to the "Apps" button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_OK because the "Apps" button is never deleted
 */
static lv_action_res_t lv_app_menu_rel_action(lv_obj_t * app_btn, lv_dispi_t * dispi)
{
	/*Close the list if opened*/
	if(app_list != NULL) {
		lv_obj_del(app_list);
		app_list = NULL;
	}
	/*Create the app. list*/
	else {
		app_list = lv_list_create(lv_scr_act(), NULL);
		lv_obj_set_style(app_list, &app_style.app_list_style);
		lv_obj_set_opa(app_list, app_style.menu_opa);
		lv_obj_set_size(app_list, app_style.app_list_w, app_style.app_list_h);
		lv_obj_set_y(app_list, app_style.menu_h);

		lv_app_dsc_t ** dsc;
		lv_obj_t * elem;
		LL_READ(app_dsc_ll, dsc) {
		    if(((*dsc)->mode & LV_APP_MODE_NOT_LIST) == 0) {
                elem = lv_list_add(app_list, NULL, (*dsc)->name, lv_app_menu_elem_rel_action);
                lv_obj_set_free_p(elem, *dsc);
                lv_obj_set_opa(elem, app_style.menu_btn_opa);
		    }
		}
	}
	return LV_ACTION_RES_OK;
}

/**
 * Called when an element of the app list is released
 * @param app_elem_btn pointer to an element of app list
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_INV because the list is dleted on release
 */
static lv_action_res_t lv_app_menu_elem_rel_action(lv_obj_t * app_elem_btn, lv_dispi_t * dispi)
{
	lv_app_dsc_t * dsc = lv_obj_get_free_p(app_elem_btn);

    /*Close the app list*/
    lv_obj_del(app_list);
    app_list = NULL;

	lv_app_inst_t * app = lv_app_run(dsc, "", NULL);
	lv_app_sc_open(app);

#if LV_APP_EFFECT_ANIM != 0 && LV_APP_EFFECT_OPA_ANIM != 0 && LV_APP_ANIM_SC != 0
    anim_t a;
    a.act_time = 0;
    a.time = LV_APP_ANIM_SC;
    a.end_cb = NULL;
    a.playback = 0;
    a.repeat = 0;
    a.var = app->sc;
    a.path = anim_get_path(ANIM_PATH_LIN);
    a.end = app_style.sc_opa;
    a.start = OPA_TRANSP;
    a.fp = (anim_fp_t) lv_obj_set_opa;
    anim_create(&a);
#endif

	return LV_ACTION_RES_INV;
}

/*-----------------------
    SHORTCUT ACTIONS
 ------------------------*/

/**
 * Called when the shortcut page is released to hide the app list and/or
 * go back from connection mode
 * @param page pointer to the sc page
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_OK because the sc page is not deleted
 */
static lv_action_res_t lv_app_sc_page_rel_action(lv_obj_t * page, lv_dispi_t * dispi)
{
	/*Close the list if opened*/
	if(app_list != NULL) {
		lv_obj_del(app_list);
		app_list = NULL;
	}

    if(con_send != NULL) {
        lv_app_inst_t * i;
        LL_READ(app_inst_ll, i) {
            if(i->sc != NULL)  lv_obj_set_style(i->sc, &app_style.sc_style);
        }
        con_send = NULL;
    }

	return LV_ACTION_RES_OK;
}

/**
 * Called when a shortcut is released to open its window (or close app list if opened) (in normal mode) or
 * add/remove it to/form a connection (in connection mode)
 * @param sc pointer to the releases shortcut object
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_OK because the sc page is not deleted
 */
static lv_action_res_t lv_app_sc_rel_action(lv_obj_t * sc, lv_dispi_t * dispi)
{
    /*Normal mode*/
    if(con_send == NULL) {

#if LV_APP_EFFECT_ANIM == 0
        lv_page_focus(sc_page, sc, false);
#else
        lv_page_focus(sc_page, sc, true);
#endif
        /*Close the list if opened*/
        if(app_list != NULL) {
            lv_obj_del(app_list);
            app_list = NULL;
        }
        /*Else open the window of the shortcut*/
        else {
            lv_app_inst_t * app = lv_obj_get_free_p(sc);
            lv_app_win_open(app);

            lv_app_win_open_anim_create(app);
        }
    }
    /*Connection mode: toggle the connection of 'con_sender' and this app */
    else {
        lv_app_inst_t * app = lv_obj_get_free_p(sc);
        if(app != con_send) { /*Do nothing with the sender*/
            lv_btns_t * style = lv_obj_get_style(sc);
            /*Add connection to this application*/
            if(style == &app_style.sc_style) {
                lv_obj_set_style(sc, &app_style.sc_rec_style);
                lv_app_con_set(con_send, app);
            } else { /*Remove the applications connection*/
                lv_obj_set_style(sc, &app_style.sc_style);
                lv_app_con_del(con_send, app);
            }
        }
    }

	return LV_ACTION_RES_OK;
}

/**
 * Called when a shortcut id long pressed to toggle normal and connection mode
 * @param sc pointer to the long presse shortcut
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_OK because the shortcut is not deleted
 */
static lv_action_res_t lv_app_sc_lpr_action(lv_obj_t * sc, lv_dispi_t * dispi)
{
    lv_app_inst_t * app_send = lv_obj_get_free_p(sc);

    if(con_send == app_send) {
        lv_app_inst_t * i;
        LL_READ(app_inst_ll, i) {
            if(i->sc != NULL)  lv_obj_set_style(i->sc, &app_style.sc_style);
        }
        con_send = NULL;
    } else {
        if(con_send != NULL) {
            lv_app_inst_t * i;
            LL_READ(app_inst_ll, i) {
                if(i->sc != NULL)  lv_obj_set_style(i->sc, &app_style.sc_style);
            }
        }

        con_send = app_send;
        lv_obj_set_style(sc, &app_style.sc_send_style);
        lv_app_inst_t * i;
        LL_READ(app_inst_ll, i) {
            if(i->sc != NULL && lv_app_con_check(con_send, i) != false) {
                lv_obj_set_style(i->sc, &app_style.sc_rec_style);
            }
        }
    }

    return LV_ACTION_RES_OK;
}

/*-----------------------
      WINDOW ACTIONS
 ------------------------*/

/**
 * Called when the close button of window is released
 * @param close_btn pointer to the close button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_OK or LV_ACTION_RES_INC depending on LV_APP_EFFECT_... settings type
 */
static lv_action_res_t lv_app_win_close_action(lv_obj_t * close_btn, lv_dispi_t * dispi)
{
	lv_obj_t * win = lv_win_get_from_ctrl_btn(close_btn);
	lv_app_inst_t * app = lv_obj_get_free_p(win);

	lv_app_kb_close(false);

#if  LV_APP_EFFECT_ANIM != 0 && LV_APP_EFFECT_OPA != 0 && LV_APP_ANIM_WIN != 0
	lv_obj_anim(app->win, LV_ANIM_FLOAT_LEFT | ANIM_OUT, LV_APP_ANIM_WIN, 0, lv_app_win_close_anim_cb);
	lv_app_sc_close(app);
	/*The animation will close the window*/
    return LV_ACTION_RES_OK;
#else
 	lv_app_close(app);
	return LV_ACTION_RES_INV;
#endif
}

/**
 * Called when the minimization button of window is released
 * @param close_minimointer to the minim. button
 * @param dispi pointer to the caller display input
 * @return LV_ACTION_RES_OK or LV_ACTION_RES_INC depending on LV_APP_EFFECT_... settings type
 */
static lv_action_res_t lv_app_win_minim_action(lv_obj_t * close_minim, lv_dispi_t * dispi)
{
	lv_obj_t * win = lv_win_get_from_ctrl_btn(close_minim);
	lv_app_inst_t * app = lv_obj_get_free_p(win);

	lv_app_kb_close(false);

	/*Make an animation on window minimization*/
	lv_action_res_t res;
	res = lv_app_win_minim_anim_create(app);

	return res;
}

/*-----------------------
        ANIMATIONS
 ------------------------*/

/**
 * Create a window open animation
 * @param app pointer to an application
 * @return LV_ACTION_RES_OK: because the window is not deleted here
 */
static lv_action_res_t lv_app_win_open_anim_create(lv_app_inst_t * app)
{
    /*Make an animation on window open*/
#if LV_APP_EFFECT_ANIM != 0 && LV_APP_ANIM_WIN != 0

    area_t cords; /*If no shortcut simulate one or load the its coordinates*/
    if(app->sc == NULL) {
        cords.x1 = LV_HOR_RES / 2 - LV_APP_SC_WIDTH / 2;
        cords.y1 = LV_VER_RES / 2 - LV_APP_SC_HEIGHT / 2;
        cords.x2 = cords.x1 + LV_APP_SC_WIDTH;
        cords.y2 = cords.y1 + LV_APP_SC_HEIGHT;
    } else {
        lv_obj_get_cords(app->sc, &cords);
    }

    anim_t a;
    a.act_time = 0;
    a.time = LV_APP_ANIM_WIN;
    a.end_cb = NULL;
    a.playback = 0;
    a.repeat = 0;
    a.var = app->win;
    a.path = anim_get_path(ANIM_PATH_LIN);

    a.start = lv_obj_get_width(app->sc);
    a.end = LV_HOR_RES;
    a.fp = (anim_fp_t) lv_obj_set_width;
    anim_create(&a);

    a.start = lv_obj_get_height(app->sc);
    a.end = LV_VER_RES;
    a.fp = (anim_fp_t) lv_obj_set_height;
    anim_create(&a);

    a.start = cords.x1;
    a.end = 0;
    a.fp = (anim_fp_t) lv_obj_set_x;
    anim_create(&a);

    a.start = cords.y1;
    a.end = 0;
    a.fp = (anim_fp_t) lv_obj_set_y;
    anim_create(&a);

#endif /*LV_APP_EFFECT_ANIM != 0 && LV_APP_ANIM_WIN != 0*/

    return LV_ACTION_RES_OK;
}

/**
 * Create a window minimization animation
 * @param app pointer to an application
 * @return LV_ACTION_RES_OK or LV_ACTION_RES_INV depending on LV_APP_EFFECT_... settings
 */
static lv_action_res_t lv_app_win_minim_anim_create(lv_app_inst_t * app)
{
#if LV_APP_EFFECT_ANIM != 0 && LV_APP_ANIM_WIN != 0
    area_t cords;
    if(app->sc == NULL) {
        cords.x1 = LV_HOR_RES / 2 - LV_APP_SC_WIDTH / 2;
        cords.y1 = LV_VER_RES / 2 - LV_APP_SC_HEIGHT / 2;
        cords.x2 = cords.x1 + LV_APP_SC_WIDTH;
        cords.y2 = cords.y1 + LV_APP_SC_HEIGHT;
    } else {
        lv_obj_get_cords(app->sc, &cords);
    }

    anim_t a;
    a.act_time = 0;
    a.time = LV_APP_ANIM_WIN;
    a.end_cb = NULL;
    a.playback = 0;
    a.repeat = 0;
    a.var = app->win;
    a.path = anim_get_path(ANIM_PATH_LIN);


    a.end = lv_obj_get_width(app->sc);
    a.start = LV_HOR_RES;
    a.fp = (anim_fp_t) lv_obj_set_width;
    anim_create(&a);

    a.end = lv_obj_get_height(app->sc);
    a.start = LV_VER_RES;
    a.fp = (anim_fp_t) lv_obj_set_height;
    anim_create(&a);

    a.end = cords.x1;
    a.start = 0;
    a.fp = (anim_fp_t) lv_obj_set_x;
    anim_create(&a);

    a.end = cords.y1;
    a.start = 0;
    a.fp = (anim_fp_t) lv_obj_set_y;
    a.end_cb = (void (*)(void *))lv_app_win_close_anim_cb;
    anim_create(&a);

    return LV_ACTION_RES_OK;
#else /*LV_APP_ANIM_WIN == 0 || LV_APP_ANIM_LEVEL == 0*/
    lv_app_win_close(app);
    return LV_ACTION_RES_INV;
#endif
}

#if LV_APP_EFFECT_ANIM != 0
/**
 * Called when the window close or minimization animation is ready to close the window
 * @param app_win pointer to a window
 */
static void lv_app_win_close_anim_cb(lv_obj_t * app_win)
{
    lv_app_inst_t * app = lv_obj_get_free_p(app_win);
    lv_app_win_close(app);
}
#endif

/**
 * Init the application styles
 */
static void lv_app_init_style(void)
{
	/*Coordinates*/
	app_style.menu_h = 40 * LV_DOWNSCALE;
	app_style.app_list_w = LV_HOR_RES / 3;
	app_style.app_list_h = (2 * LV_VER_RES) / 3;
	app_style.sc_title_margin = 2 * LV_DOWNSCALE;

	/*Opacity*/
#if LV_APP_EFFECT_OPA == 0
    app_style.menu_opa = OPA_COVER;
    app_style.menu_btn_opa = OPA_COVER;
    app_style.sc_opa = OPA_COVER;
#else
    app_style.menu_opa = OPA_80;
    app_style.menu_btn_opa = OPA_50;
    app_style.sc_opa = OPA_80;
#endif

	/*Menu style*/
	lv_rects_get(LV_RECTS_DEF,&app_style.menu_style);
	app_style.menu_style.objs.color = COLOR_BLACK;
	app_style.menu_style.gcolor = COLOR_BLACK;
	app_style.menu_style.round = 0;
	app_style.menu_style.bwidth = 0;
	app_style.menu_style.light = 0;

	lv_btns_get(LV_BTNS_DEF,&app_style.menu_btn_style);
	memcpy(&app_style.menu_btn_style.rects, &app_style.menu_style, sizeof(lv_rects_t));
	app_style.menu_btn_style.flags[LV_BTN_STATE_REL].light_en = 0;
	app_style.menu_btn_style.flags[LV_BTN_STATE_PR].light_en = 0;

	app_style.menu_btn_style.flags[LV_BTN_STATE_REL].empty = 1;
	app_style.menu_btn_style.flags[LV_BTN_STATE_PR].empty = 0;

	app_style.menu_btn_style.mcolor[LV_BTN_STATE_REL] = COLOR_BLACK;
	app_style.menu_btn_style.gcolor[LV_BTN_STATE_REL] = COLOR_BLACK;
	app_style.menu_btn_style.mcolor[LV_BTN_STATE_PR] = COLOR_GRAY;
	app_style.menu_btn_style.gcolor[LV_BTN_STATE_PR] = COLOR_GRAY;

	lv_labels_get(LV_LABELS_BTN,&app_style.menu_btn_label_style);
	app_style.menu_btn_label_style.font = LV_APP_FONT_LARGE;
	app_style.menu_btn_label_style.objs.color = COLOR_MAKE(0xd0, 0xe0, 0xf0);

	lv_imgs_get(LV_IMGS_DEF,&app_style.menu_btn_img_style);
	app_style.menu_btn_img_style.objs.color = COLOR_WHITE;
	app_style.menu_btn_img_style.recolor_opa = OPA_90;

	/*App list styles*/
	lv_lists_get(LV_LISTS_DEF,&app_style.app_list_style);
	memcpy(&app_style.app_list_style.liste_btns, &app_style.menu_btn_style, sizeof(lv_btns_t));
	memcpy(&app_style.app_list_style.bg_pages.bg_rects, &app_style.menu_style, sizeof(lv_rects_t));
	memcpy(&app_style.app_list_style.liste_labels, &app_style.menu_btn_label_style, sizeof(lv_labels_t));
	app_style.app_list_style.bg_pages.bg_rects.vpad = 0;
	app_style.app_list_style.bg_pages.bg_rects.hpad = 0;
	app_style.app_list_style.bg_pages.bg_rects.opad = 0;
	app_style.app_list_style.bg_pages.scrable_rects.objs.transp = 1;
	app_style.app_list_style.bg_pages.scrable_rects.vpad = 0;
	app_style.app_list_style.bg_pages.scrable_rects.hpad = 0;
	app_style.app_list_style.bg_pages.scrable_rects.opad = 0;
	app_style.app_list_style.bg_pages.sb_rects.objs.color = COLOR_GRAY;
	app_style.app_list_style.bg_pages.sb_rects.gcolor = COLOR_GRAY;
	app_style.app_list_style.bg_pages.sb_width = 8 * LV_DOWNSCALE;

	/*Shortcut page styles*/
	lv_pages_get(LV_PAGES_DEF,&app_style.sc_page_style);
	app_style.sc_page_style.bg_rects.empty = 1;
	app_style.sc_page_style.bg_rects.round = 0;
	app_style.sc_page_style.bg_rects.bwidth = 0;
	app_style.sc_page_style.bg_rects.vpad = app_style.menu_h;
	app_style.sc_page_style.bg_rects.hpad = 0;
	app_style.sc_page_style.bg_rects.opad = 0;
	app_style.sc_page_style.scrable_rects.objs.transp = 1;
	app_style.sc_page_style.scrable_rects.hpad = 20 * LV_DOWNSCALE;
	app_style.sc_page_style.scrable_rects.vpad = 20 * LV_DOWNSCALE;
	app_style.sc_page_style.scrable_rects.opad = 20 * LV_DOWNSCALE;

	/*Shortcut styles*/
	lv_btns_get(LV_BTNS_DEF,&app_style.sc_style);
	app_style.sc_style.mcolor[LV_BTN_STATE_REL] = COLOR_WHITE;
	app_style.sc_style.gcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0x20, 0x30, 0x40);
	app_style.sc_style.bcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0x40, 0x60, 0x80);
	app_style.sc_style.mcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0xB0, 0xD0, 0xF0);
	app_style.sc_style.gcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0x00, 0x00, 0x00);
	app_style.sc_style.bcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0xB0, 0xD0, 0xF0);
	app_style.sc_style.rects.bopa = 70;
	app_style.sc_style.rects.bwidth = 1 * LV_DOWNSCALE;

	memcpy(&app_style.sc_send_style, &app_style.sc_style, sizeof(lv_btns_t));
    app_style.sc_send_style.mcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0xFF, 0xE0, 0xE0);
    app_style.sc_send_style.gcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0x50, 0x20, 0x00);
    app_style.sc_send_style.bcolor[LV_BTN_STATE_REL] = COLOR_BLACK;
    app_style.sc_send_style.flags[LV_BTN_STATE_REL].light_en = 1;
    app_style.sc_send_style.mcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0xFF, 0xB0, 0xB0);
    app_style.sc_send_style.gcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0x20, 0x10, 0x00);
    app_style.sc_send_style.bcolor[LV_BTN_STATE_PR] = COLOR_BLACK;
    app_style.sc_send_style.flags[LV_BTN_STATE_PR].light_en = 1;
    app_style.sc_send_style.rects.light = 10 * LV_DOWNSCALE;
    app_style.sc_send_style.rects.bopa = 30;
    app_style.sc_send_style.rects.bwidth = 3 * LV_DOWNSCALE;

    memcpy(&app_style.sc_rec_style, &app_style.sc_style, sizeof(lv_btns_t));
    app_style.sc_rec_style.mcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0xE0, 0xFF, 0xE0);
    app_style.sc_rec_style.gcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0x20, 0x50, 0x20);
    app_style.sc_rec_style.bcolor[LV_BTN_STATE_REL] = COLOR_BLACK;
    app_style.sc_rec_style.flags[LV_BTN_STATE_REL].light_en = 1;
    app_style.sc_rec_style.mcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0xB0, 0xFF, 0xB0);
    app_style.sc_rec_style.gcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0x20, 0x20, 0x10);
    app_style.sc_rec_style.bcolor[LV_BTN_STATE_PR] = COLOR_BLACK;
    app_style.sc_rec_style.flags[LV_BTN_STATE_PR].light_en = 1;
    app_style.sc_rec_style.rects.light = 10 * LV_DOWNSCALE;
    app_style.sc_rec_style.rects.bopa = 30;
    app_style.sc_rec_style.rects.bwidth = 3 * LV_DOWNSCALE;

	lv_labels_get(LV_LABELS_DEF,&app_style.sc_title_style);
	app_style.sc_title_style.font = LV_APP_FONT_SMALL;
	app_style.sc_title_style.objs.color = COLOR_MAKE(0x10, 0x20, 0x30);
	app_style.sc_title_style.mid = 1;

	lv_labels_get(LV_LABELS_DEF,&app_style.sc_txt_style);
    app_style.sc_txt_style.font = LV_APP_FONT_MEDIUM;
    app_style.sc_txt_style.objs.color = COLOR_MAKE(0x20, 0x30, 0x40);
    app_style.sc_txt_style.mid = 0;

	/*Window styles*/
	lv_wins_get(LV_WINS_DEF,&app_style.win_style);
	memcpy(&app_style.win_style.header, &app_style.menu_style, sizeof(lv_rects_t));
	memcpy(&app_style.win_style.title, &app_style.menu_btn_label_style, sizeof(lv_labels_t));
	memcpy(&app_style.win_style.ctrl_btn, &app_style.menu_btn_style, sizeof(lv_btns_t));
	memcpy(&app_style.win_style.ctrl_img, &app_style.menu_btn_img_style, sizeof(lv_imgs_t));
	app_style.win_style.header_on_content = 1;
	app_style.win_style.header_opa = app_style.menu_opa;
	app_style.win_style.ctrl_btn_opa = app_style.menu_btn_opa;
	app_style.win_style.header.vpad = 5 * LV_DOWNSCALE;
	app_style.win_style.header.hpad = 5 * LV_DOWNSCALE;
	app_style.win_style.header.opad = 5 * LV_DOWNSCALE;
	app_style.win_style.content.bg_rects.vpad = app_style.win_style.ctrl_btn_h +
			                                    2 * app_style.win_style.header.vpad;
	app_style.win_style.content.bg_rects.hpad = 5 * LV_DOWNSCALE;
	app_style.win_style.content.scrable_rects.objs.transp = 1;

    lv_labels_get(LV_LABELS_DEF,&app_style.win_txt_style);
    app_style.win_txt_style.font = LV_APP_FONT_MEDIUM;
    app_style.win_txt_style.objs.color = COLOR_MAKE(0x20, 0x20, 0x20);
    app_style.win_txt_style.mid = 0;
    app_style.win_txt_style.letter_space = 1 * LV_DOWNSCALE;
}

/**
 * Create files for the icons
 */
static void lv_app_init_icons(void)
{
    lv_img_create_file("icon_add", img_add);
    lv_img_create_file("icon_bubble", img_bubble);
    lv_img_create_file("icon_calendar", img_calendar);
    lv_img_create_file("icon_clock", img_clock);
    lv_img_create_file("icon_close", img_close);
    lv_img_create_file("icon_down", img_down);
    lv_img_create_file("icon_driver", img_driver);
    lv_img_create_file("icon_eject", img_eject);
    lv_img_create_file("icon_file", img_file);
    lv_img_create_file("icon_folder", img_folder);
    lv_img_create_file("icon_left", img_left);
    lv_img_create_file("icon_ok", img_ok);
    lv_img_create_file("icon_play", img_play);
    lv_img_create_file("icon_right", img_right);
    lv_img_create_file("icon_settings", img_settings);
    lv_img_create_file("icon_shut_down", img_shut_down);
    lv_img_create_file("icon_star", img_star);
    lv_img_create_file("icon_up", img_up);
    lv_img_create_file("icon_user", img_user);
    lv_img_create_file("icon_volume", img_volume);
}
#endif /*LV_APP_ENABLE != 0*/


