# ToyServer
To build the server with bazel
bazel build //:server_main
To build the client
bazel build //:client_main

To run the server
./bazel-bin/server_main hostname port

To run the client
./bazel-bin/client_main hostname port

To build the server with make
make

To run the server
server_main hostname port
client_main hostname port

