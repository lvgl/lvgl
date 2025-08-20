    .. _xml_animations:

==========
Animations
==========

Overview
--------

XML animations are built on top of :ref:`Timeline animations <animations_timeline>`.

Timelines are composed of simple animations. For example: *"change the ``bg_opa``
of ``my_button_2`` from 0 to 255 in 500 ms."*

Each component can define its own timeline animations, which can then be played by the
component itself or by any parent components.

Defining Timelines
------------------

Timelines can be defined inside ``<screen>``\ s and ``<component>``\ s.
For ``<widget>``\ s, timelines are supported only in LVGL's UI Editor,
where C code can also be exported from them.

Example:

.. code-block:: xml

   <animations>

       <!-- Show the component and its children -->
       <timeline name="load">
           <animation prop="translate_x" target="self" start="-30" end="0" duration="500"/>
           <animation prop="opa" target="icon" start="0" end="255" duration="500"/>
           <animation prop="opa" target="text" start="0" end="255" duration="500" delay="200"/>
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

In summary: inside ``<animations>``, you can define ``<timeline>``\ s, each with a unique name
that you can reference later.

Inside a ``<timeline>``, you add ``<animation>``\ s to describe each step.
Supported properties of ``<animation>`` are:

- ``prop``: Style property to animate. All integer style properties are supported (colors are not).
- ``selector``: Style selector, e.g. ``knob|pressed``. Default: ``main|default``.
- ``target``: Name of the UI element to animate. ``self`` refers to the root element of the component (the ``<view>``).
- ``start``: Start value (integer only).
- ``end``: End value (integer only).
- ``duration``: Duration of the animation in milliseconds.
- ``delay``: Delay before starting in milliseconds.
- ``early_apply``: If ``true``, the start value is applied immediately, even during the delay.

Playing Timelines
-----------------

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

You set a ``target`` UI element and select one of its ``timeline``s to play.
If ``target="self"``, the timeline is looked up in the current component/widget/screen
(i.e. in the current XML file).

You can also set ``delay`` and ``reverse="true"`` when playing a timeline.

Under the Hood
--------------

Understanding how timelines work internally helps use them effectively.

When an XML file is registered, the contents of the ``<animations>`` section are parsed,
and the animation data is stored as a blueprint.

When an instance of a component or screen is created, ``lv_anim_timeline``\ s are
created and initialized from the saved blueprint. Each instance gets its own copy.

When a ``<play_timeline_event>`` is added to a UI element, the target and timeline
names are saved as strings. (It can't use pointers as the event can reference UI elements
that will be created only later in the ``<view>``.)

Finally, when the trigger event happens, LVGL finds the target widget by the saved name,
retrieves the specified timeline, and starts it.

Since each instance has its own timeline, you can have multiple components (e.g. 10 ``<list_item>``\ s)
and play their ``load`` timelines independently with different delays.
