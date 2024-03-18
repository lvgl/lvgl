// Copyright (c) 2013, LKC Technologies, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer. Redistributions in binary
// form must reproduce the above copyright notice, this list of conditions and
// the following disclaimer in the documentation and/or other materials
// provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "../../../lvgl.h"
#if LV_USE_BARCODE

#include "code128.h"
#include <string.h>

#define CODE128_MALLOC      lv_malloc
#define CODE128_REALLOC     lv_realloc
#define CODE128_FREE        lv_free
#define CODE128_MEMSET      lv_memset
#define CODE128_STRLEN      lv_strlen
#define CODE128_ASSERT      LV_ASSERT

#define CODE128_QUIET_ZONE_LEN 10
#define CODE128_CHAR_LEN       11
#define CODE128_STOP_CODE_LEN  13

#define CODE128_START_CODE_A 103
#define CODE128_START_CODE_B 104
#define CODE128_START_CODE_C 105

#define CODE128_MODE_A    'a'
#define CODE128_MODE_B    'b'
#define CODE128_MODE_C    'c'

#define CODE128_MIN_ENCODE_LEN (CODE128_QUIET_ZONE_LEN * 2 + CODE128_CHAR_LEN * 2 + CODE128_STOP_CODE_LEN)

static const int code128_pattern[] = {
    // value: pattern,     bar/space widths
    1740, //   0: 11011001100, 212222
    1644, //   1: 11001101100, 222122
    1638, //   2: 11001100110, 222221
    1176, //   3: 10010011000, 121223
    1164, //   4: 10010001100, 121322
    1100, //   5: 10001001100, 131222
    1224, //   6: 10011001000, 122213
    1220, //   7: 10011000100, 122312
    1124, //   8: 10001100100, 132212
    1608, //   9: 11001001000, 221213
    1604, //  10: 11001000100, 221312
    1572, //  11: 11000100100, 231212
    1436, //  12: 10110011100, 112232
    1244, //  13: 10011011100, 122132
    1230, //  14: 10011001110, 122231
    1484, //  15: 10111001100, 113222
    1260, //  16: 10011101100, 123122
    1254, //  17: 10011100110, 123221
    1650, //  18: 11001110010, 223211
    1628, //  19: 11001011100, 221132
    1614, //  20: 11001001110, 221231
    1764, //  21: 11011100100, 213212
    1652, //  22: 11001110100, 223112
    1902, //  23: 11101101110, 312131
    1868, //  24: 11101001100, 311222
    1836, //  25: 11100101100, 321122
    1830, //  26: 11100100110, 321221
    1892, //  27: 11101100100, 312212
    1844, //  28: 11100110100, 322112
    1842, //  29: 11100110010, 322211
    1752, //  30: 11011011000, 212123
    1734, //  31: 11011000110, 212321
    1590, //  32: 11000110110, 232121
    1304, //  33: 10100011000, 111323
    1112, //  34: 10001011000, 131123
    1094, //  35: 10001000110, 131321
    1416, //  36: 10110001000, 112313
    1128, //  37: 10001101000, 132113
    1122, //  38: 10001100010, 132311
    1672, //  39: 11010001000, 211313
    1576, //  40: 11000101000, 231113
    1570, //  41: 11000100010, 231311
    1464, //  42: 10110111000, 112133
    1422, //  43: 10110001110
    1134, //  44: 10001101110
    1496, //  45: 10111011000, 113123
    1478, //  46: 10111000110, 113321
    1142, //  47: 10001110110, 133121
    1910, //  48: 11101110110, 313121
    1678, //  49: 11010001110, 211331
    1582, //  50: 11000101110, 231131
    1768, //  51: 11011101000, 213113
    1762, //  52: 11011100010, 213311
    1774, //  53: 11011101110, 213131
    1880, //  54: 11101011000, 311123
    1862, //  55: 11101000110, 311321
    1814, //  56: 11100010110, 331121
    1896, //  57: 11101101000, 312113
    1890, //  58: 11101100010, 312311
    1818, //  59: 11100011010, 332111
    1914, //  60: 11101111010, 314111
    1602, //  61: 11001000010, 221411
    1930, //  62: 11110001010, 431111
    1328, //  63: 10100110000, 111224
    1292, //  64: 10100001100, 111422
    1200, //  65: 10010110000, 121124
    1158, //  66: 10010000110, 121421
    1068, //  67: 10000101100, 141122
    1062, //  68: 10000100110, 141221
    1424, //  69: 10110010000, 112214
    1412, //  70: 10110000100, 112412
    1232, //  71: 10011010000, 122114
    1218, //  72: 10011000010, 122411
    1076, //  73: 10000110100, 142112
    1074, //  74: 10000110010, 142211
    1554, //  75: 11000010010, 241211
    1616, //  76: 11001010000, 221114
    1978, //  77: 11110111010, 413111
    1556, //  78: 11000010100, 241112
    1146, //  79: 10001111010, 134111
    1340, //  80: 10100111100, 111242
    1212, //  81: 10010111100, 121142
    1182, //  82: 10010011110, 121241
    1508, //  83: 10111100100, 114212
    1268, //  84: 10011110100, 124112
    1266, //  85: 10011110010, 124211
    1956, //  86: 11110100100, 411212
    1940, //  87: 11110010100, 421112
    1938, //  88: 11110010010, 421211
    1758, //  89: 11011011110, 212141
    1782, //  90: 11011110110, 214121
    1974, //  91: 11110110110, 412121
    1400, //  92: 10101111000, 111143
    1310, //  93: 10100011110, 111341
    1118, //  94: 10001011110, 131141
    1512, //  95: 10111101000, 114113
    1506, //  96: 10111100010, 114311
    1960, //  97: 11110101000, 411113
    1954, //  98: 11110100010, 411311
    1502, //  99: 10111011110, 113141
    1518, // 100: 10111101110, 114131
    1886, // 101: 11101011110, 311141
    1966, // 102: 11110101110, 411131
    1668, // 103: 11010000100, 211412
    1680, // 104: 11010010000, 211214
    1692  // 105: 11010011100, 211232
};

static const int code128_stop_pattern = 6379; // 1100011101011, 2331112

struct code128_step {
    int prev_ix;                // Index of previous step, if any
    const char * next_input;    // Remaining input
    unsigned short len;         // The length of the pattern so far (includes this step)
    char mode;                  // State for the current encoding
    signed char code;           // What code should be written for this step
};

struct code128_state {
    struct code128_step * steps;
    int allocated_steps;
    int current_ix;
    int todo_ix;
    int best_ix;

    size_t maxlength;
};

size_t code128_estimate_len(const char * s)
{
    return CODE128_QUIET_ZONE_LEN
           + CODE128_CHAR_LEN // start code
           + CODE128_CHAR_LEN * (CODE128_STRLEN(s) * 11 / 10) // contents + 10% padding
           + CODE128_CHAR_LEN // checksum
           + CODE128_STOP_CODE_LEN
           + CODE128_QUIET_ZONE_LEN;
}

static void code128_append_pattern(int pattern, int pattern_length, char * out)
{
    // All patterns have their first bit set by design
    CODE128_ASSERT(pattern & (1 << (pattern_length - 1)));

    int i;
    for(i = pattern_length - 1; i >= 0; i--) {
        // cast avoids warning: implicit conversion from 'int' to 'char' changes value from 255 to -1 [-Wconstant-conversion]
        *out++ = (unsigned char)((pattern & (1 << i)) ? 255 : 0);
    }
}

static int code128_append_code(int code, char * out)
{
    CODE128_ASSERT(code >= 0 && code < (int)(sizeof(code128_pattern) / sizeof(code128_pattern[0])));
    code128_append_pattern(code128_pattern[code], CODE128_CHAR_LEN, out);
    return CODE128_CHAR_LEN;
}

static int code128_append_stop_code(char * out)
{
    code128_append_pattern(code128_stop_pattern, CODE128_STOP_CODE_LEN, out);
    return CODE128_STOP_CODE_LEN;
}

static signed char code128_switch_code(char from_mode, char to_mode)
{
    switch(from_mode) {
        case CODE128_MODE_A:
            switch(to_mode) {
                case CODE128_MODE_B:
                    return 100;
                case CODE128_MODE_C:
                    return 99;
            }
            break;

        case CODE128_MODE_B:
            switch(to_mode) {
                case CODE128_MODE_A:
                    return 101;
                case CODE128_MODE_C:
                    return 99;
            }
            break;

        case CODE128_MODE_C:
            switch(to_mode) {
                case CODE128_MODE_B:
                    return 100;
                case CODE128_MODE_A:
                    return 101;
            }
            break;
        default:
            break;
    }

    CODE128_ASSERT(0); // Invalid mode switch
    return -1;
}

static signed char code128a_ascii_to_code(signed char value)
{
    if(value >= ' ' && value <= '_')
        return (signed char)(value - ' ');
    else if(value >= 0 && value < ' ')
        return (signed char)(value + 64);
    else if(value == (signed char)CODE128_FNC1)
        return 102;
    else if(value == (signed char)CODE128_FNC2)
        return 97;
    else if(value == (signed char)CODE128_FNC3)
        return 96;
    else if(value == (signed char)CODE128_FNC4)
        return 101;
    else
        return -1;
}

static signed char code128b_ascii_to_code(signed char value)
{
    if(value >= ' ')  // value <= 127 is implied
        return (signed char)(value - ' ');
    else if(value == (signed char)CODE128_FNC1)
        return 102;
    else if(value == (signed char)CODE128_FNC2)
        return 97;
    else if(value == (signed char)CODE128_FNC3)
        return 96;
    else if(value == (signed char)CODE128_FNC4)
        return 100;
    else
        return -1;
}

static signed char code128c_ascii_to_code(const char * values)
{
    if(values[0] == CODE128_FNC1)
        return 102;

    if(values[0] >= '0' && values[0] <= '9' &&
       values[1] >= '0' && values[1] <= '9') {
        char code = 10 * (values[0] - '0') + (values[1] - '0');
        return code;
    }

    return -1;
}

static int code128_do_a_step(struct code128_step * base, int prev_ix, int ix)
{
    struct code128_step * previous_step = &base[prev_ix];
    struct code128_step * step = &base[ix];

    char value = *previous_step->next_input;
    // NOTE: Currently we can't encode NULL
    if(value == 0)
        return 0;

    step->code = code128a_ascii_to_code(value);
    if(step->code < 0)
        return 0;

    step->prev_ix = prev_ix;
    step->next_input = previous_step->next_input + 1;
    step->mode = CODE128_MODE_A;
    step->len = previous_step->len + CODE128_CHAR_LEN;
    if(step->mode != previous_step->mode)
        step->len += CODE128_CHAR_LEN; // Need to switch modes

    return 1;
}

static int code128_do_b_step(struct code128_step * base, int prev_ix, int ix)
{
    struct code128_step * previous_step = &base[prev_ix];
    struct code128_step * step = &base[ix];

    char value = *previous_step->next_input;
    // NOTE: Currently we can't encode NULL
    if(value == 0)
        return 0;

    step->code = code128b_ascii_to_code(value);
    if(step->code < 0)
        return 0;

    step->prev_ix = prev_ix;
    step->next_input = previous_step->next_input + 1;
    step->mode = CODE128_MODE_B;
    step->len = previous_step->len + CODE128_CHAR_LEN;
    if(step->mode != previous_step->mode)
        step->len += CODE128_CHAR_LEN; // Need to switch modes

    return 1;
}

static int code128_do_c_step(struct code128_step * base, int prev_ix, int ix)
{
    struct code128_step * previous_step = &base[prev_ix];
    struct code128_step * step = &base[ix];

    char value = *previous_step->next_input;
    // NOTE: Currently we can't encode NULL
    if(value == 0)
        return 0;

    step->code = code128c_ascii_to_code(previous_step->next_input);
    if(step->code < 0)
        return 0;

    step->prev_ix = prev_ix;
    step->next_input = previous_step->next_input + 1;

    // Mode C consumes 2 characters for codes 0-99
    if(step->code < 100)
        step->next_input++;

    step->mode = CODE128_MODE_C;
    step->len = previous_step->len + CODE128_CHAR_LEN;
    if(step->mode != previous_step->mode)
        step->len += CODE128_CHAR_LEN; // Need to switch modes

    return 1;
}

static struct code128_step * code128_alloc_step(struct code128_state * state)
{
    if(state->todo_ix >= state->allocated_steps) {
        state->allocated_steps += 1024;
        state->steps = (struct code128_step *) CODE128_REALLOC(state->steps,
                                                               state->allocated_steps * sizeof(struct code128_step));
    }

    struct code128_step * step = &state->steps[state->todo_ix];

    CODE128_MEMSET(step, 0, sizeof(*step));
    return step;
}

static void code128_do_step(struct code128_state * state)
{
    struct code128_step * step = &state->steps[state->current_ix];
    if(*step->next_input == 0) {
        // Done, so see if we have a new shortest encoding.
        if((step->len < state->maxlength) ||
           (state->best_ix < 0 && step->len == state->maxlength)) {
            state->best_ix = state->current_ix;

            // Update maxlength to avoid considering anything longer
            state->maxlength = step->len;
        }
        return;
    }

    // Don't try if we're already at or beyond the max acceptable
    // length;
    if(step->len >= state->maxlength)
        return;
    char mode = step->mode;

    code128_alloc_step(state);
    int mode_c_worked = 0;

    // Always try mode C
    if(code128_do_c_step(state->steps, state->current_ix, state->todo_ix)) {
        state->todo_ix++;
        code128_alloc_step(state);
        mode_c_worked = 1;
    }

    if(mode == CODE128_MODE_A) {
        // If A works, stick with A. There's no advantage to switching
        // to B proactively if A still works.
        if(code128_do_a_step(state->steps, state->current_ix, state->todo_ix) ||
           code128_do_b_step(state->steps, state->current_ix, state->todo_ix))
            state->todo_ix++;
    }
    else if(mode == CODE128_MODE_B) {
        // The same logic applies here. There's no advantage to switching
        // proactively to A if B still works.
        if(code128_do_b_step(state->steps, state->current_ix, state->todo_ix) ||
           code128_do_a_step(state->steps, state->current_ix, state->todo_ix))
            state->todo_ix++;
    }
    else if(!mode_c_worked) {
        // In mode C. If mode C worked and we're in mode C, trying anything
        // else is pointless since the mode C encoding will be shorter and
        // there won't be any mode switches.

        // If we're leaving mode C, though, try both in case one ends up
        // better than the other.
        if(code128_do_a_step(state->steps, state->current_ix, state->todo_ix)) {
            state->todo_ix++;
            code128_alloc_step(state);
        }
        if(code128_do_b_step(state->steps, state->current_ix, state->todo_ix))
            state->todo_ix++;
    }
}

size_t code128_encode_raw(const char * s, char * out, size_t maxlength)
{
    struct code128_state state;

    const size_t overhead = CODE128_QUIET_ZONE_LEN
                            + CODE128_CHAR_LEN // checksum
                            + CODE128_STOP_CODE_LEN
                            + CODE128_QUIET_ZONE_LEN;
    if(maxlength < overhead + CODE128_CHAR_LEN + CODE128_CHAR_LEN) {
        // Need space to encode the start character and one additional
        // character.
        return 0;
    }

    state.allocated_steps = 256;
    state.steps = (struct code128_step *) CODE128_MALLOC(state.allocated_steps * sizeof(struct code128_step));
    state.current_ix = 0;
    state.todo_ix = 0;
    state.maxlength = maxlength - overhead;
    state.best_ix = -1;

    // Initialize the first 3 steps for the 3 encoding routes (A, B, C)
    state.steps[0].prev_ix = -1;
    state.steps[0].next_input = s;
    state.steps[0].len = CODE128_CHAR_LEN;
    state.steps[0].mode = CODE128_MODE_C;
    state.steps[0].code = CODE128_START_CODE_C;

    state.steps[1].prev_ix = -1;
    state.steps[1].next_input = s;
    state.steps[1].len = CODE128_CHAR_LEN;
    state.steps[1].mode = CODE128_MODE_A;
    state.steps[1].code = CODE128_START_CODE_A;

    state.steps[2].prev_ix = -1;
    state.steps[2].next_input = s;
    state.steps[2].len = CODE128_CHAR_LEN;
    state.steps[2].mode = CODE128_MODE_B;
    state.steps[2].code = CODE128_START_CODE_B;

    state.todo_ix = 3;

    // Keep going until no more work
    do {
        code128_do_step(&state);
        state.current_ix++;
    } while(state.current_ix != state.todo_ix);

    // If no best_step, then fail.
    if(state.best_ix < 0) {
        CODE128_FREE(state.steps);
        return 0;
    }

    // Determine the list of codes
    size_t num_codes = state.maxlength / CODE128_CHAR_LEN;
    char * codes = CODE128_MALLOC(num_codes);
    CODE128_ASSERT(codes);

    struct code128_step * step = &state.steps[state.best_ix];
    size_t i;
    for(i = num_codes - 1; i > 0; --i) {
        struct code128_step * prev_step = &state.steps[step->prev_ix];
        codes[i] = step->code;
        if(step->mode != prev_step->mode) {
            --i;
            codes[i] = code128_switch_code(prev_step->mode, step->mode);
        }
        step = prev_step;
    }
    codes[0] = step->code;

    // Encode everything up to the checksum
    size_t actual_length = state.maxlength + overhead;
    CODE128_MEMSET(out, 0, CODE128_QUIET_ZONE_LEN);
    out += CODE128_QUIET_ZONE_LEN;
    for(i = 0; i < num_codes; i++)
        out += code128_append_code(codes[i], out);

    // Compute the checksum
    int sum = codes[0];
    for(i = 1; i < num_codes; i++)
        sum += (int)(codes[i] * i);
    out += code128_append_code(sum % 103, out);

    // Finalize the code.
    out += code128_append_stop_code(out);
    CODE128_MEMSET(out, 0, CODE128_QUIET_ZONE_LEN);

    CODE128_FREE(codes);
    CODE128_FREE(state.steps);
    return actual_length;
}

/**
 * @brief Encode the GS1 string
 *
 * This converts [FNC1] sequences to raw FNC1 characters and
 * removes spaces before encoding the barcodes.
 *
 * @return the length of barcode data in bytes
 */
size_t code128_encode_gs1(const char * s, char * out, size_t maxlength)
{
    size_t raw_size = CODE128_STRLEN(s) + 1;
    char * raw = CODE128_MALLOC(raw_size);
    CODE128_ASSERT(raw);
    if(!raw) {
        return 0;
    }

    char * p = raw;
    for(; *s != '\0'; s++) {
        if(strncmp(s, "[FNC1]", 6) == 0) {
            *p++ = CODE128_FNC1;
            s += 5;
        }
        else if(*s != ' ') {
            *p++ = *s;
        }
    }
    *p = '\0';

    size_t length = code128_encode_raw(raw, out, maxlength);

    CODE128_FREE(raw);

    return length;
}

#endif /*LV_USE_BARCODE*/
