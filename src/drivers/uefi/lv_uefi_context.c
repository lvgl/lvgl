/**
 * @file lv_uefi_context.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

#if LV_USE_UEFI

#include "lv_uefi_context.h"
#include "lv_uefi_private.h"

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
 *  GOLBAL VARIABLES
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

/**
 * @brief Initialize the UEFI chache variables.
 * @param image_handle The handle of the current image
 * @param system_table Pointer to the system table
 * @remark This has to be called before lv_init().
*/
void lv_uefi_init(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE * system_table)
{
    LV_ASSERT_NULL(image_handle);
    LV_ASSERT_NULL(system_table);

    gLvEfiImageHandle = image_handle;
    gLvEfiST = system_table;
    gLvEfiBS = gLvEfiST->BootServices;
    gLvEfiRT = gLvEfiST->RuntimeServices;
}

/**
 * @brief Initialize the LVGL UEFI backend.
 * @remark This is a private API which is used for LVGL UEFI backend
 *         implementation. LVGL users shouldn't use that because the
 *         LVGL has already used it in lv_init.
 */
void lv_uefi_platform_init(void)
{
    LV_ASSERT_NULL(gLvEfiImageHandle);
    LV_ASSERT_NULL(gLvEfiST);
    LV_ASSERT_NULL(gLvEfiBS);
    LV_ASSERT_NULL(gLvEfiRT);
}

/**
 * @brief Cleanup the LVGL UEFI backend.
 * @remark This is a private API which is used for LVGL UEFI backend
 *         implementation. LVGL users shouldn't use that because the
 *         LVGL has already used it in lv_deinit.
*/
void lv_uefi_platform_deinit(void)
{
    ;
}

#if LV_UEFI_USE_MEMORY_SERVICES && LV_USE_STDLIB_MALLOC == LV_STDLIB_CUSTOM
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

    // UEFI supportes no realloc, if the size grows a new memory block has to be allocated
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
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif