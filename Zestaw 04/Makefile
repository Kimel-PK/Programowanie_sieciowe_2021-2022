CFLAGS = -Wall -std=c99 -pedantic

.PHONY: all clean
all: serwer.x

%.x:
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

serwer.x: serwer.c

clean:
	$(RM) *.[ox] *.exe *.exe.stackdump