# Animation Image (lv_animimg)

## Overview

The animation image is similar to the normal 'Image' object. The only difference is that instead of one source image, you set an array of multiple source images.

You can specify a duration and repeat count.


## Parts and Styles
- `LV_PART_MAIN` A background rectangle that uses the typical background style properties and the image itself using the image style properties.


## Usage

### Image sources
To set the image in a state, use the `lv_animimg_set_src(imgbtn, dsc[], num)`.


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
