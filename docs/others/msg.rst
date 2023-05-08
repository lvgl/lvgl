=========
Messaging
=========

Messaging (``lv_msg``) is a classic `publisher subscriber <https://en.wikipedia.org/wiki/Publish%E2%80%93subscribe_pattern>`__
implementation for LVGL.

IDs
---

Both the publishers and the subscribers needs to know the message
identifiers. In ``lv_msg`` these are simple integers. For example:

.. code:: c

   #define MSG_DOOR_OPENED             1
   #define MSG_DOOR_CLOSED             2
   #define MSG_USER_NAME_CHANGED       100
   #define MSG_USER_AVATAR_CHANGED     101

You can organize the message IDs as you wish.

Both parties also need to know about the format of the payload. E.g. in
the above example :c:enumerator:`MSG_DOOR_OPENED` and :c:enumerator:`MSG_DOOR_CLOSED` might have
no payload but :c:enumerator:`MSG_USER_NAME_CHANGED` can have a :c:expr:`const char *`
payload containing the user name, and :c:enumerator:`MSG_USER_AVATAR_CHANGED` a
:c:expr:`const void *` image source with the new avatar image.

To be more precise the message ID's type is declared like this:

.. code:: c

   typedef lv_uintptr_t lv_msg_id_t;

This way, if a value in stored in a global variable (e.g. the current
temperature) then the address of that variable can be used as message ID
too by simply casting it to :c:type:`lv_msg_id_t`. It saves the creation of
message IDs manually as the variable itself serves as message ID too.

Subscribe to a message
----------------------

:c:expr:`lv_msg_subscribe(msg_id, callback, user_data)` can be used to
subscribe to message.

Don't forget that ``msg_id`` can be a constant or a variable address
too:

.. code:: c

   lv_msg_subscribe(45, my_callback_1, NULL);

   int v;
   lv_msg_subscribe((lv_msg_id_t)&v, my_callback_2, NULL);

The callback should look like this:

.. code:: c


   static void user_name_subscriber_cb(lv_msg_t * m)
   {
       /*m: a message object with the msg_id, payload, and user_data (set during subscription)*/

       ...do something...
   }

From :c:struct:`lv_msg_t` the following can be used to get some data:

- :c:expr:`lv_msg_get_id(m)`
- :c:expr:`lv_msg_get_payload(m)`
- :c:expr:`lv_msg_get_user_data(m)`

Subscribe with an lv_obj
------------------------

It's quite typical that an LVGL widget is interested in some messages.
To make it simpler :c:expr:`lv_msg_subsribe_obj(msg_id, obj, user_data)` can
be used. If a new message is published with ``msg_id`` an
:c:enumerator:`LV_EVENT_MSG_RECEIVED` event will be sent to the object.

For example:

.. code:: c

   lv_obj_add_event(user_name_label, user_name_label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
   lv_msg_subsribe_obj(MSG_USER_NAME_CHANGED, user_name_label, NULL);

   ...

   void user_name_label_event_cb(lv_event_t * e)
   {
       lv_obj_t * label = lv_event_get_target(e);
       lv_msg_t * m = lv_event_get_msg(e);
       lv_label_set_text(label, lv_msg_get_payload(m));
   }

Here ``msg_id`` also can be a variable's address:

.. code:: c

   char name[64];
   lv_msg_subsribe_obj(name, user_name_label, NULL);

Unsubscribe
~~~~~~~~~~~

:c:func:`lv_msg_subscribe` returns a pointer which can be used to unsubscribe:

.. code:: c

   void * s1;
   s1 = lv_msg_subscribe(MSG_USER_DOOR_OPENED, some_callback, NULL);

   ...

   lv_msg_unsubscribe(s1);

Send message
------------

Messages can be sent with :c:expr:`lv_msg_send(msg_id, payload)`. E.g.

.. code:: c

   lv_msg_send(MSG_USER_DOOR_OPENED, NULL);
   lv_msg_send(MSG_USER_NAME_CHANGED, "John Smith");

If have subscribed to a variable with
:c:expr:`lv_msg_subscribe((lv_msg_id_t)&v, callback, NULL)` and changed the
variable's value the subscribers can be notified like this:

.. code:: c

   v = 10;
   lv_msg_update_value(&v); //Notify all the subscribers of `(lv_msg_id_t)&v`

It's handy way of creating API for the UI too. If the UI provides some
global variables (e.g. ``int current_tempereature``) and anyone can
read and write this variable. After writing they can notify all the
elements who are interested in that value. E.g. an ``lv_label`` can
subscribe to :c:expr:`(lv_msg_id_t)&current_tempereature` and update its text
when it's notified about the new temperature.

Example
-------

.. include:: ../examples/others/msg/index.rst

API
---
