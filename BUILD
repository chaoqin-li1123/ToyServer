load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "client_main",
    srcs = ["client_main.cc"],
    deps = ["client"]
)

cc_binary(
    name = "server_main",
    srcs = ["server_main.cc"],
    deps = [":listener", ":server"],
    linkopts = ["-lpthread"]
)

cc_library(
    name = "server",
    hdrs = ["server.h"],
    deps = [":listener"],
)

cc_library(
    name = "client",
    hdrs = ["client.h"],
    deps = [":utility"]
)

cc_library(
    name = "listener",
    hdrs = ["listener.h"],
    deps = [":utility"]
)

cc_library(
    name = "utility",
    hdrs = ["utility.h"]
)