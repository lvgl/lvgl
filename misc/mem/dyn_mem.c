/**
 * @file dyn_mem.c
 * General and portable implementation of malloc and free.
 * The dynamic memory monitoring is also supported.
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_DYN_MEM != 0

#include "dyn_mem.h"
#include "../math/math_base.h"
#include <string.h>

#if DM_CUSTOM != 0
#include DM_CUST_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*The size of this union must be 4 bytes (uint32_t)*/
typedef union
{
	struct
	{
		uint32_t used:1;        //1: if the entry is used
		uint32_t d_size:31;     //Size off the data (1 means 4 bytes)
	};
	uint32_t header;            //The header (used + d_size)
}dm_header_t;

typedef struct
{
    dm_header_t header;
    uint8_t first_data;        //First data in entry data
}dm_ent_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if DM_CUSTOM == 0
static dm_ent_t  * ent_get_next(dm_ent_t * act_e);
static void * ent_alloc(dm_ent_t * e, uint32_t size);
static dm_ent_t * ent_trunc(dm_ent_t * e, uint32_t size);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if DM_CUSTOM == 0
static DM_MEM_ATTR uint8_t work_mem[DM_MEM_SIZE] ;    //Work memory for allocation
#endif

static uint32_t zero_mem;       /*Give the address of this variable if 0 byte should be allocated*/ 

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initiaize the dyn_mem module (work memory and other variables)
 */
void dm_init(void)
{
#if DM_CUSTOM == 0
    dm_ent_t * full = (dm_ent_t *)&work_mem;
    full->header.used = 0;
    /*The total mem size id reduced by the first header and the close patterns */
    full->header.d_size = DM_MEM_SIZE - sizeof(dm_header_t);
#endif
}

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory 
 */
void * dm_alloc(uint32_t size)
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

#if DM_CUSTOM == 0 /*Use the allocation from dyn_mem*/
    dm_ent_t * e = NULL;
    
    //Search for a appropriate entry
    do {
        //Get the next entry
        e = ent_get_next(e);
        
        //If there is next entry then try to allocate there
        if(e != NULL) {
            alloc = ent_alloc(e, size);
        }
    //End if there is not next entry OR the alloc. is successful
    }while(e != NULL && alloc == NULL); 
#else  /*Use custom, user defined malloc function*/
    /*Allocate a header too to store the size*/
    alloc = DM_CUST_ALLOC(size + sizeof(dm_header_t));
    if(alloc != NULL) {
        ((dm_ent_t*) alloc)->header.d_size = size;
        ((dm_ent_t*) alloc)->header.used = 1;
        alloc = &((dm_ent_t*) alloc)->first_data;
    }
#endif

#if DM_AUTO_ZERO != 0
    if(alloc != NULL) memset(alloc, 0, size);
#endif

    return alloc;
}

/**
 * Free an allocated data
 * @param data pointer to an allocated memory 
 */
void dm_free(const void * data)
{    
    if(data == &zero_mem) return;
    if(data == NULL) return;

    /*e points to the header*/
    dm_ent_t * e = (dm_ent_t *)((uint8_t *) data - sizeof(dm_header_t));
    e->header.used = 0;

#if DM_CUSTOM == 0 /*Use the free from dyn_mem*/
    /* Make a simple defrag.
     * Join the following free entries after this*/
    dm_ent_t * e_next;
    e_next = ent_get_next(e);
    while(e_next != NULL) {
        if(e_next->header.used == 0) {
            e->header.d_size += e_next->header.d_size + sizeof(e->header);
        } else {
            break;
        }
        e_next = ent_get_next(e_next);
    }
#else /*Use custom, user defined free function*/
    DM_CUST_FREE(e);
#endif
}

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data pointer to an allocated memory.
 * Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory
 */
void * dm_realloc(void * data_p, uint32_t new_size)
{
    /*data_p could be previously freed pointer (in this case it is invalid)*/
    if(data_p != NULL) {
        dm_ent_t * e = (dm_ent_t *)((uint8_t *) data_p - sizeof(dm_header_t));
        if(e->header.used == 0) data_p = NULL;
    }

    uint32_t old_size = dm_get_size(data_p);
    if(old_size == new_size) return data_p;

    void * new_p;
    new_p = dm_alloc(new_size);
    
    if(new_p != NULL && data_p != NULL) {
        /*Copy the old data to the new. Use the smaller size*/
        if(old_size != 0) {
            memcpy(new_p, data_p, MATH_MIN(new_size, old_size));
            dm_free(data_p);
        }
    }
    
    return new_p;    
}

/**
 * Join the adjacent free memory blocks
 */
void dm_defrag(void)
{
#if DM_CUSTOM == 0
    dm_ent_t * e_free;
    dm_ent_t * e_next;
    e_free = ent_get_next(NULL);

    while(1) {
        /*Search the next free entry*/
        while(e_free != NULL) {
            if(e_free->header.used != 0) {
                e_free = ent_get_next(e_free);
            } else {
                break;
            }
        }

        if(e_free == NULL) return;

        /*Joint the following free entries to the free*/
        e_next = ent_get_next(e_free);
        while(e_next != NULL) {
            if(e_next->header.used == 0) {
                e_free->header.d_size += e_next->header.d_size + sizeof(e_next->header);
            } else {
                break;
            }

            e_next = ent_get_next(e_next);
        }

        if(e_next == NULL) return;

        /*Continue from the lastly checked entry*/
        e_free = e_next;
    }
#endif
}

/**
 * Give information about the work memory of dynamic allocation
 * @param mon_p pointer to a dm_mon_p variable, 
 *              the result of the analysis will be stored here
 */
void dm_monitor(dm_mon_t * mon_p)
{
    /*Init the data*/
    memset(mon_p, 0, sizeof(dm_mon_t));
#if DM_CUSTOM == 0
    dm_ent_t * e;
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
    mon_p->size_total = DM_MEM_SIZE;
    mon_p->pct_used = 100 - (100U * mon_p->size_free) / mon_p->size_total;
    mon_p->pct_frag = (uint32_t)mon_p->size_free_big * 100U / mon_p->size_free;
    mon_p->pct_frag = 100 - mon_p->pct_frag;
#endif
}

/**
 * Give the size of an allocated memory
 * @param data pointer to an allocated memory
 * @return the size of data memory in bytes 
 */
uint32_t dm_get_size(void * data)
{
    if(data == NULL) return 0;
    if(data == &zero_mem) return 0;
    
    dm_ent_t * e = (dm_ent_t *)((uint8_t *) data - sizeof(dm_header_t));

    return e->header.d_size;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

#if DM_CUSTOM == 0
/**
 * Give the next entry after 'act_e'
 * @param act_e pointer to an entry
 * @return pointer to an entry after 'act_e'
 */
static dm_ent_t * ent_get_next(dm_ent_t * act_e)
{
    dm_ent_t * next_e = NULL;

    if(act_e == NULL) { /*NULL means: get the first entry*/ 
        next_e = (dm_ent_t * ) work_mem;
    }
    else /*Get the next entry */
    {
        uint8_t * data = &act_e->first_data;
        next_e = (dm_ent_t * )&data[act_e->header.d_size];
        
        if(&next_e->first_data >= &work_mem[DM_MEM_SIZE]) next_e = NULL;
    }
    
    return next_e;
}


/**
 * Try to do the real allocation with a given size
 * @param e try to allocate to this entry
 * @param size size of the new memory in bytes
 * @return pointer to the allocated memory or NULL if not enough memory in the entry
 */
static void * ent_alloc(dm_ent_t * e, uint32_t size)
{
    void * alloc = NULL;
    
    /*If the memory is free and big enough ten use it */
    if(e->header.used == 0 && e->header.d_size >= size) {
        /*Truncate the entry to the desired size */
        ent_trunc(e, size),
        
        e->header.used = 1;
        
        /*Save the allocated data*/
        alloc = &e->first_data;
    }
    
    return alloc;
}
 
/**
 * Truncate the data of entry to the given size
 * @param e Pointer to an entry
 * @param size new size in bytes
 * @return the new entry created from the remaining memory
 */
static dm_ent_t * ent_trunc(dm_ent_t * e, uint32_t size)
{
    dm_ent_t * new_e;
    
    /*Do let empty space  only for a header withot data*/
    if(e->header.d_size == size + sizeof(dm_header_t)) {
        size += sizeof(dm_header_t);
    }

    /* Create the new entry after the current if there is space for it */
    if(e->header.d_size != size) {
        uint8_t * e_data = &e->first_data;
        dm_ent_t * new_e = (dm_ent_t *)&e_data[size];
        new_e->header.used = 0;
        new_e->header.d_size = e->header.d_size - size - sizeof(dm_header_t);
    }
    
    /* Set the new size for the original entry */
    e->header.d_size = size;
    new_e = e;
    
    return new_e;
}

#endif /*DM_CUSTOM == 0*/

#endif
