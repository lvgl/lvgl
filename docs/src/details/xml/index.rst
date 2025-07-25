.. _xml_main:

====================
XML - Declarative UI
====================

The LVGL XML Module implements a full-featured Declarative UI for LVGL-based embedded
projects.

A "Declarative UI" is a programming paradigm where you describe what the user
interface should look like, rather than how to construct it.  You define the desired
end state of the UI based on application data, and LVGL handles the process of
assembling the UI to match that state.



Key Concepts
************


Describing the UI
-----------------

Instead of providing step-by-step instructions on how to create and modify UI
elements as source code, you declare the desired appearance of the UI and optionally
data bindings as well.

:Framework's Role:

    The LVGL XML Module then takes on the responsibility of creating, managing, rendering,
    and updating the UI based on your declarations. Once the XML files are loaded, from
    LVGL's point of view the UI identical to a source-code created UI. It means the the created
    UI elements can be modified at runtime, and performance is also identical. 

:Component-Based:

    In the LVGL XML Module, it is encouraged to break down the whole of the UI into
    smaller, reusable Components, which can be combined to build complex interfaces.


Example
-------

Imagine you want a button to change color when clicked.  In an imperative approach,
you might write code to:

    Create the button.
    Add an event listener for clicks.
    When clicked, change the button's color by changing its style.

In a declarative approach, you might:

    Define a button component with a style for its color.

    Bind that style a variable that changes when the button is clicked.

The LVGL handles the rest, automatically updating the button's appearance when
the variable changes.



Benefits of Using a Declarative UI
**********************************

:Improved Readability and Maintainability:

    Code becomes easier to understand and maintain because you're focused on what the
    UI should look like, rather than how to create it.

:Increased Developer Productivity:

    Developers can iterate faster and focus on building the user experience, rather
    than getting bogged down in low-level UI details.



.. toctree::
    :class:    toctree-1-deep
    :maxdepth: 2

    overview/index
    build_ui/index
    test
    license
