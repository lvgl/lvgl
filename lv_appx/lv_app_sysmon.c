/**
 * @file lv_app_sysmon.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_sysmon.h"
#if LV_APP_ENABLE != 0 && USE_LV_APP_SYSMON != 0

#include <stdio.h>
#include "misc/os/ptask.h"
#include "misc/os/idle.h"
#include "lvgl/lv_objx/lv_chart.h"
#include "lvgl/lv_app/lv_app_util/lv_app_notice.h"
#include "hal/systick/systick.h"

/*********************
 *      DEFINES
 *********************/
#define CPU_LABEL_COLOR "FF0000"
#define MEM_LABEL_COLOR "0000FF"

/**********************
 *      TYPEDEFS
 **********************/

/*Application specific data for an instance of this application*/
typedef struct
{

}my_app_data_t;

/*Application specific data a window of this application*/
typedef struct
{
    lv_obj_t * chart;
    lv_chart_dl_t * cpu_dl;
    lv_chart_dl_t * mem_dl;
    lv_obj_t * label;
}my_win_data_t;

/*Application specific data for a shortcut of this application*/
typedef struct
{
    lv_obj_t * bar_cpu;
    lv_obj_t * bar_mem;
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

static void sysmon_task(void * param);
static void lv_app_sysmon_refr(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_app_dsc_t my_app_dsc =
{
	.name = "Sys. monitor",
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

static uint8_t mem_pct[LV_APP_SYSMON_PNUM];
static uint8_t cpu_pct[LV_APP_SYSMON_PNUM];
static lv_style_t cpu_bar_bg;
static lv_style_t mem_bar_bg;
static lv_style_t cpu_bar_indic;
static lv_style_t mem_bar_indic;
#if USE_DYN_MEM != 0  && DM_CUSTOM == 0
static  dm_mon_t mem_mon;
#endif

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
const lv_app_dsc_t * lv_app_sysmon_init(void)
{
    ptask_create(sysmon_task, LV_APP_SYSMON_REFR_TIME, PTASK_PRIO_LOW, NULL);

    memset(mem_pct, 0, sizeof(mem_pct));
    memset(cpu_pct, 0, sizeof(cpu_pct));

    /*Create bar styles for the shortcut*/
    lv_style_get(LV_STYLE_PRETTY, &cpu_bar_bg);
    cpu_bar_bg.ccolor = COLOR_MAKE(0x40, 0x00, 0x00);
    cpu_bar_bg.font = font_get(LV_APP_FONT_MEDIUM);
    cpu_bar_bg.line_space = 0;
    cpu_bar_bg.txt_align = 1;
    cpu_bar_bg.hpad = 0;
    cpu_bar_bg.vpad = 0;

    memcpy(&mem_bar_bg, &cpu_bar_bg, sizeof(lv_style_t));
    mem_bar_bg.ccolor = COLOR_MAKE(0x00, 0x40, 0x00);

    lv_style_get(LV_STYLE_PRETTY_COLOR, &cpu_bar_indic);
    cpu_bar_indic.gcolor = COLOR_MARRON;
    cpu_bar_indic.mcolor = COLOR_RED;
    cpu_bar_indic.bcolor = COLOR_BLACK;
    //cpu_bar_indic.bwidth = 1 * LV_DOWNSCALE;
    cpu_bar_indic.bopa = OPA_50;
    cpu_bar_indic.hpad = 0 * LV_DOWNSCALE;
    cpu_bar_indic.vpad = 0 * LV_DOWNSCALE;


    memcpy(&mem_bar_indic, &cpu_bar_indic, sizeof(lv_style_t));
    mem_bar_indic.gcolor = COLOR_GREEN;
    mem_bar_indic.mcolor = COLOR_LIME;

	return &my_app_dsc;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Run an application according to 'app_dsc'
 * @param app_dsc pointer to an application descriptor
 * @param conf pointer to a lv_app_sysmon_conf_t structure with configuration data or NULL if unused
 * @return pointer to the opened application or NULL if any error occurred
 */
static void my_app_run(lv_app_inst_t * app,  void * conf)
{

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

    cord_t w = lv_obj_get_width(sc) / 5;

    /*Create 2 bars for the CPU and the Memory*/
    sc_data->bar_cpu = lv_bar_create(sc, NULL);
    lv_obj_set_size(sc_data->bar_cpu, w, 5 * lv_obj_get_height(sc) / 8);
    lv_obj_align(sc_data->bar_cpu, NULL, LV_ALIGN_IN_BOTTOM_LEFT, w, - lv_obj_get_height(sc) / 8);
    lv_obj_set_style(sc_data->bar_cpu, &cpu_bar_bg);
    lv_bar_set_style_indic(sc_data->bar_cpu, &cpu_bar_indic);
    lv_obj_set_click(sc_data->bar_cpu, false);
    lv_bar_set_range(sc_data->bar_cpu, 0, 100);
    lv_obj_t * label = lv_label_create(sc_data->bar_cpu, NULL);
    lv_label_set_text(label, "C\nP\nU");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    sc_data->bar_mem = lv_bar_create(sc, sc_data->bar_cpu);
    lv_obj_align(sc_data->bar_mem, sc_data->bar_cpu, LV_ALIGN_OUT_RIGHT_MID, w, 0);
    lv_obj_set_style(sc_data->bar_mem, &mem_bar_bg);
    lv_bar_set_style_indic(sc_data->bar_mem, &mem_bar_indic);
    label = lv_label_create(sc_data->bar_mem, NULL);
    lv_label_set_text(label, "M\nE\nM");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_app_sysmon_refr();
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
    my_win_data_t * win_data = app->win_data;

    lv_cont_set_layout(lv_page_get_scrl(lv_win_get_page(win)), LV_CONT_LAYOUT_PRETTY);

    /*Create a chart with two data lines*/
    win_data->chart = lv_chart_create(win, NULL);
    lv_obj_set_size(win_data->chart, LV_HOR_RES / 2, LV_VER_RES / 2);
    lv_obj_set_pos(win_data->chart, LV_DPI / 10, LV_DPI / 10);
    lv_chart_set_pnum(win_data->chart, LV_APP_SYSMON_PNUM);
    lv_chart_set_range(win_data->chart, 0, 100);
    lv_chart_set_type(win_data->chart, LV_CHART_LINE);
    lv_chart_set_dl_width(win_data->chart, 2 * LV_DOWNSCALE);
    win_data->cpu_dl =  lv_chart_add_data_line(win_data->chart, COLOR_RED);
    win_data->mem_dl =  lv_chart_add_data_line(win_data->chart, COLOR_BLUE);

    uint16_t i;
    for(i = 0; i < LV_APP_SYSMON_PNUM; i ++) {
        win_data->cpu_dl->points[i] = cpu_pct[i];
        win_data->mem_dl->points[i] = mem_pct[i];
    }

    /*Create a label for the details of Memory and CPU usage*/
    win_data->label = lv_label_create(win, NULL);
    lv_label_set_recolor(win_data->label, true);
    lv_obj_align(win_data->label, win_data->chart, LV_ALIGN_OUT_RIGHT_TOP, LV_DPI / 4, 0);


    lv_app_sysmon_refr();
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

/**
 * Called periodically to monitor the CPU and memory usage.
 * It refreshes the shortcuts and windows and also add notifications if there is any problem.
 * @param param unused
 */
static void sysmon_task(void * param)
{
    /*Shift out the oldest data*/
    uint16_t i;
    for(i = 1; i < LV_APP_SYSMON_PNUM; i++) {
        mem_pct[i - 1] = mem_pct[i];
        cpu_pct[i - 1] = cpu_pct[i];
    }

    /*Get CPU and memory information */
    uint8_t cpu_busy = 0;
#if USE_IDLE != 0   /*Use the more precise idle module if enabled*/
    cpu_busy = 100 - idle_get();
#else
    cpu_busy = 100 - ptask_get_idle();
#endif

    uint8_t mem_used_pct = 0;
#if  USE_DYN_MEM != 0  && DM_CUSTOM == 0
    dm_monitor(&mem_mon);
    mem_used_pct = mem_mon.pct_used;
#endif

    /*Add the CPU and memory data*/
    cpu_pct[LV_APP_SYSMON_PNUM - 1] = cpu_busy;
    mem_pct[LV_APP_SYSMON_PNUM - 1] = mem_used_pct;

    /*Refresh the shortcuts and windows*/
    lv_app_sysmon_refr();

#if USE_DYN_MEM != 0 && DM_CUSTOM == 0

    /*Handle periodic defrag. if enabled*/
#if LV_APP_SYSMON_DEFRAG_PERIOD != 0
    static uint32_t last_defrag = 0;

    if(systick_elaps(last_defrag) > LV_APP_SYSMON_DEFRAG_PERIOD) {
        dm_defrag();
        last_defrag = systick_get();
    }
#endif
    /*Add notifications if something is critical*/
    static bool mem_warn_report = false;
    if(mem_mon.size_free < LV_APP_SYSMON_MEM_WARN && mem_warn_report == false) {
        mem_warn_report = true;
        lv_app_notice_add("Critically low memory");
    }

    if(mem_mon.size_free > LV_APP_SYSMON_MEM_WARN)  mem_warn_report = false;

    static bool frag_warn_report = false;
    if(mem_mon.pct_frag > LV_APP_SYSMON_FRAG_WARN) {
        if(frag_warn_report == false) {
            frag_warn_report = true;
            lv_app_notice_add("Critical memory\nfragmentation");

            dm_defrag(); /*Defrag. if the fragmentation is critical*/
        }
    }

    if(mem_mon.pct_frag < LV_APP_SYSMON_FRAG_WARN)  frag_warn_report = false;
#endif
}

/**
 * Refresh the shortcuts and windows.
 */
static void lv_app_sysmon_refr(void)
{

    char buf_long[256];
    char buf_short[128];
    sprintf(buf_long, "%c%s CPU: %d %%%c\n\n",TXT_RECOLOR_CMD, CPU_LABEL_COLOR, cpu_pct[LV_APP_SYSMON_PNUM - 1], TXT_RECOLOR_CMD);
    sprintf(buf_short, "CPU: %d %%\n", cpu_pct[LV_APP_SYSMON_PNUM - 1]);

#if USE_DYN_MEM != 0  && DM_CUSTOM == 0
    sprintf(buf_long, "%s%c%s MEMORY: %d %%%c\nTotal: %d bytes\nUsed: %d bytes\nFree: %d bytes\nFrag: %d %%",
                  buf_long,
                  TXT_RECOLOR_CMD,
                  MEM_LABEL_COLOR,
                  mem_pct[LV_APP_SYSMON_PNUM - 1],
                  TXT_RECOLOR_CMD,
                  mem_mon.size_total,
                  mem_mon.size_total - mem_mon.size_free, mem_mon.size_free, mem_mon.pct_frag);

    sprintf(buf_short, "%sMem: %d %%\nFrag: %d %%\n",
                  buf_short, mem_pct[LV_APP_SYSMON_PNUM - 1], mem_mon.pct_frag);
#else
    sprintf(buf_long, "%s%c%s MEMORY: N/A%c", buf_long, TXT_RECOLOR_CMD, MEM_LABEL_COLOR, TXT_RECOLOR_CMD);
    sprintf(buf_short, "%sMem: N/A\nFrag: N/A", buf_short);
#endif
    lv_app_inst_t * app;
    app = lv_app_get_next(NULL, &my_app_dsc);
    while(app != NULL) {
        /*Refresh the windows*/
        my_win_data_t * win_data = app->win_data;
        if(win_data != NULL) {
            lv_label_set_text(win_data->label, buf_long);


            lv_chart_set_next(win_data->chart, win_data->mem_dl, mem_pct[LV_APP_SYSMON_PNUM - 1]);
            lv_chart_set_next(win_data->chart, win_data->cpu_dl, cpu_pct[LV_APP_SYSMON_PNUM - 1]);

        }
        /*Refresh the shortcut*/
        my_sc_data_t * sc_data = app->sc_data;
        if(sc_data != NULL) {
            lv_bar_set_value(sc_data->bar_cpu, cpu_pct[LV_APP_SYSMON_PNUM - 1]);
            lv_bar_set_value(sc_data->bar_mem, mem_pct[LV_APP_SYSMON_PNUM - 1]);
        }

        lv_app_com_send(app, LV_APP_COM_TYPE_CHAR, buf_short, strlen(buf_short));

        app = lv_app_get_next(app, &my_app_dsc);
    }
}
#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_SYSMON != 0*/
