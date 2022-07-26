CC=gcc

CFlAGS = -Wall -Werror

all: scannerCSVsorter

scannerCSVsorter: scannerCSVsorter.c scannerCSVsorter.h 

	$(CC) $(CFLAGS) scannerCSVsorter.c -o scannerCSVsorter

clean:
	rm -f scannerCSVsorter
