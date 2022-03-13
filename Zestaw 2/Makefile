CFLAGS = -Wall -std=c99 -pedantic

.PHONY: all clean
all: zadanie5.x zadanie7.x zadanie8.x zadanie9.x

%.x:
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

zadanie5.x: zadanie5.c
zadanie7.x: zadanie7.c
zadanie9.x: zadanie9.c

clean:
	$(RM) *.[ox] *.exe *.exe.stackdump