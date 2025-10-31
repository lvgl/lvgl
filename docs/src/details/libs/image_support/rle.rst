.. _rle:

=================
RLE Decompression
=================

LVGL provides a custom RLE (Run-Length Encoding) decompression method.  It can be
used to reduce binary image size.  The RLE compression is a lossless compression
method.

The LVGL's built-in binary image decoder supports RLE-compressed images.
The decoder supports both variable and file as image sources. The original
binary data is directly decoded to RAM.



Benefits
********

Most screenshot and UI images (where there are a limited number of colors) can be
compressed to save more than 70% space.  The below statistics are from a watch
project.  It shows the file count of every compress level. For rare conditions, RLE
compress may increase the file size if there's no large repetition in data.

.. image:: /_static/images/rle-compress-statistics.png
   :alt: RLE compress statistics from a watch project
   :align: center


Theory
******

The RLE algorithm is a simple compression algorithm that is based on the fact that
for many adjacent pixels, the color is the same.  The algorithm simply counts how
many repeated pixels with the same color there are, and stores the count value and
the color value.  If the up-coming pixels are not repeated, it stores the non-repeat
count value and original color values.  For more details, the script used to compress
the image can be found from ``lvgl/script/LVGLImage.py``.

.. code-block:: python

    def rle_compress(self, data: bytearray, blksize: int, threshold=16):
        index = 0
        data_len = len(data)
        compressed_data = []
        while index < data_len:
            memview = memoryview(data)
            repeat_cnt = self.get_repeat_count(
                memview[index:], blksize)
            if repeat_cnt == 0:
                # done
                break
            elif repeat_cnt < threshold:
                nonrepeat_cnt = self.get_nonrepeat_count(
                    memview[index:], blksize, threshold)
                ctrl_byte = uint8_t(nonrepeat_cnt | 0x80)
                compressed_data.append(ctrl_byte)
                compressed_data.append(
                    memview[index: index + nonrepeat_cnt * blksize])
                index += nonrepeat_cnt * blksize
            else:
                ctrl_byte = uint8_t(repeat_cnt)
                compressed_data.append(ctrl_byte)
                compressed_data.append(memview[index: index + blksize])
                index += repeat_cnt * blksize

        return b"".join(compressed_data)



.. _rle_usage:

Usage
*****

To use the RLE Decoder, enable it in ``lv_conf.h`` configuration file by setting
:c:macro:`LV_USE_RLE` to ``1``.  The RLE image can then be used in the same way as
other images.

.. code-block:: c

   lv_image_set_src(img, "path/to/image.rle");



Generating RLE Compressed Binary Images
***************************************

An RLE image binary can be directly generated from another image using script
``lvgl/script/LVGLImage.py``.

.. code-block:: bash

   ./scripts/LVGLImage.py --ofmt BIN --cf I8 --compress RLE cogwheel.png

This will decompress ``cogwheel.png``, and then re-compress it using RLE and write
the output to ``./output/cogwheel.bin``.



API
***

.. API equals:  lv_rle_decompress
