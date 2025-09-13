.. include:: custom_tools.txt

.. _glossary:

==================
|lvglpro| Glossary
==================

.. glossary::
    :sorted:

    UI Element

        |lvglpro| helps you build UI Elements.  These can be:

        :Widgets:           packaged with LVGL
        :Derived Widgets:   new Widgets you make that "inherit" from LVGL Widgets
        :Components:        combinations of Widgets, Derived Widgets and [optionally]
                            other Components

    Widget

        One of the LVGL Widgets (Base Widget, Button, Image, Scale, Roller, etc.)

        See `All Widgets`_ for more details.

    Derived Widget

        A Widget you create in |lvglpro| that extends (i.e. inherits from)
        an existing LVGL Widget.

    Component

        A combination of Widgets and other components.

    Project

        A place to build a :term:`UI Elements <UI Element>` for one or more
        displays in an embedded application.  When you have more than one display in
        your project, the display selector becomes active in the Preview Pane.

    Component Project

        A place to build one or more :term:`Components <Component>` to be re-used in
        other |lvglpro| :term:`Projects <Project>`.

    Subject

        Observer-Pattern Subject -- the datum being "observed".  Widgets, Derived
        Widgets and Components can be "bound to" Subjects using :term:`data binding`.

    data binding

        Binding of data within an application to properties, flags or states of a
        Component or Widget.  See `LVGL Observer`_ for details.

    IntelliSense

        IntelliSense is a general term for various code editing features including:
        code completion, parameter info, quick info, and member lists. IntelliSense
        features are sometimes called by other names such as "code completion",
        "content assist", and "code hinting".

    Declarative UI

        Declarative UI (user interface) is an approach to UI creation in which the
        creator describes *what* elements are needed in the UI, what data they are
        bound to, and to some degree, what they should look like, but can leave out
        details such as their exact position or in what sequence they should be
        created at run time.

    Imperative UI

        Imperative UI is an approach to UI creation in which the creator describes
        every detail of how the UI is created at run time.  Imperative UI is the
        UI-building approach you use when you build your LVGL UI in C code.

    Figma

        Figma is a collaborative web application for interface design, with additional
        offline features enabled by desktop applications for macOS and Windows.  The
        feature set of Figma focuses on user interface and user experience design,
        with an emphasis on real-time collaboration, utilizing a variety of vector
        graphics editor and prototyping tools.  The Figma mobile app for Android and
        iOS allows viewing and interacting with Figma prototypes in real-time on
        mobile and tablet devices.  See the `Figma Website <Figma_>`_ for more details.
