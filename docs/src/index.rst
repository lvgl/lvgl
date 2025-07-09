:hide-toc:

.. raw:: html

    <p style="text-align: right;">
        <a class="reference external language-selector" href="https://lvgl.100ask.net/master/index.html">
            <span>&#x04E2D;&#x02F42;</span> Chinese Translation
        </a>
    </p>

.. _lvgl_landing_page:

====================
Welcome to LVGL Docs
====================

Create beautiful UIs for any MCU, MPU and display type.

.. raw:: html

    <div class="home-cards-container">
        <div class="home-cards">
            <a href="intro/introduction/index.html" class="home-card">
                <svg><use href="#svg-home-intro"></use></svg>
                <h3>Introduction</h3>
                <p>Get familiar with LVGL.</p>
            </a>
            <a href="intro/getting_started/index.html" class="home-card">
                <svg><use href="#svg-home-getting-started"></use></svg>
                <h3>Getting Started</h3>
                <p>Learn how LVGL works.</p>
            </a>
            <a href="intro/getting_started/whats_next.html" class="home-card">
                <svg><use href="#svg-home-play"></use></svg>
                <h3>Going deeper</h3>
                <p>Get your feet wet with LVGL.</p>
            </a>
            <a href="details/integration/adding-lvgl-to-your-project/index.html" class="home-card">
                <svg><use href="#svg-home-integration"></use></svg>
                <h3>Add LVGL to your project</h3>
                <p>Learn how to add LVGL to your project for any platform, framework and display type.</p>
            </a>
            <a href="details/widgets/index.html" class="home-card">
                <svg><use href="#svg-home-widgets"></use></svg>
                <h3>Widgets</h3>
                <p>Learn to use LVGL Widgets with examples.</p>
            </a>
            <a href="contributing/index.html" class="home-card">
                <svg><use href="#svg-home-contributing"></use></svg>
                <h3>Contributing</h3>
                <p>Be part of LVGL's development.</p>
            </a>
        </div>
    </div>


Introduction
------------

.. toctree::
    :class:    toctree-1-deep  toctree-landing-page
    :maxdepth: 1

    intro/introduction/index
    intro/getting_started/index


Details
-------

.. toctree::
    :class:    toctree-1-deep  toctree-landing-page
    :maxdepth: 1

    examples
    details/integration/index
    details/common-widget-features/index
    details/widgets/index
    details/main-modules/index
    details/xml/index
    details/auxiliary-modules/index
    details/libs/index
    details/debugging/index


Appendix
--------

.. toctree::
    :class:    toctree-1-deep  toctree-landing-page
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

.. Commenting out the below for Furo theme.
    .. toctree::
        :hidden:

        intro/index
        details/index


