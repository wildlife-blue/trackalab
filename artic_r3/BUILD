load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("//config:selects.bzl", "TARGET_COMPATIBLE_WITH_HOST")

cc_library(
    name = "artic_r3",
    hdrs = ["public/artic_r3/artic_r3.h"],
    strip_include_prefix = "public",
)

cc_test(
    name = "artic_r3_test",
    srcs = ["artic_r3_test.cc"],
    target_compatible_with = select(TARGET_COMPATIBLE_WITH_HOST),
    deps = [
        ":artic_r3",
        "//config:gtest",
        "//gpio",
        "//spi",
        "@pigweed//pw_assert",
        "@pigweed//pw_chrono:system_clock",
        "@pigweed//pw_status",
    ],
)
