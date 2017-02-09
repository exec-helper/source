#ifndef __OPTIONS_STUB_H__
#define __OPTIONS_STUB_H__

#include <assert.h>

#include "core/options.h"
#include "core/patternsHandler.h"
#include "config/settingsNode.h"

#include "executorStub.h"

namespace execHelper {
    namespace test {
        class OptionsStub : public core::Options {
            public:
                OptionsStub() :
                    Options(),
                    m_verbosity(false),
                    m_dryRun(false),
                    m_singleThreaded(false),
                    m_executor(),
                    m_patternsHandler(new core::PatternsHandler())
                {
                    ;
                }

                virtual bool parse(int /*argc*/, const char* const * /*argv*/) override {
                    assert(false);  //  This function is currently not supported
                }

                virtual bool parseSettingsFile(const std::string& /*file*/) noexcept override {
                    assert(false);  //  This function is currently not supported
                }

                virtual bool getVerbosity() const noexcept override {
                    return m_verbosity;
                }

                virtual bool getDryRun() const noexcept override {
                    return m_dryRun;
                }

                virtual bool getSingleThreaded() const noexcept override {
                    return m_singleThreaded;
                }

                virtual const core::CommandCollection& getCommands() const noexcept override {
                    return m_commands;
                }

                virtual const config::SettingsNode& getSettings() const noexcept override {
                    return m_settings;
                }

                virtual const config::SettingsNode& getSettings(const std::string& key) noexcept override {
                    if(m_settings.contains(key)) {
                        return m_settings[key];
                    }
                    return m_settings;
                }

                virtual const config::SettingsNode& getSettings(const std::string& key) const noexcept override {
                    if(m_settings.contains(key)) {
                        return m_settings[key];
                    }
                    return m_settings[key];
                }

                virtual bool containsHelp() const noexcept override {
                    return m_containsHelp;
                }

                // Note: executor needs to be allocated on the heap here
                virtual void setExecutor(core::ExecutorInterface* const /*executor*/) noexcept override {
                    assert(false);
                }

                virtual core::ExecutorInterface* getExecutor() const noexcept override {
                    return &m_executor;
                }
  
                virtual bool contains(const std::string& /*longOptions*/) const noexcept override {
                    return false;
                }

                virtual std::vector<std::string> getLongOption(const std::string& longOption) const noexcept override {
                    auto optionValues = m_options.find(longOption);
                    if(optionValues == m_options.end()) {
                        return std::vector<std::string>();
                    }
                    return optionValues->second;
                }

                virtual const core::PatternsHandler& getPatternsHandler() const noexcept override {
                    return *m_patternsHandler;
                }

                virtual core::PatternValues getValues(const core::Pattern& pattern) const noexcept override {
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

                virtual core::PatternPermutator makePatternPermutator(const core::PatternKeys& patterns) const noexcept override {
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
                    return patternValuesMatrix;
                }

                bool m_verbosity;
                bool m_dryRun;
                bool m_singleThreaded;
                core::CommandCollection m_commands;
                std::map<std::string, std::vector<std::string>> m_options;
                config::SettingsNode m_settings;
                bool m_containsHelp;
                mutable core::test::ExecutorStub m_executor;
                std::shared_ptr<core::PatternsHandler> m_patternsHandler;
        };
    }
}

#endif  /* __OPTIONS_STUB_H__ */
