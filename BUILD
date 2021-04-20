load(
    "@com_github_bazelbuild_buildtools//buildifier:def.bzl",
    "buildifier",
    "buildifier_test",
)

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
