### Description of the feature or fix

A clear and concise description of what the bug or new feature is.

### Checkpoints
- [ ] Run `code-format.py` from the scripts folder. [astyle](http://astyle.sourceforge.net/install.html) needs to be installed.
- [ ] Update the [Documentation](https://github.com/lvgl/lvgl/tree/master/docs) if needed
- [ ] Add [Examples](https://github.com/lvgl/lvgl/tree/master/examples) if relevant. 
- [ ] Add [Tests](https://github.com/lvgl/lvgl/blob/master/tests/README.md) if applicable.
- [ ] If you added new options to `lv_conf_template.h` run [lv_conf_internal_gen.py](https://github.com/lvgl/lvgl/blob/release/v8.3/scripts/lv_conf_internal_gen.py) and update [Kconfig](https://github.com/lvgl/lvgl/blob/release/v8.3/Kconfig).
 
Be sure the following conventions are followed:
- [ ] Follow the [Styling guide](https://github.com/lvgl/lvgl/blob/master/docs/CODING_STYLE.md)
- [ ] Prefer `enum`s instead of macros. If inevitable to use `define`s export them with `LV_EXPORT_CONST_INT(defined_value)` right after the `define`.
- [ ] In function arguments prefer `type name[]` declaration for array parameters instead of `type * name`
- [ ] Use typed pointers instead of `void *` pointers
- [ ] Do not `malloc` into a static or global variables. Instead declare the variable in `LV_ITERATE_ROOTS` list in [`lv_gc.h`](https://github.com/lvgl/lvgl/blob/master/src/misc/lv_gc.h) and mark the variable with `GC_ROOT(variable)` when it's used. See a detaild description [here](https://docs.lvgl.io/master/get-started/bindings/micropython.html#memory-management).
- [ ] Widget constructor must follow the `lv_<widget_name>_create(lv_obj_t * parent)` pattern.
- [ ] Widget members function must start with `lv_<modul_name>` and should receive `lv_obj_t *` as first argument which is a pointer to widget object itself.  
- [ ] `struct`s should be used via an API and not modified directly via their elements.
- [ ] `struct` APIs should follow the widgets' conventions. That is to receive a pointer to the `struct` as the first argument, and the prefix of the `struct` name should be used as the prefix of the function name too (e.g.  `lv_disp_set_default(lv_disp_t * disp)`)
- [ ] Functions and `struct`s which are not part of the public API must begin with underscore in order to mark them as "private".
- [ ] Arguments must be named in H files too.
- [ ] To register and use callbacks one of the followings needs to be followed (see a detaild description [here](https://docs.lvgl.io/master/get-started/bindings/micropython.html#callbacks)): 
  - For both the registration function and the callback pass a pointer to a `struct` as the first argument. The `struct` must contain `void * user_data` field.
  - The last argument of the registration function must be `void * user_data` and the same `user_data` needs to be passed as the last argument of the callback.
  - Callback types not following these conventions should end with `xcb_t`.
