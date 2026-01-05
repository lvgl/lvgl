# EVE2 / EVE3 / EVE4 code library
This is a code library for EVE2/EVE3/EVE4 graphics controller ICs from FTDI/Bridgetek:

http://www.ftdichip.com/EVE.htm  
http://brtchip.com/eve/  
http://brtchip.com/ft81x/  
https://brtchip.com/bt81x/

It contains code for and has been used with various micro-controllers and displays.

## Controllers

I have used it so far with:

- 8-Bit AVR, specifically the 90CAN series
- Arduino: Uno R3, mini-pro, ESP8266, ESP32, Metro-M4 (DMA), STM32 Nucleo_F446RE (DMA), XMC1100, Uno R4
- Renesas F1L RH850
- Infineon Aurix TC222
- GD32VF103
- ATSAMC21J18A (DMA)
- ATSAME51J19A (DMA)
- ESP32 (DMA)
- RP2040 Baremetal (DMA) + Arduino (DMA) - Raspberry Pi Pico
- S32K144 (DMA)
- GD32C103CBT6 (DMA)
- STM32G0B1CET6

I have reports of successfully using it with:

- ATSAMV70
- ATSAMD20
- ATSAME4
- MSP430
- MSP432
- some PICs
- ATxmega128A1
- TMS320F28335

## Displays

The TFTs tested so far:

- FT810CB-HY50HD http://www.hotmcu.com/5-graphical-lcd-touchscreen-800x480-spi-ft810-p-286.html
- FT811CB-HY50HD  http://www.hotmcu.com/5-graphical-lcd-capacitive-touch-screen-800x480-spi-ft811-p-301.html
- RVT70UQFNWC0x https://riverdi.com/product/rvt70uqfnwc0x/
- RVT50
- ADAM101-LCP-SWVGA-NEW from Glyn, 10.1" 1024x600 cap-touch
- EVE2-38A https://www.matrixorbital.com/eve2-38a
- EVE2-35G https://www.matrixorbital.com/eve2-35g
- EVE2-43G https://www.matrixorbital.com/eve2-43g
- EVE2-50G https://www.matrixorbital.com/eve2-50g
- EVE2-70G https://www.matrixorbital.com/eve2-70g
- NHD-3.5-320240FT-CSXV-CTP
- RVT43ULBNWC00 (RiTFT-43-CAP-UX) https://riverdi.com/product/ritft43capux/
- RVT50AQBNWC00 (RiTFT-50-CAP) https://riverdi.com/product/ritft50cap/
- EVE3-50G https://www.matrixorbital.com/eve3-50g
- PAF90B5WFNWC01 http://www.panadisplay.com/ftdi-intelligent-display/9-inch-lcd-with-touch-with-bt815-board.html
- EVE3-43G https://www.matrixorbital.com/eve3-43g
- EVE3-35G https://www.matrixorbital.com/eve3-35g
- CFAF240400C0-030SC https://www.crystalfontz.com/product/cfaf240400c0030sca11-240x400-eve-touchscreen-tft-ft813
- CFAF320240F-035T https://www.crystalfontz.com/product/cfaf320240f035ttsa11-320x240-eve-tft-lcd-display-kit
- CFAF480128A0-039TC
- CFAF800480E0-050SC https://www.crystalfontz.com/product/cfaf800480e1050sca11-800x480-eve-accelerated-tft
- GEN4-FT813-50CTP-CLB https://4dsystems.com.au/gen4-ft813-50ctp-clb
- RVT101HVBNWC00-B https://riverdi.com/product/rvt101hvbnwc00-b/
- RVT70HSBNWC00-B https://riverdi.com/product/rvt70hsbnwc00-b/
- RVT50HQBNWC00-B https://riverdi.com/product/rvt50hqbnwc00-b/
- CFAF1024600B0-070SC-A1 https://www.crystalfontz.com/product/cfaf1024600b0070sca1-1024x600-7-inch-eve-tft
- Sunflower shield from Cowfish Studios
- GD3X Gameduino shield

## This is version 5

This is version 5 of this code library and there are a couple of changes from V4.

First of all, support for FT80x is gone. The main reason is that this allowed a nice speed improvement modification that only works with FT81x and beyond.
Then there is a hard break from FT80x to FT81x with ony 256k of memory in FT80x but 1MB in FT81x. The memory map is different and all the registers are located elsewhere.
FT810, FT811, FT812, FT813, BT815, BT816, BT817 and BT818 can use the exact same code as long none of the new features of BT81x are used - and there are plenty of modules with these available to choose from

As a side effect all commands are automatically started now. 

Second is that there are two sets of display-list building command functions now: EVE_cmd_xxx() and EVE_cmd_xxx_burst().
The EVE_cmd_xxx_burst() functions are optimized for speed, these are pure data transfer functions and do not even check anymore if burst mode is active.

## Structure

This library currently has nine files that I hope are named to make clear what these do:

- EVE.h - this has all defines for FT81x / BT81x itself, so here are options, registers, commands and macros defined
- EVE_commands.c - this has all the API functions that are to be called from an application
- EVE_commands.h - this contains the prototypes for the functions in EVE_commands.c
- EVE_config.h - this has all the parameters for the numerous supported display modules, here is definded which set of parameters is to be used
- EVE_target.c - this has non-portable specific code for a number of supported controllers, mostly to support DMA
- EVE_target.h - this has non-portable pin defines and code as "static inline" functions for all supported controllers
- EVE_target.cpp - this is for Arduino C++ targets
- EVE_cpp_wrapper.cpp - this is for Arduino C++ targets
- EVE_cpp_wrapper.h - this is for Arduino C++ targets

Addtionally there are these two:
- EVE_supplemental.c
- EVE_suppplemental.h

This has the prototype and implementation for extra functions, so far:
- EVE_widget_circle() - widget function to draw a circle
- EVE_widget_rectangle() - widget function to draw a rectangle
- EVE_polar_cartesian() - calculate coordinates from an angle and a length

## Examples

Generate a basic display list and tell EVE to use it:
````
EVE_cmd_dl(CMD_DLSTART); // tells EVE to start a new display-list
EVE_cmd_dl(DL_CLEAR_COLOR_RGB | WHITE); // sets the background color
EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
EVE_color_rgb(BLACK);
EVE_cmd_text(5, 15, 28, 0, "Hello there!");
EVE_cmd_dl(DL_DISPLAY); // put in the display list to mark its end
EVE_cmd_dl(CMD_SWAP); // tell EVE to use the new display list
while (EVE_busy());
````

Note, these commands are executed one by one, for each command chip-select is pulled low, a three byte address is send, the data for the command and its parameters is send and then chip-select is pulled high again which also makes EVE execute the command.

But there is a way to speed things up, we can get away with only sending the address once:
````
EVE_start_cmd_burst();
EVE_cmd_dl_burst(CMD_DLSTART);
EVE_cmd_dl_burst(DL_CLEAR_COLOR_RGB | WHITE);
EVE_cmd_dl_burst(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
EVE_color_rgb_burst(BLACK);
EVE_cmd_text_burst(5, 15, 28, 0, "Hello there!");
EVE_cmd_dl_burst(DL_DISPLAY);
EVE_cmd_dl_burst(CMD_SWAP);
EVE_end_cmd_burst();
while (EVE_busy());
````

This does the same as the first example but faster.
The preceding EVE_start_cmd_burst() either sets chip-select to low and sends out the three byte address.  
Or if DMA is available for the target you are compiling for with support code in EVE_target.c / EVE_target.cpp and EVE_target.h, it writes the address to EVE_dma_buffer and sets EVE_dma_buffer_index to 1.

Note the trailing "_burst" in the following functions, these are special versions of these commands that can only be used within an EVE_start_cmd_burst()/EVE_end_cmd_bust() pair.
These functions are optimized to push out data and nothing else.

The final EVE_end_cmd_burst() either pulls back the chip-select to high.  
Or if we have DMA it calls EVE_start_dma_transfer() to start pushing out the buffer in the background.

As we have 7 commands for EVE in these simple examples, the second one has the address overhead removed from six commands and therefore needs to transfer 18 bytes less over SPI.  
So even with a small 8-bit controller that does not support DMA this is a usefull optimization for building display lists.

Using DMA has one caveat: we need to limit the transfer to <4k as we are writing to the FIFO of EVEs command co-processor. This is usually not an issue though as we can shorten the display list generation with previously generated snippets that we attach to the current list with CMD_APPEND. And when we use widgets like CMD_BUTTON or CMD_CLOCK the generated display list grows by a larger amount than what we need to put into the command-FIFO so we likely reach the 8k limit of the display-list before we hit the 4k limit of the command-FIFO.
It is possible to use two or more DMA transfers to the FIFO to build a single display list, either to get around the 4k limit of the FIFO or in order to distribute the workload better of the time necessary between two display renewals.

You could for example do this, spread over three consecutive calls:
````
EVE_start_cmd_burst();
EVE_cmd_dl_burst(CMD_DLSTART);
EVE_cmd_dl_burst(DL_CLEAR_COLOR_RGB | WHITE);
EVE_end_cmd_burst();
````

````
EVE_start_cmd_burst();
EVE_cmd_dl_burst(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
EVE_color_rgb_burst(BLACK);
EVE_end_cmd_burst();
````

````
EVE_start_cmd_burst();
EVE_cmd_text_burst(5, 15, 28, 0, "Hello there!");
EVE_cmd_dl_burst(DL_DISPLAY);
EVE_cmd_dl_burst(CMD_SWAP);
EVE_end_cmd_burst();
````

But you need to check with EVE_busy() before each of these blocks.
Maybe similar like this never compiled pseudo-code:

thread_1ms_update_display()
{
    static uint8_t state = 0;
    static uint8_t count = 0;

    count++;

    if (E_OK == EVE_busy())
    {
        switch (state)
        {
            case 0:
                update_first();
                state = 1;
                break;
            case 1:
                update_second();
                state = 2;
                break;
            case 2:
                if (counter > 19)
                {
                    update_last_swap_list();
                    count = 0;
                    state = 0;
                }
                break;
        }
    }
}


## Remarks

The examples in the "example_projects" drawer are for use with AtmelStudio7.
For Arduino I am using PlatformIO with Visual Studio Code.

The platform the code is compiled for is automatically detected thru compiler flags in EVE_target.h.

The desired TFT is selected by adding a define for it to the build-environment, e.g. -DEVE_EVE3_50G
There is a list of available options at the start of EVE_config.h sorted by chipset.

The pins used for Chip-Select and Power-Down setup in the EVE_target/EVE_target_XXXXX.h file for your target with defines and these defines can be bypassed with defines in the build-environment.  
Check the apropriate header file for your desired target.

When compiling for AVR you need to provide the clock it is running at in order to make the _delay_ms() calls used to initialize the TFT work with the intended timing.
For other plattforms you need to provide a DELAY_MS(ms) function that works at least between 1ms and 56ms and is not performing these delays shorter than requested.
The DELAY_MS(ms) is only used during initialization of the FT8xx/BT8xx.

In Addition you need to initialize the pins used for Chip-Select and Power-Down in your hardware correctly to output.
Plus setup the SPI accordingly, mode-0, 8-bit, MSB-first, not more than 11MHz for the initialization.
A couple of targets already have a function EVE_init_spi() in EVE_target.c.

A word of "warning", you have to take care yourself to not send more than 4kiB at once to the command co-processor
or to not generate display lists that are longer than 8kiB.
My library does not check and re-check the command-FIFO on every step.  
Also there are no checks for the validity of function arguments.  
This is optimized for speed, so the training wheels are off.

## Post questions here

Originally the project went public in the German mikrocontroller.net forum, the thread contains some insight: https://www.mikrocontroller.net/topic/395608

Feel free to add to the discussion with questions or remarks.

New: Github has a discussions feature as well: https://github.com/RudolphRiedel/FT800-FT813/discussions
