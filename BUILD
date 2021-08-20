load(
    "@com_github_bazelbuild_buildtools//buildifier:def.bzl",
    "buildifier",
    "buildifier_test",
)
load("@rules_cc_toolchain//tools/clang_tidy:clang_tidy.bzl", "clang_tidy_config")

licenses(["notice"])  # MIT Licence

# Fix all Bazel relevant files.
buildifier(
    name = "buildifier",
)

# Test to ensure all Bazel build files are properly formatted.
buildifier_test(
    name = "buildifier_test",
    srcs = glob(
        [
            "**/*.bazel",
            "**/*.bzl",
            "**/*.oss",
            "**/*.sky",
            "**/BUILD",
        ],
    ) + ["WORKSPACE"],
    diff_command = "diff -u",
    mode = "diff",
    verbose = True,
)

clang_tidy_config(
    name = "clang_tidy_config",
    config = ".clang-tidy",
    visibility = ["//visibility:public"],
)
