```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/others/controller.md
```

# lv_obj Controller

This is something like UiViewController on iOS or Fragment on Android - to help developers better handles code structure, and object lifecycle.

## Usage

Enable `LV_USE_OBJ_CONTROLLER` in `lv_conf.h`.

### Create Controller Class

```c
struct sample_controller_t {
    /* IMPORTANT: don't miss this part */
    lv_obj_controller_t base;
    /* States, object references and data fields for this controller */
    const char *counter;
};

const lv_obj_controller_class_t sample_cls = {
        /* Initialize something needed */
        .constructor_cb = sample_controller_ctor,
        /* Create view objects */
        .create_obj_cb = sample_controller_create_obj,
        /* IMPORTANT: size of your controller struct */
        .instance_size = sizeof(struct sample_controller_t)
};
```

### Use `lv_controller_manager`

```c
/* Create controller instance, and objects will be added to container */
lv_controller_manager_t *manager = lv_controller_manager_create(container, NULL);
/* Replace current controller with instance of sample_cls, and init_argument is user defined pointer */
lv_controller_manager_replace(manager, &sample_cls, init_argument);
```

### Controller Based Navigation

```c
/* Add one instance into manager stack. View object of current controller will be destroyed,
 * but instances created in class constructor will be kept.
 */
lv_controller_manager_push(manager, &sample_cls, NULL);

/* Remove the top most controller from the stack, and bring back previous one. */
lv_controller_manager_pop(manager);
```

## Example

```eval_rst

.. include:: ../../examples/others/controller/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_obj_controller.h
  :project: lvgl

```
