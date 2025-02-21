load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_python//python:defs.bzl", "py_binary")
load("//:config/microsoft.bzl", "ms_cc_binary", "ms_cc_library")
# To build Docker image include
# load("@io_bazel_rules_docker//container:container.bzl", "container_image")

cc_library(
    name = "Llvm",
    srcs = [],
    hdrs = ["Llvm/Llvm.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        "@llvm-project//llvm:IRReader",
        "@llvm-project//llvm:Target",
        "@llvm-project//llvm:TransformUtils",
        "@llvm-project//llvm:InstCombine",
        "@llvm-project//llvm:Scalar",
        "@llvm-project//llvm:Passes",

        # VM deps
        "@llvm-project//llvm:ExecutionEngine",
        "@llvm-project//llvm:OrcJIT",
        "@llvm-project//llvm:MCJIT",
        "@llvm-project//llvm:Interpreter",
    ] + select({
        "@platforms//cpu:aarch64": [
            "@llvm-project//llvm:AArch64CodeGen",
            "@llvm-project//llvm:AArch64AsmParser",
        ],
        "@platforms//cpu:arm": [
            "@llvm-project//llvm:ARMCodeGen",
            "@llvm-project//llvm:ARMAsmParser",
        ],
        "@platforms//cpu:x86_64": [
            "@llvm-project//llvm:X86CodeGen",
            "@llvm-project//llvm:X86AsmParser",
        ],
        "//conditions:default": [
        ],
    }),
)

genrule(
    name = "WorkspaceVariables",
    outs = ["workspace.txt"],
    cmd = "cp bazel-out/stable-status.txt $@",
    stamp = 1,
)

py_binary(
    name = "VersionBuilder",
    srcs = ["Version/version_builder.py"],
    main = "version_builder.py",
)

genrule(
    name = "VersionHeaderFile",
    srcs = ["Version/Version.tpl.hpp"],
    outs = ["Version/Version.hpp"],
    cmd = "$(location //qir/qat:VersionBuilder) $(location //qir/qat:WorkspaceVariables) $(location Version/Version.tpl.hpp) $@",
    tools = [
        "//qir/qat:VersionBuilder",
        "//qir/qat:WorkspaceVariables",
    ],
)

ms_cc_library(
    name = "Version",
    srcs = ["Version/Version.cpp"],
    hdrs = ["Version/Version.hpp"],
    data = [
        "//qir/qat:VersionHeaderFile",
    ],
)

ms_cc_binary(
    name = "VersionTool",
    srcs = ["Apps/VersionTool/VersionTool.cpp"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//qir/qat:Version",
    ],
)

ms_cc_library(
    name = "QatTypes",
    srcs = glob(["QatTypes/*.cpp"]),
    hdrs = glob(["QatTypes/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Llvm",
    ],
)

ms_cc_library(
    name = "Utils",
    srcs = glob(["Utils/*.cpp"]),
    hdrs = glob([
        "Utils/*.hpp",
        "Utils/**/*.hpp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "Logging",
    srcs = glob(["Logging/*.cpp"]),
    hdrs = glob([
        "Logging/*.hpp",
        "Logging/**/*.hpp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Llvm",
        "//qir/qat:QatTypes",
        "//qir/qat:Utils",
    ],
)

ms_cc_library(
    name = "Commandline",
    srcs = glob(["Commandline/*.cpp"]),
    hdrs = glob(["Commandline/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/external:yaml",
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "Profile",
    srcs = glob(["Profile/*.cpp"]),
    hdrs = glob(["Profile/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:AllocationManager",
        "//qir/qat:Llvm",
        "//qir/qat:QatTypes",
        "//qir/qat:Validator",
    ],
)

ms_cc_library(
    name = "DivisionByZero",
    srcs = glob(["DivisionByZeroPass/*.cpp"]),
    hdrs = glob(["DivisionByZeroPass/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
        "//qir/qat:ValidationPass",
    ],
)

ms_cc_library(
    name = "Validator",
    srcs = glob(["Validator/*.cpp"]),
    hdrs = glob(["Validator/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:AllocationManager",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
        "//qir/qat:StaticResourceComponent",
        "//qir/qat:ValidationPass",
    ],
)

ms_cc_library(
    name = "ValidationPass",
    srcs = glob(["ValidationPass/*.cpp"]),
    hdrs = glob(["ValidationPass/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Commandline",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
        "//qir/qat:StaticResourceComponent",
    ],
)

ms_cc_library(
    name = "PreTransformTrimming",
    srcs = glob(["PreTransformTrimming/*.cpp"]),
    hdrs = glob(["PreTransformTrimming/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Commandline",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "PostTransformValidation",
    srcs = glob(["PostTransformValidation/*.cpp"]),
    hdrs = glob(["PostTransformValidation/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Commandline",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "StaticResourceComponent",
    srcs = glob(["StaticResourceComponent/*.cpp"]),
    hdrs = glob(["StaticResourceComponent/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Commandline",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "GroupingPass",
    srcs = glob(["GroupingPass/*.cpp"]),
    hdrs = glob(["GroupingPass/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Commandline",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "FunctionReplacementPass",
    srcs = glob(["FunctionReplacementPass/*.cpp"]),
    hdrs = glob(["FunctionReplacementPass/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Commandline",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "RemoveDisallowedAttributesPass",
    srcs = glob(["RemoveDisallowedAttributesPass/*.cpp"]),
    hdrs = glob(["RemoveDisallowedAttributesPass/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "ZExtTransformPass",
    srcs = glob(["ZExtTransformPass/*.cpp"]),
    hdrs = glob(["ZExtTransformPass/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "ModuleLoader",
    srcs = glob(["ModuleLoader/*.cpp"]),
    hdrs = glob(["ModuleLoader/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
        "//qir/qat:RemoveDisallowedAttributesPass",
    ],
)

ms_cc_library(
    name = "AllocationManager",
    srcs = glob(["AllocationManager/*.cpp"]),
    hdrs = glob(["AllocationManager/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Llvm",
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "Rules",
    srcs = glob([
        "Rules/*.cpp",
        "Rules/Patterns/*.cpp",
        "Rules/Notation/*.cpp",
    ]),
    hdrs = glob([
        "Rules/*.hpp",
        "Rules/**/*.hpp",
        "Rules/**/*.ipp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:AllocationManager",
        "//qir/qat:Commandline",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "TransformationRulesPass",
    srcs = glob(["TransformationRulesPass/*.cpp"]),
    hdrs = glob(["TransformationRulesPass/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Commandline",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:Profile",
        "//qir/qat:QatTypes",
        "//qir/qat:Rules",
    ],
)

cc_library(
    name = "DeferMeasurementPass",
    srcs = glob(["DeferMeasurementPass/*.cpp"]),
    hdrs = glob(["DeferMeasurementPass/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Commandline",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:QatTypes",
    ],
)

ms_cc_library(
    name = "Generators",
    srcs = glob(["Generators/*.cpp"]),
    hdrs = glob(["Generators/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:AllocationManager",
        "//qir/qat:Commandline",
        "//qir/qat:DeferMeasurementPass",
        "//qir/qat:FunctionReplacementPass",
        "//qir/qat:GroupingPass",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:PostTransformValidation",
        "//qir/qat:PreTransformTrimming",
        "//qir/qat:Profile",
        "//qir/qat:QatTypes",
        "//qir/qat:Rules",
        "//qir/qat:StaticResourceComponent",
        "//qir/qat:TransformationRulesPass",
        "//qir/qat:ValidationPass",
        "//qir/qat:Validator",
        "//qir/qat:ZExtTransformPass",
    ],
)

ms_cc_library(
    name = "QatConfig",
    srcs = [
        "Apps/Qat/ProfileConfiguration.cpp",
        "Apps/Qat/QatConfig.cpp",
    ],
    hdrs = [
        "Apps/Qat/ProfileConfiguration.hpp",
        "Apps/Qat/QatConfig.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:Commandline",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:ValidationPass",
    ],
)

ms_cc_library(
    name = "TestTools",
    srcs = glob(["TestTools/*.cpp"]),
    hdrs = glob(["TestTools/*.hpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//qir/qat:AllocationManager",
        "//qir/qat:Commandline",
        "//qir/qat:FunctionReplacementPass",
        "//qir/qat:Generators",
        "//qir/qat:GroupingPass",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:ModuleLoader",
        "//qir/qat:PostTransformValidation",
        "//qir/qat:PreTransformTrimming",
        "//qir/qat:Profile",
        "//qir/qat:QatConfig",
        "//qir/qat:QatTypes",
        "//qir/qat:Rules",
        "//qir/qat:StaticResourceComponent",
        "//qir/qat:TransformationRulesPass",
        "//qir/qat:ValidationPass",
        "//qir/qat:Validator",
    ],
)

ms_cc_binary(
    name = "qat",
    srcs = [
        "Apps/Qat/ProfileConfiguration.hpp",
        "Apps/Qat/Qat.cpp",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//qir/qat:AllocationManager",
        "//qir/qat:Commandline",
        "//qir/qat:DeferMeasurementPass",
        "//qir/qat:FunctionReplacementPass",
        "//qir/qat:Generators",
        "//qir/qat:GroupingPass",
        "//qir/qat:Llvm",
        "//qir/qat:Logging",
        "//qir/qat:ModuleLoader",
        "//qir/qat:PostTransformValidation",
        "//qir/qat:PreTransformTrimming",
        "//qir/qat:Profile",
        "//qir/qat:QatConfig",
        "//qir/qat:QatTypes",
        "//qir/qat:Rules",
        "//qir/qat:StaticResourceComponent",
        "//qir/qat:TransformationRulesPass",
        "//qir/qat:ValidationPass",
        "//qir/qat:Validator",
        "//qir/qat:Version",
    ],
)

pkg_tar(
    name = "qat-tar",
    srcs = [":qat"],
    mode = "0755",
    package_dir = "/opt",
    visibility = ["//visibility:public"],
)

# To enable building Docker images, add this
# container_image(
#     name = "qat-image",
#     base = "@ubuntu-linux//image",
#     cmd = ["/opt/qat"],
#     target_compatible_with = [
#         "@platforms//cpu:x86_64",
#         "@platforms//os:linux",
#     ],
#     tars = [":qat-tar"],
# )
