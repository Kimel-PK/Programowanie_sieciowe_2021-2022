CFLAGS = -Wall -std=c++17 -pedantic
LDFLAGS = 
LDLIBS = -lcurl

.PHONY: all clean
all: rest.x

%.x: %.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)
%.o: %.cpp %.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

rest.x: rest.o
rest.o: rest.cpp

clean:
	$(RM) *.[ox] *.exe *.exe.stackdump