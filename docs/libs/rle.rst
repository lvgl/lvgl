RLE Decoder
===========

The RLE Decoder is a part of LVGL that is responsible for decoding RLE
compressed original LVGL binary files. The file name suffix is always set
to '.rle' and the file header is added with another 64-bit value that
includes a 32-bit magic number and 32-bit RLE information.

The decoder supports both variable and file as image sources. The original
binary data is directly decompressed to RAM, and further decoded by LVGL's
built-in decoder.

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
can be found from `lvgl/script/LVGLImage.py`.

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


Usage
-----

To use the RLE Decoder, ensure that `LV_USE_RLE` is defined and set to `1`.
The RLE image can be used same as other images.

.. code:: c

   lv_image_set_src(img, "path/to/image.rle");

Generate RLE images
-------------------

The image can be directly generated using script `lvgl/script/LVGLImage.py`


.. code:: bash

   ./script/LVGLImage.py --ofmt RLE --cf I8 cogwheel.png
