// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "qir/qat/Commandline/ConfigurationManager.hpp"

#include "yaml-cpp/yaml.h"

#include <fstream>

using namespace microsoft::quantum;

namespace microsoft::quantum
{

void ConfigurationManager::setupArguments(ParameterParser& parser)
{
    for (auto& section : config_sections_)
    {
        if (section.enabled_by_default)
        {
            parser.addFlag("disable-" + section.id);
        }
        else
        {
            parser.addFlag("enable-" + section.id);
        }
    }

    for (auto& section : config_sections_)
    {
        for (auto& c : section.settings)
        {
            if (!c->setupArguments(parser))
            {
                throw std::runtime_error("Failed to set parser arguments up.");
            }
        }
    }
}

void ConfigurationManager::configure(ParameterParser& parser, bool experimental_mode)
{

    for (auto& section : config_sections_)
    {
        if (section.enabled_by_default || parser.has("disable-" + section.id))
        {
            *section.active = (parser.get("disable-" + section.id, "false") != "true");
        }
        else
        {
            *section.active = (parser.get("enable-" + section.id, "false") == "true");
        }
    }

    for (auto& section : config_sections_)
    {
        for (auto& c : section.settings)
        {
            // Skipping those parameters which are not available to the
            // commandline interface for configuration
            if (!c->isAvailableToCli())
            {
                continue;
            }

            // Configuring the paramter
            if (!c->configure(parser, experimental_mode))
            {
                throw std::runtime_error("Failed configure the section.");
            }
        }
    }
}

void ConfigurationManager::printHelp(bool experimental_mode) const
{
    std::cout << std::setfill(' ');

    // Enable or disable components
    std::cout << std::endl;
    std::cout << "Component configuration"
              << " - ";
    std::cout << "Used to disable or enable components" << std::endl;
    std::cout << std::endl;
    for (auto& section : config_sections_)
    {
        if (!section.id.empty())
        {
            if (section.enabled_by_default)
            {
                std::cout << std::setw(50) << std::left << ("--disable-" + section.id) << "Disables " << section.name
                          << ". ";
                std::cout << "Default: false" << std::endl;
            }
            else
            {
                std::cout << std::setw(50) << std::left << ("--enable-" + section.id) << "Enables " << section.name
                          << ". ";
                std::cout << "Default: false" << std::endl;
            }
        }
    }

    // Component configuration
    for (auto& section : config_sections_)
    {
        std::cout << std::endl;
        std::cout << section.name << " - ";
        std::cout << section.description << std::endl;
        std::cout << std::endl;

        for (auto& c : section.settings)
        {
            // Skipping those parameters which are not available to the
            // commandline interface for configuration
            if (!c->isAvailableToCli())
            {
                continue;
            }

            // Skipping experimental parameters unless if we are in experimental
            // mode.
            if (c->isExperimental() && !experimental_mode)
            {
                continue;
            }

            if (c->isFlag())
            {
                if (c->defaultValue() == "false")
                {
                    std::cout << std::setw(50) << std::left << ("--" + c->name());
                }
                else
                {
                    std::cout << std::setw(50) << std::left << ("--[no-]" + c->name());
                }
            }
            else
            {
                std::cout << std::setw(50) << std::left << ("--" + c->name());
            }

            if (c->isExperimental())
            {
                std::cout << "EXPERIMENTAL. ";
            }

            std::cout << c->description() << " ";

            std::cout << "Default: " << c->defaultValue() << std::endl;
        }
    }
}

void ConfigurationManager::printConfiguration() const
{
    std::cout << std::setfill('.');

    std::cout << "; # "
              << "Components"
              << "\n";
    for (auto& section : config_sections_)
    {
        if (!section.id.empty())
        {
            std::cout << "; " << std::setw(50) << std::left << ("disable-" + section.id) << ": "
                      << (*section.active ? "false" : "true") << "\n";
        }
    }
    std::cout << "; \n";

    for (auto& section : config_sections_)
    {
        std::cout << "; # " << section.name << "\n";
        for (auto& c : section.settings)
        {
            std::cout << "; " << std::setw(50) << std::left << c->name() << ": " << c->value() << "\n";
        }
        std::cout << "; \n";
    }
}

void ConfigurationManager::setSectionName(String const& name, String const& description)
{
    if (config_sections_.empty())
    {
        throw std::runtime_error("No section created yet.");
    }
    config_sections_.back().name        = name;
    config_sections_.back().description = description;
}

void ConfigurationManager::disableSectionByDefault()
{
    if (config_sections_.empty())
    {
        throw std::runtime_error("No section created yet.");
    }

    config_sections_.back().enabled_by_default = false;
}

void ConfigurationManager::disableSectionById(String const& id)
{
    for (auto& section : config_sections_)
    {
        if (section.id == id)
        {
            section.enabled_by_default = false;
            return;
        }
    }

    throw std::runtime_error("Section '" + id + "' not found");
}

void ConfigurationManager::enableSectionById(String const& id)
{
    for (auto& section : config_sections_)
    {
        if (section.id == id)
        {
            section.enabled_by_default = true;
            return;
        }
    }

    throw std::runtime_error("Section '" + id + "' not found");
}

DeferredValue::DeferredValuePtr ConfigurationManager::getParameter(String const& name)
{

    auto it = deferred_refs_.find(name);
    if (it != deferred_refs_.end())
    {
        return it->second;
    }

    auto ret = DeferredValue::create();

    deferred_refs_[name] = ret;

    auto it2 = parameters_.find(name);
    if (it2 != parameters_.end())
    {
        ret->setReference(it2->second);
    }

    return ret;
}

void ConfigurationManager::loadConfig(String const& filename)
{
    YAML::Node config = YAML::LoadFile(filename);

    for (auto& section : config_sections_)
    {
        if (config[section.id])
        {
            auto node = config[section.id];
            for (auto& c : section.settings)
            {
                c->setValueFromYamlNode(node);
            }
        }
    }
}

void ConfigurationManager::saveConfig(String const& filename)
{
    YAML::Node ret;
    for (auto& section : config_sections_)
    {
        if (section.id.empty())
        {
            continue;
        }

        YAML::Node config;

        for (auto& c : section.settings)
        {
            c->updateValueInYamlNode(config);
        }

        ret[section.id] = config;
    }

    std::ofstream fout(filename);
    fout << ret;
}

} // namespace microsoft::quantum
