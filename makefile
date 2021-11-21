CC       = g++
CFLAGS   = -g -Wall
CXXFLAGS = ${CFLAGS} -lstdc++ -lpthread

client_main server_main : *.cc *.h
	cc client_main.cc -o client_main $(CXXFLAGS)
	cc server_main.cc -o server_main $(CXXFLAGS)

clean :
	rm client_main server_main