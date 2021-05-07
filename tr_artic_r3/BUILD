load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("//config:selects.bzl", "TARGET_COMPATIBLE_WITH_HOST")

cc_library(
    name = "artic_r3",
    hdrs = ["public/artic_r3/artic_r3.h"],
    strip_include_prefix = "public",
)

cc_library(
    name = "standard_mode",
    hdrs = ["public/internal/standard_mode.h"],
    strip_include_prefix = "public",
    deps = ["@pigweed//pw_span"],
)

cc_test(
    name = "standard_mode_test",
    srcs = ["standard_mode_test.cc"],
    deps = [
        ":standard_mode",
        "//config:gtest",
        "@pigweed//pw_bytes",
    ],
)

cc_library(
    name = "command_mode",
    hdrs = ["public/internal/command_mode.h"],
    strip_include_prefix = "public",
    deps = [
        "@pigweed//pw_assert",
        "@pigweed//pw_status",
    ],
)

cc_test(
    name = "command_mode_test",
    srcs = ["command_mode_test.cc"],
    deps = [
        ":command_mode",
        "//config:gtest",
        "//tr_gpio",
        "//tr_spi",
    ],
)

cc_test(
    name = "artic_r3_test",
    srcs = ["artic_r3_test.cc"],
    target_compatible_with = select(TARGET_COMPATIBLE_WITH_HOST),
    deps = [
        ":artic_r3",
        "//config:gtest",
        "//tr_gpio",
        "//tr_spi",
        "@pigweed//pw_assert",
        "@pigweed//pw_chrono:system_clock",
        "@pigweed//pw_status",
    ],
)
