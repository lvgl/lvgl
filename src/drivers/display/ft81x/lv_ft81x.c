/**
 * @file lv_ft81x.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_ft81x.h"
#if LV_USE_FT81X

#include "../../../stdlib/lv_mem.h"
#include "../../../stdlib/lv_sprintf.h"
#include "../../../stdlib/lv_string.h"
#include "../../../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/* From https://github.com/lvgl/lvgl_esp32_drivers/blob/master/lvgl_tft/EVE.h */
#define EVE_CLKEXT	0x44  /* select external clock source */
#define EVE_CLKINT	0x48  /* select internal clock source */
#define EVE_CLKSEL			0x61 /* configure system clock */
#define EVE_ACTIVE	0x00  /* place FT8xx in active state */

#define DL_CLEAR		0x26000000UL /* requires OR'd arguments */
#define DL_CLEAR_RGB	0x02000000UL /* requires OR'd arguments */
#define DL_COLOR_RGB	0x04000000UL /* requires OR'd arguments */
#define DL_POINT_SIZE	0x0D000000UL /* requires OR'd arguments */
#define DL_END			0x21000000UL
#define DL_BEGIN		0x1F000000UL /* requires OR'd arguments */
#define DL_DISPLAY		0x00000000UL

#define CLR_COL              0x4
#define CLR_STN              0x2
#define CLR_TAG              0x1

/* Graphics display list swap defines */
#define EVE_DLSWAP_DONE          0UL
#define EVE_DLSWAP_LINE          1UL
#define EVE_DLSWAP_FRAME         2UL

/* Memory definitions */
#define EVE_RAM_G			0x000000UL
#define EVE_ROM_CHIPID		0x0C0000UL
#define EVE_ROM_FONT		0x1E0000UL
#define EVE_ROM_FONT_ADDR	0x2FFFFCUL
#define EVE_RAM_DL			0x300000UL
#define EVE_RAM_REG			0x302000UL
#define EVE_RAM_CMD			0x308000UL

/* Bitmap formats */
#define EVE_ARGB1555             0UL
#define EVE_L1                   1UL
#define EVE_L4                   2UL
#define EVE_L8                   3UL
#define EVE_RGB332               4UL
#define EVE_ARGB2                5UL
#define EVE_ARGB4                6UL
#define EVE_RGB565               7UL
#define EVE_PALETTED             8UL
#define EVE_TEXT8X8              9UL
#define EVE_TEXTVGA              10UL
#define EVE_BARGRAPH             11UL

/* Graphics primitives */
#define EVE_BITMAPS              1UL
#define EVE_POINTS               2UL
#define EVE_LINES                3UL
#define EVE_LINE_STRIP           4UL
#define EVE_EDGE_STRIP_R         5UL
#define EVE_EDGE_STRIP_L         6UL
#define EVE_EDGE_STRIP_A         7UL
#define EVE_EDGE_STRIP_B         8UL
#define EVE_RECTS                9UL

/* additional commands for FT81x */
#define CMD_MEDIAFIFO			0xFFFFFF39
#define CMD_PLAYVIDEO			0xFFFFFF3A
#define CMD_ROMFONT				0xFFFFFF3F
#define CMD_SETBASE				0xFFFFFF38
#define CMD_SETBITMAP			0xFFFFFF43
#define CMD_SETFONT2			0xFFFFFF3B
#define CMD_SETROTATE			0xFFFFFF36
#define CMD_SETSCRATCH			0xFFFFFF3C
#define CMD_SNAPSHOT2			0xFFFFFF37
#define CMD_VIDEOFRAME			0xFFFFFF41
#define CMD_VIDEOSTART			0xFFFFFF40

/* commands common to EVE/EVE2/EVE3 */
#define CMD_APPEND				0xFFFFFF1E
#define CMD_BGCOLOR				0xFFFFFF09
#define CMD_BUTTON				0xFFFFFF0D
#define CMD_CALIBRATE			0xFFFFFF15
#define CMD_CLOCK				0xFFFFFF14
#define CMD_COLDSTART			0xFFFFFF32
#define CMD_DIAL				0xFFFFFF2D
#define CMD_DLSTART				0xFFFFFF00
#define CMD_FGCOLOR				0xFFFFFF0A
#define CMD_GAUGE				0xFFFFFF13
#define CMD_GETMATRIX			0xFFFFFF33
#define CMD_GETPROPS			0xFFFFFF25
#define CMD_GETPTR				0xFFFFFF23
#define CMD_GRADCOLOR			0xFFFFFF34
#define CMD_GRADIENT			0xFFFFFF0B
#define CMD_INFLATE				0xFFFFFF22
#define CMD_INTERRUPT			0xFFFFFF02
#define CMD_KEYS				0xFFFFFF0E
#define CMD_LOADIDENTITY		0xFFFFFF26
#define CMD_LOADIMAGE			0xFFFFFF24
#define CMD_LOGO				0xFFFFFF31
#define CMD_MEMCPY				0xFFFFFF1D
#define CMD_MEMCRC				0xFFFFFF18
#define CMD_MEMSET				0xFFFFFF1B
#define CMD_MEMWRITE			0xFFFFFF1A
#define CMD_MEMZERO				0xFFFFFF1C
#define CMD_NUMBER				0xFFFFFF2E
#define CMD_PROGRESS			0xFFFFFF0F
#define CMD_REGREAD				0xFFFFFF19
#define CMD_ROTATE				0xFFFFFF29
#define CMD_SCALE				0xFFFFFF28
#define CMD_SCREENSAVER			0xFFFFFF2F
#define CMD_SCROLLBAR			0xFFFFFF11
#define CMD_SETFONT				0xFFFFFF2B
#define CMD_SETMATRIX			0xFFFFFF2A
#define CMD_SKETCH				0xFFFFFF30
#define CMD_SLIDER				0xFFFFFF10
#define CMD_SNAPSHOT			0xFFFFFF1F
#define CMD_SPINNER				0xFFFFFF16
#define CMD_STOP				0xFFFFFF17
#define CMD_SWAP				0xFFFFFF01
#define CMD_TEXT				0xFFFFFF0C
#define CMD_TOGGLE				0xFFFFFF12
#define CMD_TRACK				0xFFFFFF2C
#define CMD_TRANSLATE			0xFFFFFF27

#define REG_ANA_COMP         0x302184UL /* only listed in datasheet */
#define REG_BIST_EN          0x302174UL /* only listed in datasheet */
#define REG_CLOCK            0x302008UL
#define REG_CMDB_SPACE       0x302574UL
#define REG_CMDB_WRITE       0x302578UL
#define REG_CMD_DL           0x302100UL
#define REG_CMD_READ         0x3020f8UL
#define REG_CMD_WRITE        0x3020fcUL
#define REG_CPURESET         0x302020UL
#define REG_CSPREAD          0x302068UL
#define REG_CTOUCH_EXTENDED  0x302108UL
#define REG_CTOUCH_TOUCH0_XY 0x302124UL /* only listed in datasheet */
#define REG_CTOUCH_TOUCH4_X  0x30216cUL
#define REG_CTOUCH_TOUCH4_Y  0x302120UL
#define REG_CTOUCH_TOUCH1_XY 0x30211cUL
#define REG_CTOUCH_TOUCH2_XY 0x30218cUL
#define REG_CTOUCH_TOUCH3_XY 0x302190UL
#define REG_TOUCH_CONFIG     0x302168UL
#define REG_DATESTAMP        0x302564UL /* only listed in datasheet */
#define REG_DITHER           0x302060UL
#define REG_DLSWAP           0x302054UL
#define REG_FRAMES           0x302004UL
#define REG_FREQUENCY        0x30200cUL
#define REG_GPIO             0x302094UL
#define REG_GPIOX            0x30209cUL
#define REG_GPIOX_DIR        0x302098UL
#define REG_GPIO_DIR         0x302090UL
#define REG_HCYCLE           0x30202cUL
#define REG_HOFFSET          0x302030UL
#define REG_HSIZE            0x302034UL
#define REG_HSYNC0           0x302038UL
#define REG_HSYNC1           0x30203cUL
#define REG_ID               0x302000UL
#define REG_INT_EN           0x3020acUL
#define REG_INT_FLAGS        0x3020a8UL
#define REG_INT_MASK         0x3020b0UL
#define REG_MACRO_0          0x3020d8UL
#define REG_MACRO_1          0x3020dcUL
#define REG_MEDIAFIFO_READ   0x309014UL /* only listed in programmers guide */
#define REG_MEDIAFIFO_WRITE  0x309018UL /* only listed in programmers guide */
#define REG_OUTBITS          0x30205cUL
#define REG_PCLK             0x302070UL
#define REG_PCLK_POL         0x30206cUL
#define REG_PLAY             0x30208cUL
#define REG_PLAYBACK_FORMAT  0x3020c4UL
#define REG_PLAYBACK_FREQ    0x3020c0UL
#define REG_PLAYBACK_LENGTH  0x3020b8UL
#define REG_PLAYBACK_LOOP    0x3020c8UL
#define REG_PLAYBACK_PLAY    0x3020ccUL
#define REG_PLAYBACK_READPTR 0x3020bcUL
#define REG_PLAYBACK_START   0x3020b4UL
#define REG_PWM_DUTY         0x3020d4UL
#define REG_PWM_HZ           0x3020d0UL
#define REG_RENDERMODE       0x302010UL /* only listed in datasheet */
#define REG_ROTATE           0x302058UL
#define REG_SNAPFORMAT       0x30201cUL /* only listed in datasheet */
#define REG_SNAPSHOT         0x302018UL /* only listed in datasheet */
#define REG_SNAPY            0x302014UL /* only listed in datasheet */
#define REG_SOUND            0x302088UL
#define REG_SPI_WIDTH        0x302188UL /* listed with false offset in programmers guide V1.1 */
#define REG_SWIZZLE          0x302064UL
#define REG_TAG              0x30207cUL
#define REG_TAG_X            0x302074UL
#define REG_TAG_Y            0x302078UL
#define REG_TAP_CRC          0x302024UL /* only listed in datasheet */
#define REG_TAP_MASK         0x302028UL /* only listed in datasheet */
#define REG_TOUCH_ADC_MODE   0x302108UL
#define REG_TOUCH_CHARGE     0x30210cUL
#define REG_TOUCH_DIRECT_XY  0x30218cUL
#define REG_TOUCH_DIRECT_Z1Z2 0x302190UL
#define REG_TOUCH_MODE       0x302104UL
#define REG_TOUCH_OVERSAMPLE 0x302114UL
#define REG_TOUCH_RAW_XY     0x30211cUL
#define REG_TOUCH_RZ         0x302120UL
#define REG_TOUCH_RZTHRESH   0x302118UL
#define REG_TOUCH_SCREEN_XY  0x302124UL
#define REG_TOUCH_SETTLE     0x302110UL
#define REG_TOUCH_TAG        0x30212cUL
#define REG_TOUCH_TAG1       0x302134UL /* only listed in datasheet */
#define REG_TOUCH_TAG1_XY    0x302130UL /* only listed in datasheet */
#define REG_TOUCH_TAG2       0x30213cUL /* only listed in datasheet */
#define REG_TOUCH_TAG2_XY    0x302138UL /* only listed in datasheet */
#define REG_TOUCH_TAG3       0x302144UL /* only listed in datasheet */
#define REG_TOUCH_TAG3_XY    0x302140UL /* only listed in datasheet */
#define REG_TOUCH_TAG4       0x30214cUL /* only listed in datasheet */
#define REG_TOUCH_TAG4_XY    0x302148UL /* only listed in datasheet */
#define REG_TOUCH_TAG_XY     0x302128UL
#define REG_TOUCH_TRANSFORM_A 0x302150UL
#define REG_TOUCH_TRANSFORM_B 0x302154UL
#define REG_TOUCH_TRANSFORM_C 0x302158UL
#define REG_TOUCH_TRANSFORM_D 0x30215cUL
#define REG_TOUCH_TRANSFORM_E 0x302160UL
#define REG_TOUCH_TRANSFORM_F 0x302164UL
#define REG_TRACKER          0x309000UL /* only listed in programmers guide */
#define REG_TRACKER_1        0x309004UL /* only listed in programmers guide */
#define REG_TRACKER_2        0x309008UL /* only listed in programmers guide */
#define REG_TRACKER_3        0x30900cUL /* only listed in programmers guide */
#define REG_TRACKER_4        0x309010UL /* only listed in programmers guide */
#define REG_TRIM             0x302180UL
#define REG_VCYCLE           0x302040UL
#define REG_VOFFSET          0x302044UL
#define REG_VOL_PB           0x302080UL
#define REG_VOL_SOUND        0x302084UL
#define REG_VSIZE            0x302048UL
#define REG_VSYNC0           0x30204cUL
#define REG_VSYNC1           0x302050UL

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_ft81x_spi_cb_t spi_cb;
    void * user_data;
} lv_ft81x_driver_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t initialize(lv_display_t * disp, const lv_ft81x_parameters_t * params);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void delete_cb(lv_event_t * e);
static void lv_ft81x_cmd(lv_display_t * disp, uint8_t command, uint8_t parameter);
static uint8_t lv_ft81x_read_8(lv_display_t * disp, uint32_t address);
static uint16_t lv_ft81x_read_16(lv_display_t * disp, uint32_t address);
static uint32_t lv_ft81x_read_32(lv_display_t * disp, uint32_t address);
static void lv_ft81x_write_8(lv_display_t * disp, uint32_t address, uint8_t val);
static void lv_ft81x_write_16(lv_display_t * disp, uint32_t address, uint16_t val);
static void lv_ft81x_write_32(lv_display_t * disp, uint32_t address, uint32_t val);

static inline uint32_t swap_bytes_32(uint32_t x);
static inline uint16_t swap_bytes_16(uint16_t x);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#if LV_BIG_ENDIAN_SYSTEM
    #define BE_TO_OR_FROM_NATIVE_32(x) ((uint32_t)(x))
    #define BE_TO_OR_FROM_NATIVE_16(x) ((uint16_t)(x))
    #define LE_TO_OR_FROM_NATIVE_32(x) swap_bytes_32(x)
    #define LE_TO_OR_FROM_NATIVE_16(x) swap_bytes_16(x)
#else
    #define BE_TO_OR_FROM_NATIVE_32(x) swap_bytes_32(x)
    #define BE_TO_OR_FROM_NATIVE_16(x) swap_bytes_16(x)
    #define LE_TO_OR_FROM_NATIVE_32(x) ((uint32_t)(x))
    #define LE_TO_OR_FROM_NATIVE_16(x) ((uint16_t)(x))
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_ft81x_create(const lv_ft81x_parameters_t * params, lv_ft81x_spi_cb_t spi_cb, void * user_data)
{
    lv_display_t * disp = lv_display_create(params->hor_res, params->ver_res);

    lv_ft81x_driver_data_t * drv = lv_malloc_zeroed(sizeof(lv_ft81x_driver_data_t));
    LV_ASSERT_MALLOC(drv);
    drv->spi_cb = spi_cb;
    drv->user_data = user_data;
    lv_display_set_driver_data(disp, drv);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_add_event_cb(disp, delete_cb, LV_EVENT_DELETE, NULL);

    lv_result_t init_res = initialize(disp, params);
    if(init_res != LV_RESULT_OK) {
        lv_display_delete(disp);
        return NULL;
    }

    // LV_LOG_USER("READY");
    // lv_delay_ms(3000);

    uint8_t data[63];
    uint32_t u32;
    u32 = BE_TO_OR_FROM_NATIVE_32((EVE_RAM_CMD | 0x800000) << 8);
    lv_memcpy(data, &u32, 3);
    u32 = CMD_MEMSET;
    lv_memcpy(data + 3, &u32, 4);
    u32 = 0;
    lv_memcpy(data + 7, &u32, 4);
    u32 = 0x0f;
    lv_memcpy(data + 11, &u32, 4);
    u32 = 160000;
    lv_memcpy(data + 15, &u32, 4);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, data, 19);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
	lv_ft81x_write_16(disp, REG_CMD_WRITE, 16);

    uint16_t u16;
    u32 = BE_TO_OR_FROM_NATIVE_32(((EVE_RAM_CMD + 16) | 0x800000) << 8);
    lv_memcpy(data, &u32, 3);
    u32 = CMD_DLSTART; // +4
    lv_memcpy(data + 3, &u32, 4);
    u32 = DL_CLEAR_RGB | 0; // +8
    lv_memcpy(data + 7, &u32, 4);
    u32 = DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG; // +12
    lv_memcpy(data + 11, &u32, 4);
    u32 = (3UL<<24)|0; // +16
    lv_memcpy(data + 15, &u32, 4);
    u32 = (3UL<<24)|20; // +20
    lv_memcpy(data + 19, &u32, 4);
    u32 = CMD_SETBITMAP; // +24
    lv_memcpy(data + 23, &u32, 4);
    u32 = 0; // +28
    lv_memcpy(data + 27, &u32, 4);
    u16 = EVE_RGB565; // +30
    lv_memcpy(data + 31, &u16, 2);
    u16 = 800; // +32
    lv_memcpy(data + 33, &u16, 2);
    u16 = 480; // +34
    lv_memcpy(data + 35, &u16, 2);
    u16 = 0; // +36
    lv_memcpy(data + 37, &u16, 2);
    u32 = DL_BEGIN | EVE_BITMAPS; // +40
    lv_memcpy(data + 39, &u32, 4);
    #define VERTEX2F(x,y) ((1UL<<30)|(((x)&32767UL)<<15)|(((y)&32767UL)<<0))
    u32 = VERTEX2F(0, 0); // +44
    lv_memcpy(data + 43, &u32, 4);
    u32 = DL_END; // +48
    lv_memcpy(data + 47, &u32, 4);
    u32 = (3UL<<24)|0; // +52
    lv_memcpy(data + 51, &u32, 4);
    u32 = DL_DISPLAY; // +56
    lv_memcpy(data + 55, &u32, 4);
    u32 = CMD_SWAP; // +60
    lv_memcpy(data + 59, &u32, 4);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, data, 63);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
	lv_ft81x_write_16(disp, REG_CMD_WRITE, 76);

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
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    
    lv_ft81x_cmd(disp, params->has_crystal ? EVE_CLKEXT : EVE_CLKINT, 0);
    if(params->is_bt81x) lv_ft81x_cmd(disp, EVE_CLKSEL, 0x46);
    lv_ft81x_cmd(disp, EVE_ACTIVE, 0);

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

    lv_ft81x_write_8(disp, REG_PWM_DUTY, 0x80);

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
	lv_ft81x_write_8(disp, REG_CSPREAD,	 params->cspread); /* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */

	/* write a basic display-list to get things started */
	lv_ft81x_write_32(disp, EVE_RAM_DL, DL_CLEAR_RGB);
	lv_ft81x_write_32(disp, EVE_RAM_DL + 4, (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
	lv_ft81x_write_32(disp, EVE_RAM_DL + 8, DL_DISPLAY);	/* end of display list */
	lv_ft81x_write_32(disp, REG_DLSWAP, EVE_DLSWAP_FRAME);

	/* nothing is being displayed yet... the pixel clock is still 0x00 */
	lv_ft81x_write_8(disp, REG_GPIO, 0x80); /* enable the DISP signal to the LCD panel, it is set to output in REG_GPIO_DIR by default */
	lv_ft81x_write_8(disp, REG_PCLK, params->pclk); /* now start clocking data to the LCD panel */

    assert(lv_ft81x_read_16(disp, REG_CMD_READ) != 0xfff);

	assert(0 == lv_ft81x_read_16(disp, REG_CMD_WRITE));

    // uint32_t id2 = lv_ft81x_read_32(disp, 0x0C0000);
    // LV_LOG_USER("ID: 0x%08"LV_PRIx32, id2);

    return LV_RESULT_OK;
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    if (area->x1 == 0 && area->y1 == 0 && area->x2 == 799) {
        lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
        uint8_t data[3] = {0x80};
        drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
        drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, data, 3);
        uint32_t len = 50 * 2 * 800;
        while(len) {
            uint32_t sz = len < 64 ? len : 64;
            drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, px_map, sz);
            len -= sz;
            px_map += sz;
        }
        drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
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
    address = BE_TO_OR_FROM_NATIVE_32(address << 8);
    uint8_t val;
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, (uint8_t *)&address, sizeof(address));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_RECEIVE, &val, sizeof(val));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
    return val;
}

static uint16_t lv_ft81x_read_16(lv_display_t * disp, uint32_t address)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    address = BE_TO_OR_FROM_NATIVE_32(address << 8);
    uint16_t val;
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, (uint8_t *)&address, sizeof(address));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_RECEIVE, (uint8_t *)&val, sizeof(val));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
    val = LE_TO_OR_FROM_NATIVE_16(val);
    return val;
}

static uint32_t lv_ft81x_read_32(lv_display_t * disp, uint32_t address)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    address = BE_TO_OR_FROM_NATIVE_32(address << 8);
    uint32_t val;
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, (uint8_t *)&address, sizeof(address));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_RECEIVE, (uint8_t *)&val, sizeof(val));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
    val = LE_TO_OR_FROM_NATIVE_32(val);
    return val;
}

static void lv_ft81x_write_8(lv_display_t * disp, uint32_t address, uint8_t val)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    address = BE_TO_OR_FROM_NATIVE_32((address | 0x800000) << 8);
    uint8_t data[4];
    lv_memcpy(data, &address, 3);
    data[3] = val;
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, data, sizeof(data));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
}

static void lv_ft81x_write_16(lv_display_t * disp, uint32_t address, uint16_t val)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    address = BE_TO_OR_FROM_NATIVE_32((address | 0x800000) << 8);
    val = LE_TO_OR_FROM_NATIVE_16(val);
    uint8_t data[5];
    lv_memcpy(data, &address, 3);
    lv_memcpy(data + 3, &val, 2);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, data, sizeof(data));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
}

static void lv_ft81x_write_32(lv_display_t * disp, uint32_t address, uint32_t val)
{
    lv_ft81x_driver_data_t * drv = lv_display_get_driver_data(disp);
    address = BE_TO_OR_FROM_NATIVE_32((address | 0x800000) << 8);
    val = LE_TO_OR_FROM_NATIVE_32(val);
    uint8_t data[7];
    lv_memcpy(data, &address, 3);
    lv_memcpy(data + 3, &val, 4);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_ASSERT, NULL, 0);
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_SEND, data, sizeof(data));
    drv->spi_cb(disp, LV_FT81X_SPI_OPERATION_CS_DEASSERT, NULL, 0);
}

static inline uint32_t swap_bytes_32(uint32_t x)
{
    union {uint32_t u32; uint8_t u8[4];} y;
    union {uint32_t u32; uint8_t u8[4];} x2;
    x2.u32 = x;
    y.u8[0] = x2.u8[3];
    y.u8[1] = x2.u8[2];
    y.u8[2] = x2.u8[1];
    y.u8[3] = x2.u8[0];
    return y.u32;
}

static inline uint16_t swap_bytes_16(uint16_t x)
{
    union {uint16_t u16; uint8_t u8[2];} y;
    union {uint16_t u16; uint8_t u8[2];} x2;
    x2.u16 = x;
    y.u8[0] = x2.u8[1];
    y.u8[1] = x2.u8[0];
    return y.u16;
}

#endif /*LV_USE_FT81X*/
