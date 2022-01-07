```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/others/fragment.md
```

# Fragment

This is something like [Fragment on Android](https://developer.android.com/guide/components/fragments)
or [UiViewController on iOS](https://developer.apple.com/documentation/uikit/uiviewcontroller) - to help developers
better handles code structure, and object lifecycle.

## Usage

Enable `LV_USE_FRAGMENT` in `lv_conf.h`.

### Create Fragment Class

```c
struct sample_fragment_t {
    /* IMPORTANT: don't miss this part */
    lv_fragment_t base;
    /* States, object references and data fields for this fragment */
    const char *counter;
};

const lv_fragment_class_t sample_cls = {
        /* Initialize something needed */
        .constructor_cb = sample_fragment_ctor,
        /* Create view objects */
        .create_obj_cb = sample_fragment_create_obj,
        /* IMPORTANT: size of your fragment struct */
        .instance_size = sizeof(struct sample_fragment_t)
};
```

### Use `lv_fragment_manager`

```c
/* Create fragment instance, and objects will be added to container */
lv_fragment_manager_t *manager = lv_fragment_manager_create(container, NULL);
/* Replace current fragment with instance of sample_cls, and init_argument is user defined pointer */
lv_fragment_manager_replace(manager, &sample_cls, init_argument);
```

### Fragment Based Navigation

```c
/* Add one instance into manager stack. View object of current fragment will be destroyed,
 * but instances created in class constructor will be kept.
 */
lv_fragment_manager_push(manager, &sample_cls, NULL);

/* Remove the top most fragment from the stack, and bring back previous one. */
lv_fragment_manager_pop(manager);
```

## Example

```eval_rst

.. include:: ../../examples/others/fragment/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_fragment.h
  :project: lvgl

```
