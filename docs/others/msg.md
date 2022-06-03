# Messaging

Messaging (`lv_msg`) is a classic []publisher subscriber](https://en.wikipedia.org/wiki/Publish%E2%80%93subscribe_pattern) implementation for LVGL.

## IDs
Both the publishers and the subscribers needs to know the message identifiers.
In `lv_msg` these are simple `uint32_t` integers. For example:
```c
#define MSG_DOOR_OPENED             1
#define MSG_DOOR_CLOSED             2
#define MSG_USER_NAME_CHANGED       100
#define MSG_USER_AVATAR_CHANGED     101
```

You can orgnaize the message IDs as you wish.

Both parties also need to know about the format of teh payload. E.g. in the above example
`MSG_DOOR_OPENED` and `MSG_DOOR_CLOSED` has no payload but `MSG_USER_NAME_CHANGED` can have a `const char *` payload containing the user name, and `MSG_USER_AVATAR_CHANGED` a `const void *` image source with the new avatar image.


## Send message

Messages can be sent with `lv_msg_send(msg_id, payload)`. E.g.
```c
lv_msg_send(MSG_USER_DOOR_OPENED, NULL);
lv_msg_send(MSG_USER_NAME_CHANGED, "John Smith");
```

## Subscribe to a message

`lv_msg_subscribe(msg_id, callback, user_data)` can be used to subscribe to message.

The callback should look like this:
```c

static void user_name_subscriber_cb(void * s, lv_msg_t * m)
{
    /*s: a subscriber obeject, can be used to unscubscribe*/
    /*m: a message object with the msg_id, payload, and user_data (set durung subscription)*/

    ...do something...
}
```

From `lv_msg_t` the followings can be used to get some data:
- `lv_msg_get_id(m)`
- `lv_msg_get_payload(m)`
- `lv_msg_get_user_data(m)`

## Subscribe with an lv_obj
It's quite typical that an LVGL widget is interested in some messages.
To make it simpler `lv_msg_subsribe_obj(msg_id, obj, user_data)` can be used.
If a new message is published with `msg_id` an `LV_EVENT_MSG_RECEIVED` event will be sent to the object.

For example:
```c
lv_obj_add_event_cb(user_name_label, user_name_label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
lv_msg_subsribe_obj(MSG_USER_NAME_CHANGED, user_name_label, NULL);

...

void user_name_label_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target(e);
    lv_msg_t * m = lv_event_get_msg(e);
    lv_label_set_text(label, lv_msg_get_payload(m));
}

```

### Unsubscribe
`lv_msg_subscribe` returns a pointer which can be used to unsubscribe:
```c
void * s1;
s1 = lv_msg_subscribe(MSG_USER_DOOR_OPENED, some_callback, NULL);

...

lv_msg_unsubscribe(s1);
```

## Example

```eval_rst

.. include:: ../../examples/others/msg/index.rst

```
## API


```eval_rst

.. doxygenfile:: lv_msg.h
  :project: lvgl

```
