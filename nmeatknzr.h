/*
 * =====================================================================================
 *
 *       Filename:  nmeatknzr.h
 *
 *    Description:  Nmea Tokenizer head file
 *
 *        Version:  1.0
 *        Created:  2018/8/15 17:12:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jarod Lee 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __NMEA_TKNZR_H
#define __NMEA_TKNZR_H

/*
 * NMEA token is the bytes splitted by ','
 * For NMEA sentence:
 * $GNGGA       ,061407.000     ,3011.1363      ,N      ,12009.2925     ,E      ,1,05,5.5,17.3,M,0.0,M,,*44
 * --------------------------------------------------------------------------------------------
 * |token0      |token1         |token2         |token3 |token4         |token5 |......
 * |p        end|p           end|p           end|p   end|p           end|p   end|......
 * --------------------------------------------------------------------------------------------
 * @p is the ptr to the first byte.
 * @end is the prt to the last byte.
 * For "GNGGA,"(token0), @p points to first 'G' and @end points to ','
 */
struct token {
        char *p;
        char *end;
};

#define MAX_NMEA_TOKENS 64
/*
 * NMEA token table
 * One NMEA tokenizer presents one NMEA sentence 
 * @count:      the token's count
 * @tokens:     array of token
 */
struct nmea_tokenizer {
        int count;
        struct token tokens[MAX_NMEA_TOKENS];
};

struct token nmea_tokenizer_get(struct nmea_tokenizer *t, int index);   // get the token at @index from NMEA tokenizer @t
int nmea_tokenizer_init(struct nmea_tokenizer *t, char *p, char *end);  // init NMEA tokenizer from buffer between @p and @end 


#endif

