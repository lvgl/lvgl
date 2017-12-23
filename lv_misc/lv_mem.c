/**
 * @file lv_mem.c
 * General and portable implementation of malloc and free.
 * The dynamic memory monitoring is also supported.
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#include "lv_mem.h"
#include "lv_math.h"
#include <string.h>

#if LV_MEM_CUSTOM != 0
#include LV_MEM_CUSTOM_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/
#define LV_MEM_ADD_JUNK     0   /*Add memory junk on alloc (0xaa) and free(0xbb) (just for testing purposes)*/

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
}lv_mem_header_t;

typedef struct
{
    lv_mem_header_t header;
    uint8_t first_data;        /*First data byte in the allocated data (Just for easily create a pointer)*/
}lv_mem_ent_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_MEM_CUSTOM == 0
static lv_mem_ent_t  * ent_get_next(lv_mem_ent_t * act_e);
static void * ent_alloc(lv_mem_ent_t * e, uint32_t size);
static void ent_trunc(lv_mem_ent_t * e, uint32_t size);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_MEM_CUSTOM == 0
static LV_MEM_ATTR uint8_t work_mem[LV_MEM_SIZE];    /*Work memory for allocations*/
#endif

static uint32_t zero_mem;       /*Give the address of this variable if 0 byte should be allocated*/ 

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initiaiize the dyn_mem module (work memory and other variables)
 */
void lv_mem_init(void)
{
#if LV_MEM_CUSTOM == 0
    lv_mem_ent_t * full = (lv_mem_ent_t *)&work_mem;
    full->header.used = 0;
    /*The total mem size id reduced by the first header and the close patterns */
    full->header.d_size = LV_MEM_SIZE - sizeof(lv_mem_header_t);
#endif
}

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory 
 */
void * lv_mem_alloc(uint32_t size)
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

#if LV_MEM_CUSTOM == 0 /*Use the allocation from dyn_mem*/
    lv_mem_ent_t * e = NULL;
    
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

#if LV_MEM_ADD_JUNK
    if(alloc != NULL) memset(alloc, 0xaa, size);
#endif

#else  /*Use custom, user defined malloc function*/
    /*Allocate a header too to store the size*/
    alloc = LV_MEM_CUSTOM_ALLOC(size + sizeof(lv_mem_header_t));
    if(alloc != NULL) {
        ((lv_mem_ent_t*) alloc)->header.d_size = size;
        ((lv_mem_ent_t*) alloc)->header.used = 1;
        alloc = &((lv_mem_ent_t*) alloc)->first_data;
    }
#endif

    return alloc;
}

/**
 * Free an allocated data
 * @param data pointer to an allocated memory 
 */
void lv_mem_free(const void * data)
{    
    if(data == &zero_mem) return;
    if(data == NULL) return;


#if LV_MEM_ADD_JUNK
    memset((void*)data, 0xbb, lv_mem_get_size(data));
#endif

    /*e points to the header*/
    lv_mem_ent_t * e = (lv_mem_ent_t *)((uint8_t *) data - sizeof(lv_mem_header_t));
    e->header.used = 0;

#if LV_MEM_CUSTOM == 0
#if LV_MEM_AUTO_DEFRAG
    /* Make a simple defrag.
     * Join the following free entries after this*/
    lv_mem_ent_t * e_next;
    e_next = ent_get_next(e);
    while(e_next != NULL) {
        if(e_next->header.used == 0) {
            e->header.d_size += e_next->header.d_size + sizeof(e->header);
        } else {
            break;
        }
        e_next = ent_get_next(e_next);
    }
#endif
#else /*Use custom, user defined free function*/
    LV_MEM_CUSTOM_FREE(e);
#endif
}

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data pointer to an allocated memory.
 * Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory
 */
void * lv_mem_realloc(void * data_p, uint32_t new_size)
{
    /*data_p could be previously freed pointer (in this case it is invalid)*/
    if(data_p != NULL) {
        lv_mem_ent_t * e = (lv_mem_ent_t *)((uint8_t *) data_p - sizeof(lv_mem_header_t));
        if(e->header.used == 0) {
            data_p = NULL;
        }
    }

    uint32_t old_size = lv_mem_get_size(data_p);
    if(old_size == new_size) return data_p;     /*Also avoid reallocating the same memory*/

#if LV_MEM_CUSTOM == 0
    /* Only truncate the memory is possible
     * If the 'old_size' was extended by a header size in 'ent_trunc' it avoids reallocating this same memory */
    if(new_size < old_size) {
        lv_mem_ent_t * e = (lv_mem_ent_t *)((uint8_t *) data_p - sizeof(lv_mem_header_t));
        ent_trunc(e, new_size);
        return &e->first_data;
    }
#endif

    void * new_p;
    new_p = lv_mem_alloc(new_size);
    
    if(new_p != NULL && data_p != NULL) {
        /*Copy the old data to the new. Use the smaller size*/
        if(old_size != 0) {
            memcpy(new_p, data_p, LV_MATH_MIN(new_size, old_size));
            lv_mem_free(data_p);
        }
    }
    
    return new_p;    
}

/**
 * Join the adjacent free memory blocks
 */
void lv_mem_defrag(void)
{
#if LV_MEM_CUSTOM == 0
    lv_mem_ent_t * e_free;
    lv_mem_ent_t * e_next;
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
void lv_mem_monitor(lv_mem_monitor_t * mon_p)
{
    /*Init the data*/
    memset(mon_p, 0, sizeof(lv_mem_monitor_t));
#if LV_MEM_CUSTOM == 0
    lv_mem_ent_t * e;
    e = NULL;
    
    e = ent_get_next(e);
    
    while(e != NULL)  {
        if(e->header.used == 0) {
            mon_p->free_cnt++;
            mon_p->free_size += e->header.d_size;
            if(e->header.d_size > mon_p->free_biggest_size) {
                mon_p->free_biggest_size = e->header.d_size;
            }
        } else {
            mon_p->used_cnt++;
        }
        
        e = ent_get_next(e);
    }
    mon_p->total_size = LV_MEM_SIZE;
    mon_p->used_pct = 100 - (100U * mon_p->free_size) / mon_p->total_size;
    mon_p->frag_pct = (uint32_t)mon_p->free_biggest_size * 100U / mon_p->free_size;
    mon_p->frag_pct = 100 - mon_p->frag_pct;
#endif
}

/**
 * Give the size of an allocated memory
 * @param data pointer to an allocated memory
 * @return the size of data memory in bytes 
 */
uint32_t lv_mem_get_size(const void * data)
{
    if(data == NULL) return 0;
    if(data == &zero_mem) return 0;
    
    lv_mem_ent_t * e = (lv_mem_ent_t *)((uint8_t *) data - sizeof(lv_mem_header_t));

    return e->header.d_size;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_MEM_CUSTOM == 0
/**
 * Give the next entry after 'act_e'
 * @param act_e pointer to an entry
 * @return pointer to an entry after 'act_e'
 */
static lv_mem_ent_t * ent_get_next(lv_mem_ent_t * act_e)
{
    lv_mem_ent_t * next_e = NULL;

    if(act_e == NULL) { /*NULL means: get the first entry*/ 
        next_e = (lv_mem_ent_t * ) work_mem;
    }
    else /*Get the next entry */
    {
        uint8_t * data = &act_e->first_data;
        next_e = (lv_mem_ent_t * )&data[act_e->header.d_size];
        
        if(&next_e->first_data >= &work_mem[LV_MEM_SIZE]) next_e = NULL;
    }
    
    return next_e;
}


/**
 * Try to do the real allocation with a given size
 * @param e try to allocate to this entry
 * @param size size of the new memory in bytes
 * @return pointer to the allocated memory or NULL if not enough memory in the entry
 */
static void * ent_alloc(lv_mem_ent_t * e, uint32_t size)
{
    void * alloc = NULL;
    
    /*If the memory is free and big enough then use it */
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
 */
static void ent_trunc(lv_mem_ent_t * e, uint32_t size)
{
    /*Round the size up to 4*/
    if(size & 0x3 ) {
        size = size & (~0x3);
        size += 4;
    }
    
    /*Don't let empty space only for a header without data*/
    if(e->header.d_size == size + sizeof(lv_mem_header_t)) {
        size = e->header.d_size;
    }

    /* Create the new entry after the current if there is space for it */
    if(e->header.d_size != size) {
        uint8_t * e_data = &e->first_data;
        lv_mem_ent_t * after_new_e = (lv_mem_ent_t *)&e_data[size];
        after_new_e->header.used = 0;
        after_new_e->header.d_size = e->header.d_size - size - sizeof(lv_mem_header_t);
    }

    /* Set the new size for the original entry */
    e->header.d_size = size;
}

#endif
