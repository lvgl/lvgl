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


:Describing the UI:

    Instead of providing step-by-step instructions on how to create and modify UI
    elements, you declare the desired state of your UI based on the application's
    data, specifically XML files accessible by the application.


:Framework's Role:

    The LVGL XML Module then takes on the responsibility of rendering and updating the
    UI based on your declarations, efficiently handling the low-level details of
    drawing and state management.

:State-Driven:

    In declarative UI, the UI is often described as a function of the application's
    state.  When the state changes, the UI automatically updates to reflect the new
    state.

:Component-Based:

    In the LVGL XML Module, it is encouraged to break down the whole of the UI into
    smaller, reusable Components, which can be combined to build complex interfaces.


Example
-------

Imagine you want a button to change color when clicked.  In an imperative approach,
you might write code to:

    Create the button.
    Add an event listener for clicks.
    When clicked, find the button element and change its color.

In a declarative approach, you might:

    Define a button component with a property for its color.

    Bind the button's color to a variable that changes when clicked.

The framework handles the rest, automatically updating the button's appearance when
the variable changes.



Benefits of Using a Declarative UI
**********************************

:Reduced Code:

    Less code is needed to achieve the same result, as the framework
    handles the complexities of UI updates.


:Improved Readability and Maintainability:

    Code becomes easier to understand and maintain because you're focused on what the
    UI should look like, rather than how to create it.

:Increased Developer Productivity:

    Developers can iterate faster and focus on building the user experience, rather
    than getting bogged down in low-level UI details.

:Enhanced Performance:

    Declarative frameworks can often optimize UI updates, leading to better
    performance.



.. toctree::
    :class:    toctree-1-deep
    :maxdepth: 2

    overview/index
    build_ui/index
    test
    license
