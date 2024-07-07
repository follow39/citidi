load("@bazel_skylib//rules:build_test.bzl", "build_test")
load("@rules_build_error//lang/cc:defs.bzl", "cc_build_error")
load("@rules_build_error//matcher:defs.bzl", "matcher")

cc_build_error(
    name = "IsSomeTypeIsDuplicatedAssertion",
    src = "IsSomeTypeIsDuplicatedAssertion_test.cpp",
    # compile_stderr = matcher.has_substr("static assertion failed"),
    compile_stderr = matcher.has_substr("Marker types should be unique"),
    deps = ["//include:lib"],
)

build_test(
    name = "build_test",
    targets = [
        ":IsSomeTypeIsDuplicatedAssertion",
    ],
)
