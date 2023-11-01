load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "gtest",
    branch = "v1.10.x",
    build_file = "@//:gtest.BUILD",
    remote = "https://github.com/google/googletest",
)

http_archive(
    name = "hedron_compile_commands",
    strip_prefix = "bazel-compile-commands-extractor-ac6411f8f347e5525038cb7858db4969db9e74f2",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/ac6411f8f347e5525038cb7858db4969db9e74f2.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()
