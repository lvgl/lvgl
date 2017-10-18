/**
 * @file systick.c
 * Provide access to the system tick with 1 millisecond resolution
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl/lv_hal/lv_hal_tick.h>
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/
#define LV_HAL_TICK_CALLBACK_NUM     32

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t sys_time = 0;
static volatile uint8_t tick_irq_flag;
static volatile uint8_t tick_cb_sem;    /*Semaphore for tick callbacks*/
static void (*tick_callbacks[LV_HAL_TICK_CALLBACK_NUM])(void);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Call this function in every milliseconds
 */
void lv_tick_handler(void)
{
    sys_time++;

    /*Run the callback functions*/
    uint8_t i;
    for (i = 0; i < LV_HAL_TICK_CALLBACK_NUM; i++) {
        if (tick_callbacks[i] != NULL) {
            tick_callbacks[i]();
        }
    }
    tick_irq_flag = 0;       /*lv_hal_systick_get set it to know there was an IRQ*/
}

/**
 * Get the elapsed milliseconds since start up
 * @return the elapsed milliseconds
 */
uint32_t lv_tick_get(void)
{
    uint32_t result;
    do {
        tick_irq_flag = 1;
        result = sys_time;
    } while(!tick_irq_flag);     /*Systick IRQ clears this flag. Continue until make a non interrupted cycle */

    return result;
}

/**
 * Get the elapsed milliseconds science a previous time stamp
 * @param prev_tick a previous time stamp ( return value of systick_get() )
 * @return the elapsed milliseconds since 'prev_tick'
 */
uint32_t lv_tick_elaps(uint32_t prev_tick)
{
	uint32_t act_time = lv_tick_get();

	/*If there is no overflow in sys_time simple subtract*/
	if(act_time >= prev_tick) {
		prev_tick = act_time - prev_tick;
	} else {
		prev_tick = UINT32_MAX - prev_tick + 1;
		prev_tick += act_time;
	}

	return prev_tick;
}

/**
 * Add a callback function to the systick interrupt
 * @param cb a function pointer
 * @return true: 'cb' added to the systick callbacks, false: 'cb' not added
 */
bool lv_tick_add_callback(void (*cb) (void))
{
    bool suc = false;

    /*Take the semaphore. Be sure it is set*/
    do {
        tick_cb_sem = 1;
    } while(!tick_cb_sem);

    uint8_t i;
    for (i = 0; i < LV_HAL_TICK_CALLBACK_NUM; i++) {
        if (tick_callbacks[i] == NULL) {
            tick_callbacks[i] = cb;
            suc = true;
            break;
        }
    }

    /*Release the semaphore. Be sure it is cleared*/
    do {
        tick_cb_sem = 0;
    } while(tick_cb_sem);

    return suc;
}

/**
 * Remove a callback function from the tick callbacks
 * @param cb a function pointer (added with 'lv_hal_tick_add_callback')
 */
void lv_tick_rem_callback(void (*cb) (void))
{
    /*Take the semaphore. Be sure it is set*/
    do {
        tick_cb_sem = 1;
    } while(!tick_cb_sem);

    uint8_t i;
    for (i = 0; i < LV_HAL_TICK_CALLBACK_NUM; i++) {
        if (tick_callbacks[i] == cb) {
            tick_callbacks[i] = NULL;
            break;
        }
    }

    /*Release the semaphore. Be sure it is cleared*/
    do {
        tick_cb_sem = 0;
    } while(tick_cb_sem);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

