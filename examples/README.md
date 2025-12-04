# Examples Directory Requirements

```
lvgl/examples/
    index.rst        (directory-order directive since sub-dirs
                     are not presented in alphabetical order)
    anim/
        index.rst    (see below for expected contents)
        lv_example_anim_1.c
        lv_example_anim_2.c
        lv_example_anim_3.c
        lv_example_anim_4.c
        lv_example_anim_timeline_1.c
    ...
    layouts/
        flex/
            index.rst    (see below for expected contents)
            lv_example_flex_1.c
            lv_example_flex_2.c
            lv_example_flex_3.c
            etc.
        grid/
            etc.
    libs/
        index.rst        (section-heading name: "3rd-Party Libraries"
                         [since it is different than parent directory name])
        barcode/
            index.rst    (see below for expected contents)
            lv_example_barcode_1.c
            lv_example_barcode_1.h
        bmp/
            etc.
        etc.
    etc.
```


## index.rst Requirements

```
Example 1 Title  <-- required for each example
---------------  <-- required for each example
                                         <-- blank lines are ignored
.. lv_example:: anim/lv_example_anim_1   <-- path relative to the `lvgl/examples/` dir
    :language: c
```

Repeat the above pattern for each example in current directory.  That number may be zero (0) for directories like `libs/` in which all examples are in directories below that level.  See directory structure above.

For paths outside the current directory, simply provide the path to the code example relative to the `lvgl/examples/` directory.  Example from `lvgl/examples/widgets/scale/index.rst`:

```
...

A round scale style simulating a compass
----------------------------------------

.. lv_example:: widgets/scale/lv_example_scale_12
  :language: c

Axis ticks and labels with scrolling on a chart
-----------------------------------------------

.. lv_example:: widgets/chart/lv_example_chart_2     <-- path is outside scale/ dir
  :language: c
```

#### Note

Starting the example code filename with `lv_example_` is not a requirement of the `example_list.py` script, but does make it clear that it is an example, so this pattern should be preserved for new and changed examples.


### Custom Section Headings

If a section heading needs to be spelled differently than the capitalized name of the parent directory, then an `index.rst` file in that directory may contain the desired section-heading name in an `.. example_heading:` pseudo-directive.  Example from `lvgl/examples/libs/index.rst`:

```
.. example_heading: 3rd-Party Libraries
```


### Optional Directory Reordering

There are cases where it is not appropriate to present the contents of a set of subdirectories in alphabetical order.  When this is the case, a pseudo-directive in the `index.rst` file in the parent directory can be specified to govern the sequence its subdirectories are processed.  The example below is from `lvgl/examples/widgets/index.rst`.  It is provided in order to cause the "Base Widget" (obj) directory to be processed first (and thus included in the output first).

```rst
.. dir_order:

    obj
    animimg
    arc
    arclabel
    bar
    button
    buttonmatrix
    etc.
```

#### Note

A warning is issued if either:

- a subdirectory is named that does not exist, or
- a subdirectory exists that is not in the list and not in the `avoid_dirs` list.
