load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "unit",
    srcs = glob(["*.cpp"]),
    deps = [
        "//qir/external:gmock",
        "//qir/qat:AllocationManager",
        "//qir/qat:Commandline",
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
        "//qir/qat:TestTools",
        "//qir/qat:TransformationRulesPass",
        "//qir/qat:ValidationPass",
        "//qir/qat:Validator",
    ],
)
