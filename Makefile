CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2

KAYNAK = src/rezervasyon.c src/girdi.c

otobus: src/main.c $(KAYNAK) src/rezervasyon.h src/girdi.h
	$(CC) $(CFLAGS) -o $@ src/main.c $(KAYNAK)

test: test/test_rezervasyon.c $(KAYNAK) src/rezervasyon.h src/girdi.h
	$(CC) $(CFLAGS) -o test_rezervasyon test/test_rezervasyon.c $(KAYNAK)
	./test_rezervasyon

clean:
	rm -f otobus otobus.exe test_rezervasyon test_rezervasyon.exe

.PHONY: test clean
