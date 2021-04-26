load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "gtest_multiplexer",
    deps = select({
        "@platforms//os:linux": ["@gtest//:gtest_main"],
        "//conditions:default": ["@pigweed//pw_unit_test"],
    }),
)

label_flag(
    name = "gtest",
    build_setting_default = ":gtest_multiplexer",
)
