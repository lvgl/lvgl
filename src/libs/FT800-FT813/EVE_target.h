/*
@file    EVE_target.h
@brief   target specific includes, definitions and functions
@version 5.0
@date    2024-01-24
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2024 Rudolph Riedel

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

5.0
- replaced spi_transmit_async() with spi_transmit_burst()
- changed the DMA buffer from uin8_t to uint32_t
- added spi_transmit_32(uint32_t data) to help shorten EVE_commands.c a bit
- added spi_transmit_32() to all targets and changed the non-DMA version of spi_transmit_burst() to use spi_transmit_32()
- added a couple of measures to speed up things for Arduino-ESP32
- stretched out the different Arduino targets, more difficult to maintain but easier to read
- sped up ARDUINO_AVR_UNO a little by making spi_transmit() native and write only and by direct writes of EVE_CS
- reverted the chip-select optimisation for ARDUINO_AVR_UNO to avoid confusion, left in the code but commented-out
- sped up ESP8266 by using 32 bit transfers for spi_transmit_32()
- added DMA to ARDUINO_METRO_M4 target
- added a STM32 target: ARDUINO_NUCLEO_F446RE
- added DMA to ARDUINO_NUCLEO_F446RE target
- added DMA to Arduino-ESP32 target
- Bugfix: the generic Arduino target was missing EVE_cs_set() / EVE_cs_clear()
- added a native ESP32 target with DMA
- missing note: Robert S. added an AVR XMEGA target by pull-request on Github
- added an experimental ARDUINO_TEENSY41 target with DMA support - I do not have any Teensy to test this with
- added a target for the Raspberry Pi Pico - RP2040
- added a target for Arduino-BBC_MICROBIT_V2
- activated DMA for the Raspberry Pi Pico - RP2040
- added ARDUINO_TEENSY35 to the experimental ARDUINO_TEENSY41 target
- transferred the little experimental STM32 code I had over from my experimental branch
- added S32K144 support including DMA
- modified the Arduino targets to use C++ wrapper functions
- fixed a few CERT warnings
- added an Arduino XMC1100_XMC2GO target
- changed ATSAM defines so that they can be defined outside the module
- started to add a target for NXPs K32L2B3
- converted all TABs to SPACEs
- added a few lines for STM32H7
- made the pin defines optional for all targets that have one
- split the ATSAMC21 and ATSAMx51 targets into separate sections
- updated the explanation of how DMA works
- added a TMS320F28335 target
- added more defines for ATSAMC21 and ATSAMx51 - chip crises...
- added a RP2040 Arduino target using wizio-pico
- modified the WIZIOPICO target for Arduino RP2040 to also work with ArduinoCore-mbed
- removed the 4.0 history
- fixed the GD32C103 target, as a first step it works without DMA now
- added a GD32C103 target with DMA support
- moved targets to extra header files: ATSAMC21, ICCAVR, V851, XMEGA, AVR, Tricore, ATSAMx5x
- moved targets to extra header files: GD32VF103, STM32, ESP32, RP2040, S32K14x, K32L2B31, GD32C103
- moved targets to extra header files: MSP432, TMS320C28XX
- moved all the Arduino targets to extra header files
- basic maintenance: checked for violations of white space and indent rules
- fix: wrong target definition file names for XMEGA and AVR
- added STM32G0
- added ARDUINO_TEENSY40 to the Teensy 4 target
- added ARDUINO_UNOR4_MINIMA and ARDUINO_UNOR4_WIFI targets
- added ARDUINO_GIGA target
- added ARDUINO_PORTENTA_H7 target
- added generic Arduino STM32 target
- fix: Tricore and V851 targets did not use the sub-folder for the EVE_target_xx.h file
- fix: while working, the check for multiple different targets was not implemented correctly
- added ARDUINO_HLK_w80x target for W801, W806 and Air103 boards
- modified the RISC-V entry as there are ESP32 now with RISC-V core
- added detection of the Tasking compiler and added tc38x and tc39xb targets
- removed the unfortunately defunct WIZIOPICO
- added STM32WB55xx

*/

#ifndef EVE_TARGET_H
#define EVE_TARGET_H

/* While the following lines make things a lot easier like automatically */
/* compiling the code for the target you are compiling for, */
/* a few things are expected to be taken care of beforehand. */
/* - setting the Chip-Select and Power-Down pins to Output, */
/* Chip-Select = 1 and Power-Down = 0 */
/* - setting up the SPI which may or not include things like
    - setting the pins for the SPI to output or some alternate I/O function
        or mapping that functionality to that pin
    - if that is an option with the controller your are using you probably
        should set the drive-strength for the SPI pins to high
    - setting the SS pin on AVRs to output in case it is not used
        for Chip-Select or Power-Down
    - setting SPI to mode 0
    - setting SPI to 8 bit with MSB first
    - setting SPI clock to no more than 11 MHz for the init

  For the SPI transfers single 8-Bit transfers are used with
  busy-wait for completion.
  While this is okay for AVRs that run at 16MHz with the SPI at 8 MHz and
  therefore do one transfer in 16 clock-cycles, this is wasteful
  for any 32 bit controller even at higher SPI speeds.

  Check out the section for SAMC21E18A as it has code to transparently add DMA.

  If the define "EVE_DMA" is set the spi_transmit_async() is changed at
  compile time to write in a buffer instead directly to SPI.
  EVE_init() calls EVE_init_dma() which sets up the DMA channel
  and enables an IRQ for end of DMA.
  EVE_start_cmd_burst() is changed from sending the first bytes by SPI to
    resetting the the DMA buffer.
  EVE_end_cmd_burst() just calls EVE_start_dma_transfer() which triggers
    the transfer of the SPI buffer by DMA.
  EVE_busy() does nothing but to report that EVE is busy
    if there is an active DMA transfer.
  At the end of the DMA transfer an IRQ is executed which clears the DMA
    active state and calls EVE_cs_clear() by which the command buffer
    is executed by the command co-processor.
*/

#if !defined (ARDUINO)

#if defined (__IMAGECRAFT__)
#if defined (_AVR)

#include "EVE_target/EVE_target_ICCAVR.h"

#endif
#endif

#if defined (__GNUC__)

/* ################################################################## */
/* ################################################################## */

#if defined (__AVR_XMEGA__)

#include "EVE_target/EVE_target_XMEGA.h"

#endif /* XMEGA */

/* ################################################################## */
/* ################################################################## */

#if defined (__AVR__) && !defined (__AVR_XMEGA__)

#include "EVE_target/EVE_target_AVR.h"

#endif /* AVR */

/* ################################################################## */
/* ################################################################## */

#if defined (__v851__)

#include "EVE_target/EVE_target_V851.h"

#endif /* V851 */

/* ################################################################## */
/* ################################################################## */

#if defined (__TRICORE__)

#include "EVE_target/EVE_target_Tricore.h"

#endif /* __TRICORE__ */

/* ################################################################## */
/* ################################################################## */

#if defined (__SAMC21E18A__) \
    || defined (__SAMC21J18A__) \
    || defined (__SAMC21J17A__)
/* note: target as set by AtmelStudio, valid  are all from the same family */

#include "EVE_target/EVE_target_ATSAMC21.h"

#endif /* SAMC2x */

/* ################################################################## */
/* ################################################################## */

#if defined (__SAME51J19A__) \
    || defined (__SAME51J18A__) \
    || defined (__SAMD51P20A__) \
    || defined (__SAMD51J19A__) \
    || defined (__SAMD51G18A__)
/* note: target as set by AtmelStudio, valid  are all from the same family */

#include "EVE_target/EVE_target_ATSAMx5x.h"

#endif /* SAMx5x */

/* ################################################################## */
/* ################################################################## */

#if defined (__riscv) && !defined (ESP_PLATFORM)

#include "EVE_target/EVE_target_GD32VF103.h"

#endif /* __riscv */

/* ################################################################## */
/* ################################################################## */

#if defined (STM32L0) \
    || defined (STM32F0) \
    || defined (STM32F1) \
    || defined (STM32F3) \
    || defined (STM32F4) \
    || defined (STM32G4) \
    || defined (STM32H7) \
    || defined (STM32G0) \
	|| defined (STM32WB) \
	|| defined (STM32WB55xx)
/* set with "build_flags" in platformio.ini or as defines in your build environment */

#include "EVE_target/EVE_target_STM32.h"

#endif /* STM32 */

/* ################################################################## */
/* ################################################################## */

#if defined (ESP_PLATFORM)

#include "EVE_target/EVE_target_ESP32.h"

#endif /* ESP_PLATFORM */

/* ################################################################## */
/* ################################################################## */

#if defined (RP2040)
/* note: set in platformio.ini by "build_flags = -D RP2040" */

#include "EVE_target/EVE_target_RP2040.h"

#endif /* RP2040 */

/* ################################################################## */
/* ################################################################## */

#if defined (CPU_S32K148) \
    || defined (CPU_S32K144HFT0VLLT)

#include "EVE_target/EVE_target_S32K14x.h"

#endif /* S32K14x */

/* ################################################################## */
/* ################################################################## */

#if defined (CPU_K32L2B31VLH0A)

#include "EVE_target/EVE_target_K32L2B31.h"

#endif /* K32L2B3 */

/* ################################################################## */
/* ################################################################## */

#if defined (GD32C103)
/* note: set in platformio.ini by "build_flags = -D GD32C103" */

#include "EVE_target/EVE_target_GD32C103.h"

#endif /* GD32C103 */

/* ################################################################## */
/* ################################################################## */

#endif /* __GNUC__ */

/* ################################################################## */
/* ################################################################## */

#if defined (__TI_ARM__)

#if defined (__MSP432P401R__)

#include "EVE_target/EVE_target_MSP432.h"

#endif /* __MSP432P401R__ */
#endif /* __TI_ARM */

/* ################################################################## */
/* ################################################################## */

/* this is for TIs C2000 compiled with their ti-cgt-c2000 compiler which does not define this many symbols */
#if defined (__TMS320C28XX__)

#include "EVE_target/EVE_target_TMS320C28XX.h"

#endif

/* ################################################################## */
/* ################################################################## */

#if defined(__TASKING__)

#if (__CPU__ == tc38x) || (__CPU__ == tc39xb)

#include "EVE_target/EVE_target_Tricore_Tasking.h"

#endif
#endif

/* ################################################################## */
/* ################################################################## */

#endif /* !Arduino */

#if defined (ARDUINO)

#if defined (__AVR__)
//#if defined (ARDUINO_AVR_UNO)

#include "EVE_target/EVE_target_Arduino_AVR.h"

#elif defined (ARDUINO_BBC_MICROBIT_V2)

/* note: gave up and postponed implementation, this is not working, yet */
#include "EVE_target/EVE_target_Arduino_BBC_Microbit_V2.h"

#elif defined (ESP32)

#include "EVE_target/EVE_target_Arduino_ESP32.h"

#elif defined (ESP8266)

#include "EVE_target/EVE_target_Arduino_ESP8266.h"

#elif defined (ARDUINO_METRO_M4)

#include "EVE_target/EVE_target_Arduino_Metro_M4.h"

#elif defined (ARDUINO_NUCLEO_F446RE)

#include "EVE_target/EVE_target_Arduino_Nucleo_F446RE.h"

#elif defined (ARDUINO_RASPBERRY_PI_PICO)

#include "EVE_target/EVE_target_Arduino_RP2040.h"

#elif defined (ARDUINO_TEENSY41) || defined (ARDUINO_TEENSY40)

#include "EVE_target/EVE_target_Arduino_Teensy4.h"

#elif defined (ARDUINO_TEENSY35) /* note: this is mostly untested */

#include "EVE_target/EVE_target_Arduino_Teensy35.h"

#elif defined (XMC1100_XMC2GO)

#include "EVE_target/EVE_target_Arduino_XMC1100_XMC2GO.h"

#elif defined (ARDUINO_UNOR4_MINIMA) || defined (ARDUINO_UNOR4_WIFI)

#include "EVE_target/EVE_target_Arduino_UNO_R4.h"

#elif defined (ARDUINO_GIGA)

#include  "EVE_target/EVE_target_Arduino_GIGA_R1.h"

#elif defined (ARDUINO_PORTENTA_H7_M7) || defined (ARDUINO_PORTENTA_H7_M4)

#include  "EVE_target/EVE_target_Arduino_Portenta_H7.h"

#elif defined (ARDUINO_HLK_w80x)

#include  "EVE_target/EVE_target_Arduino_W80x.h"

#elif defined (ARDUINO_ARCH_STM32)

/* this is a generic STM32 target as fall-thru, it uses a buffer to transfer */
#include  "EVE_target/EVE_target_Arduino_STM32_generic.h"

#else

/* generic functions for other Arduino architectures */
#include "EVE_target/EVE_target_Arduino_generic.h"

#endif

#endif /* Arduino */

#endif /* EVE_TARGET_H_ */
