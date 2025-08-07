#ifndef EVE_TARGET_H
#define EVE_TARGET_H

#include "../../draw/eve/lv_draw_eve_private.h"
#include "../../tick/lv_tick.h"
#include "../../misc/lv_utils.h"

static inline void DELAY_MS(uint16_t ms)
{
    lv_delay_ms(ms);
}

static inline void EVE_cs_set(void)
{
    lv_draw_eve_unit_g->op_cb(lv_draw_eve_unit_g->disp, LV_DRAW_EVE_OPERATION_CS_ASSERT, NULL, 0);
}

static inline void EVE_cs_clear(void)
{
    lv_draw_eve_unit_g->op_cb(lv_draw_eve_unit_g->disp, LV_DRAW_EVE_OPERATION_CS_DEASSERT, NULL, 0);
}

static inline void EVE_pdn_set(void)
{
    lv_draw_eve_unit_g->op_cb(lv_draw_eve_unit_g->disp, LV_DRAW_EVE_OPERATION_POWERDOWN_SET, NULL, 0);
}

static inline void EVE_pdn_clear(void)
{
    lv_draw_eve_unit_g->op_cb(lv_draw_eve_unit_g->disp, LV_DRAW_EVE_OPERATION_POWERDOWN_CLEAR, NULL, 0);
}

static inline void spi_transmit(uint8_t data)
{
    lv_draw_eve_unit_g->op_cb(lv_draw_eve_unit_g->disp, LV_DRAW_EVE_OPERATION_SPI_SEND, &data, 1);
}

static inline void spi_transmit_32(uint32_t data)
{
#if LV_BIG_ENDIAN_SYSTEM
    data = lv_swap_bytes_32(data);
#endif
    lv_draw_eve_unit_g->op_cb(lv_draw_eve_unit_g->disp, LV_DRAW_EVE_OPERATION_SPI_SEND, &data, 4);
}

static inline void spi_transmit_burst(uint32_t data)
{
    spi_transmit_32(data);
}

static inline uint8_t spi_receive(uint8_t data)
{
    /* `data` is 0 everywhere `spi_receive` is called in the FT800-FT813 library */
    LV_UNUSED(data);

    uint8_t byte;
    lv_draw_eve_unit_g->op_cb(lv_draw_eve_unit_g->disp, LV_DRAW_EVE_OPERATION_SPI_RECEIVE, &byte, 1);

    return byte;
}

static inline uint8_t fetch_flash_byte(const uint8_t *p_data)
{
    return (*p_data);
}

#endif /* EVE_TARGET_H_ */
