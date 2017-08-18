#ifndef __OPTIONS_STUB_H__
#define __OPTIONS_STUB_H__

#include "config/settingsNode.h"
#include "core/options.h"
#include "core/patternsHandler.h"
#include "log/assert.h"

#include "executorStub.h"

namespace execHelper {
    namespace test {
        class OptionsStub : public core::Options {
            public:
                OptionsStub() :
                    //m_settings("OptionsStub"),
                    m_patternsHandler(new core::PatternsHandler())
                {
                    ;
                }

                bool parse(int /*argc*/, const char* const * /*argv*/) override {
                    return false;  //  This function is currently not supported
                }

                bool parseSettingsFile(const config::Path& /*file*/) noexcept override {
                    return false;  //  This function is currently not supported
                }

                bool getVerbosity() const noexcept override {
                    return m_verbosity;
                }

                bool getDryRun() const noexcept override {
                    return m_dryRun;
                }

                bool getSingleThreaded() const noexcept override {
                    return m_singleThreaded;
                }

                const core::CommandCollection& getCommands() const noexcept override {
                    return m_commands;
                }

                const config::SettingsNode& getSettings() const noexcept override {
                    return m_settings;
                }

                const config::SettingsNode& getSettings(const std::string& key) noexcept override {
                    if(m_settings.contains(key)) {
                        return m_settings[key];
                    }
                    return m_settings;
                }

                const config::SettingsNode& getSettings(const std::string& key) const noexcept override {
                    if(m_settings.contains(key)) {
                        return m_settings[key];
                    }
                    return m_settings;
                }

                bool containsHelp() const noexcept override {
                    return m_containsHelp;
                }

                // Note: executor needs to be allocated on the heap here
                void setExecutor(core::ExecutorInterface* const /*executor*/) noexcept override {
                    ensures(false);
                }

                core::ExecutorInterface* getExecutor() const noexcept override {
                    return &m_executor;
                }
  
                bool contains(const std::string& /*longOptions*/) const noexcept override {
                    return false;
                }

                std::vector<std::string> getLongOption(const std::string& longOption) const noexcept override {
                    auto optionValues = m_options.find(longOption);
                    if(optionValues == m_options.end()) {
                        return std::vector<std::string>();
                    }
                    return optionValues->second;
                }

                const core::PatternsHandler& getPatternsHandler() const noexcept override {
                    return *m_patternsHandler;
                }

                core::PatternValues getValues(const core::Pattern& pattern) const noexcept override {
                    const auto longOptionValues = m_options.find(pattern.getLongOption());
                    if(longOptionValues != m_options.end()) {
                        return longOptionValues->second;
                    }
                    const auto shortOptionValues = m_options.find(std::string(1, pattern.getShortOption()));
                    if(shortOptionValues != m_options.end()) {
                        return shortOptionValues->second;
                    }
                    return pattern.getDefaultValues();
                }

                core::PatternPermutator makePatternPermutator(const core::PatternKeys& patterns) const noexcept override {
                    std::map<core::PatternKey, core::PatternValues> patternValuesMatrix;
                    if(patterns.empty()) {
                        // Invent a map so we permutate at least once
                        patternValuesMatrix["NOKEY"] = {"NOKEY"};
                    } else {
                        for(const auto& patternKey : patterns) {
                            core::Pattern pattern = m_patternsHandler->getPattern(patternKey);
                            core::PatternValues commandlineValues = getValues(pattern);
                            if(commandlineValues.empty()) {
                                patternValuesMatrix.emplace(pattern.getKey(), pattern.getDefaultValues());
                            } else {
                                patternValuesMatrix.emplace(pattern.getKey(), commandlineValues);
                            }
                        }
                    }
                    return core::PatternPermutator(patternValuesMatrix);
                }

                bool m_verbosity{false};
                bool m_dryRun{false};
                bool m_singleThreaded{false};
                core::CommandCollection m_commands;
                std::map<std::string, std::vector<std::string>> m_options;
                config::SettingsNode m_settings{"OptionsStub"};
                bool m_containsHelp{false};
                mutable core::test::ExecutorStub m_executor;
                std::shared_ptr<core::PatternsHandler> m_patternsHandler;
        };
    }
}

#endif  /* __OPTIONS_STUB_H__ */
