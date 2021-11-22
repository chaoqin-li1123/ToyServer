load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "client_main",
    srcs = ["client_main.cc"],
    deps = ["//include:client"]
)

cc_binary(
    name = "server_main",
    srcs = ["server_main.cc"],
    deps = ["//include:server"],
    linkopts = ["-lpthread"]
)

