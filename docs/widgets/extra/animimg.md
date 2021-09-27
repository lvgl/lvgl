```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/animimg.md
```
# Animation Image (lv_animimg)

## Overview

The animation image is similar to the norlmal 'Image' object. The only difference is that instead of one source image, you set an array of multiple source images.

You can specify a duration and repeat count.



## Parts and Styles
- `LV_PART_MAIN` A background rectangle that uses the typical background style properties and the image itself using the image style properties.
  
## Usage

### Image sources
To set the image in a state, use the `lv_animimg_set_src(imgbtn, dsc[], num)`.
 

### States
Instead of the regular `lv_obj_add/clear_state()` functions the `lv_imgbtn_set_state(imgbtn, LV_IMGBTN_STATE_...)` functions should be used to manually set a state.


## Events
No special events are sent by image objects.

See the events of the Base object too.

Learn more about [Events](/overview/event).

## Keys
No Keys are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../../examples/widgets/animimg/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_animimg.h
  :project: lvgl

```
