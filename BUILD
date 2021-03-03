load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")


cc_binary(
    name = "epoll_server",
    srcs = ["epoll_server.cc"],
    deps = [":listener"]
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