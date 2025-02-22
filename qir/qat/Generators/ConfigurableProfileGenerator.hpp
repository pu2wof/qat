#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "qir/qat/Commandline/ConfigurationManager.hpp"
#include "qir/qat/Generators/LlvmPassesConfiguration.hpp"
#include "qir/qat/Generators/ProfileGenerator.hpp"
#include "qir/qat/Llvm/Llvm.hpp"
#include "qir/qat/Rules/FactoryConfig.hpp"
#include "qir/qat/Rules/RuleSet.hpp"
#include "qir/qat/TransformationRulesPass/TransformationRulesPassConfiguration.hpp"

namespace microsoft::quantum
{

/// ConfigurableProfileGenerator defines a profile that configures the rule set used by the Profile
/// pass. This profile is useful for generating dynamic profiles and is well suited for testing
/// purposes or YAML configured transformation of the IR.
class ConfigurableProfileGenerator : public ProfileGenerator
{
  public:
    using ConfigureFunction = std::function<void(RuleSet&)>; ///< Function type that configures a rule set.
    enum class SetupMode
    {
        DoNothing,
        SetupPipeline
    };

    /// Default constructor. This constructor adds components for rule transformation and LLVM passes.
    /// These are configurable through the corresponding configuration classes which can be access
    /// through the configuration manager.
    explicit ConfigurableProfileGenerator(SetupMode const& mode = SetupMode::SetupPipeline);

    /// The constructor takes a lambda function which configures the rule set. This
    /// function is invoked during the creation of the generation module. This constructor
    /// further overrides the default configuration
    explicit ConfigurableProfileGenerator(
        ConfigureFunction const&                    configure,
        TransformationRulesPassConfiguration const& profile_pass_config =
            TransformationRulesPassConfiguration::createDisabled(),
        LlvmPassesConfiguration const& llvm_config = LlvmPassesConfiguration::createDisabled());

    // Shorthand notation to access configurations
    //

    /// Returns a constant reference to the rule transformation configuration.
    TransformationRulesPassConfiguration const& ruleTransformationConfig() const;

    /// Returns a constant reference to the LLVM passes configuration.
    LlvmPassesConfiguration const& llvmPassesConfig() const;
};

} // namespace microsoft::quantum
