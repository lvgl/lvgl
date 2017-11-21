/**
 * @file slip.c
 * Implementation of SLIP protocol
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_SLIP != 0

#include "slip.h"

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
/**
 * Convert the source bytes ('src_p') according to the SLIP protocol
 * @param dest_p buffer for the SLIP convert bytes (size >= 'src_len' * 2 + 1)
 * @param src_p pointer to the non-SLIPed data
 * @param src_len the length of 'src_p' array in bytes
 * @return the length of 'dest_p' array in bytes
 */
uint32_t slip_encode(void * dest_p, void * src_p, uint32_t src_len) 
{
    uint32_t dest_pct = 0;      /*The currently written byte in 'dest'*/
    uint32_t i;
    uint8_t * src8_p =  src_p;      /*Use byte arrays when process the data*/
    uint8_t * dest8_p =  dest_p;
#if SLIP_END_FIRST != 0
    /*Add a starting END character*/
    dest8_p[dest_pct] = SLIP_END;
    dest_pct ++;
#endif
    
    /*Convert all bytes*/
    for(i = 0; i < src_len; i++) {
        switch(src8_p[i]) {
            /*Change END chars to ESC, ESC_END*/
            case SLIP_END:
                dest8_p[dest_pct] = SLIP_ESC;
                dest_pct ++;
                dest8_p[dest_pct] = SLIP_ESC_END;
                dest_pct ++;
                break;
            /*Change ESC chars to ESC, ESC_ESC*/
            case SLIP_ESC:
                dest8_p[dest_pct] = SLIP_ESC;
                dest_pct ++;
                dest8_p[dest_pct] = SLIP_ESC_ESC;
                dest_pct ++;
                break;
            /*Simply copy non-special characters*/
            default:
                dest8_p[dest_pct] = src8_p[i];
                dest_pct ++;
        }
    }
    
#if SLIP_END_FIRST == 0
    /*Add a closing END character*/
    dest8_p[dest_pct] = SLIP_END;
    dest_pct ++;
#endif
        
    return dest_pct;
}

/**
 * Init a 'slip_proc_t' variable to can be used in 'slip_proc_byte'
 * @param slip_p pointer to a slip_proc_t variable
 * @param buf_p a buffer to store decoded SLIP stream
 * @param buf_size the size of buffer in bytes
 */
void slip_proc_init(slip_proc_t * slip_p, void * buf_p, uint32_t buf_size)
{
    slip_p->buf = buf_p;
    slip_p->buf_size = buf_size;
    slip_p->data_cnt = 0;
    slip_p->escaping = 0;
    slip_p->ready = 0;
}

/**
 * Decode the SLIP coded bytes of a stream
 * @param slip_p inited by 'slip_proc_init'
 * @param next_data the next data of the stream
 * @return SLIP_READY: ready message in buffe. Get its length with: 'slip_proc_get_length'
 *         SLIP_WAIT: not ready, waiting for the next byte
 *         any error from 'slip_res_t' 
 */
slip_res_t slip_proc_byte(slip_proc_t  * slip_p, uint8_t next_data) 
{
    slip_res_t res = SLIP_WAIT;
    
    if(slip_p->ready != 0) {    /*Reset if it was a ready message*/
        slip_p->data_cnt = 0;
        slip_p->ready = 0;
    }
    
    if(slip_p->escaping != 0) {          /*Handle the escaping*/
        switch(next_data) {
            case SLIP_ESC_ESC:
                slip_p->buf[slip_p->data_cnt] = SLIP_ESC;
                break;
            case SLIP_ESC_END:
                slip_p->buf[slip_p->data_cnt] = SLIP_END;
                break;
            default:
                res = SLIP_PROT_ERR;
        }
        slip_p->escaping = 0;
    } else if (next_data == SLIP_END) { /*Finish if non-escaped END*/
        slip_p->ready = 1;
        res = SLIP_READY;
    } else if (next_data == SLIP_ESC) {
        slip_p->escaping = 1;
    }
    else {                            /*Save normal data*/
        slip_p->buf[slip_p->data_cnt] = next_data;
    }
    
    if(res == SLIP_WAIT && next_data != SLIP_ESC) {
        slip_p->data_cnt ++;
    }
    if(slip_p->data_cnt == slip_p->buf_size) {
        slip_p->data_cnt = 0;
        res = SLIP_LONG;
    }
    
    return res;  
}

/**
 * Gives the length of a READY message decoded by 'slip_proc_bytes'
 * @param slip_p pointer to a slip_proc_t variable which was used in 'slip_proc_byte'
 * @return length of the decoded message in bytes
 */
uint32_t slip_proc_get_msg_length(const slip_proc_t * slip_p)
{
    if(slip_p->ready != 0) return slip_p->data_cnt;
    else return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
