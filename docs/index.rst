.. Comment:
    This is the code that `./docs/_ext/link_roles.py` was applying to the target `.rst` files:
    :link_to_translation:`zh_CN:[中文]`
    Since around November 2024, the number of files with this link code was reduced
    to just `./docs/index.rst`.  To "un-break" the Latex/PDF generation, non-ASCII
    characters have to be removed from the `.rst` files.  These are some notes needed
    to get that done for the link below.  The byte sequence of the below 2 Chinese ideographs is:
    e4 b8 ad e6 96 87.
    I believe these are two 3-byte UTF-8 characters.
    This needs to be changed to a manual link to this Unicode characters using two
    .. |symbolname| unicode:: U+0xxxx .. CHINESE XXX CHARACTER
    directives, and link to this URL:  https://lvgl.100ask.net/master/index.html .
    Until then, the below is a manually-installed external link to the top-level
    Chinese translation page for the LVGL documentation.
    End of comment.

`[中文] <https://lvgl.100ask.net/master/index.html>`__

===========================================
LVGL:  Light and Versatile Graphics Library
===========================================

Create beautiful UIs for any MCU, MPU and display type.
*******************************************************

.. raw:: html

    <div style="margin-bottom:30px; margin-top:14px">
    <img src="_static/img/gh-header.webp" alt="LVGL Documentation" style="width:100%">
    </div>

.. raw:: html

    <div style="margin-bottom:48px">
    <a href="intro/introduction.html"><img class="home-img" src="_static/img/home_1.png" alt="Get familiar with LVGL."></a>
    <a href="intro/basics.html"><img class="home-img" src="_static/img/home_2.png" alt="Learn how LVGL works."></a>
    <a href="intro/basics.html#going-deeper"><img class="home-img" src="_static/img/home_3.png" alt="Get your feet wet with LVGL."></a>
    <a href="intro/add-lvgl-to-your-project/index.html"><img class="home-img" src="_static/img/home_4.png" alt="Learn how to add LVGL to your project for any platform, framework and display type."></a>
    <a href="details/widgets/index.html"><img class="home-img" src="_static/img/home_5.png" alt="Learn to use LVGL Widgets with examples."></a>
    <a href="CONTRIBUTING.html"><img class="home-img" src="_static/img/home_6.png" alt="Be part of LVGL's development."></a>
    </div>


Introduction
------------

.. toctree::
    :maxdepth: 1

    intro/introduction
    intro/basics
    intro/add-lvgl-to-your-project/index


Details
-------

.. toctree::
    :maxdepth: 1

    details/base-widget/index
    details/widgets/index
    details/main-components/index
    details/other-components/index
    examples
    details/debugging/index
    details/integration/index
    details/libs/index
    API/index


Appendix
--------

.. toctree::
    :maxdepth: 1

    CONTRIBUTING
    CODING_STYLE
    CHANGELOG
    ROADMAP


