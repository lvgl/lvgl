/**
 * @file strcmd.h
 * 
 */

#ifndef STRCMD_H
#define STRCMD_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_STRCMD != 0

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
/*Return codes*/
#define STRCMD_NOT_READY    (-1)
#define STRCMD_UNKNOWN      (-2)
#define STRCMD_FORMAT_ERR   (-3)
#define STRCMD_OVERFLOW     (-4)

/**********************
 *      TYPEDEFS
 **********************/

/*Internal states of command processing*/
typedef enum
{
    SC_WAIT,        /*Wait for the first data*/
    SC_CMD_REC,     /*Receiving command*/
    SC_PAR_REC,     /*Receiving paramter*/
    SC_N_REC,       /*Wait for \n*/
}sc_state_t;

typedef struct
{
    const char ** cmds;
    char * bufp;
    uint16_t buf_len;
    uint16_t bufi;
    sc_state_t state;
    int16_t last_cmd;
    uint8_t par :1; /*The command has paramter*/
}sc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a string command variable
 * @param sc_p pointer to a sc_t variable to initalize
 * @param cmds the possible commands as an array of character strings.
 *             The last command has to be "".
 * @param buf a buffer to store commands during the process (size > longest command + parameter + 8)
 * @param buf_len length of 'buf' in bytes.
 */
void strcmd_init(sc_t * sc_p, const char ** cmds, char *buf, uint16_t buf_len);

/**
 * Add a character to process.
 * @param sc_p pointer to an initialized sc_t variable
 * @param c a character to add
 * @return >= 0: id of the received command, < 0 not ready or error, see strcmd.h for return codes
 */
int16_t strcmd_add(sc_t * sc_p, char c);

/**
 * Return with parameter of the last command.
 * @param sc_p pointer to an 'sc_t' variable.
 * @return the parameter as string or "" if no parameter
 */
const char * strcmd_get_par(sc_t * sc_p);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_STRCMD*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
