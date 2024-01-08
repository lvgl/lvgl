Callbacks
---------

In C a callback is just a function pointer. But in Micropython we need to register a *Micropython callable object* for each
callback. Therefore in the Micropython binding we need to register both a function pointer and a Micropython object for every callback.

Therefore we defined a **callback convention** for the LVGL C API that expects lvgl headers to be defined in a certain
way. Callbacks that are declared according to the convention would allow the binding to register a Micropython object
next to the function pointer when registering a callback, and access that object when the callback is called.

- The basic idea is that we have ``void * user_data`` field that is used automatically by the Micropython Binding
  to save the *Micropython callable object* for a callback. This field must be provided when registering the function
  pointer, and provided to the callback function itself.
- Although called "user_data", the user is not expected to read/write that field. Instead, the Micropython glue code uses
  ``user_data`` to automatically keep track of the Micropython callable object. The glue code updates it when the callback
  is registered, and uses it when the callback is called in order to invoke a call to the original callable object.

There are a few options for defining a callback in LVGL C API:

- Option 1: ``user_data`` in a struct

  - There's a struct that contains a field called ``void * user_data``

    - A pointer to that struct is provided as the **first** argument of a callback registration function
    - A pointer to that struct is provided as the **first** argument of the callback itself

- Option 2: ``user_data`` as a function argument

  - A parameter called ``void * user_data`` is provided to the registration function as the **last** argument

    - The callback itself receives ``void *`` as the **last** argument

- Option 3: both callback and ``user_data`` are struct fields

  - The API exposes a struct with both function pointer member and ``user_data`` member

    - The function pointer member receives the same struct as its **first** argument

In practice it's also possible to mix these options, for example provide a struct pointer when registering a callback
(option 1) and provide ``user_data`` argument when calling the callback (options 2),
**as long as the same ``user_data`` that was registered is passed to the callback when it's called**.

Examples
^^^^^^^^

- :cpp:type:`lv_anim_t` contains ``user_data`` field. :cpp:func:`lv_anim_set_path_cb`
  registers `path_cb` callback. Both ``lv_anim_set_path_cb`` and :cpp:type:`lv_anim_path_cb_t`
  receive :cpp:type:`lv_anim_t` as their first argument
- ``path_cb`` field can also be assigned directly in the Python code because it's a member
  of :cpp:type:`lv_anim_t` which contains ``user_data`` field, and :cpp:type:`lv_anim_path_cb_t`
  receive :cpp:type:`lv_anim_t` as its first argument.
- :cpp:func:`lv_imgfont_create` registers ``path_cb`` and receives ``user_data`` as the last
  argument. The callback :cpp:type:`lv_imgfont_get_path_cb_t` also receives the ``user_data`` as the last argument.


More Information
^^^^^^^^^^^^^^^^

- In the `Blog <https://blog.lvgl.io/2019-08-05/micropython-pure-display-driver#using-callbacks>`__
  and in the `README <https://github.com/lvgl/lv_binding_micropython#callbacks>`__
- `[v6.0] Callback conventions  #1036 <https://github.com/lvgl/lvgl/issues/1036>`__
- Various discussions: `here <https://github.com/lvgl/lvgl/pull/3294#issuecomment-1184895335>`__
  and `here <https://github.com/lvgl/lvgl/issues/1763#issuecomment-762247629>`__
  and`here <https://github.com/lvgl/lvgl/issues/316#issuecomment-467221587>`__