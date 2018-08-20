/*
 * =====================================================================================
 *
 *       Filename:  nmeaparser.c
 *
 *    Description:  Nmea parser source file
 *
 *        Version:  1.0
 *        Created:  2018/8/15 17:43:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jarod Lee(), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "nmeaparser.h"

/*
 * Convert nmea string to integer
 * @p:          nmea string start ptr
 * @end:        nmea string end ptr
 * @return:     the integer value
 */
static int str2int(const char *p, const char *end) {
        int result = 0;
        while(p < end)
        {
                int c = *p - '0';
                if ((unsigned)c >= 10) {
                        return -1;
                }
                result = result * 10 + c;
                p++;
        }
        return  result;
}

/*
 * Convert nmea string to float
 * @p:          nmea string start ptr
 * @end:        nmea string end ptr
 * @return:     the float value
 */
static double str2float(const char *p, const char *end) {
        int   len    = end - p;
        char  temp[16];

        if (len >= (int)sizeof(temp))
                return 0.;

        memcpy( temp, p, len );
        temp[len] = 0;
        return strtod( temp, NULL );
}

/*
 * Convert nmea's latitude or longitude token to degrees
 * @tok:        latitude or longitude token
 * @return:     latitude or longitude in degrees
 */
static double convert_from_hhmm(struct token  tok) {
        double val     = str2float(tok.p, tok.end);
        int    degrees = (int)(floor(val) / 100);
        double minutes = val - degrees*100.;
        double dcoord  = degrees + minutes / 60.0;
        return dcoord;
}

/*
 * Convert nmea's latitude to degrees
 * @lat_tok:    nmea latitude token
 * @lat_hemi:   'N' means positive while 'S' means negative
 */
static double convert_latitude(struct token lat_tok, char lat_hemi)
{
        double lat = convert_from_hhmm(lat_tok);
        return (lat_hemi == 'N' ? 1 : -1) * lat;
}

/*
 * Convert nmea's longitude to degrees
 * @lon_tok:    nmea longitude token
 * @lon_hemi:   'E' means positive while 'W' means negative
 */
static double convert_longitude(struct token lon_tok, char lon_hemi)
{
        double lon = convert_from_hhmm(lon_tok);
        return (lon_hemi == 'E' ? 1 : -1) * lon;
}

/*
 * Convert nmea time token to gnss time struct
 * @tok:        nmea's time token
 * @return:     gnss_time structure
 */
static struct gnss_time convert_gnss_time(struct token tok)
{
        // HHMMSS.MS: 101943.235-> 10:19:43.235 -> hour = 10, minute = 19, second = 43, ms = 235
        double val = str2float(tok.p, tok.end);
        struct gnss_time time;

        memset(&time, 0, sizeof(struct gnss_time));

        time.hour       = (int)(val / 10000);
        time.minute     = (int)(val / 100) % 100;
        time.second     = ((int)val) % 100;
        time.ms         = (int)(0.5 + 1000 * (val - floor(val)));

        return time;
}

/*
 * Convert nmea date token into gnss_date struct
 * @tok:        nmea's date token in RMC
 * @return:     gnss_date structure
 */
static struct gnss_date convert_gnss_date(struct token tok)
{
        int val = str2int(tok.p, tok.end);
        struct gnss_date date;

        memset(&date, 0, sizeof(struct gnss_date));

        date.year = 2000 + (val % 100);
        date.month = (val / 100) % 100;
        date.day = (int)(val / 10000);

        return date;
}

/*
 * Get constellation type by nmea sentence token
 * @p:          ptr to nmea's sentence start
 * @return:     constellation type
 */
static int get_sv_constell(char *p)
{
        if (0 == memcmp(p, "GP", 2))
                return CONSTELL_TYPE_GPS;
        else if (0 == memcmp(p, "GL", 2))
                return CONSTELL_TYPE_GLN;
        else if (0 == memcmp(p, "BD", 2))
                return CONSTELL_TYPE_BDS;

        return CONSTELL_TYPE_UNKNOWN;
}

/*
 * Is the quality means fixed.
 * @quality:    fix flag in GGA
 * @return:     true for fixed, false for not fixed
 */
static bool is_quality_fixed(char quality) {
        switch(quality) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
                return true;
        default:
                return false;

        }
}

/*
 * Is the mode means fixed
 * @mode:       fix flag in RMC
 * @return:     true for fixed, false for not fixed
 */
static bool is_mode_fixed(char mode)
{
        switch(mode) {
        case 'A':
        case 'D':
        case 'E':
                return true;
        default:
                return false;
        }

}

/*
 * Parse GGA, store the parsed data in navigation data
 * @tzer:       current tokenizer
 * @navdata:    navigation data
 */
void parse_gga(struct nmea_tokenizer *tzer, struct nav_data *navdata) 
{
        struct token tok_time          = nmea_tokenizer_get(tzer, 1);
        struct token tok_latitude      = nmea_tokenizer_get(tzer, 2);
        struct token tok_latitudeHemi  = nmea_tokenizer_get(tzer, 3);
        struct token tok_longitude     = nmea_tokenizer_get(tzer, 4);
        struct token tok_longitudeHemi = nmea_tokenizer_get(tzer, 5);
        struct token tok_fixQuality    = nmea_tokenizer_get(tzer, 6);
        struct token tok_svNumInUse    = nmea_tokenizer_get(tzer, 7);
        struct token tok_hdop          = nmea_tokenizer_get(tzer, 8);
        struct token tok_altitude      = nmea_tokenizer_get(tzer, 9);

        navdata->is_fixed = is_quality_fixed(tok_fixQuality.p[0]);
        if (navdata->is_fixed) {
                navdata->time   = convert_gnss_time(tok_time);
                navdata->lat    = convert_latitude(tok_latitude, tok_latitudeHemi.p[0]);
                navdata->lon    = convert_longitude(tok_longitude, tok_longitudeHemi.p[0]);
                navdata->sv_inuse = str2int(tok_svNumInUse.p, tok_svNumInUse.end);
                navdata->hdop   = str2float(tok_hdop.p, tok_hdop.end);
                navdata->alt    = str2float(tok_altitude.p, tok_altitude.end);
        }
}

/*
 * Parse RMC, store the parsed data in navigation data
 * @tzer:       current tokenizer
 * @navdata:    navigation data
 */
void parse_rmc(struct nmea_tokenizer *tzer, struct nav_data *navdata) {
        struct token  tok_time          = nmea_tokenizer_get(tzer,1);
        struct token  tok_dataValid     = nmea_tokenizer_get(tzer,2);
        struct token  tok_latitude      = nmea_tokenizer_get(tzer,3);
        struct token  tok_latitudeHemi  = nmea_tokenizer_get(tzer,4);
        struct token  tok_longitude     = nmea_tokenizer_get(tzer,5);
        struct token  tok_longitudeHemi = nmea_tokenizer_get(tzer,6);
        struct token  tok_speed         = nmea_tokenizer_get(tzer,7);
        struct token  tok_heading       = nmea_tokenizer_get(tzer,8);
        struct token  tok_date          = nmea_tokenizer_get(tzer,9);
        struct token  tok_fixMode       = nmea_tokenizer_get(tzer,12);

        navdata->is_fixed = is_mode_fixed(tok_fixMode.p[0]);
        if (navdata->is_fixed) {
                navdata->time   = convert_gnss_time(tok_time);
                navdata->lat    = convert_latitude(tok_latitude, tok_latitudeHemi.p[0]);
                navdata->lon    = convert_longitude(tok_longitude, tok_longitudeHemi.p[0]);
                navdata->speed  = str2float(tok_speed.p, tok_speed.end);
                navdata->heading = str2float(tok_heading.p, tok_heading.end);
        }

        if (tok_dataValid.p[0] == 'A')
                navdata->date   = convert_gnss_date(tok_date);
}

/*
 * Parse GSA, store the parsed data in navigation data
 * @tzer:       current tokenizer
 * @navdata:    navigation data
 */
void parse_gsa(struct nmea_tokenizer *tzer, struct nav_data *navdata) {
        struct token tok_id      = nmea_tokenizer_get(tzer, 0);
        struct token tok_pdop    = nmea_tokenizer_get(tzer, 15);
        struct token tok_hdop    = nmea_tokenizer_get(tzer, 16);
        struct token tok_vdop    = nmea_tokenizer_get(tzer, 17);
        struct token tok_svs     = nmea_tokenizer_get(tzer, 18);

        int constell = get_sv_constell(tok_id.p);
        switch(tok_svs.p[0]) {
        case '1':
                constell = CONSTELL_TYPE_GPS;
                break;
        case '2':
                constell = CONSTELL_TYPE_GLN;
                break;
        case '4':
                constell = CONSTELL_TYPE_BDS;
                break;
        default:
                break;
        }

        int i;
        for (i = 3; i <= 14; i++) {
                struct token tok_prn = nmea_tokenizer_get(tzer, i);
                int svid = prn2svid(str2int(tok_prn.p, tok_prn.end), constell);
                if (svid > 0 && svid < MAX_SVID) {
                        navdata->sates[svid].in_use = true;
                }
        }

        navdata->pdop = str2float(tok_pdop.p, tok_pdop.end);
        navdata->hdop = str2float(tok_hdop.p, tok_hdop.end);
        navdata->vdop = str2float(tok_vdop.p, tok_vdop.end);
}

/*
 * Parse GSV, store the parsed data in navigation data
 * @tzer:       current tokenizer
 * @navdata:    navigation data
 */
void parse_gsv(struct nmea_tokenizer *tzer, struct nav_data *d) {
        struct token  tok_id            = nmea_tokenizer_get(tzer, 0);
        struct token  tok_noSentences   = nmea_tokenizer_get(tzer, 1);
        struct token  tok_sentence      = nmea_tokenizer_get(tzer, 2);
        struct token  tok_noSatellites  = nmea_tokenizer_get(tzer, 3);

        int constell            = get_sv_constell(tok_id.p);
        int noSatellites        = str2int(tok_noSatellites.p, tok_noSatellites.end);
        int sentence            = str2int(tok_sentence.p, tok_sentence.end);
        int totalSentences      = str2int(tok_noSentences.p, tok_noSentences.end);

        if (sentence == 1)
                d->sv_count = 0;

        if (noSatellites > 0) {
                int i = 0;
                while (i < 4 && d->sv_count < noSatellites) {
                        struct token  tok_prn           = nmea_tokenizer_get(tzer, i * 4 + 4);
                        struct token  tok_elevation     = nmea_tokenizer_get(tzer, i * 4 + 5);
                        struct token  tok_azimuth       = nmea_tokenizer_get(tzer, i * 4 + 6);
                        struct token  tok_snr           = nmea_tokenizer_get(tzer, i * 4 + 7);

                        int prn = str2int(tok_prn.p, tok_prn.end);
                        int svid = prn2svid(prn, constell);
                        if (svid > 0 && svid < MAX_SVID) {
                                d->sates[svid].prn      = prn; 
                                d->sates[svid].elev     = str2float(tok_elevation.p, tok_elevation.end);
                                d->sates[svid].azim     = str2float(tok_azimuth.p, tok_azimuth.end);
                                d->sates[svid].cn0      = str2float(tok_snr.p, tok_snr.end);
                                d->sates[svid].valid    = 1;
                                d->sates[svid].constell = tell_constell(svid);
                        }

                        d->sv_count++;
                        d->sv_inview++;
                        i += 1;
                }
        }

        if (sentence == totalSentences)
                d->sv_count = 0;
}

/*
 * Parse current nmea sentence
 */
void parse(struct nmea_tokenizer *tzer, struct nav_data *navdata, void (*reporter)(struct nav_data *)) {
        struct token tok = nmea_tokenizer_get(tzer, 0);

        tok.p += 2;
        // display and reset nav_data if encounter GGA sentence
        if (memcmp(tok.p, "GGA", 3) == 0) {
                // navdata_display(navdata);
                if (reporter)
                        reporter(navdata);
                
                navdata_init(navdata);
        }

        if (memcmp(tok.p, "GGA", 3) == 0) {             // parse GGA
                parse_gga(tzer, navdata);
        }
        else if (memcmp(tok.p, "RMC", 3) == 0) {        // parse RMC
                parse_rmc(tzer, navdata);
        }
        else if (memcmp(tok.p, "GSA", 3) == 0) {        // parse GSA
                parse_gsa(tzer, navdata);
        }
        else if (memcmp(tok.p, "GSV", 3) == 0) {        // parse GSV
                parse_gsv(tzer, navdata);
        }
}

/*
 * Init nmea parser
 */
void nmea_parser_init(struct nmea_parser *p)
{
        memset(p, 0, sizeof(*p));
}

/*
 * Put one char into nmea parser
 * @p:          nmea_parser
 * @c:          the character placed into nmea parser
 */
void nmea_parser_putchar(struct nmea_parser *p, char c)
{
        nmea_reader_add(p->reader, c);
        if (c == '\n') {
                if (nmea_reader_check(p->reader)) {
                        nmea_tokenizer_init(p->tzer, p->reader->buf, p->reader->buf + p->reader->pos);
                        parse(p->tzer, p->data, p->report_nav_status);
                }
                nmea_reader_init(p->reader);
        }
}
