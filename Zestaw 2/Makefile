CFLAGS = -Wall -std=c99 -pedantic

.PHONY: all clean
all: zadanie5.x zadanie7.x zadanie9-klient.x zadanie9-serwer.x

%.x:
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

zadanie5.x: zadanie5.c
zadanie7.x: zadanie7.c
zadanie9-klient.x: zadanie9-klient.c
zadanie9-serwer.x: zadanie9-serwer.c

clean:
	$(RM) *.[ox] *.exe *.exe.stackdump