.. raw:: html

    <p style="text-align: right;">
        <a class="reference external" href="https://lvgl.100ask.net/master/index.html">
            [&#x04E2D;&#x02F42;] Chinese Translation
        </a>
    </p>

.. _lvgl_landing_page:

===========================================
LVGL:  Light and Versatile Graphics Library
===========================================

Create beautiful UIs for any MCU, MPU and display type.
*******************************************************

.. raw:: html

    <div style="margin-bottom:30px; margin-top:14px">
    <img src="_static/images/gh-header.webp" alt="LVGL Documentation" style="width:100%">
    </div>

.. raw:: html

    <div style="margin-bottom:48px">
    <a href="intro/introduction.html">
        <img class="home-img" src="_static/images/home_1.png" alt="Get familiar with LVGL."></a>
    <a href="intro/getting_started.html">
        <img class="home-img" src="_static/images/home_2.png" alt="Learn how LVGL works."></a>
    <a href="intro/getting_started.html#what-s-next">
        <img class="home-img" src="_static/images/home_3.png" alt="Get your feet wet with LVGL."></a>
    <a href="details/integration/adding-lvgl-to-your-project/index.html">
        <img class="home-img" src="_static/images/home_4.png" alt="Learn how to add LVGL to your project for any platform, framework and display type."></a>
    <a href="details/widgets/index.html">
        <img class="home-img" src="_static/images/home_5.png" alt="Learn to use LVGL Widgets with examples."></a>
    <a href="contributing/index.html">
        <img class="home-img" src="_static/images/home_6.png" alt="Be part of LVGL's development."></a>
    </div>


Introduction
------------

.. toctree::
    :maxdepth: 1

    intro/introduction
    intro/getting_started


Details
-------

.. toctree::
    :maxdepth: 1

    examples
    details/integration/index
    details/common-widget-features/index
    details/widgets/index
    details/main-modules/index
    details/auxiliary-modules/index
    details/libs/index
    details/debugging/index


Appendix
--------

.. toctree::
    :maxdepth: 1

    contributing/index
    CHANGELOG
    API/index


.. The below toctree directive is merely to eliminate 2 Sphinx warnings that these 2
   files are "not included in any toctree".  Their sole purpose is so that any reader
   noticing the URL structure and (as an experiment) browsing to

   https://docs.lvgl.io/master/intro/

   or

   https://docs.lvgl.io/master/details/

   will not be met by a 404 (File not found) error from the docs server.

   Note:  this must be accompanied by "'includehidden': False" in `conf.py` in the
   `html_theme_options` dictionary so that these 2 files do not appear as redundant
   entries in the top-level navigation tree.  (This is the only toctree directive
   with the :hidden: attribute.)

.. toctree::
    :hidden:

    intro/index
    details/index


