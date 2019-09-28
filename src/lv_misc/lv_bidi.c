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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_bidi_dir_t get_next_run(const char * txt, lv_bidi_dir_t base_dir, uint32_t * len);
static void rtl_reverse(char * dest, const char * src, uint32_t len);

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
    printf("Input str: \"%s\"\n", str_in);

    uint32_t run_len = 0;
    lv_bidi_dir_t run_dir;
    uint32_t rd = 0;

    lv_bidi_dir_t dir = base_dir;

    /*Process neutral chars in the beginning*/
    while(str_in[rd] != '\0') {
        uint32_t letter = lv_txt_encoded_next(str_in, &rd);
        dir = lv_bidi_get_letter_dir(letter);
        if(dir != LV_BIDI_DIR_NEUTRAL) break;
    }

    /*if there were neutrals in the beginning apply `base_dir` on them */
    if(rd && str_in[rd] != '\0') lv_txt_encoded_prev(str_in, &rd);

    if(rd) {
        memcpy(str_out, str_in, rd);
        str_out[rd] = '\0';
        printf("%s: \"%s\"\n", base_dir == LV_BIDI_DIR_LTR ? "LTR" : "RTL", str_out);
    }

    /*Get and process the runs*/
    while(str_in[rd] != '\0') {
        run_dir = get_next_run(&str_in[rd], base_dir, &run_len);

        memcpy(str_out, &str_in[rd], run_len);
        str_out[run_len] = '\0';
        if(run_dir == LV_BIDI_DIR_LTR) {
            printf("%s: \"%s\"\n", "LTR" , str_out);
        } else {
            printf("%s: \"%s\" -> ", "RTL" , str_out);

            rtl_reverse(str_out, &str_in[rd], run_len);
            printf("\"%s\"\n", str_out);

        }

        rd += run_len;
    }
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
        if(txt[i] == '\0') {
            *len = i;
            return base_dir;
        }
    }

    lv_bidi_dir_t run_dir = dir;

    uint32_t i_prev = i;
    uint32_t i_last_strong = i;

    /*Find the next char which has different direction*/
    lv_bidi_dir_t next_dir = base_dir;
    while(txt[i] != '\0') {
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

        /*Keep weak letters as LTR*/
        if(lv_bidi_letter_is_weak(letter)) {
            uint32_t last_weak = i;
            uint32_t first_weak = i;
            while(i) {
                letter = lv_txt_encoded_prev(src, &i);
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
            memcpy(&dest[wr], &src[i], letter_size);
            wr += letter_size;
        }
    }
}

