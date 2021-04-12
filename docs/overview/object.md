```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/object.md
```
# Objects

In the LVGL the **basic building blocks** of a user interface are the objects, also called *Widgets*.
For example a [Button](/widgets/btn), [Label](/widgets/label), [Image](/widgets/img), [List](/widgets/list), [Chart](/widgets/chart) or [Text area](/widgets/textarea).

Check all the [Object types](/widgets/index) here.

## Attributes

### Basic attributes

All object types share some basic attributes:
- Position
- Size
- Parent
- Drag enable
- Click enable etc.

You can set/get these attributes with `lv_obj_set_...` and `lv_obj_get_...` functions. For example:

```c
/*Set basic object attributes*/
lv_obj_set_size(btn1, 100, 50);	 /*Button size*/
lv_obj_set_pos(btn1, 20,30);      /*Button position*/
```

To see all the available functions visit the Base object's [documentation](/widgets/obj).

### Specific attributes

The object types have special attributes too. For example, a slider has
- Min. max. values
- Current value
- Custom styles

For these attributes, every object type have unique API functions. For example for a slider:

```c
/*Set slider specific attributes*/
lv_slider_set_range(slider1, 0, 100);	   /*Set min. and max. values*/
lv_slider_set_value(slider1, 40, LV_ANIM_ON);	/*Set the current value (position)*/
lv_slider_set_action(slider1, my_action);     /*Set a callback function*/
```

The API of the object types are described in their [Documentation](/widgets/index) but you can also check the respective header files (e.g. *lv_objx/lv_slider.h*)

## Working mechanisms

### Parent-child structure

A parent object can be considered as the container of its children. Every object has exactly one parent object (except screens), but a parent can have an unlimited number of children.
There is no limitation for the type of the parent but, there are typical parent (e.g. button) and typical child (e.g. label) objects.

### Moving together

If the position of the parent is changed the children will move with the parent.
Therefore all positions are relative to the parent.

The (0;0) coordinates mean the objects will remain in the top left-hand corner of the parent independently from the position of the parent.

![](/misc/par_child1.png "Objects are moving together 1")

```c
lv_obj_t * par = lv_obj_create(lv_scr_act(), NULL); /*Create a parent object on the current screen*/
lv_obj_set_size(par, 100, 80);	                   /*Set the size of the parent*/

lv_obj_t * obj1 = lv_obj_create(par, NULL);	         /*Create an object on the previously created parent object*/
lv_obj_set_pos(obj1, 10, 10);	                   /*Set the position of the new object*/
```

Modify the position of the parent:

![](/misc/par_child2.png "Graphical objects are moving together 2")  

```c
lv_obj_set_pos(par, 50, 50);	/*Move the parent. The child will move with it.*/
```

(For simplicity the adjusting of colors of the objects is not shown in the example.)

### Visibility only on the parent

If a child is partially or fully out of its parent then the parts outside will not be visible.

![](/misc/par_child3.png "A graphical object is visible on its parent")  

```c
lv_obj_set_x(obj1, -30);	/*Move the child a little bit of the parent*/
```

### Create - delete objects

In LVGL objects can be created and deleted dynamically in run-time.
It means only the currently created objects consume RAM.
For example, if you need a chart, you can create it when required and delete it when it is not visible or necessary.

Every object type has its own **create** function with a unified prototype.
It needs two parameters:
- A pointer to the *parent* object. To create a screen give *NULL* as parent.
- Optionally, a pointer to *copy* object with the same type to copy it. This *copy* object can be *NULL* to avoid the copy operation.

All objects are referenced in C code using an `lv_obj_t` pointer as a handle. This pointer can later be used to set or get the attributes of the object.

The create functions look like this:

```c
lv_obj_t * lv_ <type>_create(lv_obj_t * parent, lv_obj_t * copy);
```

There is a common **delete** function for all object types. It deletes the object and all of its children.

```c
void lv_obj_del(lv_obj_t * obj);
```

`lv_obj_del` will delete the object immediately.
If for any reason you can't delete the object immediately you can use `lv_obj_del_async(obj)`.
It is useful e.g. if you want to delete the parent of an object in the child's `LV_EVENT_DELETE` signal.

You can remove all the children of an object (but not the object itself) using `lv_obj_clean`:

```c
void lv_obj_clean(lv_obj_t * obj);
```

## Screens

### Create screens
The screens are special objects which have no parent object. So they can be created like:
```c
lv_obj_t * scr1 = lv_obj_create(NULL, NULL);
```

Screens can be created with any object type. For example, a [Base object](/widgets/obj) or an image to make a wallpaper.

### Get the active screen
There is always an active screen on each display. By default, the library creates and loads a "Base object" as a screen for each display.  

To get the currently active screen use the `lv_scr_act()` function. 

### Load screens

To load a new screen, use `lv_scr_load(scr1)`.

#### Load screen with animation

A new screen can be loaded with animation too using `lv_scr_load_anim(scr, transition_type, time, delay, auto_del)`. The following transition types exist:
- `LV_SCR_LOAD_ANIM_NONE`: switch immediately after `delay` ms
- `LV_SCR_LOAD_ANIM_OVER_LEFT/RIGHT/TOP/BOTTOM` move the new screen over the other towards the given direction
- `LV_SCR_LOAD_ANIM_MOVE_LEFT/RIGHT/TOP/BOTTOM` move both the old and new screens  towards the given direction
- `LV_SCR_LOAD_ANIM_FADE_ON` fade the new screen over the old screen

Setting `auto_del` to `true` will automatically delete the old screen when the animation is finished.

The new screen will become active (returned by `lv_scr_act()`) when the animations starts after `delay` time.

### Handling multiple displays
Screens are created on the currently selected *default display*.
The *default display* is the last registered display with `lv_disp_drv_register` or you can explicitly select a new default display using `lv_disp_set_default(disp)`.

`lv_scr_act()`, `lv_scr_load()` and `lv_scr_load_anim()` operate on the default screen.

Visit [Multi-display support](/overview/display) to learn more.

## Parts

The widgets can have multiple parts. For example a [Button](/widgets/btn) has only a main part but a [Slider](/widgets/slider) is built from a background, an indicator and a knob. 

The name of the parts is constructed like `LV_ + <TYPE> _PART_ <NAME>`. For example `LV_BTN_PART_MAIN` or `LV_SLIDER_PART_KNOB`. The parts are usually used when styles are add to the objects. 
Using parts different styles can be assigned to the different parts of the objects. 

To learn more about the parts read the related section of the [Style overview](/overview/style#parts).


## States
The object can be in a combinations of the following states:
- **LV_STATE_DEFAULT**  Normal, released
- **LV_STATE_CHECKED** Toggled or checked
- **LV_STATE_FOCUSED** Focused via keypad or encoder or clicked via touchpad/mouse 
- **LV_STATE_EDITED**  Edit by an encoder
- **LV_STATE_HOVERED** Hovered by mouse (not supported now)
- **LV_STATE_PRESSED** Pressed
- **LV_STATE_DISABLED** Disabled or inactive

The states are usually automatically changed by the library as the user presses, releases, focuses etc an object. 
However, the states can be changed manually too. To completely overwrite the current state use `lv_obj_set_state(obj, part, LV_STATE...)`. 
To set or clear given state (but leave to other states untouched) use `lv_obj_add/clear_state(obj, part, LV_STATE_...)`
In both cases ORed state values can be used as well. E.g. `lv_obj_set_state(obj, part, LV_STATE_PRESSED | LV_PRESSED_CHECKED)`.

To learn more about the states read the related section of the [Style overview](/overview/style#states).

