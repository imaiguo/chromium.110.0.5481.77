package(
    default_applicable_licenses = ["//tensorflow_lite_support:license"],
    default_visibility = [
        "//tensorflow_lite_support:internal",
    ],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "zip_writable_mem_file",
    srcs = ["zip_writable_mem_file.cc"],
    hdrs = ["zip_writable_mem_file.h"],
    deps = [
        "@com_google_absl//absl/strings",
        "@zlib//:zlib_minizip",
    ],
)

cc_library(
    name = "zip_readonly_mem_file",
    srcs = ["zip_readonly_mem_file.cc"],
    hdrs = ["zip_readonly_mem_file.h"],
    deps = [
        "@com_google_absl//absl/strings",
        "@zlib//:zlib_minizip",
    ],
)
