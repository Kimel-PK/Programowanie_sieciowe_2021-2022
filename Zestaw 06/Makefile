CFLAGS = -Wall -std=c++17

.PHONY: all clean
all: serwer.x tester.x

%.x:
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

serwer.x: serwer.cpp

tester.x: tester.cpp

clean:
	$(RM) *.[ox] *.exe *.exe.stackdump