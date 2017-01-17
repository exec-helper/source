#include "execHelperOptions.h"

#include <iostream>
#include <string>
#include <assert.h>

#include "log/log.h"
#include "yaml/yaml.h"
#include "config/settingsNode.h"
#include "executorInterface.h"
#include "pattern.h"
#include "patternsHandler.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;

using boost::program_options::variables_map;

using execHelper::core::CommandCollection;

using execHelper::config::SettingsNode;

using execHelper::yaml::Yaml;
using execHelper::yaml::YamlFile;

namespace execHelper { namespace core {
    ExecHelperOptions::ExecHelperOptions() noexcept :
        m_verbose(false),
        m_singleThreaded(false),
        m_executor(0)
    {
        ;
    }

    ExecHelperOptions::ExecHelperOptions(const ExecHelperOptions& other) noexcept :
        m_verbose(other.m_verbose),
        m_singleThreaded(other.m_singleThreaded),
        m_settings(other.m_settings),
        m_executor(other.m_executor)
    {
        ;
    }

    bool ExecHelperOptions::operator==(const ExecHelperOptions& other) const noexcept {
        return ( 
                    m_verbose == other.m_verbose &&
                    m_singleThreaded == other.m_singleThreaded &&
                    m_settings == other.m_settings &&
                    m_patternsHandler == other.m_patternsHandler &&
                    m_executor == other.m_executor
               );
    }

    bool ExecHelperOptions::operator!=(const ExecHelperOptions& other) const noexcept {
        return !(*this == other);
    }

    bool ExecHelperOptions::getVerbosity() const noexcept {
        return m_verbose;
    }

    bool ExecHelperOptions::getSingleThreaded() const noexcept {
        return m_singleThreaded;
    }

    const CommandCollection& ExecHelperOptions::getCommands() const noexcept {
        return m_commands;
    }

    string ExecHelperOptions::getSettingsFile(int argc, const char* const * argv) const noexcept {
        variables_map optionsMap = m_optionsDescriptions.getOptionsMap(argc, argv, true);
        if(optionsMap.count("settings-file")) {
            return optionsMap["settings-file"].as<string>();
        }
        return ".exec-helper";
    }

    bool ExecHelperOptions::parse(int argc, const char* const * argv) {
        m_optionsMap = m_optionsDescriptions.getOptionsMap(argc, argv);
        if(m_optionsMap.count("verbose")) {
            m_verbose = true;
        }

        if(m_optionsMap.count("single-threaded")) {
            m_singleThreaded = true;
        }

        if(m_optionsMap.count("command")) {
            m_commands.clear();
            m_commands = m_optionsMap["command"].as<CommandCollection>();
        }

        return true;
    }

    bool ExecHelperOptions::parseSettingsFile(const std::string& file) noexcept {
        YamlFile yamlFile;
        yamlFile.file = file;
        Yaml yaml(yamlFile);
        if(! yaml.getTree({}, m_settings)) {
            LOG("Could not get settings tree");
            return false;
        }

        static const string patternsKey("patterns");
        if(m_settings.contains(patternsKey)) {
            for(const auto& pattern : m_settings[patternsKey].m_values) {
                PatternKey key = pattern.m_key;
                PatternValues defaultValues;
                for(const auto& defaultValue : pattern["default-values"].toStringCollection()) {
                    defaultValues.push_back(defaultValue);
                }
                static const string shortOptionKey("short-option");
                string shortOptionString;
                if(pattern.contains(shortOptionKey)) {
                    shortOptionString = pattern[shortOptionKey].m_values.back().m_key;
                }
                char shortOption = '\0';
                if(shortOptionString.size() > 0) {
                    shortOption = shortOptionString.at(0);
                }

                static const string longOptionKey("long-option");
                string longOption;
                if(pattern.contains(longOptionKey)) {
                    longOption = pattern[longOptionKey].m_values.back().m_key;
                }

                m_patternsHandler.addPattern(Pattern(key, defaultValues, shortOption, longOption));
                string option = longOption + "," + shortOptionString;
                string explanation = string("Values for pattern '") + key + "'";
                m_optionsDescriptions.addOption<PatternValues>(option, explanation, true);
            }
        }
        return true;
    }

    bool ExecHelperOptions::containsHelp() const noexcept {
        return (m_optionsMap.count("help") > 0U);
    }

    const config::SettingsNode& ExecHelperOptions::getSettings() const noexcept {
        return m_settings;
    }

    const config::SettingsNode& ExecHelperOptions::getSettings(const std::string& key) noexcept {
        if(m_settings.contains(key)) {
            return m_settings[key]; 
        }
        return m_settings;
    }

    const config::SettingsNode& ExecHelperOptions::getSettings(const std::string& key) const noexcept {
        if(m_settings.contains(key)) {
            return m_settings[key]; 
        }
        return m_settings;
    }


    void ExecHelperOptions::setExecutor(ExecutorInterface* const executor) noexcept {
        m_executor = executor;
    }

    ExecutorInterface* ExecHelperOptions::getExecutor() const noexcept {
        assert(m_executor != 0);
        return m_executor;
    }

    void ExecHelperOptions::printHelp() const noexcept {
        user_feedback(m_optionsDescriptions.getOptionDescriptions());
    }

    bool ExecHelperOptions::contains(const std::string& longOptions) const noexcept {
        return m_optionsMap.count(longOptions) > 0;
    }

    vector<string> ExecHelperOptions::getLongOption(const std::string& longOptions) const noexcept {
        return m_optionsMap[longOptions].as<vector<string>>();
    }

    const PatternsHandler& ExecHelperOptions::getPatternsHandler() const noexcept {
        return m_patternsHandler;
    }

    PatternValues ExecHelperOptions::getValues(const Pattern& pattern) const noexcept {
        string longOption = pattern.getLongOption();
        if(contains(longOption)) {
            return getLongOption(longOption);
        }
        return pattern.getDefaultValues();
    }

    PatternPermutator ExecHelperOptions::makePatternPermutator(const PatternKeys& patterns) const noexcept {
        std::map<core::PatternKey, core::PatternValues> patternValuesMatrix;
        for(const auto& patternKey : patterns) {
            core::Pattern pattern = m_patternsHandler.getPattern(patternKey);
            PatternValues commandlineValues = getValues(pattern);
            patternValuesMatrix.emplace(pattern.getKey(), commandlineValues);
        }
        return core::PatternPermutator(patternValuesMatrix);
    }
} }
