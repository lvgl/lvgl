.. _rle:

============
RLE Compress
============

LVGL provides a custom RLE compression method. It can be used to reduce binary
image size. The RLE compression is a lossless compression method.

The LVGL's built-in binary image decoder supports RLE compressed images.
The decoder supports both variable and file as image sources. The original
binary data is directly decoded to RAM

Benefits
--------

Based on test result from a watch project. Most of the images can be compressed
to save more than 70% space as show in below statistic. It shows the file count
of every compress level. For rare conditions, RLE compress may increase the file
size if there's no large repetition in data.

.. image:: /libs/rle-compress-statistics.png
   :alt: RLE compress statistics from a watch project
   :align: center


Theory
------

The RLE algorithm is a simple compression algorithm that is based on the fact that
the for many pixels, the color is the same. The algorithm simply counts how many
repeated data are there and store the count value and the color value.
If the coming pixels are not repeated, it stores the non-repeat count value and
original color value. For more details, the script used to compress the image
can be found from ``lvgl/script/LVGLImage.py``.

.. code:: python

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
                    memview[index: index + nonrepeat_cnt*blksize])
                index += nonrepeat_cnt * blksize
            else:
                ctrl_byte = uint8_t(repeat_cnt)
                compressed_data.append(ctrl_byte)
                compressed_data.append(memview[index: index + blksize])
                index += repeat_cnt * blksize

        return b"".join(compressed_data)

.. _rle_usage:

Usage
-----

To use the RLE Decoder, enable it in ``lv_conf.h`` configuration file by setting :c:macro:`LV_USE_RLE` to `1`.
The RLE image can be used same as other images.

.. code:: c

   lv_image_set_src(img, "path/to/image.rle");

Generate RLE compressed binary images
-------------------------------------

The image can be directly generated using script ``lvgl/script/LVGLImage.py``


.. code:: bash

   ./script/LVGLImage.py --ofmt BIN --cf I8 --compress RLE cogwheel.png
