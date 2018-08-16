CC=gcc
CFLAGS=-Wall
RM=del
TARGET=main
SOURCES=main.c nmeardr.c nmeatknzr.c navdata.c nmeaparser.c

main:
	$(CC) -o $(TARGET) $(SOURCES) $(CFLAGS)
clean:
	$(RM) main

.PHONY: main clean



