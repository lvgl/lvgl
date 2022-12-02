/**
 * @file lv_msg.h
 *
 */

#ifndef LV_MSG_H
#define LV_MSG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../core/lv_obj.h"
#if LV_USE_MSG

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t id;            /*Identifier of the message*/
    void * user_data;       /*Set the the user_data set in `lv_msg_subscribe`*/
    void * _priv_data;      /*Used internally*/
    const void * payload;   /*Pointer to the data of the message*/
} lv_msg_t;

typedef void (*lv_msg_subscribe_cb_t)(void * s, lv_msg_t * msg);

typedef void (*lv_msg_request_cb_t)(void * r, uint32_t msg_id);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Called internally to initialize the message module
 */
void lv_msg_init(void);

/**
 * Subscribe to an `msg_id`
 * @param msg_id        the message ID to listen to
 * @param cb            callback to call if a message with `msg_id` was sent
 * @param user_data     arbitrary data which will be available in `cb` too
 * @return              pointer to a "subscribe object". It can be used the unsubscribe.
 */
void * lv_msg_subsribe(uint32_t msg_id, lv_msg_subscribe_cb_t cb, void * user_data);

/**
 * Subscribe an `lv_obj` to a message.
 * `LV_EVENT_MSG_RECEIVED` will be triggered if a message with matching ID was sent
 * @param msg_id        the message ID to listen to
 * @param obj           pointer to an `lv_obj`
 * @param user_data     arbitrary data which will be available in `cb` too
 * @return              pointer to a "subscribe object". It can be used the unsubscribe.
 */
void * lv_msg_subsribe_obj(uint32_t msg_id, lv_obj_t * obj, void * user_data);

/**
 * Cancel a previous subscription
 * @param s             pointer to a "subscibe object".
 *                      Return value of `lv_msg_subsribe` or `lv_msg_subsribe_obj`
 */
void lv_msg_unsubscribe(void * s);

/**
 * Send a message with a given ID and payload
 * @param msg_id        ID of the message to send
 * @param data          pointer to the data to send
 */
void lv_msg_send(uint32_t msg_id, const void * payload);

/**
 * Get the ID of a message object. Typically used in the subscriber callback.
 * @param m             pointer to a message object
 * @return              the ID of the message
 */
uint32_t lv_msg_get_id(lv_msg_t * m);

/**
 * Get the payload of a message object. Typically used in the subscriber callback.
 * @param m             pointer to a message object
 * @return              the payload of the message
 */
const void * lv_msg_get_payload(lv_msg_t * m);

/**
 * Get the user data of a message object. Typically used in the subscriber callback.
 * @param m             pointer to a message object
 * @return              the user data of the message
 */
void * lv_msg_get_user_data(lv_msg_t * m);

/**
 * Get the message object from an event object. Can be used in `LV_EVENT_MSG_RECEIVED` events.
 * @param e     pointer to an event object
 * @return      the message object or NULL if called with unrelated event code.
 */
lv_msg_t * lv_event_get_msg(lv_event_t * e);

/*Fix typo*/
static inline void * lv_msg_subscribe(uint32_t msg_id, lv_msg_subscribe_cb_t cb, void * user_data)
{
    return lv_msg_subsribe(msg_id, cb, user_data);
}

static inline void * lv_msg_subscribe_obj(uint32_t msg_id, lv_obj_t * obj, void * user_data)
{
    return lv_msg_subsribe_obj(msg_id, obj, user_data);
}

/**********************
 * GLOBAL VARIABLES
 **********************/

extern lv_event_code_t LV_EVENT_MSG_RECEIVED;

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_MSG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MSG_H*/
