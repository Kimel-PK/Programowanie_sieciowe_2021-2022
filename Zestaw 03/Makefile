CFLAGS = -Wall -std=c99 -pedantic

.PHONY: all clean
all: zadanie4.x

%.x:
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

zadanie4.x: zadanie4.c

clean:
	$(RM) *.[ox] *.exe *.exe.stackdump