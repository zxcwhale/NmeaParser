/*
 * =====================================================================================
 *
 *       Filename:  navdata.c
 *
 *    Description:  Nav data source file
 *
 *        Version:  1.0
 *        Created:  2018/8/15 17:31:22
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
#include "navdata.h"

#define PRN_PLUS_GLN    64
#define PRN_PLUS_BDS    200
/*
 * PRN and SVID
 * PRN is the satellite's NO. in NMEA protocol. Normally from 1 to 32.
 * SVID is the satellite's id in all constellations.
 * 1-64:        GPS 
 * 65-96:       GLNONASS 
 * 201-232:     Beidou
 */

/*
 * Convert prn and constell to svid
 * @prn:        prn of the satellite
 * @constell:   constellation type
 * @return:     svid of the satellite
 */
int prn2svid(int prn, int constell) 
{
        switch (constell) {
        case CONSTELL_TYPE_GPS:
                break;
        case CONSTELL_TYPE_GLN:
                if (prn > 0 && prn < 33)
                        prn += PRN_PLUS_GLN;
                break;
        case CONSTELL_TYPE_BDS:
                if (prn > 0 && prn < 33)
                        prn += PRN_PLUS_BDS;
                break;
        default:
                break;
        }

        return prn;
}

/*
 * Tell the constellation type of a SVID
 * @sivd:       the SVID
 * @return:     the constellationtype
 */
int tell_constell(int svid) 
{
        if (svid > 0 && svid < 65) {
                return CONSTELL_TYPE_GPS;
        }
        else if (svid > 64 && svid < 97) {
                return CONSTELL_TYPE_GLN;
        }
        else if (svid > 200 && svid < 233) {
                return CONSTELL_TYPE_BDS;
        }

        return CONSTELL_TYPE_UNKNOWN;
}

/*
 * Get constellation's name string
 * @constell:   constellation type
 * @return:     the name string
 */
const char *constell_name(int constell)
{
        switch(constell) {
        case CONSTELL_TYPE_GPS:
                return "GPS";
        case CONSTELL_TYPE_GLN:
                return "GLN";
        case CONSTELL_TYPE_BDS:
                return "BDS";
        default:
                return "UNKNOWN";
        }
}

/*
 * Init navigation data to 0
 * @navdata:    the navigation data
 */
void navdata_init(struct nav_data *navdata) 
{
        memset(navdata, 0, sizeof(struct nav_data));
}

/*
 * Print navigation data and wait for user's keystroke
 * @navdata:    the navigation data
 */
void navdata_display(struct nav_data *navdata) {
        printf("NAVDATA:\n");
        printf("NAVDATA.FIX_VALID = %d\n", navdata->is_fixed);
        printf("NAVDATA.DATE = %d-%02d-%02d\n", navdata->date.year, navdata->date.month, navdata->date.day);
        printf("NAVDATA.TIME= %02d:%02d:%02d.%03d\n", navdata->time.hour, navdata->time.minute, navdata->time.second, navdata->time.ms);
        printf("NAVDATA.LAT = %.6f\n", navdata->lat);
        printf("NAVDATA.LON = %.6f\n", navdata->lon);
        printf("NAVDATA.ALT = %.2f\n", navdata->alt);
        printf("NAVDATA.HEADING = %.2f\n", navdata->heading);
        printf("NAVDATA.SPEED = %.2f\n", navdata->speed);
        printf("NAVDATA.HDOP = %.1f\n", navdata->hdop);
        printf("NAVDATA.VDOP = %.1f\n", navdata->vdop);
        printf("NAVDATA.PDOP = %.1f\n", navdata->pdop);
        printf("NAVDATA.NUM_SV_FIX = %d\n", navdata->sv_inuse);
        printf("NAVDATA.NUM_SV_VIEW = %d\n", navdata->sv_inview);

        int svid;
        for (svid = 0; svid < MAX_SVID; svid++) { 
                struct sate *sate = navdata->sates + svid;
                if (sate->valid)
                        printf("NAVDATA.SATE[%02d]: constell=%s prn=%02d, cn0=%02d, azim=%03d, elev=%02d, use=%d\n", 
                                        svid, constell_name(sate->constell), sate->prn, sate->cn0, sate->azim, sate->elev, sate->in_use);
        } 

        printf("\n");
        printf("Press any key to continue...");
        getchar();
}
