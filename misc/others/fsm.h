/**
 * @file fsm.h
 * Finite State Machine
 */

#ifndef FSM_H
#define FSM_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    FSM_SUBSTATE_INIT,
    FSM_SUBSTATE_RUN,
    FSM_SUBSTATE_LEAVE,
}fsm_substate_t;

typedef int16_t fsm_state_t;

typedef void (*fsm_func_t)(fsm_substate_t substate, int32_t * param);

typedef struct {
    fsm_state_t state_act;
    fsm_func_t * funcs;
    int32_t param;
}fsm_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void fsm_handler(fsm_t * fsm);
void fsm_set_state(fsm_t * fsm, fsm_state_t state);
fsm_state_t fsm_get_state(const fsm_t * fsm);


/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
