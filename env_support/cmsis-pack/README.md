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

#include <stdint.h>
#include "RTE_Components.h"
...
```

4. Remove macro definitions for
   - LV_USE_GPU_STM32_DMA2D
   - LV_USE_GPU_NXP_PXP
   - LV_USE_GPU_NXP_VG_LITE
   - LV_USE_GPU_SWM341_DMA2D
5. Update macro `LV_ATTRIBUTE_MEM_ALIGN` and `LV_ATTRIBUTE_MEM_ALIGN_SIZE`  to force a WORD alignment.
```c
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE     4
#define LV_ATTRIBUTE_MEM_ALIGN          __attribute__((aligned(4)))
```
6. Update Theme related macros:

```c
#ifdef RTE_GRAPHICS_LVGL_USE_EXTRA_THEMES
    /*A simple, impressive and very complete theme*/
    #define LV_USE_THEME_DEFAULT 1
    #if LV_USE_THEME_DEFAULT

        /*0: Light mode; 1: Dark mode*/
        #define LV_THEME_DEFAULT_DARK 0

        /*1: Enable grow on press*/
        #define LV_THEME_DEFAULT_GROW 1

        /*Default transition time in [ms]*/
        #define LV_THEME_DEFAULT_TRANSITION_TIME 80
    #endif /*LV_USE_THEME_DEFAULT*/

    /*A very simple theme that is a good starting point for a custom theme*/
    #define LV_USE_THEME_BASIC 1

    /*A theme designed for monochrome displays*/
    #define LV_USE_THEME_MONO 1
#else
    #define LV_USE_THEME_DEFAULT    0
    #define LV_USE_THEME_BASIC      0
    #define LV_USE_THEME_MONO       0
#endif
```
7. Update `LV_TICK_CUSTOM` related macros:
```c
/*Use a custom tick source that tells the elapsed time in milliseconds.
 *It removes the need to manually update the tick with `lv_tick_inc()`)*/
#ifdef __PERF_COUNTER__
    #define LV_TICK_CUSTOM 1
    #if LV_TICK_CUSTOM
        extern uint32_t SystemCoreClock;
        #define LV_TICK_CUSTOM_INCLUDE             "perf_counter.h"

        #if __PER_COUNTER_VER__ < 10902ul
            #define LV_TICK_CUSTOM_SYS_TIME_EXPR    ((uint32_t)get_system_ticks() / (SystemCoreClock / 1000ul))
        #else
            #define LV_TICK_CUSTOM_SYS_TIME_EXPR    get_system_ms()
        #endif
    #endif   /*LV_TICK_CUSTOM*/
#else
    #define LV_TICK_CUSTOM 0
    #if LV_TICK_CUSTOM
        #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"         /*Header for the system time function*/
        #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())    /*Expression evaluating to current system time in ms*/
    #endif   /*LV_TICK_CUSTOM*/
#endif       /*__PERF_COUNTER__*/
```
9. Thoroughly remove the `DEMO USAGE` section.
10. Thoroughly remove the '3rd party libraries' section.
10. rename '**lv_conf_template.h**' to '**lv_conf_cmsis.h**'.



## STEP 2 Check, Update and Run the 'gen_pack.sh'

```sh
if [ `uname -s` = "Linux" ]
  then
  CMSIS_PACK_PATH="/home/$USER/.arm/Packs/ARM/CMSIS/5.7.0/"
  PATH_TO_ADD="$CMSIS_PACK_PATH/CMSIS/Utilities/Linux64/"
else
  CMSIS_PACK_PATH="/C/Users/gabriel/AppData/Local/Arm/Packs/ARM/CMSIS/5.7.0"
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

