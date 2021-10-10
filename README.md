# ToyServer
To build the server
bazel build //:server_main
To build the client
bazel build //:client_main

To run the server
./bazel-bin/server_main port

To run the client
./bazel-bin/client_main hostname port