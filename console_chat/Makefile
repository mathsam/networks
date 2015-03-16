all: chat.x

CPPC = g++
CPPFLAGS = -std=c++11 -pthread

tcp_sockets.o: tcp_sockets.cpp
	$(CPPC) $(CPPFLAGS) -c tcp_sockets.cpp -o tcp_sockets.o

main.o: main.cpp
	$(CPPC) $(CPPFLAGS) -c main.cpp -o main.o

chat.x: main.o tcp_sockets.o
	$(CPPC) $(CPPFLAGS) -o chat.x main.o tcp_sockets.o

clean:
	rm *o *x
