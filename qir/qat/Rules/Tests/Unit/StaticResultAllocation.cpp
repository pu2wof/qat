// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "gtest/gtest.h"
#include "qir/qat/Generators/ConfigurableProfileGenerator.hpp"
#include "qir/qat/Generators/PostTransformConfig.hpp"
#include "qir/qat/GroupingPass/GroupingPass.hpp"
#include "qir/qat/Llvm/Llvm.hpp"
#include "qir/qat/Rules/Factory.hpp"
#include "qir/qat/TestTools/IrManipulationTestHelper.hpp"

#include <functional>

using namespace microsoft::quantum;

namespace
{
using IrManipulationTestHelperPtr = std::shared_ptr<IrManipulationTestHelper>;
IrManipulationTestHelperPtr newIrManip(std::string const& script)
{
    IrManipulationTestHelperPtr ir_manip = std::make_shared<IrManipulationTestHelper>();

    ir_manip->declareOpaque("Qubit");
    ir_manip->declareOpaque("Result");

    ir_manip->declareFunction("%Qubit* @__non_standard_allocator()");
    ir_manip->declareFunction("i8* @__non_standard_int_allocator()");
    ir_manip->declareFunction("%Result* @__quantum__qis__m__body(%Qubit*)");

    if (!ir_manip->fromBodyString(script))
    {
        llvm::outs() << ir_manip->getErrorMessage() << "\n";
        exit(-1);
    }
    return ir_manip;
}

} // namespace

// Single allocation with action and then release
TEST(RuleSetTestSuite, ResultTranslatedTo)
{
    auto ir_manip = newIrManip(R"script(
  %result1 = call %Result* @__quantum__qis__m__body(%Qubit* null)
  %result2 = call %Result* @__quantum__qis__m__body(%Qubit* null)
  %result3 = call %Result* @__quantum__qis__m__body(%Qubit* null)
  %result4 = call %Result* @__quantum__qis__m__body(%Qubit* null)
  %result5 = call %Result* @__quantum__qis__m__body(%Qubit* null)    
  )script");

    auto configure_profile = [](RuleSet& rule_set)
    {
        auto factory =
            RuleFactory(rule_set, BasicAllocationManager::createNew(), BasicAllocationManager::createNew(), nullptr);
        factory.useStaticResultAllocation();
    };

    auto profile = std::make_shared<ConfigurableProfileGenerator>(
        std::move(configure_profile), TransformationRulesPassConfiguration::createDisabled(),
        LlvmPassesConfiguration::createDisabled());

    ConfigurationManager& configuration_manager = profile->configurationManager();
    configuration_manager.setConfig(GroupingPassConfiguration::createDisabled());
    configuration_manager.setConfig(PostTransformConfig::createDisabled());

    ir_manip->applyProfile(profile);

    EXPECT_TRUE(ir_manip->hasInstructionSequence(
        {"%result1 = inttoptr i64 0 to %Result*",
         "call void @__quantum__qis__mz__body(%Qubit* null, %Result* %result1)",
         "%result2 = inttoptr i64 1 to %Result*",
         "call void @__quantum__qis__mz__body(%Qubit* null, %Result* %result2)",
         "%result3 = inttoptr i64 2 to %Result*",
         "call void @__quantum__qis__mz__body(%Qubit* null, %Result* %result3)",
         "%result4 = inttoptr i64 3 to %Result*",
         "call void @__quantum__qis__mz__body(%Qubit* null, %Result* %result4)",
         "%result5 = inttoptr i64 4 to %Result*",
         "call void @__quantum__qis__mz__body(%Qubit* null, %Result* %result5)"}));

    EXPECT_FALSE(
        ir_manip->hasInstructionSequence({
            "%result1 = call %Result* @__quantum__qis__m__body(%Qubit* null)",
        }) ||
        ir_manip->hasInstructionSequence({
            "%result1 = call %Result* @__quantum__qis__m__body(%Qubit* null)",
        }));

    EXPECT_FALSE(
        ir_manip->hasInstructionSequence({
            "%result2 = call %Result* @__quantum__qis__m__body(%Qubit* null)",
        }) ||
        ir_manip->hasInstructionSequence({
            "%result2 = call %Result* @__quantum__qis__m__body(%Qubit* null)",
        }));

    EXPECT_FALSE(
        ir_manip->hasInstructionSequence({
            "%result3 = call %Result* @__quantum__qis__m__body(%Qubit* null)",
        }) ||
        ir_manip->hasInstructionSequence({
            "%result3 = call %Result* @__quantum__qis__m__body(%Qubit* null)",
        }));

    EXPECT_FALSE(
        ir_manip->hasInstructionSequence({
            "%result4 = call %Result* @__quantum__qis__m__body(%Qubit* null)",
        }) ||
        ir_manip->hasInstructionSequence({
            "%result4 = call %Result* @__quantum__qis__m__body(%Qubit* null)",
        }));

    EXPECT_FALSE(
        ir_manip->hasInstructionSequence({
            "%result5 = call %Result* @__quantum__qis__m__body(%Qubit* null)",
        }) ||
        ir_manip->hasInstructionSequence({
            "%result5 = call %Result* @__quantum__qis__m__body(%Qubit* null)",
        }));
}
