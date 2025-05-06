/*
@file    EVE_target_STM32.h
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
- extracted from EVE_target.h
- basic maintenance: checked for violations of white space and indent rules
- added STM32WB55xx

*/

#ifndef EVE_TARGET_STM32_H
#define EVE_TARGET_STM32_H

#if !defined (ARDUINO)
#if defined (__GNUC__)

/* set with "build_flags" in platformio.ini or as defines in your build environment */
#if defined (STM32L0) \
    || defined (STM32F0) \
    || defined (STM32F1) \
    || defined (STM32F3) \
    || defined (STM32F4) \
    || defined (STM32G4) \
    || defined (STM32H7) \
    || defined (STM32G0) \
	|| defined (STM32WB55xx)

#if defined (STM32L0) /* set with "build_flags = -D STM32L0" in platformio.ini */
#include "stm32l0xx.h"
#include "stm32l0xx_hal.h"
#include "stm32l0xx_ll_spi.h"
#endif

#if defined (STM32F0) /* set with "build_flags = -D STM32F0" in platformio.ini */
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_ll_spi.h"
#endif

#if defined (STM32F1) /* set with "build_flags = -D STM32F1" in platformio.ini */
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_spi.h"
#endif

#if defined (STM32F3) /* set with "build_flags = -D STM32F3" in platformio.ini */
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_spi.h"
#endif

#if defined (STM32F4) /* set with "build_flags = -D STM32F4" in platformio.ini */
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_spi.h"
#endif

#if defined (STM32G4) /* set with "build_flags = -D STM32G4" in platformio.ini */
#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_spi.h"
#endif

#if defined (STM32H7) /* set with "build_flags = -D STM32H7" in platformio.ini */
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_ll_spi.h"
#endif

#if defined (STM32G0) /* set with "build_flags = -D STM32G0" in platformio.ini */
#include "stm32g0xx.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_ll_spi.h"
#endif

#if defined (STM32WB55xx) /* set with "build_flags = -D STM32WB55xx" in platformio.ini */
#include "stm32wbxx.h"
#include "stm32wbxx_hal.h"
#include "stm32wbxx_ll_spi.h"
#endif


/* you may define these in your build-environment to use different settings */
#if !defined (EVE_CS)
#define EVE_CS_PORT GPIOA
#define EVE_CS GPIO_PIN_4
#endif

#if !defined (EVE_PDN)
#define EVE_PDN_PORT GPIOA
#define EVE_PDN GPIO_PIN_3
#endif

#if !defined (EVE_SPI)
#define EVE_SPI SPI1
#endif

#if !defined (EVE_DMA_INSTANCE)
#define EVE_DMA_INSTANCE DMA2
#endif

#if !defined (EVE_DMA_CHANNEL)
#define EVE_DMA_CHANNEL 3
#endif

#if !defined (EVE_DMA_STREAM)
#define EVE_DMA_STREAM 3
#endif
/* you may define these in your build-environment to use different settings */

/* do not activate, it is not working, yet */
//#define EVE_DMA /* to be defined in the build-environment */

#if defined (EVE_DMA)
    extern uint32_t EVE_dma_buffer[1025U];
    extern volatile uint16_t EVE_dma_buffer_index;
    extern volatile uint8_t EVE_dma_busy;

    void EVE_init_dma(void);
    void EVE_start_dma_transfer(void);
#endif

#define DELAY_MS(ms) HAL_Delay(ms)

static inline void EVE_pdn_clear(void)
{
    HAL_GPIO_WritePin(EVE_PDN_PORT, EVE_PDN, GPIO_PIN_SET);
}

static inline void EVE_pdn_set(void)
{
    HAL_GPIO_WritePin(EVE_PDN_PORT, EVE_PDN, GPIO_PIN_RESET);
}

static inline void EVE_cs_clear(void)
{
    HAL_GPIO_WritePin(EVE_CS_PORT, EVE_CS, GPIO_PIN_SET);
}

static inline void EVE_cs_set(void)
{
    HAL_GPIO_WritePin(EVE_CS_PORT, EVE_CS, GPIO_PIN_RESET);
}

#if defined (STM32H7)
static inline void spi_transmit(uint8_t data)
{
    LL_SPI_TransmitData8(EVE_SPI, data);
    while (!LL_SPI_IsActiveFlag_TXP(EVE_SPI)) {}
    while (!LL_SPI_IsActiveFlag_RXWNE(EVE_SPI)) {}
    LL_SPI_ReceiveData8(EVE_SPI); /* dummy read-access to clear SPI_SR_RXWNE */
}
#else
static inline void spi_transmit(uint8_t data)
{
    LL_SPI_TransmitData8(EVE_SPI, data);
    while (!LL_SPI_IsActiveFlag_TXE(EVE_SPI)) {}
    while (!LL_SPI_IsActiveFlag_RXNE(EVE_SPI)) {}
    LL_SPI_ReceiveData8(EVE_SPI); /* dummy read-access to clear SPI_SR_RXNE */
}
#endif

static inline void spi_transmit_32(uint32_t data)
{
    spi_transmit((uint8_t)(data & 0x000000ff));
    spi_transmit((uint8_t)(data >> 8));
    spi_transmit((uint8_t)(data >> 16));
    spi_transmit((uint8_t)(data >> 24));
}

/* spi_transmit_burst() is only used for cmd-FIFO commands */
/* so it *always* has to transfer 4 bytes */
static inline void spi_transmit_burst(uint32_t data)
{
#if defined (EVE_DMA)
    EVE_dma_buffer[EVE_dma_buffer_index++] = data;
#else
    spi_transmit_32(data);
#endif
}

#if defined (STM32H7)
static inline uint8_t spi_receive(uint8_t data)
{
    LL_SPI_TransmitData8(EVE_SPI, data);
    while (!LL_SPI_IsActiveFlag_TXP(EVE_SPI)) {}
    while (!LL_SPI_IsActiveFlag_RXWNE(EVE_SPI)) {}
    return (LL_SPI_ReceiveData8(EVE_SPI));
}
#else
static inline uint8_t spi_receive(uint8_t data)
{
    LL_SPI_TransmitData8(EVE_SPI, data);
    while (!LL_SPI_IsActiveFlag_TXE(EVE_SPI)) {}
    while (!LL_SPI_IsActiveFlag_RXNE(EVE_SPI)) {}
    return (LL_SPI_ReceiveData8(EVE_SPI));
}
#endif

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif  /* STM32 */

#endif /* __GNUC__ */

#endif /* !Arduino */

#endif /* EVE_TARGET_STM32_H */
