#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "nmeaparser.h"

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

