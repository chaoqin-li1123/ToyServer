CC       = g++
LIBS 	 =  -lstdc++ -lpthread
CFLAGS   = -g -Wall -I. -O3
CXXFLAGS = $(CFLAGS) $(LIBS)

all : server_main client_main
.PHONY : all

vpath %.h include
vpath %.cc exe

client_main : client_main.cc client.h utility.h
	cc $< -o $@ $(CXXFLAGS)

server_main : server_main.cc server.h listener.h utility.h
	cc $< -o $@ $(CXXFLAGS)

.PHONY : clean
clean :
	-rm client_main server_main