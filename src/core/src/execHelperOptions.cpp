#include "execHelperOptions.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>

#include "config/settingsNode.h"
#include "log/assertions.h"
#include "yaml/yaml.h"

#include "executorInterface.h"
#include "logger.h"
#include "pattern.h"
#include "patternsHandler.h"

using std::string;
using std::stringstream;
using std::setw;
using std::vector;

using boost::filesystem::is_regular_file;
using boost::filesystem::is_symlink;
using boost::optional;
using boost::program_options::variables_map;

using execHelper::core::CommandCollection;

using execHelper::config::Path;
using execHelper::config::SettingsNode;

using execHelper::yaml::Yaml;

namespace execHelper { namespace core {
    ExecHelperOptions::ExecHelperOptions() noexcept :
        m_settings("ExecHelperRoot")
    {
        ;
    }

    void ExecHelperOptions::swap(ExecHelperOptions& other) noexcept {
        m_settings.swap(other.m_settings);
    }

    bool ExecHelperOptions::operator==(const ExecHelperOptions& other) const noexcept {
        return ( 
                    m_verbose == other.m_verbose &&
                    m_dryRun == other.m_dryRun &&
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

    bool ExecHelperOptions::getDryRun() const noexcept {
        return m_dryRun;
    }

    bool ExecHelperOptions::getSingleThreaded() const noexcept {
        return m_singleThreaded;
    }

    optional<log::LogLevel> ExecHelperOptions::getLogLevel() const noexcept {
        if(m_optionsMap.count("debug") == 0) {
            return boost::none;
        }
        string debugLevel;
        try {
            debugLevel = m_optionsMap["debug"].as<string>();
        } catch(const boost::bad_any_cast&) {
            return boost::none;
        }
        if(debugLevel == "none") {
            return boost::none;
        }
        try {
           return log::toLogLevel(debugLevel);
        } catch(const log::InvalidLogLevel& e) {
            user_feedback_error("Invalid log level: '" + debugLevel + "'. Ignoring option...");
        }
        return boost::none;
    }

    const CommandCollection& ExecHelperOptions::getCommands() const noexcept {
        return m_commands;
    }

    string ExecHelperOptions::getSettingsFile(int argc, const char* const * argv) const noexcept {
        variables_map optionsMap = m_optionsDescriptions.getOptionsMap(argc, argv, true);
        if(optionsMap.count("settings-file") > 0) {
            try {
                return optionsMap["settings-file"].as<string>();
            } catch(const boost::bad_any_cast&) {
                return ".exec-helper";
            }
        }
        return ".exec-helper";
    }

    bool ExecHelperOptions::parse(int argc, const char* const * argv) {
        m_optionsMap = m_optionsDescriptions.getOptionsMap(argc, argv);
        if(m_optionsMap.count("verbose") > 0) {
            m_verbose = true;
        }

        if(m_optionsMap.count("dry-run") > 0) {
            m_dryRun = true;
        }

        if(m_optionsMap.count("single-threaded") > 0) {
            m_singleThreaded = true;
        }

        if(m_optionsMap.count("command") > 0) {
            m_commands.clear();
            try {
                m_commands = m_optionsMap["command"].as<CommandCollection>();
            } catch(const boost::bad_any_cast&) {
                return false;
            }
        }

        return true;
    }

    bool ExecHelperOptions::parseSettingsFile(const Path& file) noexcept {
        if(!is_regular_file(file) && !is_symlink(file)) {
            user_feedback_error("File '" << file.native() << "' does not exist or is not a file");
            return false;
        }

        Yaml yaml(file);
        if(! yaml.getTree({}, &m_settings)) {
            LOG(error) << "Could not get settings tree";
            return false;
        }

        static const string patternsKey("patterns");
        if(m_settings.contains(patternsKey)) {
            for(const auto& pattern : m_settings[patternsKey].values()) {
                const SettingsNode& patternSettings = m_settings[patternsKey][pattern];
                PatternValues defaultValues;
                for(const auto& defaultValue : patternSettings["default-values"].values()) {
                    defaultValues.push_back(defaultValue);
                }
                static const string shortOptionKey("short-option");
                string shortOptionString;
                if(patternSettings.contains(shortOptionKey)) {
                    SettingsNode::SettingsKeys shortOptionStrings = patternSettings[shortOptionKey].values();
                    if(shortOptionStrings.empty()) {
                        user_feedback_error("The value for the short option of " << pattern << " is empty");
                        return false;
                    }
                    shortOptionString = shortOptionStrings.back();
                }
                char shortOption = '\0';
                if(! shortOptionString.empty()) {
                    shortOption = shortOptionString.at(0);
                }

                static const string longOptionKey("long-option");
                string longOption;
                if(patternSettings.contains(longOptionKey)) {
                    SettingsNode::SettingsKeys longOptionStrings = patternSettings[longOptionKey].values();
                    if(longOptionStrings.empty()) {
                        user_feedback_error("The value for the long option of " << pattern << " is empty");
                        return false;
                    }
                    longOption = longOptionStrings.back();
                }

                m_patternsHandler.addPattern(Pattern(pattern, defaultValues, shortOption, longOption));
                string option(longOption);
                option.append(",").append(shortOptionString);
                string explanation = string("Values for pattern '") + pattern + "'";
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


    void ExecHelperOptions::setExecutor(ExecutorInterface* executor) noexcept {
        m_executor = executor;
    }

    ExecutorInterface* ExecHelperOptions::getExecutor() const noexcept {
        expectsMessage(m_executor != nullptr, "executor can not be null");
        return m_executor;
    }

    void ExecHelperOptions::printHelp() const noexcept {
        user_feedback(m_optionsDescriptions.getOptionDescriptions());
        if(m_settings.contains("commands")) {
            user_feedback("Configured commands:");
            for(const auto& command : m_settings["commands"].values()) {
                stringstream commandStream;
                commandStream << "  " << std::left << setw(20) << command;
                SettingsNode::SettingsValues commmandDescription = m_settings["commands"].get({command}, {});
                if(! commmandDescription.empty()) {
                    // Add an extra whitespace in case the key is longer than the minimum width that was set
                    commandStream << " " << commmandDescription.back();
                }
                user_feedback(commandStream.str());
            }
        }
    }

    bool ExecHelperOptions::contains(const string& longOptions) const noexcept {
        return m_optionsMap.count(longOptions) > 0;
    }

    vector<string> ExecHelperOptions::getLongOption(const string& longOptions) const noexcept {
        try {
            return m_optionsMap[longOptions].as<vector<string>>();
        } catch(const boost::bad_any_cast&) {
            return vector<string>();
        }
    }

    const PatternsHandler& ExecHelperOptions::getPatternsHandler() const noexcept {
        return m_patternsHandler;
    }

    PatternValues ExecHelperOptions::getValues(const Pattern& pattern) const noexcept {
        const string& longOption = pattern.getLongOption();
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
} // namespace core
} // namespace execHelper
