# TODO(zacmustin): describe this package.
# TODO(zacmustin): branch this file properly from tf/python/lib/core/BUILD. This is just to avoid
# lots of annoying merge conflicts.

package(
    default_visibility = [
        "//visibility:public",
    ],
    licenses = ["notice"],
)

filegroup(
    name = "numpy_hdr",
    srcs = ["numpy.h"],
)

filegroup(
    name = "basic_hdrs",
    srcs = [
        "numpy.h",
    ],
)

cc_library(
    name = "bfloat16_hdr",
    hdrs = [
        "bfloat16.h",
    ],
    deps = [
        "//third_party/python_runtime:headers",  # build_cleaner: keep; DNR: b/35864863
    ],
)

cc_library(
    name = "bfloat16_lib",
    srcs = [
        "bfloat16.cc",
        "float8_e4m3b11.cc",
    ],
    hdrs = [
        "bfloat16.h",
        "float8_e4m3b11.h",
    ],
    deps = [
        ":custom_float",
        "//tensorflow/tsl/platform:logging",
        "//tensorflow/tsl/platform:types",
        "//tensorflow/tsl/python/lib/core:numpy",
        "//third_party/eigen3",
        "//third_party/python_runtime:headers",  # build_cleaner: keep; DNR: b/35864863
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "custom_float",
    hdrs = [
        "custom_float.h",
    ],
    deps = [
        "//tensorflow/tsl/platform:logging",
        "//tensorflow/tsl/platform:types",
        "//tensorflow/tsl/python/lib/core:numpy",
        "//third_party/eigen3",
        "//third_party/python_runtime:headers",  # build_cleaner: keep; DNR: b/35864863
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "numpy",
    srcs = ["numpy.cc"],
    hdrs = ["numpy.h"],
    deps = [
        "//third_party/py/numpy:headers",
        "//third_party/python_runtime:headers",
    ],
)
