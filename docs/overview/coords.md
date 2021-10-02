```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/coords.md
```
# Positions, sizes, and layouts

## Overview
Similarly to many other parts of LVGL, the concept of setting the coordinates was inspired by CSS. LVGL has by no means a complete implementation of CSS but a comparable subset is implemented (sometimes with minor adjustments).

In short this means:
- Explicitly set coordinates are stored in styles (size, position, layouts, etc.)
- support min-width, max-width, min-height, max-height
- have pixel, percentage, and "content" units 
- x=0; y=0 coordinate means the top-left corner of the parent plus the left/top padding plus border width
- width/height means the full size, the "content area" is smaller with padding and border width 
- a subset of flexbox and grid layouts are supported
 
###  Units
- pixel: Simply a position in pixels. An integer always means pixels. E.g. `lv_obj_set_x(btn, 10)`
- percentage: The percentage of the size of the object or its parent (depending on the property). `lv_pct(value)` converts a value to percentage. E.g. `lv_obj_set_width(btn, lv_pct(50))`
- `LV_SIZE_CONTENT`: Special value to set the width/height of an object to involve all the children. It's similar to `auto` in CSS. E.g. `lv_obj_set_width(btn, LV_SIZE_CONTENT)`.

### Boxing model
LVGL follows CSS's [border-box](https://developer.mozilla.org/en-US/docs/Web/CSS/box-sizing) model.
An object's "box" is built from the following parts:
- bounding box: the width/height of the elements.
- border width: the width of the border. 
- padding: space between the sides of the object and its children. 
- content: the content area which is the size of the bounding box reduced by the border width and padding.

![The box models of LVGL: The content area is smaller than the bounding box with the padding and border width](/misc/boxmodel.png)

The border is drawn inside the bounding box. Inside the border LVGL keeps a "padding margin" when placing an object's children. 

The outline is drawn outside the bounding box.

### Important notes
This section describes special cases in which LVGL's behavior might be unexpected.

#### Postponed coordinate calculation
LVGL doesn't recalculate all the coordinate changes immediately. This is done to improve performance. 
Instead, the objects are marked as "dirty" and before redrawing the screen LVGL checks if there are any "dirty" objects. If so it refreshes their position, size and layout.

In other words, if you need to get the coordinate of an object and the coordinates were just changed, LVGL needs to be forced to recalculate the coordinates. 
To do this call `lv_obj_update_layout(obj)`.

The size and position might depend on the parent or layout. Therefore `lv_obj_update_layout` recalculates the coordinates of all objects on the screen of `obj`.

#### Removing styles
As it's described in the [Using styles](#using-styles) section, coordinates can also be set via style properties. 
To be more precise, under the hood every style coordinate related property is stored as a style property. If you use `lv_obj_set_x(obj, 20)` LVGL saves `x=20` in the local style of the object.

This is an internal mechanism and doesn't matter much as you use LVGL. However, there is one case in which you need to be aware of the implementation. If the style(s) of an object are removed by 
```c
lv_obj_remove_style_all(obj)
```

or
```c
lv_obj_remove_style(obj, NULL, LV_PART_MAIN);
```
the earlier set coordinates will be removed as well.

For example:
```c
/*The size of obj1 will be set back to the default in the end*/
lv_obj_set_size(obj1, 200, 100);  /*Now obj1 has 200;100 size*/
lv_obj_remove_style_all(obj1);    /*It removes the set sizes*/


/*obj2 will have 200;100 size in the end */
lv_obj_remove_style_all(obj2);
lv_obj_set_size(obj2, 200, 100);
```

## Position

### Simple way
To simply set the x and y coordinates of an object use:
```c
lv_obj_set_x(obj, 10);        //Separate...
lv_obj_set_y(obj, 20);
lv_obj_set_pos(obj, 10, 20); 	//Or in one function
```

By default, the x and y coordinates are measured from the top left corner of the parent's content area. 
For example if the parent has five pixels of padding on every side the above code will place `obj` at (15, 25) because the content area starts after the padding.

Percentage values are calculated from the parent's content area size. 
```c
lv_obj_set_x(btn, lv_pct(10)); //x = 10 % of parent content area width
```

### Align
In some cases it's convenient to change the origin of the positioning from the default top left. If the origin is changed e.g. to bottom-right, the (0,0) position means: align to the bottom-right corner. 
To change the origin use:
```c
lv_obj_set_align(obj, align);
``` 

To change the alignment and set new coordinates:
```c
lv_obj_align(obj, align, x, y);
```

The following alignment options can be used:
- `LV_ALIGN_TOP_LEFT`
- `LV_ALIGN_TOP_MID`
- `LV_ALIGN_TOP_RIGHT`
- `LV_ALIGN_BOTTOM_LEFT`
- `LV_ALIGN_BOTTOM_MID`
- `LV_ALIGN_BOTTOM_RIGHT`
- `LV_ALIGN_LEFT_MID`
- `LV_ALIGN_RIGHT_MID`
- `LV_ALIGN_CENTER`

It's quite common to align a child to the center of its parent, therefore a dedicated function exists:
```c
lv_obj_center(obj);

//Has the same effect
lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
```

If the parent's size changes, the set alignment and position of the children is updated automatically.

The functions introduced above align the object to its parent. However, it's also possible to align an object to an arbitrary reference object.
```c
lv_obj_align_to(obj_to_align, reference_obj, align, x, y);
```

Besides the alignments options above, the following can be used to align an object outside the reference object:

- `LV_ALIGN_OUT_TOP_LEFT`
- `LV_ALIGN_OUT_TOP_MID`
- `LV_ALIGN_OUT_TOP_RIGHT`
- `LV_ALIGN_OUT_BOTTOM_LEFT`
- `LV_ALIGN_OUT_BOTTOM_MID`
- `LV_ALIGN_OUT_BOTTOM_RIGHT`
- `LV_ALIGN_OUT_LEFT_TOP`
- `LV_ALIGN_OUT_LEFT_MID`
- `LV_ALIGN_OUT_LEFT_BOTTOM`
- `LV_ALIGN_OUT_RIGHT_TOP`
- `LV_ALIGN_OUT_RIGHT_MID`
- `LV_ALIGN_OUT_RIGHT_BOTTOM`

For example to align a label above a button and center the label horizontally:
```c
lv_obj_align_to(label, btn, LV_ALIGN_OUT_TOP_MID, 0, -10);
```

Note that, unlike with `lv_obj_align()`, `lv_obj_align_to()` can not realign the object if its coordinates or the reference object's coordinates change. 

## Size

### Simple way
The width and the height of an object can be set easily as well:
```c
lv_obj_set_width(obj, 200);       //Separate...
lv_obj_set_height(obj, 100);
lv_obj_set_size(obj, 200, 100); 	//Or in one function
```

Percentage values are calculated based on the parent's content area size. For example to set the object's height to the screen height:
```c
lv_obj_set_height(obj, lv_pct(100));
```

The size settings support a special value: `LV_SIZE_CONTENT`. It means the object's size in the respective direction will be set to the size of its children. 
Note that only children on the right and bottom sides will be considered and children on the top and left remain cropped. This limitation makes the behavior more predictable.

Objects with `LV_OBJ_FLAG_HIDDEN` or `LV_OBJ_FLAG_FLOATING` will be ignored by the `LV_SIZE_CONTENT` calculation.

The above functions set the size of an object's bounding box but the size of the content area can be set as well. This means an object's bounding box will be enlarged with the addition of padding.
```c
lv_obj_set_content_width(obj, 50); //The actual width: padding left + 50 + padding right
lv_obj_set_content_height(obj, 30); //The actual width: padding top + 30 + padding bottom
```

The size of the bounding box and the content area can be retrieved with the following functions:
```c
lv_coord_t w = lv_obj_get_width(obj);
lv_coord_t h = lv_obj_get_height(obj);
lv_coord_t content_w = lv_obj_get_content_width(obj);
lv_coord_t content_h = lv_obj_get_content_height(obj);
```

## Using styles
Under the hood the position, size and alignment properties are style properties. 
The above described "simple functions" hide the style related code for the sake of simplicity and set the position, size, and alignment properties in the local styles of the object.

However, using styles to set the coordinates has some great advantages:
- It makes it easy to set the width/height/etc. for several objects together. E.g. make all the sliders 100x10 pixels sized. 
- It also makes possible to modify the values in one place.
- The values can be partially overwritten by other styles. For example `style_btn` makes the object `100x50` by default but adding `style_full_width` overwrites only the width of the object.
- The object can have different position or size depending on state. E.g. 100 px wide in `LV_STATE_DEFAULT` but 120 px in `LV_STATE_PRESSED`.
- Style transitions can be used to make the coordinate changes smooth. 


Here are some examples to set an object's size using a style:
```c
static lv_style_t style;
lv_style_init(&style);
lv_style_set_width(&style, 100);

lv_obj_t * btn = lv_btn_create(lv_scr_act());
lv_obj_add_style(btn, &style, LV_PART_MAIN);
```

As you will see below there are some other great features of size and position setting. 
However, to keep the LVGL API lean, only the most common coordinate setting features have a "simple" version and the more complex features can be used via styles. 

## Translation

Let's say the there are 3 buttons next to each other. Their position is set as described above. 
Now you want to move a button up a little when it's pressed. 

One way to achieve this is by setting a new Y coordinate for the pressed state:
```c
static lv_style_t style_normal;
lv_style_init(&style_normal);
lv_style_set_y(&style_normal, 100);

static lv_style_t style_pressed;
lv_style_init(&style_pressed);
lv_style_set_y(&style_pressed, 80);

lv_obj_add_style(btn1, &style_normal, LV_STATE_DEFAULT);
lv_obj_add_style(btn1, &style_pressed, LV_STATE_PRESSED);

lv_obj_add_style(btn2, &style_normal, LV_STATE_DEFAULT);
lv_obj_add_style(btn2, &style_pressed, LV_STATE_PRESSED);

lv_obj_add_style(btn3, &style_normal, LV_STATE_DEFAULT);
lv_obj_add_style(btn3, &style_pressed, LV_STATE_PRESSED);
```

This works, but it's not really flexible because the pressed coordinate is hard-coded. If the buttons are not at y=100, `style_pressed` won't work as expected. Translations can be used to solve this:
```c
static lv_style_t style_normal;
lv_style_init(&style_normal);
lv_style_set_y(&style_normal, 100);

static lv_style_t style_pressed;
lv_style_init(&style_pressed);
lv_style_set_translate_y(&style_pressed, -20);

lv_obj_add_style(btn1, &style_normal, LV_STATE_DEFAULT);
lv_obj_add_style(btn1, &style_pressed, LV_STATE_PRESSED);

lv_obj_add_style(btn2, &style_normal, LV_STATE_DEFAULT);
lv_obj_add_style(btn2, &style_pressed, LV_STATE_PRESSED);

lv_obj_add_style(btn3, &style_normal, LV_STATE_DEFAULT);
lv_obj_add_style(btn3, &style_pressed, LV_STATE_PRESSED);
```

Translation is applied from the current position of the object. 

Percentage values can be used in translations as well. The percentage is relative to the size of the object (and not to the size of the parent). For example `lv_pct(50)` will move the object with half of its width/height.

The translation is applied after the layouts are calculated. Therefore, even laid out objects' position can be translated.

The translation actually moves the object. That means it makes the scrollbars and `LV_SIZE_CONTENT` sized objects react to the position change.


## Transformation
Similarly to position, an object's size can be changed relative to the current size as well. 
The transformed width and height are added on both sides of the object. This means a 10 px transformed width makes the object 2x10 pixels wider.

Unlike position translation, the size transformation doesn't make the object "really" larger. In other words scrollbars, layouts, and `LV_SIZE_CONTENT` will not react to the transformed size. 
Hence, size transformation is "only" a visual effect. 

This code enlarges a button when it's pressed:
```c
static lv_style_t style_pressed;
lv_style_init(&style_pressed);
lv_style_set_transform_width(&style_pressed, 10);
lv_style_set_transform_height(&style_pressed, 10);

lv_obj_add_style(btn, &style_pressed, LV_STATE_PRESSED);
```

### Min and Max size
Similarly to CSS, LVGL also supports `min-width`, `max-width`, `min-height` and `max-height`. These are limits preventing an object's size from becoming smaller/larger than these values. 
They are especially useful if the size is set by percentage or `LV_SIZE_CONTENT`.
```c
static lv_style_t style_max_height;
lv_style_init(&style_max_height);
lv_style_set_y(&style_max_height, 200);

lv_obj_set_height(obj, lv_pct(100));
lv_obj_add_style(obj, &style_max_height, LV_STATE_DEFAULT); //Limit the  height to 200 px
```

Percentage values can be used as well which are relative to the size of the parent's content area.
```c
static lv_style_t style_max_height;
lv_style_init(&style_max_height);
lv_style_set_y(&style_max_height, lv_pct(50));

lv_obj_set_height(obj, lv_pct(100));
lv_obj_add_style(obj, &style_max_height, LV_STATE_DEFAULT); //Limit the height to half parent height
```

## Layout

### Overview
Layouts can update the position and size of an object's children. They can be used to automatically arrange the children into a line or column, or in much more complicated forms. 

The position and size set by the layout overwrites the "normal" x, y, width, and height settings. 

There is only one function that is the same for every layout: `lv_obj_set_layout(obj, <LAYOUT_NAME>)` sets the layout on an object.
For further settings of the parent and children see the documentation of the given layout.

### Built-in layout
LVGL comes with two very powerful layouts:
- Flexbox
- Grid

Both are heavily inspired by the CSS layouts with the same name.

### Flags
There are some flags that can be used on objects to affect how they behave with layouts:
- `LV_OBJ_FLAG_HIDDEN` Hidden objects are ignored in layout calculations.
- `LV_OBJ_FLAG_IGNORE_LAYOUT` The object is simply ignored by the layouts. Its coordinates can be set as usual.
- `LV_OBJ_FLAG_FLOATING` Same as `LV_OBJ_FLAG_IGNORE_LAYOUT` but the object with `LV_OBJ_FLAG_FLOATING` will be ignored in `LV_SIZE_CONTENT` calculations.

These flags can be added/removed with `lv_obj_add/clear_flag(obj, FLAG);`

### Adding new layouts

LVGL can be freely extended by a custom layout like this:
```c
uint32_t MY_LAYOUT;

...

MY_LAYOUT = lv_layout_register(my_layout_update, &user_data);

...

void my_layout_update(lv_obj_t * obj, void * user_data)
{
	/*Will be called automatically if it's required to reposition/resize the children of "obj" */	
}
```

Custom style properties can be added which can be retrieved and used in the update callback. For example:
```c
uint32_t MY_PROP;
...

LV_STYLE_MY_PROP = lv_style_register_prop();

...
static inline void lv_style_set_my_prop(lv_style_t * style, uint32_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MY_PROP, v);
}

```

## Examples
