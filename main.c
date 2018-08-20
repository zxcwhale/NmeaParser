#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "nmeaparser.h"

/*
 * Print navigation data and wait for user's keystroke
 * @navdata:    the navigation data
 */
void display_navdata(struct nav_data *navdata) {
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

/**
 * Read nmea sample file
 * Parse nmea protocol
 * Display navigation status
 */
int main(void)
{
        char buf[1024];
        struct nmea_parser parser[1];

        nmea_parser_init(parser);
        parser->report_nav_status = display_navdata; 

        // Open NMEA sample file
        FILE *f = fopen("nmea.txt", "rb");
        if (f == NULL) {
                printf("Can not open file: nmea.txt\n");
                return 1;
        }

        do {
                int readn = fread(buf, 1, sizeof(buf), f);      // read data from NMEA file
                int i;

                for (i = 0; i < readn; i++)                     // parse nmea data
                        nmea_parser_putchar(parser, buf[i]);

        } while (!feof(f));

        fclose(f);
        return 0;
}

