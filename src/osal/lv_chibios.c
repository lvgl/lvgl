/**
 * @file lv_chibios.c
 *
 * Port of LVGL OS abstraction layer using ChibiOS.
 *
 */

#include "lv_os.h"
#if LV_USE_OS == LV_OS_CHIBIOS
#include "../core/lv_global.h"

/**********************
 *   STATIC PROTOTYPES
 **********************/
static void prvRunThread(void * arg);
static void prvMutexInit(lv_mutex_t * pxMutex);
static void prvCheckMutexInit(lv_mutex_t * pxMutex);
static void prvCondInit(lv_thread_sync_t * pxCond);
static void prvCheckCondInit(lv_thread_sync_t * pxCond);

/**********************
 *   STATIC VARIABLES
 **********************/
#define globals LV_GLOBAL_DEFAULT()

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_result_t lv_thread_init(lv_thread_t * pxThread, const char * const name,
                           lv_thread_prio_t xSchedPriority,
                           void (*pvStartRoutine)(void *), size_t usStackSize,
                           void * xAttr)
{
    pxThread->pTaskArg = xAttr;
    pxThread->pvStartRoutine = pvStartRoutine;
    pxThread->pThreadHandle = chThdCreateFromHeap(NULL, usStackSize, name, (NORMALPRIO + xSchedPriority),
                                                  prvRunThread, pxThread);
    if(pxThread->pThreadHandle == NULL) {
        LV_LOG_ERROR("chThdCreateFromHeap failed!");
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_thread_delete(lv_thread_t * pxThread)
{
    chThdTerminate(pxThread->pThreadHandle);
    chThdWait(pxThread->pThreadHandle);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_init(lv_mutex_t * pxMutex)
{
    prvCheckMutexInit(pxMutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock(lv_mutex_t * pxMutex)
{
    prvCheckMutexInit(pxMutex);
    chMtxLock(&pxMutex->mtx);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_unlock(lv_mutex_t * pxMutex)
{
    prvCheckMutexInit(pxMutex);
    chMtxUnlock(&pxMutex->mtx);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_delete(lv_mutex_t * pxMutex)
{
    pxMutex->is_initialized = false;
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * pxCond)
{
    prvCheckCondInit(pxCond);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * pxCond)
{
    lv_result_t res = LV_RESULT_OK;
    prvCheckCondInit(pxCond);

    chMtxLock(&pxCond->sync_mtx);
    if(pxCond->sync_signal) {
        pxCond->sync_signal = false;
        chMtxUnlock(&pxCond->sync_mtx);
    }
    else {
        pxCond->waiting_threads++;
        chMtxUnlock(&pxCond->sync_mtx);

        chBSemWait(&pxCond->bsem);
    }
    return res;
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * pxCond)
{
    prvCheckCondInit(pxCond);

    chMtxLock(&pxCond->sync_mtx);
    if(pxCond->waiting_threads > 0) {
        uint32_t threads_to_release = pxCond->waiting_threads;
        pxCond->waiting_threads = 0;
        chMtxUnlock(&pxCond->sync_mtx);

        for(uint32_t i = 0; i < threads_to_release; i++) {
            chBSemSignal(&pxCond->bsem);
        }
    }
    else {
        pxCond->sync_signal = true;
        chMtxUnlock(&pxCond->sync_mtx);
    }
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * pxCond)
{
    chMtxLock(&pxCond->sync_mtx);
    pxCond->waiting_threads = 0;
    pxCond->sync_signal = false;
    chMtxUnlock(&pxCond->sync_mtx);
    pxCond->is_initialized = false;
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * pxCond)
{
    prvCheckCondInit(pxCond);

    chSysLockFromISR();
    uint32_t threads_to_release = pxCond->waiting_threads;
    if(threads_to_release > 0) {
        pxCond->waiting_threads = 0;
    }
    else {
        pxCond->sync_signal = true;
    }

    for(uint32_t i = 0; i < threads_to_release; i++) {
        chBSemSignalI(&pxCond->bsem);
    }
    chSysUnlockFromISR();

    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void prvRunThread(void * arg)
{
    lv_thread_t * pxThread = (lv_thread_t *)arg;
    pxThread->pvStartRoutine(pxThread->pTaskArg);
}

static void prvMutexInit(lv_mutex_t * pxMutex)
{
    chMtxObjectInit(&pxMutex->mtx);
    pxMutex->is_initialized = true;
}

static void prvCheckMutexInit(lv_mutex_t * pxMutex)
{
    if(!pxMutex->is_initialized) {
        chSysLock();
        if(!pxMutex->is_initialized) {
            prvMutexInit(pxMutex);
        }
        chSysUnlock();
    }
}

static void prvCondInit(lv_thread_sync_t * pxCond)
{
    pxCond->is_initialized = true;
    pxCond->sync_signal = false;
    pxCond->waiting_threads = 0;
    chBSemObjectInit(&pxCond->bsem, FALSE);
    chMtxObjectInit(&pxCond->sync_mtx);
}

static void prvCheckCondInit(lv_thread_sync_t * pxCond)
{
    if(!pxCond->is_initialized) {
        chSysLock();
        prvCondInit(pxCond);
        chSysUnlock();
    }
}

#if LV_USE_SYSMON

_Static_assert(CH_DBG_STATISTICS == TRUE, "CH_DBG_STATISTICS must be TRUE in chconf.h");
_Static_assert(CH_CFG_USE_TM == TRUE, "CH_CFG_USE_TM must be TRUE in chconf.h");

static volatile uint32_t last_idle_pct = 0;

static THD_WORKING_AREA(waIdleStat, 128);
static THD_FUNCTION(IdleStatThread, arg)
{
    (void)arg;
    chRegSetThreadName("IdleStat");

    thread_t * idle_tp = chSysGetIdleThreadX();
    uint64_t prev_idle = idle_tp->stats.cumulative;
    uint64_t prev_total = 0;

    while(true) {
        chThdSleepMilliseconds(1000);
        uint64_t total = 0;
        thread_t * tp = chRegFirstThread();
        do {
            total += tp->stats.cumulative;
            tp = chRegNextThread(tp);
        } while(tp);

        uint64_t idle = idle_tp->stats.cumulative;
        uint64_t delta_total = total - prev_total;
        uint64_t delta_idle = idle - prev_idle;

        last_idle_pct = (uint32_t)((delta_idle * 100UL) / delta_total);

        prev_total = total;
        prev_idle = idle;
    }
}

static void idleStatInit(void)
{
    static bool initialized = false;

    if(initialized) {
        return;
    }

    initialized = true;

    chThdCreateStatic(waIdleStat, sizeof(waIdleStat), NORMALPRIO - 1, IdleStatThread, NULL);
}

uint32_t lv_os_get_idle_percent(void)
{
    idleStatInit();
    return last_idle_pct;
}
#endif
#endif /* LV_USE_OS == LV_OS_CHIBIOS */