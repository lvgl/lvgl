

# Lottie player
Allows to use Lottie animations in LVGL. Taken from this [base repository](https://github.com/ValentiWorkLearning/lv_rlottie)

LVGL provides the interface to [Samsung/rlottie](https://github.com/Samsung/rlottie) library's C API. That is the actual Lottie player is not part of LVGL, it needs to be built separately.

## Build Rlottie
To build Samsung's Rlottie C++14-compatible compiler and optionally CMake 3.14 or higher is required.

To build on desktop you can follow the instructions from Rlottie's [README](https://github.com/Samsung/rlottie/blob/master/README.md). In the most basic case it looks like this:
```
mkdir rlottie_workdir
cd rlottie_workdir
git clone https://github.com/Samsung/rlottie.git
mkdir build
cd build
cmake ../rlottie
make -j
sudo make install
```

And finally add the `-lrlottie` flag to your linker.

On embedded systems you need to take care of integrating Rlottie to the given build system.

ESP-IDF example at bottom

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

## Controlling animations

LVGL provides two functions to control the animation mode: `lv_rlottie_set_play_mode` and `lv_rlottie_set_current_frame`.
You'll combine your intentions when calling the first method, like in these examples:
```c
lv_obj_t * lottie = lv_rlottie_create_from_file(scr, 128, 128, "test.json");
lv_obj_center(lottie);
// Pause to a specific frame
lv_rlottie_set_current_frame(lottie, 50);
lv_rlottie_set_play_mode(lottie, LV_RLOTTIE_CTRL_PAUSE); // The specified frame will be displayed and then the animation will pause

// Play backward and loop
lv_rlottie_set_play_mode(lottie, LV_RLOTTIE_CTRL_PLAY | LV_RLOTTIE_CTRL_BACKWARD | LV_RLOTTIE_CTRL_LOOP);

// Play forward once (no looping)
lv_rlottie_set_play_mode(lottie, LV_RLOTTIE_CTRL_PLAY | LV_RLOTTIE_CTRL_FORWARD);
```

The default animation mode is **play forward with loop**.

If you don't enable looping, a `LV_EVENT_READY` is sent when the animation can not make more progress without looping.

To get the number of frames in an animation or the current frame index, you can cast the `lv_obj_t` instance to a `lv_rlottie_t` instance and inspect the `current_frame` and `total_frames` members.

## Additional rlottie utility
```c
lv_obj_t * lottie = lv_rlottie_create_from_file(scr, 128, 128, "test.json");
lv_obj_center(lottie);

//Adjusts the render size of animation to 128,50
//This action to be used with an lvgl animation to smoothly resize from 128 to 50 over a period of time
lv_rlottie_set_render_height(lottie, 50); 

//adjusts the render width of the animation to 50,50
//This action to be used with an lvgl animation to smoothly resize from 128 to 50 over a period of time
lv_rlottie_set_render_width(lottie, 50); 

//Changes the end frame of the animation - useful to chain animations into a single file for memory savings rather than creating a new animation
lv_rlottie_set_dest_frame(lottie, 15); 

//Changes the start frame of the animation - useful to chain animations into a single file for memory savings rather than creating a new animation
lv_rlottie_set_start_frame(lottie, 15);

//Example usage of start/dest frame utility
//Frames 0-15 contain a loading animation
//Frames 16-30 contain an green checkmark animation
//Frames 31-45 contain a red x animation
//Packing these into the same lottie file saves space and utilizes the same buffer while still allowing for dynamic changes in the animation to play, without needing to build 2-3 separate animations
```

# ESP-IDF Example

## Background
Rlottie can be expensive to render on embedded hardware. Lottie animations tend to use a large amount of CPU time and can use large portions of RAM. This will vary from lottie to lottie but in general for best performance:
* Limit total # of frames in the animation
* Where possible, try to avoid bezier type animations
* Limit animation render size

If your ESP32 chip does not have SPIRAM you will face severe limitations in render size.

To give a better idea on this, lets assume you want to render a 240x320 lottie animation.

In order to pass initialization of the lv_rlottie_t object, you need 240x320x32/8 (307k) available memory. The latest ESP32-S3 has 256kb RAM available for this (before freeRtos and any other initialization starts taking chunks out). So while you can probably start to render a 50x50 animation without SPIRAM, PSRAM is highly recommended.

Additionally, while you might be able to pass initialization of the lv_rlottie_t object, as rlottie renders frame to frame, this consumes additional memory. A 30 frame animation that plays over 1 second probably has minimal issues, but a 300 frame animation playing over 10 seconds could very easily crash due to lack of memory as rlottie renders, depending on the complexity of the animation.

Rlottie will not compile for the IDF using the -02 compiler option at this time.

For stability in lottie animations, I found that they run best in the IDF when enabling LV_MEM_CUSTOM (using stdlib.h)

## IDF Setup

Where the LVGL simulator uses the installed rlottie project, with the IDF, recommended to use rlottie as a submodule under your components directory. You'll also need to add a CMakeLists.txt file to tell the idf how to build rlottie.

```
cd 'your-project-directory'
git add submodule https://github.com/Samsung/rlottie.git ./components/rlottie/rlottie
cd components/rlottie
touch CMakeLists.txt
```

Inside the newly created CMakeLists.txt add the below:

```

cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/rlottie/inc"
                    )

set(LOTTIE_MODULE OFF)
set(LOTTIE_THREAD OFF)
set(BUILD_SHARED_LIBS OFF)
option(BUILD_TESTING OFF)

function(install)
endfunction()

function(export)
endfunction()

add_subdirectory(rlottie)
target_link_libraries(${COMPONENT_LIB} INTERFACE rlottie)
```

Now things get a little fuzzy, there is probably a much better way to go about this, but for now a few minor edits to rlottie:

Open rlottie/rlottie/CMakeLists.txt and comment out, or remove lines 98-101. Dynamic linking will not work on the idf.

```
@98
if (LOTTIE_MODULE)
    # for dlopen, dlsym and dlclose dependancy
     target_link_libraries(rlottie PRIVATE ${CMAKE_DL_LIBS})
endif()
```

In order to get rlottie to compile in IDF land, we also need to remove references to the dynamic linking, specifically around how rlottie handles image loading.

Open rlottie/rlottie/src/vector/vimageloader.cpp

Comment lines 9, 64-69, 74, and 78-79. At time of writing, this has not caused any issues in rendering of lottie animations on an ESP.

```
@9 
#include <dlfcn.h>

@64 
imageLoad = reinterpret_cast<lottie_image_load_f>(
             dlsym(dl_handle, "lottie_image_load"));
imageFree = reinterpret_cast<lottie_image_free_f>(
             dlsym(dl_handle, "lottie_image_free"));
imageFromData = reinterpret_cast<lottie_image_load_data_f>(
             dlsym(dl_handle, "lottie_image_load_from_data"));

@74
if (dl_handle) dlclose(dl_handle);

@78-79
dl_handle = dlopen(LOTTIE_IMAGE_MODULE_PLUGIN, RTLD_LAZY);
return (dl_handle == nullptr);
```

While unecessary, removing the rlottie/rlottie/example folder can remove many un-needed files for this embedded LVGL application

Additionally, you'll need to enable LV_USE_RLOTTIE through idf.py menuconfig under LVGL component settings.

From here, you can use the relevant LVGL lv_rlottie functions to create lottie animations in LVGL on embedded hardware!

Please note, that while lottie animations are capable of running on many ESP chips, below is recommended for best performance. 

* ESP32-S3-WROOM-1-N16R8
  * 16mb quad spi flash
  * 8mb octal spi PSRAM
* IDF4.4 or higher

The Esp-box devkit meets this spec and https://github.com/espressif/esp-box is a great starting point to adding lottie animations.

## Additional changes to make use of SPIRAM

lv_alloc/realloc do not make use of SPIRAM. Given the high memory usage of lottie animations, it is recommended to shift as much out of internal DRAM into SPIRAM as possible. In order to do so, SPIRAM will need to be enabled in the menuconfig options for your given espressif chip.

There may be a better solution for this, but for the moment the recommendation is to make local modifications to the lvgl component in your espressif project. This is as simple as swapping lv_alloc/lv_realloc calls in lv_rlottie.c with heap_caps_malloc (for IDF) with the appropriate MALLOC_CAP call - for SPIRAM usage this is MALLOC_CAP_SPIRAM.

```c
rlottie->allocated_buf = heap_caps_malloc(allocaled_buf_size+1, MALLOC_CAP_SPIRAM);
```

## Example
```eval_rst

.. include:: ../../examples/libs/rlottie/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_rlottie.h
  :project: lvgl
