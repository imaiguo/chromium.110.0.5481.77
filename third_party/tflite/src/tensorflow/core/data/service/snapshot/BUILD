# Distributed snapshot library.

load("//tensorflow:tensorflow.bzl", "tf_cc_test")

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    default_visibility = [
        "//tensorflow:internal",
    ],
    licenses = ["notice"],
)

cc_library(
    name = "snapshot_stream_writer",
    srcs = ["snapshot_stream_writer.cc"],
    hdrs = ["snapshot_stream_writer.h"],
    deps = [
        "//tensorflow/core:framework",
        "//tensorflow/core:protos_all_cc",
        "//tensorflow/core/data:snapshot_utils",
        "//tensorflow/core/data/service:common_proto_cc",
        "//tensorflow/core/data/service:task_runner",
        "//tensorflow/core/data/service:worker_proto_cc",
        "//tensorflow/core/lib/gtl:cleanup",
        "//tensorflow/tsl/platform:env",
        "//tensorflow/tsl/platform:errors",
        "//tensorflow/tsl/platform:mutex",
        "//tensorflow/tsl/platform:status",
        "//tensorflow/tsl/platform:thread_annotations",
    ],
)

tf_cc_test(
    name = "snapshot_stream_writer_test",
    size = "small",
    srcs = ["snapshot_stream_writer_test.cc"],
    deps = [
        ":snapshot_stream_writer",
        "//tensorflow/core:framework",
        "//tensorflow/core:test",
        "//tensorflow/core:test_main",
        "//tensorflow/core/data:snapshot_utils",
        "//tensorflow/core/data/service:task_runner",
        "//tensorflow/tsl/platform:env",
        "//tensorflow/tsl/platform:errors",
        "//tensorflow/tsl/platform:status",
        "//tensorflow/tsl/platform:status_matchers",
        "//tensorflow/tsl/protobuf:protos_all_cc",
    ],
)
