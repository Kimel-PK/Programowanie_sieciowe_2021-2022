CFLAGS = -Wall -std=c99 -pedantic

.PHONY: all clean
all: zadanie1.x zadanie2.x zadanie3.x zadanie4.x zadanie6.x zadanie7.x zadanie8.x

%.x:
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

zadanie1.x: zadanie1.c
zadanie2.x: zadanie2.c
zadanie3.x: zadanie3.c
zadanie4.x: zadanie4.c
zadanie6.x: zadanie6.c
zadanie7.x: zadanie7.c
zadanie8.x: zadanie8.c

clean:
	$(RM) *.[ox] *.exe *.exe.stackdump