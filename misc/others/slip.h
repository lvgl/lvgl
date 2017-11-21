/**
 * @file slip.h
 * 
 */

#ifndef SLIP_H
#define SLIP_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_SLIP != 0

#include <stdint.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    SLIP_READY,     /*Message is ready in buffer*/
    SLIP_WAIT,      /*Waiting for the next byte*/
    SLIP_LONG,      /*Buffer is not big enough for the message*/
    SLIP_PROT_ERR,  /*Protocol error*/
}slip_res_t;

typedef struct
{
    uint8_t * buf;
    uint32_t buf_size;
    uint32_t data_cnt;
    uint8_t escaping :1;
    uint8_t ready :1;
}slip_proc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Convert the source bytes ('src_p') according to the SLIP protocol
 * @param dest_p buffer for the SLIP convert bytes (size >= 'src_len' * 2 + 1)
 * @param src_p pointer to the non-SLIPed data
 * @param src_len the length of 'src_p' array in bytes
 * @return the length of 'dest_p' array in bytes
 */
uint32_t slip_encode(void * dest_p, void * src_p, uint32_t src_len);

/**
 * Init a 'slip_proc_t' variable to can be used in 'slip_proc_byte'
 * @param slip_p pointer to a slip_proc_t variable
 * @param buf_p a buffer to store decoded SLIP stream
 * @param buf_size the size of buffer in bytes
 */
void slip_proc_init(slip_proc_t * slip_p, void * buf_p, uint32_t buf_size);

/**
 * Decode the SLIP coded bytes of a stream
 * @param slip_p inited by 'slip_proc_init'
 * @param next_data the next data of the stream
 * @return SLIP_READY: ready message in buffe. Get its length with: 'slip_proc_get_length'
 *         SLIP_WAIT: not ready, waiting for the next byte
 *         any error from 'slip_res_t'
 */
slip_res_t slip_proc_byte(slip_proc_t  * slip_p, uint8_t next_data);

/**
 * Gives the length of a READY message decoded by 'slip_proc_bytes'
 * @param slip_p pointer to a slip_proc_t variable which was used in 'slip_proc_byte'
 * @return length of the decoded message in bytes
 */
uint32_t slip_proc_get_msg_length(const slip_proc_t * slip_p);

/**********************
 *      MACROS
 **********************/

#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
