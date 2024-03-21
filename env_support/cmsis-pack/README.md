# How to Create CMSIS-Pack



## STEP 1 Update 'lv_conf_cmsis.h'

1. Copy the **lv_conf_template.h** to '**cmsis-pack**' directory

2. Set the macro protector to '1'

```c
...
/* clang-format off */
#if 1 /*Set it to "1" to enable content*/
...
```

remove the misleading guide above this code segment.

```c
/*
 * Copy this file as `lv_conf.h`
 * 1. simply next to the `lvgl` folder
 * 2. or any other places and
 *    - define `LV_CONF_INCLUDE_SIMPLE`
 *    - add the path as include path
 */
```


3. Add including for '**RTE_Components.h**'

```c
#ifndef LV_CONF_H
#define LV_CONF_H

#include "RTE_Components.h"
...
```
4. Remove macro definitions for

   - LV_USE_DEMO_WIDGETS
   
   - LV_USE_DEMO_BENCHMARK
   
   - LV_USE_IME_PINYIN
   
   - LV_USE_OS
   
   - LV_USE_FILE_EXPLORER
   
   - LV_USE_DEMO_WIDGETS
   
   - LV_USE_DEMO_KEYPAD_AND_ENCODER
   
   - LV_USE_DEMO_BENCHMARK
   
   - LV_USE_DEMO_RENDER
   
   - LV_USE_DEMO_STRESS
   
   - LV_USE_DEMO_MUSIC
   
   - LV_USE_DEMO_FLEX_LAYOUT
   
   - LV_USE_DEMO_MULTILANG
   
   - LV_USE_DEMO_TRANSFORM
   
   - LV_USE_DEMO_SCROLL
   
   - LV_USE_DEMO_VECTOR_GRAPHIC
   
   - LV_USE_DRAW_VGLITE
   
   - LV_USE_DRAW_VG_LITE
   
   - LV_USE_DRAW_PXP
   
   - LV_USE_DRAW_SDL
   
   - LV_USE_DRAW_ARM2D
   
   - LV_USE_SNAPSHOT
   
   - LV_USE_MONKEY
   
   - LV_USE_GRIDNAV
   
   - LV_USE_FRAGMENT
   
   - LV_USE_IMGFONT
   
   - LV_USE_LINUX_DRM
   
   - LV_USE_TFT_ESPI
   
   - LV_USE_ST7735
   
   - LV_USE_ST7789
   
   - LV_USE_ST7796
   
   - LV_USE_ILI9341
   
     
   
5. Update `LV_LOG_PRINTF` to `1` and `LV_LOG_LEVEL` to `LV_LOG_LEVEL_USER`


6. Set `LV_FONT_MONTSERRAT_12`, `LV_FONT_MONTSERRAT_24` and `LV_FONT_MONTSERRAT_16` to `1` (So Widgets and Benchmark can be compiled correctly, this is for improving the out of box experience.)


7. Update macro `LV_ATTRIBUTE_MEM_ALIGN` and `LV_ATTRIBUTE_MEM_ALIGN_SIZE`  to force a WORD alignment.
```c
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE     1
#define LV_DRAW_BUF_STRIDE_ALIGN		4
#define LV_ATTRIBUTE_MEM_ALIGN          __attribute__((aligned(4)))
```
Make sure `LV_MEM_SIZE` is no less than `(128*1024U)`.

8. Remove following macro definitions in the `3rd party libraries` section:

    - \#define LV_USE_FS_STDIO 0
    - \#define LV_USE_FS_POSIX 0
    - \#define LV_USE_FS_WIN32 0
    - \#define LV_USE_FS_FATFS 0
    - #define LV_USE_FS_LITTLEFS 0
    - #define LV_USE_FS_MEMFS 0
    - \#define LV_USE_LODEPNG 0
    - #define LV_USE_LIBPNG 0
    - \#define LV_USE_BMP 0
    - \#define LV_USE_RLE 0
    - #define LV_USE_TJPGD 0
    - #define LV_USE_LIBJPEG_TURBO 0
    - \#define LV_USE_GIF 0
    - \#define LV_USE_BARCODE 0
    - \#define LV_USE_QRCODE 0
    - \#define LV_USE_FREETYPE 0
    - \#define LV_USE_TINY_TTF 0
    - \#define LV_USE_RLOTTIE 0
    - \#define LV_USE_FFMPEG 0

9. update the definition of following macros: `LV_USE_VECTOR_GRAPHIC`, `LV_USE_THORVE_INTERNAL` and `LV_USE_THORVE_EXTERNAL` as 

    ```c
    /*Enable Vector Graphic APIs*/
    #ifndef LV_USE_VECTOR_GRAPHIC
    #   define LV_USE_VECTOR_GRAPHIC  0
    
    /* Enable ThorVG (vector graphics library) from the src/libs folder */
    #   define LV_USE_THORVG_INTERNAL 0
    
    /* Enable ThorVG by assuming that its installed and linked to the project */
    #   define LV_USE_THORVG_EXTERNAL 0
    #endif
    ```

10. update the definition of following macros: `LV_USE_LZ4`, `LV_USE_LZ4_INTERNAL` and `LV_USE_LZ4_EXTERNAL` as 

    ```c
    /*Enable LZ4 compress/decompress lib*/
    #ifndef LV_USE_LZ4
    #   define LV_USE_LZ4  0
    
    /*Use lvgl built-in LZ4 lib*/
    #   define LV_USE_LZ4_INTERNAL  0
    
    /*Use external LZ4 library*/
    #   define LV_USE_LZ4_EXTERNAL  0
    #endif
    ```


11. Add the following code to `HAL SETTINGS`:

```c
/*customize tick-get */
#if defined(__PERF_COUNTER__) && __PERF_COUNTER__
    #define LV_GLOBAL_INIT(__GLOBAL_PTR)                                    \
            do {                                                            \
                lv_global_init((lv_global_t *)(__GLOBAL_PTR));              \
                extern uint32_t perfc_tick_get(void);                       \
                (__GLOBAL_PTR)->tick_state.tick_get_cb = perfc_tick_get;    \
            } while(0)
#endif
```



12. Replace the macro definition:

```c
#define  LV_USE_DRAW_SW_ASM     LV_DRAW_SW_ASM_NONE
```

with:

```c
    #if !defined(LV_USE_DRAW_SW_ASM) && defined(RTE_Acceleration_Arm_2D)
        /*turn-on helium acceleration when Arm-2D and the Helium-powered device are detected */
        #if defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE
            #define LV_USE_DRAW_SW_ASM  LV_DRAW_SW_ASM_HELIUM
            #define LV_USE_DRAW_ARM2D   1
        #endif
    #endif

    #ifndef LV_USE_DRAW_SW_ASM
        #define  LV_USE_DRAW_SW_ASM     LV_DRAW_SW_ASM_NONE
    #endif
```

13. Update macro `LV_PROFILER_INCLUDE`:

```c
#define LV_PROFILER_INCLUDE "src/misc/lv_profiler_builtin.h"
```



14. rename '**lv_conf_template.h**' to '**lv_conf_cmsis.h**'.



## STEP 2 Check, Update and Run the 'gen_pack.sh'

```sh
if [ `uname -s` = "Linux" ]
  then
  CMSIS_PACK_PATH="/home/$USER/.arm/Packs/ARM/CMSIS/5.7.0/"
  PATH_TO_ADD="$CMSIS_PACK_PATH/CMSIS/Utilities/Linux64/"
else
  CMSIS_PACK_PATH="/C/Users/$USER/AppData/Local/Arm/Packs/ARM/CMSIS/5.7.0"
  PATH_TO_ADD="/C/Program Files (x86)/7-Zip/:$CMSIS_PACK_PATH/CMSIS/Utilities/Win32/:/C/xmllint/"
fi
[[ ":$PATH:" != *":$PATH_TO_ADD}:"* ]] && PATH="${PATH}:${PATH_TO_ADD}"
echo $PATH_TO_ADD appended to PATH
echo " "
```



### A. For Windows users

Update the '**CMSIS_PACK_PATH**' accordingly (Usually just replace the name gabriel with your own windows account name is sufficient.).

Update the '**PATH_TO_ADD**' to point to the installation folders of **7Zip** and **xmllint**.

Launch the git-bash and go to the cmsis-pack folder.

enter the following command:

```sh
./gen_pack.sh
```



### B. For Linux Users

Update '**PATH_TO_ADD**' if necessary.

go to the **cmsis-pack** folder.

enter the following command:

```sh
./gen_pack.sh
```
