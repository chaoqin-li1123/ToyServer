CC       = g++
CFLAGS   = -g -Wall
CXXFLAGS = ${CFLAGS} -lstdc++ -lpthread

client_main server_main : client_main.o server_main.o
	cc client_main.o -o client_main $(CXXFLAGS)
	cc server_main.o -o server_main $(CXXFLAGS)

client_main.o : client_main.cc client.h
server_main.o : server_main.cc server.h utility.h listener.h

clean :
	rm *.o client_main server_main