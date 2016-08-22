#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int BOOL;
#define TRUE 1
#define FALSE 0

#define MAX_SV_COUNT 48
#define MAX_SVID 256

typedef enum {
    GPS = 0,
    GLN,
    BDS,
    MAX_SV_SYS,
}SV_SYS;


typedef struct {
    int sys;
    int prn;
    int cn0;
    int elev;
    int azim;
    BOOL inUse;
}SATE;

const int PRN_PLUS_ARRAY[MAX_SV_SYS] = {0, 64, 200};
int encode_prn(int raw_prn, int sv_sys) {
    if (raw_prn < PRN_PLUS_ARRAY[sv_sys]) {
        return raw_prn + PRN_PLUS_ARRAY[sv_sys];
    }
    return raw_prn;
}

int get_sv_sys_from_prn(int encoded_prn) {
    if (encoded_prn > PRN_PLUS_ARRAY[BDS]) {
        return BDS;
    }
    else if (encoded_prn > PRN_PLUS_ARRAY[GLN]) {
        return GLN;
    }
    return GPS;
}

int decode_prn(int encoded_prn) {
    int sv_sys = get_sv_sys_from_prn(encoded_prn);

    return encoded_prn - PRN_PLUS_ARRAY[sv_sys];
}

typedef struct {
    int year;
    int month;
    int day;
}DATE;

typedef struct {
    int hour;
    int minute;
    int second;
    int ms;
}TIME;

typedef struct {
    DATE date;
    TIME time;
    BOOL fixValid;
    double lat;
    double lon;
    float alt;
    float speed;
    float heading;
    float hdop;
    float vdop;
    float pdop;
    int numSvFix;
    int numSvView;
    SATE svArray[MAX_SV_COUNT];
    char svFixMask[MAX_SVID];
}NAVDATA;

void navdata_init(NAVDATA *navdata) {
    memset(navdata, 0, sizeof(NAVDATA));
}

const char SV_TOKEN_ARRAY[3][4] = {"GPS", "GLN", "BDS"};
void navdata_display(NAVDATA *navdata) {
    printf("NAVDATA:\n");
    printf("NAVDATA.FIX_VALID = %d\n", navdata->fixValid);
    printf("NAVDATA.DATE = %d-%02d-%02d\n", navdata->date.year, navdata->date.month, navdata->date.day);
    printf("NAVDATA.TIME = %02d:%02d:%02d.%03d\n", navdata->time.hour, navdata->time.minute, navdata->time.second, navdata->time.ms);
    printf("NAVDATA.LAT = %.6f\n", navdata->lat);
    printf("NAVDATA.LON = %.6f\n", navdata->lon);
    printf("NAVDATA.ALT = %.2f\n", navdata->alt);
    printf("NAVDATA.HEADING = %.2f\n", navdata->heading);
    printf("NAVDATA.SPEED = %.2f\n", navdata->speed);
    printf("NAVDATA.HDOP = %.1f\n", navdata->hdop);
    printf("NAVDATA.VDOP = %.1f\n", navdata->vdop);
    printf("NAVDATA.PDOP = %.1f\n", navdata->pdop);
    printf("NAVDATA.NUM_SV_FIX = %d\n", navdata->numSvFix);
    printf("NAVDATA.NUM_SV_VIEW = %d\n", navdata->numSvView);
    int i = 0;
    for (i = 0; i < navdata->numSvView; i++) {
        SATE *sate = &navdata->svArray[i];
        printf("NAVDATA.SATE[%02d]: sys=%s prn=%02d, cn0=%02d, azim=%03d, elev=%02d, use=%d\n", i, SV_TOKEN_ARRAY[sate->sys], sate->prn, sate->cn0, sate->azim, sate->elev, sate->inUse);
    }
    printf("\n");
    printf("Press any key to continue...");
    getchar();
}

#define MAX_NMEA_LENGTH 1024
typedef struct {
    char buffer[MAX_NMEA_LENGTH];
    int count;
}NmeaReader;

void nmea_reader_init(NmeaReader *reader) {
    memset(reader, 0, sizeof(NmeaReader));
}

void nmea_reader_add(NmeaReader *reader, char c) {
    if (c == '$') {
        // nmeaReader->curIndex = 0;
        nmea_reader_init(reader);
    }
    int index = reader->count;
    reader->buffer[index] = c;
    index += 1;
    reader->count = index;
}

char nmea_reader_get(NmeaReader *reader, int index){
    return reader->buffer[index];
}

void nmea_reader_set(NmeaReader *reader, int index, char v) {
    reader->buffer[index] = v;
}

int nmea_reader_count(NmeaReader *reader) {
    return reader->count;
}

void nmea_reader_set_count(NmeaReader *reader, int n) {
    reader->count = n;
}

char *nmea_reader_token(NmeaReader *reader, int n) {
    return &reader->buffer[n];
}

//convert hex to int, for example: 1->1, A->10, F->15
char hexArray[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                   '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
int hex2int(char hexchar) {
    int i = 0;
    for (i = 0; i < 16; i ++) {
        if (hexArray[i] == hexchar)
            return i;
    }
    return -1;
}

BOOL nmea_reader_check(NmeaReader nmeaReader) {
    unsigned char chksumCal = 0xAA;
    unsigned char chksumRead = 0xBB;
    int IDLE = 0;
    int CAL_CHECKSUM = 1;
    int READ_CHECKSUM = 2;
    int state = IDLE;
    int i = 0;
    for (i = 0; i < nmea_reader_count(&nmeaReader); i++) {
        char curChar = nmea_reader_get(&nmeaReader, i);
        if (curChar == '$') {
            state = CAL_CHECKSUM;
            chksumCal = 0;
            continue;
        }
        else if (curChar == '*'){
            state = READ_CHECKSUM;
            chksumRead = 0;
            continue;
        }
        else if (curChar == '\r' || curChar == '\0') {
            break;
        }

        if (state == CAL_CHECKSUM) {
            chksumCal ^= (unsigned char)curChar;
        }
        else if (state == READ_CHECKSUM) {
            int ret = hex2int(curChar);
            if (ret != -1) {
                chksumRead *= 16;
                chksumRead += ret;
            }
            else {
                return FALSE;
            }
        }
    }
    if (chksumRead == chksumCal) {
        return TRUE;
    }
    else {
        printf("Check sum check failed.\n");
        return FALSE;
    }
}

typedef char *Token;
#define MAX_NMEATOKEN_COUNT 64
typedef struct {
    int count;
    Token tokens[MAX_NMEATOKEN_COUNT];
}NmeaTokenizer;

void nmea_tokenizer_init(NmeaTokenizer *tokenizer) {
    memset(tokenizer, 0, sizeof(NmeaTokenizer));
}

void nmea_tokenizer_set(NmeaTokenizer *tokenizer, Token nmeaToken, int index) {
    tokenizer->tokens[index] = nmeaToken;
}

Token nmea_tokenizer_get(NmeaTokenizer *tokenizer, int index) {
    return tokenizer->tokens[index];
}

int nmea_tokenizer_count(NmeaTokenizer *tokenizer) {
    return tokenizer->count;
}

void nmea_tokenizer_set_count(NmeaTokenizer *tokenizer, int n) {
    tokenizer->count = n;
}

void nmea_reader_to_tokenizer(NmeaReader *reader, NmeaTokenizer *tokenizer) {
    int i, counter = 0;
    BOOL flag = TRUE;
    for (i = 0; i < nmea_reader_count(reader); i++) {
        if (flag) {
            Token token = (Token)nmea_reader_token(reader, i);
            nmea_tokenizer_set(tokenizer, token, counter);
//            printf("[%02d] %s", counter, token);
            counter += 1;
            flag = FALSE;
        }

        char ch = nmea_reader_get(reader, i);
        if (ch == ',') {
            nmea_reader_set(reader, i, 0);
            flag = TRUE;
        }
    }
    nmea_tokenizer_set_count(tokenizer, counter);
}

double nmae_ll_to_double(Token value, Token sign) {
    double ddmm = atof(value);
    int dd = (int)(ddmm / 100);
    double mm = ddmm - dd * 100;
    double degrees = dd + mm / 60.0;
    if (strcmp(sign, "W") == 0 || strcmp(sign, "S") == 0) {
        degrees = -degrees;
    }

    return degrees;
}

TIME nmea_utc_to_time(Token utcToken) {
    double utc = atof(utcToken); // HHMMSS.MS: 101943.235-> 10:19:43.235 -> hour = 10, minute = 19, second = 43, ms = 235
    int n = (int)utc;
    TIME time;
    time.ms = (int)(1000 * (utc - n));
    time.hour = (n / 10000);
    time.minute = (n / 100) % 100;
    time.second = n % 100;
    return time;
}

int nmea_parser_get_sv_sys(Token head) {
    if (memcmp(head + 1, "BD", 2) == 0) {
        return BDS;
    }
    if (memcmp(head + 1, "GL", 2) == 0) {
        return GLN;
    }
    return GPS;
}

BOOL checkFixValid(Token fixValidToken) {
    if (strcmp(fixValidToken, "1") == 0 || strcmp(fixValidToken, "2") == 0 || strcmp(fixValidToken, "6") == 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL nmea_parser_gga(NmeaTokenizer *tokenizer, NAVDATA *navdata) {
    if (nmea_tokenizer_count(tokenizer) < 10) {
        return FALSE;
    }
    // get pix from tokens
    Token utcToken = nmea_tokenizer_get(tokenizer, 1);
    Token latToken = nmea_tokenizer_get(tokenizer, 2);
    Token nsToken = nmea_tokenizer_get(tokenizer, 3);
    Token lonToken = nmea_tokenizer_get(tokenizer, 4);
    Token ewToken = nmea_tokenizer_get(tokenizer, 5);
    Token fixValidToken = nmea_tokenizer_get(tokenizer, 6);
    Token numSvFixedToken = nmea_tokenizer_get(tokenizer, 7);
    Token hdopToken = nmea_tokenizer_get(tokenizer, 8);
    Token altToken = nmea_tokenizer_get(tokenizer, 9);

    navdata->fixValid = checkFixValid(fixValidToken);
    if (navdata->fixValid) {
        navdata->time = nmea_utc_to_time(utcToken);
        navdata->lon = nmae_ll_to_double(lonToken, ewToken);
        navdata->lat = nmae_ll_to_double(latToken, nsToken);
        navdata->numSvFix = atoi(numSvFixedToken);
        navdata->hdop = atof(hdopToken);
        navdata->alt = atof(altToken);
    }
    return TRUE;
}

BOOL nmea_parser_rmc(NmeaTokenizer *tokenizer, NAVDATA *navdata) {
    if (nmea_tokenizer_count(tokenizer) < 10) {
        return FALSE;
    }

    Token utcToken = nmea_tokenizer_get(tokenizer, 1);
    Token pixToken = nmea_tokenizer_get(tokenizer, 2);
    Token latToken = nmea_tokenizer_get(tokenizer, 3);
    Token nsToken = nmea_tokenizer_get(tokenizer, 4);
    Token lonToken = nmea_tokenizer_get(tokenizer, 5);
    Token ewToken = nmea_tokenizer_get(tokenizer, 6);
    Token speedToken = nmea_tokenizer_get(tokenizer, 7);
    Token headingToken = nmea_tokenizer_get(tokenizer, 8);
    Token utcDateToken = nmea_tokenizer_get(tokenizer, 9);

    navdata->fixValid = strcmp(pixToken, "A") == 0;
    if (navdata->fixValid) {
        navdata->time = nmea_utc_to_time(utcToken);
        navdata->lon = nmae_ll_to_double(lonToken, ewToken);
        navdata->lat = nmae_ll_to_double(latToken ,nsToken);
        navdata->speed = atof(speedToken);
        navdata->heading = atof(headingToken);
        int shortDate = atoi(utcDateToken); // DDMMYY: 100816 -> 2016-08-10
        navdata->date.year = shortDate % 100 + 2000;
        navdata->date.month = (shortDate / 100) % 100;
        navdata->date.day = shortDate / 10000;
    }
    return TRUE;
}

BOOL nmea_parser_gsa(NmeaTokenizer *tokenizer, NAVDATA *navdata) {
    if (nmea_tokenizer_count(tokenizer) < 18) {
        return FALSE;
    }

    Token headToken = nmea_tokenizer_get(tokenizer, 0);
    int svSys = nmea_parser_get_sv_sys(headToken);
    int i, prn;
    for (i = 3; i <= 14;i++) {
        prn = atoi(nmea_tokenizer_get(tokenizer, i));
        if (prn > 0) {
            prn = encode_prn(prn, svSys);
            navdata->svFixMask[prn] = 1;
        }
    }
    Token pdopToken = nmea_tokenizer_get(tokenizer, 15);
    Token hdopToken = nmea_tokenizer_get(tokenizer, 16);
    Token vdopToken = nmea_tokenizer_get(tokenizer, 17);

    navdata->pdop = atof(pdopToken);
    navdata->hdop = atof(hdopToken);
    navdata->vdop = atof(vdopToken);

    return TRUE;
}

BOOL nmea_parser_gsv(NmeaTokenizer *tokenizer, NAVDATA *navdata) {
    if (nmea_tokenizer_count(tokenizer) < 3) {
        return FALSE;
    }

    Token headToken = nmea_tokenizer_get(tokenizer, 0);
    int svSys = nmea_parser_get_sv_sys(headToken);
    int i, prn;

    for (i = 4; i + 4 <= nmea_tokenizer_count(tokenizer); i += 4) {
        Token prnToken = nmea_tokenizer_get(tokenizer, i);
        Token elevToken = nmea_tokenizer_get(tokenizer, i + 1);
        Token azimToken = nmea_tokenizer_get(tokenizer, i + 2);
        Token cn0Token = nmea_tokenizer_get(tokenizer, i + 3);

        prn = atoi(prnToken);
        if (prn > 0) {
            prn = encode_prn(prn, svSys);
            int n = navdata->numSvView;

            SATE *sate = &navdata->svArray[n];
            sate->prn = decode_prn(prn);
            sate->sys = get_sv_sys_from_prn(prn);
            sate->inUse = navdata->svFixMask[prn] != 0;
            sate->cn0 = atoi(cn0Token);
            sate->azim = atoi(azimToken);
            sate->elev = atoi(elevToken);

            navdata->numSvView = n + 1;
        }
    }
    return TRUE;
}

BOOL nmea_parser_check_cmd(Token head, const char type[]) {
    if (memcmp(head + 3, type, 3) == 0) {
        return TRUE;
    }
    return FALSE;
}

void nmea_parser(NmeaTokenizer *tokenizer, NAVDATA *navdata) {
    Token headToken = nmea_tokenizer_get(tokenizer, 0);

    if (nmea_parser_check_cmd(headToken, "GGA")) {
        navdata_display(navdata);
        navdata_init(navdata);
    }

    if (nmea_parser_check_cmd(headToken, "GGA")) {
        nmea_parser_gga(tokenizer, navdata);
    }
    else if (nmea_parser_check_cmd(headToken, "RMC")) {
        nmea_parser_rmc(tokenizer, navdata);
    }
    else if (nmea_parser_check_cmd(headToken, "GSA")) {
        nmea_parser_gsa(tokenizer, navdata);
    }
    else if (nmea_parser_check_cmd(headToken, "GSV")) {
        nmea_parser_gsv(tokenizer, navdata);
    }
}

int main(void)
{
    NmeaReader reader;
    nmea_reader_init(&reader);

    NmeaTokenizer tokenizer;

    NAVDATA navdata;
    navdata_init(&navdata);

    FILE *f = fopen("nmea.txt", "rb");
    if (f != NULL) {
        char buffer[1024];
        int n = fread(buffer, 1, sizeof(buffer), f);
        while (n > 0) {
            int i = 0;
            for (i = 0; i < n; i++) {
                char ch = buffer[i];
                nmea_reader_add(&reader, ch);
                if (ch == '\n') {
                    if (nmea_reader_check(reader)) {
                        nmea_tokenizer_init(&tokenizer);
                        nmea_reader_to_tokenizer(&reader, &tokenizer);
                        nmea_parser(&tokenizer, &navdata);
                    }
                }
            }
            n = fread(buffer, 1, sizeof(buffer), f);
        }
    }
    else {
        printf("Can not open file: nmea.txt\n");
    }
    return 0;
}

