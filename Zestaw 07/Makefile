CFLAGS = -std=c++17 -lcurl
LDFLAGS = 
LDLIBS = 

.PHONY: all clean
all: status.x

%.x: %.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)
%.o: %.cpp %.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<

status.x: status.o
status.o: status.cpp

clean:
	$(RM) *.[ox] *.exe *.exe.stackdump