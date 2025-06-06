/*
@file    EVE_commands.h
@brief   contains FT8xx / BT8xx function prototypes
@version 5.0
@date    2023-12-29
@author  Rudolph Riedel

@section LICENSE

MIT License

Copyright (c) 2016-2023 Rudolph Riedel

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
- added prototype for EVE_cmd_plkfreq()
- replaced BT81X_ENABLE with "EVE_GEN > 2"
- removed FT81X_ENABLE as FT81x already is the lowest supported chip revision now
- removed the formerly as deprected marked EVE_get_touch_tag()
- changed EVE_color_rgb() to use a 32 bit value like the rest of the color commands
- removed the meta-commands EVE_cmd_point(), EVE_cmd_line() and EVE_cmd_rect()
- removed obsolete functions EVE_get_cmdoffset(void) and EVE_report_cmdoffset(void) - cmdoffset is gone
- renamed EVE_LIB_GetProps() back to EVE_cmd_getprops() since it does not do anything special to justify a special name
- added prototype for helper function EVE_memWrite_sram_buffer()
- added prototypes for EVE_cmd_bitmap_transform() and EVE_cmd_bitmap_transform_burst()
- added prototype for EVE_cmd_playvideo()
- added prototypes for EVE_cmd_setfont_burst() and EVE_cmd_setfont2_burst()
- added prototype for EVE_cmd_videoframe()
- restructured: functions are sorted by chip-generation and within their group in alphabetical order
- reimplementedEVE_cmd_getmatrix() again, it needs to read values, not write them
- added prototypes for EVE_cmd_fontcache() and EVE_cmd_fontcachequery()
- added prototype for EVE_cmd_flashprogram()
- added prototype for EVE_cmd_calibratesub()
- added prototypes for EVE_cmd_animframeram(), EVE_cmd_animframeram_burst(), EVE_cmd_animstartram(),
EVE_cmd_animstartram_burst()
- added prototypes for EVE_cmd_apilevel(), EVE_cmd_apilevel_burst()
- added prototypes for EVE_cmd_calllist(), EVE_cmd_calllist_burst()
- added prototype for EVE_cmd_getimage()
- added prototypes for EVE_cmd_hsf(), EVE_cmd_hsf_burst()
- added prototype for EVE_cmd_linetime()
- added prototypes for EVE_cmd_newlist(), EVE_cmd_newlist_burst()
- added prototypes for EVE_cmd_runanim(), EVE_cmd_runanim_burst()
- added prototype for EVE_cmd_wait()
- removed the history from before 4.0
- added an enum with return codes to have the functions return something more meaningfull
- finally removed EVE_cmd_start() after setting it to deprecatd with the first 5.0 release
- renamed EVE_cmd_execute() to EVE_execute_cmd() to be more consistent, this is is not an EVE command
- added the return-value of EVE_FIFO_HALF_EMPTY to EVE_busy() to indicate there is more than 2048 bytes available
- removed the 4.0 history
- added parameter width to EVE_calibrate_manual()
- changed the varargs versions of cmd_button, cmd_text and cmd_toggle to use an array of uint32_t values to comply with MISRA-C
- fixed some MISRA-C issues
- basic maintenance: checked for violations of white space and indent rules
- more linter fixes for minor issues like variables shorter than 3 characters
- added EVE_color_a() / EVE_color_a_burst()
- removed EVE_cmd_newlist_burst() prototype as the function got removed earlier
- added prototype for EVE_write_display_parameters()
- added EVE_memRead_sram_buffer()
- added EVE_FAULT_RECOVERED to the list of return codes
- added defines for the state of the external flash
- added protype for EVE_get_and_reset_fault_state()
- put E_OK and E_NOT_OK in #ifndef/#endif guards as these are usually defined
  already in AUTOSAR projects
- renamed EVE_FAIL_CHIPID_TIMEOUT to EVE_FAIL_REGID_TIMEOUT as suggested by #93 on github
- changed a number of function parameters from signed to unsigned following the
    updated BT81x series programming guide V2.4
- commented out EVE_cmd_regread() prototype
- removed prototype for EVE_cmd_hsf_burst()

*/

#ifndef EVE_COMMANDS_H
#define EVE_COMMANDS_H

#include "EVE.h"

#if !defined E_OK
#define E_OK 0U
#endif

#if !defined E_NOT_OK
#define E_NOT_OK 1U
#endif

#define EVE_FAIL_REGID_TIMEOUT 2U
#define EVE_FAIL_RESET_TIMEOUT 3U
#define EVE_FAIL_PCLK_FREQ 4U
#define EVE_FAIL_FLASH_STATUS_INIT 5U
#define EVE_FAIL_FLASH_STATUS_DETACHED 6U
#define EVE_FAIL_FLASHFAST_NOT_SUPPORTED 7U
#define EVE_FAIL_FLASHFAST_NO_HEADER_DETECTED 8U
#define EVE_FAIL_FLASHFAST_SECTOR0_FAILED 9U
#define EVE_FAIL_FLASHFAST_BLOB_MISMATCH 10U
#define EVE_FAIL_FLASHFAST_SPEED_TEST 11U
#define EVE_IS_BUSY 12U
#define EVE_FIFO_HALF_EMPTY 13U
#define EVE_FAULT_RECOVERED 14U

#define EVE_FLASH_STATUS_INIT 0U
#define EVE_FLASH_STATUS_DETACHED 1U
#define EVE_FLASH_STATUS_BASIC 2U
#define EVE_FLASH_STATUS_FULL 3U

/* ##################################################################
    helper functions
##################################################################### */

void EVE_cmdWrite(uint8_t const command, uint8_t const parameter);

uint8_t EVE_memRead8(uint32_t const ft_address);
uint16_t EVE_memRead16(uint32_t const ft_address);
uint32_t EVE_memRead32(uint32_t const ft_address);
void EVE_memWrite8(uint32_t const ft_address, uint8_t const ft_data);
void EVE_memWrite16(uint32_t const ft_address, uint16_t const ft_data);
void EVE_memWrite32(uint32_t const ft_address, uint32_t const ft_data);
void EVE_memWrite_flash_buffer(uint32_t const ft_address, const uint8_t *p_data, uint32_t const len);
void EVE_memWrite_sram_buffer(uint32_t const ft_address, const uint8_t *p_data, uint32_t const len);
void EVE_memRead_sram_buffer(uint32_t const ft_address, uint8_t *p_data, uint32_t const len);
uint8_t EVE_busy(void);
uint8_t EVE_get_and_reset_fault_state(void);
void EVE_execute_cmd(void);

/* ##################################################################
    commands and functions to be used outside of display-lists
##################################################################### */

/* EVE4: BT817 / BT818 */
#if EVE_GEN > 3

void EVE_cmd_flashprogram(uint32_t dest, uint32_t src, uint32_t num);
void EVE_cmd_fontcache(uint32_t font, uint32_t ptr, uint32_t num);
void EVE_cmd_fontcachequery(uint32_t *p_total, uint32_t *p_used);
void EVE_cmd_getimage(uint32_t *p_source, uint32_t *p_fmt, uint32_t *p_width, uint32_t *p_height, uint32_t *p_palette);
void EVE_cmd_linetime(uint32_t dest);
void EVE_cmd_newlist(uint32_t adr);
uint32_t EVE_cmd_pclkfreq(uint32_t ftarget, int32_t rounding);
void EVE_cmd_wait(uint32_t usec);

#endif /* EVE_GEN > 3 */

/* EVE3: BT815 / BT816 */
#if EVE_GEN > 2

void EVE_cmd_clearcache(void);
void EVE_cmd_flashattach(void);
void EVE_cmd_flashdetach(void);
void EVE_cmd_flasherase(void);
uint32_t EVE_cmd_flashfast(void);
void EVE_cmd_flashspidesel(void);
void EVE_cmd_flashread(uint32_t dest, uint32_t src, uint32_t num);
void EVE_cmd_flashsource(uint32_t ptr);
void EVE_cmd_flashspirx(uint32_t dest, uint32_t num);
void EVE_cmd_flashspitx(uint32_t num, const uint8_t *p_data);
void EVE_cmd_flashupdate(uint32_t dest, uint32_t src, uint32_t num);
void EVE_cmd_flashwrite(uint32_t ptr, uint32_t num, const uint8_t *p_data);
void EVE_cmd_inflate2(uint32_t ptr, uint32_t options, const uint8_t *p_data, uint32_t len);

#endif /* EVE_GEN > 2 */

void EVE_cmd_getprops(uint32_t *p_pointer, uint32_t *p_width, uint32_t *p_height);
uint32_t EVE_cmd_getptr(void);
void EVE_cmd_inflate(uint32_t ptr, const uint8_t *p_data, uint32_t len);
void EVE_cmd_interrupt(uint32_t msec);
void EVE_cmd_loadimage(uint32_t ptr, uint32_t options, const uint8_t *p_data, uint32_t len);
void EVE_cmd_mediafifo(uint32_t ptr, uint32_t size);
void EVE_cmd_memcpy(uint32_t dest, uint32_t src, uint32_t num);
uint32_t EVE_cmd_memcrc(uint32_t ptr, uint32_t num);
void EVE_cmd_memset(uint32_t ptr, uint8_t value, uint32_t num);
void EVE_cmd_memzero(uint32_t ptr, uint32_t num);
void EVE_cmd_playvideo(uint32_t options, const uint8_t *p_data, uint32_t len);
void EVE_cmd_setrotate(uint32_t rotation);
void EVE_cmd_snapshot(uint32_t ptr);
void EVE_cmd_snapshot2(uint32_t fmt, uint32_t ptr, int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt);
void EVE_cmd_track(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t tag);
void EVE_cmd_videoframe(uint32_t dest, uint32_t result_ptr);
/*void EVE_cmd_memwrite(uint32_t dest, uint32_t num, const uint8_t *p_data);*/
/*uint32_t EVE_cmd_regread(uint32_t ptr);*/

/* ##################################################################
    patching and initialization
##################################################################### */

#if EVE_GEN > 2
uint8_t EVE_init_flash(void);
#endif /* EVE_GEN > 2 */

void EVE_write_display_parameters(void);
uint8_t EVE_init(void);

/* ##################################################################
    functions for display lists
##################################################################### */

void EVE_start_cmd_burst(void);
void EVE_end_cmd_burst(void);

/* EVE4: BT817 / BT818 */
#if EVE_GEN > 3

void EVE_cmd_animframeram(int16_t xc0, int16_t yc0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_animframeram_burst(int16_t xc0, int16_t yc0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_animstartram(int32_t chnl, uint32_t aoptr, uint32_t loop);
void EVE_cmd_animstartram_burst(int32_t chnl, uint32_t aoptr, uint32_t loop);
void EVE_cmd_apilevel(uint32_t level);
void EVE_cmd_apilevel_burst(uint32_t level);
void EVE_cmd_calibratesub(uint16_t xc0, uint16_t yc0, uint16_t width, uint16_t height);
void EVE_cmd_calllist(uint32_t adr);
void EVE_cmd_calllist_burst(uint32_t adr);
void EVE_cmd_hsf(uint32_t hsf);
void EVE_cmd_runanim(uint32_t waitmask, uint32_t play);
void EVE_cmd_runanim_burst(uint32_t waitmask, uint32_t play);

#endif /* EVE_GEN > 3 */

/* EVE3: BT815 / BT816 */
#if EVE_GEN > 2

void EVE_cmd_animdraw(int32_t chnl);
void EVE_cmd_animdraw_burst(int32_t chnl);
void EVE_cmd_animframe(int16_t xc0, int16_t yc0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_animframe_burst(int16_t xc0, int16_t yc0, uint32_t aoptr, uint32_t frame);
void EVE_cmd_animstart(int32_t chnl, uint32_t aoptr, uint32_t loop);
void EVE_cmd_animstart_burst(int32_t chnl, uint32_t aoptr, uint32_t loop);
void EVE_cmd_animstop(int32_t chnl);
void EVE_cmd_animstop_burst(int32_t chnl);
void EVE_cmd_animxy(int32_t chnl, int16_t xc0, int16_t yc0);
void EVE_cmd_animxy_burst(int32_t chnl, int16_t xc0, int16_t yc0);
void EVE_cmd_appendf(uint32_t ptr, uint32_t num);
void EVE_cmd_appendf_burst(uint32_t ptr, uint32_t num);
uint16_t EVE_cmd_bitmap_transform(int32_t xc0, int32_t yc0, int32_t xc1, int32_t yc1, int32_t xc2, int32_t yc2, int32_t tx0,
                                  int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2);
void EVE_cmd_bitmap_transform_burst(int32_t xc0, int32_t yc0, int32_t xc1, int32_t yc1, int32_t xc2, int32_t yc2, int32_t tx0,
                                    int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2);
void EVE_cmd_fillwidth(uint32_t pixel);
void EVE_cmd_fillwidth_burst(uint32_t pixel);
void EVE_cmd_gradienta(int16_t xc0, int16_t yc0, uint32_t argb0, int16_t xc1, int16_t yc1, uint32_t argb1);
void EVE_cmd_gradienta_burst(int16_t xc0, int16_t yc0, uint32_t argb0, int16_t xc1, int16_t yc1, uint32_t argb1);
void EVE_cmd_rotatearound(int32_t xc0, int32_t yc0, uint32_t angle, int32_t scale);
void EVE_cmd_rotatearound_burst(int32_t xc0, int32_t yc0, uint32_t angle, int32_t scale);

void EVE_cmd_button_var(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);
void EVE_cmd_button_var_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);
void EVE_cmd_text_var(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);
void EVE_cmd_text_var_burst(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);
void EVE_cmd_toggle_var(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font, uint16_t options, uint16_t state, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);
void EVE_cmd_toggle_var_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font, uint16_t options, uint16_t state, const char *p_text, uint8_t num_args, const uint32_t p_arguments[]);

#endif /* EVE_GEN > 2 */

void EVE_cmd_dl(uint32_t command);
void EVE_cmd_dl_burst(uint32_t command);

void EVE_cmd_append(uint32_t ptr, uint32_t num);
void EVE_cmd_append_burst(uint32_t ptr, uint32_t num);
void EVE_cmd_bgcolor(uint32_t color);
void EVE_cmd_bgcolor_burst(uint32_t color);
void EVE_cmd_button(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_button_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_calibrate(void);
void EVE_cmd_clock(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t hours, uint16_t mins, uint16_t secs, uint16_t msecs);
void EVE_cmd_clock_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t hours, uint16_t mins, uint16_t secs, uint16_t msecs);
void EVE_cmd_dial(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t val);
void EVE_cmd_dial_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t val);
void EVE_cmd_fgcolor(uint32_t color);
void EVE_cmd_fgcolor_burst(uint32_t color);
void EVE_cmd_gauge(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void EVE_cmd_gauge_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void EVE_cmd_getmatrix(int32_t *p_a, int32_t *p_b, int32_t *p_c, int32_t *p_d, int32_t *p_e, int32_t *p_f);
void EVE_cmd_gradcolor(uint32_t color);
void EVE_cmd_gradcolor_burst(uint32_t color);
void EVE_cmd_gradient(int16_t xc0, int16_t yc0, uint32_t rgb0, int16_t xc1, int16_t yc1, uint32_t rgb1);
void EVE_cmd_gradient_burst(int16_t xc0, int16_t yc0, uint32_t rgb0, int16_t xc1, int16_t yc1, uint32_t rgb1);
void EVE_cmd_keys(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_keys_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_number(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, int32_t number);
void EVE_cmd_number_burst(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, int32_t number);
void EVE_cmd_progress(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_progress_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_romfont(uint32_t font, uint32_t romslot);
void EVE_cmd_romfont_burst(uint32_t font, uint32_t romslot);
void EVE_cmd_rotate(uint32_t angle);
void EVE_cmd_rotate_burst(uint32_t angle);
void EVE_cmd_scale(int32_t scx, int32_t scy);
void EVE_cmd_scale_burst(int32_t scx, int32_t scy);
void EVE_cmd_scrollbar(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t size, uint16_t range);
void EVE_cmd_scrollbar_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t size, uint16_t range);
void EVE_cmd_setbase(uint32_t base);
void EVE_cmd_setbase_burst(uint32_t base);
void EVE_cmd_setbitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height);
void EVE_cmd_setbitmap_burst(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height);
void EVE_cmd_setfont(uint32_t font, uint32_t ptr);
void EVE_cmd_setfont_burst(uint32_t font, uint32_t ptr);
void EVE_cmd_setfont2(uint32_t font, uint32_t ptr, uint32_t firstchar);
void EVE_cmd_setfont2_burst(uint32_t font, uint32_t ptr, uint32_t firstchar);
void EVE_cmd_setscratch(uint32_t handle);
void EVE_cmd_setscratch_burst(uint32_t handle);
void EVE_cmd_sketch(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint32_t ptr, uint16_t format);
void EVE_cmd_sketch_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint32_t ptr, uint16_t format);
void EVE_cmd_slider(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_slider_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val, uint16_t range);
void EVE_cmd_spinner(int16_t xc0, int16_t yc0, uint16_t style, uint16_t scale);
void EVE_cmd_spinner_burst(int16_t xc0, int16_t yc0, uint16_t style, uint16_t scale);
void EVE_cmd_text(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_text_burst(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, const char *p_text);
void EVE_cmd_toggle(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font, uint16_t options, uint16_t state, const char *p_text);
void EVE_cmd_toggle_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font, uint16_t options, uint16_t state, const char *p_text);
void EVE_cmd_translate(int32_t tr_x, int32_t tr_y);
void EVE_cmd_translate_burst(int32_t tr_x, int32_t tr_y);

void EVE_color_rgb(uint32_t color);
void EVE_color_rgb_burst(uint32_t color);
void EVE_color_a(uint8_t alpha);
void EVE_color_a_burst(uint8_t alpha);

/* ##################################################################
    special purpose functions
##################################################################### */

void EVE_calibrate_manual(uint16_t width, uint16_t height);

#endif /* EVE_COMMANDS_H */
