/*
 * =====================================================================================
 *
 *       Filename:  nmeaparser.h
 *
 *    Description:  Nmea parser head file
 *
 *        Version:  1.0
 *        Created:  2018/8/15 17:42:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jarod Lee  
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __NMEA_PARSER_H
#define __NMEA_PARSER_H

#include "nmeardr.h"
#include "nmeatknzr.h"
#include "navdata.h"

/*
 * Nmea's parser struct
 */
struct nmea_parser {
        struct nmea_reader      reader[1];      // read nmea data
        struct nmea_tokenizer   tzer[1];        // split nmea tokens
        struct nav_data         data[1];        // store navigation data

        void (*report_nav_status)(struct nav_data *navdata);    // callback function to report navigtaion status
};

void nmea_parser_init(struct nmea_parser *p);
void nmea_parser_putchar(struct nmea_parser *p, char c);

#endif
