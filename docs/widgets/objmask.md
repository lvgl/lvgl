```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/objmask.md
```
# Object mask (lv_objmask)

## Overview
The *Object mask* is capable of add some mask to drawings when its children is drawn. 

## Parts and Styles
The Object mask has only a main part called `LV_OBJMASK_PART_BG` and it uses the typical background style properties. 

## Usage

### Adding mask

Before adding a mask to the *Object mask* the mask should be initialized:
```c
    lv_draw_mask_<type>_param_t mask_param;
    lv_draw_mask_<type>_init(&mask_param, ...);
    lv_objmask_mask_t * mask_p = lv_objmask_add_mask(objmask, &mask_param);
```

Lvgl supports the following mask types:
- **line** clip the pixels on the top/bottom left/right of a line. Can be initialized from two points or a point and an angle:
- **angle** keep the pixels only between a given start and end angle 
- **radius** keep the pixel only inside a rectangle which can have radius (can for a circle too). Can be inverted to keep the pixel outside of the rectangle.
- **fade** fade vertically (change the pixels opacity according to their y position)
- **map** use an alpha mask (a byte array) to describe the pixels opacity.

The coordinates in the mask are relative to the Object. That is if the object moves the masks move with it.

For the details of the mask *init* function see the [API](#api) documentation below.

### Update mask
AN existing mask can be updated with `lv_objmask_update_mask(objmask, mask_p, new_param)`, where `mask_p` is return value of `lv_objmask_add_mask`.

### Remove mask
A mask can be removed with `lv_objmask_remove_mask(objmask, mask_p)`

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_objmask/index.rst


```

## API

```eval_rst

.. doxygenfile:: lv_objmask.h
  :project: lvgl
  
.. doxygenfile:: lv_draw_mask.h
  :project: lvgl

```
