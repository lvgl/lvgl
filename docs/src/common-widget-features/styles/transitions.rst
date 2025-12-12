
.. _style_transitions:

===========
Transitions
===========

Overview
*********

By default, when a Widget changes state (e.g. it's pressed) the new
properties from the new state are set immediately. However, with
transitions it's possible to play an animation on state change. For
example, on pressing a button its background color can be animated to
the pressed color over 300 ms.

The parameters of the transitions are stored in the styles. It's
possible to set

- the time of the transition
- the delay before starting the transition
- the animation path (also known as the timing or easing function)
- the properties to animate

The transition properties can be defined for each state. For example,
setting a 500 ms transition time in the default state means that when
the Widget goes to the default state a 500 ms transition time is
applied. Setting a 100 ms transition time in the pressed state causes a
100 ms transition when going to the pressed state. This example
configuration results in going to the pressed state quickly and then
going back to default slowly.

Usage
*****

To describe a transition an :cpp:struct:`lv_style_transition_dsc_t` variable needs to be
initialized and added to a style:

.. code-block:: c

   /* Only its pointer is saved so must static, global or dynamically allocated */
   static const lv_style_prop_t trans_props[] = {
                                               LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR,
                                               0, /* End marker */
   };

   static lv_style_transition_dsc_t trans1;
   lv_style_transition_dsc_init(&trans1, trans_props, lv_anim_path_ease_out, duration_ms, delay_ms);

   lv_style_set_transition(&style1, &trans1);

