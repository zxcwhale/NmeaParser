/*
 * =====================================================================================
 *
 *       Filename:  nmeatknzr.c
 *
 *    Description:  nmea tokenizer source file
 *
 *        Version:  1.0
 *        Created:  2018/8/15 17:14:56
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include "nmeatknzr.h"

/*
 * Get token from nmea_tokenizer
 * @t:          the nmea_tokenizer
 * @index:      index of token
 * @return:     the token at @index. If @index outof range, return a dummy one.
 */
struct token nmea_tokenizer_get(struct nmea_tokenizer *t, int index) 
{
        static char dummy[] = "";
        struct token tok;
        if (index < 0 || index >= t->count) {
                tok.p = tok.end = dummy;
        }
        else {
                tok = t->tokens[index];
        }
        return tok;
}

/*
 * Init nmea_tokenizer from ptr of NMEA sentence
 * @r:          the nmea_tokenizer
 * @p:          ptr to the nmea sentence start
 * @end:        ptr to the nmea sentence end
 * @return:     the token's count
 */
int nmea_tokenizer_init(struct nmea_tokenizer *t, char *p, char *end) 
{
        int count = 0;
        char *q;

        if (p < end && p[0] == '$') {           // skip leading '$'
                p += 1;
        }

        if (end > p && end[-1] == '\n') {       // skip tailing '\n'
                end -= 1;
        }

        if (end > p && end[-1] == '\r') {       // skip tailing '\r'
                end -= 1;
        }

        if (end >= p + 3 && end[-3] == '*') {   // skip to '*'
                end -= 3;
        }

        while (p < end) {                       // split buffer by ','
                q = (char *)memchr(p, ',', end - p);
                if (q == NULL) {
                        q = end;
                }
                if (q >= p) {
                        if (count < MAX_NMEA_TOKENS) {
                                t->tokens[count].p = p;
                                t->tokens[count].end = q;
                                count += 1;
                        }
                }

                if (q < end) {
                        q += 1;
                }
                p = q;
        }
        t->count = count;
        return count;
}

