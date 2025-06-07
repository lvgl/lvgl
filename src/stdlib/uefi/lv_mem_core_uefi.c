/**
 * @file lv_mem_core_uefi.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_mem.h"
#if LV_USE_UEFI
#if LV_UEFI_USE_MEMORY_SERVICES && LV_USE_STDLIB_MALLOC == LV_STDLIB_CUSTOM
#include "../drivers/uefi/lv_uefi_private.h"

/*********************
 *      DEFINES
 *********************/

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
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
typedef struct _mem_header_t {
    size_t size;
    uint8_t data[0];
} mem_header_t;

void lv_mem_init(void)
{
    LV_ASSERT_NULL(gLvEfiBS);

    return; /*Nothing to init*/
}

void lv_mem_deinit(void)
{
    return; /*Nothing to deinit*/
}

void * lv_malloc_core(size_t size)
{
    size_t size_with_header = size + sizeof(mem_header_t);
    mem_header_t * ptr = NULL;

    if(gLvEfiBS->AllocatePool(EfiBootServicesData, size_with_header, (void **)&ptr) != EFI_SUCCESS) return NULL;

    ptr->size = size;

    return ptr->data;
}

void * lv_realloc_core(void * p, size_t new_size)
{
    mem_header_t * p_header = NULL;
    uintptr_t p_address = (uintptr_t)p;
    void * p_new = NULL;

    if(p == NULL) return lv_malloc_core(new_size);
    // Check for invalid pointers
    if(p_address < sizeof(mem_header_t)) return NULL;

    p_address -= sizeof(mem_header_t);
    p_header = (mem_header_t *) p_address;

    // UEFI supports no realloc, if the size grows a new memory block has to be allocated
    if(p_header->size > new_size) return p;

    p_new = lv_malloc_core(new_size);
    lv_memcpy(p_new, p, p_header->size);
    lv_free_core(p);

    return p_new;
}

void lv_free_core(void * p)
{
    uintptr_t p_address = (uintptr_t)p;
    if(p_address < sizeof(mem_header_t)) return;

    p_address -= sizeof(mem_header_t);

    gLvEfiBS->FreePool((void *)p_address);
}

void lv_mem_monitor_core(lv_mem_monitor_t * mon_p)
{
    /*Not supported*/
    LV_UNUSED(mon_p);
    return;
}

lv_result_t lv_mem_test_core(void)
{
    /*Not supported*/
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
#endif