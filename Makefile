CC=gcc
CFLAGS=-Wall
RM=del
TARGET=main
SOURCES=main.c

main:
	$(CC) -o $(TARGET) $(SOURCES) $(CFLAGS)
clean:
	$(RM) main

.PHONY: main clean



