/**
 * @file lv_bidi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_bidi.h"
#include <stddef.h>
#include "lv_txt.h"

#if LV_USE_BIDI

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void process_paragraph(const char * str_in, char * str_out, uint32_t len, lv_bidi_dir_t base_dir);
static uint32_t get_next_paragraph(const char * txt);
static lv_bidi_dir_t get_next_run(const char * txt, lv_bidi_dir_t base_dir, uint32_t * len);
static void rtl_reverse(char * dest, const char * src, uint32_t len);
static uint32_t char_change_to_pair(uint32_t letter);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_bidi_process(const char * str_in, char * str_out, lv_bidi_dir_t base_dir)
{
    printf("\nInput str: \"%s\"\n", str_in);


    uint32_t par_start = 0;
    uint32_t par_len;

    while(str_in[par_start] == '\n' || str_in[par_start] == '\r') {
        str_out[par_start] = str_in[par_start];
        par_start ++;
    }

    while(str_in[par_start] != '\0') {
        par_len = get_next_paragraph(&str_in[par_start]);
        process_paragraph(&str_in[par_start], &str_out[par_start], par_len, base_dir);
        par_start += par_len;

        while(str_in[par_start] == '\n' || str_in[par_start] == '\r') {
            str_out[par_start] = str_in[par_start];
            par_start ++;
        }
    }

    str_out[par_start] = '\0';

    printf("\nOutput str: \"%s\"\n", str_out);



}

lv_bidi_dir_t lv_bidi_detect_base_dir(const char * txt)
{
    uint32_t i = 0;
    uint32_t letter;
    while(txt[i] != '\0') {
        letter = lv_txt_encoded_next(txt, &i);

        lv_bidi_dir_t dir;
        dir = lv_bidi_get_letter_dir(letter);
        if(dir == LV_BIDI_DIR_RTL || dir == LV_BIDI_DIR_LTR) return dir;
    }

    /*If there were no strong char earlier return with the default base dir */
    return LV_BIDI_BASE_DIR_DEF;
}

lv_bidi_dir_t lv_bidi_get_letter_dir(uint32_t letter)
{
    if(lv_bidi_letter_is_rtl(letter)) return LV_BIDI_DIR_RTL;
    if(lv_bidi_letter_is_neutral(letter)) return LV_BIDI_DIR_NEUTRAL;
    if(lv_bidi_letter_is_weak(letter)) return LV_BIDI_DIR_WEAK;

    return LV_BIDI_DIR_LTR;
}

bool lv_bidi_letter_is_weak(uint32_t letter)
{
    uint32_t i = 0;
    static const char weaks[] = "0123456789";

    do {
        uint32_t x = lv_txt_encoded_next(weaks, &i);
        if(letter == x) {
            return true;
        }
    } while(weaks[i] != '\0');

    return false;
}

bool lv_bidi_letter_is_rtl(uint32_t letter)
{
//     if(letter >= 0x7f && letter <= 0x2000) return true;
     if(letter >= 0x5d0 && letter <= 0x5ea) return true;
//    if(letter >= 'a' && letter <= 'z') return true;

    return false;
}

bool lv_bidi_letter_is_neutral(uint32_t letter)
{
    uint16_t i;
    static const char neutrals[] = " \t\n\r.,:;'\"`!?%/\\=()[]{}<>@#&$|";
    for(i = 0; neutrals[i] != '\0'; i++) {
        if(letter == (uint32_t)neutrals[i]) return true;
    }

    return false;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void process_paragraph(const char * str_in, char * str_out, uint32_t len, lv_bidi_dir_t base_dir)
{
    printf("new paragraph\n");

    char print_buf[256];

    uint32_t run_len = 0;
    lv_bidi_dir_t run_dir;
    uint32_t rd = 0;
    uint32_t wr;
    if(base_dir == LV_BIDI_DIR_RTL) wr = len;
    else wr = 0;

    str_out[len] = '\0';

    lv_bidi_dir_t dir = base_dir;

    /*Process neutral chars in the beginning*/
    while(rd < len) {
        uint32_t letter = lv_txt_encoded_next(str_in, &rd);
        dir = lv_bidi_get_letter_dir(letter);
        if(dir != LV_BIDI_DIR_NEUTRAL) break;
    }

    if(rd && str_in[rd] != '\0') lv_txt_encoded_prev(str_in, &rd);

    if(rd) {
        if(base_dir == LV_BIDI_DIR_LTR) {
            memcpy(&str_out[wr], str_in, rd);
            wr += rd;
        } else {
            wr -= rd;
            memcpy(&str_out[wr], str_in, rd);
        }
        memcpy(print_buf, str_in, rd);
        print_buf[rd] = '\0';
        printf("%s: \"%s\"\n", base_dir == LV_BIDI_DIR_LTR ? "LTR" : "RTL", print_buf);
    }

    /*Get and process the runs*/
    while(rd < len) {
        run_dir = get_next_run(&str_in[rd], base_dir, &run_len);

        memcpy(print_buf, &str_in[rd], run_len);
        print_buf[run_len] = '\0';
        if(run_dir == LV_BIDI_DIR_LTR) {
            printf("%s: \"%s\"\n", "LTR" , print_buf);
        } else {
            printf("%s: \"%s\" -> ", "RTL" , print_buf);

            rtl_reverse(print_buf, &str_in[rd], run_len);
            printf("\"%s\"\n", print_buf);
        }

        if(base_dir == LV_BIDI_DIR_LTR) {
            if(run_dir == LV_BIDI_DIR_LTR)  memcpy(&str_out[wr], &str_in[rd], run_len);
            else rtl_reverse(&str_out[wr], &str_in[rd], run_len);
           wr += run_len;
       } else {
           wr -= run_len;
           if(run_dir == LV_BIDI_DIR_LTR)  memcpy(&str_out[wr], &str_in[rd], run_len);
           else rtl_reverse(&str_out[wr], &str_in[rd], run_len);
       }

        rd += run_len;
    }

    printf("result: %s\n", str_out);

}

static uint32_t get_next_paragraph(const char * txt)
{
    uint32_t i = 0;

    lv_txt_encoded_next(txt, &i);

    while(txt[i] != '\0' && txt[i] != '\n' && txt[i] != '\r') {
        lv_txt_encoded_next(txt, &i);
    }

    return i;
}

static lv_bidi_dir_t get_next_run(const char * txt, lv_bidi_dir_t base_dir, uint32_t * len)
{
    uint32_t i = 0;
    uint32_t letter;

    letter = lv_txt_encoded_next(txt, NULL);
    lv_bidi_dir_t dir = lv_bidi_get_letter_dir(letter);

    /*Find the first strong char. Skip the neutrals.*/
    while(dir == LV_BIDI_DIR_NEUTRAL || dir == LV_BIDI_DIR_WEAK) {
        letter = lv_txt_encoded_next(txt, &i);
        dir = lv_bidi_get_letter_dir(letter);
        if(txt[i] == '\0' || txt[i] == '\n' || txt[i] == '\r') {
            *len = i;
            return base_dir;
        }
    }

    lv_bidi_dir_t run_dir = dir;

    uint32_t i_prev = i;
    uint32_t i_last_strong = i;

    /*Find the next char which has different direction*/
    lv_bidi_dir_t next_dir = base_dir;
    while(txt[i] != '\0'&& txt[i] != '\n' && txt[i] != '\r') {
        letter = lv_txt_encoded_next(txt, &i);
        next_dir  = lv_bidi_get_letter_dir(letter);

        /*New dir found?*/
        if((next_dir == LV_BIDI_DIR_RTL || next_dir == LV_BIDI_DIR_LTR) && next_dir != run_dir) {
            /*Include neutrals if `run_dir == base_dir` */
            if(run_dir == base_dir) *len = i_prev;
            /*Exclude neutrals if `run_dir != base_dir` */
            else *len = i_last_strong;

            return run_dir;
        }

        if(next_dir != LV_BIDI_DIR_NEUTRAL) i_last_strong = i;

        i_prev = i;
    }


    /*Handle end of of string. Apply `base_dir` on trailing neutrals*/

    /*Include neutrals if `run_dir == base_dir` */
    if(run_dir == base_dir) *len = i_prev;
    /*Exclude neutrals if `run_dir != base_dir` */
    else *len = i_last_strong;

    return run_dir;

}

static void rtl_reverse(char * dest, const char * src, uint32_t len)
{
    uint32_t i = len;
    uint32_t wr = 0;

    while(i) {
        uint32_t letter = lv_txt_encoded_prev(src, &i);

        /*Keep weak letters (numbers) as LTR*/
        if(lv_bidi_letter_is_weak(letter)) {
            uint32_t last_weak = i;
            uint32_t first_weak = i;
            while(i) {
                letter = lv_txt_encoded_prev(src, &i);
                /*No need to call `char_change_to_pair` because there not such chars here*/

                /*Finish on non-weak char */
                /*but treat number and currency related chars as weak*/
                if(lv_bidi_letter_is_weak(letter) == false && letter != '.' && letter != ',' && letter != '$') {
                    lv_txt_encoded_next(src, &i);   /*Rewind one letter*/
                    first_weak = i;
                    break;
                }
            }
            if(i == 0) first_weak = 0;

            memcpy(&dest[wr], &src[first_weak], last_weak - first_weak + 1);
            wr += last_weak - first_weak + 1;

        }
        /*Simply store in reversed order*/
        else {
            uint32_t letter_size = lv_txt_encoded_size((const char *)&src[i]);
            /*Swap arithmetical symbols*/
            if(letter_size == 1) {
                uint32_t new_letter = letter = char_change_to_pair(letter);
                dest[wr] = (uint8_t)new_letter;
                wr += 1;
            }
            /*Just store the letter*/
            else {
                memcpy(&dest[wr], &src[i], letter_size);
                wr += letter_size;
            }
        }
    }
}

static uint32_t char_change_to_pair(uint32_t letter)
{
    static uint8_t left[] = {"<({["};
    static uint8_t right[] = {">)}]"};

    uint8_t i;
    for(i = 0; left[i] != '\0'; i++) {
        if(letter == left[i]) return right[i];
    }

    for(i = 0; right[i] != '\0'; i++) {
        if(letter == right[i]) return left[i];
    }

    return letter;
}

#endif /*LV_USE_BIDI*/
