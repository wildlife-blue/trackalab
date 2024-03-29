workspace(
    name = "trackalab",
)

load(
    "@bazel_tools//tools/build_defs/repo:git.bzl",
    "git_repository",
)
load(
    "@bazel_tools//tools/build_defs/repo:http.bzl",
    "http_archive",
)

git_repository(
    name = "platforms",
    commit = "682394a1a13d5c655f7adc14956ba2ff7f569860",
    remote = "https://github.com/silvergasp/platforms.git",
)

http_archive(
    name = "io_bazel_stardoc",
    sha256 = "c9794dcc8026a30ff67cf7cf91ebe245ca294b20b071845d12c192afe243ad72",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/stardoc/releases/download/0.5.0/stardoc-0.5.0.tar.gz",
        "https://github.com/bazelbuild/stardoc/releases/download/0.5.0/stardoc-0.5.0.tar.gz",
    ],
)

load("@io_bazel_stardoc//:setup.bzl", "stardoc_repositories")

stardoc_repositories()

# Setup host toolchain.
# Required by: All.
# Used in modules: All CC modules.
git_repository(
    name = "rules_cc_toolchain",
    commit = "a4ad06c805fc0b38b29f7572de89b95e943e443d",
    remote = "https://github.com/silvergasp/rules_cc_toolchain.git",
)

load("@rules_cc_toolchain//:rules_cc_toolchain_deps.bzl", "rules_cc_toolchain_deps")

rules_cc_toolchain_deps()

load("@rules_cc_toolchain//cc_toolchain:cc_toolchain.bzl", "register_cc_toolchains")

register_cc_toolchains()

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
    commit = "fe1f0f0fcb3ee43063dea96e4216157ee13b5334",
    remote = "https://github.com/silvergasp/bazel-embedded.git",
)

# Fetch pigweed.
# Required by: tracka.
# Used by modules: None.
git_repository(
    name = "pigweed",
    commit = "a6039dd7bff372b32c61db853862a8b9c4f16846",
    remote = "https://pigweed.googlesource.com/pigweed/pigweed",
)

load("@pigweed//pw_build:target_config.bzl", "pigweed_config")

# Configure Pigweeds backend.
pigweed_config(
    name = "pigweed_config",
    build_file = "@pigweed//targets:default_config.BUILD",
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

# Setup conda python distribution
# Required by: ecg.
# Used in modules: None.
http_archive(
    name = "rules_conda",
    sha256 = "8298379474beb05f815afc33a42eb1732f8ebdab3aa639569473eae75e6e072b",
    url = "https://github.com/spietras/rules_conda/releases/download/0.0.5/rules_conda-0.0.5.zip",
)

load("@rules_conda//:defs.bzl", "conda_create", "load_conda", "register_toolchain")

# Download and install conda.
load_conda(
    version = "4.8.4",  # optional, defaults to 4.8.4
)

# Create environment with python3.
conda_create(
    name = "py3_env",
    timeout = 600,
    clean = True,
    environment = "@//third_party/conda:py3_environment.yml",
    quiet = True,
)

# Register pythons from environment as toolchain.
register_toolchain(
    py3_env = "py3_env",
)

# Setup python rules.
# Required by: tracka.
# Used in modules: None.
http_archive(
    name = "rules_python",
    sha256 = "778197e26c5fbeb07ac2a2c5ae405b30f6cb7ad1f5510ea6fdac03bded96cc6f",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_python/releases/download/0.2.0/rules_python-0.2.0.tar.gz",
        "https://github.com/bazelbuild/rules_python/releases/download/0.2.0/rules_python-0.2.0.tar.gz",
    ],
)

# Setup absl.
# Required by: gtest.
# Used in modules: None.
http_archive(
    name = "com_google_absl",
    strip_prefix = "abseil-cpp-98eb410c93ad059f9bba1bf43f5bb916fc92a5ea",
    urls = ["https://github.com/abseil/abseil-cpp/archive/98eb410c93ad059f9bba1bf43f5bb916fc92a5ea.zip"],
)

# Setup gtest rules.
# Required by: tracka.
# Used in modules: config, spi, gpio, artic_r3.
git_repository(
    name = "com_google_googletest",
    commit = "9614d8c1d69dee9de2e0771a0da2b69a34964e33",
    remote = "https://github.com/google/googletest.git",
)
