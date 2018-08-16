/*
 * =====================================================================================
 *
 *       Filename:  nmeardr.c
 *
 *    Description:  Nmea reader source file
 *
 *        Version:  1.0
 *        Created:  2018/8/15 16:58:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jarod Lee 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "nmeardr.h"

/*
 * Init nmea reader to zero
 * @r:          the nmea reader
 */
void nmea_reader_init(struct nmea_reader *r)
{
        memset(r, 0, sizeof(struct nmea_reader));
}

/*
 * Add 1 byte to nmea reader
 * @r:          the nmea reader
 * @c:          the byte to be added
 */
void nmea_reader_add(struct nmea_reader *r, char c)
{
        if (c == '$') {         // reset position if encounter '$'
                r->pos = 0;
        }
        r->buf[r->pos] = c; 
        r->pos = (r->pos + 1) % MAX_NMEARDR_SIZE;
}

/*
 * Check if nmea reader's checksum if okay
 * @r:          the nmea reader
 * @return:     if checksum is correct return true, else return false
 */
bool nmea_reader_check(struct nmea_reader *r)
{
        unsigned char chk = 0;
        char tmp[4];
        char *p = r->buf;
        char *end = r->buf + r->pos;

        if (*p == '$')          // skip leading '$'
                p++;

        while (*p != '*' && p < end)    // xor the payload byte
                chk ^= *p++;

        if (*p == '*') {        // compare with checksum in nmea sentence
                sprintf(tmp, "%02X", chk);
                return 0 == memcmp(tmp, p + 1, 2);
        }

        return false;
}

