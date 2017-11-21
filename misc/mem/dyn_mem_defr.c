/**
 * @file dyn_mem.c
 * Implementation of special memory allocation which
 * always provides 0 % fragmentation.
 * It is NOT COMAPTIBLE with normal malloc/free.
 */

/*
 * !!!IMPORTANT!!!!
 * It is NOT COMAPTIBLE with normal malloc/free.
 * The dmd_alloc function can allocate a new memory and give pointer to it
 * but this pointer is special:
 * - to store this special pointer use the dp tag (means Dynamic Pointer)
 *   e.g. int dp * x = dmd_alloc(sizeof(int)) (and not simply int * x)
 * - to convert this pointer to normal pointer use the da() tag (means Dynamic Access)
 *   e.g. *da(x) = 5;
 * - never store the pointer converted by da(), only use it locally
 * 
 * How dose it works?
 * This type of allocation separates the memory entry descriptors
 * from the allocated memory chunks. The descriptors are located
 * at the end of memory and contains a pointer to their memory chunk.
 * dmd_alloc gives a pointer to a descriptor which stores the pointer
 * of the memory chunk. So it is a void ** and NOT void *.
 * That is why it is not compatible with malloc.
 * Because the memory chunk addressed are not stored by the user
 * they can be moved to achieve 0 fragmentation.
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_DYN_MEM_DEFR != 0

#include "dyn_mem_defr.h"
#include "../math/math_base.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/

static dmd_ent_t  * ent_get_next(dmd_ent_t * act_e);
static dmd_ent_t * ent_get_next_ord(dmd_ent_t * e);
static void * ent_alloc(dmd_ent_t * e, uint32_t size);
static dmd_ent_t * ent_trunc(dmd_ent_t * e, uint32_t size);

/**********************
 *  STATIC VARIABLES
 **********************/
static DM_MEM_ATTR uint8_t work_mem[DM_MEM_SIZE] ;    //Work memory for allocation 
static uint32_t zero_mem;       /*Give the address of this variable if 0 byte should be allocated*/ 
static dmd_ent_t * master_e;     /*The 1 big free entry*/
static dmd_ent_t * last_e;       /*The last entry*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initiaize the dyn_mem module (work memory and other variables)
 */
void dmd_init(void)
{
    dmd_ent_t * e = (dmd_ent_t *)&work_mem[DM_MEM_SIZE - sizeof(dmd_ent_t)];
    e->header.d_size = DM_MEM_SIZE - sizeof(dmd_ent_t);
    e->header.used = 0;
    e->data_p = &work_mem[0];
 
    master_e = e;
    last_e = e;
}

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory 
 * !!!IMPORTANT!!! it is special pointer,  see the file header for more information
 */
void * dmd_alloc(uint32_t size)
{
    if(size == 0) {
        return &zero_mem;
    }
    
    /*Round the size up to 4*/
    if(size & 0x3 ) { 
        size = size & (~0x3);
        size += 4;
    }    
    
    void * alloc = NULL;
    alloc = ent_alloc(master_e, size);
    return alloc;
}

/**
 * Free an allocated data
 * @param data pointer to an allocated memory 
 * (without da() tag, see file header for more information)
 */
void dmd_free(void * data)
{    
    if(data == &zero_mem) return;
    if(data == NULL) return;
    
    /*Mark the entry as free*/
    dmd_ent_t * free_e = data;
    free_e->header.used = 0;
    
    /*Join this free area into the master_e.
     To achieve this move all data after p forward with p->header.d_size spaces.
     Hence p->data will shifted directly before  master_e->data_p */
    dmd_ent_t * next_e;
    uint32_t i;
    
    /*Pre-load the next_e*/
    next_e = ent_get_next_ord(free_e);
    /*Iterate through all entries after act_e until reach master_e*/
    while(next_e != master_e) {
        /* Swap the data of entries and their data_p */
        for(i = 0; i < next_e->header.d_size; i++) {
            free_e->data_p[i] = next_e->data_p[i];
        }

        next_e->data_p = free_e->data_p; /*Put the next on the free*/
        free_e->data_p = &next_e->data_p[next_e->header.d_size]; /*Put the free after the next*/
        
        /* Get the next e in order*/
        next_e = ent_get_next_ord(free_e);
    }
    
    /*Join free_e with master_e*/
    master_e->header.d_size += free_e->header.d_size;
    master_e->data_p = free_e->data_p;
    
    /*Mark the entry position as free*/
    memset(free_e->data_p, 0, free_e->header.d_size);
    free_e->data_p = NULL;
    free_e->header.d_size = 0;
    
    /* Increment last_e if it was freed and increase master_e size*/
    if(free_e == last_e) {
        do {
            master_e->header.d_size += sizeof(dmd_ent_t);
            last_e ++;
        }while(last_e->data_p == NULL);
        
    }
}

/**
 * Reallocate a memory with a the size. The old content will be kept.
 * @param data pointer to an allocated memory. (without da() tag see file header for more information)
 * Its content will be copied to the new memory block and freed
 * @param new_size the desires new size in byte
 * @return pointer to the new memory
 * !!!IMPORTANT!!! it is special pointer,  see the file header for more information

 */
void * dmd_realloc(void * data_p, uint32_t new_size)
{
    uint8_t dp * new_p;
    
    new_p = dmd_alloc(new_size);
    
    if(new_p != NULL && data_p != NULL) {
        /*Copy the old data to the new. Use the smaller size*/
        uint32_t old_size = dmd_get_size(data_p);
        if(old_size != 0) {
            memcpy(da(new_p), da(((uint8_t dp *)data_p)), min(new_size, old_size));
            dmd_free(data_p);
        }
    }
    
    return new_p;    
}


/**
 * Give information about the work memory of dynamic allocation
 * @param mon_p pointer to a mon_p variable, the result of the analysis will be stored here
 */
void dmd_monitor(dmd_mon_t * mon_p)
{
    //Init the data
    memset(mon_p, 0, sizeof(dmd_mon_t));
    
    dmd_ent_t * e;
    e = NULL;
    
    e = ent_get_next(e);
    
    while(e != NULL)  {
        if(e->header.used == 0) {
            mon_p->cnt_free++;
            mon_p->size_free += e->header.d_size;
            if(e->header.d_size > mon_p->size_free_big) {
                mon_p->size_free_big = e->header.d_size;
            }
        } else {
            mon_p->cnt_used++;
        }
        
        e = ent_get_next(e);
    }
    
    mon_p->pct_frag = (uint32_t)mon_p->size_free_big * 100U / mon_p->size_free;
    mon_p->pct_frag = 100 - mon_p->pct_frag;
}

/**
 * Give the size a allocated memory
 * @param data pointer to an allocated memory (without da() tag see file header for more information )
 * @return the size of data memory in bytes 
 */
uint32_t dmd_get_size(void * data)
{
    if(data == &zero_mem) return 0;
    
    dmd_ent_t * e = data;   /*p is entry*/
    return e->header.d_size;

}
/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Give the next entry after 'act_e'
 * @param act_e pointer to an entry
 * @return pointer to an entry after 'act_e'
 */
static dmd_ent_t  * ent_get_next(dmd_ent_t * act_e)
{
    dmd_ent_t * next_e = NULL;

    if(act_e == NULL) {
        next_e = master_e;
    } else {
        next_e = act_e - 1;
        
        if(next_e < last_e) {
            next_e = NULL;
        }
    }
    
    return next_e;
}

/**
 * With DM_POL_DEFRAG the order of entries not same like the order of allocated memories.
 * This function gives the next entry in the order of allocated memories 
 * @param e pointer to an entry 
 * @return pointer to an entry which data begins after the data of 'e'
 */
static dmd_ent_t * ent_get_next_ord(dmd_ent_t * e)
{
    dmd_ent_t * next_e;
    
    uint8_t * next_data_p = &e->data_p[e->header.d_size];
    
    for(next_e = last_e; next_e < master_e; next_e++) {
        if(next_e->data_p == next_data_p) {
            break;
        }
    }
    
    return next_e;
}   
 

/**
 * Try to do the real allocation with a given size
 * @param e try to allocate to this entry
 * @param size size of the new memory in bytes
 * @return pointer to the allocated memory or NULL if not enough memory in the entry
 */
static void * ent_alloc(dmd_ent_t * e, uint32_t size)
{
    void * alloc = NULL;
    
    /* Always allocate to the master_e (e == master_e) */
    
    /* In master_e has to remain at leat 2 entry space:
     * 1 entry space to ensure space for new allocation and
     * an other to be sure master_e size will not be 0 */
    if(e->header.d_size >= size + 2 * sizeof(dmd_ent_t))
    {
        alloc = ent_trunc(e, size);       
    }
    
    return alloc;
}
 
/**
 * Truncate the data of entry to the given size
 * @param e Pointer to an entry
 * @param size new size in bytes
 * @return the new entry created from the remaining memory
 */
static dmd_ent_t * ent_trunc(dmd_ent_t * e, uint32_t size)
{
    dmd_ent_t * new_e;
    
    /*Search place for a new entry at the end of the memory*/
    for(new_e = master_e; new_e >= last_e; new_e--) {
        /*Free entry found before last_e*/
        if(new_e->data_p == NULL) {
            break;
        }
    }
    
    if(new_e < last_e) {
        /*New entry is allocated so master entry become smaller*/
        master_e->header.d_size -= sizeof(dmd_ent_t);
        last_e --;
    }
    
    /*Set the parameters in the new last_e*/
    new_e->header.d_size = size;
    new_e->header.used = 1;
    new_e->data_p = master_e->data_p;
    
    /*Update master_e*/
    master_e->data_p = master_e->data_p + size;
    master_e->header.d_size -= size;
    
    return new_e;
}

#endif
