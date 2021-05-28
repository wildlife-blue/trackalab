load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "tr_spi",
    srcs = ["public/tr_spi/spi.h"],
    includes = ["public"],
    deps = [
        "//tr_utils:attributes",
        "@pigweed//pw_bytes",
        "@pigweed//pw_result",
        "@pigweed//pw_span",
        "@pigweed//pw_status",
    ],
)
