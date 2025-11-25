.. _xml_animations:

==========
Animations
==========

Overview
********

XML animations are built on top of :ref:`Timeline animations <animations_timeline>`.

Timelines are composed of simple animations. For example: *"change the ``bg_opa``
of ``my_button_2`` from 0 to 255 in 500 ms."*

Each Component can define its own timeline animations, which can then be played by the
Component itself or by any parent Component.



Defining Timelines
******************

Timelines can be defined inside ``<screen>``\ s and ``<component>``\ s.
For ``<widget>``\ s, timelines are supported only in LVGL's UI Editor,
where C code can also be exported from them.

Example:

.. code-block:: xml

    <component>
       <animations>

           <!-- Show Component and its children -->
           <timeline name="load">
               <animation prop="translate_x" target="self" start="-30" end="0" duration="500"/>
               <animation prop="opa" target="text" start="0" end="255" duration="500" delay="200"/>

               <include_timeline target="icon" timeline="show_up" delay="300"/>
           </timeline>

           <!-- Shake horizontally -->
           <timeline name="shake">
               <animation prop="translate_x" target="self" start="0" end="-30" duration="150"/>
               <animation prop="translate_x" target="self" start="-30" end="30" duration="300" delay="150"/>
               <animation prop="translate_x" target="self" start="30" end="0" duration="150" delay="450"/>
           </timeline>
       </animations>

       <view>
           <lv_button width="200">
               <my_icon name="icon" src="image1"/>
               <lv_label name="text" text="Click me"/>
           </lv_button>
       </view>
    </component>

In summary: inside ``<animations>``, you can define ``<timeline>``\ s, each with a unique name
that you can reference later.

Inside a ``<timeline>``, you add ``<animation>``\ s to describe each step.
Supported properties of ``<animation>`` are:

- ``prop``: Style property to animate. All integer, percentage and color style properties are supported.
- ``selector``: Style selector, e.g. ``knob|pressed``. Default: ``main|default``.
- ``target``: Name of the UI element to animate. ``self`` refers to the root element of the Component (the ``<view>``).
- ``start``: Start value (integer only).
- ``end``: End value (integer only).
- ``duration``: Duration of the animation in milliseconds.
- ``delay``: Delay before starting in milliseconds. Default is 0.
- ``early_apply``: If ``true``, the start value is applied immediately, even during the delay. Default is ``false``.

``<include_timeline>`` also can be used in ``<timeline>``\ s to "merge" the animations
of another timeline. Imagine that in the example above ``my_icon`` defines a ``"show_up"`` timeline
which fades in and enlarges the icon. All these are described in the ``my_icon.xml`` in an
encapsulated way but can be referenced in other timelines.

To include a timeline, use the following properties:

- ``target``: name of the target UI element whose timeline should be included. ``self`` refers to the root element of the Component (the ``<view>``).
- ``timeline``: name of the timeline to include. Must be defined in the ``target``'s XML file.
- ``delay``: Delay before starting in milliseconds. Default is 0.



Playing Timelines
*****************

Timelines can be triggered by events (e.g. click) using ``<play_timeline_event>``
as a child of any widget.

Example:

.. code-block:: xml

   <view>
       <lv_label name="title" text="Hello world!"/>
       <custom_button name="button" y="20">
           <play_timeline_event trigger="clicked" target="button" timeline="bounce"/>
           <lv_label text="Click me"/>
       </custom_button>
   </view>

You set a ``target`` UI element and select one of its ``timeline``\ s to play.
If ``target="self"``, the timeline is looked up in the current Component/widget/Screen
(i.e. in the current XML file).

You can also set a ``delay`` and ``reverse="true"`` when playing a timeline.



Under the Hood
**************

Understanding how timelines work internally helps in using them effectively.

When an XML file is registered, the contents of the ``<animations>`` section are parsed,
and the ``<timeline>``'s data is stored as a "blueprint". The descriptors store the targets'
names as strings.

When an instance of a Component or Screen is created, as the last step ``lv_anim_timeline``\ s are
created and initialized from the saved "blueprints". If ``<include_timeline>``\ s are also used,
the requested timeline is included in the Component's timeline at this point too.
As all the children are also created at this point, the saved animation target names are resolved
to pointers by using :cpp:expr:`lv_obj_find_by_name`.


The created timeline instances and their names are saved in the Component's instance.

Since each instance has its own timeline, you can have multiple Components (e.g. 10 ``<list_item>``\ s)
and play their ``load`` timelines independently with different delays.

When a ``<play_timeline_event>`` is added to a UI element, the target and timeline
names are saved as strings. It cannot use pointers as the event can reference UI elements
that will be created only later in the ``<view>``.

Finally, when the play timeline event is triggered, the selected timeline is retrieved by its name from the target
and started according to the other parameters (reverse, delay, etc).



