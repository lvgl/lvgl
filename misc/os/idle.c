/**
 * @file idle.c
 * Measure the CPU load by measuring how long can CPU count (increment the variable) 
 * if do nothing else in reference time.
 * After it in the main loop also counts. 
 * The ratio of the last count in given time and the reference count will give the idle ratio. 
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_IDLE != 0

#include MISC_SYSTICK_INCLUDE

/*********************
 *      DEFINES
 *********************/
#define IDLE_REF_MEAS_TIME   100    /*Start up reference measurement time in ms*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void idle_meas_cb(void);
static void idle_meas(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static volatile uint16_t idle_meas_interval = 1;    /*The current measure interval*/
static volatile uint32_t idle_cnt = 0;              /*Store the current counting*/
static volatile uint32_t idle_cnt_last = 0;         /*Saved idle counter from the last round*/
static volatile uint8_t idle_null = 0;              /*Query to clear the idle counter*/
static uint32_t idle_cnt_ref = 1;                   /*Reference counter store the count for 0% CPU load*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * Init the idle module. It will make a reference measurement.
 */
void idle_init(void)
{
    /*Set the reference measurement time*/
    idle_meas_interval = IDLE_REF_MEAS_TIME;
    
    MISC_SYSTICK_ADD_CB(idle_meas_cb);
    
    while(idle_cnt_last == 0) {
        idle_meas();
    }
    
    /*We know how many counts occurred in a reference time. 
     * Now calculate how many counts could occurred with IDLE_REFR_RATE */
    idle_cnt_ref = (uint32_t)((uint32_t)idle_cnt_last * IDLE_REFR_RATE) /
                    IDLE_REF_MEAS_TIME;
    
    /*Set up for the normal idle measurement*/
    idle_meas_interval = IDLE_REFR_RATE;
    idle_cnt_last = idle_cnt_ref;   /*Until the first meas. show 100% idle*/
}

/**
 * Return with the measured CPU idle percentage
 * @return The CPU idle in percentage 
 */
uint8_t idle_get(void)
{
    uint32_t cnt_tmp = idle_cnt_last;
    
    uint8_t idle_pct = (uint32_t)((uint32_t)cnt_tmp * 100 / idle_cnt_ref);
    
    return idle_pct;
}

/**
 * Use it instead of an empty while(1) loop to measure the idle time
 */
void idle_loop(void)
{
    volatile uint32_t stop = 0;
    while(stop == 0) idle_meas();
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Call in every ms to save 'idle_cnt' 
 */
static void idle_meas_cb(void)
{
    static uint16_t ms = 0;
    
    ms++;
    if(ms == idle_meas_interval) {
        idle_cnt_last = idle_cnt;
        idle_null = 1;
        idle_cnt = 0;
        ms = 0;
    }
}

/**
 * Used in idle_loop to measure the idle time
 */
static void idle_meas(void)
{
    /* Interesting: On assembly level the old value of idle_cnt can be kept so
     * clear is here as well not only in the interrupt */
    if(idle_null == 1 && idle_cnt != 0)
    {
        idle_null = 0;
        idle_cnt = 0;
    }

    /* Increment the idle_cnt
     * It will show how long was the CPU in the main loop.
     * If it is a smaller value then it was a lot of interrupt
     * so the CPU load is big */
    idle_cnt++;
}


#endif
