/*
 * =====================================================================================
 *
 *       Filename:  navdata.h
 *
 *    Description:  Navdata head file
 *
 *        Version:  1.0
 *        Created:  2018/8/15 17:22:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jarod Lee 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __NAV_DATA_H
#define __NAV_DATA_H
#include <stdbool.h>

#define MAX_SVID        300

enum {
        CONSTELL_TYPE_UNKNOWN = 0,
        CONSTELL_TYPE_GPS,
        CONSTELL_TYPE_GLN,
        CONSTELL_TYPE_BDS,
        MAX_CONSTELL_TYPES,
};

/*
 * Satellite structure
 */
struct sate {
        int constell;   // GPS or GLN or BDS....
        int prn;
        int cn0;        
        int elev;       // elevation
        int azim;       // azimuth
        bool in_use;    // is the satellite used in fix
        bool valid;     // is the satellite valid
};

/*
 * Gnss date structure
 */
struct gnss_date {
        int year;
        int month;
        int day;
};

/*
 * Gnss time structure
 */
struct gnss_time {
        int hour;
        int minute;
        int second;
        int ms;
};

/*
 * Navigation data
 */
struct nav_data {
        struct gnss_date date;
        struct gnss_time time;
        bool    is_fixed;
        double  lat;
        double  lon;
        float   alt;
        float   speed;
        float   heading;
        float   hdop;
        float   vdop;
        float   pdop;
        int     sv_inuse;
        int     sv_inview;
        int     sv_count;
        struct sate sates[MAX_SVID];
};

int prn2svid(int prn, int constell);    // convert @prn and @constell to SVID
int tell_constell(int svid);            // tell the constellation type of @svid
const char *constell_name(int constell);        // get constellation type's name
void navdata_init(struct nav_data *d);          // int nav_data to 0

#endif
