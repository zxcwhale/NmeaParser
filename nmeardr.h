/*
 * =====================================================================================
 *
 *       Filename:  nmeardr.h
 *
 *    Description:  Nmea reader's head file
 *
 *        Version:  1.0
 *        Created:  2018/8/15 16:55:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jarod Lee 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __NMEA_READER_H
#define __NMEA_READER_H

#define MAX_NMEARDR_SIZE 1024
/*
 * Nmea reader structure
 * Read one NMEA sentence
 * Always keep '$' as first byte and '\n' as last byte
 */
struct nmea_reader {
        char buf[MAX_NMEARDR_SIZE];     // nmea sentence buffer
        int pos;                        // index of first free byte 
};


void nmea_reader_init(struct nmea_reader *r);           // init nmea reader to 0
void nmea_reader_add(struct nmea_reader *r, char c);    // add 1 byte c to nmea reader
bool nmea_reader_check(struct nmea_reader *r);          // check nmea checksum of nmea reader

#endif

