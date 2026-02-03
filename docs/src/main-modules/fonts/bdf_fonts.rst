
.. _bdf_font:

========
BDF Font
========

Overview
********

Small displays with low resolution don't look pretty with automatically rendered fonts. A bitmap font provides
the solution, but it's necessary to convert the bitmap font (BDF) to a TTF.

Convert BDF to TTF
******************

BDF are bitmap fonts where fonts are not described in outlines but in pixels. BDF files can be used but
they must be converted into the TTF format using ``mkttf``, which can be found
in this GitHub repository:  https://github.com/Tblue/mkttf .  This tool uses potrace to generate outlines from
the bitmap information. The bitmap itself will be embedded into the TTF as well. `lv_font_conv <https://github.com/lvgl/lv_font_conv/>`__ uses
the embedded bitmap but it also needs the outlines. One might think you can use a fake MS Bitmap
only sfnt (ttf) (TTF without outlines) created by fontforge, but this will not work.

Install imagemagick, python3, python3-fontforge and potrace

On Ubuntu Systems, just type

.. code:: bash

    sudo apt install imagemagick python3-fontforge potrace

Clone mkttf

.. code:: bash

    git clone https://github.com/Tblue/mkttf

Read the mkttf docs.

Former versions of imagemagick needs the imagemagick call in front of convert, identify and so on.
But newer versions don't. So you might want to change 2 lines in ``potrace-wrapper.sh`` ---
open ``potrace-wrapper.sh`` and remove imagemagick from line 55 and line 64:

line 55

.. code:: bash

    wh=($(identify -format '%[width]pt %[height]pt' "${input?}"))

line 64

.. code:: bash

    convert "${input?}" -sample '1000%' - \

It might be necessary to change the mkttf.py script.

line 1

.. code:: bash

    #!/usr/bin/env python3

Example
*******

.. code-block:: console

    cd mkttf
    ./mkttf.py ./TerminusMedium-12-12.bdf
    Importing bitmaps from 0 additional fonts...
    Importing font `./TerminusMedium-12-12.bdf' into glyph background...
    Processing glyphs...
    Saving TTF file...
    Saving SFD file...
    Done!

The TTF ``TerminusMedium-001.000.ttf`` will be created from ``./TerminusMedium-12-12.bdf``.

To create a font for LVGL:

.. code:: bash

    lv_font_conv --bpp 1 --size 12 --no-compress --font TerminusMedium-001.000.ttf --range 0x20-0x7e,0xa1-0xff --format lvgl -o terminus_1bpp_12px.c

:note: use 1-bpp because we don't use anti-aliasing. It doesn't look sharp on displays with a low resolution.


