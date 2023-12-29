cc_library(
    name = "lib",
    hdrs = [
        "lib.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":utils",
        "@gtest//:gtest_main",
    ],
)

cc_library(
    name = "utils",
    hdrs = [
        "utils.hpp",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "test",
    srcs = ["test.cpp"],
    deps = [
        ":lib",
        "@gtest//:gtest_main",
    ],
)
