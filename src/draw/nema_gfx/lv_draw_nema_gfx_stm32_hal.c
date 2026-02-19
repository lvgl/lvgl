/**
 * @file lv_draw_nema_gfx_stm32_hal.c
 *
 * Global functions that implement some HAL functionality
 * which Nema will call directly.
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_NEMA_GFX

#if LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32

#include "lv_draw_nema_gfx_utils.h"

#include "../../misc/lv_types.h"
#include "../../misc/lv_assert.h"
#include "../../stdlib/lv_string.h"
#include "../../osal/lv_os_private.h"

#include <assert.h>
#include <string.h>

#include LV_NEMA_STM32_HAL_INCLUDE

#if defined(__ZEPHYR__)
    #include <zephyr/kernel.h>
    #include <zephyr/irq.h>
    static struct k_sem gpu2d_sync;
    static GPU2D_HandleTypeDef hgpu2d;
#else
    static lv_thread_sync_t sync;
    extern GPU2D_HandleTypeDef hgpu2d;
#endif
static volatile int last_cl_id = -1;
/*********************
 *      DEFINES
 *********************/
#define RING_SIZE                      1024 /* Ring Buffer Size in byte */

/* NemaGFX byte pool size in bytes.
 * One byte per peixel for masking/stencling plus 10240 for additional allocations.
 */
#if defined(LV_NEMA_GFX_MAX_RESX) && defined(LV_NEMA_GFX_MAX_RESY)
    #define NEMAGFX_MEM_POOL_SIZE          ((LV_NEMA_GFX_MAX_RESX * LV_NEMA_GFX_MAX_RESY) + 10240)
#else
    /* LV_USE_NEMA_VG is 0 so masking/stencling memory is not needed. */
    #define NEMAGFX_MEM_POOL_SIZE          10240
#endif

#ifndef LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM
    #define LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if defined(__ZEPHYR__)
    static void stm32_gpu2d_isr(const void * arg);
    static void stm32_gpu2d_interrupt_init(void);
#endif

#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    static void GPU2D_CommandListCpltCallback(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_NEMA_LIB == LV_NEMA_LIB_M55
    static uint8_t nemagfx_pool_mem[NEMAGFX_MEM_POOL_SIZE] LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM; /* NemaGFX memory pool */
#else
    static uint8_t nemagfx_pool_mem[NEMAGFX_MEM_POOL_SIZE]; /* NemaGFX memory pool */
#endif
static nema_ringbuffer_t ring_buffer_str;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#if defined(__ZEPHYR__)
static void stm32_gpu2d_isr(const void * arg)
{
    HAL_GPU2D_IRQHandler(&hgpu2d);
}
#endif

#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    static void GPU2D_CommandListCpltCallback(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID)
#else
    void HAL_GPU2D_CommandListCpltCallback(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID)
#endif
{
    LV_UNUSED(hgpu2d);

    last_cl_id = CmdListID;
#if defined(__ZEPHYR__)
    k_sem_give(&gpu2d_sync);
#else
    lv_thread_sync_signal_isr(&sync);
#endif
}

#if defined(__ZEPHYR__)
static void stm32_gpu2d_interrupt_init(void)
{
    IRQ_CONNECT(DT_IRQN(DT_NODELABEL(gpu2d)),
                DT_IRQ_BY_NAME(DT_NODELABEL(gpu2d), gpu2d, priority),
                stm32_gpu2d_isr,
                NULL,
                0);
    irq_enable(DT_IRQN(DT_NODELABEL(gpu2d)));
}
#endif

int32_t nema_sys_init(void)
{
    int error_code = 0;

#if defined(__ZEPHYR__)
    /* Enable GPU2D clock */
    __HAL_RCC_GPU2D_CLK_ENABLE();
    /* Force reset GPU2D peripheral */
    __HAL_RCC_GPU2D_FORCE_RESET();
    __HAL_RCC_GPU2D_RELEASE_RESET();
    /* Enable ICACHE and set Associativity Mode*/
    HAL_ICACHE_Enable();
    HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY);

    hgpu2d.Instance = GPU2D;

    if(HAL_GPU2D_Init(&hgpu2d) != HAL_OK) {
        LV_LOG_ERROR("HAL_GPU2D_Init failed\n");
        return -1;
    }

    /* Configure  GPU2D Interrupt */
    stm32_gpu2d_interrupt_init();

    k_sem_init(&gpu2d_sync, 0, 1);
#else
    lv_thread_sync_init(&sync);
#endif

    /* Setup GPU2D Callback */
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    /* Register Command List Complete Callback */
    HAL_GPU2D_RegisterCommandListCpltCallback(&hgpu2d, GPU2D_CommandListCpltCallback);
#endif

    /* Initialise Mem Space */
    error_code = tsi_malloc_init_pool_aligned(0, (void *)nemagfx_pool_mem, (uintptr_t)nemagfx_pool_mem,
                                              NEMAGFX_MEM_POOL_SIZE, 1, 8);
    LV_ASSERT(error_code == 0);

    /* Allocate ring_buffer memory */
    ring_buffer_str.bo = nema_buffer_create(RING_SIZE);
    LV_ASSERT(ring_buffer_str.bo.base_virt);

    /* Initialize Ring Buffer */
    error_code = nema_rb_init(&ring_buffer_str, 1);
    if(error_code < 0) {
        return error_code;
    }

    /* Reset last_cl_id counter */
    last_cl_id = 0;

    return error_code;
}

uint32_t nema_reg_read(uint32_t reg)
{
    return HAL_GPU2D_ReadRegister(&hgpu2d, reg);
}

void nema_reg_write(uint32_t reg, uint32_t value)
{
    HAL_GPU2D_WriteRegister(&hgpu2d, reg, value);
}




int nema_wait_irq(void)
{
#if defined(__ZEPHYR__)
    return k_sem_take(&gpu2d_sync, K_FOREVER);
#else
    lv_thread_sync_wait(&sync);
    return 0;
#endif
}


int nema_wait_irq_cl(int cl_id)
{
    while(last_cl_id < cl_id) {
        (void)nema_wait_irq();
    }

    return 0;
}

int nema_wait_irq_brk(int brk_id)
{
    while(nema_reg_read(GPU2D_BREAKPOINT) == 0U) {
        (void)nema_wait_irq();
    }

    return 0;
}

void nema_host_free(void * ptr)
{
    tsi_free(ptr);
}

void * nema_host_malloc(unsigned size)
{
    return tsi_malloc(size);
}

nema_buffer_t nema_buffer_create(int size)
{
    nema_buffer_t bo;
    lv_memset(&bo, 0, sizeof(bo));
    bo.base_virt = tsi_malloc(size);
    bo.base_phys = (uint32_t)bo.base_virt;
    bo.size      = size;
    LV_ASSERT_MSG(bo.base_virt != 0, "Unable to allocate memory in nema_buffer_create");

    return bo;
}

nema_buffer_t nema_buffer_create_pool(int pool, int size)
{
    LV_UNUSED(pool);

    return nema_buffer_create(size);
}

void * nema_buffer_map(nema_buffer_t * bo)
{
    return bo->base_virt;
}

void nema_buffer_unmap(nema_buffer_t * bo)
{
    LV_UNUSED(bo);
}

void nema_buffer_destroy(nema_buffer_t * bo)
{
    if(bo->fd == -1) {
        return; /* Buffer weren't allocated! */
    }

    tsi_free(bo->base_virt);

    bo->base_virt = (void *)0;
    bo->base_phys = 0;
    bo->size      = 0;
    bo->fd        = -1; /* Buffer not allocated */
}

uintptr_t nema_buffer_phys(nema_buffer_t * bo)
{
    return bo->base_phys;
}

void nema_buffer_flush(nema_buffer_t * bo)
{
    LV_UNUSED(bo);
}

int nema_mutex_lock(int mutex_id)
{
    int retval = 0;

    LV_UNUSED(mutex_id);

    return retval;
}

int nema_mutex_unlock(int mutex_id)
{
    int retval = 0;

    LV_UNUSED(mutex_id);

    return retval;
}

void platform_disable_cache(void)
{

}

void platform_invalidate_cache(void)
{

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32 */

#endif /* LV_USE_NEMA_GFX */
