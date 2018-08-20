# NmeaParser

This project demonstrate how to parser NMEA protocol.

## How to use

1. Include "nmeaparser.h" in your source file.
2. Create a  nmeaparser: `struct nmea_parser parser[1];`.
3. Init the `parser` with `nmea_parser_init(parser);`.
4. Create your navigation report function: `void my_reporter(struct nav_data *navdata){...}`.
5. Setup `parser`'s report funtion: `parser->report_nav_status = my_reporter;`.
6. When read data from serialport or file, add thee data character by character to the parser with `nmea_parser_putchar(parser, c)`.
7. When navigation data is ready, `parser` will trigger `my_reporter` function to report navigation status.

## Modules

| Module           | Files                         | Description                                 |
| ---------------- | ----------------------------- | ------------------------------------------- |
| `nmea_reader`    | `nmeardr.h` `nmeardr.c`       | Read NMEA data, contains one NMEA sentence. |
| `nmea_tokenizer` | `nmeatknzr.h` `nmeatknzr.c`   | Split NMEA sentence into `token`s.          |
| `parser`         | `nmeaparser.h` `nmeaparser.c` | Parse nmea from `token`s.                   |
| `nav_data`       | `navdata.h` `navdata.c`       | Navigation data.                            |

1. Add NMEA data to `nmea_reader` by character.
2. If encounter '\n',  using `nmea_tokenizer` split `nmea_reader`.
3. Parse `nmea_tokenizer` , and store results int `nav_data`.
4. If encounter `GGA` sentence, print `nav_data`.

## Navigation data

#### Date and Time

Date and time are in UTC.

#### Latitude

In degrees.

Positive for North, and negative for South.

#### Longitude

In degrees.

Positive for  East, and negative for West.

#### Satellite's PRN and SVID

`PRN` is the satellite's NO. in NMEA. Normally from 1 to 32.

`SVID` is the index in satellite's array.

| SVID range | Constellation type |
| ---------- | ------------------ |
| 1-64       | GPS                |
| 65-96      | GLONASS            |
| 201-232    | Beidou             |



