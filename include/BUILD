load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "server",
    hdrs = ["server.h"],
    deps = [":listener"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "client",
    hdrs = ["client.h"],
    deps = [":utility"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "listener",
    hdrs = ["listener.h"],
    deps = [":utility"],
    visibility = ["//exe:__pkg__"],
)

cc_library(
    name = "utility",
    hdrs = ["utility.h"],
    visibility = ["//visibility:public"],
)