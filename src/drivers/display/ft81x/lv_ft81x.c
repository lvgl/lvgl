/**
 * @file lv_ft81x.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_ft81x.h"
#if LV_USE_FT81X

#include "lv_ft81x_defines.h"

#include "../../../stdlib/lv_mem.h"
#include "../../../stdlib/lv_sprintf.h"
#include "../../../stdlib/lv_string.h"
#include "../../../misc/lv_types.h"
#include "../../../misc/lv_utils.h"

/*********************
 *      DEFINES
 *********************/

/* Increase as functionality is added if needed. */
#define LV_FT81X_CMD_BUF_SIZE 63

/* The PWM value that corresponds to the backlight being "on".
   0x80 was found to work on at least two boards but should
   be changed as needed. */
#define PWM_DUTY_BACKLIGHT_ON 0x80

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_ft81x_spi_cb_t spi_cb;
    void * user_data;
    uint16_t cmd_offset;
} lv_ft81x_driver_data_t;

typedef struct {
    uint32_t buf_len;
    uint8_t buf[LV_FT81X_CMD_BUF_SIZE];
} lv_ft81x_cmd_list_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t initialize(lv_display_t * disp, const lv_ft81x_parameters_t * params);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void delete_cb(lv_event_t * e);
static void lv_ft81x_cmd(lv_display_t * disp, uint8_t command, uint8_t parameter);
static uint8_t lv_ft81x_read_8(lv_display_t * disp, uint32_t address);
static uint16_t lv_ft81x_read_16(lv_display_t * disp, uint32_t address);
/* static uint32_t lv_ft81x_read_32(lv_display_t * disp, uint32_t address); */
static void lv_ft81x_write_8(lv_display_t * disp, uint32_t address, uint8_t val);
static void lv_ft81x_write_16(lv_display_t * disp, uint32_t address, uint16_t val);
static void lv_ft81x_write_32(lv_display_t * disp, uint32_t address, uint32_t val);
static void lv_ft81x_cmd_list_init(lv_display_t * disp, lv_ft81x_cmd_list_t * cmd_list);
static void lv_ft81x_cmd_list_add_16(lv_display_t * disp, lv_ft81x_cmd_list_t * cmd_list, uint16_t value);
static void lv_ft81x_cmd_list_add_32(lv_display_t * disp, lv_ft81x_cmd_list_t * cmd_list, uint32_t value);
static void lv_ft81x_cmd_list_send(lv_display_t * disp, lv_ft81x_cmd_list_t * cmd_list);
static void lv_ft81x_encode_read_address(void * dst_4_bytes, uint32_t address);
static void lv_ft81x_encode_write_address(void * dst_3_bytes, uint32_t address);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#if LV_BIG_ENDIAN_SYSTEM
    #define BE_TO_OR_FROM_NATIVE_32(x) ((uint32_t)(x))
    #define BE_TO_OR_FROM_NATIVE_16(x) ((uint16_t)(x))
    #define LE_TO_OR_FROM_NATIVE_32(x) lv_swap_bytes_32(x)
    #define LE_TO_OR_FROM_NATIVE_16(x) lv_swap_bytes_16(x)
#else
    #define BE_TO_OR_FROM_NATIVE_32(x) lv_swap_bytes_32(x)
    #define BE_TO_OR_FROM_NATIVE_16(x) lv_swap_bytes_16(x)
    #define LE_TO_OR_FROM_NATIVE_32(x) ((uint32_t)(x))
    #define LE_TO_OR_FROM_NATIVE_16(x) ((uint16_t)(x))
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_ft81x_create(const lv_ft81x_parameters_t * params, void * partial_buf, uint32_t buf_size,
                               lv_ft81x_spi_cb_t spi_cb, void * user_data)
{
    LV_ASSERT_NULL(spi_cb);

    lv_display_t * disp = lv_display_create(params->hor_res, params->ver_res);

    lv_ft81x_driver_data_t * drv = lv_malloc_zeroed(sizeof(lv_ft81x_driver_data_t));
    LV_ASSERT_MALLOC(drv);
    drv->spi_cb = spi_cb;
    drv->user_data = user_data;
    drv->cmd_offset = 0;
    lv_display_set_driver_data(disp, drv);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(disp, partial_buf, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_add_event_cb(disp, delete_cb, LV_EVENT_DELETE, NULL);

    lv_result_t init_res = initialize(disp, params);
    if(init_res != LV_RESULT_OK) {
        lv_display_delete(disp);
        return NULL;
    }

    return disp;
}

void * lv_ft81x_get_user_data(lv_display_t * disp)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    return drv->user_data;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t initialize(lv_display_t * disp, const lv_ft81x_parameters_t * params)
{
    lv_ft81x_cmd(disp, params->has_crystal ? EVE_CLKEXT : EVE_CLKINT, 0);
    if(params->is_bt81x) lv_ft81x_cmd(disp, EVE_CLKSEL, 0x46);
    lv_ft81x_cmd(disp, EVE_ACTIVE, 0);

    /* at least 40 ms is needed for EVE to become ready. */
    lv_delay_ms(40);

    uint32_t start_millis = lv_tick_get();
    while(lv_ft81x_read_8(disp, REG_ID) != 0x7c) {
        if(lv_tick_get() - start_millis > 1000) {
            LV_LOG_ERROR("failed to read 0x7C from the ID register 1000 ms after activation.");
            return LV_RESULT_INVALID;
        }
        lv_delay_ms(1);
    };

    start_millis = lv_tick_get();
    while(lv_ft81x_read_8(disp, REG_CPURESET) & 0x03) {
        if(lv_tick_get() - start_millis > 1000) {
            LV_LOG_ERROR("CPURESET register coprocessor and touch engines not in \"working status\" 1000 ms after activation.");
            return LV_RESULT_INVALID;
        }
        lv_delay_ms(1);
    };

    if(params->is_bt81x) lv_ft81x_write_32(disp, REG_FREQUENCY, 72000000);

    lv_ft81x_write_8(disp, REG_PWM_DUTY, PWM_DUTY_BACKLIGHT_ON);

    lv_ft81x_write_16(disp, REG_HSIZE,   params->hor_res);   /* active display width */
    lv_ft81x_write_16(disp, REG_HCYCLE,  params->hcycle);  /* total number of clocks per line, incl front/back porch */
    lv_ft81x_write_16(disp, REG_HOFFSET, params->hoffset); /* start of active line */
    lv_ft81x_write_16(disp, REG_HSYNC0,  params->hsync0);  /* start of horizontal sync pulse */
    lv_ft81x_write_16(disp, REG_HSYNC1,  params->hsync1);  /* end of horizontal sync pulse */
    lv_ft81x_write_16(disp, REG_VSIZE,   params->ver_res);   /* active display height */
    lv_ft81x_write_16(disp, REG_VCYCLE,  params->vcycle);  /* total number of lines per screen, including pre/post */
    lv_ft81x_write_16(disp, REG_VOFFSET, params->voffset); /* start of active screen */
    lv_ft81x_write_16(disp, REG_VSYNC0,  params->vsync0);  /* start of vertical sync pulse */
    lv_ft81x_write_16(disp, REG_VSYNC1,  params->vsync1);  /* end of vertical sync pulse */
    lv_ft81x_write_8(disp, REG_SWIZZLE,  params->swizzle); /* FT8xx output to LCD - pin order */
    lv_ft81x_write_8(disp, REG_PCLK_POL, params->pclkpol); /* LCD data is clocked in on this PCLK edge */
    lv_ft81x_write_8(disp, REG_CSPREAD,
                     params->cspread); /* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */

    /* write a basic display-list to get things started */
    lv_ft81x_write_32(disp, EVE_RAM_DL, DL_CLEAR_RGB);
    lv_ft81x_write_32(disp, EVE_RAM_DL + 4, (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
    lv_ft81x_write_32(disp, EVE_RAM_DL + 8, DL_DISPLAY);    /* end of display list */
    lv_ft81x_write_32(disp, REG_DLSWAP, EVE_DLSWAP_FRAME);

    /* nothing is being displayed yet... the pixel clock is still 0x00 */
    lv_ft81x_write_8(disp, REG_GPIO,
                     0x80); /* enable the DISP signal to the LCD panel, it is set to output in REG_GPIO_DIR by default */
    lv_ft81x_write_8(disp, REG_PCLK, params->pclk); /* now start clocking data to the LCD panel */

    LV_ASSERT(lv_ft81x_read_16(disp, REG_CMD_READ) != 0xfff);

    LV_ASSERT(0 == lv_ft81x_read_16(disp, REG_CMD_WRITE));


    lv_ft81x_cmd_list_t cmd_list;
    lv_ft81x_cmd_list_init(disp, &cmd_list);
    lv_ft81x_cmd_list_add_32(disp, &cmd_list, CMD_MEMSET);
    lv_ft81x_cmd_list_add_32(disp, &cmd_list, 0); /* address */
    lv_ft81x_cmd_list_add_32(disp, &cmd_list, 0x00); /* val */
    lv_ft81x_cmd_list_add_32(disp, &cmd_list, 2 * params->hor_res * params->ver_res); /* count */
    lv_ft81x_cmd_list_send(disp, &cmd_list);

    lv_ft81x_cmd_list_init(disp, &cmd_list);

    lv_ft81x_cmd_list_add_32(disp, &cmd_list, CMD_DLSTART);

    lv_ft81x_cmd_list_add_32(disp, &cmd_list, DL_CLEAR_RGB | 0); /* clear to black */
    lv_ft81x_cmd_list_add_32(disp, &cmd_list, DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);

    lv_ft81x_cmd_list_add_32(disp, &cmd_list, TAG(0));

    lv_ft81x_cmd_list_add_32(disp, &cmd_list, TAG(20));
    lv_ft81x_cmd_list_add_32(disp, &cmd_list, CMD_SETBITMAP);
    lv_ft81x_cmd_list_add_32(disp, &cmd_list, 0); /* address */
    lv_ft81x_cmd_list_add_16(disp, &cmd_list, EVE_RGB565);
    lv_ft81x_cmd_list_add_16(disp, &cmd_list, params->hor_res);
    lv_ft81x_cmd_list_add_16(disp, &cmd_list, params->ver_res);
    lv_ft81x_cmd_list_add_16(disp, &cmd_list, 0);

    lv_ft81x_cmd_list_add_32(disp, &cmd_list, DL_BEGIN | EVE_BITMAPS);
    lv_ft81x_cmd_list_add_32(disp, &cmd_list, VERTEX2F(0, 0));
    lv_ft81x_cmd_list_add_32(disp, &cmd_list, DL_END);

    lv_ft81x_cmd_list_add_32(disp, &cmd_list, TAG(0));

    lv_ft81x_cmd_list_add_32(disp, &cmd_list, DL_DISPLAY);

    lv_ft81x_cmd_list_add_32(disp, &cmd_list, CMD_SWAP);

    lv_ft81x_cmd_list_send(disp, &cmd_list);


    return LV_RESULT_OK;
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);

    if(drv->spi_cb == NULL) {
        LV_LOG_ERROR("The SPI callback is NULL.");
        return;
    }

    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    uint32_t disp_row_bytes = hor_res * 2;

    uint32_t address = disp_row_bytes * area->y1 + area->x1 * 2;
    uint8_t encoded_address[3];

    if(lv_area_get_width(area) == hor_res) {
        lv_ft81x_encode_write_address(encoded_address, address);
        drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
        drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, encoded_address, sizeof(encoded_address));
        drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, px_map, lv_area_get_size(area) * 2);
        drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
    }
    else {
        uint32_t area_height = lv_area_get_height(area);
        uint32_t area_row_bytes = lv_area_get_width(area) * 2;
        for(uint32_t i = 0; i < area_height; i++) {
            lv_ft81x_encode_write_address(encoded_address, address);
            drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
            drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, encoded_address, sizeof(encoded_address));
            drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, px_map, area_row_bytes);
            drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
            address += disp_row_bytes;
            px_map += area_row_bytes;
        }
    }

    lv_display_flush_ready(disp);
}

static void delete_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    lv_free(drv);
}

static void lv_ft81x_cmd(lv_display_t * disp, uint8_t command, uint8_t parameter)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    uint8_t data[3] = {command, parameter, 0x00};
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, data, sizeof(data));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
}

static uint8_t lv_ft81x_read_8(lv_display_t * disp, uint32_t address)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    lv_ft81x_encode_read_address(&address, address);
    uint8_t val;
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, &address, sizeof(address));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_RECEIVE, &val, sizeof(val));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
    return val;
}

static uint16_t lv_ft81x_read_16(lv_display_t * disp, uint32_t address)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    lv_ft81x_encode_read_address(&address, address);
    uint16_t val;
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, &address, sizeof(address));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_RECEIVE, &val, sizeof(val));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
    val = LE_TO_OR_FROM_NATIVE_16(val);
    return val;
}

static void lv_ft81x_write_8(lv_display_t * disp, uint32_t address, uint8_t val)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    uint8_t data[4];
    lv_ft81x_encode_write_address(data, address);
    data[3] = val;
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, data, sizeof(data));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
}

static void lv_ft81x_write_16(lv_display_t * disp, uint32_t address, uint16_t val)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    val = LE_TO_OR_FROM_NATIVE_16(val);
    uint8_t data[5];
    lv_ft81x_encode_write_address(data, address);
    lv_memcpy(data + 3, &val, 2);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, data, sizeof(data));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
}

static void lv_ft81x_write_32(lv_display_t * disp, uint32_t address, uint32_t val)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    val = LE_TO_OR_FROM_NATIVE_32(val);
    uint8_t data[7];
    lv_ft81x_encode_write_address(data, address);
    lv_memcpy(data + 3, &val, 4);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, data, sizeof(data));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
}

static void lv_ft81x_cmd_list_init(lv_display_t * disp, lv_ft81x_cmd_list_t * cmd_list)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    LV_ASSERT_MSG(LV_FT81X_CMD_BUF_SIZE >= 3, "increase LV_FT81X_CMD_BUF_SIZE as needed");
    lv_ft81x_encode_write_address(cmd_list->buf, EVE_RAM_CMD + drv->cmd_offset);
    cmd_list->buf_len = 3;
}

static void lv_ft81x_cmd_list_add_16(lv_display_t * disp, lv_ft81x_cmd_list_t * cmd_list, uint16_t value)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    uint8_t * buf_dst = &cmd_list->buf[cmd_list->buf_len];
    cmd_list->buf_len += 2;
    LV_ASSERT_MSG(cmd_list->buf_len <= LV_FT81X_CMD_BUF_SIZE, "increase LV_FT81X_CMD_BUF_SIZE as needed");
    value = LE_TO_OR_FROM_NATIVE_16(value);
    lv_memcpy(buf_dst, &value, 2);
    drv->cmd_offset = (drv->cmd_offset + 2) & 0xfff; /* circular */
}

static void lv_ft81x_cmd_list_add_32(lv_display_t * disp, lv_ft81x_cmd_list_t * cmd_list, uint32_t value)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    uint8_t * buf_dst = &cmd_list->buf[cmd_list->buf_len];
    cmd_list->buf_len += 4;
    LV_ASSERT_MSG(cmd_list->buf_len <= LV_FT81X_CMD_BUF_SIZE, "increase LV_FT81X_CMD_BUF_SIZE as needed");
    value = LE_TO_OR_FROM_NATIVE_32(value);
    lv_memcpy(buf_dst, &value, 4);
    drv->cmd_offset = (drv->cmd_offset + 4) & 0xfff; /* circular */
}

static void lv_ft81x_cmd_list_send(lv_display_t * disp, lv_ft81x_cmd_list_t * cmd_list)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, cmd_list->buf, cmd_list->buf_len);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
    lv_ft81x_write_16(disp, REG_CMD_WRITE, drv->cmd_offset);
}

static void lv_ft81x_encode_read_address(void * dst_4_bytes, uint32_t address)
{
    address = BE_TO_OR_FROM_NATIVE_32(address << 8);
    lv_memcpy(dst_4_bytes, &address, 4);
}

static void lv_ft81x_encode_write_address(void * dst_3_bytes, uint32_t address)
{
    address = BE_TO_OR_FROM_NATIVE_32((address | 0x800000) << 8);
    lv_memcpy(dst_3_bytes, &address, 3);
}

#endif /*LV_USE_FT81X*/
