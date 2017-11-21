/**
 * @file fsm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "fsm.h"
#include <stddef.h>

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
void fsm_handler(fsm_t * fsm)
{
    if(fsm->funcs[fsm->state_act] != NULL) {
        fsm->funcs[fsm->state_act](FSM_SUBSTATE_RUN, &fsm->param);
    }
}


void fsm_set_state(fsm_t * fsm, fsm_state_t state)
{
    if(fsm->funcs[fsm->state_act] != NULL) {
        fsm->funcs[fsm->state_act](FSM_SUBSTATE_LEAVE, &fsm->param);
    }

    fsm->state_act = state;

    if(fsm->funcs[fsm->state_act] != NULL) {
        fsm->funcs[fsm->state_act](FSM_SUBSTATE_INIT, &fsm->param);
    }
}

fsm_state_t fsm_get_state(const fsm_t * fsm)
{
    return fsm->state_act;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
