workspace(
    name = "tracka",
)

load(
    "@bazel_tools//tools/build_defs/repo:git.bzl",
    "git_repository",
)
load(
    "@bazel_tools//tools/build_defs/repo:http.bzl",
    "http_archive",
)

# Set up Starlark library
# Required by: io_bazel_rules_go, com_google_protobuf.
# Used in modules: None.
# This must be instantiated before com_google_protobuf as protobuf_deps() pulls
# in an older version of bazel_skylib. However io_bazel_rules_go requires a
# newer version.
http_archive(
    name = "bazel_skylib",
    sha256 = "1c531376ac7e5a180e0237938a2536de0c54d93f5c278634818e0efc952dd56c",
    urls = [
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

# Set up protobuf rules
# Required by: pigweed, com_github_bazelbuild_buildtools.
# Used in modules: //pw_protobuf.
http_archive(
    name = "com_google_protobuf",
    sha256 = "71030a04aedf9f612d2991c1c552317038c3c5a2b578ac4745267a45e7037c29",
    strip_prefix = "protobuf-3.12.3",
    url = "https://github.com/protocolbuffers/protobuf/archive/v3.12.3.tar.gz",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

# Setup embedded C/C++ toolchains.
# Required by: tracka, pigweed
# Used in modules: //:All.
git_repository(
    name = "bazel_embedded",
    commit = "89c05fa415218abd2e24fa7016cb7903317d606b",
    remote = "https://github.com/silvergasp/bazel-embedded.git",
)

# Fetch pigweed.
# Required by: tracka.
# Used by modules: None.
git_repository(
    name = "pigweed",
    commit = "077dc10b3811612c679073c796ed8a404e3e1520",
    remote = "https://pigweed.googlesource.com/pigweed/pigweed",
)

# Instantiate Pigweed configuration for embedded toolchain,
# this must be called before bazel_embedded_deps.
load(
    "@pigweed//pw_build:pigweed_toolchain_upstream.bzl",
    "toolchain_upstream_deps",
)

toolchain_upstream_deps()

# Configure bazel_embedded toolchains and platforms.
load(
    "@bazel_embedded//:bazel_embedded_deps.bzl",
    "bazel_embedded_deps",
)

bazel_embedded_deps()

load(
    "@bazel_embedded//platforms:execution_platforms.bzl",
    "register_platforms",
)

register_platforms()

# Fetch gcc-arm-none-eabi compiler and register for toolchain
# resolution.
load(
    "@bazel_embedded//toolchains/compilers/gcc_arm_none_eabi:gcc_arm_none_repository.bzl",
    "gcc_arm_none_compiler",
)

gcc_arm_none_compiler()

load(
    "@bazel_embedded//toolchains/gcc_arm_none_eabi:gcc_arm_none_toolchain.bzl",
    "register_gcc_arm_none_toolchain",
)

register_gcc_arm_none_toolchain()

# Fetch LLVM/Clang compiler and register for toolchain resolution.
load(
    "@bazel_embedded//toolchains/compilers/llvm:llvm_repository.bzl",
    "llvm_repository",
)

llvm_repository(
    name = "com_llvm_compiler",
)

load(
    "@bazel_embedded//toolchains/clang:clang_toolchain.bzl",
    "register_clang_toolchain",
)

register_clang_toolchain()

# Setup Golang toolchain rules
# Required by: bazel_gazelle, com_github_bazelbuild_buildtools.
# Used in modules: None.
http_archive(
    name = "io_bazel_rules_go",
    sha256 = "d1ffd055969c8f8d431e2d439813e42326961d0942bdf734d2c95dc30c369566",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_go/releases/download/v0.24.5/rules_go-v0.24.5.tar.gz",
        "https://github.com/bazelbuild/rules_go/releases/download/v0.24.5/rules_go-v0.24.5.tar.gz",
    ],
)

load(
    "@io_bazel_rules_go//go:deps.bzl",
    "go_register_toolchains",
    "go_rules_dependencies",
)

go_rules_dependencies()

go_register_toolchains()

# Setup Bazel package manager for golang
# Required by: com_github_bazelbuild_buildtools.
# Used in modules: None
http_archive(
    name = "bazel_gazelle",
    sha256 = "b85f48fa105c4403326e9525ad2b2cc437babaa6e15a3fc0b1dbab0ab064bc7c",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-gazelle/releases/download/v0.22.2/bazel-gazelle-v0.22.2.tar.gz",
        "https://github.com/bazelbuild/bazel-gazelle/releases/download/v0.22.2/bazel-gazelle-v0.22.2.tar.gz",
    ],
)

load("@bazel_gazelle//:deps.bzl", "gazelle_dependencies")

gazelle_dependencies()

# Setup Bazel buildtools (Bazel linter and formatter).
# Required by: tracka.
# Used in modules: //:all (Bazel specific tools).
http_archive(
    name = "com_github_bazelbuild_buildtools",
    sha256 = "c28eef4d30ba1a195c6837acf6c75a4034981f5b4002dda3c5aa6e48ce023cf1",
    strip_prefix = "buildtools-4.0.1",
    url = "https://github.com/bazelbuild/buildtools/archive/4.0.1.tar.gz",
)

# Setup Bazel's licensing API.
# Required by: tracka.
# Used in modules: //:all (Bazel specific tools).
git_repository(
    name = "rules_license",
    commit = "a40563d6ea1ea5d6745762bc04eb3f07d14e3fb2",
    remote = "https://github.com/bazelbuild/rules_license.git",
    shallow_since = "1589311098 -0400",
)
