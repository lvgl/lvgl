/**
 * @file dyn_mem.h
 * 
 */

#ifndef DYN_MEM_H
#define DYN_MEM_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"

#if USE_DYN_MEM_DEFR != 0

#include <stdint.h>
#include <stddef.h>

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
}dmd_headr_t;

typedef struct 
{
    uint8_t * data_p;
    
    dmd_headr_t header;

}dmd_ent_t;

typedef struct
{
    uint32_t cnt_free;
    uint32_t cnt_used;
    uint32_t size_free;
    uint32_t size_free_big;
    uint32_t pct_frag;
}dmd_mon_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initiaize the dyn_mem module (work memory and other variables)
 */
void dmd_init(void);

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory
 * !!!IMPORTANT!!! it is special pointer,  see the file header for more information
 */
void * dmd_alloc(uint32_t size);

/**
 * Free an allocated data
 * @param data pointer to an allocated memory
 * (without da() tag, see file header for more information)
 */
void dmd_free(void * data);

/**
 * Give information about the work memory of dynamic allocation
 * @param mon_p pointer to a mon_p variable, the result of the analysis will be stored here
 */
void dmd_monitor(dmd_mon_t * mon_p);

/**
 * Give the size a allocated memory
 * @param data pointer to an allocated memory (without da() tag see file header for more information )
 * @return the size of data memory in bytes
 */
uint32_t dmd_get_size(void * data);

/**********************
 *      MACROS
 **********************/

#define da(p) (*p)
#define dp *


#endif /*USE_DYN_MEM*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

