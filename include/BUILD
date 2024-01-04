cc_library(
    name = "lib",
    hdrs = [
        "lib.hpp",
        "utils.hpp",
        "condition.hpp"
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
