```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/rlottie.md
```


# Lottie player
Allows to use Lottie animations in LVGL. Taken from this [base repository](https://github.com/ValentiWorkLearning/lv_rlottie)

LVGL provides the interface to [Samsung/rlottie](https://github.com/Samsung/rlottie) library's C API. That is the actual Lottie player is not part of LVGL, it needs to be built separately.

## Build Rlottie
To build Samsung's Rlottie C++14-compatible compiler and optionally CMake 3.14 or higher is required.

To build on desktop you can follow the instrutions from Rlottie's [README](https://github.com/Samsung/rlottie/blob/master/README.md). In the most basic case it looks like this:
```
mkdir rlottie_workdir
cd rlottie_workdir
git clone https://github.com/Samsung/rlottie.git
mkdir build 
cd build 
cmake ../rlottie
make -j
make install
```

And finally add the `-lrlottie` flag to your linker.

On embedded systems you need to take care of integrating Rlottie to the given build system. 


## Usage

You can use animation from files or raw data (text). In either case first you need to enable `LV_USE_RLOTTIE` in `lv_conf.h`.


The `width` and `height` of the object be set in the *create* function and the animation will be scaled accordingly.

### Use Rlottie from file

To create a Lottie animation from file use:
```c
  lv_obj_t * lottie = lv_rlottie_create_from_file(parent, width, height, "path/to/lottie.json");
```

Note that, Rlottie uses the standard STDIO C file API, so you can use the path "normally" and no LVGL specific driver letter is required.

 
### Use Rlottie from raw string data

`lv_example_rlottie_approve.c` contains an example animation in raw format. Instead storing the JSON string a hex array is stored for the following reasons:
- avoid escaping `"` in the JSON file
- some compilers don't support very long strings

`lvgl/scripts/filetohex.py` can be used to convert a Lottie file a hex array. E.g.:
```
./filetohex.py path/to/lottie.json > out.txt
```

To create an animation from raw data:

```c
extern const uint8_t lottie_data[];
lv_obj_t* lottie = lv_rlottie_create_from_raw(parent, width, height, (const char *)lottie_data);
```

## Getting animations

Lottie is standard and popular format so you can find many animation files on the web.
For example: https://lottiefiles.com/ 

You can also create your own animations with Adobe After Effects or similar software.

## Example
```eval_rst

.. include:: ../../examples/libs/rlottie/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_rlottie.h
  :project: lvgl