/**
 * @file lv_app_benchmark.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_benchmark.h"
#if LV_APP_ENABLE != 0 && USE_LV_APP_BENCHMARK != 0

#include "../lv_app/lv_app_util/lv_app_kb.h"
#include "lvgl/lv_obj/lv_refr.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define SHADOW_WIDTH    (LV_DPI / 8)
#define IMG_RECOLOR     OPA_30
#define OPACITY         OPA_60


/**********************
 *      TYPEDEFS
 **********************/

/*Application specific data for an instance of this application*/
typedef struct
{
    uint8_t wp          :1;
    uint8_t recolor     :1;
    uint8_t upscalse    :1;
    uint8_t shdw      :1;
    uint8_t opa         :1;
}my_app_data_t;

/*Application specific data a window of this application*/
typedef struct
{
    lv_obj_t * wp;
    lv_obj_t * value_l;
    lv_style_t style_wp;
    lv_style_t style_value_l;
    lv_style_t style_btn_rel;
    lv_style_t style_btn_pr;
    lv_style_t style_btn_trel;
    lv_style_t style_btn_tpr;
    lv_style_t style_btn_ina;
}my_win_data_t;

/*Application specific data for a shortcut of this application*/
typedef struct
{
    lv_obj_t * label;
}my_sc_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void my_app_run(lv_app_inst_t * app, void * conf);
static void my_app_close(lv_app_inst_t * app);
static void my_com_rec(lv_app_inst_t * app_send, lv_app_inst_t * app_rec, lv_app_com_type_t type , const void * data, uint32_t size);
static void my_sc_open(lv_app_inst_t * app, lv_obj_t * sc);
static void my_sc_close(lv_app_inst_t * app);
static void my_win_open(lv_app_inst_t * app, lv_obj_t * win);
static void my_win_close(lv_app_inst_t * app);

static void refr_monitor(uint32_t time_ms, uint32_t px_num);
static lv_action_res_t run_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc);
static lv_action_res_t wp_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc);
static lv_action_res_t recolor_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc);
static lv_action_res_t upscale_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc);
static lv_action_res_t shadow_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc);
static lv_action_res_t opa_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc);
/**********************
 *  STATIC VARIABLES
 **********************/
static lv_app_dsc_t my_app_dsc =
{
	.name = "Benchmark",
	.mode = LV_APP_MODE_NONE,
	.app_run = my_app_run,
	.app_close = my_app_close,
	.com_rec = my_com_rec,
	.win_open = my_win_open,
	.win_close = my_win_close,
	.sc_open = my_sc_open,
	.sc_close = my_sc_close,
	.app_data_size = sizeof(my_app_data_t),
	.sc_data_size = sizeof(my_sc_data_t),
	.win_data_size = sizeof(my_win_data_t),
};

static lv_style_t style_win_scrl;
static lv_style_t style_sc_btn_rel;
static lv_style_t style_sc_btn_pr;

static bool caputre_next;

static const color_int_t img_benchmark_bg[];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the application
 * @return pointer to the application descriptor of this application
 */
const lv_app_dsc_t * lv_app_benchmark_init(void)
{
    lv_refr_set_monitor_cb(refr_monitor);

    lv_img_create_file("app_bm_wp", img_benchmark_bg);

    lv_style_get(LV_STYLE_TRANSP, &style_win_scrl);
    style_win_scrl.opad = 2 * SHADOW_WIDTH + SHADOW_WIDTH / 8 ;
    style_win_scrl.hpad = SHADOW_WIDTH;
    style_win_scrl.vpad = SHADOW_WIDTH;

    lv_style_get(LV_STYLE_BTN_REL, &style_sc_btn_rel);
    style_sc_btn_rel.font = font_get(LV_IMG_DEF_SYMBOL_FONT);


    lv_style_get(LV_STYLE_BTN_PR, &style_sc_btn_pr);
    style_sc_btn_pr.font = font_get(LV_IMG_DEF_SYMBOL_FONT);

	return &my_app_dsc;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Run an application according to 'app_dsc'
 * @param app_dsc pointer to an application descriptor
 * @param conf pointer to a lv_app_benchmark_conf_t structure with configuration data or NULL if unused
 * @return pointer to the opened application or NULL if any error occurred
 */
static void my_app_run(lv_app_inst_t * app, void * conf)
{
    /*Initialize the application*/
	my_app_data_t * ad = app->app_data;
	ad->opa = 0;
	ad->recolor = 0;
	ad->shdw = 0;
	ad->upscalse = 0;
	ad->wp = 0;


}

/**
 * Close a running application.
 * Close the Window and the Shortcut too if opened.
 * Free all the allocated memory by this application.
 * @param app pointer to an application
 */
static void my_app_close(lv_app_inst_t * app)
{
    /*No dynamically allocated data in 'my_app_data'*/
}

/**
 * Read the data have been sent to this application
 * @param app_send pointer to an application which sent the message
 * @param app_rec pointer to an application which is receiving the message
 * @param type type of data from 'lv_app_com_type_t' enum
 * @param data pointer to the sent data
 * @param size length of 'data' in bytes
 */
static void my_com_rec(lv_app_inst_t * app_send, lv_app_inst_t * app_rec,
                       lv_app_com_type_t type , const void * data, uint32_t size)
{
	if(type == LV_APP_COM_TYPE_CHAR) {      /*data: string*/
	    my_sc_data_t * sc_data = app_rec->sc_data;
	    if (sc_data->label != NULL) {
	        lv_label_set_text_array(sc_data->label, data, size);
	        lv_obj_align(sc_data->label , NULL,LV_ALIGN_CENTER, 0, 0);
	    }
	}
}

/**
 * Open a shortcut for an application
 * @param app pointer to an application
 * @param sc pointer to an object where the application
 *           can create content of the shortcut
 */
static void my_sc_open(lv_app_inst_t * app, lv_obj_t * sc)
{
    my_sc_data_t * sc_data = app->sc_data;

    lv_cont_set_layout(sc, LV_CONT_LAYOUT_CENTER);

    sc_data->label = lv_label_create(sc, NULL);
	lv_label_set_text(sc_data->label, "N/A ms");

	lv_obj_t * btn = lv_btn_create(sc, NULL);
	lv_btn_set_rel_action(btn, run_rel_action);
	lv_cont_set_fit(btn, true, true);
	lv_btn_set_styles(btn, &style_sc_btn_rel, &style_sc_btn_pr, NULL, NULL, NULL);

	lv_obj_t * btn_l = lv_label_create(btn, NULL);
	lv_label_set_text(btn_l, SYMBOL_PLAY);
}

/**
 * Close the shortcut of an application
 * @param app pointer to an application
 */
static void my_sc_close(lv_app_inst_t * app)
{
    /*No dynamically allocated data in 'my_sc_data'*/
}


/**
 * Open the application in a window
 * @param app pointer to an application
 * @param win pointer to a window object where
 *            the application can create content
 */
static void my_win_open(lv_app_inst_t * app, lv_obj_t * win)
{
    my_win_data_t * wdata = app->win_data;
    my_app_data_t * adata = app->app_data;

    lv_style_get(LV_STYLE_BTN_INA, &wdata->style_value_l);
    wdata->style_value_l.ccolor = COLOR_BLACK;
    lv_style_get(LV_STYLE_BTN_REL, &wdata->style_btn_rel);
    lv_style_get(LV_STYLE_BTN_PR, &wdata->style_btn_pr);
    lv_style_get(LV_STYLE_BTN_TREL, &wdata->style_btn_trel);
    lv_style_get(LV_STYLE_BTN_TPR, &wdata->style_btn_tpr);
    lv_style_get(LV_STYLE_BTN_INA, &wdata->style_btn_ina);
    lv_style_get(LV_STYLE_PLAIN, &wdata->style_wp);
    wdata->style_wp.ccolor = COLOR_RED;//MAKE(0x10, 0x20, 0x30);

    if(adata->opa == 0) {
        wdata->style_btn_rel.opa = OPA_COVER;
        wdata->style_btn_pr.opa  = OPA_COVER;
        wdata->style_btn_trel.opa =OPA_COVER;
        wdata->style_btn_tpr.opa = OPA_COVER;
        wdata->style_btn_ina.opa = OPA_COVER;
    } else {
        wdata->style_btn_rel.opa = OPACITY;
        wdata->style_btn_pr.opa  = OPACITY;
        wdata->style_btn_trel.opa =OPACITY;
        wdata->style_btn_tpr.opa = OPACITY;
        wdata->style_btn_ina.opa = OPACITY;
    }

    if(adata->shdw == 0) {
        wdata->style_btn_rel.swidth =  0;
        wdata->style_btn_pr.swidth  =  0;
        wdata->style_btn_trel.swidth = 0;
        wdata->style_btn_tpr.swidth =  0;
        wdata->style_btn_ina.swidth =  0;
    } else {
        wdata->style_btn_rel.swidth =  SHADOW_WIDTH;
        wdata->style_btn_pr.swidth  =  SHADOW_WIDTH;
        wdata->style_btn_trel.swidth = SHADOW_WIDTH;
        wdata->style_btn_tpr.swidth =  SHADOW_WIDTH;
        wdata->style_btn_ina.swidth =  SHADOW_WIDTH;
    }

    if(adata->recolor == 0) {
        wdata->style_wp.img_recolor =  OPA_TRANSP;
    } else {
        wdata->style_wp.img_recolor =  IMG_RECOLOR;
    }

    lv_obj_set_style(lv_page_get_scrl(lv_win_get_page(win)), &style_win_scrl);

	wdata->wp = lv_img_create(win, NULL);
	lv_obj_set_protect(wdata->wp, LV_PROTECT_PARENT);
	lv_obj_set_parent(wdata->wp, lv_win_get_page(win));
	lv_img_set_file(wdata->wp, "U:/app_bm_wp");
	lv_obj_set_size(wdata->wp, LV_HOR_RES, LV_VER_RES - lv_obj_get_height(lv_win_get_header(win)));
	lv_obj_set_pos(wdata->wp, 0, 0);
	lv_obj_set_style(wdata->wp, &wdata->style_wp);
    lv_img_set_auto_size(wdata->wp, false);
	if(adata->wp == 0) lv_obj_set_hidden(wdata->wp, true);
    if(adata->upscalse != 0) lv_img_set_upscale(wdata->wp, true);

	/* The order is changed because the wallpaper's parent change
	 * Therefore add the scrollable again */
	lv_obj_set_parent(lv_page_get_scrl(lv_win_get_page(win)), lv_win_get_page(win));

	lv_cont_set_layout(lv_page_get_scrl(lv_win_get_page(win)), LV_CONT_LAYOUT_PRETTY);

	lv_obj_t * holder;

	holder = lv_cont_create(win, NULL);
	lv_cont_set_fit(holder, true, true);
	lv_obj_set_style(holder, &wdata->style_btn_ina);
	lv_page_glue_obj(holder, true);

    wdata->value_l = lv_label_create(holder, NULL);
    lv_obj_set_style(wdata->value_l, &wdata->style_value_l);
	lv_label_set_text(wdata->value_l, "Screen load: N/A ms\nN/A px/ms");

	lv_obj_t * btn;
	btn = lv_btn_create(win, NULL);
	lv_obj_set_free_p(btn, app);
    lv_page_glue_obj(btn, true);
	lv_cont_set_fit(btn, true, true);
	lv_btn_set_styles(btn, &wdata->style_btn_rel, &wdata->style_btn_pr, &wdata->style_btn_trel, &wdata->style_btn_tpr, NULL);
	lv_btn_set_rel_action(btn, run_rel_action);

	lv_obj_t * btn_l;
	btn_l = lv_label_create(btn, NULL);
	lv_label_set_text(btn_l, "Run\ntest!");
	lv_obj_set_protect(btn, LV_PROTECT_FOLLOW);

    btn = lv_btn_create(win, btn);
    lv_btn_set_tgl(btn, true);
    lv_obj_clr_protect(btn, LV_PROTECT_FOLLOW);
    if(adata->wp != 0) lv_btn_set_state(btn, LV_BTN_STATE_TREL);
    else lv_btn_set_state(btn, LV_BTN_STATE_REL);
    lv_btn_set_rel_action(btn, wp_rel_action);
    btn_l = lv_label_create(btn, btn_l);
    lv_label_set_text(btn_l, "Wallpaper");


    btn = lv_btn_create(win, btn);
    if(adata->recolor != 0) lv_btn_set_state(btn, LV_BTN_STATE_TREL);
    else lv_btn_set_state(btn, LV_BTN_STATE_REL);
    lv_btn_set_rel_action(btn, recolor_rel_action);
    btn_l = lv_label_create(btn, btn_l);
    lv_label_set_text(btn_l, "Wp. recolor!");


    btn = lv_btn_create(win, btn);
    if(adata->upscalse != 0) lv_btn_set_state(btn, LV_BTN_STATE_TREL);
    else lv_btn_set_state(btn, LV_BTN_STATE_REL);
    lv_btn_set_rel_action(btn, upscale_rel_action);
    btn_l = lv_label_create(btn, btn_l);
    lv_label_set_text(btn_l, "Wp. upscalse!");


    btn = lv_btn_create(win, btn);
    if(adata->shdw != 0) lv_btn_set_state(btn, LV_BTN_STATE_TREL);
    else lv_btn_set_state(btn, LV_BTN_STATE_REL);
    lv_btn_set_rel_action(btn, shadow_rel_action);
    btn_l = lv_label_create(btn, btn_l);
    lv_label_set_text(btn_l, "Shadow");

    btn = lv_btn_create(win, btn);
    if(adata->opa != 0) lv_btn_set_state(btn, LV_BTN_STATE_TREL);
    else lv_btn_set_state(btn, LV_BTN_STATE_REL);
    lv_btn_set_rel_action(btn, opa_rel_action);
    btn_l = lv_label_create(btn, btn_l);
    lv_label_set_text(btn_l, "Opacity");

}

/**
 * Close the window of an application
 * @param app pointer to an application
 */
static void my_win_close(lv_app_inst_t * app)
{

}

/*--------------------
 * OTHER FUNCTIONS
 ---------------------*/

static void refr_monitor(uint32_t time_ms, uint32_t px_num)
{
    if(caputre_next != false) {
        lv_app_inst_t * app = NULL;
        app = lv_app_get_next(app, &my_app_dsc);

        char w_buf[256];
        if(time_ms != 0) sprintf(w_buf, "Screen load: %d ms\n%d px/ms", time_ms, px_num/time_ms);
        else sprintf(w_buf, "Screen load: %d ms\nN/A px/ms", time_ms);

        char s_buf[16];
        sprintf(s_buf, "%d ms", time_ms);

        while(app != NULL) {
            if(app->win_data != NULL) {
               my_win_data_t * wdata = app->win_data;
               lv_label_set_text(wdata->value_l, w_buf);
            }

            if(app->sc_data != NULL) {
               my_sc_data_t * sdata = app->sc_data;
               lv_label_set_text(sdata->label, s_buf);
            }

            app = lv_app_get_next(app, &my_app_dsc);
        }


        caputre_next = false;
    }
}

static lv_action_res_t run_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc)
{
    lv_obj_inv(lv_scr_act());
    caputre_next = true;

    return LV_ACTION_RES_OK;
}

static lv_action_res_t wp_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc)
{

    lv_app_inst_t * app = lv_obj_get_free_p(btn);
    my_win_data_t * wdata = app->win_data;
    my_app_data_t * adata = app->app_data;

    if(lv_btn_get_state(btn) == LV_BTN_STATE_TREL) {
        adata->wp = 1;
        lv_obj_set_hidden(wdata->wp, false);
    } else {
        adata->wp = 0;
        lv_obj_set_hidden(wdata->wp, true);
    }

    return LV_ACTION_RES_OK;
}

static lv_action_res_t recolor_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(btn);
    my_win_data_t * wdata = app->win_data;
    my_app_data_t * adata = app->app_data;

    if(lv_btn_get_state(btn) == LV_BTN_STATE_TREL) {
        adata->recolor = 1;
        wdata->style_wp.img_recolor = IMG_RECOLOR;
    } else {
        adata->recolor = 0;
        wdata->style_wp.img_recolor = OPA_TRANSP;
    }

    lv_obj_refr_style(wdata->wp);

    return LV_ACTION_RES_OK;
}

static lv_action_res_t upscale_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(btn);
    my_win_data_t * wdata = app->win_data;
    my_app_data_t * adata = app->app_data;

    if(lv_btn_get_state(btn) == LV_BTN_STATE_TREL) {
        adata->upscalse = 1;
        lv_img_set_upscale(wdata->wp, true);
    } else {
        adata->upscalse = 0;
        lv_img_set_upscale(wdata->wp, false);
    }

    lv_obj_set_size(wdata->wp, LV_HOR_RES, LV_VER_RES - lv_obj_get_height(lv_win_get_header(app->win)));

    return LV_ACTION_RES_OK;
}

static lv_action_res_t shadow_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(btn);
    my_win_data_t * wdata = app->win_data;
    my_app_data_t * adata = app->app_data;

    if(lv_btn_get_state(btn) == LV_BTN_STATE_TREL) {
        adata->shdw = 1;
        wdata->style_btn_rel.swidth = SHADOW_WIDTH;
        wdata->style_btn_pr.swidth  =  SHADOW_WIDTH;
        wdata->style_btn_trel.swidth = SHADOW_WIDTH;
        wdata->style_btn_tpr.swidth =  SHADOW_WIDTH;
        wdata->style_btn_ina.swidth =  SHADOW_WIDTH;
    } else {
        adata->shdw = 0;
        wdata->style_btn_rel.swidth = 0;
        wdata->style_btn_pr.swidth  = 0;
        wdata->style_btn_trel.swidth =0;
        wdata->style_btn_tpr.swidth = 0;
        wdata->style_btn_ina.swidth = 0;
    }

    lv_style_refr_objs(&wdata->style_btn_rel);
    lv_style_refr_objs(&wdata->style_btn_pr);
    lv_style_refr_objs(&wdata->style_btn_trel);
    lv_style_refr_objs(&wdata->style_btn_tpr);
    lv_style_refr_objs(&wdata->style_btn_ina);

    return LV_ACTION_RES_OK;
}

static lv_action_res_t opa_rel_action(lv_obj_t * btn, lv_indev_proc_t * indev_proc)
{
    lv_app_inst_t * app = lv_obj_get_free_p(btn);
    my_win_data_t * wdata = app->win_data;
    my_app_data_t * adata = app->app_data;

    if(lv_btn_get_state(btn) == LV_BTN_STATE_TREL) {
        adata->opa = 1;
        wdata->style_btn_rel.opa = OPACITY;
        wdata->style_btn_pr.opa  = OPACITY;
        wdata->style_btn_trel.opa =OPACITY;
        wdata->style_btn_tpr.opa = OPACITY;
        wdata->style_btn_ina.opa = OPACITY;
    } else {
        adata->opa = 0;
        wdata->style_btn_rel.opa = OPA_COVER;
        wdata->style_btn_pr.opa  = OPA_COVER;
        wdata->style_btn_trel.opa =OPA_COVER;
        wdata->style_btn_tpr.opa = OPA_COVER;
        wdata->style_btn_ina.opa = OPA_COVER;
    }

    lv_style_refr_objs(&wdata->style_btn_rel);
    lv_style_refr_objs(&wdata->style_btn_pr);
    lv_style_refr_objs(&wdata->style_btn_trel);
    lv_style_refr_objs(&wdata->style_btn_tpr);
    lv_style_refr_objs(&wdata->style_btn_ina);

    return LV_ACTION_RES_OK;
}


/*Exceptionally store the data because the big array would be bothering*/

#if COLOR_DEPTH == 8
static const color_int_t img_benchmark_bg[] = {
/*HEADER
   Width = 40
   Height = 40
   Transp: 0
   Color depth: 8*/
40, 128, 2, 2,

/*IMAGE DATA*/
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 5, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 5, 5, 0, 5, 5, 5, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 5, 5, 0, 5, 5, 0, 0, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 5, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 5, 0, 0, 5, 5, 5, 5, 5, 5, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 5, 5, 0, 0, 0, 5, 5, 5, 0, 5, 5, 5, 5, 0, 0, 5, 5, 5, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 5, 5, 5, 5, 5, 5, 5, 5, 0, 5, 5, 5, 5, 5, 0, 5, 5, 5, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 5, 5, 5, 0, 5, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 5, 5, 5, 5, 0, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 5, 5, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 5, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
#endif /*COLOR_DEPTH*/

#if COLOR_DEPTH == 16
static const color_int_t img_benchmark_bg[] = {
/*HEADER
   Width = 40
   Height = 40
   Transp: 0
   Color depth: 16*/
32808, 1026,

/*IMAGE DATA*/
32, 32, 32, 2113, 2113, 4258, 6339, 4258, 4258, 4226, 4226, 2145, 4226, 2113, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 2145, 2145, 4226, 4226, 4226, 4226, 4226, 2145, 32, 32, 32, 32,
0, 32, 2113, 32, 6339, 10565, 8452, 4258, 4226, 4226, 2145, 4226, 6339, 4226, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 2145, 4226, 2145, 2145, 4226, 4226, 4226, 6371, 6339, 4226, 32, 32, 32,
32, 32, 2113, 4226, 8484, 8484, 8452, 6371, 4226, 2145, 4226, 4258, 4258, 4258, 2145, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 2145, 4226, 4226, 4226, 4226, 2145, 2145, 4226, 6339, 4258, 4258, 4226, 32, 32,
32, 32, 4258, 6371, 8452, 6371, 6371, 6339, 4226, 2145, 4226, 4226, 2145, 4226, 4226, 2113, 32, 0, 0, 0, 0, 32, 0, 0, 32, 2113, 2145, 2145, 2145, 4226, 4226, 4226, 2145, 4226, 6339, 4226, 6371, 6339, 2145, 32,
2113, 2145, 6371, 6339, 4226, 4226, 4226, 4226, 4258, 4258, 4226, 2145, 4226, 2145, 2145, 4226, 4258, 32, 0, 0, 0, 0, 0, 2113, 6339, 4226, 2113, 2145, 2145, 2145, 4226, 4226, 4258, 4226, 4258, 4258, 4258, 6339, 6339, 2145,
4226, 4258, 6339, 4226, 4226, 2145, 2145, 2145, 4226, 4226, 4226, 2145, 2145, 2145, 4226, 6339, 8452, 6339, 32, 0, 0, 0, 2113, 6339, 6371, 6339, 2145, 2113, 2145, 2145, 4226, 4226, 4226, 4226, 4226, 4226, 4226, 4258, 4258, 4226,
4226, 4226, 2145, 4226, 2145, 2145, 4226, 4226, 2145, 4226, 4226, 4226, 2145, 4226, 6371, 6371, 6371, 8452, 4258, 32, 0, 2113, 6371, 6371, 6371, 6339, 6339, 2145, 2145, 4226, 2145, 2145, 2145, 4226, 4258, 4226, 4226, 4226, 2145, 4226,
4226, 2145, 4226, 2145, 2145, 4226, 4226, 4226, 4226, 4226, 2145, 2145, 4258, 6371, 8452, 6371, 6371, 6371, 8452, 6339, 2145, 6339, 8452, 6371, 6339, 6371, 6371, 6339, 4226, 2145, 2145, 2145, 2145, 2145, 4258, 4258, 2145, 2145, 2145, 2145,
4226, 2145, 4226, 2113, 2145, 4226, 4258, 4226, 2145, 2145, 2145, 4258, 6371, 8452, 6371, 6339, 6371, 6371, 8452, 8452, 6339, 8452, 8452, 8452, 6371, 6371, 8452, 8452, 6339, 4226, 2145, 2145, 2145, 2145, 4226, 4258, 2145, 2113, 2145, 2145,
2145, 2145, 2113, 2145, 4226, 4226, 4226, 2145, 2145, 2113, 4226, 6339, 6371, 6371, 6371, 6371, 6371, 8452, 6371, 8452, 8452, 6371, 8452, 8452, 6339, 6371, 8452, 8452, 8452, 6371, 4226, 2145, 4226, 4226, 4226, 4226, 4226, 4226, 2145, 4226,
4226, 4226, 2145, 4226, 4226, 2145, 4226, 2145, 2113, 2145, 6339, 6371, 6371, 8452, 8452, 8452, 8484, 8452, 6371, 6371, 6371, 6371, 6371, 6371, 6371, 6371, 6371, 6371, 8452, 8452, 6339, 2145, 2145, 4226, 4226, 2145, 4226, 4226, 2145, 4226,
2145, 4226, 4258, 2145, 2145, 2145, 2113, 2145, 4226, 6339, 6371, 6371, 6371, 8452, 8484, 8484, 8452, 6371, 6371, 6371, 6371, 6371, 6371, 8452, 6371, 6339, 6371, 6371, 6371, 8452, 8452, 6339, 4226, 2145, 2145, 4226, 4226, 2145, 4226, 2145,
32, 2113, 4226, 4226, 2145, 2145, 2113, 2145, 8452, 8452, 6371, 6371, 6339, 8484, 8484, 8484, 8484, 8452, 6371, 6339, 6371, 6371, 6371, 6371, 6371, 6371, 6339, 6339, 6339, 6339, 8452, 8484, 6371, 2145, 2145, 4226, 2145, 4226, 4226, 2113,
0, 0, 2145, 4226, 4226, 4226, 4258, 6371, 6371, 8452, 6371, 6339, 6339, 8452, 8484, 8452, 8452, 8452, 6371, 6371, 8452, 8452, 8452, 6371, 6371, 6339, 6371, 6339, 6371, 6371, 8452, 8452, 8484, 8452, 4226, 4226, 4258, 4258, 2113, 32,
0, 0, 32, 2113, 2145, 6339, 8452, 8452, 6339, 6339, 8452, 6371, 6339, 6371, 10565, 8484, 8452, 8484, 8452, 6371, 6339, 8452, 8452, 6371, 6371, 6339, 6371, 6371, 6371, 8452, 8452, 8452, 8484, 10565, 6371, 4258, 4226, 2145, 32, 0,
0, 0, 0, 0, 2113, 6339, 8452, 6371, 6371, 6339, 6371, 8452, 6371, 6371, 8452, 8484, 8452, 8452, 8484, 8452, 4258, 8452, 8452, 6339, 6339, 6339, 6339, 6371, 6371, 6371, 6371, 8452, 8452, 8452, 8484, 8452, 2113, 0, 0, 32,
0, 0, 0, 0, 0, 4258, 6371, 6371, 6371, 6371, 6371, 6371, 6371, 6371, 8452, 8452, 8452, 8484, 8452, 2145, 32, 4226, 6371, 6371, 6371, 6339, 6371, 6371, 6371, 6371, 6371, 6371, 8452, 8452, 8452, 4226, 32, 0, 0, 32,
0, 0, 0, 0, 0, 0, 4226, 8452, 6371, 6371, 6371, 6339, 6371, 8452, 8452, 8452, 8484, 8452, 2145, 0, 0, 0, 4226, 8452, 6371, 6371, 6371, 6339, 6339, 6371, 6371, 6371, 8452, 8484, 4258, 0, 32, 32, 32, 0,
0, 0, 0, 0, 0, 0, 32, 4258, 6371, 6371, 6371, 6371, 6371, 8452, 8452, 8452, 8452, 2145, 0, 0, 32, 0, 0, 4226, 6371, 6371, 6339, 6339, 6371, 6371, 6371, 6371, 6371, 2145, 0, 0, 32, 32, 32, 0,
0, 0, 0, 0, 0, 0, 0, 32, 6339, 6371, 6371, 6371, 6371, 8452, 8484, 8452, 4226, 32, 0, 0, 0, 0, 0, 0, 4226, 6339, 6339, 6339, 6371, 6371, 6371, 6371, 4226, 0, 32, 32, 32, 32, 0, 0,
0, 0, 0, 0, 0, 0, 0, 2113, 6339, 6371, 6371, 6371, 6371, 8452, 8484, 8484, 4258, 32, 0, 0, 0, 0, 0, 32, 4258, 6339, 6339, 6371, 6371, 6371, 8452, 8452, 4258, 32, 32, 32, 32, 32, 0, 32,
0, 0, 0, 32, 0, 0, 2113, 6339, 8484, 8452, 6371, 6371, 8452, 8484, 8484, 8484, 8452, 4226, 0, 0, 0, 0, 32, 6339, 8452, 6371, 6371, 6371, 6371, 6371, 8452, 8452, 8484, 6371, 2113, 32, 32, 32, 32, 32,
0, 0, 0, 0, 0, 2113, 6339, 8452, 8484, 8484, 8452, 6371, 8452, 10597, 8452, 8452, 8452, 6371, 4226, 0, 0, 32, 4258, 8452, 8452, 8452, 8452, 6371, 6339, 6371, 6371, 6371, 8484, 8484, 6339, 2113, 0, 32, 32, 32,
0, 0, 0, 0, 2113, 6371, 8452, 8452, 8452, 8452, 8452, 8452, 8484, 10597, 8452, 8452, 6371, 8452, 8452, 4258, 2113, 6339, 8452, 8452, 8452, 6371, 8452, 8452, 8452, 8452, 6371, 6371, 8452, 8484, 8484, 6371, 2113, 32, 32, 32,
0, 0, 0, 32, 2145, 6339, 8484, 8452, 8452, 8452, 8452, 8452, 8484, 8484, 8452, 8452, 6371, 8452, 8452, 10565, 8484, 8452, 8484, 8452, 8452, 6371, 8452, 8484, 8452, 8484, 8452, 6371, 8484, 8484, 10597, 6371, 6339, 2145, 0, 32,
0, 0, 0, 2145, 4226, 2145, 6339, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8452, 8484, 8452, 8452, 8452, 8452, 8452, 6371, 8452, 8452, 8452, 8484, 8484, 8484, 8484, 12678, 6371, 4226, 6371, 4258, 32, 0,
0, 32, 2145, 4226, 4226, 2145, 2145, 6339, 8484, 10565, 8452, 8452, 8452, 8452, 8484, 8484, 8452, 8452, 6371, 6371, 6371, 6339, 8452, 8452, 8452, 8452, 6371, 8452, 8484, 8484, 8452, 8484, 10565, 8484, 4258, 4226, 6339, 6339, 2145, 32,
2113, 4226, 4258, 4226, 4226, 4226, 2145, 2145, 8452, 8484, 8452, 6371, 8452, 8452, 8484, 8452, 8452, 6371, 6339, 6339, 6339, 6371, 6371, 8452, 8452, 8452, 6371, 6371, 6339, 8452, 8484, 8484, 6371, 4226, 4258, 6339, 4226, 4258, 6339, 4226,
2145, 2145, 4226, 4226, 4226, 4226, 2145, 2145, 2145, 10565, 8452, 8452, 8452, 6371, 8452, 8452, 8452, 6371, 8452, 6371, 6339, 6371, 6371, 6371, 8452, 6371, 6371, 6339, 6339, 6371, 6371, 6339, 4226, 4258, 4226, 6339, 6339, 4258, 6339, 4226,
2145, 2145, 2113, 4226, 4226, 4226, 4226, 4226, 2145, 4226, 6339, 6371, 6371, 8452, 8452, 8452, 8452, 8452, 6371, 6371, 6371, 6339, 8452, 8452, 8452, 8452, 8452, 8452, 6371, 6371, 4258, 2145, 4226, 4258, 6339, 4258, 4258, 4226, 4226, 4258,
2145, 2145, 2145, 2145, 2145, 4226, 4226, 4226, 4226, 2145, 2145, 6371, 8484, 8452, 8452, 8452, 8452, 8452, 8452, 6371, 6371, 6371, 8452, 8484, 8452, 8452, 8452, 8484, 8484, 6339, 2145, 2145, 4226, 4226, 4258, 6339, 4226, 2145, 4226, 4226,
2145, 2145, 4226, 4226, 4226, 4258, 4226, 4258, 4258, 4226, 2145, 4226, 6371, 8452, 8452, 6371, 8452, 8452, 8452, 8452, 6339, 8452, 8452, 8452, 8484, 10565, 8452, 8484, 6371, 4226, 4226, 4258, 6339, 6339, 6339, 6339, 4226, 2145, 2145, 4226,
2145, 2145, 2145, 4226, 4226, 6339, 6339, 6339, 4226, 4226, 4226, 2145, 4258, 6339, 8452, 8452, 8452, 8452, 8484, 4258, 2113, 6339, 8452, 8452, 8452, 8484, 8452, 6339, 4258, 4226, 6339, 6339, 6339, 6339, 6339, 6339, 4226, 2145, 2145, 4258,
2145, 4226, 4226, 4226, 4258, 4226, 6371, 6339, 4226, 4226, 4226, 4226, 2145, 4226, 6339, 8452, 8452, 8452, 4258, 32, 0, 2113, 6339, 8484, 10565, 8484, 6339, 4226, 4258, 6339, 6339, 6339, 6339, 4258, 4226, 2145, 4258, 4226, 4226, 4258,
2145, 4226, 4258, 4258, 4226, 4226, 4226, 4258, 6339, 4258, 4226, 2145, 2145, 2145, 4226, 6371, 8452, 4226, 32, 0, 0, 0, 32, 6371, 10565, 8452, 4226, 4226, 4258, 4258, 6339, 6339, 6371, 4226, 2145, 4226, 4226, 4226, 4258, 4226,
32, 2113, 6339, 4258, 4258, 4226, 4226, 4226, 4258, 4258, 4226, 4226, 4226, 4226, 4226, 4226, 4226, 32, 0, 0, 0, 0, 0, 2113, 8452, 6339, 4258, 4226, 4226, 4226, 4258, 6339, 4258, 4258, 4226, 4258, 6339, 4258, 4226, 2113,
0, 0, 2113, 4258, 4258, 4258, 4258, 4226, 2145, 2145, 4226, 4226, 4226, 4258, 4258, 2113, 32, 32, 32, 32, 32, 0, 32, 2113, 2145, 4258, 6371, 6339, 4258, 6339, 4258, 4226, 4258, 8484, 6371, 8452, 8484, 6339, 2113, 0,
0, 0, 0, 2113, 6339, 6339, 4258, 4226, 2145, 2145, 2145, 4226, 6339, 4258, 2145, 32, 32, 32, 32, 32, 0, 0, 32, 32, 2113, 2145, 6339, 8452, 8452, 4258, 4226, 4258, 6339, 8484, 10565, 10597, 8452, 2145, 32, 0,
0, 0, 0, 0, 4258, 6371, 6339, 4226, 4226, 2145, 2145, 4226, 4258, 4226, 32, 0, 32, 32, 32, 32, 0, 32, 0, 32, 32, 32, 2145, 8452, 8452, 4258, 4226, 4226, 4258, 8484, 8452, 8484, 4226, 0, 0, 0,
0, 0, 0, 0, 32, 4226, 4258, 6339, 4226, 4226, 4226, 4226, 2145, 32, 32, 0, 0, 32, 32, 32, 0, 0, 0, 0, 32, 2113, 2113, 2145, 4226, 4226, 4226, 4226, 6339, 6371, 4226, 2145, 32, 0, 0, 0,
};
#endif /*COLOR_DEPTH == 16*/

#if COLOR_DEPTH == 24
static const color_int_t img_benchmark_bg[] = {
/*HEADER
   Width = 40
   Height = 40
   Transp: 0
   Color depth: 24*/
100827176,

/*IMAGE DATA*/

263172, 460551, 460551, 526344, 723723, 1447446, 1644825, 1447446, 1447446, 1052688, 1052688, 921102, 1052688, 723723, 263172, 197379, 131586, 65793, 65793, 0, 65793, 0, 65793, 65793, 65793, 65793, 65793, 328965, 789516, 855309, 1250067, 1052688, 1052688, 1184274, 1184274, 986895, 394758, 394758, 394758, 328965,
131586, 460551, 526344, 394758, 1776411, 2631720, 2105376, 1447446, 1250067, 1052688, 855309, 1052688, 1579032, 1250067, 460551, 65793, 65793, 65793, 65793, 65793, 0, 65793, 65793, 0, 0, 0, 328965, 986895, 1184274, 789516, 921102, 1250067, 1052688, 1052688, 1842204, 1644825, 1052688, 328965, 263172, 328965,
394758, 460551, 526344, 1052688, 2434341, 2434341, 2236962, 1907997, 1052688, 855309, 1052688, 1315860, 1381653, 1447446, 855309, 263172, 0, 0, 131586, 131586, 131586, 131586, 131586, 65793, 0, 263172, 855309, 1052688, 1052688, 1052688, 1118481, 921102, 986895, 1052688, 1579032, 1513239, 1513239, 1118481, 394758, 328965,
263172, 460551, 1447446, 1842204, 2171169, 1973790, 1973790, 1776411, 1052688, 986895, 1184274, 1250067, 921102, 1118481, 1118481, 723723, 263172, 0, 65793, 131586, 197379, 263172, 131586, 131586, 328965, 657930, 921102, 855309, 921102, 1052688, 1052688, 1052688, 855309, 1250067, 1579032, 1250067, 1842204, 1710618, 986895, 263172,
657930, 921102, 1842204, 1776411, 1250067, 1118481, 1052688, 1118481, 1315860, 1381653, 1052688, 855309, 1052688, 855309, 921102, 1184274, 1381653, 394758, 65793, 131586, 131586, 131586, 65793, 526344, 1579032, 1052688, 723723, 789516, 855309, 921102, 1052688, 1184274, 1315860, 1052688, 1315860, 1447446, 1513239, 1710618, 1644825, 855309,
1052688, 1315860, 1710618, 1250067, 1052688, 986895, 921102, 921102, 1052688, 1052688, 1184274, 855309, 855309, 921102, 1052688, 1776411, 2302755, 1644825, 394758, 0, 131586, 131586, 657930, 1776411, 1907997, 1710618, 855309, 723723, 855309, 986895, 1052688, 1052688, 1184274, 1184274, 1052688, 1184274, 1250067, 1513239, 1447446, 1184274,
1184274, 1052688, 986895, 1052688, 921102, 986895, 1184274, 1052688, 921102, 1052688, 1052688, 1052688, 986895, 1118481, 1907997, 1907997, 1973790, 2171169, 1513239, 394758, 65793, 657930, 1973790, 1973790, 1842204, 1776411, 1579032, 855309, 789516, 1052688, 986895, 986895, 986895, 1250067, 1315860, 1118481, 1250067, 1118481, 986895, 1118481,
1250067, 986895, 1052688, 855309, 986895, 1250067, 1184274, 1052688, 1052688, 1052688, 986895, 921102, 1315860, 1973790, 2105376, 1842204, 1842204, 1973790, 2171169, 1579032, 789516, 1710618, 2105376, 1973790, 1776411, 1907997, 1973790, 1710618, 1118481, 855309, 855309, 855309, 921102, 986895, 1315860, 1381653, 986895, 855309, 789516, 986895,
1250067, 855309, 1052688, 657930, 986895, 1118481, 1315860, 1118481, 986895, 986895, 855309, 1381653, 1907997, 2105376, 1842204, 1776411, 1842204, 1907997, 2105376, 2171169, 1776411, 2105376, 2236962, 2105376, 1842204, 1973790, 2171169, 2236962, 1776411, 1052688, 855309, 986895, 921102, 921102, 1118481, 1513239, 921102, 657930, 921102, 855309,
986895, 855309, 657930, 855309, 1052688, 1118481, 1118481, 986895, 855309, 657930, 1052688, 1776411, 1907997, 1907997, 1973790, 1907997, 1907997, 2171169, 1973790, 2236962, 2171169, 1973790, 2105376, 2105376, 1776411, 1973790, 2236962, 2236962, 2302755, 1842204, 1118481, 855309, 1052688, 1052688, 1052688, 1184274, 1118481, 1052688, 921102, 1052688,
1250067, 1250067, 855309, 1052688, 1052688, 986895, 1052688, 855309, 657930, 855309, 1710618, 1907997, 1907997, 2105376, 2171169, 2302755, 2368548, 2105376, 1973790, 1907997, 1842204, 1842204, 1907997, 1907997, 1842204, 1907997, 1973790, 1973790, 2171169, 2302755, 1776411, 855309, 855309, 1052688, 1052688, 986895, 1184274, 1184274, 855309, 1052688,
986895, 1052688, 1315860, 855309, 855309, 855309, 723723, 789516, 1052688, 1710618, 1973790, 1842204, 1973790, 2302755, 2434341, 2434341, 2302755, 1973790, 1842204, 1842204, 1842204, 1973790, 1907997, 2236962, 1907997, 1776411, 1907997, 1907997, 1973790, 2105376, 2236962, 1776411, 1052688, 855309, 855309, 1184274, 1052688, 986895, 1184274, 986895,
328965, 657930, 1052688, 1052688, 855309, 855309, 723723, 921102, 2105376, 2236962, 1907997, 1973790, 1710618, 2368548, 2434341, 2434341, 2368548, 2236962, 1842204, 1710618, 1842204, 1842204, 1907997, 1973790, 1907997, 1842204, 1776411, 1710618, 1776411, 1776411, 2171169, 2368548, 1907997, 855309, 986895, 1052688, 986895, 1118481, 1052688, 723723,
131586, 131586, 855309, 1118481, 1052688, 1118481, 1447446, 1907997, 1842204, 2105376, 1907997, 1776411, 1776411, 2171169, 2500134, 2171169, 2171169, 2236962, 1907997, 1907997, 2105376, 2105376, 2171169, 1973790, 1842204, 1776411, 1842204, 1776411, 1842204, 1842204, 2171169, 2105376, 2434341, 2105376, 1118481, 1052688, 1381653, 1315860, 657930, 328965,
65793, 131586, 263172, 657930, 855309, 1579032, 2236962, 2171169, 1776411, 1776411, 2105376, 1842204, 1776411, 1973790, 2631720, 2434341, 2302755, 2368548, 2105376, 1842204, 1776411, 2236962, 2171169, 1842204, 1842204, 1776411, 1842204, 1842204, 1842204, 2105376, 2105376, 2105376, 2500134, 2631720, 1907997, 1381653, 1052688, 789516, 263172, 197379,
65793, 131586, 131586, 131586, 526344, 1644825, 2105376, 1842204, 1907997, 1776411, 1907997, 2105376, 1842204, 1973790, 2105376, 2368548, 2236962, 2302755, 2368548, 2171169, 1513239, 2105376, 2171169, 1776411, 1644825, 1710618, 1644825, 1842204, 1973790, 1973790, 1842204, 2105376, 2236962, 2236962, 2565927, 2105376, 657930, 131586, 131586, 328965,
65793, 131586, 197379, 131586, 197379, 1315860, 1973790, 1973790, 1907997, 1907997, 1907997, 1842204, 1842204, 1973790, 2171169, 2236962, 2302755, 2434341, 2302755, 855309, 263172, 1184274, 1842204, 1973790, 1842204, 1710618, 1907997, 1842204, 1842204, 1973790, 1973790, 1842204, 2105376, 2302755, 2105376, 1184274, 328965, 197379, 131586, 263172,
65793, 131586, 131586, 65793, 0, 197379, 1250067, 2105376, 1973790, 1907997, 1973790, 1776411, 1842204, 2236962, 2171169, 2105376, 2500134, 2171169, 789516, 131586, 197379, 197379, 1184274, 2171169, 1907997, 1907997, 1842204, 1710618, 1710618, 1842204, 1907997, 1842204, 2171169, 2368548, 1315860, 65793, 263172, 394758, 263172, 197379,
65793, 131586, 131586, 131586, 65793, 0, 263172, 1315860, 1907997, 1907997, 1842204, 1907997, 1973790, 2302755, 2302755, 2236962, 2171169, 855309, 131586, 131586, 263172, 131586, 197379, 1052688, 1973790, 1907997, 1710618, 1710618, 1907997, 1842204, 1907997, 1973790, 1973790, 986895, 131586, 197379, 394758, 394758, 263172, 131586,
65793, 197379, 65793, 131586, 131586, 131586, 65793, 394758, 1579032, 1907997, 1907997, 1907997, 1973790, 2171169, 2565927, 2171169, 1118481, 263172, 131586, 197379, 197379, 131586, 131586, 197379, 1118481, 1776411, 1776411, 1776411, 1842204, 1907997, 1973790, 1973790, 1052688, 131586, 263172, 328965, 394758, 328965, 197379, 197379,
131586, 131586, 131586, 197379, 131586, 131586, 131586, 592137, 1644825, 1973790, 1842204, 1973790, 1973790, 2236962, 2500134, 2368548, 1447446, 263172, 65793, 197379, 197379, 131586, 131586, 394758, 1447446, 1776411, 1776411, 1842204, 1907997, 1973790, 2105376, 2171169, 1513239, 394758, 328965, 460551, 460551, 328965, 197379, 263172,
197379, 197379, 197379, 328965, 197379, 131586, 592137, 1710618, 2368548, 2302755, 1907997, 1907997, 2236962, 2434341, 2368548, 2434341, 2236962, 1052688, 131586, 65793, 197379, 131586, 328965, 1579032, 2302755, 1973790, 1842204, 1842204, 1842204, 1907997, 2105376, 2302755, 2500134, 1907997, 657930, 394758, 394758, 394758, 394758, 328965,
131586, 131586, 131586, 197379, 131586, 526344, 1776411, 2302755, 2434341, 2368548, 2171169, 1842204, 2302755, 2894892, 2302755, 2236962, 2171169, 1907997, 1118481, 197379, 131586, 394758, 1513239, 2236962, 2171169, 2105376, 2105376, 1842204, 1710618, 1842204, 1907997, 1907997, 2368548, 2565927, 1644825, 592137, 197379, 394758, 328965, 328965,
65793, 131586, 131586, 65793, 526344, 1842204, 2302755, 2236962, 2302755, 2302755, 2105376, 2105376, 2434341, 2894892, 2302755, 2105376, 1973790, 2105376, 2105376, 1381653, 592137, 1644825, 2236962, 2302755, 2105376, 1973790, 2171169, 2171169, 2105376, 2105376, 1973790, 1973790, 2171169, 2565927, 2565927, 1973790, 657930, 460551, 460551, 263172,
0, 65793, 65793, 263172, 855309, 1776411, 2434341, 2105376, 2236962, 2105376, 2302755, 2302755, 2368548, 2368548, 2105376, 2105376, 1973790, 2171169, 2302755, 2829099, 2565927, 2302755, 2500134, 2105376, 2105376, 1842204, 2171169, 2368548, 2302755, 2368548, 2171169, 1907997, 2368548, 2500134, 2894892, 1842204, 1579032, 789516, 197379, 263172,
0, 0, 197379, 789516, 1052688, 921102, 1710618, 2302755, 2105376, 2171169, 2302755, 2236962, 2105376, 2105376, 2302755, 2171169, 2171169, 2302755, 2105376, 2434341, 2302755, 2171169, 2302755, 2105376, 2105376, 1973790, 2105376, 2302755, 2302755, 2368548, 2434341, 2368548, 2500134, 3158064, 1973790, 1118481, 1907997, 1315860, 328965, 131586,
65793, 263172, 986895, 1184274, 1052688, 855309, 921102, 1644825, 2368548, 2631720, 2302755, 2236962, 2171169, 2302755, 2565927, 2368548, 2105376, 2105376, 1907997, 1907997, 1973790, 1776411, 2105376, 2171169, 2171169, 2105376, 1907997, 2302755, 2500134, 2434341, 2236962, 2434341, 2829099, 2434341, 1513239, 1184274, 1644825, 1776411, 921102, 394758,
657930, 1052688, 1381653, 1184274, 1052688, 1052688, 855309, 855309, 2105376, 2368548, 2105376, 1973790, 2171169, 2302755, 2565927, 2302755, 2171169, 1907997, 1776411, 1710618, 1776411, 1973790, 1842204, 2105376, 2236962, 2105376, 1907997, 1842204, 1776411, 2236962, 2368548, 2500134, 1973790, 1184274, 1513239, 1710618, 1250067, 1381653, 1776411, 1118481,
855309, 921102, 1118481, 1118481, 1184274, 1052688, 986895, 855309, 855309, 2631720, 2105376, 2105376, 2105376, 1973790, 2236962, 2302755, 2105376, 1907997, 2171169, 1907997, 1710618, 1842204, 1842204, 1842204, 2236962, 1842204, 1842204, 1710618, 1776411, 1842204, 1842204, 1579032, 1118481, 1513239, 1250067, 1579032, 1579032, 1513239, 1644825, 1052688,
986895, 855309, 723723, 1052688, 1052688, 1052688, 1250067, 1052688, 986895, 1052688, 1644825, 1907997, 1973790, 2302755, 2236962, 2236962, 2236962, 2236962, 1973790, 1907997, 1907997, 1710618, 2105376, 2302755, 2105376, 2105376, 2105376, 2105376, 1973790, 1973790, 1315860, 789516, 1118481, 1513239, 1579032, 1381653, 1513239, 1118481, 1052688, 1315860,
855309, 855309, 855309, 855309, 986895, 1184274, 1052688, 1118481, 1052688, 921102, 855309, 1842204, 2368548, 2302755, 2236962, 2105376, 2105376, 2236962, 2171169, 1973790, 1907997, 1842204, 2105376, 2434341, 2236962, 2171169, 2302755, 2434341, 2500134, 1776411, 986895, 855309, 1118481, 1250067, 1513239, 1776411, 1184274, 855309, 1052688, 1118481,
855309, 789516, 1052688, 1052688, 1052688, 1381653, 1250067, 1315860, 1447446, 1052688, 855309, 1052688, 1973790, 2302755, 2105376, 1973790, 2105376, 2171169, 2302755, 2171169, 1710618, 2105376, 2171169, 2171169, 2500134, 2631720, 2105376, 2434341, 1907997, 1052688, 1118481, 1513239, 1579032, 1579032, 1776411, 1776411, 1184274, 789516, 986895, 1184274,
986895, 855309, 789516, 1052688, 1250067, 1644825, 1776411, 1579032, 1184274, 1052688, 1052688, 986895, 1315860, 1776411, 2171169, 2171169, 2105376, 2302755, 2368548, 1315860, 592137, 1710618, 2236962, 2236962, 2171169, 2500134, 2171169, 1710618, 1315860, 1184274, 1644825, 1776411, 1776411, 1776411, 1644825, 1579032, 1184274, 921102, 855309, 1315860,
921102, 1052688, 1052688, 1184274, 1315860, 1250067, 1842204, 1776411, 1250067, 1184274, 1118481, 1052688, 986895, 1052688, 1710618, 2302755, 2302755, 2302755, 1315860, 263172, 65793, 526344, 1644825, 2368548, 2631720, 2434341, 1644825, 1052688, 1315860, 1644825, 1579032, 1710618, 1579032, 1447446, 1052688, 921102, 1315860, 1118481, 1118481, 1315860,
855309, 1184274, 1513239, 1315860, 1052688, 1118481, 1184274, 1513239, 1579032, 1381653, 1118481, 986895, 986895, 986895, 1052688, 1907997, 2302755, 1184274, 263172, 131586, 131586, 131586, 460551, 1973790, 2829099, 2171169, 1052688, 1184274, 1315860, 1381653, 1644825, 1644825, 1842204, 1250067, 986895, 1184274, 1052688, 1250067, 1513239, 1250067,
394758, 657930, 1644825, 1381653, 1315860, 1184274, 1184274, 1184274, 1315860, 1381653, 1052688, 1052688, 1052688, 1184274, 1118481, 1118481, 1052688, 328965, 131586, 197379, 131586, 131586, 197379, 592137, 2105376, 1710618, 1447446, 1184274, 1052688, 1250067, 1447446, 1579032, 1513239, 1513239, 1250067, 1513239, 1579032, 1513239, 1118481, 592137,
197379, 197379, 657930, 1381653, 1513239, 1315860, 1315860, 1250067, 789516, 855309, 1250067, 1250067, 1052688, 1513239, 1315860, 657930, 328965, 328965, 328965, 263172, 263172, 197379, 328965, 526344, 855309, 1513239, 1973790, 1710618, 1315860, 1579032, 1381653, 1184274, 1447446, 2368548, 1907997, 2105376, 2500134, 1644825, 657930, 131586,
197379, 131586, 131586, 657930, 1710618, 1776411, 1447446, 1052688, 855309, 855309, 921102, 1118481, 1579032, 1447446, 855309, 328965, 328965, 394758, 328965, 263172, 131586, 197379, 263172, 460551, 657930, 789516, 1710618, 2236962, 2105376, 1447446, 1250067, 1381653, 1776411, 2368548, 2631720, 3026478, 2236962, 789516, 263172, 131586,
197379, 197379, 131586, 197379, 1381653, 1973790, 1579032, 1184274, 1052688, 921102, 855309, 1052688, 1447446, 1052688, 328965, 131586, 328965, 460551, 460551, 328965, 131586, 263172, 197379, 328965, 460551, 460551, 921102, 2236962, 2105376, 1447446, 1118481, 1250067, 1513239, 2434341, 2302755, 2565927, 1052688, 131586, 131586, 131586,
197379, 131586, 131586, 131586, 328965, 1052688, 1447446, 1644825, 1250067, 1052688, 1184274, 1052688, 855309, 394758, 263172, 197379, 131586, 263172, 328965, 263172, 197379, 131586, 197379, 197379, 263172, 592137, 657930, 921102, 1118481, 1250067, 1118481, 1118481, 1710618, 1973790, 1250067, 855309, 263172, 131586, 197379, 131586,
};
#endif /*COLOR_DEPTH == 24*/

#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_BENCHMARK != 0*/
